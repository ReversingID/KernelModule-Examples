/*
    Memory Allocation
    Archive of Reversing.ID

    Tested on FreeBSD 12.0-RELEASE

    Alokasi memory di level kernel.

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

#include <sys/types.h>
#include <sys/malloc.h>


/*
    Kernel-Space adalah ruang memory yang hanya ditempati oleh kernel dan module.
    Alokasi memory merupakan operasi raw. Semua operasi alokasi dan dealokasi ditangani
    oleh kernel dan module bertanggung-jawab atas segala operasi yang terjadi.

    Kesalahan penanganan memory tidak hanya berdampak bagi module tetapi keseluruhan kernel.
    Pada beberapa kasus tertentu, kerusakan memory (memory corruption) dapat menyebabkan
    Kernel Panic.
*/

char * buffer;

/*
    Seperti halnya alokasi di level user-space, kernel-space menyediakan beberapa
    fungsi untuk alokasi dan dealokasi memory.

        void * malloc   (unsigned long size, struct malloc_type * type, int flags);
        void   free     (void * addr, struct malloc_type * type);
        void   realloc  (void * addr, unsigned long size, struct malloc_type * type, int flags);
        void   reallocf (void * addr, unsigned long size, struct malloc_type * type, int flags);

    malloc()   : mengalokasikan ruang memory baru.
    free()     : membebaskan (hapus) ruang yang sudah ada.
    realloc()  : mengubah ukuran dari ruang memory yang sudah dialokasikan.
    reallocf() : serupa dengan realloc namun membebaskan ruang memory jika alokasi ulang
                 gagal dilakukan.
*/

/*
    Alokasi data membutuhkan informasi malloc_type.
    Struktur malloc_type mendeskripsikan tujuan alokasi memory.
    Tidak ada impact ke performance, hanya dibutuhkan untuk memory profiling dan
    sanity checks.
*/
MALLOC_DECLARE(M_BUFFER);
MALLOC_DEFINE(M_BUFFER, "malloc_buffer", "data buffer by malloc");

/*
    Flags pada alokasi memory mendeskripsikan karakteristik operasi yang dilakukan.
    Beberapa flag yang sering digunakan:

        M_ZERO  : memory yang dialokasi akan diisi dengan nol.
        M_NOWAIT: operasi akan mengembalikan NULL jika tidak bisa dilakukan segera.
        M_WAITOK: operasi akan menunggu hingga resource tersedia (tidak akan NULL).
*/


static int event_handler (struct module * m, int evt_type, void *arg)
{
    int retval = 0;

    switch (evt_type)
    {
        case MOD_LOAD:
            uprintf("revid: alokasikan 32 chars\n");
            buffer = malloc (32 * sizeof(char), M_BUFFER, M_WAITOK);

            uprintf("revid: realokasi 256 chars\n");
            buffer = realloc (buffer, 256 * sizeof(char), M_BUFFER, M_WAITOK);
            break;

        case MOD_UNLOAD:
            uprintf("revid: dealokasi alamat memory\n");
            free(buffer, M_BUFFER);
            break;

        default:
            retval = EOPNOTSUPP;
            break;
    }

    return retval;
}

static moduledata_t kld_data = {
    "revid_kld",
    event_handler,
    NULL
};

DECLARE_MODULE(revid_kld, kld_data, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);