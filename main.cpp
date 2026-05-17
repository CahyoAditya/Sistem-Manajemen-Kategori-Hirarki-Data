#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <limits>
#include <iomanip>
#include <cctype>
#include <cmath>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

enum class StructureType
{
    HashMapTree,
    PointerTree
};

const vector<string> kStructureNames = {"HashMap Tree", "Pointer Tree"};

bool supportsUtf8()
{
    const char *lang = getenv("LANG");
    return lang != nullptr && string(lang).find("UTF-8") != string::npos;
}

bool supportsColor()
{
    return isatty(STDOUT_FILENO) != 0;
}

string repeatChar(char c, int count)
{
    return string(max(0, count), c);
}

string repeatString(const string &s, int count)
{
    string result;
    for (int i = 0; i < count; ++i)
        result += s;
    return result;
}

string trim(const string &value)
{
    size_t start = 0;
    while (start < value.size() && isspace((unsigned char)value[start]))
        start++;
    size_t end = value.size();
    while (end > start && isspace((unsigned char)value[end - 1]))
        end--;
    return value.substr(start, end - start);
}

string truncateText(const string &value, size_t width)
{
    if (value.size() <= width)
        return value;
    if (width <= 3)
        return string(width, '.');
    return value.substr(0, width - 3) + "...";
}

int getMenuChoice(int minChoice, int maxChoice)
{
    int choice;
    while (true)
    {
        if (!(cin >> choice) || choice < minChoice || choice > maxChoice)
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "  ✗ Input tidak valid. Masukkan angka " << minChoice << "-" << maxChoice << ": ";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return choice;
    }
}

bool confirmPrompt(const string &prompt)
{
    while (true)
    {
        cout << prompt;
        string response;
        getline(cin, response);
        response = trim(response);
        if (response.empty())
            continue;
        char c = response[0];
        if (c == 'y' || c == 'Y')
            return true;
        if (c == 'n' || c == 'N')
            return false;
        cout << "  ✗ Input tidak valid. Masukkan y/n: ";
    }
}

void printSeparator(char ch = '=', int width = 64)
{
    cout << repeatChar(ch, width) << "\n";
}

string getBoxTopBorder()
{
    if (supportsUtf8())
        return string("╔") + repeatString("═", 62) + string("╗");
    return "+" + repeatChar('=', 62) + "+";
}

string getBoxBottomBorder()
{
    if (supportsUtf8())
        return string("╚") + repeatString("═", 62) + string("╝");
    return "+" + repeatChar('=', 62) + "+";
}

string getBoxMidSeparator()
{
    if (supportsUtf8())
        return string("╠") + repeatString("═", 62) + string("╣");
    return "+" + repeatChar('=', 62) + "+";
}

string getBoxLine(const string &content)
{
    string border = supportsUtf8() ? "║" : "|";
    int contentLen = content.length();
    int padding = max(0, 62 - contentLen);
    int padLeft = padding / 2;
    int padRight = padding - padLeft;
    return border + repeatChar(' ', padLeft) + content + repeatChar(' ', padRight) + border;
}

void printBoxedContent(const vector<string> &lines)
{
    cout << getBoxTopBorder() << "\n";
    for (const string &line : lines)
    {
        string border = supportsUtf8() ? "║" : "|";
        int contentLen = line.length();
        int padding = max(0, 62 - contentLen);
        int padRight = padding;
        cout << border << " " << left << setw(61) << line << border << "\n";
    }
    cout << getBoxBottomBorder() << "\n";
}

void printHeader(StructureType active, size_t nodeCount, double loadTimeMs, const string &fileName)
{
    cout << "\n";
    vector<string> lines;
    lines.push_back("SISTEM MANAJEMEN KATEGORI & HIRARKI DATA");
    lines.push_back("Struktur Data P4 — Kelompok 10 — IPB University 2026");
    printBoxedContent(lines);
    
    cout << getBoxTopBorder() << "\n";
    string border = supportsUtf8() ? "║" : "|";
    cout << border << "  Struktur Aktif : " << left << setw(43) << kStructureNames[static_cast<int>(active)] << border << "\n";
    cout << border << "  Node Termuat   : " << left << setw(43) << nodeCount << border << "\n";
    cout << border << "  Waktu Load     : " << fixed << setprecision(2) << left << setw(43) << (to_string(loadTimeMs) + " ms") << border << "\n";
    if (!fileName.empty())
    {
        cout << border << "  Dataset file   : " << left << setw(43) << fileName << border << "\n";
    }
    cout << getBoxBottomBorder() << "\n";
}

void printMenuTitle(const string &title)
{
    cout << "\n";
    cout << getBoxTopBorder() << "\n";
    string border = supportsUtf8() ? "║" : "|";
    string titleLine = "  [" + title + "]";
    cout << border << " " << left << setw(61) << titleLine << border << "\n";
    cout << getBoxBottomBorder() << "\n";
}

void printMetric(const string &name, double ms)
{
    cout << "  [METRIK] Waktu " << left << setw(20) << name << " : " << fixed << setprecision(3) << ms << " ms\n";
}

void printSuccess(const string &message)
{
    cout << "  ✓ " << message << "\n";
}

void printError(const string &message)
{
    cout << "  ✗ " << message << "\n";
}

void printWarning(const string &message)
{
    cout << "  ! " << message << "\n";
}

