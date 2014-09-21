#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include "../include/avtreg.h"
#include <sys/malloc.h>	/* malloc */

main(int argc, char *argv[])
{
u_char	buf[296*13*2];

int	readbuf;
int	arvid;
u_char	count;
u_short	i;
char	*avt_device = "/dev/ravt0";
int	file;
static struct set_rw	SetRW;

	if (argc != 2)
		return(1);

	arvid = open(avt_device, O_RDWR, 0);
	if (arvid<0) {
		printf ("Error open device %s\n", avt_device);
		exit(2);
		};
	file = open(argv[1], O_RDONLY, 0);
	if (file < 0) {
		printf ("Error open file %s\n", argv[1]);
		exit(2);
		};
#if 0
	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(arvid, VSET_WRITE, &SetRW);
#else
	SetRW.rate = 325;
	SetRW.flag = IORAW;
	ioctl(arvid, VSET_WRITE, &SetRW);
#endif
	fprintf(stderr,"Press ENTER ...");	getchar();
	for (count=0; count < 255; count++) {
		bzero(buf, sizeof(buf));
#if 1
		for (i=0; i<sizeof(buf); i++) buf[i] = i & 0xf2;
		/* +count; */
#else
		read(file, buf, sizeof(buf));
#endif
		while (!(write (arvid, buf, sizeof(buf))));
/* getchar(); */
		};
	close (file);
	close (arvid);
}
