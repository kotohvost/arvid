#include <stdio.h>
#include <stdlib.h>	/* atoi ... */
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <string.h>	/* bcopy ... */
#include "../../include/avtreg.h"
#include "../../include/rsgen.h"

/* ������� */
extern "C" struct _SY XX;
extern "C" int Decoder200(u_char *data, u_char *code, int start, int count, int group);
extern "C" u_int crc32(u_char *data, u_int count);
extern "C" u_int crc32s(u_char *data, u_int count);

/* ���������� ���������� */
u_int	Starts, Size, Sizes, Ends;
char	*Filename;

/* ����������� ������� � ����� */
int	arvid, file;

/* ����� ��������� ����� */
static struct	TPB	TPB;

/* ����������� ������ */
static	GF sect[512];
struct SSect {
	u_int	sect;	/* ���������� ����� ������� */
	u_char	d[512];	/* ������ ������� */
};
static struct SSect	strsect;

class	ReadSector {
 public:
	u_char	maxsector;	/* ����. ���. �������� � ������� ������ */
 private:
	int	rawbflag;	/* ���� ������������� ������ ReadRaw() */
	u_char	*rawb;		/* ��������� �� RAW ����� */
	u_char	startcode;	/* ��������� �� ������ ������ � ����� */
	u_char	cursector;	/* ������� ������ � ����� */
 public:
	/* ����������� */
		ReadSector() {
		printf("ReadSector constructor\n");
		maxsector = 8;	/* 200 K/s */
		rawb=new u_char[SIZE200];
		rawbflag = 0;
		cursector = 0;
		};
	/* ���������� */
		~ReadSector() {
		printf("ReadSector destructor\n");
		delete rawb;
		};
	/* �� ������ RAW ������ */
	/* � ����������� ���� ��������� � private */
	int	ReadRaw(int device) {
		int	tmp;
		while(!(tmp = read(device, rawb, SIZE200)));
		/* ���� �������� */
		rawbflag = 1;
		return (tmp);
		};
/*	int	Decoder200(GF *, GF *rawb, int, int, int); */
	/* �� ������ ������ ���� � ����� */
	/* � ����������� ���� ��������� � private */
	u_int	FindCode();
	/* �� ������������� ������� sector � ������� ����� */
	/* � ����������� ���� ��������� � private */
	int	DecSector(int sector);
	/*
	 * �� ������ ���������� �������
	 * ��� ������������� ���������� ReadRaw()
	 */
	int	ReadData(int device);
};

/*
 * �� ������ ������ ����� � ���������� TPB
 */
u_int ReadSector :: FindCode()
{
u_char	i;
u_int	crc;
static	u_char	tpb[44];
/* ��������� ����� ��� �������� ����� */
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
printf("Sector is: %u, ", TPB.sector);
				return(startcode);
				}
			}
		startcode += 18;
		i++;
		}
	return (0);
}

/*
 * ������������� ������� � �������� ��� ��
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

	/* ���������� ����� ������� */
	strsect.sect = TPB.sector + sector;
	Decoder200(strsect.d, rawb+startcode, 18*sector, 18, NGR);
	Decoder200(tmp, rawb+startcode, 144+((sector&7) >> 1), 1, NGR);
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
 * �� ������ �������.
 * device - ���������� ��� ������ �����
 */
int ReadSector :: ReadData(int device)
{
	/*
	 * ���� ������� ������ ������ ������������� ���������� ��������
	 * ��� ������ ��������� � �����, �� ������� ��������� �����
	 */
	if ((cursector > maxsector) || (cursector > TPB.Nsector))
		rawbflag = 0;
	/* ����� ������� ��������� ? */
	if (rawbflag == 0) {
	/* �� - ������ ���� */
		ReadRaw(device);
	/* ������������� ����� ������ ������ */
		FindCode();
	/* �������� ������� ������ � ����� */
		cursector = 0;
		};
	/* ���� ������ ���� - ������ */
	if (TPB.Nsector) {
		DecSector(cursector);
		cursector++;
		return (1);
		}
	else	{
	/* ������ - ���� �������� */
	/* ��������� ���������� */
		rawbflag = 0;
		return (0);
		}
}

/*
 * ����� �������. ���������� ����� ������� � ������� �����, ��� -1
 */
int FindSector()
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

/* ��������� ���������� ��������� ������ */
struct	set_rw	SetRW;

