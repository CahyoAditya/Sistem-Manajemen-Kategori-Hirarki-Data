# Sistem Manajemen Kategori & Hirarki Data E-Commerce

Sistem manajemen kategori dan subkategori data e-commerce yang membentuk struktur hierarki non-linear (N-ary Tree). Dikembangkan menggunakan bahasa pemrograman C++ sebagai pemenuhan tugas mata kuliah Struktur Data.

Sistem ini dirancang untuk menguji, memanipulasi, dan membandingkan performa dua pendekatan struktur data yang berbeda dalam menangani data bertingkat berskala besar (hingga 10.000 node).

Note: Nanti UTF-8 ganti full ke ASCII agar OS Windows dapat berjalan dengan baik

---

## 1. Identitas Kelompok
**Kelompok 10 - Struktur Data P4**  
Departemen Ilmu Komputer  
Sekolah Sains Data, Matematika, dan Informatika  
IPB University - 2026

*   **Aditya Cahyo Nugroho** (M0403241109)
*   **Zivanka Aurellia Astadewi Maheswari** (M0403241111)
*   **Anisa Nur Rohmah** (M0403241035)
*   **Nazwa Nadya Rahma** (M0403241060)

---

## 2. Abstrak
Pengelolaan data hierarki merupakan kebutuhan penting pada berbagai sistem informasi, khususnya pada platform e-commerce yang memiliki struktur kategori dan subkategori bertingkat. Pemilihan struktur data yang tepat berpengaruh terhadap efisiensi operasi pengelolaan data seperti penambahan, pencarian, penelusuran, dan penghapusan kategori. Proyek ini bertujuan mengimplementasikan serta membandingkan performa dua pendekatan struktur data untuk merepresentasikan hierarki kategori, yaitu HashMap Tree (Hash Map + Adjacency List) dan Pointer Tree menggunakan metode Left Child Right Sibling (LCRS). Sistem dikembangkan menggunakan bahasa pemrograman C++ dengan fitur pengelolaan kategori berupa insert, search, traversal, dan cascading delete. Pengujian dilakukan menggunakan dataset berukuran 1.000, 5.000, dan 10.000 node untuk mengukur waktu eksekusi masing-masing operasi pada kedua struktur data. Hasil benchmark menunjukkan bahwa HashMap Tree memiliki performa yang lebih baik pada proses build dan insert, sedangkan Pointer Tree lebih unggul pada operasi search, traversal, dan delete.

**Kata Kunci:** struktur data, hierarki kategori, HashMap Tree, Left Child Right Sibling (LCRS).

---

## 3. Pendahuluan

### Latar Belakang & Urgensi Masalah
Pada berbagai sistem informasi berskala besar, seperti platform e-commerce, arsip digital, dan sistem manajemen konten, data sering kali disusun dalam bentuk kategori dan subkategori yang bersifat hierarkis. Struktur hierarki ini, baik berupa kategori produk, klasifikasi dokumen, maupun struktur menu aplikasi, sangat krusial untuk memudahkan pengguna dalam melakukan pengelompokan, pencarian, dan navigasi data.

Pada tahap awal pengembangan sistem, kategori dan relasinya sering dikelola secara sederhana, misalnya menggunakan daftar datar (flat list) yang sekadar mengandalkan penanda relasi induk-anak (parent-child). Pendekatan ini masih mampu ketika jumlah kategori relatif sedikit. Namun, seiring berjalannya waktu, sistem dihadapkan pada pertumbuhan jumlah kategori, kedalaman tingkat hierarki, dan kompleksitas keterhubungan data yang terus meningkat.

Dalam kondisi data berskala besar, pengelolaan kategori menggunakan struktur yang tidak tepat memunculkan berbagai kendala teknis. Operasi dasar seperti pencarian kategori tertentu, penelusuran subkategori, hingga penghapusan kategori beserta seluruh turunannya menjadi sangat lambat dan rentan memicu inkonsistensi data. Oleh karena itu, diperlukan sistem manajemen kategori dan hierarki data sederhana namun kokoh. Pada proyek ini kami mengimplementasikan dan membandingkan dua pendekatan struktur data yaitu HashMap Tree dan Pointer Tree.

