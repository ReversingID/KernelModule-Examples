# Kernel Module Examples

Linux Loadable Kernel Module (LKM)

## Introduction

`Loadable Kernel Module (LKM)` adalah istilah di Linux untuk menyebut Kernel Module. Sebuah LKM merupakan object code, potongan kode yang dapat dimuat (`load`) dan dibongkar (`unload`) dari kernel pada saat runtime.

Kernel Linux bersifat modular. Modularitas membuat kode kernel dapat dibagi menjadi dua bagian, yaitu kode inti dan modul-modul yang melengkapi. Hal ini menjadikan pengembangan dan integrasi setiap modul menjadi lebih mudah. Pengembangan dapat bersifat independen selama mengikuti kerangka yang telah ditetapkan. Penambahan fungsionalitas kernel pun dapat dilakukan tanpa harus membangun ulang kernel setiap perubahan terjadi.

Beberapa informasi yang perlu diketahui (mungkin tidak berisi info terkini):

- [Pengenalan kernel Linux](_introduction.md)
- [Perintah penting](_commands.md)
- [Persiapan lingkungan pengembangan](_preparation.md)
- [Struktur data penting](_structures.md)
