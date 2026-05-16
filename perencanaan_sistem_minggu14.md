# Perencanaan Sistem Minggu ke-14
## Sistem Manajemen Kategori & Hirarki Data
**Kelompok 10 — Struktur Data P4, IPB University 2026**

---

## 1. Ringkasan Status Saat Ini (Minggu ke-7)

Kelompok telah berhasil mengimplementasikan **Struktur Data 1: N-ary Tree berbasis Hash Map** (`unordered_map` + Adjacency List vector) dengan tiga operasi utama:

| Operasi | Kompleksitas | Catatan |
|---|---|---|
| Insert | O(1) rata-rata | Menggunakan hash ke `categoryMap` dan `nameToIdMap` |
| Search | O(1) rata-rata | Resolusi nama via `nameToIdMap` |
| Traversal (DFS) | O(K) | K = jumlah node turunan |
| Cascading Delete | O(K) | Rekursif ke seluruh subtree |

**Bottleneck yang ditemukan:** Operasi Traversal dan Delete mengalami pembengkakan waktu signifikan (33ms → 358ms untuk 1K–10K data) akibat overhead I/O (`cout`) dan penghapusan rekursif memori.

---

## 2. Target Minggu ke-14 (Final)

Berdasarkan rubrik proyek dan hasil evaluasi sementara, target akhir adalah:

- ✅ Implementasi **2 struktur data** yang berbeda secara arsitektur
- ✅ **Grafik perbandingan** waktu eksekusi & penggunaan memori
- ✅ **Demo aplikasi** CLI sistem manajemen kategori & hirarki data
- ✅ **Laporan analisis** performa & rekomendasi struktur data

---

## 3. Kandidat Struktur Data untuk Implementasi Kedua

Berdasarkan materi praktikum (Praktikum 1–12) dan kebutuhan sistem hierarki kategori, berikut evaluasi tiap kandidat:

---

### 3.1 N-ary Tree Berbasis Pointer (Linked List Rekursif) ⭐ *Rekomendasi Utama*

**Deskripsi:** Setiap node `Category` memiliki pointer ke anak pertama (`firstChild`) dan saudara berikutnya (`nextSibling`) — pola *Left-Child Right-Sibling* (LCRS). Alternatif lain: pointer langsung ke `vector<CategoryNode*> children`.

```cpp
struct CategoryNode {
    string id, name, parentId;
    int level;
    bool isActive;
    CategoryNode* firstChild;   // anak pertama
    CategoryNode* nextSibling;  // saudara kanan
    CategoryNode* parent;       // pointer ke induk
};
```

**Kelebihan untuk konteks ini:**
- Delete node beserta subtree-nya cukup bebaskan memori langsung via pointer — tidak perlu iterasi `child_ids` vector lalu lookup ke HashMap
- Traversal DFS alami dan hemat memori (tidak perlu menyimpan ID string tambahan)
- Mencerminkan struktur hierarki secara eksplisit di memori

**Kekurangan:**
- Search berdasarkan ID/nama memerlukan DFS O(n) — kecuali dikombinasikan dengan indeks sekunder
- Lebih kompleks secara implementasi dibanding HashMap

**Relevansi ke praktikum:** Materi Praktikum 3 (Linked List), Praktikum 10–11 (Tree, BST), konsep pointer dari Praktikum 1

---

### 3.2 Binary Search Tree (BST) ❌ *Kurang Sesuai*

BST mengasumsikan setiap node memiliki maksimal 2 anak dan data terurut secara kunci. Struktur kategori e-commerce bersifat N-ary (satu kategori bisa punya banyak subkategori) sehingga BST tidak merepresentasikan hierarki secara natural.

---

### 3.3 Hash Map + Adjacency List (sudah ada) vs. Pointer Tree — Perbandingan yang Tepat

