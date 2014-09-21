/*
 * ����� ������������ ��� ������ � �������� ���������� ��.
 */
#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "string.h"
#include "errormes.h"		// ��������� �� �������

static	ushort	IRC_ON[] = {
	0xffff, 0xfffe, 0xfffc, 0xfff8, 0xfff0, 0xffe0, 0xffc0, 0xff80,
	0xff00, 0xfe00, 0xfc00, 0xf800, 0xf000, 0xe000, 0xc000, 0x8000 };
static	ushort	IRC_OFF[] = {
	0x0000, 0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f,
	0x00ff, 0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff };
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

class VCR {
 public:
	vcrfile	*svcr;		// ��������� �� ������� ���������� ��
	u_char	fvcr;		// ���� ������� ������� ���������
	int	keynums;	// ���������� ������
	u_short	*skey;		// ��������� �� ����� ��� ������������������
	int	word_23e;	// ��������� (???)
	int	IsRepet;	// ���� ������� ����������
	int	vcr_2;		// +02h ??
	u_short	*Sbuffer;	// +06h ����� �����
	int	modesize;	// +08h ����. ����� Arvid'a ��� ������� ���. ��������
	int	kadrsize;	// +0Ah ����. ����� Arvid'a (4096)
	int	numkadr;	// +0Eh	...
	int	vcr_12;		// +12h ??
	int	pause;		// +14h ���� ������� �����
	u_short	*Sbufkod;	// +16h ��� ���� ����� ������
	u_short	*Nbufkod;	// +1Ah ��������� �� ��������� ���
	u_short	kodn;		// +1Eh ��� ��� ��������������
	int	sizeper;	// +20h ����� ������ ������� � �����
	int	bufbit;		// +22h ������ modesize � �����
	int	startflag;	// +26h �����
	int	bit;		// +28h ������� ��� � ��������� �����
	u_short	*bufkadr;	// +2Ah ����� �����
	int	vcr_2C;		// +2Ch ??
	int	vcr_2E;		// +2Eh ??
	int	vcr_30;		// +30h ??
	int	vcr_32;		// +32h ??
	int	vcr_34;		// +34h ??
 public:
	VCR();			// constructor
	~VCR();			// destructor
 int	LoadVCR(char *fname);	// �������� �� �����
 int	KeyNum();		// ���������� ������ � VCR'e
 int	Uncompress (int key);	// ���������� ������������������ �����
 int	FindRepetitor(int key);	// ������� ���������� � �������
 int	VCR_1950();		// ���������� ��������
 void	IRCtoPause(int bit, int sizeper, u_short kod, u_short *buffer);
 void	IRCtoMod(int bit, int sizeper, u_short kod, u_short *buffer);
 void	IRCtoNMod(int bit, int sizeper, u_short kod, u_short *buffer);
 void	CorrectBufBit(int bits);
	// ���������� ��, key - ����� ������� ������. '0'
 int	IrcKeyTrn(int fdout, int key);
	// ���������� ��, key - ���������� �������� �������. 'STOP'
 int	IrcKeyTrn(int fdout, char *key);
};

/*
 * �������� ������� ���������� �� �� ����� (VCR)
 * ��������:
 * KeyNum()
 * ����������:
 * 0 - �������
 */
int VCR::LoadVCR(char *fname) {
int	handle;		// ���������� �����

//	printf("LoadVCR: %s", fname);
	if (!svcr) {
	// ������� ����� ��� �������
//		svcr = (u_char *)malloc(0x996);
		svcr = (vcrfile *)malloc(sizeof(vcrfile));
		};
	// ������� ����
//	printf(" open");
	handle = open(fname, O_RDONLY, 0);
	if (handle < 0) {
		errinf(EOPEN, "LoadVCR: Error open file\n");
		return (EOPEN);
		};
	// ��������� �������
//	printf(" read");
//	if (read(handle, svcr, 0x996) != 0x996) {
	if (read(handle, svcr, sizeof(vcrfile)) != sizeof(vcrfile)) {
		errinf(EREAD, "LoadVCR: Read error file\n");
		return (EREAD);
		};
	close(handle);
	fvcr = 1;
//	printf("\n");
	keynums = KeyNum();
	return (0);
};

/*
 * ����.
 * -1 - ������
 */
int VCR::KeyNum() {
int	key;

	if (!fvcr) {
		errinf(ETABLENONE, "KeyNum: Table not loaded\n");
		return (-1);
		}
	// ������� ������ �� �������
	for (key = 0; (svcr->key[key] != 0xff & key < 49) ; key++);
	return (key);
};

/*
 * Uncompress
 * �������� ����������:
 * XXXXxxxx - ���
 *	XXXX - �����
 *	xxxx - �����
 * 0 - �������
 */
