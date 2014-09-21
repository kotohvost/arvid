#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include <machine/soundcard.h>
#include "../include/avtreg.h"
#include <sys/malloc.h>	/* malloc */

main(int argc, char *argv) {
u_char	buf[296*13*2];

int	readbuf;
int	arvid;
u_char	count;
u_short	i;
u_char	nodecode;
char	*avt_device = "/dev/ravt0";
int	audio;
char	*audio_device = "/dev/audio";
static struct set_rw	SetRW;

	arvid = open(avt_device, O_RDWR, 0);
	if (arvid<0)
	{
	 printf ("Error open device %s\n", avt_device);
	 exit(2);
	}
	audio = open(audio_device, O_RDONLY, 0);
	if (audio<0)
	{
	 printf ("Error open device %s\n", audio_device);
	 exit(2);
	}
	ioctl(audio, SNDCTL_DSP_RESET);
	i = 44100;
	ioctl(audio, SNDCTL_DSP_SPEED, &i);
	i = 1;
	ioctl(audio, SNDCTL_DSP_STEREO, &i);
	i = AFMT_S16_LE;
	ioctl(audio, SNDCTL_DSP_SETFMT, &i);
	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(arvid, VSET_WRITE, &SetRW);
/*	SetRW.rate = 325;
	SetRW.flag = IORAW;
	ioctl(arvid, VSET_WRITE, &SetRW);
*/	fprintf(stderr,"Press ENTER ...");	getchar();
	for (count=0; count < 255; count++) {
		bzero(buf, sizeof(buf));
/*		for (i=0; i<sizeof(buf); i++) buf[i] = i+count; */
		read(audio, buf, sizeof(buf));
		while (!(write (arvid, buf, sizeof(buf))));
/* getchar(); */
		}
		close (audio);
	close (arvid);
}