Ini adalah perbandingan yang paling bermakna secara akademis karena:
- Keduanya merepresentasikan N-ary Tree, namun dengan pendekatan penyimpanan memori yang berbeda
- Hash Map unggul di Search (O(1)), Pointer Tree unggul di Delete struktural
- Perbedaan ini dapat diukur secara kuantitatif dengan stress test yang sama

---

### 3.4 B-Tree ❌ *Terlalu Kompleks untuk Scope Proyek*

B-Tree (Praktikum 12) cocok untuk penyimpanan berbasis disk dan database berskala besar. Untuk jumlah kategori 10.000 node dalam memori, overhead implementasi B-Tree tidak sebanding dengan manfaatnya. Di luar scope proyek ini.

---

### 3.5 Hash Table dengan Separate Chaining / Linear Probing ❌ *Sudah Tercakup*

Praktikum 7 membahas ini, namun `unordered_map` yang sudah digunakan pada Struktur Data 1 pada dasarnya sudah mengimplementasikan hash table. Bukan pilihan yang menghasilkan perbandingan yang berarti.

---

## 4. Keputusan Arsitektur Final

| | Struktur Data 1 (Sudah ada) | Struktur Data 2 (Dikembangkan) |
|---|---|---|
| **Nama** | N-ary Tree via Hash Map | N-ary Tree via Pointer (LCRS) |
| **Penyimpanan node** | `unordered_map<string, Category>` | `CategoryNode*` (heap, `new`/`delete`) |
| **Relasi anak** | `vector<string> child_ids` (Adjacency List) | `CategoryNode* firstChild` + `nextSibling` |
| **Akses by ID** | O(1) via hash map | O(n) DFS — ditambah indeks sekunder opsional |
| **Traversal** | O(K) DFS rekursif via child_ids lookup | O(K) DFS rekursif via pointer langsung |
| **Delete** | O(K) + overhead lookup hash per node | O(K) pointer langsung, dealokasi memori eksplisit |
| **Memori** | Overhead string ID duplikat di vector & map | Overhead pointer (8 byte/pointer), lebih efisien |

---

## 5. Rencana Implementasi Detail

### 5.1 Struktur File Program

```
project/
├── main.cpp                  # Entry point, menu CLI
├── hashmap/
│   ├── CategoryHashMap.h     # Definisi struct Category + class
│   └── CategoryHashMap.cpp   # Implementasi Insert/Search/Delete/Traversal
├── pointertree/
│   ├── CategoryTree.h        # Definisi struct CategoryNode + class
│   └── CategoryTree.cpp      # Implementasi Insert/Search/Delete/Traversal
├── benchmark/
│   └── Benchmark.h           # Utilitas pengukuran waktu & memori
├── dataset/
│   └── dataset_kategori.json # Dataset yang sama untuk kedua struktur
└── README.md
```

---

### 5.2 Implementasi Struktur Data 2: Pointer Tree

#### Struct Node

```cpp
struct CategoryNode {
    string id;
    string name;
    string parentId;
    int level;
    bool isActive;
    CategoryNode* firstChild;   // anak pertama (linked list horizontal)
    CategoryNode* nextSibling;  // saudara berikutnya
    CategoryNode* parent;       // pointer ke induk (untuk delete dari parent)

    CategoryNode(string id, string name, string parentId, int level)
        : id(id), name(name), parentId(parentId), level(level),
          isActive(true), firstChild(nullptr), nextSibling(nullptr), parent(nullptr) {}
};
```

#### Class CategoryPointerTree

```cpp
class CategoryPointerTree {
private:
    CategoryNode* root;
    // Indeks sekunder untuk Search O(1) — boleh digunakan opsional
    unordered_map<string, CategoryNode*> idIndex;
    unordered_map<string, CategoryNode*> nameIndex;

public:
    void insert(string id, string name, string parentId);
    CategoryNode* searchById(string id);
    CategoryNode* searchByName(string name);
    void traverse(CategoryNode* node, int depth);   // DFS rekursif
    void cascadeDelete(CategoryNode* node);          // DFS delete + free memori
    void printHierarchy(string rootId);
};
```

#### Operasi Insert

