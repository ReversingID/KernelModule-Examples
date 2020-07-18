/*
    Disk Geometry
    Archive of Reversing.ID
    Based on Linux Kernel Module Programming Guide

    Tested on Linux Kernel 4.9.0-11-amd64 (Debian)

    Mendapatkan informasi geometri disk.

Build:
    $ make

Module info:
    $ modinfo code.ko

List module:
    $ lsmod

Load:
    $ insmod code.ko

Testing:
    $ fdisk -l /dev/revida

Unload:
    $ rmmod code
*/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/device.h>

#include <linux/fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Reversing.ID");
MODULE_DESCRIPTION("Example of Block Driver (Linux)");


/* ****************** Type Definition & Data Declaration ****************** */

#define DEV_NAME        "revid"
#define KSECTOR_SIZE    512
#define NR_SECTORS      1024
#define NDEVS           16

/* parameter module */
static int  major;                          /* Major number yang digunakan device driver */
static int  sector_size = KSECTOR_SIZE;     /* Ukuran sebuah sector */
static int  nsectors = NR_SECTORS;          /* Jumlah sector yang ada */

/* representasi internal dari perangkat fiktif */
struct devdata_t 
{
    unsigned long size;             /* ukuran "disk" */
    spinlock_t lock;                /* spin-lock untuk mutex */
    u8 * data;                      /* data di kernel-space yang dialokasikan sebanyak "size" bytes */
    struct gendisk * gd;
    struct request_queue * queue;
};
static struct devdata_t    device;
static struct hd_geometry  hdgeo;

module_param (major,        int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
module_param (sector_size,  int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
module_param (nsectors,     int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);


/* ********************* Internal Functions Prototype ********************* */

/* penanganan I/O request */
static void dev_transfer (struct devdata_t * dev, sector_t sector, unsigned long nsect, char * buffer, int write);
static void dev_request  (struct request_queue * queue);

/* dapatkan geometry dari device */
static int  dev_getgeo   (struct block_device * bd, struct hd_geometry * geo);

static struct block_device_operations fops = 
{
    .owner  = THIS_MODULE,
    .getgeo = dev_getgeo
};

static int  setup_block_device  (struct devdata_t * dev, int dev_nr);
static void delete_block_device (struct devdata_t * dev);


/* ****************** Loadable Kernel Module Initialize ******************* */

static int __init code_init(void)
{
    int result;

    printk(KERN_INFO "revid: arg sector: %d\n", nsectors);
    printk(KERN_INFO "revid: arg block size: %d\n", sector_size);
    printk(KERN_INFO "revid: arg major: %d\n", major);

    /* register block device, dapatkan major number */
    result = register_blkdev (major, DEV_NAME);
    if (result < 0)
    {
        printk (KERN_ALERT "revid: registering block device failed with %d\n", result);
        return result;
    }

    major = result;
    printk (KERN_INFO "revid: register major = %d\n", major);

    return setup_block_device(&device, 0);
}

static void __exit code_exit(void)
{
    delete_block_device (&device);
    unregister_blkdev (major, DEV_NAME);
}

module_init(code_init);
module_exit(code_exit);


/* ******************* Internal Functions Implementation ******************* */

/*
    Helper function untuk melakukan setup terhadap Block Device.
    - setup internal device
    - ciptakan request queue
    - alokasi buffer

    setup juga mendaftarkan handler untuk menangani request ke disk.
    
    setiap disk device direpresentasikan melalui struktur gendisk.
*/
static int  
setup_block_device (struct devdata_t * dev, int dev_nr)
{
    memset (dev, 0, sizeof (struct devdata_t));

    /* setup internal device */
    spin_lock_init (&dev->lock);
    dev->size   = nsectors * sector_size;
    dev->data   = vmalloc (dev->size);
    if (dev->data == NULL)
    {
        printk (KERN_ALERT "revid: unable to allocate %ld memory\n", dev->size);
        return -ENOMEM;
    }

    /* setup HD geometry untuk digunakan dalam dev_getgeo() */
    hdgeo.heads     = 16;
    hdgeo.sectors   = 32;
    hdgeo.start     = 0;
    hdgeo.cylinders = dev->size / (KSECTOR_SIZE * hdgeo.heads * hdgeo.sectors);

    /* mendapatkan request queue */
    dev->queue = blk_init_queue(dev_request, &dev->lock);
    if (dev->queue != NULL)
    {
        blk_queue_logical_block_size (dev->queue, sector_size);

        /* setup struktur gendisk */
        dev->gd = alloc_disk(NDEVS);
        if (dev->gd)
        {
            dev->gd->major = major;
            dev->gd->first_minor = dev_nr * NDEVS;
            dev->gd->fops  = &fops;
            dev->gd->queue = dev->queue;
            dev->gd->private_data = dev;

            snprintf(dev->gd->disk_name, 32, "revid%c", 'a' + dev_nr);
            set_capacity(dev->gd, nsectors);
            add_disk(dev->gd);

            return 0;
        }
        unregister_blkdev(major, DEV_NAME);
    }

    vfree(dev->data);
    dev->data = NULL;

    return -ENOMEM;
}

/*
    Helper function untuk pembersihan Block Device.
    - hapus gendisk
    - hapus queue
    - dealokasi buffer 
*/
static void 
delete_block_device (struct devdata_t * dev)
{
    if (dev->gd)
    {
        del_gendisk (dev->gd);
        put_disk (dev->gd);
    }
    
    if (dev->queue)
        blk_cleanup_queue(dev->queue);
    
    if (dev->data)
        vfree (dev->data);
}


/*
    Dipanggil untuk menangani proses transfer data.
*/
static void 
dev_transfer (struct devdata_t * dev, sector_t sector, unsigned long nsect, char * buffer, int write)
{
    
}

/*
    Dipanggil untuk menangani request yang diberikan kepada disk.

    Terdapat beberapa macam request, namun kita hanya tertarik kepada request
    untuk transfer data.
*/
static void 
dev_request  (struct request_queue * q)
{
    struct request * req;
    struct devdata_t * dev;

    req = blk_fetch_request(q);
    while (req != NULL)
    {
        /* periksa apakah req merupakan command */
        if (req == NULL || req->cmd_type != REQ_TYPE_FS)
        {
            printk (KERN_NOTICE "revid: skip non-fs request\n");
            __blk_end_request_all(req, -EIO);
            break;
        }

        dev = req->rq_disk->private_data;

        /* operasikan data yang dilewatkan melalui struktur bio (Block I/O) */
        dev_transfer (dev, blk_rq_pos(req), blk_rq_cur_sectors(req), bio_data(req->bio), rq_data_dir(req));

        if (! __blk_end_request_cur(req, 0))
            req = blk_fetch_request(q);
    }
}

/* 
    Dipanggil untuk mendapatkan ukuran geometry dari disk.

    Perintah HDIO_GETGEO ditangani oleh blkdev_ioctl() yang kemudian 
    akan memanggil fungsi ini.
*/
static int 
dev_getgeo (struct block_device * bd, struct hd_geometry * geo)
{
    /* Tidak ada geometry real di "disk" ini, sehingga data dummy diberikan. */
    geo->cylinders  = hdgeo.cylinders;
    geo->heads      = hdgeo.heads;
    geo->sectors    = hdgeo.sectors;
    geo->start      = hdgeo.start;

    return 0;
}