void printMenuItems(const vector<string> &items)
{
    cout << "\n";
    for (const string &item : items)
    {
        cout << item << "\n";
    }
    cout << "\n";
}

void printMenuItemsBoxed(const vector<string> &items)
{
    cout << getBoxTopBorder() << "\n";
    string border = supportsUtf8() ? "║" : "|";
    for (const string &item : items)
    {
        int contentLen = item.length();
        int padding = max(0, 62 - contentLen);
        cout << border << " " << left << setw(61) << item << border << "\n";
    }
    cout << getBoxBottomBorder() << "\n";
}

size_t estimateMemoryUsageKB(size_t nodeCount)
{
    const size_t estimatedPerNodeBytes = 120; // estimasi kasar
    return (nodeCount * estimatedPerNodeBytes) / 1024;
}

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

// Inisialisasi Node
struct Category
{
    string id;
    string name;
    string parent_id;
    int level;
    bool is_active;
    vector<string> child_ids;
};

struct AppState
{
    StructureType activeStructure = StructureType::HashMapTree;
    string activeFile = "";
    vector<Category> dataset;
    bool datasetLoaded = false;
    double loadTimeMs = 0.0;
    size_t nodeCount = 0;
    size_t rootCount = 0;
    int maxDepth = 0;
    string deepestNodeId;
    string deepestNodeName;
    size_t widestNodeSize = 0;
    string widestNodeId;
};

// Bungkus semua function di dalam class
class CategoryMapManager
{
private:
    // Kenapa di dalam class? karena perlu unordered_map 
    // yang bisa diakses semua function
    unordered_map<string, Category> categoryMap;
    unordered_map<string, string> nameToIdMap;
    string dbFile = "dataset_kategori.json";

    void deleteRecursive(string id)
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


public:
    void printTree(string root_id, string indent)
    {
        // Base case
        if (categoryMap.count(root_id) == 0 || categoryMap[root_id].is_active == false)
            return;

        Category cat = categoryMap[root_id];
        cout << indent << "|-- [" << cat.id << "] " << cat.name << "\n";

        for (int i = 0; i < cat.child_ids.size(); i++)
        {
            string child_id = cat.child_ids[i];

            // Rekursif dan indensasi
            printTree(child_id, indent + "    ");
        }
    }

    void loadData()
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

    void saveData()
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

    // 1. Insert Kategori
    void insertCategory(string id, string name, string parent_id)
    {
        auto start_insert = chrono::high_resolution_clock::now();

        if (!insertCategoryRaw(id, name, parent_id))
            return;

        auto end_insert = chrono::high_resolution_clock::now();
        chrono::duration<double, std::milli> duration_insert = end_insert - start_insert;

        cout << "  ✓ Sukses! Kategori ditambahkan.\n";
        cout << "  [METRIK] Waktu Insert : " << duration_insert.count() << " ms\n";

        saveData();
    }

    bool insertCategoryRaw(const string &id, const string &name, const string &parent_id)
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

    bool deleteCategoryRaw(const string &id)
    {
        if (categoryMap.count(id) == 0)
            return false;

        deleteRecursive(id);
        return true;
    }

    // 2. Search Kategori
    void searchCategory(string query)
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
            chrono::duration<double, std::milli> duration_all = end_all - start_all;
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

            chrono::duration<double, std::milli> duration_search = end_search - start_search;
            chrono::duration<double, std::milli> duration_traverse = end_traverse - start_traverse;

            cout << "\n[METRIK] Waktu Search O(1) Lookup         : " << duration_search.count() << " ms\n";
            cout << "[METRIK] Waktu Menampilkan Subkategori (DFS): " << duration_traverse.count() << " ms\n";
        }
        else
        {
            cout << "Error: Kategori tidak ditemukan.\n";
        }
    }

    bool existsCategory(string query) const
    {
        string searchId = query;
        if (nameToIdMap.count(query) > 0)
        {
            searchId = nameToIdMap.at(query);
        }
        return categoryMap.count(searchId) > 0;
    }

    // 3. Delete Kategori
    void deleteCategory(string id)
    {
        if (categoryMap.count(id) == 0)
        {
            cout << "Error: Kategori tidak ditemukan.\n";
            return;
        }

        auto start_delete = chrono::high_resolution_clock::now();

        deleteRecursive(id);

        auto end_delete = chrono::high_resolution_clock::now();
        chrono::duration<double, std::milli> duration_delete = end_delete - start_delete;

        cout << "Sukses: Kategori ID '" << id << "' beserta seluruh turunannya dihapus.\n";
        cout << "[METRIK] Waktu Delete Cascading memori: " << duration_delete.count() << " ms\n";

        saveData();
    }

    void loadDataFromJsonFile(const string& filename)
    {
        dbFile = filename;
        loadData();
    }

    void loadDataFromVector(const vector<Category>& categories)
    {
        categoryMap.clear();
        nameToIdMap.clear();

        for (const Category& cat : categories)
        {
            categoryMap[cat.id] = cat;
            nameToIdMap[cat.name] = cat.id;
        }
    }

    string resolveId(const string& query) const
    {
        if (nameToIdMap.count(query) > 0)
            return nameToIdMap.at(query);
        return query;
    }

    bool exists(const string& query) const
    {
        string id = resolveId(query);
        return categoryMap.count(id) > 0;
    }

    const Category* getCategory(const string& query) const
    {
        string id = resolveId(query);
        auto it = categoryMap.find(id);
        return it == categoryMap.end() ? nullptr : &it->second;
    }

    vector<string> getChildIds(const string& id) const
    {
        if (categoryMap.count(id) == 0)
            return {};
        return categoryMap.at(id).child_ids;
    }

    vector<string> getRootIds() const
    {
        vector<string> roots;
        for (const auto& pair : categoryMap)
        {
            const Category& cat = pair.second;
            if (cat.parent_id.empty() && cat.is_active)
            {
                roots.push_back(pair.first);
            }
        }
        return roots;
    }

    size_t countSubtreeNodes(const string& root_id)
    {
        if (categoryMap.count(root_id) == 0)
            return 0;

        return countSubtreeNodesRecursive(root_id);
    }

