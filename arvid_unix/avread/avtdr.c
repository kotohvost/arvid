#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include <machine/avtreg.h>
#include <sys/malloc.h>	/* malloc */

avt_kadr	data;
avt_kadr	*bufr;
u_short		buf[8*1056];

main(int argc, char *argv)
{

int	i, count, readbuf;
int	arvid, rarvid;
int	endtdr;
long	sector;
char	*avt_device = "/dev/ravt0";

	bufr = (void *)malloc (sizeof(buf), M_TEMP, M_NOWAIT);
	if (!bufr)
	{
	 printf ("Error buffer allocate\n");
	 exit(1);
	}
	rarvid = open ( avt_device, O_RDONLY, 0);
	if (rarvid<0)
	{
	 printf ("Error open device %s\n", avt_device);
	 exit(2);
	}
	
	endtdr = 1;
	while (endtdr)
	{
nextread:
	readbuf = read (rarvid, bufr, sizeof(buf));
	if (!readbuf) goto nextread;
 
	bcopy (bufr, &buf, readbuf);
fprintf (stderr,"Read %d\n", readbuf);
/* ждем блок с ТДР заголовком */
	count = 0;
	while (count<readbuf)
	{
	bcopy (&buf[count], &data, sizeof(data));
	fprintf (stderr,"\n%d\n", data.TPB.Tapetime);
	fprintf (stderr,"%d\n", SECTRNUM(data.TPB.TapeSectr[0],\
					data.TPB.TapeSectr[1],\
					data.TPB.TapeSectr[2]));
	if (data.TPB.TapeFlag != TDRBLOCK) goto nextcount;
	
	if (data.TPB.TapeFlag == TDRBLOCK)
	{
	 for (i=0; i < 1024; i++) printf ("%c%c", data.data[i], data.data[i] >> 8);
	}
nextcount:
	count+=2112;
	}
	} /* while (endtdr) */

	close (rarvid);
/*	close (file); */
}
