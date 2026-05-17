#include "data_structures.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

const vector<string> kStructureNames = {"HashMap Tree", "Pointer Tree"};

bool isValidCategoryId(const string &id)
{
    if (id.empty() || id.size() > 50)
        return false;
    for (char c : id)
    {
        if (isspace((unsigned char)c))
            return false;
    }
    return true;
}

bool isValidCategoryName(const string &name)
{
    return !name.empty() && name.size() <= 100;
}

size_t estimateMemoryUsageKB(size_t nodeCount)
{
    const size_t estimatedPerNodeBytes = 120;
    return (nodeCount * estimatedPerNodeBytes) / 1024;
}

void CategoryMapManager::deleteRecursive(const string &id)
{
    if (categoryMap.count(id) == 0)
        return;

    vector<string> children = categoryMap[id].child_ids;
    for (int i = 0; i < children.size(); i++)
    {
        deleteRecursive(children[i]);
    }

    string parent_id = categoryMap[id].parent_id;
    if (parent_id != "" && categoryMap.count(parent_id) > 0)
    {
        for (int i = 0; i < categoryMap[parent_id].child_ids.size(); i++)
        {
            if (categoryMap[parent_id].child_ids[i] == id)
            {
                categoryMap[parent_id].child_ids.erase(categoryMap[parent_id].child_ids.begin() + i);
                break;
            }
        }
    }

    nameToIdMap.erase(categoryMap[id].name);
    categoryMap.erase(id);
}

size_t CategoryMapManager::countSubtreeNodesRecursive(const string &id)
{
    if (categoryMap.count(id) == 0)
        return 0;

    size_t count = 1;
    for (const string &child_id : categoryMap[id].child_ids)
    {
        count += countSubtreeNodesRecursive(child_id);
    }
    return count;
}

void CategoryMapManager::printTree(const string &root_id, const string &indent)
{
    if (categoryMap.count(root_id) == 0 || categoryMap[root_id].is_active == false)
        return;

    Category cat = categoryMap[root_id];
    cout << indent << "|-- [" << cat.id << "] " << cat.name << "\n";

    for (int i = 0; i < cat.child_ids.size(); i++)
    {
        string child_id = cat.child_ids[i];
        printTree(child_id, indent + "    ");
    }
}

void CategoryMapManager::loadData()
{
    ifstream file(dbFile);
    if (!file.is_open())
    {
        cout << "[INFO] File tidak ditemukan. Memulai database baru.\n";
        return;
    }

    json j;
    file >> j;
    categoryMap.clear();
    nameToIdMap.clear();

    for (auto it = j.begin(); it != j.end(); ++it)
    {
        Category cat;
        cat.id = it.value()["id"];
        cat.name = it.value()["name"];
        cat.parent_id = it.value()["parent_id"];
        cat.level = it.value()["level"];
        cat.is_active = it.value()["is_active"];

        for (int i = 0; i < it.value()["child_ids"].size(); i++)
        {
            cat.child_ids.push_back(it.value()["child_ids"][i]);
        }
        categoryMap[cat.id] = cat;
        nameToIdMap[cat.name] = cat.id;
    }
}

void CategoryMapManager::saveData()
{
    json j;
    for (auto it = categoryMap.begin(); it != categoryMap.end(); ++it)
    {
        Category cat = it->second;
        j[cat.id] = {
            {"id", cat.id},
            {"name", cat.name},
            {"parent_id", cat.parent_id},
            {"level", cat.level},
            {"is_active", cat.is_active},
            {"child_ids", cat.child_ids}};
    }

    ofstream file(dbFile);
    if (file.is_open())
    {
        file << j.dump(4);
        file.close();
    }
}

void CategoryMapManager::insertCategory(const string &id, const string &name, const string &parent_id)
{
    auto start_insert = chrono::high_resolution_clock::now();

    if (!insertCategoryRaw(id, name, parent_id))
        return;

    auto end_insert = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_insert = end_insert - start_insert;

    cout << "  ✓ Sukses! Kategori ditambahkan.\n";
    cout << "  [METRIK] Waktu Insert : " << duration_insert.count() << " ms\n";

    saveData();
}

