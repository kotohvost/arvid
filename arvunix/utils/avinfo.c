#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include "avtreg.h"

struct	avt_info	ptr;

main(int argc, char *argv) {
int	arvid;
char	*avt_device = "/dev/ravt0";

	arvid = open(avt_device, O_WRONLY, 0);
	if (arvid < 0) {
		printf("Error open '%s'\n", avt_device);
		return(1);
	}
	if (ioctl(arvid, VGET_INFO, &ptr) < 0) {
		printf("Error ioctl(VGET_INFO)\n");
		close(arvid);
		return(2);
	}
	printf ("Type:\tArvid %d\n", ptr.arvid);
	if (ptr.port)
		printf ("Port:\t\t%x\n", ptr.port);
	if (ptr.drq)
		printf ("Drq:\t\t%d\n", ptr.drq);
	printf ("Irq:\t\t%d\n", ffs(ptr.irq)-1);
	printf ("MaxPhase:\t%d\n", ptr.maxphase);
	printf ("SetPhase:\t%d\n", ptr.phase);
	printf ("MaxRate:\t%d Kbyte/s\n", ptr.maxrate);
	printf ("SetRate:\t%d Kbyte/s\n", ptr.rate);
	printf ("Version:\t%d.%d\n\n", ptr.ver_major, ptr.ver_minor);

	close (arvid);
}
