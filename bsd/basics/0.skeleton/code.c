/*
    FreeBSD KLD Skeleton
    Archive of Reversing.ID

    Tested on FreeBSD 12.0-RELEASE

    Kerangka kode FreeBSD KLD (Kernel Loadable) sesuai standard terkini.
    Template kosong.

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


/* ****************** Type Definition & Data Declaration ****************** */


/* ********************* Internal Functions Prototype ********************* */


/* *************************** Helper Functions *************************** */


/* ****************** Loadable Kernel Module Initialize ******************* */

static int event_handler (struct module * m, int evt_type, void *arg)
{
    int retval = 0;

    switch (evt_type)
    {
        case MOD_LOAD:
            break;

        case MOD_UNLOAD:
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


/* ******************* Internal Functions Implementation ******************* */