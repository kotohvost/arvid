#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/soundcard.h>
#include "../include/avtreg.h"
#include "../include/rsgen.h"

extern int Decoder200(u_char *data, u_char *code, u_int start, u_int count, u_int group);
extern struct _SY XX;

main(int argc, char *argv)
{
u_char	buf[SIZE200];
u_char	aud[1764*2];
u_char	startcode;

int	i;
int	arvid;
char	*avt_device = "/dev/ravt0";
static struct set_rw	SetRW;
int	audio;
char	*audio_device = "/dev/audio";

	arvid = open(avt_device, O_RDONLY, 0);
	if (arvid<0)
	{
	 printf ("Error open device %s\n", avt_device);
	 exit(2);
	}
	audio = open(audio_device, O_WRONLY, 0);
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
	i = AFMT_S16_LE;
	ioctl(audio, SNDCTL_DSP_SETFMT, &i);
/*	SetRW.rate = 325;
	SetRW.flag = IORAW;
	ioctl(arvid, VSET_READ, &SetRW);
*/	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(arvid, VSET_READ, &SetRW);
	for(i=0; i < 3000; i++) {
		while (!(read(arvid, buf, sizeof(buf))));
/*		write(audio, buf, sizeof(buf)); */
		startcode = 10;
		while (startcode >= 46) {
			XX.Decod_eX[3] = 0;
			Decoder200(aud, buf+startcode, 0, 1, NGR);
			if (!(XX.Decod_eX[3])) goto exitfind;
			startcode += 18;
			}
exitfind:
	XX.Decod_eX[0] = XX.Decod_eX[1] = XX.Decod_eX[2] = XX.Decod_eX[3] = 0;
		Decoder200(aud, buf+startcode, 0, 126, NGR);
printf("1: %d, 2: %d, 3: %d, f: %d\n", XX.Decod_eX[0],
	XX.Decod_eX[1], XX.Decod_eX[2], XX.Decod_eX[3]);
		write(audio, aud, sizeof(aud));
/*		bzero(buf, sizeof(buf)); */
		}
	close (audio);
	close (arvid);
}
