# Sistem Manajemen Kategori & Hirarki Data E-Commerce

Sistem manajemen kategori dan subkategori data e-commerce yang membentuk struktur hierarki non-linear (N-ary Tree). Dikembangkan menggunakan C++ sebagai pemenuhan tugas mata kuliah Struktur Data.

Sistem ini dirancang untuk menguji, memanipulasi, dan membandingkan performa dua pendekatan struktur data yang berbeda dalam menangani data bertingkat berskala besar (hingga 10.000 node).

---

## 🧠 Struktur Data yang Digunakan

Sistem ini mengimplementasikan dua pendekatan untuk merepresentasikan N-ary Tree:

### 1. **Hash Map + Adjacency List** (HashMap Tree)
Pendekatan berbasis pemetaan hash untuk indexing cepat.

**Struktur:**
```cpp
struct Category {
    string id;              // ID unik kategori
    string name;            // Nama kategori
    string parent_id;       // ID induk (parent)
    int level;              // Kedalaman di tree (root = 0)
    bool is_active;         // Status kategori
    vector<string> child_ids;  // Daftar ID anak
};

unordered_map<string, Category> categoryMap;      // Mapping ID → Category
unordered_map<string, string> nameToIdMap;        // Mapping Nama → ID
```

**Kompleksitas:**
| Operasi | Waktu | Ruang |
|---------|-------|-------|
| Insert | O(1) | O(1) |
| Search (by ID) | O(1) | - |
| Search (by Name) | O(1) | - |
| Delete | O(K) | - |
| Traversal | O(n) | O(h) |

*K = jumlah node yang dihapus (subtree), n = total node, h = kedalaman*

---

### 2. **Pointer Tree dengan LCRS** (Pointer Tree)
Pendekatan berbasis pointer dengan pola Left-Child Right-Sibling (LCRS).

**Struktur:**
```cpp
struct CategoryNode {
    string id;                  // ID unik
    string name;                // Nama kategori
    string parent_id;           // ID induk
    int level;                  // Kedalaman
    bool is_active;             // Status
    
    CategoryNode* firstChild;   // Pointer ke anak pertama
    CategoryNode* nextSibling;  // Pointer ke sibling berikutnya
    CategoryNode* parent;       // Pointer ke parent
};

CategoryNode* root;                          // Root node
unordered_map<string, CategoryNode*> idIndex;        // Mapping ID → Node
unordered_map<string, CategoryNode*> nameIndex;      // Mapping Nama → Node
```

**Kompleksitas:**
| Operasi | Waktu | Ruang |
|---------|-------|-------|
| Insert | O(1)* | O(1) |
| Search (by ID) | O(1)** | - |
| Search (by Name) | O(1)** | - |
| Delete | O(K) | - |
| Traversal | O(n) | O(h) |

*\* dengan index sekunder, \*\* dengan index*

---

## 🎯 Alasan Menggunakan Struktur Data Ini

### **Hash Map + Adjacency List**
**Keuntungan:**
- Akses node sangat cepat O(1) menggunakan ID
- Implementasi sederhana dan mudah dipahami
- Operasi traversal efisien menggunakan adjacency list
- Memory footprint lebih kecil dibanding pointer tree
- Cocok untuk operasi berbasis ID

**Kekurangan:**
- Kurang intuitif untuk operasi tree traversal manual
- Overhead hashing untuk setiap akses
- Lebih kompleks untuk operasi struktur (re-parenting, dll)

### **Pointer Tree (LCRS)**
**Keuntungan:**
- Representasi tree yang natural dan intuitif
- Dukungan penuh untuk operasi tree klasik (pre-order, in-order, post-order)
- Memory locality lebih baik untuk traversal depth-first
- Mudah untuk operasi seperti re-parenting
- Fleksibel untuk modifikasi struktur

**Kekurangan:**
- Overhead pointer (lebih banyak memory untuk setiap node)
- Traversal manual lebih kompleks dibanding tree implisit
- Garbage collection kompleks saat penghapusan
- Perlu index sekunder untuk pencarian O(1)

### **Pilihan untuk Proyek Ini:**
- **HashMap Tree:** Optimal untuk sistem yang membutuhkan akses cepat berbasis ID dan operasi pencarian
- **Pointer Tree:** Optimal untuk sistem yang membutuhkan operasi tree kompleks dan visualisasi hierarki

---

## 📊 Alur Sistem dan Contoh Penggunaan

