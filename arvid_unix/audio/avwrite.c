#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include <machine/soundcard.h>
#include "../include/avtreg.h"
#include "../include/rsgen.h"

extern int Coder200 (u_char *data, u_char *code, u_int start, u_int count, u_int group);

main(int argc, char *argv)
{
u_char	buf[SIZE200];
u_char	aud[1764*2];

int	readbuf;
int	arvid;
u_char	count;
u_short	i;
u_char	nodecode;
char	*avt_device = "/dev/ravt0";
int	audio;
char	*audio_device = "/dev/audio";
static struct set_rw	SetRW;

	arvid = open(avt_device, O_WRONLY, 0);
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
	fprintf(stderr,"Press ENTER ...");	getchar();
	ioctl(audio, SNDCTL_DSP_RESET);
	i = 44100;
	ioctl(audio, SNDCTL_DSP_SPEED, &i);
	i = 1;
	ioctl(audio, SNDCTL_DSP_STEREO, &i);
/*	i = 4096;
	ioctl(audio, SNDCTL_DSP_SETFRAGMENT, &i);
*/	i = AFMT_S16_LE;
	ioctl(audio, SNDCTL_DSP_SETFMT, &i);
/*	SetRW.rate = 325;
	SetRW.flag = IORAW;
	ioctl(arvid, VSET_WRITE, &SetRW);
*/	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(arvid, VSET_WRITE, &SetRW);
	while(1) {
		bzero(buf, sizeof(buf));
		read(audio, aud, sizeof(aud));
		Coder200(aud, buf+46, 0, 126, NGR);
/*		bcopy(aud, buf, sizeof(aud)); */
		while (!(write (arvid, buf, sizeof(buf))));
		} 
	close (audio);
	close (arvid);
}
