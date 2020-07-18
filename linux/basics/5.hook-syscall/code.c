/*
    Hook Syscall
    Archive of Reversing.ID
    Based on Linux Kernel Module Programming Guide

    Tested on Linux Kernel 4.9.0-11-amd64 (Debian)

    Hook callback untuk menggantikan syscall tertentu.
    Hook syscall getdents()

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

#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/dirent.h>
#include <linux/version.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Reversing.ID");

/*
    Syscall (system call) adalah layanan yang disediakan oleh kernel
    kepada program di user-space untuk menangani operasi-operasi low-level
    yang umumnya berkaitan langsung dengan abstraksi resource.

    Syscall dikelompokkan dalam sebuah tabel (Syscall Table). Setiap Syscall
    diidentifikasi dengan ID berupa bilangan bulat. Setiap entry pada 
    tabel merupakan alamat yang merujuk kepada handler yang sesuai.

    Permintaan syscall oleh user-space program dilakukan dengan melakukan
    interupsi. Eksekusi berpindah dari user-space ke kernel-space. 
    Kernel kemudian mengidentifikasi ID dan mengeksekusi handler yang sesuai
    dari tabel syscall.
*/


/*
    Hook dilakukan terhadap:

    getdents, getdents64
    Dapatkan isi dari direktori (directory entry)

    int getdents  (unsigned int fd, struct linux_dirent *dirp,
                   unsigned int count);
    int getdents64(unsigned int fd, struct linux_dirent64 *dirp,
                   unsigned int count);
*/

unsigned long cr0;

/* pointer ke syscall table */
static unsigned long *__sys_call_table;

/* pointer ke syscall original */
typedef asmlinkage int (*_o_getdents_t  ) (unsigned int, struct linux_dirent *, unsigned int);
typedef asmlinkage int (*_o_getdents64_t) (unsigned int, struct linux_dirent64 *, unsigned int);

_o_getdents_t   ptr_getdents;
_o_getdents64_t ptr_getdents64;

/* Dapatkan alamat syscall table */
unsigned long *
get_syscall_table_addr (void)
{
    unsigned long * result;

#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 4, 0)
    result = (unsigned long *) kallsyms_lookup_name("sys_call_table");
    return result;
#else 
    unsigned long i;

    for (i = (unsigned long) sys_close; i < ULONG_MAX; i += sizeof(void*))
    {
        result = (unsigned long*) i;

        if (result[__NR_close] == (unsigned long) sys_close)
            return result;
    }

    return NULL;
#endif 
}

/* syscall akan memanggil fungsi ini */
asmlinkage int 
new_getdents   (unsigned int fd, struct linux_dirent __user *dirent, unsigned int count)
{
    int retval = ptr_getdents(fd, dirent, count);

    /* proses data di sini */

    return retval;
}

asmlinkage int 
new_getdents64 (unsigned int fd, struct linux_dirent64 __user *dirent, unsigned int count)
{
    int retval = ptr_getdents64(fd, dirent, count);

    /* proses data di sini */

    return retval;
}

static inline void
protect_memory(void)
{
    write_cr0(cr0);
}

static inline void
unprotect_memory(void)
{
    write_cr0(cr0 & ~0x00010000);
}


static int __init code_init(void)
{
    /* temukan alamat syscall table */
    __sys_call_table = get_syscall_table_addr ();
    if (! __sys_call_table)
        return -1;

    cr0 = read_cr0 ();

    ptr_getdents   = (_o_getdents_t  ) __sys_call_table[__NR_getdents];
    ptr_getdents64 = (_o_getdents64_t) __sys_call_table[__NR_getdents64];

    printk (KERN_INFO "Syscall Table at %p\n", __sys_call_table);
    printk (KERN_INFO "   __NR_getdents   at %p\n", (void*) __sys_call_table[__NR_getdents]);
    printk (KERN_INFO "   __NR_getdents64 at %p\n", (void*) __sys_call_table[__NR_getdents64]);

    unprotect_memory();
    __sys_call_table[__NR_getdents]   = (unsigned long) new_getdents;
    __sys_call_table[__NR_getdents64] = (unsigned long) new_getdents64;
    protect_memory();

    printk (KERN_INFO "New address:\n");
    printk (KERN_INFO "   __NR_getdents   at %p\n", (void*) __sys_call_table[__NR_getdents]);
    printk (KERN_INFO "   __NR_getdents64 at %p\n", (void*) __sys_call_table[__NR_getdents64]);

    return 0;
}

/*

*/
static void __exit code_exit(void)
{
    unprotect_memory();
    __sys_call_table[__NR_getdents]   = (unsigned long) ptr_getdents;
    __sys_call_table[__NR_getdents64] = (unsigned long) ptr_getdents64;
    protect_memory();
}

module_init(code_init);
module_exit(code_exit);