#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/param.h>
#include "../include/rsgen.h"
#include "../include/avtreg.h"
#include <sys/malloc.h>	/* malloc */

/*
 * процедуры декодирования и кодирования блока информации.
 */
/* Reed-Solomon Code/Decode */

extern struct _SY XX;
extern int Decoder200 (u_char *data, u_char *code, u_int start, u_int count, u_int group);
extern int Coder200 (u_char *data, u_char *code, u_int start, u_int count, u_int group);
extern int decode200sect (u_char *data, u_char *code, u_int sect);
extern int code200sect (u_char *data, u_char *code, u_int sect);

struct TPB	TapeTPB;
int rate=200;	/* Текущая скорость работы карты */

int load_tpb (u_char *bufraw)
{
int		i;
u_int		crc;
HEADER_G	*header_g;	/* заголовок при 100 К/с */
HEADER_RS	*header_rs;	/* заголовок при 200 К/с */
HEADER_50	*header_50;	/* заголовок при 325 К/с */
#define	NGR	149
u_char		tmp[44];	/* временный буфер */
int		crcok = 0;

	if (rate == 100) {	/* 100K/s */
		}
	if (rate == 200) {	/* 200K/s */
		header_rs = (HEADER_RS *)tmp;
		XX.Decod_eX[0] = 0;
		XX.Decod_eX[1] = 0;
		XX.Decod_eX[2] = 0;
		XX.Decod_eX[3] = 0;
		Decoder200(tmp, bufraw+46, 148, 1, NGR);
		/*
		 * Если при декодировании небыло
		 * фатальных ошибок - продолжаем.
		 */
		if (!(XX.Decod_eX[3])) {
		crc = crc32(tmp, (sizeof(HEADER_RS) - sizeof(u_int))/2);
		if (crc == header_rs->tstsum) {
			TapeTPB.format = header_rs->format;
			for (i=0; i<8; i++)
				TapeTPB.name[i] = header_rs->name[i];
			TapeTPB.ident   = header_rs->ident;
			TapeTPB.length  = header_rs->length;
			TapeTPB.time    = header_rs->tapetime;
			TapeTPB.sector  = header_rs->sector;
			TapeTPB.Nsector = header_rs->nsect;
			crcok = 1;
			}
		else
			crcok = 0; /* crc32 */
			} /* XX->fatal */
		} /* 200 */
/*	if (crc16 (kadr, 1032, 11) == kadr->TPB.TapeCRC)
	{
	 TapeTPB.format  = kadr->TPB.TapeFormat;
	 for (i=0; i<8; i++) TapeTPB.name[i] = kadr->TPB.TapeName[i];
	 TapeTPB.ident   = kadr->TPB.TapeID;
	 TapeTPB.length  = kadr->TPB.TapeTime;
	 TapeTPB.time    = kadr->TPB.Tapetime;
	 TapeTPB.sector  = SECTRNUM(	kadr->TPB.TapeSectr[0],\
	 				kadr->TPB.TapeSectr[1],\
	 				kadr->TPB.TapeSectr[2]);
	 TapeTPB.Nsector = kadr->TPB.TapeNsector;
	}
*/
	return (crcok);
};

/*
 * Основная процедура
 */
main(int argc, char *argv)
{
int	i, count, readbuf;
int	arvid;
int	startdr, endtape;
long	sector;
char	*avt_device = "/dev/ravt0";
u_char	nametdr[8+1+3+1];
int	tdr;
u_char	*p = NULL;
static	u_char	buf[296*13*2];
static	u_char	sect[512];
static struct set_rw	SetRW;

	arvid = open(avt_device, O_RDWR, 0);
	if (arvid<0)
	{
	 printf ("Error open device %s\n", avt_device);
	 exit(2);
	}
	fprintf(stderr,"Press ENTER ...");
	getchar();
	/*
	 * после установки скорости, очередь в карте сбрасывается
	 */
	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(arvid, VSET_READ, &SetRW);
	/*
	 * начнем считывать кадры и декодировать их
	 */
/*	for (count=0; count < 65; count++) { */
	startdr = 0;
	endtape = 0;
	while (!startdr) {
		while (!(readbuf = read (arvid, buf, SIZE200)));
fprintf (stderr,"Read %d\n", readbuf);

		if (load_tpb(buf)) {
			printf ("F:%X, ", TapeTPB.format);
			printf ("N:");	for (i=0; i<8; i++)
			printf ("%c", TapeTPB.name[i]);
			printf (", ");
			printf ("ID:%d, ", TapeTPB.ident);
			printf ("L:%d, ", TapeTPB.length & MASKLEN);
			printf ("T:%d, ", TapeTPB.time);
			printf ("S:%d, ", TapeTPB.sector & MASKSECT200);
			printf ("Sn:%d\n", TapeTPB.Nsector);
			if (TapeTPB.sector & DIRECTORY200) startdr =1;
			if (TapeTPB.length & TAIL_ATTR) endtape = 1;
			}
		}
	/*
	 * Имя ленты совпадает с именем TDR
	 */
	p = nametdr;
	strcpy(nametdr, "12345678.TDR");
	for (i=0; i<8; i++) nametdr[i] = TapeTPB.name[i];

	tdr = open(p, O_WRONLY | O_CREAT, 1444);
 
	while (!endtape & startdr) {
		for (i=0; i < TapeTPB.Nsector; i++) {
fprintf (stderr,"sect %d, %d\n", (TapeTPB.sector & MASKSECT200)+i, tdr);
			decode200sect(sect, buf+46, i);
			write(tdr, sect, 512);
			}
		while (!(readbuf = read (arvid, buf, SIZE200)));
fprintf (stderr,"Read %d\n", readbuf);
		if (load_tpb(buf)) {
			if (TapeTPB.sector & DIRECTORY200) startdr =1;
			if (TapeTPB.length & TAIL_ATTR) endtape = 1;
			}
		}
	close (tdr);
	close (arvid);
}
