#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include "../../include/avtreg.h"
#include <sys/malloc.h>	/* malloc */

u_short	Bbuf[237*13*4];

void init(u_short *buf) {
u_short	*pbuf;
int	nk, str;

	pbuf = buf;
	for (nk = 0; nk < 4; nk++) {
		memset(pbuf, 0xffff, 237*13*2);
		for (str = 0; str < 237; str++)
			pbuf[str*13] = 0x75;
		pbuf[13] &= 0xffbf;
		pbuf[237*13-13] &= 0xffdf;
		pbuf += 237 * 13;
	}
	pbuf = buf;
	for (nk = 0; nk < 13; nk++) {
		*pbuf = 0x79;
		pbuf += 10;
	}
	*pbuf |= 0x08;
	buf[0] &= 0xffbf;
	buf[10] &= 0xffdf;
};

main(int argc, char *argv[])
{
u_char	buf[237*13*2];

int	readbuf;
int	arvid;
u_char	count;
u_short	i;
char	*avt_device = "/dev/ravt0";
int	file, fdout;
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
	fdout = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0640);
	fprintf(stderr,"Press ENTER ...");	getchar();
	init(Bbuf);
	if (fdout >= 0)
		write(fdout, Bbuf, sizeof(Bbuf));

	SetRW.rate = 200;
	SetRW.mode = IOIRC;
	ioctl(arvid, VSET_WRITE, &SetRW);
	while(read(file, buf, sizeof(buf))) {
		for (i=0; i<sizeof(buf); i++)
			buf[i] ^= 0xff;
		if (fdout >= 0)
			write(fdout, buf, sizeof(buf));
		while (!(write(arvid, buf, sizeof(buf))));
	};

	fprintf(stderr,"Write OK ...");	getchar();
	if (fdout >= 0)
		close(fdout);
	close(file);
	close(arvid);
}
