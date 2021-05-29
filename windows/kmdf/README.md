# Kernel Module Examples

Kernel-Mode Driver Framework (KMDF)

## Introduction

`Kernel-Mode Driver Framework (KMDF)` adalah penyederhanaan pengembangan Kernel Module dengan berorientasi kepada framework dan library. KMDF driver bersifat object-oriented, event-driven dan link secara dinamis dengan Kernel Mode Driver Framework saat runtime.

#### Struktur Driver

KMDF mengimplementasikan fungsi `DriverEntry()` sebagai entrypoint serta beberapa fungsi lain yang digunakan untuk menangani I/O request yang relevan.

KMDF mendaftarkan sebuah dispatch routine yang digunakan untuk menerima `IRP (I/O Request Packet)` dari I/O manager, melakukan parsing, dan kemudian memanggil fungsi Event Callback untuk merespon event yang terjadi.

Secara umum, sebuah driver atau kernel module memiliki:

- fungsi `DriverEntry()` 
- fungsi `EvtDriverDeviceAdd()`
- satu atau lebih I/O queue
- satu atau lebih fungsi I/O event callback

#### Device Objects & Role

Sebuah Kernel Module menciptakan satu atau lebih Device Object. Setiap Device Object merepresentasikan Device Role sebagai target dari I/O Request. `Physical Device Object (PDO)` merepresentasikan Bus Driver, `Functional Device Object (FDO)` merepresentasikan Function Driver, dan `Filter Device Object (Filter DO)` merepresentasikan Filter Driver.

Dalam KMDF, setiap role ditentukan secara eksplisit (PDO, FDO, dan Filter DO) dengan event callback yang sesuai akan didaftarkan untuk role tersebut. KMDF juga memiliki konfigurasi agar setiap Device Object menerima tipe I/O request tertentu. Framework hanya akan memanggil driver untuk menangani I/O request yang telah ditentukan.