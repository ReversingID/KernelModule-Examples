# Kernel Module Examples

BSD Kernel Loadable (KLD).

Perintah-perintah penting yang sering digunakan berkaitan dengan KLD dan pengembangannya.

## Kernel Version

Mendapatkan versi kernel yang sedang berjalan.

```
$ uname -r
```

## Module Compilation 

Compile kernel di luar kernel source tree. Untuk melakukan hal ini dibutuhkan sebuah Makefile. Lihat contoh di setiap direktori untuk informasi lebih lanjut.

```
$ make
```

## Module Operation

#### Module Information

Mendapatkan daftar modul yang sedang dimuat ke kernel.

```
$ kldstat
```

#### Load/Unload Module

Memuat modul. Seluruh dependensi yang dibutuhkan akan dimuat secara otomatis.

```
$ klload module_name
```

Mengeluarkan modul (unload) dari kernel.

```
$ klunload module_name
```