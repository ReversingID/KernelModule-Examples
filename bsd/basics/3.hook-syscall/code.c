/*
    Hook Syscall
    Archive of Reversing.ID

    Tested on FreeBSD 12.0-RELEASE

    Hook callback untuk menggantikan syscall tertentu.

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

#include <sys/syscall.h>

#if defined(__FreeBSD__)
#include <sys/sysent.h>
#endif


/*
    Syscall (system call) adalah layanan yang disediakan oleh kernel
    kepada program di user-space untuk menangani operasi-operasi low-level
    yang umumnya berkaitan langsung dengan abstraksi resource.

    Syscall dikelompokkan dalam sebuah tabel (Syscall Table). Setiap Syscall
    diidentifikasi dengan ID berupa bilangan bulat. Setiap entry pada 
    tabel merupakan alamat yang merujuk kepada handler yang sesuai.

    Permintaan syscall oleh user-space program dilakukan dengan melakukan
    interupsi. Eksekusi berpindah dari user-space ke kernel-space. 
    Kernel kemudian mengidentifikasi ID dan mengeksekusi handler yang sesuai
    dari tabel syscall.
*/

/*
    Hook dilakukan terhadap:

    SYS_mkdir
    buat direktori baru

    int mkdir (const char * path, mode_t mode);
*/

/* pointer ke syscall table */
struct sysent * table;

/* pointer ke syscall original */
typedef int (* _o_mkdir_t ) (struct thread * td, void * args);

_o_mkdir_t  ptr_mkdir;

/* syscall akan memanggil fungsi ini */
static int 
new_mkdir (struct thread * td, void * args)
{
    int retval = ptr_mkdir(td, args);

    /* proses data di sini */

    return retval;
}


/* Dapatkan alamat syscall table */
static void *
get_syscall_table_addr(void)
{
    return &sysent[0];
}


static int event_handler (struct module * m, int evt_type, void *arg)
{
    int retval = 0;

    switch (evt_type)
    {
        case MOD_LOAD:
            table = (struct sysent *) get_syscall_table_addr();
            if (! table)
                retval = -1;
            else 
            {
                ptr_mkdir = (_o_mkdir_t) table[SYS_mkdir].sy_call;

                uprintf("revid: Syscall table at %p\n", table);
                uprintf("    SYS_mkdir at %p\n", table[SYS_mkdir].sy_call);
                
                table[SYS_mkdir].sy_call = (void *) new_mkdir;

                uprintf("revid: New address:\n");
                uprintf("    SYS_mkdir at %p\n", table[SYS_mkdir].sy_call);
            }
            break;

        case MOD_UNLOAD:
            table[SYS_mkdir].sy_call = (void *) ptr_mkdir;
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