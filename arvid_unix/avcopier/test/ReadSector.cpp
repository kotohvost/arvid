#include <stdio.h>
#include <stdlib.h>	/* atoi ... */
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <string.h>	/* bcopy ... */
#include "../../include/avtreg.h"
#include "../../include/rsgen.h"

/* Декодер */
extern "C" struct _SY XX;
extern "C" int Decoder200(u_char *data, u_char *code, int start, int count, int group);
extern "C" u_int crc32(u_char *data, u_int count);
extern "C" u_int crc32s(u_char *data, u_int count);

/* Общая структура ленты */
static struct	TPB	TPB;

/* прочитанный сектор */
static	GF sect[512];
struct SSect {
	u_int	sect;	/* физический номер сектора */
	u_char	d[512];	/* данные сектора */
};
static struct SSect	strsect;

/*
 * ПП поиска начала кадра и заполнения TPB
 */
u_int ReadSector :: FindCode()
{
u_char	i;
u_int	crc;
static	u_char	tpb[44];
/* Структура ленты для текущего кадра */
HEADER_RS	*header_rs;

	startcode = 10;
	i = 0;
	while (i < 3) {
		header_rs = (HEADER_RS *)tpb;
		XX.Decod_eX[3] = 0;
		Decoder200(tpb, rawb+startcode, 148, 1, NGR);
/*		printf("Err: %d\n", XX.Decod_eX[3]); */
		if (!(XX.Decod_eX[3])) {
			crc = crc32(tpb, (sizeof(HEADER_RS)-sizeof(u_int))/2);
			if (crc == header_rs->tstsum) {
				TPB.format	= header_rs->format;
				for (i=0; i < 8; i++)
				 TPB.name[i]	= header_rs->name[i];
				TPB.ident	= header_rs->ident;
				TPB.length	= header_rs->length & MASKLEN;
				TPB.flags	= header_rs->length & 0xf000;
				TPB.time	= header_rs->tapetime;
				TPB.sector	= header_rs->sector & MASKSECT200;
				TPB.Nsector	= header_rs->nsect;
				TPB.flags	|=header_rs->sector & DIRECTORY200;
				return(startcode);
				}
			}
		startcode += 18;
		i++;
		}
	return (0);
}

/*
 * Декодирование сектора и проверка его КС
 */
int ReadSector :: DecSector(int sector)
{
int	i;
GF	tmp[44];
u_int	crc, crcs;
typedef	struct {
	u_int	add[6];
} ADD_BLOCK;
ADD_BLOCK	*crcsp;

	/* физический номер сектора */
	strsect.sect = TPB.sector + sector;
	Decoder200(strsect.d, rawb+startcode, 18*sector, 18, NGR);
	Decoder200(tmp, rawb+startcode, 144+((sector&7) >> 1), 1, NGR);
	bcopy(tmp+8*(sector&1), strsect.d+504, 8);
	crc = crc32s(strsect.d, 512/2);
	crcsp = (ADD_BLOCK *)tmp;	/* + 16 + 4*(i&1)); */
	crcs = crcsp->add[4+(sector&1)];
	if (crc == crcs) return (1);
	return (0);
}

/*
 * ПП чтения сектора.
 * device - дескриптор для чтения кадра
 */
int ReadSector :: ReadData(int device)
{
	/*
	 * если текущий сектор больше максимального количества секторов
	 * или больше имеющихся в кадре, то следует пополнить буфер
	 */
	if ((cursector > maxsector) || (cursector > TPB.Nsector))
		rawbflag = 0;
	/* буфер следует пополнить ? */
	if (!rawbflag) {
	/* да - читаем кадр */
		ReadRaw(device);
	/* устанавливаем адрес начала данных */
		FindCode();
	/* обнуляем текущий сектор в кадре */
		cursector = 0;
		};
	/* если сектор есть - читаем */
	if (TPB.Nsector) {
		DecSector(cursector);
		cursector++;
		return (1);
		}
	else	{
	/* ошибка - нету секторов */
	/* установим требование */
		rawbflag = 0;
		return (0);
		}
}
