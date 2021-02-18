# Kernel Module Examples

Simple examples for learning Kernel Module of various platform.

### Selayang Pandang

For English version, see [README.EN.md](README.EN.md).

Repositori ini menghimpun contoh-contoh kode dan kerangka program untuk membangun Kernel Module di beragam platform.

Semua code yang disertakan dalam repositori ini mencakup instruksi kompilasi yang dibutuhkan.

### Pengenalan Kernel Module

Kernel Module adalah potongan kode yang memperluas atau memperkaya kemampuan kernel Operating System. Sebagian besar operating system mendukung Kernel Module dengan istilah berbeda. Beberapa istilah populer antara lain:

- `Kernel Loadable` module (kld) untuk `FreeBSD`
- `Kernel Extension` (kext) untuk `MacOS`
- `Loadble Kernel Module` (lkm) untuk `linux`

Kernel Module sering disebut pula dengan driver atau `device driver`. Meskipun banyak kernel module diciptakan untuk mengimplementasikan interface antara kernel dan perangkat, namun istilah ini tidak sepenuhnya benar. Kernel Module tidak hanya digunakan sebagai antarmuka ke perangkat, namun juga dapat mengimplementasikan fitur tertentu di sisi kernel.

Dukungan terhadap Kernel Module membuat kernel menjadi modular dan lebih fleksibel. Penambahan fungsionalitas kernel dapat dilakukan tanpa harus membangun ulang kernel setiap perubahan terjadi. 

### Disclaimer

Repository ini merupakan inisiatif dari komunitas Reversing.ID untuk mendalami pengembangan Kernel Module.

Reversing.ID tidak berafiliasi dengan vendor atau vendor platform manapun. Repositori ini memiliki tujuan utama sebagai bahan belajar.

### Struktur dan Konten

Repositori ini terbagi menjadi beberapa bagian dengan direktori berbeda, berdasarkan platform Kernel Module.

### How to Download?

Repository dapat diunduh secara keseluruhan atau kode dapat diunduh secara terpisah. 

### How to Contribute?

Ini adalah projek terbuka.

Kamu bisa memberikan sumbangsih berupa kode untuk arsitektur yang belum terdaftar, menambah atau memodifikasi kode yang sudah ada untuk memberikan informasi yang lebih baik.

Yang harus kamu lakukan:

- melakukan pull request.
- mengirimkan email ke pengurus [at] reversing.id
- memberi tahu di telegram @ReversingID