```
1. Buat node baru: CategoryNode* newNode = new CategoryNode(...)
2. Jika parentId kosong → jadikan root
3. Cari parent node via idIndex (O(1)) atau DFS (O(n))
4. Sisipkan newNode ke linked list anak dari parent:
   - Jika parent->firstChild == nullptr: parent->firstChild = newNode
   - Jika tidak: traverse sibling list, tempel di akhir
5. Set newNode->parent = parentNode
6. Update idIndex dan nameIndex
```

#### Operasi Cascading Delete

```
deleteSubtree(node):
  for each child c of node (via sibling list):
      deleteSubtree(c)   // rekursif hapus semua keturunan
  idIndex.erase(node->id)
  nameIndex.erase(node->name)
  delete node            // bebaskan memori heap

delete(targetId):
  node = searchById(targetId)
  if node->parent:
      lepaskan node dari sibling list parent-nya
  deleteSubtree(node)
```

**Hipotesis:** Karena tidak ada overhead lookup HashMap per node saat delete (langsung traversal pointer), operasi delete pada dataset 10.000 node seharusnya lebih cepat dibanding Struktur Data 1.

---

### 5.3 Perbaikan Struktur Data 1 (Mitigasi Bottleneck)

Sebelum final, lakukan perbaikan berikut pada implementasi HashMap agar perbandingan lebih adil (menghapus overhead I/O buatan):

- Pisahkan fungsi **traversal tanpa print** (untuk benchmarking murni) dan **traversal dengan print** (untuk demo)
- Gunakan `std::ostringstream` buffer, cetak sekali di akhir — bukan `cout` per iterasi
- Pastikan pengukuran waktu (`chrono`) hanya mengukur operasi struktur data, bukan I/O

---

## 6. Rencana Eksperimen & Benchmarking

### 6.1 Dataset

Dataset yang digunakan **sama persis** untuk kedua struktur, di-load dari `dataset_kategori.json`:

| Ukuran | Keterangan |
|---|---|
| 1.000 node | Dataset kecil |
| 5.000 node | Dataset menengah |
| 10.000 node | Dataset besar (stress test) |

### 6.2 Metrik yang Diukur

Untuk setiap kombinasi (ukuran dataset × struktur data × operasi):

```cpp
auto start = chrono::high_resolution_clock::now();
// --- jalankan operasi tanpa I/O print ---
auto end = chrono::high_resolution_clock::now();
double ms = chrono::duration<double, milli>(end - start).count();
```

| Metrik | Cara Ukur |
|---|---|
| Waktu Insert (ms) | Insert semua node dari dataset |
| Waktu Search (ms) | Search 100 node random, ambil rata-rata |
| Waktu Traversal (ms) | DFS seluruh tree tanpa print |
| Waktu Cascading Delete (ms) | Delete root kategori dengan subtree terbesar |
| Penggunaan Memori (KB) | Selisih RSS proses sebelum dan sesudah load (`/proc/self/status` di Linux / `getrusage`) |

### 6.3 Tabel Hasil yang Diharapkan

```
Operasi       | 1K HashMap | 1K Pointer | 5K HashMap | 5K Pointer | 10K HashMap | 10K Pointer
Insert        |            |            |            |            |             |
Search        |            |            |            |            |             |
Traversal     |            |            |            |            |             |
Delete        |            |            |            |            |             |
Memori (KB)   |            |            |            |            |             |
```

### 6.4 Visualisasi

Buat grafik perbandingan menggunakan salah satu pendekatan:
- **Python (matplotlib):** plot data dari hasil CSV benchmark
- **Gnuplot:** langsung dari terminal
- **Manual:** tabel dan grafik di laporan PDF

---

## 7. Jadwal Kerja Minggu ke-8 hingga ke-14

