/*
    Archive of Reversing.ID

    File sederhana untuk menguji operasi IOCTL
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/* deklarasi command yang dipahami oleh device */
#define REVID_BASE      'r'
#define REVID_RESET     _IO(REVID_BASE, 0)
#define REVID_STOP      _IO(REVID_BASE, 1)
#define REVID_GSTAT     _IOR(REVID_BASE, 2, int)
#define REVID_SSTAT     _IOW(REVID_BASE, 3, int)

#define REVID_MAXNR     3


/* prototipe fungsi */
void ioctl_reset(int fd);
void ioctl_stop(int fd);
int  ioctl_getstatus(int fd);
void ioctl_setstatus(int fd, int status);


int main(void)
{
    int status;

    /* open device */
    int fd = open("/dev/revid", O_RDWR);
    if (fd < 0)
    {
        printf("error on opening /dev/revid\n");
        exit(0);
    }

    printf("executing REVID_RESET ...\n");
    ioctl_reset(fd);
    getchar();

    printf("executing REVID_STOP ...\n");
    ioctl_stop(fd);
    getchar();

    printf("executing REVID_GSTAT ...\n");
    status = ioctl_getstatus(fd);
    printf("   status = %d\n", status);
    getchar();

    status ++;
    
    printf("executing REVID_SSTAT ...\n");
    printf("   status = %d\n", status);
    ioctl_setstatus(fd, status);
    getchar();

    printf("all command successful ...\n");

    close(fd);

    return 0;
}

void ioctl_reset(int fd)
{
    int retval;

    retval = ioctl (fd, REVID_RESET);
    if (retval < 0)
    {
        printf("revid-test: ioctl REVID_RESET failed: %d\n", retval);
        exit(-1);
    }
}

void ioctl_stop(int fd)
{
    int retval;

    retval = ioctl (fd, REVID_STOP);
    if (retval < 0)
    {
        printf("revid-test: ioctl REVID_STOP failed: %d\n", retval);
        exit(-1);
    }
}

int  ioctl_getstatus(int fd)
{
    int retval;
    int status;

    retval = ioctl (fd, REVID_GSTAT, &status);
    if (retval < 0)
    {
        printf("revid-test: ioctl REVID_GSTAT failed: %d\n", retval);
        exit(-1);
    }

    return status;
}

void ioctl_setstatus(int fd, int status)
{
    int retval;

    retval = ioctl (fd, REVID_SSTAT, &status);
    if (retval < 0)
    {
        printf("revid-test: ioctl REVID_SSTAT failed: %d\n", retval);
        exit(-1);
    }
}