int VCR::Uncompress (int key) {
vcrfile	*nvcr;		// ��������� �� ������� ����������
u_short	*nkey;		// ��������� �� ������������������
u_short	maxpaus=0;	// ������������ ����� �����
u_char	n, p;
int	i, keytmp;

	// �������� � �������� ����� ������������������
	if (key >= keynums) {
		errinf(ENUMKEY, "Uncompress: Key number error\n");
		return(ENUMKEY);
		}
	// ����������� �������
	if (!skey) {
		skey = (u_short *)malloc(0x100 * 2);
		};
	nvcr = svcr;
	nkey = skey;
	// key -> num buffer
	keytmp = svcr->key[key] * 0x68;
//	printf("KeyTmp: %d/%d\n", keytmp, svcr->key[key]);
	for (i=0; i < 0x68; i++) {
		p = svcr->a[0x23A + keytmp + i];
//		printf("%02X.",p);
	// �����
		n = p & 0x0f;
		if (n == 0x0f) break;
		*nkey = svcr->type.paket[n];
//		printf("%04X.", *nkey);
		nkey++;
	// �����
		n = p >> 4;
		if (n == 0x0f) break;
		*nkey = svcr->type.pause[n];
		if (maxpaus < *nkey)
			maxpaus = *nkey;
//		printf("%04X.", *nkey);
		nkey++;
		};
	*nkey = 0;			/* ��������� */
	// ��������� ��������������� �������
	// ��� ���������
	if (!svcr->a[0x238]) {
	int	tmp;
	// ������ � ���.
		tmp = svcr->a[0x1BC] * maxpaus / 1000;
		if (tmp > 82) {
			word_23e = 3;
			}
		else if (tmp > 62) {
			word_23e = 2;
			}
		else if (tmp > 42) {
			word_23e = 1;
			}
		else	{
			word_23e = 0;
			}
		};
	/* ���������� ���������� ��� � ������� */
	if (svcr->a[0x238]) {
	// ���������������� ������
		sizeper = svcr->a[0x1BC] * 340 / 238;
		}
	else	{
	// �������������� ������
		sizeper = svcr->a[0x1BC] * 3;
		};
	sizeper++;
	/* ��������� ������ ���������� �������� � ����� */
	modesize = (kadrsize / (sizeper * 12)) * (sizeper * 12);
	/* ��������� ����� ������ � ���� ������ */
	Sbufkod = skey;
	startflag = 1;
	/* �������� ������� ���������� � ������� */
	IsRepet = FindRepetitor(key);
printf("Rep: %d\n", IsRepet);
	/*
	 * 1795
	 */
	vcr_32 = 0;
	// ��������� �� ����� ����� ������
	if (!Sbuffer) {
		Sbuffer = (u_short *)malloc(0x1000*2);
		};
	// �������
	bzero(Sbuffer, 0x1000*2);
		/*
		 * ����� ������������ ����� � �����
		 * � ���������� 0xEA0 ����.
		 * ����� ��������������� ����� ������ IRC.
		 */
	/* 1932 */
	/* 18B7 */
	/* 1887 */
		vcr_12 = 0;
		Nbufkod = Sbufkod;
		pause = startflag;
		kodn = 0;
		vcr_2C = vcr_2 = 0;
	/* end 1887 */
		numkadr = vcr_30 = 0;
	/* MakeIRCbufbit */
		bufbit = modesize * 16;
		bit = 0;
		bufkadr = Sbuffer;
	/* end MakeIRCbufbit */
		/* ������������� ����� ������� ����� � ����� */
	/* end 18B7 */
		vcr_2E = pause = 1;
		/* ����������������� � ��������� �������. */
	/* end 1932 */
	return (0);
};

/*
 * ����� ���������� � �������. �������� ������ ������ �� �������. !!!
 */
int VCR::FindRepetitor(int key) {
int	i=0;

	while (svcr->a[0x158 + i] != 0xff) {
		if (svcr->a[0x158 + i] == key)
			return (1);
		i++;
		};
	return (0);
};

/*
 * �������������� ������������������ � ������
 */
int VCR::VCR_1950() {
int	bp2;
int	tmpkod;

/*	if (IRCout()) {
		tmp = c_1602();
		c_1A0C();
		bp2++;
		if ((bp2 > 4) || tmp)
			return(tmp);
		}
*/
	/*
	 * 1602
	 */
	while (!vcr_12 && bufbit) {
	// pause for start
		if (vcr_2E) {
			vcr_2E = 0;
			kodn = 3000 / sizeper;
			};
		if (!kodn) {
			kodn = *Nbufkod++;
			pause ^= 1;
			};
	printf("kod: %04X/%08X ", kodn, bufbit);
		if (kodn) {
			if (bufbit < (kodn * sizeper)) {
				tmpkod = bufbit / sizeper;
				}
			else	{
				tmpkod = kodn;
				}
	printf("%04X ", tmpkod);
		// kod to signal
			if (pause) {
		printf("Pause\n");
				IRCtoPause(bit, sizeper, tmpkod, bufkadr);
				}
			else	{
		printf("Signal\n");
				if (!svcr->a[0x238]) {
					IRCtoMod(bit, sizeper, tmpkod, bufkadr);
					}
				else	{
					IRCtoNMod(bit, sizeper, tmpkod, bufkadr);
					}
				} /* pause */
			kodn -= tmpkod;
			bufbit -= tmpkod * sizeper;
			CorrectBufBit(tmpkod * sizeper);
			}
		else	{
			vcr_12 = 1;
			} /* kodn */
		}; /* while */
	/* 170D */
	if (vcr_12 <= 3) {
		if (bufbit) {
			if (vcr_34)
				goto c_1745;
			tmpkod = bufbit / sizeper;
//	printf("kod: %04X/%08X ", kodn, bufbit);
	printf("%04X ", tmpkod);
	printf("Pause\n");
			IRCtoPause(bit, sizeper, tmpkod, bufkadr);
			} /* bufbit */
	/* MakeIRCbufbit */
		bufbit = modesize << 4;	// *16
		bit = 0;
		bufkadr = Sbuffer;
	/* end MakeIRCbufbit */
c_1745:
		if (vcr_12) {
			if (!vcr_34)	{ vcr_12++; }
			else		{ vcr_12 = 3; }
			}
		}
	/* 175B */
	if (vcr_12 == 3)
		return (1);
	return (0);
};

