# Kernel Module Examples

Linux Loadable Kernel Module (LKM)

## Loadable Kernel Module

Kernel Linux bersifat modular. Modularitas membuat kode kernel dapat dibagi menjadi duab bagian yaitu kode inti serta modul-modul yang melengkapi. Hal ini menjadikan pengembangan dan integrasi setiap module menjadi lebih mudah. Setiap pengembangan modul dapat bersifat independen selama mengikuti kerangka yang telah ditetapkan. Penambahan fungsionalitas kernel pun dapat dilakukan tanpa harus membangun ulang kernel setiap perubahan terjadi.

#### Device Drivers

Salah satu kelas / kategori LKM adalah `device driver`. Kategori ini adalah kategori paling lazim, umumnya menyediakan fungsionalitas hardware seperti serial port. Unix menganut filososi bahwa segala hal di Unix adalah file, termasuk perangkat. Setiap perangkat direpresentasikan sebagai sebuah file khusus yang terletak di `/dev` dan disebut pula sebagai `device file`. Device driver menyediakan mekanisme komunikasi antara perangkat dan user program.

### Major and Minor Numbers

Setiap perangkat dengan kategori yang sama dikendalikan oleh device driver yang sama. Sebagai contoh, berikut adalah beberapa device file untuk perangkat disk.

```
$ ls -l /dev/sda*
brw-rw---- 1 root disk 8, 0 Aug 26 12:10 /dev/sda
brw-rw---- 1 root disk 8, 1 Aug 26 12:10 /dev/sda1
brw-rw---- 1 root disk 8, 2 Aug 26 12:10 /dev/sda2
```

Perhatikan angka-angka yang dipisahkan oleh koma (dalam hal ini contohnya 8,1). Angka pertama merupakan `Major Number`. Angka kedua disebut sebagai `Minor Number`. Major Number merepresentasikan identitas unik dari driver yang berhak menangani operasi terhadap suatu jenis perangkat. Sementara Minor Number digunakan oleh driver untuk membedakan perangkat satu dengan perangkat lain yang dikendalikan.

### Device Types

Perangkat dibagi menjadi dua tipe: `character device` dan `block device`. Klasifikasi berdasarkan segi bagaimana data dioperasikan. Sebuah block device mengoperasikan data dalam bentuk block (kumpulan data dengan ukuran yang tetap), sehingga data diakses dalam kelipatan block. Sementara character device dapat beroperasi data individu dengan ukuran berapapun.

Ketika instalasi terjadi, semua perangkat yang terdeteksi akan didaftarkan. Pembentukan device file dilakukan oleh perintah `mknod`. Untuk membentuk sebuah character device baru dengan nama `RevID` dengan major/minor 12 dan 2, dapat dilakukan sesederhana memberikan perintah berikut:

```
$ mknod /dev/RevID c 12 2
```

Tidak ada aturan khusus yang memaksa semua device file harus diletakkan di direktori /dev. Ini merupakan sebuah kesepakatan. Sehingga sah-sah saja untuk menciptakan device file di direktori lain untuk keperluan testing, namun sangat disarankan untuk mengikuti kesepakatan yang ada.