static	ReadSector	RS;
u_int	downflag;

/*
 * ��������� �� ����� 8 �������� ����������
 * ���� ��������� ��������� �� ���� ������
 */
u_char	*SectrSeek;

	if (argc != 4) {
		printf("Usage: %s #start #size filename\n", argv[0]);
		exit (1);
		}
/*
 * ��������� ���������� ����������
 */
	Starts = atoi(argv[1]);
	Size = atoi(argv[2]);
	Sizes = Size / 512;
	if ((Sizes * 512) < Size) Sizes += 1;
	Ends = Starts + Sizes - 1;

	Filename = argv[3];
	printf("StartSec: %u, Size: %u, SizeInSec: %u, EndSec: %u, Filename: %s\n",
		 Starts, Size, Sizes, Ends, Filename);
/*
 * ��������� ������
 */
	arvid = open(avt_device, O_RDWR, 0);
	if (arvid < 0) {
		printf("Error open device %s\n", avt_device);
		exit (2);
		}
/*
 * ��������� ���� ��� ������
 * TODO:
 *	������� �������� �� ������������ ����
 *	������� ����� � ���� ������� (???)
 */
	file = open(Filename, O_RDWR|O_CREAT, 0666);
	if (file < 0) {
		printf("Error open file %s\n", Filename);
		exit (3);
		}
	printf("Please press PLAY key in you BM and press ENTER in keyboard\n");
		getchar();
/*
 * ���������� ������ ���������� ������ ��� ��������� ��������
 * ��������� ������ �������� ������ ��������������� ����������
 * "�������"
 */
/*	for (u_int seek=0; seek < Sizes; seek++) write(file, strsect.d, 512); */
/*	printf("lseek: %d\n", lseek(file, 0L, SEEK_SET)); */
	SectrSeek = (u_char *)malloc(Sizes);
	for (u_int seek=0; seek < Sizes; seek++) SectrSeek[seek] = 1;

/* ��������� ���� � �������� ������ */
	SetRW.rate = 200;
	SetRW.flag = IORAW;
	ioctl(arvid, VSET_READ, &SetRW);
/* ������ ����� */
	kadr = -1;
	downflag = 0;
	while (kadr < 0) {
/*		Read = RS.ReadRaw(arvid); */
/*		while(!(Read = read(arvid, code, SIZE200))); */
/*		printf("Read: %d\n", Read); */
/*	write(file, code, Read); */
/*		RS.FindCode(); */
/*		kadr = FindSector(); */
		if (RS.ReadData(arvid)) {
			if ((strsect.sect>=Starts)&&(strsect.sect<=Ends)) {
/*				printf("Sector: %u, ", strsect.sect);
				printf("data: %u\n", (strsect.sect-Starts)*512);
*/				if (SectrSeek[strsect.sect-Starts]) {
					int	tmpsizesect = 512;

					if (Size <= tmpsizesect) {
						tmpsizesect = Size;
						kadr = 1;
						};
					lseek(file, ((strsect.sect-Starts)*512), SEEK_SET);
					write(file, strsect.d, tmpsizesect);
		/* ���� �������� - ������� ��� �������� */
					SectrSeek[strsect.sect-Starts]=0;
					Size -= 512;
					}
				else
					printf("DUP\n");
				downflag = 0;
				}
			}
		if ((TPB.sector > Ends) && (!downflag)) {
			printf("Sorry, start sector skiping.");
			printf("Please press REW key in BM\n");
			printf("Please press ENTER for start\n");
			downflag = 1;
		/* �������� ������� */
/*			SetRW.rate = 200; */
/*			SetRW.flag = IORAW; */
/*			ioctl(arvid, VSET_READ, &SetRW); */
			}
#if 0
/*		Decoder200(data, code+codestart, 0, 149, NGR); */
/*	write(file, data, sizeof(data)); */
/*	write(file, tpb, sizeof(tpb)); */
/*		printf("Tape: "); */
/*		for (Read=0; Read<8; Read++)	printf("%c", TPB.name[Read]); */
#endif
		}
/*
 * ������ ������
 */
/*	printf("Sector: %u\n", strsect.sect); */
/*	write(file, strsect.d, 512); */
/*	write(file, tpb, sizeof(tpb)); */

/*
 * ��������� ���������� ������
 */
	free(SectrSeek);
/*
 * ��������� ���� � �����
 */
	close (file);
	close (arvid);
}

