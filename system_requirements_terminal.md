# System Requirements: Terminal Interface
## Sistem Manajemen Kategori & Hirarki Data
**Kelompok 10 — Struktur Data P4, IPB University 2026**

---

## 1. Gambaran Umum Antarmuka

Aplikasi berjalan sebagai **program CLI interaktif berbasis menu** yang dikompilasi dengan G++ dan dijalankan langsung di terminal. Antarmuka dirancang untuk mendukung dua mode: **mode operasional** (manajemen kategori) dan **mode benchmark** (perbandingan performa struktur data).

```
╔══════════════════════════════════════════════════════════════╗
║       SISTEM MANAJEMEN KATEGORI & HIRARKI DATA              ║
║       Struktur Data P4 — Kelompok 10 — IPB University       ║
╠══════════════════════════════════════════════════════════════╣
║  Struktur Aktif : [1] HashMap Tree  [2] Pointer Tree        ║
║  Dataset        : dataset_kategori.json (10.000 node)       ║
║  Status         : Loaded ✓   Waktu load: 42.31 ms           ║
╚══════════════════════════════════════════════════════════════╝
```

---

## 2. Struktur Menu Utama

### 2.1 Hierarki Menu

```
MAIN MENU
├── [1] Pilih Struktur Data
│     ├── [1] HashMap Tree (unordered_map + Adjacency List)
│     └── [2] Pointer Tree (N-ary Linked List / LCRS)
│
├── [2] Manajemen Kategori          ← Operasi CRUD
│     ├── [1] Tambah Kategori
│     ├── [2] Cari Kategori
│     ├── [3] Tampilkan Hierarki
│     └── [4] Hapus Kategori
│
├── [3] Benchmark & Perbandingan    ← Stress test otomatis
│     ├── [1] Benchmark Struktur Aktif
│     ├── [2] Benchmark Kedua Struktur (side-by-side)
│     └── [3] Ekspor Hasil ke CSV
│
├── [4] Dataset
│     ├── [1] Load Dataset dari File
│     ├── [2] Informasi Dataset Aktif
│     └── [3] Reset / Kosongkan Tree
│
└── [0] Keluar
```

---

## 3. Spesifikasi Layar per Menu

### 3.1 Header Global (selalu tampil di atas)

Header ditampilkan ulang setiap kali menu di-render agar pengguna selalu tahu konteks aktif.

```
================================================================
  SISTEM MANAJEMEN KATEGORI & HIRARKI DATA
================================================================
  Struktur Aktif : HashMap Tree
  Node Termuat   : 10.000
  Waktu Load     : 42.31 ms
================================================================
```

**Requirement:**
- Lebar baris konsisten: **64 karakter**
- Karakter border: `=`, `║`, `╔`, `╗`, `╚`, `╝`, `╠`, `╣` (ASCII extended, atau fallback ke `=` dan `|` jika terminal tidak mendukung UTF-8)
- Struktur aktif diperbarui real-time saat pengguna berpindah pilihan

---

### 3.2 Menu [1] — Pilih Struktur Data

```
================================================================
  [PILIH STRUKTUR DATA]
================================================================

  Pilih struktur data yang akan digunakan:

  [1] HashMap Tree
      Deskripsi : N-ary Tree berbasis unordered_map + vector
      Search    : O(1) | Insert : O(1) | Delete : O(K)
      Cocok untuk: akses acak cepat berdasarkan ID/nama

  [2] Pointer Tree (LCRS)
      Deskripsi : N-ary Tree berbasis pointer rekursif
      Search    : O(n) / O(1)* | Insert : O(1)* | Delete : O(K)
      Cocok untuk: traversal hierarki & delete struktural
      *) dengan indeks sekunder

  Pilihan saat ini: [1] HashMap Tree

  Masukkan pilihan (1/2): _
```

**Requirement:**
- Menampilkan deskripsi singkat dan kompleksitas masing-masing struktur
- Jika dataset sudah ter-load, konfirmasi apakah data akan di-reload ke struktur baru:
  ```
  ! Dataset aktif akan di-load ulang ke struktur baru.
    Lanjutkan? (y/n): _
  ```

---

### 3.3 Menu [2.1] — Tambah Kategori

```
================================================================
  [TAMBAH KATEGORI]  Struktur: HashMap Tree | Node: 10.000
================================================================

  ID Kategori       : _
  Nama Kategori     : _
  ID Induk (Parent) : _              [Enter = jadikan Root]
  Status            : [1] Aktif  [2] Non-Aktif   (default: 1)

----------------------------------------------------------------
  Tekan Enter untuk konfirmasi, ESC untuk batal
================================================================
```

