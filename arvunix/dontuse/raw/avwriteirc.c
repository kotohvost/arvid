#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include <sys/malloc.h>	/* malloc */
#include "avtreg.h"

main(int argc, char *argv) {
u_char	buf[256*8*2];
int		readbuf, arvid;
u_char		count;
u_short		i;
u_char		nodecode;
char		*avt_device = "/dev/ravt0";
struct set_rw	SetRW;

	arvid = open(avt_device, O_RDWR, 0);
	if (arvid<0) {
		printf ("Error open device %s\n", avt_device);
		exit(2);
	}
	SetRW.rate = 200;
	SetRW.mode = IOIRC;
	ioctl(arvid, VSET_WRITE, &SetRW);
	fprintf(stderr,"Press ENTER ...");	getchar();
#if 1
	for (count=0; count < 255; count++) {
		for (i=0; i < sizeof(buf); i++)
			buf[i] = count;
		while(!write(arvid, buf, sizeof(buf)));
		fprintf(stderr, "%d ", count);
	}
#endif
	fprintf(stderr,"Press ENTER ...");	getchar();
	fprintf(stderr, "\n");
	close (arvid);
}
