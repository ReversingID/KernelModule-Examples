/*
    Hello
    Archive of Reversing.ID

    Tested on FreeBSD 12.0-RELEASE

    Alur sederhana module dengan menampilkan pesan "Hello World!" saat inisialisasi
    dan "Goodbye World!" saat terminasi.

Build:
    $ make

List module:
    $ kldstat

Load:
    $ klload ./revid_kld.ko

Unload:
    $ klunload revid_kld

*/

#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>


/*
    Kernel Module membutuhkan sebuah fungsi entry point untuk menangani
    event yang terjadi kepada module (load, unload, dsb).

    Sembarang fungsi yang akan menjadi entrypoint terlebih dahulu didaftarkan
    melalui DECLARE_MODULE().

    Umumnya entrypoint akan mengidentifikasi event yang datang dan eksekusi
    aktivitas yang sesuai.
*/

static int event_handler (struct module * m, int evt_type, void *arg)
{
    int retval = 0;

    switch (evt_type)
    {
        case MOD_LOAD:      /* terjadi saat module disisipkan ke kernel */
            uprintf("Hello World!\n");
            break;

        case MOD_UNLOAD:    /* terjadi saat module dibongkar dari kernel */
            uprintf("Goodbye World!\n");
            break;
        
        case MOD_SHUTDOWN:  /* terjadi saat shutdown */
        case MOD_QUIESCE:   /* terjadi sesaat sebelum module akan dibongkar */

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