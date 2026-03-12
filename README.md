# Sistem Manajemen Kategori & Hirarki Data E-Commerce

Proyek ini adalah implementasi sistem manajemen kategori dan subkategori data e-commerce yang membentuk struktur hierarki (non-linear). Dikembangkan menggunakan bahasa C++ sebagai pemenuhan tugas mata kuliah Struktur Data.

Sistem ini dirancang untuk menguji, memanipulasi, dan membandingkan performa struktur data dalam menangani data bertingkat berskala besar (hingga 10.000 *nodes*).

## 🚀 Fitur Utama
1. **Penambahan Kategori (Insert):** Menambahkan kategori root baru atau menyisipkan subkategori di bawah kategori induk tertentu secara instan $O(1)$.
2. **Pencarian Universal (Search):** Pencarian kategori instan $O(1)$ menggunakan ID maupun Nama Kategori.
3. **Penelusuran Pohon (Tree Traversal):** Menampilkan struktur turunan kategori (subkategori) dalam bentuk visual pohon menggunakan algoritma *Depth-First Search* (DFS).
4. **Penghapusan Berantai (Cascading Delete):** Menghapus sebuah kategori beserta **seluruh** anak dan turunannya secara rekursif agar tidak ada *orphan data*.
5. **Benchmarking Terintegrasi:** Dilengkapi dengan *timer* presisi tinggi (`<chrono>`) untuk mengukur waktu eksekusi operasi di memori secara seketika (Waktu Insert, Waktu Search, Waktu Traversal, dan Waktu Delete).

## 🧠 Struktur Data yang Digunakan
Sistem ini menggunakan struktur data **N-ary Tree yang direpresentasikan melalui Hash Map dan Adjacency List**.
* **Hash Map Utama (`std::unordered_map`):** Digunakan untuk menyimpan seluruh *node* kategori. *Key* berupa ID Kategori, dan *Value* berupa Objek Kategori. Memungkinkan akses data acak yang sangat cepat $O(1)$.
* **Indeks Resolusi Nama (`std::unordered_map`):** Hash map sekunder untuk memetakan Nama Kategori ke ID Kategori.
* **Adjacency List (`std::vector<string>`):** Disematkan di dalam setiap objek kategori untuk menyimpan daftar ID anak-anaknya (*child_ids*). Digunakan untuk menelusuri hierarki dari atas ke bawah.

## 🛠️ Prasyarat (Prerequisites)
Untuk melakukan kompilasi dan menjalankan program ini, pastikan sistem Anda memiliki:
1. **Compiler C++** yang mendukung standar C++11 atau lebih baru (seperti GCC/MinGW, Clang, atau MSVC).
2. **Library nlohmann/json:** Program ini bergantung pada library pihak ketiga untuk pemrosesan JSON.
   * Unduh file `json.hpp` dari [Repositori Resmi nlohmann/json](https://github.com/nlohmann/json).
   * Letakkan file `json.hpp` di dalam satu folder (direktori) yang sama dengan file `main.cpp` Anda.

## ⚙️ Cara Kompilasi dan Menjalankan
Buka terminal/Command Prompt, arahkan ke direktori proyek, lalu jalankan perintah berikut:

**1. Proses Kompilasi**
Disarankan menggunakan *flag* optimasi `-O2` agar hasil pengukuran waktu (*benchmarking*) lebih akurat dan stabil.
```g++ -O2 main.cpp -o main```

**2. Menjalankan Program**
Untuk pengguna Windows:
```main.exe```
Untuk pengguna Linux/macOS:
```./main```

## 📂 Struktur File
main.cpp : Berisi source code utama sistem, logika struktur data, dan antarmuka CLI.

json.hpp : Library header-only untuk parsing data JSON.

dataset_kategori.json : File basis data lokal yang otomatis terbuat/diperbarui oleh sistem untuk menyimpan struktur hierarki.

## 👥 Tim Pengembang
Kelompok 10 - Struktur Data P4  
Departemen Ilmu Komputer  
Sekolah Sains Data, Matematika, dan Informatika  
IPB University - 2026

Aditya Cahyo Nugroho (M0403241109)  
Zivanka Aurellia Astadewi Maheswari (M0403241111)  
Anisa Nur Rohmah (M0403241035)  
Nazwa Nadya Rahma (M0403241060)  


**Saran Tambahan Saat Demo:**
Ingatlah untuk selalu menggunakan perintah `g++ -O2 main.cpp -o main` saat men-*compile* program sebelum mengambil data eksperimen untuk laporan Anda. *Flag* `-O2` akan menyuruh *compiler* untuk mengoptimalkan memori dan kecepatan prosesor secara maksimal, sehingga angka `<chrono>` yang Anda dapatkan benar-benar menunjukkan performa murni struktur datanya.
