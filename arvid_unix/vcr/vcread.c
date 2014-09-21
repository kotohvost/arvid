#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include "../include/avtreg.h"
#include <sys/malloc.h>	/* malloc */

#define NUM	80

u_char	buf[312*12*2*NUM];

main(int argc, char *argv)
{
struct	set_rw	SetRW;
int	i, count, readbuf;
int	arvid, rarvid;
int	endtdr;
long	sector;
char	*avt_device = "/dev/ravt0";

	rarvid = open ( avt_device, O_RDONLY, 0644);
	if (rarvid<0)
	{
	 printf ("Error open device %s\n", avt_device);
	 exit(2);
	}
	fprintf(stderr,"Press ENTER ...");
	fgetc(stdin);
	endtdr = 312*12*2;
	SetRW.rate = 200;
	SetRW.flag = IOIRC;
	ioctl(rarvid, VSET_READ, &SetRW);
	for (count=0; count < endtdr*(NUM-1); count+=readbuf) {
		readbuf = read (rarvid, buf+count, endtdr*(NUM - (count/endtdr)));
/* fprintf (stderr,"Read %d\n", count); */
		} 
fprintf (stderr,"Read %d\n", count);
	{
	 for (i=0; i < count; i++)
		printf ("%c", buf[i]);
	}
	close (rarvid);
}
