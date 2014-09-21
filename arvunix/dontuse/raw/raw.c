#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include "../include/avtreg.h"

static struct set_rw	SetRW;
/* avt_raw	raw; */

main(int argc, char *argv)
{
int	i;
int	arvid;
char	*avt_device = "/dev/avt0";

	arvid = open ( avt_device, O_RDWR, 0);
fprintf ( stderr, "Ready... press RETURN"); getchar ();

	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(arvid, VSET_READ, &SetRW);
/*    for (i=0; i < RamSize; i++) printf ("%c", raw.data[i]); */
/*	for (i=0; i < 0x100; i++) printf ("%c", raw.data[i]); */
fprintf ( stderr, "Ready... press RETURN"); getchar ();
    close(arvid);
}
