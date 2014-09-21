#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "string.h"
/*
#include "io.h"
#include "alloc.h"
*/

/*
 * ��������� VCR �����
 */
struct vcrfile {
	u_char	a[0];		/* ���� ��� ������������� */
	double	ID;		/* +8 � ����� ��� Version Id = 2*/
	u_char	b[0x10];
	u_char	Power[6];
	u_char	c[0x1a];
	struct	perehodto {
		u_char	str[6];
		};
	struct	perehodto	FromStop[10];
	struct	perehodto	FromPlay[10];
	struct	perehodfr {
		u_char	st[6];
		u_char	pl[6];
		};
	struct	perehodfr	ToStPl[10];
	u_char	z[0x5c];
	ushort	rep;		/* +184 ������� ���������� */
	ushort	nrep;		/* ���-�� �������� ���������� */
	ushort	nkey;		/* ���-�� �������. ������ */
	u_char	key[0x32];	/* +18A ��������� �� ����� ������ ������ */
	ushort	period;		/* +1BC ������ ������� � ��� */
	struct	type {
		u_short	paket[11];
		u_short	pause[11];
		}	type;
	u_char	zz[0x7AC];
};
struct vcrfile vcrfile;

/*
 * ��������� ��� ����������
 */
struct	XXX {
	char	kadr;		/* +12h */
	char	flag;		/* +14h   - ���� ����� */
	ushort	*keybuf;	/* +1ah   - ����� ���������� ���� */
	ushort	kod;		/* +1eh   - ������� ��� ��� ���������� */
	u_int	kodsize;	/* ������� ������ ���� � ����� */
	ushort	mod;		/* +20h   - ��������� */
	u_int	bitsize;	/* +22/24 - ������� ������� � ����� � ������ */
	ushort	sbit;		/* +28h   - ������� ����� ���� � ������ */
	ushort	*buf;		/* ����� ������ ��� ������ */
	ushort	bufn;		/* +2ah   - ������� ����p ����� � ������ */
	char	nkadr;		/* +2eh */
};
static struct XXX	sconv;

/*
 * ��������� ����� �������� ���� � �������
 */
ushort	keybuf[0xfe];

/*
 * Uncompress
 * �������� ����������:
 * XXXXxxxx - ���
 *	XXXX - �����
 *	xxxx - �����
 */
void VCRUncompress (ushort *buf, int key)
{
u_char	n, p;
int	i;

	for (i=0; i < 0x68; i++) {
		p = vcrfile.a[0x23a + key*0x68 + i];
		n = p & 0x0f;
		if (n == 0x0f) break;
		*buf++ = vcrfile.type.paket[n];
		n = p >> 4;
		if (n == 0x0f) break;
		*buf++ = vcrfile.type.pause[n];
		};
	buf = 0;			/* ��������� */
};

static	ushort	IRC_OFF[] = {
	0xffff, 0xfffe, 0xfffc, 0xfff8, 0xfff0, 0xffe0, 0xffc0, 0xff80,
	0xff00, 0xfe00, 0xfc00, 0xf800, 0xf000, 0xe000, 0xc000, 0x8000 };
static	ushort	IRC_ON[] = {
	0x0000, 0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f,
	0x00ff, 0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff };

void IRCToMod (ushort ax, ushort dx, ushort cx)
{
unsigned char	dh, dl;
ushort	bx, bp;

	bp = ax;
	dl = dx;
	dh = dx;
	dh >>= 1;
	dl -= dh;
	if (cx == 0) goto L1B75;
	if (cx < 0x10) goto L1B65;
	printf("..");
	if ((cx -= 0x10) == 0) goto L1B75;
L1B65:
	cx &= 0x0f;
	bx = cx;
	bx <<= 1;
	printf("%X.", IRC_OFF[bx]);
	cx -= 0x10;
L1B75:
	cx += dl;
	ax = cx;
	if ((ax >>= 4) == 0) goto L1B8C;
L1B81:
	printf("FFFF.");
	ax--;
	if (ax > 0) goto L1B81;
L1B8C:
	cx &= 0x0f;
	bx = cx;
	if (cx == 0) goto L1B9E;
	bx <<= 1;
	printf("%X.", IRC_ON[bx]);
	cx -= 0x10;
L1B9E:
	cx += dh;
	ax = cx;
	if ((ax >>= 4) == 0) goto L1BB5;
L1BAA:
	printf("0000.");
	ax--;
	if (ax > 0) goto L1BAA;
L1BB5:
	cx &= 0x0f;
	bx = cx;
	if (cx == 0) goto L1BC7;
	bx <<= 1;
	printf("%X.", IRC_OFF[bx]);
	cx -= 0x10;
L1BC7:
	bp--;
	if (bp > 0) goto L1B75;
};