### **Alur Umum Aplikasi**
```
┌─────────────────────┐
│   Buka Aplikasi     │
└────────┬────────────┘
         │
    ┌────▼─────┐
    │  Pilih   │──► 1: Pilih Struktur Data (HashMap / Pointer)
    │  Menu    │──► 2: Manajemen Kategori (Insert, Search, Delete, View)
    │  Utama   │──► 3: Benchmark & Perbandingan
    │          │──► 4: Load Dataset
    │          │──► 0: Keluar
    └────┬─────┘
         │
    ┌────▼──────────────────┐
    │  Eksekusi Operasi     │
    │  (Insert/Search/etc)  │
    └────┬──────────────────┘
         │
    ┌────▼──────────────────┐
    │  Simpan ke Disk       │
    │  (dataset_kategori.json)
    └────┬──────────────────┘
         │
    ┌────▼──────────┐
    │  Kembali Menu │
    │  atau Keluar  │
    └───────────────┘
```

### **Contoh 1: Insert Kategori**
```
Input:
  ID: ELEC
  Nama: Elektronik
  Parent: (kosong)
  
Hasil (HashMap Tree):
  categoryMap["ELEC"] = {id: "ELEC", name: "Elektronik", parent_id: "", level: 0, child_ids: []}
  nameToIdMap["Elektronik"] = "ELEC"
  
Waktu: O(1)

Input:
  ID: PHONE
  Nama: Ponsel
  Parent: ELEC
  
Hasil (HashMap Tree):
  categoryMap["PHONE"] = {id: "PHONE", name: "Ponsel", parent_id: "ELEC", level: 1, child_ids: []}
  categoryMap["ELEC"].child_ids = ["PHONE"]
  nameToIdMap["Ponsel"] = "PHONE"
  
Waktu: O(1)
```

### **Contoh 2: Struktur Tree yang Terbentuk**
```
Dataset:
  - ELEC (Elektronik) - root
    - PHONE (Ponsel)
      - SMARTPHONE (Smartphone)
      - FEATURE (Feature Phone)
    - COMP (Komputer)
      - LAPTOP (Laptop)
      - DESKTOP (Desktop)

Visualisasi Tree:
|-- [ELEC] Elektronik
    |-- [PHONE] Ponsel
        |-- [SMARTPHONE] Smartphone
        |-- [FEATURE] Feature Phone
    |-- [COMP] Komputer
        |-- [LAPTOP] Laptop
        |-- [DESKTOP] Desktop
```

### **Contoh 3: Delete Cascading**
```
Input: Hapus kategori "PHONE"

Operasi:
1. Cari semua anak dari PHONE (recursive)
   - Ditemukan: SMARTPHONE, FEATURE
2. Hapus semua anak
   - Hapus SMARTPHONE
   - Hapus FEATURE
3. Hapus PHONE dari parent (ELEC)
   - categoryMap["ELEC"].child_ids.erase("PHONE")
4. Hapus PHONE dari map
   - categoryMap.erase("PHONE")
   - nameToIdMap.erase("Ponsel")

Hasil:
|-- [ELEC] Elektronik
    |-- [COMP] Komputer
        |-- [LAPTOP] Laptop
        |-- [DESKTOP] Desktop

Waktu: O(K) dimana K = jumlah node yang dihapus
```

### **Contoh 4: Search**
```
Input: Cari "PHONE"

Hasil (HashMap Tree):
  Waktu: O(1) - Langsung akses categoryMap["PHONE"]
  
Input: Cari "Ponsel"

Hasil (HashMap Tree):
  Waktu: O(1) - Langsung akses nameToIdMap["Ponsel"] → dapatkan ID → akses categoryMap

Output:
  ID: PHONE
  Nama: Ponsel
  Parent: ELEC
  Level: 1
  Jumlah Anak: 2
```

---

## 🏗️ Struktur Kode yang Sudah Dibuat

### **File Utama:**

#### **1. `data_structures.h`**
Header file yang mendefinisikan:
- Struct `Category`: Representasi node kategori untuk HashMap Tree
- Struct `CategoryNode`: Representasi node untuk Pointer Tree
- Struct `AppState`: Status aplikasi (struktur aktif, dataset, statistik)
- Struct `BenchmarkResult`: Hasil benchmark operasi
- Class `CategoryMapManager`: Manager untuk HashMap Tree
- Class `CategoryPointerTree`: Manager untuk Pointer Tree