/*
 * ���������� ������� Pause.
 */
void VCR::IRCtoPause(int bit, int sizeper, u_short kod, u_short *buffer) {
int	tmpb, tmpk;

	tmpb = kod * sizeper;
	if (bit) {
		*buffer++ &= IRC_OFF[bit];
		tmpb -= 0x10 - bit;
		};
	tmpk = tmpb >> 4;
	if (tmpb & 0x0f)
		tmpk++;
	bzero(buffer, tmpk << 1);
};

/*
 * ���������� � �������������� ������
 */
void VCR::IRCtoMod(int bit, int sizeper, u_short kod, u_short *buffer) {
int	perdh, perdl, tmpb;

	perdh = sizeper >> 1;
	perdl = sizeper - perdh;
	if (bit) {
		if (bit >= 0x10) {
			buffer++;
			bit -= 0x10;
			}
		if (bit) {
			bit &= 0x0f;
			*buffer++ = IRC_ON[bit];
			bit -= 0x10;
			}
		} /* bit */
	for (; kod; kod--) {
		bit += perdl;
		tmpb = bit >> 4;
		if (tmpb)
			for (; tmpb; tmpb--)
				*buffer++ = 0xffff;
		bit &= 0x0f;
		if (bit) {
			*buffer++ = IRC_OFF[bit];
			bit -= 0x10;
			}
		bit += perdh;
		tmpb = bit >> 4;
		if (tmpb)
			for (; tmpb; tmpb--)
				*buffer++ = 0;
		bit &= 0x0f;
		if (bit) {
			*buffer++ = IRC_ON[bit];
			bit -= 0x10;
			}
		} /* kod */
};

void VCR::IRCtoNMod(int bit, int sizeper, u_short tmpkod, u_short *bufkadr) {
};

void VCR::CorrectBufBit(int bits) {
printf("bufk: %X/%X+%X=", bufkadr, bit, bits/16);
	bit += bits & 0x0f;
	bufkadr += bits/16;
	if (bit >= 0x10) {
		bufkadr++;
		bit &= 0x0f;
		}
printf("%X/%X\n", bufkadr, bit);
};

// ���������� ��, key - ����� ������� ������
int VCR::IrcKeyTrn(int fdout, int key) {
int	stat, buf;
u_short	*sbuf, *nbuf;
#define	IKTBNUM	4

	stat = Uncompress(key);
	if (stat)
		return (stat);
	stat = 1;
//	write (fdout, Sbuffer, modesize * 2);
	sbuf = nbuf = (u_short *)malloc(modesize * 2 * IKTBNUM);
	bzero(sbuf, modesize * 2 * IKTBNUM);
	buf = 0;
	while (stat) {
		if (VCR_1950())
			stat = 0;
		bcopy(Sbuffer, nbuf, modesize * 2);
		nbuf += modesize;
		buf++;
		if ((buf > 3) || !stat) {
			write (fdout, sbuf, modesize * 2 * buf);
			nbuf = sbuf;
			buf = 0;
			};
		};
	free (sbuf);
	return (0); 
};

// ���������� ��, key - ���������� �������� �������
int VCR::IrcKeyTrn(int fdout, char *key) {
};

/*
 * Constructor
 */
VCR::VCR() {
//	printf("Constructor");
	svcr = NULL; skey = Sbuffer = bufkadr = Sbufkod = Nbufkod = NULL;
	fvcr = 0;
	keynums = 0;
	modesize = 0;
	kadrsize = 0x1000;
	sizeper = 0;
//	printf("\n");
};

/*
 * Destructor
 */
VCR::~VCR(){
//	printf("Destructor");
	// ����� �����
	if  (Sbuffer) {
		free(Sbuffer);
		}
	// ������������������ �����
	if (skey) {
		free(skey);
		}
	// ������� ����������
	if (svcr) {
		free(svcr);
		}
//	printf("\n");
};