main (arg, argv)
int	arg;
char	*argv[];
{
int	handle;
int	keynum, i;
ushort	kod;		/* ������� ��� ��� ���������� */

	if (arg!=2) {
	/* ��� ����� �� �������� */
		printf("\nUsage: %s keynum\n", argv[0]);
		exit(1);
		}
	/* handle = open (argv[1], O_RDONLY|O_BINARY ); */
	handle=open("akair110.vcr\0", O_RDONLY, 0);

	/* �������� �������? */
	if (handle == -1) {
		printf("\nError opening %s\n", argv[1]);
		exit(2);
		}
	/* ������� ��������� � ��������� � ��������� */
	if ( read(handle, &vcrfile, sizeof(vcrfile)) != sizeof(struct vcrfile)) {
		printf("\nRead error %s\n", argv[1]);
		exit(1);
		}

	/* ���������� ������ */
	for (i=0; (vcrfile.a[0x18a+i] != 0xff & i < 49) ; i++);
	printf("������������ ������: %d\n", i);

	keynum=atoi (argv[1]);
	/* �� 0 �� i-1 */
	if (keynum >= i) {
		printf("\nError key num %d\n", keynum);
		exit (3);
		}

	bzero(&keybuf, sizeof(keybuf));
	/* uncompress */
	VCRUncompress(keybuf, keynum);

	printf("\n");
printf("size: 996 = %x\n", sizeof(vcrfile));
printf("start: %x\n", &vcrfile);
printf("power: %x\n", &vcrfile.Power[0]);
printf("FrSt: %x\n", &vcrfile.FromStop[0]);
printf("FrPl: %x\n", &vcrfile.FromPlay[0]);
printf("ToSP: %x\n", &vcrfile.ToStPl[0]);
printf("Rep: %x\n", &vcrfile.rep);
printf("Key: %x\n", &vcrfile.key[0]);
printf("Per: %x\n", &vcrfile.period);

/* ��������� ��������������� ������� � ������������������ ����� */

	sconv.kadr	= 0;		/* ������� */
	sconv.nkadr	= 0;		/* ��� ����� */
	sconv.kod	= 0;		/* ������� */
	sconv.flag	= 1;		/* ������� ��� */
	sconv.keybuf	= keybuf;	/* ����� ������ � �������� */
	if (vcrfile.a[0x238] != 0) {
		sconv.mod = (int)((vcrfile.period * 340) / 238);
		}
	else	{
		sconv.mod = vcrfile.period * 3;
		}
	sconv.mod++;			/* ��������� */
	/* ������ ���������� �������� � ����� ����� */
	sconv.bitsize = (int)(4096 / (sconv.mod * 12)) * (sconv.mod * 12) * 16;
printf("Mod: %x\n", sconv.mod);
printf("BSz: %x\n", sconv.bitsize);

	/* ���� ��������� */
	if (sconv.nkadr != 0) {
		sconv.nkadr = 0;
		sconv.kod = (int) (3000 / sconv.mod);
		}
/* 1624 */
	/* ����� ��� �� ������ */
	if (sconv.kod == 0) {
		sconv.kod = *sconv.keybuf;
		sconv.keybuf++;
		sconv.flag ^= 1;	/* ������������� ���� */
		}
/* 163b */
	/* ����� ������������������ */
	if (sconv.kod == 0) {
		sconv.kadr = 1;
		goto L170D;
		}
/* 1649 */
	/* ���������� ���������� ��� ��� ���� */
	/* ���� ��� = 333�� */
	sconv.kodsize = sconv.kod * sconv.mod;
	if (sconv.kodsize > sconv.bitsize) {
/* 166e */
		/* ��������� �� ����������� ����� */
		kod = sconv.kod;
		}
	else	{
/* 167e */
		kod = sconv.kod;
		}
/* 1681 */
	/* ������� ��� �����?����� */
	if (sconv.flag != 0) {
		/* ������������ ����� */
		}
	else	{
/* 169b */
		if (vcrfile.a[0x184 + 0xb4] == 0) {
		/* ������������ �������������� ������ */
			IRCToMod(kod, sconv.mod, sconv.sbit);
			}
		else	{
/* 16c0 */
		/* ������������ ���������������� ������ */
			}
		}
/* 16d1 */
	/* ������������ */
/* 170D */
L170D:
/*	for (i=0; (keybuf[i] != 0 & i < sizeof(keybuf)); i++) */ {
		printf ("\n%x\n",keybuf[0]);
		}; /* print keybuf */
	printf("\n");

	close(handle);
};
