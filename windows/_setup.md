# Kernel Module Examples

Windows Kernel Module Driver.

## Build Environment

Pengembangan driver memerlukan `Windows SDK` dan `Windows Driver Kit (WDK)` dengan versi yang sesuai. Pendekatan yang sangat direkomendasikan dalam mengembangkan driver adalah melalui `Visual Studio`.

Keseluruhan kode dalam repository ini dibangun dan diuji menggunakan setup sebagai berikut:

- Windows 10 version 10.0.19041
- Visual Studio Community 2019
- Windows 10 SDK 10.0.19041.0
- Windows Driver Kit 10.0.19041.0

Download dan install `Visual Studio Community 2019` melalui [laman berikut](https://visualstudio.microsoft.com/thank-you-downloading-visual-studio/?sku=Community&rel=16).

Ketika instalasi komponen, cari dan install `Windows 10 SDK`.

Download dan install `Windows Driver Kit (WDK)` melalui [laman berikut](https://docs.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk)

## Project

Setiap driver yang dikembangkan harus dikelola ke dalam sebuah project. Repositori ini hanya akan menyimpan source code spesifik terhadap topik-topik tertentu sehingga pembuatan projek harus dilakukan mandiri.

Pastikan telah mempersiapkan development environment!

Untuk membuat project baru (KMDF), lakukan:

- klik **File** - **New project**
- klik **Kernel Mode Driver, Empty (KMDF)**
- sesuaikan nama project dan lokasi.

Tambahkan source code yang dibutuhkan.

- klik kanan pada nama project
- klik **Add**
- klik **New item**
- klik **C++ File (.cpp)** 
- masukkan nama yang sesuai.

Lakukan pengaturan project dengan cara:

- klik kanan pada nama project
- klik **Properties**
- pilih konfigurasi target (Debug/Release dsb).
- navigasi ke **Configuration Properties** - **C/C++** - **Code Generation**
    - set **Security Check** menjadi `Disable Security Check (/GS-)`
    - set **Spectre Migitation** menjadi `Disabled`
- navigasi ke **Configuration Properties** - **Linker** - **Advanced**
    - set **Entry Point** menjadi `DriverEntry`
- navigasi ke **Configuration Properties** - **Inf2Cat** - **General**
    - set **Run Inf2Cat** menjadi `No`

## Debugging Environment

Debugging kernel (dan driver/kernel module) tidak dapat dilakukan pada mesin yang sama. Diperlukan environment terpisah untuk menjalankan kernel dan kernel module yang akan diuji.

Setup yang diperlukan sebagai berikut:

- Target Machine.
- Debugger Machine.

Debugging dilakukan secara remote (`Remote Debugging`) dengan `Target Machine` dapat berupa mesin fisik maupun VM (vmware, virtualbox, dsb).

Berdasarkan koneksi, setidaknya terdapat dua cara debugging, yaitu:

- Serial line
- Network

#### Serial Line 

`Target Machine` dan `Debugger machine` dihubungkan dengan serial port. Jika `Target Machine` berupa VM maka koneksi serial terjadi secara virtual dengan named pipe.

Pada VM (vmware), lakukan konfigurasi sebagai berikut:

- pastikan `Target Machine` dalam keadaan mati (turn off).
- klik **VM** - **Settings**
- pada tab **Hardware**, klik **Add**
- pilih **Serial Port** dan klik **Finish**
- kembali pada tab **Hardware**, pilih **Use named pipe**
    - set named pipe sebagai `\\.\pipe\target` (atau nama apapun)

Kemudian jalankan perintah berikut untuk mengaktifkan fitur debugging pada kernel setiap booting. Eksekusi pada cmd dengan hak akses Administrator.

```
$ bcdedit /debug on
$ bcdedit /dbgsettings serial debugport:2 baudrate:115200
$ bcdedit /set nointegritychecks on
$ bcdedit /set testsigning on
```

Restart `Target Machine` untuk load pengaturan yang telah diberikan.

Pengaturan serial line hanya dilakukan sekali saja. Untuk setiap kali debugging akan dilakukan, lakukan pengaturan berikut pada debugger (`windbg`):

- klik **File** - **Kernel debug**
- pilih tab **COM** 
    - set **Baud Rate** menjadi 115200
    - set **Port** menjadi `\\.\pipe\target`
    - centang **Pipe** dan **Reconnect**

Restart `Target Machine` dan tunggu sampai `windbg` mendapat komunikasi dari `Target Machine`. 

#### Network

`Target Machine` dan `Debugger machine` dihubungkan dengan TCP/IP network. Jika `Target Machine` berupa VM maka koneksi terjadi secara virtual.

Download [Debugging Tools](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/debugger-download-tools)

Pada `Target machine`, lakukan:

- buat direktori `C:\KDNET` 
- salin `kdnet.exe` dan `VerifiedNICList.xml` ke `C:\KDNET`.

Kemudian jalankan perintah berikut untuk mengaktifkan fitur debugging pada kernel setiap booting. Eksekusi pada cmd dengan hak akses Administrator.

```
$ bcdedit /debug on
$ bcdedit /dbgsettings net hostip:HOST port:53000
$ bcdedit /set nointegritychecks on
$ bcdedit /set testsigning on
```

dengan HOST adalah IP address dari `Debugger Machine`.

Sebuah key akan diberikan ketika mengatur `/dbgsettings`. Simpan key tersebut, anggap sebagai KEY.

Restart `Target Machine` untuk load pengaturan yang telah diberikan.

Pengaturan serial line hanya dilakukan sekali saja. Untuk setiap kali debugging akan dilakukan, lakukan pengaturan berikut pada debugger (`windbg`):

- klik **File** - **Kernel debug**
- pilih tab **NET** 
    - set **Port** menjadi 53000
    - set **Key** masukkan KEY yang diberikan oleh `bcdedit /dbgsettings`

Restart `Target Machine` dan tunggu sampai `windbg` mendapat komunikasi dari `Target Machine`. 