bool CategoryMapManager::insertCategoryRaw(const string &id, const string &name, const string &parent_id)
{
    if (categoryMap.count(id) > 0)
        return false;

    int current_level = 1;

    if (!parent_id.empty())
    {
        if (categoryMap.count(parent_id) > 0)
        {
            categoryMap[parent_id].child_ids.push_back(id);
            current_level = categoryMap[parent_id].level + 1;
        }
        else
        {
            return false;
        }
    }

    Category newCat = {id, name, parent_id, current_level, true, {}};
    categoryMap[id] = newCat;
    nameToIdMap[name] = id;
    return true;
}

bool CategoryMapManager::deleteCategoryRaw(const string &id)
{
    if (categoryMap.count(id) == 0)
        return false;

    deleteRecursive(id);
    return true;
}

void CategoryMapManager::searchCategory(const string &query)
{
    if (query == "" || query == "ALL" || query == "all")
    {
        auto start_all = chrono::high_resolution_clock::now();

        cout << "\n=== Menampilkan Seluruh Hierarki ===\n";
        for (auto it = categoryMap.begin(); it != categoryMap.end(); ++it)
        {
            Category cat = it->second;
            if (cat.parent_id == "" && cat.is_active == true)
            {
                printTree(cat.id, "");
            }
        }

        auto end_all = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration_all = end_all - start_all;
        cout << "\n[METRIK] Waktu Menampilkan Seluruh Data: " << duration_all.count() << " ms\n";
        return;
    }

    auto start_search = chrono::high_resolution_clock::now();

    string searchId = query;
    if (nameToIdMap.count(query) > 0)
    {
        searchId = nameToIdMap[query];
    }

    bool found = categoryMap.count(searchId) > 0;

    auto end_search = chrono::high_resolution_clock::now();

    if (found)
    {
        Category cat = categoryMap[searchId];
        cout << "\n=== Ditemukan ===\n";
        cout << "ID       : " << cat.id << "\n";
        cout << "Nama     : " << cat.name << "\n";
        cout << "Level    : " << cat.level << "\n";

        cout << "\n--- Struktur Turunan ---\n";

        auto start_traverse = chrono::high_resolution_clock::now();
        printTree(searchId, "");
        auto end_traverse = chrono::high_resolution_clock::now();

        chrono::duration<double, milli> duration_search = end_search - start_search;
        chrono::duration<double, milli> duration_traverse = end_traverse - start_traverse;

        cout << "\n[METRIK] Waktu Search O(1) Lookup         : " << duration_search.count() << " ms\n";
        cout << "[METRIK] Waktu Menampilkan Subkategori (DFS): " << duration_traverse.count() << " ms\n";
    }
    else
    {
        cout << "Error: Kategori tidak ditemukan.\n";
    }
}

bool CategoryMapManager::existsCategory(const string &query) const
{
    string searchId = query;
    if (nameToIdMap.count(query) > 0)
    {
        searchId = nameToIdMap.at(query);
    }
    return categoryMap.count(searchId) > 0;
}

void CategoryMapManager::deleteCategory(const string &id)
{
    if (categoryMap.count(id) == 0)
    {
        cout << "Error: Kategori tidak ditemukan.\n";
        return;
    }

    auto start_delete = chrono::high_resolution_clock::now();

    deleteRecursive(id);

    auto end_delete = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration_delete = end_delete - start_delete;

    cout << "Sukses: Kategori ID '" << id << "' beserta seluruh turunannya dihapus.\n";
    cout << "[METRIK] Waktu Delete Cascading memori: " << duration_delete.count() << " ms\n";
}

void CategoryMapManager::loadDataFromJsonFile(const string &filename)
{
    dbFile = filename;
    loadData();
}

void CategoryMapManager::loadDataFromVector(const vector<Category> &categories)
{
    categoryMap.clear();
    nameToIdMap.clear();

    for (const Category &cat : categories)
    {
        categoryMap[cat.id] = cat;
        nameToIdMap[cat.name] = cat.id;
    }
}

string CategoryMapManager::resolveId(const string &query) const
{
    if (nameToIdMap.count(query) > 0)
        return nameToIdMap.at(query);
    return query;
}

bool CategoryMapManager::exists(const string &query) const
{
    string id = resolveId(query);
    return categoryMap.count(id) > 0;
}

const Category *CategoryMapManager::getCategory(const string &query) const
{
    string id = resolveId(query);
    auto it = categoryMap.find(id);
    return it == categoryMap.end() ? nullptr : &it->second;
}