**Setelah submit — output sukses:**
```
  ✓ Sukses! Kategori ditambahkan.
  ─────────────────────────────────
  ID      : CAT-042
  Nama    : Elektronik
  Parent  : ROOT
  Level   : 0
  Status  : Aktif
  ─────────────────────────────────
  [METRIK] Waktu Insert : 0.668 ms
================================================================
```

**Validasi & output error:**
```
  ✗ Error: ID 'CAT-042' sudah terdaftar dalam sistem.
  ✗ Error: ID Parent 'CAT-999' tidak ditemukan.
  ✗ Error: ID dan Nama kategori tidak boleh kosong.
```

**Requirement:**
- ID Kategori: string, tidak boleh duplikat, tidak boleh mengandung spasi
- Nama Kategori: string, boleh mengandung spasi
- ID Induk: opsional; jika kosong → node menjadi root baru
- Tampilkan waktu eksekusi insert setelah operasi selesai (`[METRIK]`)

---

### 3.4 Menu [2.2] — Cari Kategori

```
================================================================
  [CARI KATEGORI]  Struktur: HashMap Tree | Node: 10.000
================================================================

  Masukkan ID atau Nama kategori (Enter = tampilkan semua):
  > _
```

**Output hasil pencarian tunggal:**
```
  ✓ Kategori ditemukan.
  ════════════════════════════════
  ID      : CAT-012
  Nama    : Laptop
  Parent  : CAT-001 (Elektronik)
  Level   : 1
  Status  : Aktif
  Anak    : 3 subkategori
  ════════════════════════════════
  [METRIK] Waktu Search : 0.008 ms

  Tampilkan subkategori? (y/n): _
```

**Output subkategori (jika y):**
```
  Hierarki dari [CAT-012] Laptop:
  ──────────────────────────────────────────
  |-- [CAT-101] Gaming Laptop
  |     |-- [CAT-201] RTX Series
  |     └── [CAT-202] Budget Gaming
  |-- [CAT-102] Ultrabook
  └── [CAT-103] 2-in-1 Laptop
  ──────────────────────────────────────────
  Total subkategori: 5 node
  [METRIK] Waktu Traversal : 0.204 ms
```

**Requirement:**
- Jika input kosong (Enter): tampilkan seluruh hierarki dari semua root node
- Indentasi menggunakan karakter `|--` dan `└──` dengan tambahan 2 spasi per level
- Level kedalaman maksimal ditampilkan: tidak terbatas (sesuai data)
- Jika nama/ID tidak ditemukan:
  ```
  ✗ Kategori dengan ID/Nama 'xxx' tidak ditemukan.
  ```

---

### 3.5 Menu [2.3] — Tampilkan Hierarki

```
================================================================
  [TAMPILKAN HIERARKI]  Struktur: HashMap Tree | Node: 10.000
================================================================

  Mode tampilan:
  [1] Seluruh hierarki (semua root)
  [2] Dari kategori tertentu (masukkan ID)
  [3] Hanya sampai kedalaman tertentu (masukkan depth)

  Pilihan: _
```

**Output mode [1] — contoh 5 node pertama (truncated):**
```
  === Seluruh Hierarki Kategori ===

  |-- [C001] Elektronik
  |     |-- [C011] Smartphone
  |     |     |-- [C111] Android
  |     |     └── [C112] iOS
  |     └── [C012] Laptop
  |-- [C002] Fashion
  |     |-- [C021] Pakaian Pria
  ...
  (menampilkan 10.000 dari 10.000 node)
  [METRIK] Waktu Menampilkan Seluruh Data : 358.27 ms
```

**Requirement:**
- Untuk dataset >1.000 node, tampilkan konfirmasi dulu:
  ```
  ! Dataset berisi 10.000 node. Output mungkin panjang.
    Lanjutkan tampilkan ke terminal? (y/n): _
  ```
- Mode depth membatasi kedalaman rekursi (misal: depth=2 → hanya tampil 2 level)
- Gunakan paginasi opsional: setiap 40 baris tampil `-- Tekan Enter untuk lanjut --`

---

### 3.6 Menu [2.4] — Hapus Kategori

```
================================================================
  [HAPUS KATEGORI]  Struktur: HashMap Tree | Node: 10.000
================================================================

  Masukkan ID Kategori yang akan dihapus: _
```

