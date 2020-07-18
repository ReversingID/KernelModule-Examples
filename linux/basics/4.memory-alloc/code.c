/*
    Memory Allocation
    Archive of Reversing.ID
    Based on Linux Kernel Module Programming Guide

    Tested on Linux Kernel 4.9.0-11-amd64 (Debian)

    Alokasi memory di level kernel.

Build:
    $ make

Module info:
    $ modinfo code.ko

List module:
    $ lsmod

Load:
    $ insmod code

Unload:
    $ rmmod code
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>         /* kmalloc(), kfree() */
#include <linux/vmalloc.h>      /* vmalloc() */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Reversing.ID");

/*
    Kernel-Space adalah ruang memory yang hanya ditempati oleh kernel dan module.
    Alokasi memory merupakan operasi raw. Semua operasi alokasi dan dealokasi ditangani
    oleh kernel dan module bertanggung-jawab atas segala operasi yang terjadi.

    Kesalahan penanganan memory tidak hanya berdampak bagi module tetapi keseluruhan kernel.
    Pada beberapa kasus tertentu, kerusakan memory (memory corruption) dapat menyebabkan
    Kernel Panic.


    Terdapat dua API yang disediakan untuk alokasi, yaitu:

        void * kmalloc (size_t size, int flags);
        void * vmalloc (unsigned long size);
    
    kmalloc() mengalokasikan memory yang kontigu secara virtual dan fisikal.
    vmalloc() mengalokasikan memory yang kontigu secara virtual namun tidak menjamin secara
    fisikal.
*/


/*
    Alokasikan ruang sebanyak "size" integer.
    Nilai size valid pada rentang 1 - 20
*/
unsigned int * kdata;       /* alokasi dengan kmalloc */
unsigned int * vdata;       /* alokasi dengan vmalloc */
unsigned int size;

module_param(size, uint, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(m_int, "Size of buffer");


static int __init code_init(void)
{
    int i;

    /* validasi rentang nilai size */
    if (size >= 20 || size == 0) size = 20;

    /* alokasi memory di sisi kernel dengan kmalloc() */
    kdata = kmalloc (size * sizeof(unsigned int), GFP_KERNEL);
    if (! kdata)
    {
        printk(KERN_INFO "Can't allocate %d integers (kmalloc)\n", size);
        return -ENOMEM;
    }

    /* alokasi memory dengan vmalloc() */
    vdata = vmalloc (size * sizeof(unsigned int));
    if (! vdata)
    {
        printk(KERN_INFO "Can't allocate %d integers (vmalloc)\n", size);
        return -ENOMEM;
    }

    printk(KERN_INFO "Allocated (kmalloc) at %p by %d integers\n", kdata, size);
    printk(KERN_INFO "Allocated (vmalloc) at %p by %d integers\n", vdata, size);

    /* penggunaan buffer seperti biasa */
    for (i = 0; i < size; i++)
        kdata[i] = vdata[i] = i;

    return 0;
}

static void __exit code_exit(void)
{
    /* pembersihan memory yang telah dialokasi */
    if (kdata) kfree(kdata);
    if (vdata) vfree(vdata);
}

module_init(code_init);
module_exit(code_exit);