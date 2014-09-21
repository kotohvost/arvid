#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include "../include/avtreg.h"

struct	avt_info	ptr;

main(int argc, char *argv)
{

int	arvid;
char	*avt_device = "/dev/avt0";

  arvid = open ( avt_device, O_WRONLY, 0);
    printf("Open status: %ud\n", arvid);

    printf ("Read info\n");
    ioctl( arvid, VGET_INFO, &ptr);
    printf ("Type:\tArvid %d\n", ptr.controller);
    printf ("Port:\t%x\n", ptr.port);
    printf ("Irq:\t%d\n", ffs(ptr.irq)-1);
    printf ("Drq:\t%d\n", ptr.drq);
    printf ("MaxPhase:\t%d\n", ptr.maxphase);
    printf ("SetPhase:\t%d\n", ptr.phase);
    printf ("MaxRate:\t%d Kbyte/s\n", ptr.maxrate);
    printf ("SetRate:\t%d Kbyte/s\n", ptr.rate);
    printf ("Version:\t%d.%d\n\n", ptr.ver_major, ptr.ver_minor);
    printf ("Flags:\t%x\n", ptr.flags);

  close (arvid);
}