### Rumusan Masalah
1. Bagaimana merancang dan mengimplementasikan sistem manajemen hierarki data menggunakan pendekatan HashMap Tree dan Pointer Tree untuk mengatasi kelemahan penelusuran pada struktur datar atau flat list?
2. Bagaimana perbandingan performa waktu pada saat eksekusi antara kedua struktur data tersebut yaitu HashMap Tree dan Pointer Tree?
3. Pendekatan struktur data manakah yang lebih efisien dalam mengeksekusi operasi-operasi yang telah dilakukan?

### Tujuan dan Batasan Proyek
**Tujuan Proyek:**
1. Mengimplementasikan sistem manajemen kategori yaitu membangun sebuah program hierarki data dengan menggunakan dua pendekatan struktur data yang berbeda, yaitu HashMap Tree (kombinasi dari Hash Map dan Adjacency List), serta Pointer Tree dengan metode Left Child Right Sibling (LCRS).
2. Mengukur dan menganalisis perbandingan performa berdasarkan waktu eksekusi dari kedua struktur data tersebut pada operasi dasar pengelolaan hierarki.
3. Menganalisis hasil performa yang telah dilakukan untuk mengetahui kelebihan dari masing-masing struktur data tersebut.

**Batasan Proyek:**
1. Pengujian dilakukan pada memori utama (RAM) menggunakan dataset taksonomi e-commerce Amazon dengan skala maksimal 10.000 node.
2. Operasi yang diuji terbatas pada Build, Insert, Search, Traversal (DFS), dan Cascading Delete.
3. Program dikembangkan menggunakan bahasa C++11 standar dan dieksekusi melalui antarmuka CLI (Command Line Interface).

---

## 4. Landasan Teori & Studi Terkait

### Konsep Struktur Data yang Digunakan
Pengelolaan kategori dalam sistem informasi berskala besar pada dasarnya adalah permasalahan manajemen data hierarkis. Secara matematis dan komputasional, data hierarkis direpresentasikan menggunakan struktur pohon (Tree). Mengingat sebuah kategori induk dapat memiliki jumlah subkategori yang tidak terbatas, struktur dasar yang digunakan adalah N-ary Tree (Cormen et al., 2009).

Untuk mengimplementasikan N-ary Tree di dalam memori, proyek ini menguji dua konsep struktur data utama:
1. **HashMap Tree (Hash Map + Adjacency List):** Pendekatan ini merupakan hibrida yang memanfaatkan konsep Hash Table dan representasi graf Adjacency List. Penggunaan Hash Table memungkinkan pencarian elemen dalam kompleksitas waktu rata-rata O(1) (Goodrich et al., 2014). Untuk membangun hierarki, setiap kategori menyimpan array ID dari anak-anaknya. Konsep ini sangat mengunggulkan kecepatan akses langsung, namun memisahkan secara logis hubungan antar node dari struktur penyimpanan fisiknya.
2. **Pointer Tree (Left-Child Right-Sibling / LCRS):** LCRS adalah teknik optimasi memori klasik untuk merepresentasikan N-ary Tree. Alih-alih menyimpan array pointer berukuran dinamis yang berpotensi memboroskan memori, LCRS mengubah pohon bercabang banyak menjadi struktur berderajat dua yang menyerupai Binary Tree (Cormen et al., 2009). Setiap node hanya memiliki dua pointer utama: firstchild (menunjuk anak pertama) dan nextsibling (menunjuk saudara terdekat).

### Kompleksitas Waktu & Ruang Teoritis

| Operasi | HashMap Tree (Waktu) | HashMap Tree (Ruang) | Pointer Tree (Waktu) | Pointer Tree (Ruang) |
| :--- | :---: | :---: | :---: | :---: |
| **Insert** | O(1) | O(1) | O(1)* | O(1) |
| **Search (by ID)** | O(1) | - | O(1)** | - |
| **Search (by Name)** | O(1) | - | O(1)** | - |
| **Delete** | O(K) | - | O(K) | - |
| **Traversal** | O(n) | O(h) | O(n) | O(h) |

*\* dengan indeks sekunder, \*\* dengan indeks. K = jumlah node yang dihapus (subtree), n = total node, h = kedalaman pohon.*

