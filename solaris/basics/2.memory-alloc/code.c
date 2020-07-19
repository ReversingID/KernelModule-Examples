/*
    Memory Allocation
    Archive of Reversing.ID

    Tested on Solaris 11.4 (amd64)

    Alokasi memory di level kernel

Build:
    $ gcc -D_KERNEL -m64 -mcmodel=kernel -mno-red-zone -ffreestanding
-nodefaultlibs -c code.c
    $ ld -r code.o -o revid

Module info:
    $ kldd revid

Load:
    $ modload revid

Unload:
    $ modunload -i revid
*/

#include <sys/ddi.h>
#include <sys/sunddi.h>
#include <sys/modctl.h>

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

        void * kmem_alloc (size_t size, int flag);
        void   kmem_free (void * cp, size_t size);

    kmem_malloc()   : mengalokasikan ruang memory baru.
    kmem_free()     : membebaskan (hapus) ruang yang sudah ada.
*/

/*
    Flag pada alokasi memory mendeskripsikan karakteristik operasi yang dilakukan.
    Beberapa flag yang sering digunakan:
        KM_SLEEP    : operasi memperbolehkan blocking jika request belum dapat dilakukan.
        KM_NOSLEEP  : operasi akan mengembalikan NULL jika tidak bisa dilakukan segera.
*/

extern struct mod_ops mod_miscops;

static struct modlmisc mm = {
    &mod_miscops,
    "Solaris LKM"
};

static struct modlinkage ml = {
    MODREV_1,
    (void *) &mm,
    NULL
};


int _init (void)
{
    int ret;

    ret = mod_install (&ml);
    if (ret != 0)
        cmn_err (CE_NOTE, "revid: could not install module\n");
    else 
    {
        cmn_err (CE_NOTE, "revid: allocate 32 chars\n");
        buffer = kmem_alloc (32 * sizeof(char), KM_SLEEP);

        cmn_err (CE_NOTE, "revid: allocate at: %p\n", buffer);
    }

    return ret;
}

int _info (struct modinfo * info)
{
    return mod_info(&ml, info);
}

int _fini (void)
{
    int ret;

    ret = mod_remove (&ml);
    if (ret != 0)
        cmn_err (CE_NOTE, "revid: could not install module\n");
    else 
    {
        cmn_err (CE_NOTE, "revid: deallocate memory\n");
        if (buffer)
            kmem_free (buffer, 32);
    }
    
    return ret;
}