**Konfirmasi sebelum hapus:**
```
  ! PERINGATAN: Menghapus kategori akan menghapus SEMUA
    subkategori di bawahnya secara berantai (Cascading Delete).

  Target : [CAT-001] Elektronik (Level 0)
  Dampak : 247 node akan dihapus

  Konfirmasi hapus? (y/n): _
```

**Output sukses:**
```
  ✓ Sukses! Kategori dan seluruh turunannya dihapus.
  ─────────────────────────────────────────────────
  Node dihapus  : 248 (termasuk target)
  Node tersisa  : 9.752
  ─────────────────────────────────────────────────
  [METRIK] Waktu Delete Cascading : 17.13 ms
```

**Requirement:**
- Sebelum hapus, tampilkan jumlah node yang akan terdampak
- Jika kategori adalah root satu-satunya, tampilkan warning tambahan:
  ```
  ! Ini adalah satu-satunya root. Tree akan menjadi kosong.
  ```
- Jika ID tidak ditemukan:
  ```
  ✗ Kategori 'CAT-999' tidak ditemukan. Penghapusan dibatalkan.
  ```

---

### 3.7 Menu [3] — Benchmark & Perbandingan

#### 3.7.1 Menu [3.1] — Benchmark Struktur Aktif

```
================================================================
  [BENCHMARK]  Struktur: HashMap Tree
================================================================

  Pilih ukuran dataset:
  [1] 1.000 node    [2] 5.000 node    [3] 10.000 node
  [4] Semua ukuran (otomatis)

  Pilihan: _

  Menjalankan benchmark... (jangan tutup terminal)
  ──────────────────────────────────────────────────
  [■■■■■■■■░░░░░░░░░░░░] Load 1.000 node   ...  2.14 ms ✓
  [■■■■■■■■■■■■■■■░░░░░] Load 5.000 node   ... 10.87 ms ✓
  [■■■■■■■■■■■■■■■■■■■■] Load 10.000 node  ... 21.43 ms ✓
  ──────────────────────────────────────────────────
```

**Output tabel hasil:**
```
  ╔═══════════════╦══════════════╦══════════════╦══════════════╗
  ║  Operasi      ║   1.000 node ║   5.000 node ║  10.000 node ║
  ╠═══════════════╬══════════════╬══════════════╬══════════════╣
  ║ Insert        ║    0.027 ms  ║    0.039 ms  ║    0.034 ms  ║
  ║ Search        ║    0.008 ms  ║    0.010 ms  ║    0.007 ms  ║
  ║ Traversal     ║   33.431 ms  ║  133.675 ms  ║  358.277 ms  ║
  ║ Delete        ║   17.131 ms  ║  120.816 ms  ║  326.404 ms  ║
  ║ Memori (KB)   ║     312 KB   ║   1.541 KB   ║   3.082 KB   ║
  ╚═══════════════╩══════════════╩══════════════╩══════════════╝

  [i] Traversal & Delete diukur tanpa output I/O terminal.
```

---

#### 3.7.2 Menu [3.2] — Benchmark Kedua Struktur (Side-by-Side)

```
================================================================
  [BENCHMARK PERBANDINGAN]  HashMap Tree vs Pointer Tree
================================================================

  Menggunakan dataset 10.000 node untuk kedua struktur...
  ──────────────────────────────────────────────────────────────

  ╔══════════════╦════════════════════╦════════════════════╦════════╗
  ║  Operasi     ║  HashMap Tree (ms) ║  Pointer Tree (ms) ║ Selisih║
  ╠══════════════╬════════════════════╬════════════════════╬════════╣
  ║ Insert       ║         0.034      ║         0.041      ║ +0.007 ║
  ║ Search       ║         0.007      ║         0.009      ║ +0.002 ║
  ║ Traversal    ║       358.277      ║       312.441      ║ -45.84 ║
  ║ Delete       ║       326.404      ║       198.223      ║-128.18 ║
  ║ Memori (KB)  ║       3.082        ║         2.341      ║  -0.74 ║
  ╚══════════════╩════════════════════╩════════════════════╩════════╝

  Keterangan Selisih: (+) = Pointer Tree lebih lambat/besar
                      (-) = Pointer Tree lebih cepat/kecil

  Kesimpulan Otomatis:
  ✓ HashMap Tree unggul di: Insert, Search
  ✓ Pointer Tree unggul di: Traversal, Delete, Penggunaan Memori
```

