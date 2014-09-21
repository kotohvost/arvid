#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include "../include/avtreg.h"
#include <sys/malloc.h>	/* malloc */

main(int argc, char *argv)
{
u_char	buf[SIZE200];

int	i, count, readbuf;
int	arvid;
int	endtdr;
long	sector;
char	*avt_device = "/dev/ravt0";
static struct set_rw	SetRW;

	arvid = open(avt_device, O_RDWR, 0);
	if (arvid<0)
	{
	 printf ("Error open device %s\n", avt_device);
	 exit(2);
	}
	SetRW.rate = 200;
	SetRW.flag = IORAW;
fprintf(stderr,"%d",	ioctl(arvid, VSET_READ, &SetRW));
	fprintf(stderr,"Press ENTER ...");
	getchar();
	for (count=0; count < 20; count++) {
		while(!(readbuf=read (arvid, buf, sizeof(buf))));
fprintf (stderr,"Read %d\n", readbuf);
		for (i=0; i < sizeof(buf); i++)	printf ("%c", buf[i]);
		} 
	close (arvid);
}
