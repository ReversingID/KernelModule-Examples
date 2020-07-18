/*
    Hello
    Archive of Reversing.ID
    Based on Linux Kernel Module Programming Guide
    
    Tested on Linux Kernel 4.9.0-11-amd64 (Debian)
    
    Alur sederhana module dengan menampilkan pesan "Hello World!" saat inisialisasi
    dan "Goodbye World!" saat terminasi.

Build:
    $ make

Module info:
    $ modinfo code.ko

List module:
    $ lsmod

Load:
    $ insmod code.ko

Unload:
    $ rmmod code
 */

#include <linux/module.h>       /* Semua module butuh ini */
#include <linux/kernel.h>       /* Dibutuhkan untuk KERN_INFO */
#include <linux/init.h>

/*
    Kernel Module membutuhkan setidaknya dua fungsi, yaitu entrypoint (inisialisasi)
    dan terminasi (pembersihan module).

    Sembarang fungsi yang akan menjadi entrypoint terlebih dahulu didaftarkan melalui
    module_init(). Fungsi yang menjadi terminator didaftarkan dengan module_exit().
    Fungsi entrypoint akan dipanggil ketika module dimuat (load) ke kernel, sementara
    fungsi terminator akan dipanggil ketika module dibongkar (unload) atau dikeluarkan
    dari kernel.

    Umumnya entrypoint akan meregistrasi handler untuk menangani suatu event di kernel,
    atau menggantikan fungsi kernel dengan kode tertentu yang didefinisikan. Sementara,
    terminator digunakan untuk mengembalikan (restore) apapun yang telah dilakukan di 
    tahap inisialisasi.

    Terdapat beberapa atribut seperti __init, __exit, dan __initdata yang merupakan macro
    untuk fungsi dan variabel.
*/

/* variabel bersifat global akan dialokasikan di ruang kernel */
static int idata __initdata = 135;

static int __init code_init (void)
{
    printk(KERN_INFO "Hello World!\n");
    return 0;
}

static void __exit code_exit (void)
{
    printk(KERN_INFO "Goodbye World!\n");
}

module_init(code_init);
module_exit(code_exit);

/*
Note:

    Pada pengembangan Kernel Module sebelumnya, fungsi entrypoint dan terminator
    didefinisikan dengan init_module() dan cleanup_module().
    Cara ini telah ditinggalkan.

    int  init_module (void) { return 0; }
    void cleanup_module (void) { }
*/