#### **2. `data_structures.cpp`**
Implementasi operasi inti:
- **CategoryMapManager:**
  - `insertCategory()`: Insert node kategori
  - `searchCategory()`: Cari kategori by ID/Name
  - `deleteCategory()`: Hapus kategori (cascading)
  - `loadData()` / `saveData()`: Persistensi JSON
  - `printTree()`: Visualisasi tree (DFS)
  - `deleteRecursive()`: Helper untuk cascading delete
  - `countSubtreeNodes()`: Hitung jumlah subtree

- **CategoryPointerTree:**
  - `loadDataFromVector()`: Load data dari vector
  - `searchById()` / `searchByName()`: Cari node
  - `printTree()`: Visualisasi tree
  - `clearSubtree()`: Hapus subtree recursively
  - Index management untuk O(1) search

#### **3. `ui.h` dan `ui.cpp`**
Antarmuka pengguna:
- `printHeader()`: Header dengan informasi status
- `printMenuTitle()` / `printMenuItems()`: Menu display
- `printHashMapTreeUI()`: Visualisasi HashMap Tree
- `printPointerTreeUI()`: Visualisasi Pointer Tree
- `getMenuChoice()`: Input menu dengan validasi
- `confirmPrompt()`: Konfirmasi aksi
- `printBenchmarkSummary()`: Hasil benchmark
- Helper functions: `trim()`, `truncateText()`, `supportsUtf8()`, dll

#### **4. `main.cpp`**
Logika utama aplikasi:
- Main loop dengan menu interaktif
- Menu 1: Pilih struktur data (HashMap/Pointer)
- Menu 2: Manajemen kategori (Insert, Search, Delete, View)
- Menu 3: Benchmark & perbandingan dua struktur
- Menu 4: Load dataset dari JSON
- Event handling dan validasi input

#### **5. `json.hpp`**
Library nlohmann/json untuk parsing JSON file (header-only).

#### **6. Dataset Files:**
- `dataset_kategori.json`: Database lokal kategori (auto-generated)
- `dataset_1000.json`: Test dataset 1000 node
- `dataset_5000.json`: Test dataset 5000 node
- `dataset_10000.json`: Test dataset 10000 node

#### **7. Output:**
- `benchmark_result.csv`: Hasil benchmark dalam format CSV

---

### **Fitur yang Sudah Diimplementasikan:**

**Core Operations:**
- Insert kategori (root & child)
- Search kategori (by ID & Name)
- Delete kategori (cascading)
- Traversal & visualisasi tree
- Persistent storage (JSON)

**Struktur Data:**
- HashMap + Adjacency List implementation
- Pointer Tree (LCRS) implementation
- Dual structure support dengan switching

**Benchmark:**
- High-precision timer (`<chrono>`)
- Operasi diukur: Insert, Search, Traversal, Delete
- Perbandingan dua struktur data
- CSV export untuk analisis

**User Interface:**
- Menu interaktif dengan validasi
- Visualisasi tree dengan indentation
- Status display (active structure, node count, load time)
- Pesan error/warning/success yang jelas
- Support UTF-8 dan color output

---

## Prasyarat & Cara Kompilasi

### **Prasyarat:**
1. Compiler C++11 atau lebih baru (GCC, Clang, MSVC)
2. Library nlohmann/json (file `json.hpp` sudah included)

### **Kompilasi:**
```bash
g++ -O2 -std=c++11 -o main main.cpp data_structures.cpp ui.cpp
// atau
make clean && make
```

### **Menjalankan:**
```bash
./main              # Linux/macOS
main.exe            # Windows
```

---

## Dataset yang Tersedia

| File | Jumlah Node | Ukuran (KB) | Kedalaman | Penggunaan |
|------|-------------|------------|-----------|-----------|
| dataset_kategori.json | Dinamis | Bervariasi | Bervariasi | Database lokal |
| dataset_1000.json | 1.000 | ~50 | ~8 | Testing dasar |
| dataset_5000.json | 5.000 | ~250 | ~10 | Testing menengah |
| dataset_10000.json | 10.000 | ~500 | ~12 | Testing performa |

---

## 👥 Tim Pengembang

**Kelompok 10 - Struktur Data P4**  
Departemen Ilmu Komputer  
Sekolah Sains Data, Matematika, dan Informatika  
IPB University - 2026

- **Aditya Cahyo Nugroho** (M0403241109)
- **Zivanka Aurellia Astadewi Maheswari** (M0403241111)
- **Anisa Nur Rohmah** (M0403241035)
- **Nazwa Nadya Rahma** (M0403241060)  