/*
    Argument Passing
    Archive of Reversing.ID
    Based on Linux Kernel Module Programming Guide

    Tested on Linux Kernel 4.9.0-11-amd64 (Debian)

    Pemberian dan pemrosesan argumen pada module.

Build:
    $ make

Module info:
    $ modinfo code.ko

List module:
    $ lsmod

Load:
    Bandingkan pesan dari perintah berikut
    $ insmod code.ko
    $ insmod code.ko m_string="Reversing.ID" m_byte=255 m_intArray=-1
    $ insmod code.ko m_string="sebuahstringyangsangatpanjangdiberikankemodul" m_intArray=1,3

Unload:
    $ rmmod code
*/

#include <linux/module.h>       /* Semua module butuh ini */
#include <linux/kernel.h>       /* Dibutuhkan untuk KERN_INFO */
#include <linux/init.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Reversing.ID");

/*
    Setiap module dapat memiliki sebanyak apapun argumen yang dapat diberikan 
    kepadanya.Argumen ini ekivalen dengan argc/argv pada fungsi main() di program 
    standard.

    Agar argumen dapat digunakan, terlebih dahulu informasi argumen ini harus 
    dideklarasikan. Deklarasi menggunakan macro module_param(). Pada saat pemuatan 
    module (menggunakan insmod), argumen ini akan diisi secara otomatis.

    Variabel static akan berada di area global pada kernel. Variabel ini akan hidup 
    selama kernel berjalan dan LKM dimuat di kernel. Dengan demikian gunakan variabel 
    statis seperlunya.

    Argumen dapat bersifat opsional, apabila argumen tidak diberikan, maka nilai yang 
    ada pada deklarasi akan digunakan.
*/

#define MAX_ARR 2

/*
Tipe data:
    bool
    invbool
        menyimpan nilai boolean (true atau false).
        invbool adalah inversi nilai, sehingga true dianggap sebagai false, begitu juga
        sebaliknya.
    charp
        char pointer, menunjuk ke alamat memory yang dialokasikan untuk string.
    short
    int
    long
    ushort
    uint
    ulong
        bilangan bulat (integer) dengan berbagai ukuran.
        versi yang diawali dengan u menandakan sebagai bilangan unsigned.
*/

static short int m_short  = 1;
static int       m_int    = 420;
static long int  m_long   = 9999;
static char    * m_string = "Rev.ID";
static int       m_intArray[MAX_ARR] = { -1, -1 };
static int       arr_argc = 0;

/* 
Prototipe:
    module_param(name, type, permission);

    * parameter pertama merupakan nama variabel (yang akan jadi nama parameter module)
    * parameter kedua merupakan tipe data
    * parameter ketiga merupakan permission (ketika parameter diekspos ke sysfs).

Integer dapat berupa integer bertanda (sign) maupun tak bertanda (unsigned)
*/

module_param(m_short, short, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(m_short, "A short integer");
module_param(m_int, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(m_int, "An integer");
module_param(m_long, long, S_IRUSR);
MODULE_PARM_DESC(m_long, "A long integer");
module_param(m_string, charp, 0000);
MODULE_PARM_DESC(m_string, "A character string");

/*
Prototipe:
    module_param_array(name, type, num, permission);

    * parameter pertama merupakan nama variabel (yang akan jadi nama parameter module)
    * parameter kedua merupakan tipe data
    * parameter ketiga merupakan pointer ke variabel yang akan menyimpan jumlah elemen array
      yang diberikan parameter.
    * parameter keempat merupakan permission (ketika parameter diekspos ke sysfs).
*/

module_param_array(m_intArray, int, &arr_argc, 0000);
MODULE_PARM_DESC(m_intArray, "An array of integers");

static int __init code_init(void)
{
    int i;

    printk(KERN_INFO "m_short  (short integer): %hd\n", m_short);
    printk(KERN_INFO "m_int    (integer): %d\n", m_int);
    printk(KERN_INFO "m_long   (long integer): %ld\n", m_long);
    printk(KERN_INFO "m_string (string): %s\n", m_string);

    for (i = 0; i < arr_argc; i++)
    {
        printk(KERN_INFO "m_intArray[%d] = %d\n", i, m_intArray[i]);
    }

    printk(KERN_INFO "got %d arguments for m_intArray.\n", arr_argc);
    return 0;
}

static void __exit code_exit(void)
{
    printk(KERN_INFO "Goodbye World!\n");
}

module_init(code_init);
module_exit(code_exit);