**Requirement:**
- Benchmark dijalankan 3 kali per operasi, diambil nilai median (untuk meminimalkan noise)
- Notasi `+`/`-` pada kolom selisih menunjukkan perbandingan relatif terhadap HashMap Tree sebagai baseline
- Kesimpulan otomatis di-generate berdasarkan nilai numerik

---

#### 3.7.3 Menu [3.3] — Ekspor Hasil ke CSV

```
================================================================
  [EKSPOR BENCHMARK]
================================================================

  Nama file output  : benchmark_result.csv
  Lokasi            : ./output/benchmark_result.csv

  ✓ File berhasil diekspor.
    Format: operasi, ukuran_data, hashmap_ms, pointer_ms

  Buka file? (hanya di environment yang mendukung): (y/n): _
```

**Format CSV output:**
```csv
operasi,ukuran_data,hashmap_ms,pointer_ms
Insert,1000,0.027,0.033
Insert,5000,0.039,0.044
Insert,10000,0.034,0.041
Search,1000,0.008,0.009
...
```

---

### 3.8 Menu [4] — Dataset

#### 3.8.1 Load Dataset dari File

```
================================================================
  [LOAD DATASET]
================================================================

  Path file (default: ./dataset/dataset_kategori.json): _

  Loading...
  ──────────────────────────────────────────────────
  Membaca file         ... ✓  (2.1 MB)
  Parsing JSON         ... ✓  (10.000 record)
  Membangun struktur   ... ✓  (42.31 ms)
  Memvalidasi hierarki ... ✓  (0 orphan node)
  ──────────────────────────────────────────────────
  ✓ Dataset berhasil dimuat. 10.000 node aktif.
```

**Output error jika file tidak ditemukan:**
```
  ✗ Error: File 'dataset_xyz.json' tidak ditemukan.
  ✗ Error: Format JSON tidak valid pada baris 142.
  ✗ Error: Terdapat 5 orphan node (parent ID tidak ditemukan).
            Orphan node dilewati dan tidak dimuat.
```

**Requirement:**
- Path file bisa relatif atau absolut
- Jika dataset sudah ter-load sebelumnya, konfirmasi reset:
  ```
  ! Dataset sebelumnya (10.000 node) akan dihapus.
    Lanjutkan? (y/n): _
  ```
- Validasi orphan node (parent tidak ada): catat dan laporkan, jangan crash

---

#### 3.8.2 Informasi Dataset Aktif

```
================================================================
  [INFO DATASET]
================================================================

  File sumber      : dataset_kategori.json
  Total node       : 10.000
  Node root        : 12 (kategori tanpa induk)
  Kedalaman max    : 6 level
  Node terdalam    : [CAT-8821] "Aksesoris Kamera Mirrorless"
  Node terluas     : [CAT-001] "Elektronik" (247 anak langsung/tidak langsung)
  Struktur aktif   : HashMap Tree
  Memori digunakan : ~3.082 KB
```

---

## 4. Konvensi Tampilan Global

### 4.1 Karakter & Formatting

| Elemen | Karakter | Contoh |
|---|---|---|
| Border header/footer | `=` (64x) | `================================================================` |
| Separator ringan | `-` (64x) | `----------------------------------------------------------------` |
| Indentasi hierarki anak | `\|--` | `\|-- [C001] Elektronik` |
| Indentasi hierarki anak terakhir | `└──` | `└── [C002] Fashion` |
| Indikator sukses | `✓` atau `[OK]` | `✓ Sukses!` |
| Indikator error | `✗` atau `[ERR]` | `✗ Error: ...` |
| Indikator peringatan | `!` atau `[WARN]` | `! PERINGATAN: ...` |
| Indikator info | `[i]` | `[i] Dataset berisi 10.000 node` |
| Indikator metrik | `[METRIK]` | `[METRIK] Waktu Insert : 0.668 ms` |
| Progress bar | `■░` | `[■■■■■░░░░░]` |

> **Catatan implementasi:** Gunakan karakter ASCII fallback (`|--`, `+--`, `*`) jika terminal tidak mendukung UTF-8. Deteksi via `LANG` environment variable atau compile-time flag.

### 4.2 Warna Terminal (Opsional, via ANSI Escape Code)

```cpp
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"   // sukses ✓
#define COLOR_RED     "\033[31m"   // error ✗
#define COLOR_YELLOW  "\033[33m"   // peringatan !
#define COLOR_CYAN    "\033[36m"   // header & label
#define COLOR_BOLD    "\033[1m"    // teks tebal
```

Nonaktifkan warna jika output di-redirect ke file (`./program > out.txt`):
```cpp
bool useColor = isatty(STDOUT_FILENO); // POSIX check
```