### Studi/Kasus Sejenis
Struktur kategori pada e-commerce seperti Tokopedia atau Amazon menggunakan arsitektur katalog produk bertingkat. Penelitian sejenis menunjukkan bahwa struktur data berbasis hash map unggul pada query cepat berbasis kunci tunggal, sedangkan struktur berbasis pointer lebih unggul dalam melakukan manipulasi struktural (seperti re-parenting atau cascading deletes) dan penelusuran pohon yang dalam secara efisien.

---

## 5. Desain Sistem & Metodologi

### Desain Arsitektur Program
Program Sistem Manajemen Kategori dan Hierarki Data dirancang menggunakan pendekatan *layered architecture* yang membagi program ke dalam beberapa lapisan berdasarkan tanggung jawab masing-masing komponen. Pendekatan ini bertujuan untuk meningkatkan modularitas kode, mempermudah pemeliharaan program, serta memisahkan logika antarmuka, pemrosesan data, struktur data, dan penyimpanan data.

Pada implementasinya, program terdiri atas empat lapisan utama:
1. **Presentation Layer (CLI Menu):** Menangani antarmuka CLI yang berinteraksi langsung dengan pengguna.
2. **Business Logic Layer (Category Manager):** Mengatur alur operasi kategori: Search Module, Insert Module, Cascading Delete Module, dan DFS Traversal Module.
3. **Data Layer (Struktur Data):** Berisi rancangan struktur HashMap Tree (unordered_map + vector) dan Pointer Tree (LCRS dinamis dengan index map sekunder).
4. **Persistence & Output Layer:** Menangani pembacaan/penulisan berkas `dataset_kategori.json` (File Storage Module) dan pencatatan statistik benchmark ke `output/benchmark_result.csv` (Benchmark Module).

```
                  ┌────────────────────────────────┐
                  │       Presentation Layer       │
                  │           (CLI Menu)           │
                  └───────────────┬────────────────┘
                                  │
                  ┌───────────────▼────────────────┐
                  │      Business Logic Layer      │
                  │       (Category Manager)       │
                  └───────────────┬────────────────┘
                                  │
                  ┌───────────────▼────────────────┐
                  │           Data Layer           │
                  │   (HashMap Tree / Pointer LCRS)│
                  └───────────────┬────────────────┘
                                  │
                  ┌───────────────▼────────────────┐
                  │   Persistence & Output Layer   │
                  │  (JSON DB & CSV Benchmark Out) │
                  └────────────────────────────────┘
```

### Penjelasan Setiap Struktur Data yang Digunakan
*   **HashMap Tree (Hash Map + Adjacency List):** Struktur kategori dibungkus dalam struct `Category` yang menyimpan array `vector<string> child_ids` sebagai adjacency list. Lookup cepat berbasis ID ditangani oleh `unordered_map<string, Category> categoryMap` dan lookup nama lewat `unordered_map<string, string> nameToIdMap`.
*   **Pointer Tree (Left-Child Right-Sibling):** Pohon direpresentasikan secara eksplisit lewat struktur node `CategoryNode` yang memiliki pointer dinamis `firstChild`, `nextSibling`, dan `parent`. Dilengkapi pengindeksan sekunder `unordered_map<string, CategoryNode*> idIndex` dan `nameIndex` untuk pencarian O(1).

### Algoritma Utama (Pseudocode)

#### 1. Algoritma Insert Kategori
```text
IF ID sudah ada THEN
    tampilkan error
    STOP
END IF

Buat node baru

IF memiliki parent THEN
    hubungkan node dengan parent
END IF

Simpan ke struktur data
Perbarui indeks pencarian
Simpan ke file JSON
```

#### 2. Algoritma Search Kategori
```text
Cari pada index berdasarkan ID atau Nama
IF ditemukan THEN
    tampilkan informasi kategori
ELSE
    tampilkan pesan not found
END IF
```

#### 3. Algoritma Cascading Delete
```text
DELETE_CATEGORY(node)
    FOR setiap child DO
        DELETE_CATEGORY(child)
    END FOR
    hapus referensi parent
    hapus node dari struktur data
    hapus indeks
    simpan perubahan ke JSON
```

