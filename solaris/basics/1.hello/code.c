/*
    Hello
    Archive of Reversing.ID

    Tested on Solaris 11.4 (amd64)

    Alur sederhana module dengan menampilkan pesan "Hello World!" saat inisialisasi
    dan "Goodbye World!" saat terminasi.

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
    Kernel Module membutuhkan setidaknya dua fungsi, yaitu _init() sebagai entrypoint 
    (inisialisasi) dan _fini() sebagai terminasi (pembersihan module). Fungsi _init() 
    akan dipanggil ketika module dimuat (load) ke kernel, sementara fungsi _fini() 
    akan dipanggil ketika module dibongkar (unload) atau dikeluarkan dari kernel.
    Selain itu terdapat fungsi _info() yang digunakan untuk memberikan informasi
    tentang module ketika introspeksi dilakukan (misal menggunakan modinfo).
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
        cmn_err (CE_NOTE, "Hello World!\n");

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
        cmn_err (CE_NOTE, "Goodbye World!\n");
    
    return ret;
}