### 4.3 Lebar Terminal

- Target lebar: **64–80 karakter** per baris
- Jika nama kategori terlalu panjang (>30 karakter), truncate dengan `...`:
  ```
  |-- [C001] Elektronik & Gadget Terkini dan Te...
  ```

---

## 5. Spesifikasi Input

| Input | Tipe | Validasi |
|---|---|---|
| Pilihan menu | `int` | Harus dalam range yang valid; input non-angka ditolak dengan pesan error |
| ID Kategori | `string` | Tidak boleh kosong, tidak boleh mengandung spasi, maks 50 karakter |
| Nama Kategori | `string` | Tidak boleh kosong, maks 100 karakter |
| ID Parent | `string` | Boleh kosong (→ root); jika diisi, harus ada di sistem |
| Konfirmasi (y/n) | `char` | Hanya terima `y`, `Y`, `n`, `N`; input lain diulang |
| Path file | `string` | Divalidasi keberadaan file sebelum proses load |

**Penanganan input tidak valid:**
```cpp
// Contoh pola
int getMenuChoice(int min, int max) {
    int choice;
    while (!(cin >> choice) || choice < min || choice > max) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  ✗ Input tidak valid. Masukkan angka " << min << "-" << max << ": ";
    }
    return choice;
}
```

---

## 6. Spesifikasi Output Metrik (Benchmark)

Semua output waktu eksekusi menggunakan `chrono::high_resolution_clock`:

```cpp
auto start = chrono::high_resolution_clock::now();
// --- operasi tanpa cout ---
auto end   = chrono::high_resolution_clock::now();
double ms  = chrono::duration<double, milli>(end - start).count();
printf("  [METRIK] Waktu %-20s : %.3f ms\n", namaOperasi, ms);
```

**Format output metrik:**
```
  [METRIK] Waktu Insert              :   0.668 ms
  [METRIK] Waktu Search              :   0.008 ms
  [METRIK] Waktu Traversal           : 358.277 ms
  [METRIK] Waktu Delete Cascading    : 326.404 ms
```

**Aturan benchmark:**
- Operasi Traversal dan Delete **tidak boleh mencetak output ke terminal** selama pengukuran waktu berlangsung (simpan ke buffer string, cetak setelah timer berhenti)
- Setiap benchmark diulang **3 kali**, tampilkan nilai median
- Pengukuran memori menggunakan `getrusage(RUSAGE_SELF, &usage)` (Linux/macOS) atau estimasi manual via `sizeof(node) × jumlah_node`

---

## 7. Alur Program Lengkap (Flowchart Tekstual)

```
START
  │
  ├─► Tampilkan splash screen & header
  ├─► Prompt: Load dataset? (y/n)
  │     ├─ y ─► Load file → validasi → bangun struktur
  │     └─ n ─► Mulai dengan tree kosong
  │
  └─► LOOP: Tampilkan Main Menu
        │
        ├─ [1] Pilih Struktur ──► Update header, reload data jika perlu
        │
        ├─ [2] Manajemen
        │     ├─ [1] Tambah  ──► Input → Validasi → Insert → Tampilkan metrik
        │     ├─ [2] Cari    ──► Input → Search → Tampilkan detail → Opsional traversal
        │     ├─ [3] Hierarki──► Pilih mode → Traversal → Tampilkan tree
        │     └─ [4] Hapus   ──► Input → Konfirmasi → Cascade Delete → Tampilkan metrik
        │
        ├─ [3] Benchmark
        │     ├─ [1] Single  ──► Pilih ukuran → Jalankan 3x → Tampilkan tabel
        │     ├─ [2] Compare ──► Jalankan keduanya → Tampilkan side-by-side
        │     └─ [3] Ekspor  ──► Tulis CSV ke ./output/
        │
        ├─ [4] Dataset
        │     ├─ [1] Load    ──► Input path → Load → Validasi → Konfirmasi
        │     ├─ [2] Info    ──► Tampilkan statistik dataset
        │     └─ [3] Reset   ──► Konfirmasi → Hapus semua node
        │
        └─ [0] Keluar ──────► Bebaskan memori → Tampilkan pesan keluar → EXIT
```

---

## 8. Struktur Kode Antarmuka

### 8.1 File yang Direkomendasikan

