#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include <sys/malloc.h>	/* malloc */

/* avt_raw		*bufr; */
unsigned char	buf[296*9*2];
unsigned char	rs[44][149];

main(int argc, char *argv) {

int	i, c, count, readbuf;
long	sector;

	readbuf = fread (buf, 1, sizeof(buf), stdin);

fprintf (stderr,"Read %d\n", readbuf);

	for (i=0; i<sizeof(rs); i++) rs[0][i] = buf[i+82];

	for (i=0; i<149; i++)
		for (c=0; c<44; c++)
			printf ("%c", rs[c][i]);
}