private:
    size_t countSubtreeNodesRecursive(const string& id)
    {
        if (categoryMap.count(id) == 0)
            return 0;

        size_t count = 1;
        for (const string& child_id : categoryMap[id].child_ids)
        {
            count += countSubtreeNodesRecursive(child_id);
        }
        return count;
    }
};

struct CategoryNode
{
    string id;
    string name;
    string parent_id;
    int level;
    bool is_active;
    CategoryNode* firstChild;
    CategoryNode* nextSibling;
    CategoryNode* parent;

    CategoryNode(const string& id, const string& name, const string& parent_id, int level)
        : id(id), name(name), parent_id(parent_id), level(level), is_active(true), firstChild(nullptr), nextSibling(nullptr), parent(nullptr)
    {
    }
};

class CategoryPointerTree
{
private:
    CategoryNode* root;
    unordered_map<string, CategoryNode*> idIndex;
    unordered_map<string, CategoryNode*> nameIndex;

    void clearSubtree(CategoryNode* node)
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

    void attachChild(CategoryNode* parent, CategoryNode* child)
    {
        if (parent->firstChild == nullptr)
        {
            parent->firstChild = child;
        }
        else
        {
            CategoryNode* current = parent->firstChild;
            while (current->nextSibling != nullptr)
            {
                current = current->nextSibling;
            }
            current->nextSibling = child;
        }
        child->parent = parent;
    }

    size_t countNodesRecursive(const CategoryNode* node) const
    {
        if (node == nullptr)
            return 0;

        size_t count = 1;
        count += countNodesRecursive(node->firstChild);
        count += countNodesRecursive(node->nextSibling);
        return count;
    }

public:
    CategoryPointerTree() : root(nullptr)
    {
        root = new CategoryNode("ROOT", "ROOT", "", 0);
        root->is_active = true;
    }

    ~CategoryPointerTree()
    {
        clearTree();
    }

    void clearTree()
    {
        clearSubtree(root);
        root = nullptr;
        idIndex.clear();
        nameIndex.clear();
    }

