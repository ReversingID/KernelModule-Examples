# Kernel Module Examples

Solaris Loadable Kernel Module (LKM)

Perintah-perintah penting yang sering digunakan berkaitan dengan pengembangan kernel module.

## Kernel Version

Mendapatkan versi kernel yang sedang berjalan.

```
$ uname -r
```

## Module Location

Sebuah kernel module dapat ditemukan pada beberapa direktori default, yakni:

- `/kernel/`
- `/platform/i86pc/kernel/`
- `/usr/kernel/`

Sebagian besar module berada di dalam direktori `/kernel`.

## Module Compilation

Compile kernel module di luar kernel source tree (di luar direktori source code kernel).

Umumnya perintah compile dilakukan sebagai berikut (64-bit):

```
$ gcc -D_KERNEL -m64 -mcmodel=kernel -mno-red-zone -ffreestanding
-nodefaultlibs -c code.c
$ ld -dy -r code.o -o code
```

Solaris kernel tidak banyak menyediakan fungsi C di dalam kernel. Apabila fungsi standard library C ingin digunakan maka fungsi tersebut harus diekstrak dari `/lib/libc.a` dan lakukan link secara manual.

```
$ ar -x /lib/libc.a memcpy.o
$ ld -dy -r code.o memcpy.o -o code
```

## Module Operation

Modul disimpan di direktori `/lib/modules/kernel_version` dengan `kernel_version` merupakan rangkaian angka dan huruf yang mengidentifikasi versi kernel.

#### Module Information

Mendapatkan daftar modul yang sedang dimuat ke kernel.

```
$ modinfo
```

Mendapatkan informasi dependensi sebuah module

```
$ kldd module
```

#### Load/Unload Module

Memuat modul.

```
# modload module_name
```

Mengeluarkan modul (unload) dari kernel

```
# modunload -i module_id
```