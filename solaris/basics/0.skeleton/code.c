/*
    Solaris LKM Skeleton
    Archive of Reversing.ID

    Tested on Solaris 11.4 (amd64)

    Kerangka kode Solaris Kernel Module sesuai standard terkini.
    Template kosong.

Build:
    $ gcc -D_KERNEL -m64 -mcmodel=kernel -mno-red-zone -ffreestanding
-nodefaultlibs -c code.c
    $ ld -dy -r code.o -o revid

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


/* ****************** Type Definition & Data Declaration ****************** */

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


/* ********************* Internal Functions Prototype ********************* */


/* *************************** Helper Functions *************************** */


/* ****************** Loadable Kernel Module Initialize ******************* */

int _init (void)
{
    return mod_install (&ml);
}

int _info (struct modinfo * info)
{
    return mod_info (&ml, info);
}

int _fini (void)
{
    return mod_remove (&ml);
}



/* ******************* Internal Functions Implementation ******************* */