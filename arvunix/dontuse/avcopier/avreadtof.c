#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include "../include/avtreg.h"
#include "../include/rsgen.h"

/* Декодер */
extern struct _SY XX;
extern int Decoder200(GF *data, GF *code, int start, int count, int group);

/*
 * Глобальные переменные:
 * Starts	- стартовый сектор
 * Size		- размер в байтах
 * Sizes	- размер в секторах
 * Ends		- конечный сектор
 * Filename	- фаил для вывода
 */
u_int	Starts, Size, Sizes, Ends;
char	*Filename;
/* дескрипторы девайса и файла вывода */
int	arvid, file;

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
u_int FindCode(GF *code)
{
u_char	startcode, i;
u_int	crc;
static	u_char	tpb[44];
/* Структура ленты для текущего кадра */
HEADER_RS	*header_rs;

	startcode = 10;
	i = 0;
	while (i < 3) {
		header_rs = (HEADER_RS *)tpb;
		XX.Decod_eX[3] = 0;
		Decoder200(tpb, code+startcode, 148, 1, NGR);
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
int DecSector(GF *code, int sector)
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
	Decoder200(strsect.d, code, 18*sector, 18, NGR);
	Decoder200(tmp, code, 144+((sector&7) >> 1), 1, NGR);
	bcopy(tmp+8*(sector&1), strsect.d+504, 8);
	crc = crc32s(strsect.d, 512/2);
	crcsp = (ADD_BLOCK *)tmp;	/* + 16 + 4*(i&1)); */
	crcs = crcsp->add[4+(sector&1)];
/*		printf("CRC = %X %X\n", crc, crcs); */
	if (crc == crcs) return (1);
/*		write(file, sect, sizeof(sect)); */
	return (0);
}

/*
 * Поиск сектора. Возвращает номер сектора в текущем кадре, или -1
 */
int FindSector(GF *data)
{
int	i;

	printf("KSector: %u\n", TPB.sector);
	for(i=0; i < TPB.Nsector; i++) {
/*		printf("Sector: %u\n", TPB.sector + i); */
/*		DecSector(data, i); */
		if ((TPB.sector+i) == Starts) {
			printf("Start sector found\n");
			return (i);
			}
		}
	return (-1);
}

main(int argc, char *argv[])
{
int	i, kadr;
char	*avt_device = "/dev/ravt0";

/* структура управления скоростью чтения */
struct	set_rw	SetRW;

/* Буферы для чтения и декодирования данных */
static	GF data[NRS*NGR];
static	GF code[SIZE200];
u_char	codestart;	/* переменная указывающая на начало кода в кадре */
u_int	crc;

	if (argc != 4) {
		printf("Usage: %s #start #size filename\n", argv[0]);
		exit (1);
		}
/*
 * Заполняем глобальные переменные
 */
	Starts = atoi(argv[1]);
	Size = atoi(argv[2]);
	Sizes = Size / 512;
	if ((Sizes * 512) < Size) ;
	else Sizes += 1;
	Ends = Starts + Sizes;

	Filename = argv[3];
	printf("StartSec: %u, Size: %u, SizeInSec: %u, EndSec: %u, Filename: %s\n",
		 Starts, Size, Sizes, Ends, Filename);
/*
 * Открываем девайс
 */
	arvid = open(avt_device, O_RDWR, 0);
	if (arvid < 0) {
		printf("Error open device %s\n", avt_device);
		exit (2);
		}
/*
 * Открываем фаил для вывода
 * TODO:
 *	сделать проверку на существующий фаил
 *	вывести маску в фаил конфига (???)
 */
	file = open(Filename, O_RDWR|O_CREAT, 0666);
	if (file < 0) {
		printf("Error open file %s\n", Filename);
		exit (3);
		}
	printf("Please press PLAY key in you BM and press ENTER in keyboard\n");
		getchar();
/* Установим фазу и скорость чтения */
	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(arvid, VSET_READ, &SetRW);
/* Читаем кадры */
	kadr = -1;
	while (kadr < 0) {
		int Read;

		while(!(Read = read(arvid, code, SIZE200)));
/*		printf("Read: %d\n", Read); */
/*	write(file, code, Read); */
		if (codestart = FindCode(code)) {
			kadr = FindSector(code+codestart);
			}
		if (TPB.sector > Starts) {
			printf("Sorry, start sector skiping.");
			printf("Please press REW key in BM\n");
			printf("Please press ENTER for start\n");
			getchar();
		/* сбросить очередь */
			SetRW.rate = 200;
			SetRW.flag = IORAW;
			ioctl(arvid, VSET_READ, &SetRW);
			}
/*		Decoder200(data, code+codestart, 0, 149, NGR); */
/*	write(file, data, sizeof(data)); */
/*	write(file, tpb, sizeof(tpb)); */
/*		printf("Tape: "); */
/*		for (Read=0; Read<8; Read++)	printf("%c", TPB.name[Read]); */
		}
/*
 * Сектор найден
 */
	DecSector(code+codestart, kadr);
	printf("Sector: %u\n", strsect.sect);
	write(file, strsect.d, 512);
/*	write(file, tpb, sizeof(tpb)); */

/*
 * Закрываем фаил и арвид
 */
	close (file);
	close (arvid);
}

#if 0
/*
 * ПП чтения одного сектора
 * сама должна учитывать что буфер опустел и требуется дополнительное чтение
 * data - буфер для чтения
 */
ReadSect(u_char *data)
{
int	ReadKadr;	/* глобальная переменная указывающая на неоходимость
			   чтения данных из драйвера */
	if (ReadKadr) read(arvid, 
}
#endif