vector<string> CategoryMapManager::getChildIds(const string &id) const
{
    if (categoryMap.count(id) == 0)
        return {};
    return categoryMap.at(id).child_ids;
}

vector<string> CategoryMapManager::getRootIds() const
{
    vector<string> roots;
    for (const auto &pair : categoryMap)
    {
        const Category &cat = pair.second;
        if (cat.parent_id.empty() && cat.is_active)
        {
            roots.push_back(pair.first);
        }
    }
    return roots;
}

size_t CategoryMapManager::countSubtreeNodes(const string &root_id)
{
    if (categoryMap.count(root_id) == 0)
        return 0;
    return countSubtreeNodesRecursive(root_id);
}

CategoryNode::CategoryNode(const string &id, const string &name, const string &parent_id, int level)
    : id(id), name(name), parent_id(parent_id), level(level), is_active(true), firstChild(nullptr), nextSibling(nullptr), parent(nullptr)
{
}

CategoryPointerTree::CategoryPointerTree() : root(nullptr)
{
    root = new CategoryNode("ROOT", "ROOT", "", 0);
    root->is_active = true;
}

CategoryPointerTree::~CategoryPointerTree()
{
    clearTree();
}

void CategoryPointerTree::clearSubtree(CategoryNode *node)
{
    if (node == nullptr)
        return;

    clearSubtree(node->firstChild);
    clearSubtree(node->nextSibling);

    if (!node->id.empty())
    {
        idIndex.erase(node->id);
        nameIndex.erase(node->name);
    }

    delete node;
}

void CategoryPointerTree::attachChild(CategoryNode *parent, CategoryNode *child)
{
    if (parent->firstChild == nullptr)
    {
        parent->firstChild = child;
    }
    else
    {
        CategoryNode *current = parent->firstChild;
        while (current->nextSibling != nullptr)
        {
            current = current->nextSibling;
        }
        current->nextSibling = child;
    }
    child->parent = parent;
}

size_t CategoryPointerTree::countNodesRecursive(const CategoryNode *node) const
{
    if (node == nullptr)
        return 0;

    size_t count = 1;
    count += countNodesRecursive(node->firstChild);
    count += countNodesRecursive(node->nextSibling);
    return count;
}

void CategoryPointerTree::clearTree()
{
    clearSubtree(root);
    root = nullptr;
    idIndex.clear();
    nameIndex.clear();
}

void CategoryPointerTree::loadDataFromVector(const vector<Category> &categories)
{
    clearTree();
    root = new CategoryNode("ROOT", "ROOT", "", 0);
    root->is_active = true;

    for (const Category &cat : categories)
    {
        CategoryNode *node = new CategoryNode(cat.id, cat.name, cat.parent_id, cat.level);
        node->is_active = cat.is_active;
        idIndex[cat.id] = node;
        nameIndex[cat.name] = node;
    }

    for (const Category &cat : categories)
    {
        CategoryNode *node = idIndex[cat.id];
        if (cat.parent_id.empty() || idIndex.count(cat.parent_id) == 0)
        {
            attachChild(root, node);
        }
        else
        {
            attachChild(idIndex[cat.parent_id], node);
        }
    }
}

CategoryNode *CategoryPointerTree::searchById(const string &id) const
{
    if (idIndex.count(id) == 0)
        return nullptr;
    return idIndex.at(id);
}

CategoryNode *CategoryPointerTree::searchByName(const string &name) const
{
    if (nameIndex.count(name) == 0)
        return nullptr;
    return nameIndex.at(name);
}

bool CategoryPointerTree::exists(const string &query) const
{
    return idIndex.count(query) > 0 || nameIndex.count(query) > 0;
}

CategoryNode *CategoryPointerTree::findNode(const string &query) const
{
    if (idIndex.count(query) > 0)
        return idIndex.at(query);
    if (nameIndex.count(query) > 0)
        return nameIndex.at(query);
    return nullptr;
}

vector<CategoryNode *> CategoryPointerTree::getRootNodes() const
{
    vector<CategoryNode *> roots;
    CategoryNode *current = root->firstChild;
    while (current != nullptr)
    {
        roots.push_back(current);
        current = current->nextSibling;
    }
    return roots;
}

