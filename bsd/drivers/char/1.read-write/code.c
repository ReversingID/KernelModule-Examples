/*
    Read-Write
    Archive of Reversing.ID

    Tested on FreeBSD 12.0-RELEASE

    Menangani operasi baca dan tulis pada device.

Build:
    $ make

List module:
    $ kldstat

Load:
    $ klload ./revid_kld.ko

Testing

Unload:
    $ klunload revid_kld

*/

#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>

#include <sys/conf.h>
#include <sys/uio.h>
#include <sys/malloc.h>


/* ****************** Type Definition & Data Declaration ****************** */

#define DEV_NAME    "revid"
#define BUFF_LEN    256

struct devdata_t 
{
    struct cdev * dev;
    size_t length;
    char   buffer[BUFF_LEN];
};

struct devdata_t device;

/* ********************* Internal Functions Prototype ********************* */

static d_open_t     dev_open;
static d_close_t    dev_close;
static d_read_t     dev_read;
static d_write_t    dev_write;

static struct cdevsw fops = {
    .d_version  = D_VERSION,
    .d_open     = dev_open,
    .d_close    = dev_close,
    .d_read     = dev_read,
    .d_write    = dev_write,
    .d_name     = "revid"
};

/* *********************** Kernel Module Initialize *********************** */

static int event_handler (struct module * m, int evt_type, void *arg)
{
    int retval = 0;

    switch (evt_type)
    {
        case MOD_LOAD:
            device.dev = make_dev (&fops, 0, UID_ROOT, GID_WHEEL, 0600, "revid");
            uprintf("Driver loaded\n");
            break;

        case MOD_UNLOAD:
            destroy_dev (device.dev);
            uprintf("Driver unloaded\n");
            break;

        default:
            retval = EOPNOTSUPP;
            break;
    }

    return retval;
}

/*
    Informasi tentang module disimpan ke dalam struktur moduledata_t.
*/
static moduledata_t kld_data = {
    "revid_kld",            /* class name atau nama module */
    event_handler,          /* event handler */
    NULL                    /* pointer ke private data */
};

/*
    Register event handler (dalam moduledata_t)

    DECLARE_MODULE(name, data, sub, order);
        sub menyatakan subsystem kernel, dalam hal ini module merupakan driver
        order menyatakan urutan inisialisasi dalam subsystem.
*/
DECLARE_MODULE(revid_kld, kld_data, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);


/* ******************* Internal Functions Implementation ******************* */

/*
    Dipanggil untuk menginisialisasi perangkat dan resource.
*/
static int 
dev_open (struct cdev * dev, int oflags, int devtype, struct thread * td)
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
dev_close (struct cdev * dev, int fflags, int devtype, struct thread * td)
{
    /* Letakkan kode spesifik di sini */

    /* return 0 jika release sukses */
    return 0;
}

/*
    Dipanggil untuk menangani proses pembacaan data dari file.
    Data dikirimkan dari device ke user.
*/
static int 
dev_read (struct cdev * dev, struct uio * uio, int ioflag)
{
    int retval;
    int length = MIN(uio->uio_resid, 
                (device.length - uio->uio_offset > 0 ? 
                device.length - uio->uio_offset : 0));

    retval = uiomove (device.buffer + uio->uio_offset, length, uio);
    if (retval != 0)
        uprintf ("Write failed.\n");

    return retval;
}

/*
    Dipanggil untuk menangani proses penulisan data ke file.
    Data dikirimkan dari user ke device.
*/
static int 
dev_write (struct cdev * dev, struct uio * uio, int ioflag)
{
    int retval;
    int length = MIN(uio->uio_iov->iov_len, BUFF_LEN - 1);

    /* salin data dari user-space ke buffer di kernel-space */
    retval = copyin (uio->uio_iov->iov_base, device.buffer, length);
    if (retval != 0)
    {
        uprintf ("Write failed.\n");
        return retval;
    }

    device.buffer[length] = 0;
    device.length = length;

    return retval;
}