| Minggu | Target | Penanggung Jawab (Saran) |
|---|---|---|
| 8–9 | Implementasi `CategoryPointerTree` (Insert, Search, DFS Traversal) | Aditya + Zivanka |
| 10 | Implementasi Cascading Delete pada Pointer Tree | Anisa |
| 11 | Integrasi ke `main.cpp`, menu CLI berjalan untuk kedua struktur | Nazwa |
| 12 | Perbaikan bottleneck I/O pada HashMap; jalankan benchmark 3 ukuran dataset | Semua |
| 13 | Analisis hasil, pembuatan grafik perbandingan, penulisan laporan | Semua |
| 14 | Finalisasi laporan, demo CLI, presentasi | Semua |

---

## 8. Analisis Teoritis yang Perlu Ditulis di Laporan

### 8.1 Perbandingan Kompleksitas

| Operasi | HashMap Tree | Pointer Tree (tanpa indeks) | Pointer Tree (dengan idIndex) |
|---|---|---|---|
| Insert | O(1) amortized | O(n) cari posisi parent + O(sibling) | O(1) cari parent + O(sibling) |
| Search by ID | O(1) | O(n) DFS | O(1) |
| Traversal | O(K) | O(K) | O(K) |
| Delete (cascade) | O(K) × O(1) hash lookup | O(K) pointer langsung | O(K) pointer langsung |
| Memori per node | Tinggi (string duplikat di map + vector) | Rendah (4 pointer + data) | Moderat (+indeks map) |

### 8.2 Analisis Trade-off

**HashMap Tree unggul ketika:**
- Akses random by ID/nama sangat sering (O(1) guaranteed)
- Dataset relatif kecil (<5.000 node) sehingga overhead memori tidak kritis
- Tim tidak familiar dengan manajemen pointer manual

**Pointer Tree unggul ketika:**
- Operasi delete dan restrukturisasi hierarki sangat sering dilakukan
- Efisiensi memori menjadi prioritas (tidak ada duplikasi ID string)
- Traversal struktural (parent→child→sibling) mendominasi use case

**Rekomendasi untuk sistem e-commerce:**
> Gunakan pendekatan **hibrid**: Pointer Tree sebagai struktur utama untuk menyimpan dan menelusuri hierarki, dikombinasikan dengan `unordered_map<string, CategoryNode*>` sebagai indeks sekunder untuk pencarian O(1) by ID/nama. Ini adalah pola yang digunakan pada database engine nyata.

---

## 9. Checklist Final Sebelum Presentasi Minggu ke-14

- [ ] Kedua struktur data berjalan tanpa error pada dataset 10.000 node
- [ ] Menu CLI menampilkan pilihan struktur data yang digunakan
- [ ] Benchmark berjalan otomatis dan menghasilkan tabel output
- [ ] Grafik perbandingan tersedia (gambar/PDF/terminal)
- [ ] Laporan analisis mencakup: latar belakang, implementasi, hasil eksperimen, analisis, kesimpulan
- [ ] Repository GitHub diperbarui dengan kode final
- [ ] Demo siap: Insert → Search → Traversal → Delete pada skenario nyata
- [ ] Kode sudah dibersihkan (tidak ada debug `cout` yang tidak perlu)

---

## 10. Referensi dari Materi Praktikum

| Materi | Relevansi |
|---|---|
| Praktikum 1 – Pointer, Struct, STL | Dasar implementasi struct node dan pointer manajemen memori |
| Praktikum 2 – Array & Vector | Dasar penggunaan `vector<string>` sebagai Adjacency List (Struktur Data 1) |
| Praktikum 3 – Linked List | Pola traversal sibling list pada Pointer Tree (LCRS) |
| Praktikum 7 – Hash Table | Dasar `unordered_map` (Separate Chaining) yang dipakai HashMap Tree |
| Praktikum 8–9 – Graf & DFS | Algoritma DFS untuk Traversal & Cascading Delete |
| Praktikum 10–11 – Tree & BST | Konsep node pointer, manajemen `new`/`delete`, traversal rekursif |

---

*Dokumen ini bersifat panduan perencanaan dan dapat diperbarui seiring progres implementasi.*
