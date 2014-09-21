#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include "../include/avtreg.h"
#include <sys/malloc.h>	/* malloc */

main(int argc, char *argv[])
{
u_char	buf[249*13*2];

int	readbuf;
int	arvid;
u_char	count;
u_short	i;
char	*avt_device = "/dev/ravt0";
int	file;
static struct set_rw	SetRW;

	arvid = open(avt_device, O_RDWR, 0);
	if (arvid < 0) {
		printf ("Error open device %s\n", avt_device);
		exit(2);
	};
	file = open(argv[1], O_RDONLY, 0);
	if (file < 0) {
		printf ("Error open file %s\n", argv[1]);
		close(arvid);
		exit(2);
	};
	fprintf(stderr,"Press ENTER ...");	getchar();

	SetRW.rate = 200;
	SetRW.flag = IOIRC;
	ioctl(arvid, VSET_WRITE, &SetRW);
	while(read(file, buf, sizeof(buf))) {
		while (!(write (arvid, buf, sizeof(buf))));
	};

	fprintf(stderr,"Write OK ...");	getchar();
	close(file);
	close(arvid);
}