```
project/
├── main.cpp              # Entry point & loop menu utama
├── ui/
│   ├── display.h         # Fungsi-fungsi tampilan (header, tabel, hierarki)
│   ├── display.cpp
│   ├── input.h           # Fungsi-fungsi validasi input
│   └── input.cpp
├── benchmark/
│   ├── Benchmark.h       # Timer, pengukur memori, tabel hasil
│   └── Benchmark.cpp
├── hashmap/
│   ├── CategoryHashMap.h
│   └── CategoryHashMap.cpp
├── pointertree/
│   ├── CategoryTree.h
│   └── CategoryTree.cpp
├── dataset/
│   └── DataLoader.h      # Parser JSON ke struktur data
└── output/               # Folder output CSV benchmark (auto-created)
```

### 8.2 Fungsi Display Utama

```cpp
// display.h
void printHeader(string strukturAktif, int jumlahNode, double waktuLoad);
void printMainMenu();
void printSeparator(char c = '=', int width = 64);
void printCategoryDetail(Category& cat);
void printHierarchyTree(/* node */, int depth, bool isLast);
void printBenchmarkTable(BenchmarkResult results[], int count);
void printComparisionTable(BenchmarkResult hashmap, BenchmarkResult pointer);
void printSuccess(string pesan);
void printError(string pesan);
void printWarning(string pesan);
void printMetric(string namaOperasi, double ms);
void clearScreen();   // system("clear") atau system("cls")
```

---

## 9. Contoh Sesi Lengkap (End-to-End Demo)

```
================================================================
  SISTEM MANAJEMEN KATEGORI & HIRARKI DATA
  Struktur Data P4 — Kelompok 10 — IPB University 2026
================================================================
  Struktur Aktif : HashMap Tree
  Node Termuat   : 0
================================================================

  Load dataset saat startup? (y/n): y
  Path file (Enter = default): 
  
  Loading dataset_kategori.json ...
  ✓ 10.000 node berhasil dimuat dalam 42.31 ms.

================================================================
  MENU UTAMA
================================================================
  [1] Pilih Struktur Data
  [2] Manajemen Kategori
  [3] Benchmark & Perbandingan
  [4] Dataset
  [0] Keluar
----------------------------------------------------------------
  Pilih menu: 2

================================================================
  MANAJEMEN KATEGORI  |  HashMap Tree  |  10.000 node
================================================================
  [1] Tambah Kategori
  [2] Cari Kategori
  [3] Tampilkan Hierarki
  [4] Hapus Kategori
  [0] Kembali
----------------------------------------------------------------
  Pilih menu: 2

  Masukkan ID atau Nama: Elektronik

  ✓ Kategori ditemukan.
  ════════════════════════════════════════
  ID      : C001
  Nama    : Elektronik
  Parent  : (root)
  Level   : 0
  Status  : Aktif
  Anak    : 247 subkategori
  ════════════════════════════════════════
  [METRIK] Waktu Search : 0.007 ms

  Tampilkan subkategori? (y/n): y

  Hierarki dari [C001] Elektronik:
  ──────────────────────────────────────────────────────────
  |-- [C011] Smartphone
  |     |-- [C111] Android
  |     |     |-- [C211] Samsung Galaxy Series
  |     |     └── [C212] Xiaomi Series
  |     └── [C112] iOS
  |-- [C012] Laptop
  ...
  (menampilkan 247 dari 247 node)
  ──────────────────────────────────────────────────────────
  [METRIK] Waktu Traversal : 33.431 ms

  Tekan Enter untuk kembali ke menu...
```

---

## 10. Checklist Implementasi Antarmuka

- [ ] Header global selalu tampil dan terupdate setelah setiap operasi
- [ ] Semua input divalidasi; input tidak valid menampilkan pesan error dan meminta ulang
- [ ] Konfirmasi `(y/n)` selalu muncul sebelum operasi destruktif (delete, reset, reload)
- [ ] Semua operasi menampilkan `[METRIK]` waktu eksekusi
- [ ] Benchmark tidak mencampur I/O dengan timer
- [ ] Ekspor CSV berfungsi dan format sesuai spesifikasi
- [ ] Tampilan hierarki menggunakan indentasi yang konsisten per level
- [ ] Program tidak crash pada input edge case (tree kosong, ID tidak ada, file tidak ada)
- [ ] `Ctrl+C` (SIGINT) ditangani gracefully: bebaskan memori sebelum exit
- [ ] Kode display dipisah dari kode logika struktur data (separation of concerns)

---

*Dokumen ini merupakan acuan implementasi antarmuka terminal untuk demo minggu ke-14.*
*Desain dapat disesuaikan selama konsistensi format dan kelengkapan fitur terpenuhi.*
