/*
    Linux LKM Skeleton
    Archive of Reversing.ID

    Kerangka kode Linux LKM (Loadable Kernel Module) sesuai standard terkini.
    Template kosong.

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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Reversing.ID");
MODULE_DESCRIPTION("Skeleton of Loadable Kernel Module");
MODULE_SUPPORTED_DEVICE("testdevice");


/* ****************** Type Definition & Data Declaration ****************** */


/* ********************* Internal Functions Prototype ********************* */


/* ****************** Loadable Kernel Module Initialize ******************* */

static int __init code_init (void)
{
    return 0;
}

static void __exit code_exit (void)
{

}

module_init(code_init);
module_exit(code_exit);


/* ******************* Internal Functions Implementation ******************* */