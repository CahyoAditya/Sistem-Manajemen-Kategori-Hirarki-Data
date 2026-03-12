#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

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
        auto start_insert = chrono::high_resolution_clock::now();   // Hanya timer, nothing special

        // Base case untuk ID sudah ada
        if (categoryMap.count(id) > 0)
        {
            cout << "Error: ID Kategori sudah ada!\n";
            return;
        }

        int current_level = 1;

        if (parent_id != "")
        {
            // Base case masuk ke Parent
            if (categoryMap.count(parent_id) > 0)
            {
                categoryMap[parent_id].child_ids.push_back(id);
                current_level = categoryMap[parent_id].level + 1;
            }
            else
            {
                cout << "Error: Parent ID tidak ditemukan!\n";
                return;
            }
        }

        Category newCat = {id, name, parent_id, current_level, true, {}};
        categoryMap[id] = newCat;
        nameToIdMap[name] = id;

        auto end_insert = chrono::high_resolution_clock::now();     // Matiin timer, nothing special tbh
        chrono::duration<double, std::milli> duration_insert = end_insert - start_insert;

        cout << "Sukses: Kategori ditambahkan.\n";
        cout << "[METRIK] Waktu Insert memori: " << duration_insert.count() << " ms\n";

        saveData();
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
};

int main()
{
    CategoryMapManager manager;
    manager.loadData();

    int choice;
    do
    {
        cout << "\n==============================================\n";
        cout << "1. Tambah Kategori\n";
        cout << "2. Cari & Tampilkan (Langsung Enter utk Semua)\n";
        cout << "3. Hapus Kategori\n";
        cout << "0. Keluar\n";
        cout << "Pilih menu: ";

        cin >> choice;

        // Validasi agar terminal tidak looping tak terbatas jika diinput huruf
        if (cin.fail())
        {
            cin.clear();
            choice = -1;
        }
        cin.ignore(10000, '\n');

        string id, name, parent_id, query;

        if (choice == 1)
        {
            cout << "\n--- Tambah Kategori ---\n";
            cout << "ID Kategori : ";
            getline(cin, id);
            cout << "Nama Kategori: ";
            getline(cin, name);
            cout << "ID Parent (Kosongkan jika ini Root): ";
            getline(cin, parent_id);
            manager.insertCategory(id, name, parent_id);
        }
        else if (choice == 2)
        {
            cout << "\n--- Cari / Tampilkan ---\n";
            cout << "Masukkan ID/Nama (Atau langsung tekan Enter): ";
            getline(cin, query);
            manager.searchCategory(query);
        }
        else if (choice == 3)
        {
            cout << "\n--- Hapus Kategori ---\n";
            cout << "WARNING: Menghapus kategori akan menghapus SEMUA produk/kategori di bawahnya!\n";
            cout << "Masukkan ID Kategori: ";
            getline(cin, query);
            manager.deleteCategory(query);
        }
        else if (choice == 0)
        {
            cout << "Program selesai.\n";
        }
        else
        {
            cout << "Pilihan tidak valid, silakan coba lagi.\n";
        }

    } while (choice != 0);

    return 0;
}