/*
    Device Class
    Archive of Reversing.ID
    Based on Linux Kernel Module Programming Guide

    Tested on Linux Kernel 4.9.0-11-amd64 (Debian)

    Mendaftarkan class (/sys/class) & device (/dev)

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
    Linux mengelola device node melalui udev. Proses penyisipan (insert) 
    dan pembongkaran (remove) module dapat dilakukan secara otomatis 
    berdasarkan major dan minor number yang sesuai. Dengan mendaftarkan
    class dan device, entry untuk perangkat akan secara otomatis diciptakan
    tanpa perlu membuat node secara manual.

    Directory /sys merupakan bagian dari sysfs yang berisi informasi device
    berdasarkan kategori tertentu. Pseudo filesystem sysfs menawarkan interface
    ke komponen dan data struktur di kernel. Secara spesifik /sys/class 
    berisi informasi Device Class yang telah didaftarkan ke system. Device
    Class merupakan kelompok device dengan karakteristik serupa (misal: terminal,
    network device, block device, graphic device, sound device, dsb).
*/

/* ****************** Type Definition & Data Declaration ****************** */

#define DEV_NAME    "revid"
#define CLASS_NAME  "revid"

struct devdata_t 
{
    struct cdev dev;
    size_t size;
    u8   * buffer;

    /* Deklarasikan data di sini */

};

struct devdata_t device;

static int  major = 0;
static struct class  * revid_class = NULL;
static struct device * revid_dev  = NULL;


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

/*
    Untuk menyederhanakan, registrasi device dilakukan secara dinamis.
    Major number akan ditentukan oleh kernel.
*/
static int __init code_init(void)
{
    dev_t dev;
    int result;

    /* alokasikan major number untuk device. */
    result = alloc_chrdev_region(&dev, 0, 1, DEV_NAME);
    if (result != 0)
    {
        printk(KERN_ALERT "Registering char device failed with %d\n", result);
        return result;
    }

    major  = MAJOR(dev);

    /* inisialisasi device */
    cdev_init(&device.dev, &fops);
    cdev_add (&device.dev, MKDEV(major, 0), 1);

    /* register device Class ke /sys/class */
    revid_class = class_create (THIS_MODULE, CLASS_NAME);
    if (IS_ERR (revid_class))
    {
        printk (KERN_ALERT "Failed to register device class\n");

        unregister_chrdev_region (MKDEV(major, 0), 1);
        return PTR_ERR (revid_class);
    }
    printk (KERN_INFO "device class registered correctly");

    /* register device driver ke /dev */
    revid_dev   = device_create (revid_class, NULL, MKDEV(major, 0), NULL, DEV_NAME);
    if (IS_ERR (revid_dev))
    {
        printk (KERN_ALERT "Failed to create the device\n");

        class_unregister (revid_class);
        class_destroy (revid_class);
        unregister_chrdev_region (MKDEV(major, 0), 1);
        return PTR_ERR (revid_dev);
    }
    printk (KERN_INFO "device class registered correctly");

    return 0;
}

static void __exit code_exit(void)
{
    if (! IS_ERR (revid_dev))
        device_destroy (revid_class, MKDEV(major, 0));
    
    if (! IS_ERR(revid_class))
    {
        class_unregister (revid_class);
        class_destroy (revid_class);
    }

    if (major)
        unregister_chrdev_region (MKDEV(major, 0), 1);
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