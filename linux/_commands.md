# Kernel Module Examples

Linux Loadable Kernel Module (LKM)

Perintah-perintah penting yang sering digunakan berkaitan dengan LKM dan pengembangannya.

## Kernel Version

Mendapatkan versi kernel yang sedang berjalan.

```
$ uname -r
```

## Module Compilation

Compile kernel di luar kernel source tree (di luar direktori source code kernel). Untuk melakukan hal ini dibutuhkan sebuah `Makefile`. Lihat contoh di setiap direktori untuk informasi lebih lanjut.

```
$ make
```

## Module Operation

Modul disimpan di direktori `/lib/modules/kernel_version` dengan `kernel_version` merupakan rangkaian angka dan huruf yang mengidentifikasi versi kernel.

#### Module Information

Mendapatkan daftar modul yang sedang dimuat ke kernel.

```
$ lsmod
```

Sebagai alternatif, hal yang serupa dapat dicapai dengan membaca file `/proc/modules` dengan privilege root.

```
# cat /proc/modules
```

Menampilkan informasi tentang modul.

```
$ modinfo module_name
```

Untuk menampilkan informasi yang lebih komprehensif dari seluruh modul yang ada.

```
$ modprobe -c | less
```

Menampilkan konfigurasi dari modul tertentu.

```
$ modprobe -c | grep module_name
```

Menampilkan daftar dependensi dari sebuah modul (ataupun alias), termasuk modul itu sendiri.

```
$ modprobe --show-depends module_name
```

#### Load/Unload Module

Memuat modul. Seluruh dependensi yang dibutuhkan akan dimuat secara otomatis.

```
# modprobe module_name
```

Memuat modul dengan menyebut filename (file yang tak terdapat di `/lib/modules/kernel_version`)

```
# insmod filename [args]
```

Mengeluarkan modul (unload) dari kernel (pilih salah satu perintah)

```
# modprobe -r module_name
# rmmod module_name
```

## Configuration

#### Setting Module Options

Setup secara manual ketika modul akan dimuat. Setiap modul dapat memiliki argumen yang tak terbatas jumlahnya. Argumen ini dapat diberikan secara manual melalui perintah dengan format berikut

```
# modprobe module_name argname=value
```

Argumen juga dapat diberikan secara otomatis menggunakan file yang didefinisikan di direktori `/etc/modprobe.d`. Direktori ini digunakan oleh `udev` yang akan memberikan argumen tersebut ke `modprobe` untuk memuat modul selama system boot. Tidak ada batasan dalam penamaan sebuah file konfigurasi, namun setiap file konfigurasi harus diakhiri dengan ekstensi `.conf`. Konten dari file tersebut memiliki format sebagai berikut:

```
options module_name argname=value
```

#### Aliasing

Alias adalah nama lain dari modul. Nama lain ini digunakan untuk merujuk modul yang terdefinisi. Hal ini sangat membantu apabila terdapat nama modul yang sangat panjang. Alias didefinisikan di dalam file konfigurasi di direktori `/etc/modprobe.d`.

Format alias adalah sebagai berikut.

```
alias shortname really_long_module_name
```

#### Blacklisting

Blacklist adalah mekanisme untuk mencegah pemuatan sebuah modul. Blacklist dideklarasikan di file konfigurasi di direktori `/etc/modprobe.d` dengan format sebagai berikut.

```
blacklist modname
```