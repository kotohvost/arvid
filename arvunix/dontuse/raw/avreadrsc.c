#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include "../include/avtreg.h"
#include <sys/malloc.h>	/* malloc */

/* avt_raw		*bufr; */
unsigned char	buf[296*13*2];
unsigned char	rs[50][152];

main(int argc, char *argv)
{

int	i, c, count, readbuf;
long	sector;

	while (readbuf = fread (buf, 1, sizeof(buf), stdin)) {
 
fprintf (stderr,"Read %d\n", readbuf);

	for (i=0; i<sizeof(rs); i++) rs[0][i] = buf[i+13*2*2];
	for (i=0; i<152; i++)
	 for (c=0; c<44; c++)
		printf ("%c", rs[c][i]);
	}
}
