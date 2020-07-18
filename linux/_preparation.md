# Build-Environment Preparation

## Packages Installation

Untuk membangun modul dan memuat/membongkar (load/unload) modul tersebut dengan sukses, terdapat beberapa package yang perlu dipasang dan dipersiapkan. Secara spesifik, operasi terhadap modul memerlukan utility seperti `modprobe`, `insmod`, `depmod`, `rmmod` untuk menampilkan daftar modul yang telah dimuat, memuat modul, melihat daftar dependensi dari sebuah modul, dan mengeluarkan modul dari kernel. Sementara, pembuatan modul memerlukan compiler serta `make` untuk membangun modul secara otomatis. Hal lain yang perlu dipersiapkan adalah kernel headers dengan versi yang sama dengna versi kernel yang dipakai saat ini.

### Pacman (Arch Linux Derivatives)

Arch Linux bersifat rolling release, sehingga besar kemungkinan kita akan dipaksa untuk menginstall package dengan versi paling mutakhir yang tersedia. Hal ini menjadi sedikit bermasalah ketika terdapat beberapa API yang berubah sehingga perlu adanya penyesuaian terhadap kode modul.

Secara umum untuk memenuhi semua package yang dibutuhkan dapat dilakukan dengan menjalankan perintah berikut.

```
# pacman -S gcc kmod
# pacman -S linux linux-headers
```

### APT (Debian Derivatives)

APT digunakan oleh Debian serta beberapa turunan langsung maupun tak langsung.

```
# apt install build-essential kmod
# apt install linux-headers-$(uname -r)
```

### RPM (RedHat Derivatives)

RPM dikembangkan oleh Red-Hat dan menjadi format package di distro Red-Hat serta turunannya.

```
# yum install gcc gcc-c++ make
# yum install kernel-devel
```