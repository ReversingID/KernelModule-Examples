/*
    Module Documentation
    Archive of Reversing.ID
    Based on Linux Kernel Module Programming Guide
    
    Tested on Linux Kernel 4.9.0-11-amd64 (Debian)
    
    Pemberian informasi license dan dokumentasi yang digunakan oleh module

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
    Terdapat beberapa pilihan license yang didukung:
        - "GPL"
        - "GPL v2"
        - "GPL and additional rights"
        - "Dual BSD/GPL"
        - "Dual MIT/GPL"
        - "Dual MPL/GPL"
        - "Proprietary"
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Reversing.ID");
MODULE_DESCRIPTION("Sample driver");
MODULE_SUPPORTED_DEVICE("testdevice");

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