/*
    Registration
    Archive of Reversing.ID
    Based on Linux Kernel Module Programming Guide
    
    Tested on Linux Kernel 4.9.0-11-amd64 (Debian)

    Mendaftarkan device dengan major dan minor number

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
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/device.h>

#include <linux/cdev.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Reversing.ID");
MODULE_DESCRIPTION("Example of Char Driver (Linux)");

/*
    Di Unix, device (character/block) diakses melalui file. Sebuah LKM menangani
    operasi-operasi yang terjadi terhadap device tersebut. Untuk dapat digunakan, 
    operasi-operasi yang berupa sekumpulan fungsi ini terlebih dahulu harus 
    didaftarkan sebagai "hook" ke kernel.
*/

/* ****************** Type Definition & Data Declaration ****************** */

#define DEV_NAME    "revid"
#define NDEVS  5

struct devdata_t 
{
    struct cdev dev;
    size_t size;
    u8   * buffer;

    /* Deklarasikan data di sini */

};
/* buat pool 5 minor yang dikelola LKM ini */
struct devdata_t devices[NDEVS];

static int  major = 0;
module_param(major, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);


/* ********************* Internal Functions Prototype ********************* */

static int      dev_open    (struct inode * inode, struct file * file);
static int      dev_release (struct inode * inode, struct file * file);
static ssize_t  dev_read    (struct file * file, char __user * buffer, size_t size, loff_t * offset);
static ssize_t  dev_write   (struct file * file, const char __user * buffer, size_t size, loff_t * offset);

static const struct file_operations fops = 
{
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .release = dev_release,
    .read    = dev_read,
    .write   = dev_write,
};


/* ****************** Loadable Kernel Module Initialize ******************* */

static int __init code_init(void)
{
    int i,  result;
    dev_t   dev;

    /* 
    register sejumlah NDEVS device yang akan ditangani.

    major number harus diketahui di awal.
    */
    if (major)
    {
        dev = MKDEV(major, 0);
        result = register_chrdev_region(dev, NDEVS, DEV_NAME);
    }
    /* 
    alternatifnya, alokasikan device secara dinamis.
    alokasikan NDEVS minor dimulai dari minor 0
    */
    else 
    {
        result = alloc_chrdev_region(&dev, 0, NDEVS, DEV_NAME);
        major  = MAJOR(dev);
    }

    if (result != 0)
    {
        printk(KERN_ALERT "revid: registering char device failed with %d\n", result);
        return result;
    }

    /* 
    inisialisasi setiap device file untuk minor yang berbeda.

    dalam contoh ini, jumlah device dialokasikan secara statis.
    */
    for (i = 0; i < NDEVS; i++)
        /* inisialisasi struktur cdev */
        cdev_init(&devices[i].dev, &fops);

    /* tambahkan character device ke system */
    cdev_add(&devices[i].dev, MKDEV(major, 0), NDEVS);

    /* letakkan kode inisialisasi di sini */


    return 0;
}

static void __exit code_exit(void)
{
    int i;

    /* release semua minor */
    for (i = 0; i < NDEVS; i++)
    {
        cdev_del(&devices[i].dev);
    }

    /* berlaku untuk kedua macam registrasi device */
    unregister_chrdev_region(MKDEV(major, 0), NDEVS);
}

module_init(code_init);
module_exit(code_exit);


/* ******************* Internal Functions Implementation ******************* */

/*
    Dipanggil untuk menginisialisasi perangkat dan resource.
*/
static int 
dev_open (struct inode * inode, struct file * file)
{
    /* Letakkan kode spesifik di sini */

    /* return 0 jika open sukses */
    return 0;
}

/*
    Dipanggil untuk membebaskan / melepas resource yang digunakan.
    Dalam hal ini tidak ada resource yang dialokasikan.
*/
static int 
dev_release (struct inode * inode, struct file * file)
{
    /* Letakkan kode spesifik di sini */

    /* return 0 jika release sukses */
    return 0;
}

/*
    Dipanggil untuk menangani proses pembacaan data dari file.
    Data dikirimkan dari device ke user.
*/
static ssize_t  
dev_read (struct file * file, char __user * buffer, size_t size, loff_t * offset)
{
    /* Letakkan kode spesifik di sini */

    /* return nilai yang dibaca */
    return 0;
}

/*
    Dipanggil untuk menangani proses penulisan data ke file.
    Data dikirimkan dari user ke device.
*/
static ssize_t  
dev_write (struct file * file, const char __user * buffer, size_t size, loff_t * offset)
{
    /* Letakkan kode spesifik di sini */

    /* return nilai yang ditulis */
    return 0;
}