void CategoryPointerTree::printTree(CategoryNode *node, const string &indent) const
{
    if (node == nullptr)
        return;

    if (!node->id.empty())
    {
        cout << indent << "|-- [" << node->id << "] " << node->name << "\n";
    }

    printTree(node->firstChild, indent + "    ");
    if (!node->id.empty())
    {
        printTree(node->nextSibling, indent);
    }
}

void CategoryPointerTree::printHierarchy() const
{
    printTree(root->firstChild, "");
}

size_t CategoryPointerTree::countSubtreeNodes(const string &id) const
{
    const CategoryNode *node = nullptr;
    if (id.empty())
        node = root->firstChild;
    else if (idIndex.count(id) > 0)
        node = idIndex.at(id);

    if (node == nullptr)
        return 0;

    return countNodesRecursive(node);
}

bool CategoryPointerTree::insertCategory(const string &id, const string &name, const string &parent_id)
{
    if (idIndex.count(id) > 0)
        return false;

    CategoryNode *newNode = new CategoryNode(id, name, parent_id, 1);
    newNode->is_active = true;

    CategoryNode *parentNode = root;
    if (!parent_id.empty())
    {
        if (idIndex.count(parent_id) == 0)
        {
            delete newNode;
            return false;
        }
        parentNode = idIndex[parent_id];
        newNode->level = parentNode->level + 1;
    }

    attachChild(parentNode, newNode);
    idIndex[id] = newNode;
    nameIndex[name] = newNode;
    return true;
}

bool CategoryPointerTree::removeFromParent(CategoryNode *node)
{
    if (node == nullptr || node->parent == nullptr)
        return false;

    CategoryNode *parentNode = node->parent;
    if (parentNode->firstChild == node)
    {
        parentNode->firstChild = node->nextSibling;
    }
    else
    {
        CategoryNode *current = parentNode->firstChild;
        while (current != nullptr && current->nextSibling != node)
        {
            current = current->nextSibling;
        }
        if (current != nullptr)
        {
            current->nextSibling = node->nextSibling;
        }
    }

    node->parent = nullptr;
    node->nextSibling = nullptr;
    return true;
}

void CategoryPointerTree::deleteSubtree(CategoryNode *node)
{
    if (node == nullptr)
        return;

    clearSubtree(node->firstChild);
    node->firstChild = nullptr;

    idIndex.erase(node->id);
    nameIndex.erase(node->name);
    delete node;
}

bool CategoryPointerTree::deleteCategory(const string &id)
{
    if (idIndex.count(id) == 0)
        return false;

    CategoryNode *node = idIndex[id];
    removeFromParent(node);
    deleteSubtree(node);
    return true;
}

vector<Category> loadCategoryListFromJson(const string &filename)
{
    vector<Category> categories;
    ifstream file(filename);
    if (!file.is_open())
    {
        return categories;
    }

    json j;
    try
    {
        file >> j;
    }
    catch (const exception &)
    {
        return categories;
    }

    for (auto it = j.begin(); it != j.end(); ++it)
    {
        Category cat;
        cat.id = it.value()["id"];
        cat.name = it.value()["name"];
        cat.parent_id = it.value()["parent_id"];
        cat.level = it.value()["level"];
        cat.is_active = it.value()["is_active"];

        cat.child_ids.clear();
        for (const auto &child : it.value()["child_ids"])
        {
            cat.child_ids.push_back(child);
        }

        categories.push_back(cat);
    }

    return categories;
}