---

## 6. Implementasi

### Detail Implementasi Tiap Struktur Data
*   **HashMap Tree:**
    ```cpp
    struct Category {
        string id;
        string name;
        string parent_id;
        int level;
        bool is_active;
        vector<string> child_ids;
    };
    unordered_map<string, Category> categoryMap;
    unordered_map<string, string> nameToIdMap;
    ```
*   **Pointer Tree (LCRS):**
    ```cpp
    struct CategoryNode {
        string id;
        string name;
        string parent_id;
        int level;
        bool is_active;
        CategoryNode* firstChild;
        CategoryNode* nextSibling;
        CategoryNode* parent;
    };
    CategoryNode* root;
    unordered_map<string, CategoryNode*> idIndex;
    unordered_map<string, CategoryNode*> nameIndex;
    ```

### Mekanisme Penyimpanan File (Read/Write)
*   **Proses Read Data:** Program membaca data berformat JSON dari disk menggunakan library `json.hpp` milik nlohmann/json. Data di-parse secara linear ke dalam list `std::vector<Category>`, kemudian didistribusikan ke struktur data aktif (HashMap atau LCRS) untuk menyusun relasi induk-anak di memori.
*   **Proses Write Data:** Saat kategori ditambahkan atau dihapus, data di memori dikonversi kembali menjadi representasi flat JSON array dan ditulis ke berkas `dataset_kategori.json` menggunakan stream berkas C++ (`std::ofstream`).

### Penjelasan Modul Penting
*   `CategoryMapManager`: Mengimplementasikan dan mengelola manipulasi struktur data HashMap Tree di memori.
*   `CategoryPointerTree`: Mengimplementasikan dan mengelola modifikasi penautan pointer pada LCRS Tree di memori.
*   `File Storage Module`: Menangani persistensi data JSON ke disk secara modular.
*   `Benchmark Module`: Menyediakan timer resolusi tinggi (`std::chrono::high_resolution_clock`) untuk merekam jejak durasi eksekusi serta modul interaksi memori sistem operasi.
*   `Traversal dan Cascading Delete Module`: Menangani pencarian traversal DFS untuk visualisasi hirarki dan cascading delete untuk pembebasan memori.

---

## 7. Eksperimen & Pengujian

### Skenario Uji (Ukuran Data, Variasi Input)
Eksperimen performa pada sistem manajemen kategori e-commerce ini menggunakan dataset taksonomi produk nyata dari Amazon Reviews 2023.
*   **Ukuran Data:** Diuji pada tiga skala volume data, yaitu **1.000 node** (skala kecil), **5.000 node** (skala menengah), dan **10.000 node** (skala besar).
*   **Variasi Input:** Operasi yang diuji meliputi **Build** (konstruksi pohon), **Insert** (penyisipan node baru), **Search** (pencarian), **Traversal** (DFS subtree), dan **Delete** (cascading delete).

### Metode Pengukuran Waktu & Memori
*   **Waktu:** Diukur secara programatis menggunakan pustaka `<chrono>` dengan presisi tinggi (`high_resolution_clock`). Hasil pencatatan dalam satuan milidetik (ms).
*   **Memori:** RAM Resident Set Size (RSS) aktif diukur saat runtime dengan membaca nilai `VmRSS` dari `/proc/self/status` (untuk Linux) dan menggunakan fungsi `GetProcessMemoryInfo` (untuk Windows). Logika fallback ke estimasi teoretis (120 byte per node) diterapkan jika selisih alokasi bernilai 0.

### Replikasi Eksperimen
Setiap operasi benchmark pada masing-masing skala diulang sebanyak **3 kali** (`repeat = 3`) untuk meminimalkan fluktuasi noise dari proses latar belakang OS (seperti alokasi memori dinamis dan *context switching*). Nilai tengah (**median**) dari seluruh iterasi diambil sebagai hasil komputasi representatif. Hasilnya diekspor langsung ke berkas CSV (`output/benchmark_result.csv`).

---

## 8. Hasil & Analisis

### Tabel Perbandingan Performa

Berikut adalah rekapitulasi waktu eksekusi (milidetik) dan penggunaan memori (Kilobyte) yang didapatkan secara empiris dari mesin pengujian menggunakan dataset Amazon Beauty:

| Operasi | Ukuran Data | HashMap Tree | Pointer Tree / LCRS | Selisih Waktu | Analisis Temuan |
| :--- | :--- | :---: | :---: | :---: | :--- |
| **Build** | 1.000 node | 0.466 ms | 0.896 ms | +0.430 ms | HashMap lebih cepat saat inisialisasi |
| **Insert** | 1.000 node | 0.005 ms | 0.007 ms | +0.002 ms | Berimbang (O(1)) |
| **Search** | 1.000 node | 0.0005 ms | 0.0001 ms | -0.0004 ms | Berimbang (O(1) via map indeks) |
| **Traversal**| 1.000 node | 0.145 ms | 0.005 ms | -0.140 ms | Pointer lebih cepat (LCRS pointer traversal) |
| **Delete** | 1.000 node | 1.870 ms | 0.374 ms | -1.496 ms | Pointer lebih cepat dibebaskan |
| **Memory** | 1.000 node | 117 KB | 117 KB | 0 KB | Penggunaan memori awal setara |
| **Build** | 5.000 node | 2.981 ms | 29.228 ms | +26.247 ms | HashMap jauh lebih cepat |
| **Insert** | 5.000 node | 0.020 ms | 0.030 ms | +0.010 ms | Keduanya sebanding |
| **Search** | 5.000 node | 0.0006 ms | 0.0002 ms | -0.0004 ms | Sangat cepat di bawah 1 mikrodetik |
| **Traversal**| 5.000 node | 0.912 ms | 0.071 ms | -0.841 ms | Pointer LCRS menunjukkan keunggulannya |
| **Delete** | 5.000 node | 24.419 ms | 2.191 ms | -22.228 ms | Pointer LCRS jauh lebih efisien |
| **Memory** | 5.000 node | 585 KB | 585 KB | 0 KB | RAM yang ditempati setara |
| **Build** | 10.000 node| 6.984 ms | 128.440 ms | +121.456 ms| Konstruksi HashMap Tree sangat instan |
| **Insert** | 10.000 node| 0.034 ms | 0.139 ms | +0.105 ms | Keduanya berkinerja setara |
| **Search** | 10.000 node| 0.0008 ms | 0.0002 ms | -0.0006 ms | Keduanya konstan O(1) |
| **Traversal**| 10.000 node| 2.552 ms | 0.179 ms | -2.373 ms | Pointer Tree 14 kali lebih cepat |
| **Delete** | 10.000 node| 89.879 ms | 5.312 ms | -84.567 ms | Pointer Tree jauh lebih unggul |
| **Memory** | 10.000 node| 1171 KB | 1171 KB | 0 KB | Memori fisik aktif setara |

### Analisis Waktu (Build, Insert, Search, Traversal, Delete)
*   **Build:** HashMap Tree menunjukkan efisiensi yang absolut pada proses inisialisasi pada skala masif. Pada pemrosesan dataset Amazon dengan volume sepuluh ribu node, pembentukan struktur dengan tabel hash diselesaikan hanya dalam waktu 6,98 milidetik, sementara struktur Pointer Tree memakan waktu komputasi secara eksponensial hingga menembus angka 128,44 milidetik akibat masifnya proses penelusuran sibling secara linier dari node kiri ke kanan untuk menemukan posisi sisipan yang valid.
*   **Insert & Search:** Laju kecepatan akses pada operasi instan seperti Insert dan Search cenderung berjalan berimbang dan sangat efisien (mendekati 0 milidetik), mengonfirmasi bahwa kedua pendekatan tersebut dirancang untuk mengakomodasi akses langsung ke referensi memori node yang dituju dalam kompleksitas asimtotik konstan O(1).
*   **Traversal & Delete:** Sebaliknya, struktur Pointer Tree (LCRS) membuktikan dominasinya secara telak pada operasi yang melibatkan mobilitas lintas hierarki dan operasi destruktif terhadap memori. Penelusuran pada sepuluh ribu node dieksekusi secara nyaris instan pada 0,18 milidetik dengan Pointer Tree, jauh mengungguli perulangan pencarian beruntun pada mekanisme ketetanggaan milik HashMap Tree yang menyentuh durasi 2,55 milidetik.
*   **Cascading Delete:** Signifikansi efisiensi ini membesar secara dramatis ketika sistem melakukan operasi pelepasan alokasi memori berantai. Pointer Tree hanya memerlukan waktu 5,31 milidetik untuk meruntuhkan seluruh struktur skala raksasa tersebut, berbanding terbalik secara ekstrem dengan pembebasan ruang pada HashMap Tree yang melambat hingga membutuhkan 89,88 milidetik karena harus menghapus ribuan entri secara manual dari hash map dan memodifikasi array anak di parent, yang memicu overhead reorganisasi tabel hash.

