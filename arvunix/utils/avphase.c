#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include "avtreg.h"

main(int argc, char *argv[]) {
int	arvid;
u_char	phase;
char	*avt_device = "/dev/ravt0";

	arvid = open(avt_device, O_WRONLY, 0);

	if (argc > 1) {
		sscanf(argv[1], "%d", &phase);
		printf("Set phase: %d\n", phase);
		ioctl(arvid, VSET_PHASE, &phase);
	}
	ioctl(arvid, VGET_PHASE, &phase);
	printf ("Phase: %d\n", phase);
	close (arvid);
}