DatasetLoadResult loadDatasetFromJsonFile(const string &filename)
{
    DatasetLoadResult result;
    ifstream file(filename);
    if (!file.is_open())
    {
        result.errorMessage = "File tidak ditemukan.";
        return result;
    }

    auto start = chrono::high_resolution_clock::now();

    json j;
    try
    {
        file >> j;
    }
    catch (const exception &)
    {
        result.errorMessage = "Format JSON tidak valid.";
        return result;
    }

    vector<Category> allCategories;
    for (auto it = j.begin(); it != j.end(); ++it)
    {
        Category cat;
        cat.id = it.value()["id"];
        cat.name = it.value()["name"];
        cat.parent_id = it.value()["parent_id"];
        cat.level = it.value()["level"];
        cat.is_active = it.value()["is_active"];

        cat.child_ids.clear();
        for (const auto &child : it.value()["child_ids"])
        {
            cat.child_ids.push_back(child);
        }

        allCategories.push_back(cat);
    }

    unordered_set<string> validIds;
    for (const auto &cat : allCategories)
    {
        validIds.insert(cat.id);
    }

    vector<Category> validCategories = allCategories;
    vector<string> orphanIds;
    bool removed = true;
    while (removed)
    {
        removed = false;
        vector<Category> next;
        for (const auto &cat : validCategories)
        {
            if (cat.parent_id.empty() || validIds.count(cat.parent_id) > 0)
            {
                next.push_back(cat);
            }
            else
            {
                orphanIds.push_back(cat.id);
                validIds.erase(cat.id);
                removed = true;
            }
        }
        validCategories.swap(next);
    }

    auto end = chrono::high_resolution_clock::now();
    result.categories = move(validCategories);
    result.orphanIds = move(orphanIds);
    result.loadTimeMs = chrono::duration<double, milli>(end - start).count();
    result.success = true;
    return result;
}

void calculateDatasetStats(const vector<Category> &categories, AppState &state)
{
    state.nodeCount = categories.size();
    state.rootCount = 0;
    state.maxDepth = 0;
    state.deepestNodeId.clear();
    state.deepestNodeName.clear();
    state.widestNodeSize = 0;
    state.widestNodeId.clear();

    unordered_map<string, const Category *> index;
    for (const auto &cat : categories)
    {
        index[cat.id] = &cat;
    }

    vector<string> roots;
    for (const auto &cat : categories)
    {
        if (cat.parent_id.empty() || index.count(cat.parent_id) == 0)
        {
            roots.push_back(cat.id);
        }
    }
    state.rootCount = roots.size();

    unordered_map<string, size_t> subtreeCache;
    function<size_t(const string &)> computeSubtreeSize = [&](const string &id) -> size_t {
        if (subtreeCache.count(id) > 0)
            return subtreeCache[id];
        size_t total = 1;
        const Category *node = index.at(id);
        for (const auto &childId : node->child_ids)
        {
            if (index.count(childId) > 0)
            {
                total += computeSubtreeSize(childId);
            }
        }
        subtreeCache[id] = total;
        return total;
    };

    function<void(const string &, int)> dfs = [&](const string &id, int depth) {
        if (depth > state.maxDepth)
        {
            state.maxDepth = depth;
            state.deepestNodeId = id;
            state.deepestNodeName = index.at(id)->name;
        }
        size_t subtreeSize = computeSubtreeSize(id);
        if (subtreeSize > state.widestNodeSize)
        {
            state.widestNodeSize = subtreeSize;
            state.widestNodeId = id;
        }
        for (const auto &childId : index.at(id)->child_ids)
        {
            if (index.count(childId) > 0)
                dfs(childId, depth + 1);
        }
    };

    if (roots.empty() && !categories.empty())
    {
        state.deepestNodeId = categories.front().id;
        state.deepestNodeName = categories.front().name;
        state.widestNodeId = categories.front().id;
        state.widestNodeSize = computeSubtreeSize(categories.front().id);
        state.maxDepth = 1;
    }

    for (const auto &rootId : roots)
    {
        dfs(rootId, 1);
    }
}

void buildActiveStructures(AppState &state, CategoryMapManager &hashManager, CategoryPointerTree &pointerTree)
{
    hashManager.loadDataFromVector(state.dataset);
    pointerTree.loadDataFromVector(state.dataset);
}

void collectSubtreeIdsHashMap(const CategoryMapManager &manager, const string &id, vector<string> &result)
{
    result.push_back(id);
    for (const string &childId : manager.getChildIds(id))
    {
        collectSubtreeIdsHashMap(manager, childId, result);
    }
}

void collectSubtreeIdsPointerTree(CategoryNode *node, vector<string> &result)
{
    if (node == nullptr)
        return;

    result.push_back(node->id);
    for (CategoryNode *child = node->firstChild; child != nullptr; child = child->nextSibling)
    {
        collectSubtreeIdsPointerTree(child, result);
    }
}

