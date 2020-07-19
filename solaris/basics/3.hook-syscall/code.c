/*
    Hook Syscall
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

#include <sys/syscall.h>

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


    Pada Solaris, syscall disimpan sebagai array systent[].
    ID untuk setiap syscall dapat dilihat pada /usr/include/sys/syscall.h
*/

/*
    Hook dilakukan terhadap:

    SYS_read
    Mendapatkan data dari sebuah file yang terbuka.

    int read (int fd, void * buf, size_t nbyte);
*/

/* pointer ke syscall table */
struct sysent * table;

/* pointer ke syscall original */
typedef int (* _o_read_t) (int fd, void * buf, size_t nbyte);

_o_read_t ptr_read;

/* syscall akan memanggil fungsi ini */
static int 
new_read (int fd, void * buf, size_t nbyte)
{
    int retval = ptr_read(fd, buf, nbyte);

    /* proses data di sini */

    return retval;
}


/* Dapatkan alamat syscall table */
static void *
get_syscall_table_addr(void)
{
    return &sysent[0];
}


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
        table = (struct sysent *) get_syscall_table_addr();
        if (table)
        {
            ptr_read = (_o_read_t) table[SYS_read].sy_callc;

            uprintf("revid: Syscall table at %p\n", table);
            uprintf("    SYS_read at %p\n", table[SYS_read].sy_callc);
            
            table[SYS_read].sy_callc = (void *) new_read;

            uprintf("revid: New address:\n");
            uprintf("    SYS_read at %p\n", table[SYS_read].sy_callc);
        }
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
        table[SYS_read].sy_callc = (void *) ptr_read;
    }
    
    return ret;
}