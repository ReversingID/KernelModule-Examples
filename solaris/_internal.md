# Kernel Modules Examples

Solaris Loadable Kernel Module (LKM)

## Introduction

Kernel Solaris merupakan microkernel dengan hampir keseluruhan fungsionalitas (contoh: ip, scsi, ufs, dsb) diimplementasikan sebagai Kernel Module.

## Modules Types

LKM adalah file binary object.

LKM didefinisikan di `/usr/include/sys/modctl.h` sebagai salah satu dari tujuh tipe:

- device drivers
- system calls
- file system
- misc
- stream modules
- scheduling classes
- exec file type

## Symbols & Module Information

Kernel mengelola sebuah tabel secara dinamis untuk menyimpan informasi module. Sebuah pseudodevice bernama `/dev/ksyms`, berisi symbol (nama variabel dan fungsi) dari seluruh module yang telah dimuat.



#### Booting

Proses bootstrap dan inisialisasi kernel terbagi atas beberapa tahap:

- loading `bootblock`: read dan load `bootblock` ke memory, umumnya berada di 