void removeCategoriesFromDataset(vector<Category> &dataset, const unordered_set<string> &idsToRemove)
{
    vector<Category> newData;
    newData.reserve(dataset.size());
    for (const auto &cat : dataset)
    {
        if (idsToRemove.count(cat.id) == 0)
            newData.push_back(cat);
    }
    dataset.swap(newData);
}

BenchmarkResult benchmarkStructure(const vector<Category> &categories, StructureType type, int repeat)
{
    BenchmarkResult result;
    result.memKb = estimateMemoryUsageKB(categories.size());
    result.label = to_string(categories.size()) + " node";

    vector<double> buildTimes;
    vector<double> insertTimes;
    vector<double> searchTimes;
    vector<double> traversalTimes;
    vector<double> deleteTimes;

    string sampleId = categories.empty() ? string() : categories.front().id;
    string sampleName = categories.empty() ? string() : categories.front().name;

    for (int run = 0; run < repeat; ++run)
    {
        if (type == StructureType::HashMapTree)
        {
            CategoryMapManager manager;
            auto startBuild = chrono::high_resolution_clock::now();
            manager.loadDataFromVector(categories);
            auto endBuild = chrono::high_resolution_clock::now();
            buildTimes.push_back(chrono::duration<double, milli>(endBuild - startBuild).count());

            if (!sampleId.empty())
            {
                string insertId = "BM_INS_" + to_string(run);
                auto startInsert = chrono::high_resolution_clock::now();
                manager.insertCategoryRaw(insertId, "BenchmarkInsert", sampleId);
                auto endInsert = chrono::high_resolution_clock::now();
                insertTimes.push_back(chrono::duration<double, milli>(endInsert - startInsert).count());

                auto startSearch = chrono::high_resolution_clock::now();
                manager.exists(sampleId);
                auto endSearch = chrono::high_resolution_clock::now();
                searchTimes.push_back(chrono::duration<double, milli>(endSearch - startSearch).count());

                auto startTraversal = chrono::high_resolution_clock::now();
                manager.countSubtreeNodes(sampleId);
                auto endTraversal = chrono::high_resolution_clock::now();
                traversalTimes.push_back(chrono::duration<double, milli>(endTraversal - startTraversal).count());

                auto startDelete = chrono::high_resolution_clock::now();
                manager.deleteCategoryRaw(sampleId);
                auto endDelete = chrono::high_resolution_clock::now();
                deleteTimes.push_back(chrono::duration<double, milli>(endDelete - startDelete).count());
            }
        }
        else
        {
            CategoryPointerTree tree;
            auto startBuild = chrono::high_resolution_clock::now();
            tree.loadDataFromVector(categories);
            auto endBuild = chrono::high_resolution_clock::now();
            buildTimes.push_back(chrono::duration<double, milli>(endBuild - startBuild).count());

            if (!sampleId.empty())
            {
                string insertId = "BM_INS_" + to_string(run);
                auto startInsert = chrono::high_resolution_clock::now();
                tree.insertCategory(insertId, "BenchmarkInsert", sampleId);
                auto endInsert = chrono::high_resolution_clock::now();
                insertTimes.push_back(chrono::duration<double, milli>(endInsert - startInsert).count());

                auto startSearch = chrono::high_resolution_clock::now();
                tree.exists(sampleId);
                auto endSearch = chrono::high_resolution_clock::now();
                searchTimes.push_back(chrono::duration<double, milli>(endSearch - startSearch).count());

                auto startTraversal = chrono::high_resolution_clock::now();
                tree.countSubtreeNodes(sampleId);
                auto endTraversal = chrono::high_resolution_clock::now();
                traversalTimes.push_back(chrono::duration<double, milli>(endTraversal - startTraversal).count());

                auto startDelete = chrono::high_resolution_clock::now();
                tree.deleteCategory(sampleId);
                auto endDelete = chrono::high_resolution_clock::now();
                deleteTimes.push_back(chrono::duration<double, milli>(endDelete - startDelete).count());
            }
        }
    }

    auto medianValue = [&](vector<double> values) -> double {
        if (values.empty())
            return 0.0;
        sort(values.begin(), values.end());
        return values[values.size() / 2];
    };

    result.build = medianValue(move(buildTimes));
    result.insert = medianValue(move(insertTimes));
    result.search = medianValue(move(searchTimes));
    result.traversal = medianValue(move(traversalTimes));
    result.remove = medianValue(move(deleteTimes));
    return result;
}
