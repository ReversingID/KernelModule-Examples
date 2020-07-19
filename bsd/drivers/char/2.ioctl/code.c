/*
    I/O Control
    Archive of Reversing.ID

    Tested on FreeBSD 12.0-RELEASE

    Berkomunikasi dengan device menggunakan IOCTL.

Build:
    $ make

List module:
    $ kldstat

Load:
    $ klload ./revid_kld.ko

Testing:
    $ cc test.c -o test && ./test

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
#include <sys/ioccom.h>


/*
    Device file merepresentasikan perangkat fisik. Selain operasi baca tulis,
    sebagian perangkat membutuhkan penanganan spesifik. Operasi ini 
    dapat diimplementasikan sebagai command melalui mekanisme IOCTL
    untuk berinteraksi spesifik dengan perangkat.

    Command IOCTL adalah user-defined command yang dapat berbeda antar 
    perangkat.

    Secara singkat, interface IOCTL menangani semua operasi yang tidak bisa
    ditangani oleh operasi read/write (bukan transfer data).
*/

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

/* dummy data yang akan di-get/set oleh IOCTL */
int status;


/* ********************* Internal Functions Prototype ********************* */

static d_open_t     dev_open;
static d_close_t    dev_close;
static d_read_t     dev_read;
static d_write_t    dev_write;

static d_ioctl_t    dev_ioctl;

static struct cdevsw fops = {
    .d_version  = D_VERSION,
    .d_open     = dev_open,
    .d_close    = dev_close,
    .d_read     = dev_read,
    .d_write    = dev_write,
    .d_ioctl    = dev_ioctl,
    .d_name     = "revid"
};

/*
    Pendefinisian command IOCTL untuk perangkat.
    Command merupakan sebuah byte, umumnya tersusun terurut dimulai dari
    sebuah command dasar (base).

    Terdapat 4 kemungkinan command: _IO, _IOR, _IOW, _IOWR
    dengan
        R: Read, command mengharapkan adanya pembacaan data
        W: Write, command akan melakukan penulisan data
*/
#define REVID_BASE      'r'
#define REVID_RESET     _IO(REVID_BASE, 0)
#define REVID_STOP      _IO(REVID_BASE, 1)
#define REVID_GSTAT     _IOR(REVID_BASE, 2, int)
#define REVID_SSTAT     _IOW(REVID_BASE, 3, int)

#define REVID_MAXNR     3


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

/*
    parameter cmd adalam command (perintah) yang dikirimkan dari user-space.
    Jika command membutuhkan argumen (misal buffer) maka argumen adakan dilewatkan
    melalui parameter arg.

    Sebelum menerapkan IOCTL, beberapa command harus didefinisikan.
    Setiap command merupakan angka, berurutan mulai dari 0 sebagai command pertama.
*/
static int 
dev_ioctl (struct cdev * dev, u_long cmd, caddr_t data, int fflag, struct thread * td)
{
    int retval = 0;
    int * arg = (int *) data;

    /* proses command */
    switch (cmd)
    {
        case REVID_RESET:
            /* reset perangkat */
            uprintf ("revid: receiving REVID_RESET\n");
            break;
        case REVID_STOP:
            /* hentikan operasi pada perangkat */
            uprintf ("revid: receiving REVID_STOP\n");
            break;
        case REVID_GSTAT:
            /* dapatkan status dari perangkat */
            * arg = status;
            uprintf ("revid: giving status to %d\n", status);
            break;
        case REVID_SSTAT:
            /* set status perangkat, cast arg sebagai nilai yang diharapkan */
            status = * arg;
            uprintf ("revid: receiving status as %d\n", status);
            break;
        default:
            retval = ENOTTY;
            break;
    }
    return retval;
}