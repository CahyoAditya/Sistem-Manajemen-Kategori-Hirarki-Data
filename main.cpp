#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include "ui.h"

using namespace std;

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
        cout << border << "  [4] Dataset" << repeatChar(' ', 49) << border << "\n";
        cout << border << "  [0] Keluar" << repeatChar(' ', 50) << border << "\n";
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
                    hashManager.saveData();
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
                hashManager.saveData();
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