### Analisis Penggunaan Memori
Jejak memori fisik aktif (Resident Set Size) dari kedua struktur data relatif setara pada pengujian statis (berkisar antara 117 KB hingga 1171 KB). Namun, Pointer Tree secara teoritis memiliki footprint memori per node yang sedikit lebih besar karena overhead penyimpanan tiga pointer eksplisit per node (`firstChild`, `nextSibling`, `parent`), sedangkan HashMap Tree menghemat memori pointer namun membayar overhead memori pada struktur internal *bucket* `unordered_map` dan vector dinamis.

### Diskusi Trade-off
Karakteristik rasio metrik ini memvalidasi teori fundamental sistem bahwa kedekatan letak referensial pada struktur node melalui pointer komplementer (induk, anak, dan tetangga) mampu mereduksi overhead pencarian di memori sekunder dengan begitu drastis, menjadikannya arsitektur yang ideal dan termumpuni untuk skenario di mana hierarki kategori perlu dibaca menyeluruh atau dimutasi secara masif. Sebaliknya, pendekatan Hash Table sangat efisien untuk proses konstruksi data karena akses terhadap elemen dapat dilakukan secara langsung tanpa harus membangun hubungan pointer yang kompleks.

---

## 9. Kesimpulan & Rekomendasi

### Kesimpulan
Berdasarkan hasil implementasi dan pengujian yang telah dilakukan, dapat disimpulkan bahwa kedua struktur data yang digunakan, yaitu HashMap Tree dan Pointer Tree (LCRS), mampu mengelola data hierarki kategori dengan baik namun memiliki karakteristik performa yang berbeda.
*   HashMap Tree lebih unggul pada proses build dan insert karena memanfaatkan mekanisme hashing yang memungkinkan akses data dilakukan secara cepat.
*   Pointer Tree memiliki performa yang lebih baik pada operasi search, traversal, dan delete karena hubungan antar node direpresentasikan secara langsung melalui pointer.

### Struktur Data Paling Optimal dan Alasannya
Untuk **Sistem Manajemen Kategori E-Commerce** yang dinamis, **Pointer Tree (LCRS)** merupakan struktur yang paling optimal. Meskipun proses konstruksi awalnya (Build) lebih lambat, sistem kategori biasanya jarang melakukan pembentukan ulang seluruh pohon secara real-time. Operasi harian e-commerce seperti navigasi subkategori (Traversal) dan penghapusan kategori beserta produk di bawahnya (Delete) berjalan jauh lebih cepat menggunakan Pointer LCRS, menjaga aplikasi tetap responsif.

### Saran Pengembangan Lanjutan
1.  **Optimasi Insert LCRS:** Menyimpan pointer ke saudara terakhir (lastChild) pada `CategoryNode` untuk menghilangkan proses penelusuran linear sibling, mengubah kompleksitas insert menjadi murni O(1).
2.  **Persistensi Database Nyata:** Mengganti penyimpanan flat JSON dengan database graf (seperti Neo4j) atau database relasional dengan representasi Adjacency List / Nested Set untuk manajemen kategori berskala jutaan node.

---

## 🏗️ Cara Kompilasi & Menjalankan

### Kompilasi:
```bash
g++ -O2 -std=c++11 -o main main.cpp data_structures.cpp ui.cpp
# atau
make clean && make
```

### Menjalankan:
```bash
./main              # Linux/macOS
main.exe            # Windows
```
>>>>>>> 36f4e65 (feat: implement process memory tracking in benchmarking suite and include memory metrics in reports)
