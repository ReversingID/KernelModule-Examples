/*
    Read-Write
    Archive of Reversing.ID
    Based on Linux Kernel Module Programming Guide

    Tested on Linux Kernel 4.9.0-11-amd64 (Debian)

    Menangani operasi baca dan tulis pada device.

Build:
    $ make

Module info:
    $ modinfo code.ko

List module:
    $ lsmod

Load:
    $ insmod code.ko

Testing:
    $ cat /dev/revid
    $ echo "Reversing.ID" > /dev/revid
    $ cat /dev/revid

Unload:
    $ rmmod code
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Reversing.ID");
MODULE_DESCRIPTION("Example of Char Driver (Linux)");


/* ****************** Type Definition & Data Declaration ****************** */

#define DEV_NAME    "revid"
#define CLASS_NAME  "revid"
#define BUFF_LEN    256

struct devdata_t 
{
    struct cdev dev;
    size_t size;
    u8   * buffer;

    /* Deklarasikan data di sini */
    int    open;
};

struct devdata_t device;

static int  major = 0;
static struct class *  revid_class = NULL;      /* daftarkan class sebagai /sys/class/revid/cdev */
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
    cdev_add(&device.dev, MKDEV(major, 0), 1);

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


    /* alokasikan buffer untuk baca/tulis */
    device.buffer = kmalloc(BUFF_LEN * sizeof(u8), GFP_KERNEL);
    if (! device.buffer)
    {
        printk (KERN_ALERT "Unable to allocate buffer for device\n");

        device_destroy (revid_class, MKDEV(major, 0));
        class_unregister (revid_class);
        class_destroy (revid_class);
        unregister_chrdev_region (MKDEV(major, 0), 1);
        return -ENOMEM;
    }

    device.open = 0;
    device.size = 0;

    return 0;
}

static void __exit code_exit(void)
{
    if (device.buffer)
    {
        kfree(device.buffer);
        device.buffer = NULL;
    }

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
    struct devdata_t * data;

    data = container_of (inode->i_cdev, struct devdata_t, dev);
    file->private_data = data;

    data->open ++;
    printk (KERN_INFO DEV_NAME ": device has been opened %d times\n", data->open);

    return 0;
}

/*
    Dipanggil untuk membebaskan / melepas resource yang digunakan.
    Dalam hal ini tidak ada resource yang dialokasikan.
*/
static int 
dev_release (struct inode * inode, struct file * file)
{
    return 0;
}

/*
    Dipanggil untuk menangani proses pembacaan data dari file.
    Data dikirimkan dari device ke user.
*/
static ssize_t 
dev_read (struct file * file, char __user * buffer, size_t size, loff_t * offset)
{
    struct devdata_t * data;
    ssize_t errc, retval;

    /* private_data menunjuk ke data file devdata_t */
    data = (struct devdata_t*) file->private_data;

    /* transfer data */
    errc = copy_to_user (buffer, data->buffer + *offset, data->size);

    /* sukses membaca semua data */
    if (errc == 0)
    {
        /* kosongkan buffer */
        retval = data->size;
        printk (KERN_INFO "sent %zu characters to user\n", data->size);
        data->size = 0;

        return retval;
    }
    /* sukses membaca sebagian data */
    else 
    {
        printk (KERN_INFO "sent %zd characters to user\n", errc);
        return -EFAULT;
    }
}

/*
    Dipanggil untuk menangani proses penulisan data ke file.
    Data dikirimkan dari user ke device.
*/
static ssize_t 
dev_write (struct file * file, const char __user * buffer, size_t size, loff_t * offset)
{
    struct devdata_t * data;

    /* private_data menunjuk ke data file devdata_t */
    data = (struct devdata_t*) file->private_data;

    if (size > BUFF_LEN)
        size = BUFF_LEN - 1;

    /* transfer data */
    copy_from_user (data->buffer + *offset, buffer, size);
    data->size = size;

    printk (KERN_INFO "receive %zu characters from the user\n", size);

    /* return jumlah bytes yang ditulis */
    return size;
}