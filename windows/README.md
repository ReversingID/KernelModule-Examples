# Kernel Module Examples

Windows Kernel Module Driver

## Introduction

Windows menyediakan abstraksi terhadap setiap komponen dalam operating system melalui API, termasuk pembangunan Kernel Module. Pengembangan Kernel Module dapat dilakukan melalui dua pendekatan, yaitu:

- [Windows Driver Model (WDM)](wdm)
- [Windows Driver Framework (WDF)](kmdf).

`Windows Driver Model (WDM)` sangat terikat erat dengan operating system. Driver (kernel module) akan berinteraksi secara langsung dengan kernel menggunakan system service routine dan memanipulasi struktur kernel.

`Windows Driver Framework (WDF)` adalah framework yang mengastraksi sebuah driver. WDF berfokus kepada pengembangan driver. Operasi low-level dan interaksi dengan sistem akan ditangani oleh framework sehingga driver dapat berfokus kepada flow utama. WDF dibagi atas dua yaitu `Kernel-Mode Driver Framework (KMDF)` dan `User-Mode Driver Framework (UMDF)`. Namun dalam repository ini hanya akan dibahas `Kernel Mode Driver Framework (KMDF)`.