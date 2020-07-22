/*
    Kernel Thread 
    Archive of Reversing.ID
    Based on Linux Kernel Module Programming Guide

    Tested on Linux Kernel 4.9.0-11-amd64 (Debian)



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

#include <linux/kthread.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Reversing.ID");

/*
    Linux merupakan kernel multithreading. Kemampuan multithreading juga tersedia
    bagi Kernel Module melalui fasilitas Kernel Thread. Thread digunakan untuk
    secara bersamaan (concurrent) menjalankan task / routine tertentu.

    Setiap thread dapat menerima parameter.

    Setiap thread dijadwalkan oleh kernel seperti thread lainnya.
*/


/*
    Routine yang akan dipanggil untuk menjalankan task tertentu.

    Setiap Kernel Thread (kthread) memiliki sebuah object task_struct yang 
    mengacu kepada task saat ini. Task tersebut dapat diakses melalui identifier 
    "current" yang dapat berbeda antar thread.

    current->comm adalah nama command yang menciptakan thread ini
    current->pid  adalah process yang ditetapkan untuk menjalankan thread ini.
*/
static int thread_routine (void * arg)
{
    printk (KERN_INFO "revid: thread %s pid %d\n", current->comm, current->pid);
    return 0;
}


static int __init code_init(void)
{
    struct task_struct * task1;
    struct task_struct * task2;
    int errc;

    printk (KERN_INFO "revid: starting 2 threads\n");

    task1 = kthread_run (thread_routine, NULL, "thread-1");
    if (IS_ERR(task1))
    {
        printk (KERN_INFO "revid: cannot create thread task1\n");
        errc = PTR_ERR(task1);
        task1 = NULL;
        return errc;
    }

    task2 = kthread_run (thread_routine, NULL, "thread-2");
    if (IS_ERR(task1))
    {
        printk (KERN_INFO "revid: cannot create thread task2\n");
        errc = PTR_ERR(task1);
        task1 = NULL;
        return errc;
    }

    printk (KERN_INFO "revid: main thread %s pid %d\n", current->comm, current->pid);

    return 0;
}

static void __exit code_exit(void)
{

}

module_init(code_init);
module_exit(code_exit);