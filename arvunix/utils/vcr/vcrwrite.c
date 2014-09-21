#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include "../include/avtreg.h"
#include <sys/malloc.h>	/* malloc */

main(int argc, char *argv[])
{
struct	set_rw	SetRW;
int	readbuf, count1, count2;
int	arvid, fdin;
u_short	i;
struct	bbb {
	u_short	d1[3744];
	u_short	d2[14976];
};
struct	bbb	buf;
u_char	nodecode;
char	*avt_device = "/dev/ravt0";

	if (argc < 2) {
		printf("Usage: %s IRC.OUT\n", argv[0]);
		exit (1);
		}
	arvid = open(avt_device, O_WRONLY, 0644);
	if (arvid<0)
	{
	 printf ("Error open device %s\n", avt_device);
	 exit(2);
	}
	fdin = open (argv[1], O_RDONLY, 0644);
	SetRW.rate = 200;
	SetRW.flag = IODATA;
	ioctl (arvid, VSET_WRITE, &SetRW);
	SetRW.rate = 200;
	SetRW.flag = IOIRC;
	ioctl (arvid, VSET_WRITE, &SetRW);
/*	fprintf (stderr,"Press ENTER ..."); 	getchar(); */
	bzero(buf, sizeof(buf));
	while (readbuf=read(fdin, buf.d1, sizeof(buf)))
/*	readbuf=read(fdin, buf.d2, sizeof(buf.d2)); */
		{
		printf("read %d\n", readbuf);
		count1 = count2 = 0;
		while (readbuf) {
			while((count1=write (arvid, buf.d1+count2, readbuf)) < 0);
			readbuf -= count1;
			count2 += count1;
			printf("write %d/%d\n", count1, readbuf);
			}
/*		bzero(buf, sizeof(buf)); */
		};
/*		printf("%d ", write(arvid, buf, 2556*2)); */
/* fprintf (stderr,"Read %d\n", count); */
	fprintf (stderr,"Press ENTER ..."); 	getchar();
	close (fdin);
	close (arvid);
}