    void loadDataFromVector(const vector<Category>& categories)
    {
        clearTree();
        root = new CategoryNode("ROOT", "ROOT", "", 0);
        root->is_active = true;

        for (const Category& cat : categories)
        {
            CategoryNode* node = new CategoryNode(cat.id, cat.name, cat.parent_id, cat.level);
            node->is_active = cat.is_active;
            idIndex[cat.id] = node;
            nameIndex[cat.name] = node;
        }

        for (const Category& cat : categories)
        {
            CategoryNode* node = idIndex[cat.id];
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

    CategoryNode* searchById(const string& id) const
    {
        if (idIndex.count(id) == 0)
            return nullptr;
        return idIndex.at(id);
    }

    CategoryNode* searchByName(const string& name) const
    {
        if (nameIndex.count(name) == 0)
            return nullptr;
        return nameIndex.at(name);
    }

    bool exists(const string& query) const
    {
        return idIndex.count(query) > 0 || nameIndex.count(query) > 0;
    }

    CategoryNode* findNode(const string& query) const
    {
        if (idIndex.count(query) > 0)
            return idIndex.at(query);
        if (nameIndex.count(query) > 0)
            return nameIndex.at(query);
        return nullptr;
    }

    vector<CategoryNode*> getRootNodes() const
    {
        vector<CategoryNode*> roots;
        CategoryNode* current = root->firstChild;
        while (current != nullptr)
        {
            roots.push_back(current);
            current = current->nextSibling;
        }
        return roots;
    }

    void printTree(CategoryNode* node, const string& indent) const
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

    void printHierarchy() const
    {
        printTree(root->firstChild, "");
    }

    size_t countSubtreeNodes(const string& id) const
    {
        const CategoryNode* node = nullptr;
        if (id.empty())
            node = root->firstChild;
        else if (idIndex.count(id) > 0)
            node = idIndex.at(id);

        if (node == nullptr)
            return 0;

        return countNodesRecursive(node);
    }

    bool insertCategory(const string& id, const string& name, const string& parent_id)
    {
        if (idIndex.count(id) > 0)
            return false;

        CategoryNode* newNode = new CategoryNode(id, name, parent_id, 1);
        newNode->is_active = true;

        CategoryNode* parentNode = root;
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

    bool removeFromParent(CategoryNode* node)
    {
        if (node == nullptr || node->parent == nullptr)
            return false;

        CategoryNode* parentNode = node->parent;
        if (parentNode->firstChild == node)
        {
            parentNode->firstChild = node->nextSibling;
        }
        else
        {
            CategoryNode* current = parentNode->firstChild;
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

    void deleteSubtree(CategoryNode* node)
    {
        if (node == nullptr)
            return;

        clearSubtree(node->firstChild);
        node->firstChild = nullptr;

        idIndex.erase(node->id);
        nameIndex.erase(node->name);
        delete node;
    }

    bool deleteCategory(const string& id)
    {
        if (idIndex.count(id) == 0)
            return false;

        CategoryNode* node = idIndex[id];
        removeFromParent(node);
        deleteSubtree(node);
        return true;
    }
};

vector<Category> loadCategoryListFromJson(const string& filename)
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

struct DatasetLoadResult
{
    vector<Category> categories;
    vector<string> orphanIds;
    double loadTimeMs = 0.0;
    string errorMessage;
    bool success = false;
};

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

string branchPrefix(bool isLast)
{
    if (supportsUtf8())
        return isLast ? "└── " : "├── ";
    return isLast ? "+-- " : "|-- ";
}

string childPrefix(bool isLast)
{
    if (supportsUtf8())
        return isLast ? "    " : "│   ";
    return isLast ? "    " : "|   ";
}

void printHashMapTreeUI(const CategoryMapManager &manager, const string &id, const string &prefix, bool isLast, int depthLimit, int currentDepth)
{
    const Category *cat = manager.getCategory(id);
    if (cat == nullptr)
        return;

    cout << prefix << branchPrefix(isLast) << "[" << cat->id << "] " << truncateText(cat->name, 40) << "\n";
    if (depthLimit >= 0 && currentDepth >= depthLimit)
        return;

    vector<string> children = manager.getChildIds(cat->id);
    for (size_t i = 0; i < children.size(); ++i)
    {
        bool nextIsLast = (i + 1 == children.size());
        printHashMapTreeUI(manager, children[i], prefix + childPrefix(isLast), nextIsLast, depthLimit, currentDepth + 1);
    }
}

void printPointerTreeUI(const CategoryPointerTree &tree, CategoryNode *node, const string &prefix, bool isLast, int depthLimit, int currentDepth)
{
    if (node == nullptr)
        return;

    cout << prefix << branchPrefix(isLast) << "[" << node->id << "] " << truncateText(node->name, 40) << "\n";
    if (depthLimit >= 0 && currentDepth >= depthLimit)
        return;

    vector<CategoryNode *> children;
    for (CategoryNode *child = node->firstChild; child != nullptr; child = child->nextSibling)
    {
        children.push_back(child);
    }

    for (size_t i = 0; i < children.size(); ++i)
    {
        bool nextIsLast = (i + 1 == children.size());
        printPointerTreeUI(tree, children[i], prefix + childPrefix(isLast), nextIsLast, depthLimit, currentDepth + 1);
    }
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

void printHierarchyUI(const AppState &state, const CategoryMapManager &hashManager, const CategoryPointerTree &pointerTree, int mode, const string &id, int depthLimit)
{
    if (state.activeStructure == StructureType::HashMapTree)
    {
        if (mode == 1)
        {
            vector<string> roots = hashManager.getRootIds();
            for (size_t i = 0; i < roots.size(); ++i)
            {
                bool isLast = (i + 1 == roots.size());
                printHashMapTreeUI(hashManager, roots[i], "", isLast, depthLimit, 1);
            }
        }
        else if (mode == 2)
        {
            printHashMapTreeUI(hashManager, id, "", true, depthLimit, 1);
        }
    }
    else
    {
        if (mode == 1)
        {
            vector<CategoryNode *> roots = pointerTree.getRootNodes();
            for (size_t i = 0; i < roots.size(); ++i)
            {
                bool isLast = (i + 1 == roots.size());
                printPointerTreeUI(pointerTree, roots[i], "", isLast, depthLimit, 1);
            }
        }
        else if (mode == 2)
        {
            CategoryNode *node = pointerTree.findNode(id);
            if (node != nullptr)
                printPointerTreeUI(pointerTree, node, "", true, depthLimit, 1);
        }
    }
}

double medianValue(vector<double> values)
{
    if (values.empty())
        return 0.0;
    sort(values.begin(), values.end());
    return values[values.size() / 2];
}

struct BenchmarkResult
{
    string label;
    double build = 0.0;
    double insert = 0.0;
    double search = 0.0;
    double traversal = 0.0;
    double remove = 0.0;
    double memKb = 0.0;
};

BenchmarkResult benchmarkStructure(const vector<Category> &categories, StructureType type, int repeat = 3)
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

    result.build = medianValue(move(buildTimes));
    result.insert = medianValue(move(insertTimes));
    result.search = medianValue(move(searchTimes));
    result.traversal = medianValue(move(traversalTimes));
    result.remove = medianValue(move(deleteTimes));
    return result;
}

void printBenchmarkSummary(const vector<BenchmarkResult> &results, const string &title)
{
    printMenuTitle(title);
    cout << "  ╔═══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╗\n";
    cout << "  ║ Dataset       ║ Build (ms)   ║ Insert (ms)  ║ Search (ms)  ║ Traversal(ms)║ Delete (ms)   ║\n";
    cout << "  ╠═══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n";
    for (const auto &row : results)
    {
        cout << "  ║ " << left << setw(13) << row.label
             << " ║ " << right << setw(10) << fixed << setprecision(3) << row.build
             << " ║ " << setw(10) << row.insert
             << " ║ " << setw(10) << row.search
             << " ║ " << setw(11) << row.traversal
             << " ║ " << setw(11) << row.remove << " ║\n";
    }
    cout << "  ╚═══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╝\n";
}

void printBenchmarkComparison(const BenchmarkResult &hashResult, const BenchmarkResult &pointerResult)
{
    printMenuTitle("BENCHMARK PERBANDINGAN");
    cout << "  ╔══════════════╦════════════════════╦════════════════════╦══════════╗\n";
    cout << "  ║ Operasi      ║ HashMap Tree (ms)  ║ Pointer Tree (ms)  ║ Selisih   ║\n";
    cout << "  ╠══════════════╬════════════════════╬════════════════════╬══════════╣\n";
    auto printRow = [&](const string &label, double hashValue, double pointerValue) {
        double diff = pointerValue - hashValue;
        cout << "  ║ " << left << setw(12) << label
             << " ║ " << right << setw(18) << fixed << setprecision(3) << hashValue
             << " ║ " << setw(18) << pointerValue
             << " ║ " << setw(8) << showpos << fixed << setprecision(3) << diff << noshowpos << " ║\n";
    };
    printRow("Build", hashResult.build, pointerResult.build);
    printRow("Insert", hashResult.insert, pointerResult.insert);
    printRow("Search", hashResult.search, pointerResult.search);
    printRow("Traversal", hashResult.traversal, pointerResult.traversal);
    printRow("Delete", hashResult.remove, pointerResult.remove);
    cout << "  ╚══════════════╩════════════════════╩════════════════════╩══════════╝\n";
    cout << "\n  Keterangan Selisih: (+) Pointer Tree lebih lambat/besar, (-) lebih cepat/kecil\n";
}

bool exportBenchmarkCsv(const string &outputFile, const vector<string> &datasetFiles, const vector<BenchmarkResult> &hashResults, const vector<BenchmarkResult> &pointerResults)
{
    if (datasetFiles.size() != hashResults.size() || datasetFiles.size() != pointerResults.size())
        return false;
    string folder = "output";
    mkdir(folder.c_str(), 0755);
    ofstream csv(outputFile);
    if (!csv.is_open())
        return false;

    csv << "operasi,ukuran_data,hashmap_ms,pointer_ms\n";
    for (size_t i = 0; i < datasetFiles.size(); ++i)
    {
        string label = hashResults[i].label;
        csv << "Build," << label << "," << hashResults[i].build << "," << pointerResults[i].build << "\n";
        csv << "Insert," << label << "," << hashResults[i].insert << "," << pointerResults[i].insert << "\n";
        csv << "Search," << label << "," << hashResults[i].search << "," << pointerResults[i].search << "\n";
        csv << "Traversal," << label << "," << hashResults[i].traversal << "," << pointerResults[i].traversal << "\n";
        csv << "Delete," << label << "," << hashResults[i].remove << "," << pointerResults[i].remove << "\n";
    }
    csv.close();
    return true;
}

void benchmarkMenu()
{
    vector<string> datasetFiles = {"dataset_1000.json", "dataset_5000.json", "dataset_10000.json"};
    printMenuTitle("BENCHMARK & PERBANDINGAN");
    vector<string> benchmarkItems = {
        "[1] Benchmark Struktur Aktif",
        "[2] Benchmark Kedua Struktur (side-by-side)",
        "[3] Ekspor Hasil ke CSV",
        "[0] Kembali"
    };
    printMenuItemsBoxed(benchmarkItems);
    cout << "  Pilihan: ";
    int choice = getMenuChoice(0, 3);

    if (choice == 1)
    {
        printMenuTitle("BENCHMARK STRUKTUR AKTIF");
        cout << "  Pilih ukuran dataset:\n";
        cout << "  [1] 1.000 node    [2] 5.000 node    [3] 10.000 node    [4] Semua ukuran\n";
        cout << "  Pilihan: ";
        int sizeChoice = getMenuChoice(1, 4);

        vector<BenchmarkResult> results;
        if (sizeChoice == 4)
        {
            for (const auto &file : datasetFiles)
            {
                vector<Category> categories = loadCategoryListFromJson(file);
                if (!categories.empty())
                {
                    results.push_back(benchmarkStructure(categories, StructureType::HashMapTree));
                }
            }
            printBenchmarkSummary(results, "Benchmark Struktur Aktif");
        }
        else
        {
            string file = datasetFiles[sizeChoice - 1];
            vector<Category> categories = loadCategoryListFromJson(file);
            if (categories.empty())
            {
                printError("Tidak dapat memuat dataset " + file + ".");
                return;
            }
            auto result = benchmarkStructure(categories, StructureType::HashMapTree);
            printBenchmarkSummary({result}, "Benchmark Struktur Aktif");
        }
    }
    else if (choice == 2)
    {
        printMenuTitle("BENCHMARK PERBANDINGAN");
        string compareFile = "dataset_10000.json";
        vector<Category> categories = loadCategoryListFromJson(compareFile);
        if (categories.empty())
        {
            printError("Tidak dapat memuat dataset 10.000 node.");
            return;
        }
        auto hashResult = benchmarkStructure(categories, StructureType::HashMapTree);
        auto pointerResult = benchmarkStructure(categories, StructureType::PointerTree);
        printBenchmarkComparison(hashResult, pointerResult);
    }
    else if (choice == 3)
    {
        vector<BenchmarkResult> hashResults;
        vector<BenchmarkResult> pointerResults;
        for (const auto &file : datasetFiles)
        {
            vector<Category> categories = loadCategoryListFromJson(file);
            if (!categories.empty())
            {
                hashResults.push_back(benchmarkStructure(categories, StructureType::HashMapTree));
                pointerResults.push_back(benchmarkStructure(categories, StructureType::PointerTree));
            }
        }
        string outputFile = "output/benchmark_result.csv";
        if (exportBenchmarkCsv(outputFile, datasetFiles, hashResults, pointerResults))
        {
            printSuccess("File berhasil diekspor ke " + outputFile);
        }
        else
        {
            printError("Gagal menulis file CSV.");
        }
    }
}

int main()
{
    AppState state;
    CategoryMapManager hashManager;
    CategoryPointerTree pointerTree;

    bool running = true;
    while (running)
    {
        printHeader(state.activeStructure, state.nodeCount, state.loadTimeMs, state.activeFile);
        cout << "\n";
        cout << getBoxTopBorder() << "\n";
        string border = supportsUtf8() ? "║" : "|";
        cout << border << "  [1] Pilih Struktur Data" << repeatChar(' ', 37) << border << "\n";
        cout << border << "  [2] Manajemen Kategori" << repeatChar(' ', 38) << border << "\n";
        cout << border << "  [3] Benchmark & Perbandingan" << repeatChar(' ', 32) << border << "\n";
        cout << border << "  [4] Dataset" << repeatChar(' ', 50) << border << "\n";
        cout << border << "  [0] Keluar" << repeatChar(' ', 51) << border << "\n";
        cout << getBoxBottomBorder() << "\n";
        cout << "  Pilih menu: ";

        int choice = getMenuChoice(0, 4);

        if (choice == 1)
        {
            printMenuTitle("PILIH STRUKTUR DATA");
            vector<string> structMenuItems = {
                "[1] HashMap Tree",
                "    Deskripsi : N-ary Tree berbasis unordered_map + vector",
                "    Search    : O(1) | Insert : O(1) | Delete : O(K)",
                "",
                "[2] Pointer Tree (LCRS)",
                "    Deskripsi : N-ary Tree berbasis pointer rekursif",
                "    Search    : O(n) / O(1)* | Insert : O(1)* | Delete : O(K)",
                "    *) dengan indeks sekunder"
            };
            printMenuItemsBoxed(structMenuItems);
            cout << "  Pilihan saat ini: [" << (state.activeStructure == StructureType::HashMapTree ? 1 : 2) << "] " << kStructureNames[static_cast<int>(state.activeStructure)] << "\n";
            cout << "  Masukkan pilihan (1/2): ";
            int structChoice = getMenuChoice(1, 2);
            StructureType selected = structChoice == 1 ? StructureType::HashMapTree : StructureType::PointerTree;
            if (selected != state.activeStructure)
            {
                if (state.datasetLoaded)
                {
                    if (confirmPrompt("  ! Dataset aktif akan di-load ulang ke struktur baru. Lanjutkan? (y/n): "))
                    {
                        state.activeStructure = selected;
                    }
                }
                else
                {
                    state.activeStructure = selected;
                }
            }
        }
        else if (choice == 2)
        {
            printMenuTitle("MANAJEMEN KATEGORI");
            vector<string> categMenuItems = {
                "[1] Tambah Kategori",
                "[2] Cari Kategori",
                "[3] Tampilkan Hierarki",
                "[4] Hapus Kategori",
                "[0] Kembali"
            };
            printMenuItemsBoxed(categMenuItems);
            cout << "  Pilihan: ";
            int subChoice = getMenuChoice(0, 4);
            if (subChoice == 1)
            {
                printMenuTitle("TAMBAH KATEGORI");
                string id, name, parentId;
                cout << "  ID Kategori       : ";
                getline(cin, id);
                id = trim(id);
                cout << "  Nama Kategori     : ";
                getline(cin, name);
                name = trim(name);
                cout << "  ID Induk (Parent) : ";
                getline(cin, parentId);
                parentId = trim(parentId);
                if (!isValidCategoryId(id))
                {
                    printError("ID Kategori tidak valid. Tidak boleh kosong, mengandung spasi, atau lebih dari 50 karakter.");
                    continue;
                }
                if (!isValidCategoryName(name))
                {
                    printError("Nama Kategori tidak valid. Tidak boleh kosong atau lebih dari 100 karakter.");
                    continue;
                }
                if (!parentId.empty())
                {
                    bool parentExists = hashManager.exists(parentId) || pointerTree.exists(parentId);
                    if (!parentExists)
                    {
                        printError("ID Parent '" + parentId + "' tidak ditemukan.");
                        continue;
                    }
                }
                bool inserted = false;
                if (state.activeStructure == StructureType::HashMapTree)
                {
                    if (!hashManager.insertCategoryRaw(id, name, parentId))
                    {
                        printError("ID '" + id + "' sudah terdaftar dalam sistem.");
                        continue;
                    }
                    inserted = true;
                }
                else
                {
                    if (!pointerTree.insertCategory(id, name, parentId))
                    {
                        printError("ID '" + id + "' sudah terdaftar atau Parent tidak ditemukan.");
                        continue;
                    }
                    inserted = true;
                }
                if (inserted)
                {
                    int level = 1;
                    if (!parentId.empty())
                    {
                        const Category *parentCat = hashManager.getCategory(parentId);
                        if (parentCat != nullptr)
                            level = parentCat->level + 1;
                        else
                        {
                            CategoryNode *parentNode = pointerTree.findNode(parentId);
                            if (parentNode != nullptr)
                                level = parentNode->level + 1;
                        }
                    }
                    if (!parentId.empty())
                    {
                        for (auto &cat : state.dataset)
                        {
                            if (cat.id == parentId)
                            {
                                cat.child_ids.push_back(id);
                                break;
                            }
                        }
                    }
                    state.dataset.push_back(Category{id, name, parentId, level, true, {}});
                    calculateDatasetStats(state.dataset, state);
                    buildActiveStructures(state, hashManager, pointerTree);
                    printSuccess("Kategori berhasil ditambahkan.");
                }
            }
            else if (subChoice == 2)
            {
                printMenuTitle("CARI KATEGORI");
                cout << "  Masukkan ID atau Nama kategori (Enter = tampilkan semua):\n  > ";
                string query;
                getline(cin, query);
                query = trim(query);
                if (query.empty())
                {
                    if (state.nodeCount > 1000)
                    {
                        if (!confirmPrompt("  ! Dataset berisi " + to_string(state.nodeCount) + " node. Output mungkin panjang. Lanjutkan tampilkan ke terminal? (y/n): "))
                            continue;
                    }
                    printHierarchyUI(state, hashManager, pointerTree, 1, "", -1);
                    continue;
                }
                bool found = false;
                string foundId;
                if (state.activeStructure == StructureType::HashMapTree)
                {
                    const Category *cat = hashManager.getCategory(query);
                    if (cat != nullptr)
                    {
                        found = true;
                        foundId = cat->id;
                        printSuccess("Kategori ditemukan.");
                        cout << "  ID      : " << cat->id << "\n";
                        cout << "  Nama    : " << cat->name << "\n";
                        cout << "  Parent  : " << (cat->parent_id.empty() ? "(root)" : cat->parent_id) << "\n";
                        cout << "  Level   : " << cat->level << "\n";
                        cout << "  Status  : " << (cat->is_active ? "Aktif" : "Non-Aktif") << "\n";
                    }
                }
                else
                {
                    CategoryNode *node = pointerTree.findNode(query);
                    if (node != nullptr)
                    {
                        found = true;
                        foundId = node->id;
                        printSuccess("Kategori ditemukan.");
                        cout << "  ID      : " << node->id << "\n";
                        cout << "  Nama    : " << node->name << "\n";
                        cout << "  Parent  : " << (node->parent == nullptr || node->parent->id == "ROOT" ? "(root)" : node->parent->id) << "\n";
                        cout << "  Level   : " << node->level << "\n";
                        cout << "  Status  : " << (node->is_active ? "Aktif" : "Non-Aktif") << "\n";
                    }
                }
                if (!found)
                {
                    printError("Kategori dengan ID/Nama '" + query + "' tidak ditemukan.");
                    continue;
                }
                if (confirmPrompt("  Tampilkan subkategori? (y/n): "))
                {
                    printHierarchyUI(state, hashManager, pointerTree, 2, foundId, -1);
                }
            }
            else if (subChoice == 3)
            {
                printMenuTitle("TAMPILKAN HIERARKI");
                cout << "  Mode tampilan:\n";
                cout << "  [1] Seluruh hierarki (semua root)\n";
                cout << "  [2] Dari kategori tertentu (masukkan ID)\n";
                cout << "  [3] Hanya sampai kedalaman tertentu (masukkan depth)\n";
                cout << "  Pilihan: ";
                int viewChoice = getMenuChoice(1, 3);
                if ((viewChoice == 1 || viewChoice == 3) && state.nodeCount > 1000)
                {
                    if (!confirmPrompt("  ! Dataset berisi " + to_string(state.nodeCount) + " node. Output mungkin panjang. Lanjutkan tampilkan ke terminal? (y/n): "))
                        continue;
                }
                if (viewChoice == 1)
                {
                    printHierarchyUI(state, hashManager, pointerTree, 1, "", -1);
                }
                else if (viewChoice == 2)
                {
                    cout << "  Masukkan ID kategori: ";
                    string id;
                    getline(cin, id);
                    id = trim(id);
                    bool exists = hashManager.exists(id) || pointerTree.exists(id);
                    if (!exists)
                    {
                        printError("Kategori '" + id + "' tidak ditemukan.");
                        continue;
                    }
                    printHierarchyUI(state, hashManager, pointerTree, 2, id, -1);
                }
                else if (viewChoice == 3)
                {
                    cout << "  Masukkan kedalaman maksimum: ";
                    int depth;
                    while (!(cin >> depth) || depth < 1)
                    {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "  ✗ Input tidak valid. Masukkan angka >= 1: ";
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    printHierarchyUI(state, hashManager, pointerTree, 1, "", depth);
                }
            }
            else if (subChoice == 4)
            {
                printMenuTitle("HAPUS KATEGORI");
                cout << "  Masukkan ID Kategori yang akan dihapus: ";
                string id;
                getline(cin, id);
                id = trim(id);
                bool exists = hashManager.exists(id) || pointerTree.exists(id);
                if (!exists)
                {
                    printError("Kategori '" + id + "' tidak ditemukan. Penghapusan dibatalkan.");
                    continue;
                }
                size_t impacted = 0;
                bool isRoot = false;
                if (state.activeStructure == StructureType::HashMapTree)
                {
                    impacted = hashManager.countSubtreeNodes(id);
                    auto roots = hashManager.getRootIds();
                    isRoot = find(roots.begin(), roots.end(), id) != roots.end();
                }
                else
                {
                    CategoryNode *node = pointerTree.findNode(id);
                    if (node != nullptr)
                    {
                        impacted = pointerTree.countSubtreeNodes(id);
                        vector<CategoryNode *> roots = pointerTree.getRootNodes();
                        isRoot = find(roots.begin(), roots.end(), node) != roots.end();
                    }
                }
                if (isRoot && state.rootCount == 1)
                {
                    printWarning("Ini adalah satu-satunya root. Tree akan menjadi kosong.");
                }
                cout << "  Target : [" << id << "]\n";
                cout << "  Dampak : " << impacted << " node akan dihapus\n";
                if (!confirmPrompt("  Konfirmasi hapus? (y/n): "))
                    continue;

                vector<string> removedIds;
                if (state.activeStructure == StructureType::HashMapTree)
                {
                    collectSubtreeIdsHashMap(hashManager, id, removedIds);
                    hashManager.deleteCategoryRaw(id);
                    pointerTree.deleteCategory(id);
                }
                else
                {
                    CategoryNode *node = pointerTree.findNode(id);
                    collectSubtreeIdsPointerTree(node, removedIds);
                    pointerTree.deleteCategory(id);
                    hashManager.deleteCategoryRaw(id);
                }
                unordered_set<string> removeSet(removedIds.begin(), removedIds.end());
                removeCategoriesFromDataset(state.dataset, removeSet);
                calculateDatasetStats(state.dataset, state);
                buildActiveStructures(state, hashManager, pointerTree);
                printSuccess("Kategori dan seluruh turunannya dihapus.");
                cout << "  Node dihapus  : " << impacted << "\n";
                cout << "  Node tersisa : " << state.nodeCount << "\n";
            }
        }
        else if (choice == 3)
        {
            benchmarkMenu();
        }
        else if (choice == 4)
        {
            printMenuTitle("DATASET");
            vector<string> datasetItems = {
                "[1] Load Dataset dari File",
                "[2] Informasi Dataset Aktif",
                "[3] Reset / Kosongkan Tree",
                "[0] Kembali"
            };
            printMenuItemsBoxed(datasetItems);
            cout << "  Pilihan: ";
            int dataChoice = getMenuChoice(0, 3);
            if (dataChoice == 1)
            {
                cout << "  Path file (default: ./dataset_kategori.json): ";
                string path;
                getline(cin, path);
                path = trim(path);
                if (path.empty())
                    path = "dataset_kategori.json";
                if (state.datasetLoaded)
                {
                    if (!confirmPrompt("  ! Dataset sebelumnya (" + to_string(state.nodeCount) + " node) akan dihapus. Lanjutkan? (y/n): "))
                        continue;
                }
                DatasetLoadResult result = loadDatasetFromJsonFile(path);
                if (!result.success)
                {
                    printError(result.errorMessage + "\n");
                    continue;
                }
                state.dataset = move(result.categories);
                state.activeFile = path;
                state.datasetLoaded = true;
                state.loadTimeMs = result.loadTimeMs;
                calculateDatasetStats(state.dataset, state);
                buildActiveStructures(state, hashManager, pointerTree);
                printSuccess("Dataset berhasil dimuat. " + to_string(state.nodeCount) + " node aktif.");
                if (!result.orphanIds.empty())
                {
                    printWarning("Terdapat " + to_string(result.orphanIds.size()) + " orphan node. Orphan node dilewati dan tidak dimuat.");
                }
            }
            else if (dataChoice == 2)
            {
                if (!state.datasetLoaded)
                {
                    printWarning("Tidak ada dataset aktif.");
                    continue;
                }
                printMenuTitle("INFO DATASET");
                cout << "  File sumber      : " << state.activeFile << "\n";
                cout << "  Total node       : " << state.nodeCount << "\n";
                cout << "  Node root        : " << state.rootCount << "\n";
                cout << "  Kedalaman max    : " << state.maxDepth << " level\n";
                cout << "  Node terdalam    : [" << state.deepestNodeId << "] \"" << state.deepestNodeName << "\"\n";
                cout << "  Node terluas     : [" << state.widestNodeId << "] (" << state.widestNodeSize << " node)\n";
                cout << "  Struktur aktif   : " << kStructureNames[static_cast<int>(state.activeStructure)] << "\n";
                cout << "  Memori digunakan : ~" << estimateMemoryUsageKB(state.nodeCount) << " KB\n";
            }
            else if (dataChoice == 3)
            {
                if (!state.datasetLoaded)
                {
                    printWarning("Tidak ada dataset untuk direset.");
                    continue;
                }
                if (!confirmPrompt("  ! Reset akan menghapus semua node dari memori. Lanjutkan? (y/n): "))
                    continue;
                state.dataset.clear();
                state.datasetLoaded = false;
                state.nodeCount = 0;
                state.rootCount = 0;
                state.maxDepth = 0;
                state.activeFile.clear();
                state.loadTimeMs = 0.0;
                hashManager.loadDataFromVector({});
                pointerTree.loadDataFromVector({});
                printSuccess("Dataset berhasil direset. Tree kosong.");
            }
        }
        else if (choice == 0)
        {
            running = false;
            printSuccess("Program selesai.");
        }
    }

    return 0;
}