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

/* ����� ��������� ����� */
static struct	TPB	TPB;

/* ����������� ������ */
static	GF sect[512];
struct SSect {
	u_int	sect;	/* ���������� ����� ������� */
	u_char	d[512];	/* ������ ������� */
};
static struct SSect	strsect;

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
	if (crc == crcs) return (1);
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
	if (!rawbflag) {
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
