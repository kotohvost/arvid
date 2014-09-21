#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include "../include/avtreg.h"

struct	avt_info	ptr;
struct	avt_vcr		vcr;

main(int argc, char *argv)
{

int	arvid;
char	*avt_device = "/dev/avt0";

 arvid = open(avt_device, O_WRONLY, 0);
 bzero (&vcr, sizeof(vcr));
 ioctl(arvid, VSET_VCR, &vcr);
 ioctl(arvid, VGET_INFO, &ptr);
    printf ("Type: %d\n", ptr.controller);
    printf ("Port: %x\n", ptr.port);
    printf ("Irq:  %d\n", ffs(ptr.irq)-1);
    printf ("Drq:  %d\n", ptr.drq);
    printf ("MaxPhase: %d\n", ptr.maxphase);
    printf ("MaxRate: %d Kbyte/s\n", ptr.maxrate);
    printf ("Version: %d.%d\n\n", ptr.ver_major, ptr.ver_minor);
    printf ("Flags: %x\n", ptr.flags);

  close (arvid);
}
