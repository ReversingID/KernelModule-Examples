# Kernel Module Examples

Windows Kernel Module Driver

Perintah-perintah penting yang sering digunakan berkaitan dengan Windows Driver dan pengembangannya.

## Kernel Version

Mendapatkan versi kernel yang sedang berjalan.

```
$ systeminfo | findstr Build
```

## Module Operation

Module di-load sebagai service dengan tipe kernel.

#### Module Information

Mendapatkan daftar module yang sedang dimuat ke kernel.

```
$ driverquery /v
```

#### Load/Unload Module

Module dimuat sebagai service dengan tipe kernel. Sehingga, untuk memuat module maka sebuah service harus dibuat terlebih dahulu.

Load module

```
$ sc create SvcDriver type=kernel binpath=path/code.sys
$ sc start SvcDriver
```

Unload module

```
$ sc stop SvcDriver
$ sc delete SvcDriver
```

Sebagai alternatif, driver dapat dimuat dengan aplikasi pihak ketiga seperti:

- [OSR Driver Loader](https://www.osronline.com/article.cfm%5Earticle=157.htm)
