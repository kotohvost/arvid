#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include "avtreg.h"

main(int argc, char *argv) {
u_char	buf[8192];
char	fname[255];			int	fd;
int	i, count, readbuf;
char	*avt_device = "/dev/ravt0";	int	arvid;
struct set_rw	SetRW;
int		phase;

	arvid = open(avt_device, O_RDWR, 0);
	if (arvid < 0) {
		printf ("Error open device %s\n", avt_device);
		exit(2);
	}
	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(arvid, VSET_READ, &SetRW);
	sleep(1);
	phase = 2;
	ioctl(arvid, VSET_PHASE, &phase);
	bzero(fname, sizeof(fname));
/*	fprintf(stderr,"Press ENTER ..."); getchar(); */

/*	for (count=0; count < 65; count++)
		while(!(readbuf=read(arvid, buf, sizeof(buf))));
*/
	for (count=0; count < 40; count++) {
		bzero(buf, sizeof(buf));
		while (!(readbuf=read(arvid, buf, sizeof(buf))));
		sprintf(fname, "200_%d", count);
fprintf(stderr,"Read %d, %s\n", readbuf, fname);
		if ((fd=open(fname, O_WRONLY|O_CREAT, 0644)) > 0) {
			write(fd, buf, sizeof(buf));
			close(fd);
		}
	/*	for (i=0; i < sizeof(buf); i++)	printf ("%c", buf[i]); */
	} 
	close (arvid);
}
