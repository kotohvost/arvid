/*
 * ����� ������������ ��� ������ � �������� ���������� ��.
 */
#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "string.h"
#include "ctype.h"
#include "errormes.h"		// ��������� �� �������
#include "Irc.h"

#if 0
#define DEBUG_IRCOUT
#endif

static	ushort	IRC_ON[] = {
	0xffff, 0xfffe, 0xfffc, 0xfff8, 0xfff0, 0xffe0, 0xffc0, 0xff80,
	0xff00, 0xfe00, 0xfc00, 0xf800, 0xf000, 0xe000, 0xc000, 0x8000 };
static	ushort	IRC_OFF[] = {
	0x0000, 0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f,
	0x00ff, 0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff };

static char	*debuginfo;

/*
 * Constructor
 */
VCR::VCR() {
#ifdef DEBUG_IRCOUT
	printf("Constructor");
#endif
	svcr = NULL; skey = Sbuffer = bufkadr = Sbufkod = Nbufkod = NULL;
//	fvcr = 0;
	keynums = 0;
	curmode = 0;
	modesize = 0;
	kadrsize = 0x1000;
	sizeper = 0;
	vcr_34 = 0;
#ifdef DEBUG_IRCOUT
	printf("\n");
#endif
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

#ifdef DEBUG_IRCOUT
	printf("LoadVCR: %s", fname);
	// ������� ����
	printf(" open");
#endif
	handle = open(fname, O_RDONLY, 0);
	if (handle < 0) {
		errinf(EOPEN, "LoadVCR: Error open file\n");
		return (EOPEN);
	}
	if (!svcr) {
	// ������� ����� ��� �������
//		svcr = (u_char *)malloc(0x996);
		svcr = (vcrfile *)malloc(sizeof(vcrfile));
	}
	// ��������� �������
#ifdef DEBUG_IRCOUT
	printf(" read");
//	if (read(handle, svcr, 0x996) != 0x996) {
#endif
	if (read(handle, svcr, sizeof(vcrfile)) != sizeof(vcrfile)) {
		errinf(EREAD, "LoadVCR: Read error file\n");
		free(svcr);
		svcr = NULL;
		return (EREAD);
	}
	close(handle);
//	fvcr = 1;
#ifdef DEBUG_IRCOUT
	printf("CRC: %X\n", svcr->tstsum);
//	printf("%s\n", svcr->kname[0]);
#endif
	keynums = KeyNum();
	return (0);
};

/*
 * ����.
 * 0 - ������
 */
int VCR::KeyNum() {
int	key;

	if (svcr == NULL) {
		errinf(ETABLENONE, "KeyNum: Table not loaded\n");
		return (0);
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

	if (svcr == NULL) {
		errinf(ETABLENONE, "Uncompress: Table not loaded\n");
		return (1);
	}
	// �������� � �������� ����� ������������������
	if (key >= keynums) {
		errinf(ENUMKEY, "Uncompress: Key number error\n");
		return(ENUMKEY);
	}
	// ����������� �������
	if (!skey) {
		skey = (u_short *)malloc(0x100 * 2);
	}
	nvcr = svcr;
	nkey = skey;
	// key -> num buffer
//	keytmp = svcr->key[key] * 0x68;
	keytmp = svcr->key[key];
#ifdef DEBUG_IRCOUT
	printf("KeyTmp: %d/%d\n", keytmp, svcr->key[key]);
#endif
	for (i=0; i < 0x68; i++) {
//		p = svcr->a[0x23A + keytmp + i];
		p = svcr->pack[keytmp][i];
#ifdef DEBUG_IRCOUT
	printf("%02X.",p);
#endif
	// �����
		n = p & 0x0f;
		if (n == 0x0f) break;
		*nkey = svcr->type.paket[n];
#ifdef DEBUG_IRCOUT
	printf("%04X.", *nkey);
#endif
		nkey++;
	// �����
		n = p >> 4;
		if (n == 0x0f) break;
		*nkey = svcr->type.pause[n];
		if (maxpaus < *nkey)
			maxpaus = *nkey;
#ifdef DEBUG_IRCOUT
	printf("%04X.", *nkey);
#endif
		nkey++;
	}
	*nkey = 0;			/* ��������� */
	// ��������� ��������������� �������
	// ��� ���������
//	if (!svcr->a[0x238]) {
	if (!svcr->mod) {
	int	tmp;
	// ������ � ���.
		tmp = svcr->a[0x1BC] * maxpaus / 1000;
		if (tmp > 82) {
			word_23e = 3;
		} else if (tmp > 62) {
			word_23e = 2;
		} else if (tmp > 42) {
			word_23e = 1;
		} else {
			word_23e = 0;
		}
	}
	/* ���������� ���������� ��� � ������� */
//	if (svcr->a[0x238]) {
	if (svcr->mod) {
	// ���������������� ������
		sizeper = svcr->a[0x1BC] * 340 / 238;
	} else {
	// �������������� ������
		sizeper = svcr->a[0x1BC] * 3;
	}
	sizeper++;
	/* ��������� ������ ���������� �������� � ������ */
	modesize = (kadrsize / (sizeper * 12)) * (sizeper * 12);
	/* ��������� ����� ������ � ���� ������ */
	Sbufkod = skey;
	startflag = 1;
	/* �������� ������� ���������� � ������� */
	IsRepet = FindRepetitor(key);
#ifdef DEBUG_IRCOUT
	printf("Rep: %d\n", IsRepet);
#endif
	/*
	 * 1795
	 */
	vcr_32 = 0;
	// ��������� �� ����� ����� ������
	if (!Sbuffer) {
		Sbuffer = (u_short *)malloc(0x1000*2);
	}
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
		fEndKey = 0;
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
		fStartP = pause = 1;
		/* ����������������� � ��������� �������. */
	/* end 1932 */
	return (0);
};

/*
 * ����� ���������� � �������. �������� ������ ������ �� �������. !!!
 */
int VCR::FindRepetitor(int key) {
int	i=0;

	while (svcr->a[0x158 + i] != 0xff)
		if (svcr->a[0x158 + i] == key)
			return (1);
		else
			i++;

	return (0);
};

/*
 * �������������� ������������������ � ������
 */
int VCR::VCR_1950() {
int	bp2;
int	tmpkod;

	/*
	 * 1602
	 */
	while (!fEndKey && bufbit) {
	// pause for start
		if (fStartP) {
			fStartP = 0;
			kodn = 3000 / sizeper;
		}
		if (!kodn) {
			kodn = *Nbufkod++;
			pause ^= 1;
		}
#ifdef DEBUG_IRCOUT
	printf("kod: %04X/%08X ", kodn, bufbit);
#endif
		if (kodn) {
			if (bufbit < (kodn * sizeper))
				tmpkod = bufbit / sizeper;
			else
				tmpkod = kodn;
#ifdef DEBUG_IRCOUT
	printf("%04X ", tmpkod);
#endif
		// kod to signal
			if (pause)
				IRCtoPause(bit, sizeper, tmpkod, bufkadr);
			else {
				if (!svcr->a[0x238])
					IRCtoMod(bit, sizeper, tmpkod, bufkadr);
				else
					IRCtoNMod(bit, sizeper, tmpkod, bufkadr);
			} /* pause */
			kodn -= tmpkod;
			bufbit -= tmpkod * sizeper;
			CorrectBufBit(tmpkod * sizeper);
		} else {
			fEndKey = 1;
		} /* kodn */
	} /* while */
	/* 170D */
	if (fEndKey <= 3) {
		if (bufbit) {
			if (vcr_34)
				goto c_1745;
			tmpkod = bufbit / sizeper;
#ifdef DEBUG_IRCOUT
	printf("kod: %04X/%08X %04X ", kodn, bufbit, tmpkod);
#endif
			IRCtoPause(bit, sizeper, tmpkod, bufkadr);
			} /* bufbit */
	/* MakeIRCbufbit */
		bufbit = modesize << 4;	// *16
		bit = 0;
		bufkadr = Sbuffer;
	/* end MakeIRCbufbit */
c_1745:
		if (fEndKey) {
			if (!vcr_34)
				{ fEndKey++; }
			else
				{ fEndKey = 3; }
		}
	}
#ifdef DEBUG_IRCOUT
	printf("VCR_1950 done (%d/%d)\n", bufbit, bufbit/8);
#endif
	/* 175B */
	if (fEndKey == 3)
		return (1);
	return (0);
};

/*
 * ���������� ������� Pause.
 */
void VCR::IRCtoPause(int bit, int sizeper, u_short kod, u_short *buffer) {
int	tmpb, tmpk;

#ifdef DEBUG_IRCOUT
	printf("Pause\n");
#endif
	tmpb = kod * sizeper;
	if (bit) {
		*buffer++ &= IRC_OFF[bit];
		tmpb -= 0x10 - bit;
	}
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

#ifdef DEBUG_IRCOUT
	printf("Signal-M\n");
#endif
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
printf("Signal-N\n");
};

void VCR::CorrectBufBit(int bits) {
#ifdef DEBUG_IRCOUT
	printf("bufk: %X/%X+%X=", bufkadr, bit, bits/16);
#endif
	bit += bits & 0x0f;
	bufkadr += bits/16;
	if (bit >= 0x10) {
		bufkadr++;
		bit &= 0x0f;
	}
#ifdef DEBUG_IRCOUT
	printf("%X/%X\n", bufkadr, bit);
#endif
};

/*
 * ���������� ��, key - ����� ������� ������
 * 0 - OK
 */
int VCR::IrcKeyTrn(int fdout, int key) {
int	stat, buf, delay;
u_short	*sbuf, *nbuf, *pbuf;
#define	IKTBNUM	5

	stat = Uncompress(key);
	if (stat)
		return (stat);
	stat = 1;
//	write (fdout, Sbuffer, modesize * 2);
	sbuf = nbuf = pbuf = (u_short *)malloc(312*12 * 2 * IKTBNUM);
	bzero(sbuf, 312*12 * 2 * IKTBNUM);
	buf = 0;
	delay = 0;
	while (stat) {
		if (VCR_1950())
			stat = 0;
		bcopy(Sbuffer, nbuf, modesize * 2);
		nbuf += modesize;
		/* ���� ��������� ����� ����� ������ ��� ����� */
		if (nbuf+modesize > sbuf+312*12*IKTBNUM || stat == 0) {
		/* ����� ������ ����� ������ ������� ������ */
			int	ctmp, wtmp, cbuf;
			/* ��������� ���������� ���������� ������� */
			cbuf = (nbuf - sbuf) / (312 * 12);
			/* ������ ��� ������ */
			wtmp = 312*12*2 * cbuf;
#ifdef DEBUG_IRCOUT
		printf("cbuf: %d, wtmp: %d\n", cbuf, wtmp);
#endif
			/* ������� */
			for (pbuf = sbuf; wtmp && (ctmp != -1);) {
				do {
					ctmp = write(fdout, pbuf, 312*12*2);
				} while (ctmp != 312*12*2 && ctmp != -1);
				pbuf += ctmp/2;
				wtmp -= ctmp;
			}
			/* ���������� ���������� */
			wtmp = nbuf - sbuf - cbuf*312*12;
			wtmp *= 2;
#ifdef DEBUG_IRCOUT
		printf("wtmp: %d\n", wtmp);
#endif
			/* ���� ����� ������� */
			if (stat == 0 && wtmp) {
				do {
					ctmp = write(fdout, pbuf, wtmp);
				} while (ctmp != wtmp && ctmp != -1);
				nbuf = sbuf;
			} else {
				/* ��������� � ������ */
				bcopy(pbuf, sbuf, wtmp);
				/* � �������� */
				nbuf = sbuf + wtmp/2;
			}
		}
	}
	free (sbuf);
	/* �������� */
	delay = svcr->a[0x8be] / 50;
	if (!delay)
		delay++;
	printf("IrcKeyTrn: Sleep %d/%d sec\n", svcr->a[0x8be], delay);
	sleep(delay);
	return (0); 
};

// ���������� ��, key - ���������� �������� �������
// 0 - OK
int VCR::IrcKeyTrn(int fdout, char *key) {
int	keynum;

	keynum = 0;
	if (isdigit(*key)) {
	// �����
		keynum = atoi(key);
		return(IrcKeyTrn(fdout, keynum));
	} else {
		if ((keynum = NameToNum(key)) != -1)
			return(IrcKeyTrn(fdout, keynum));
	}
	return(1);
};

/*
 * ������� �� �� �������� (curmode) ��������� � ��������� 'key'.
 * 0 - OK
 */
int VCR::SetMode(int fdout, int key) {

	if (curmode == key)
	/* ��������� ��������� */
		return(0);

	curmode = key;
	return(IrcKeyTrn(fdout, key));
};

/*
 * ������� �� �� �������� (curmode) ��������� � ��������� 'key'.
 * 0 - OK
 */
int VCR::SetMode(int fdout, char *keyname) {
int	key;

	if ((key = NameToNum(keyname)) == -1)
		return(1);
	return(SetMode(fdout, key));
};

/*
 * ������� ����� ������� � �� �����
 */
int VCR::NameToNum(char *name) {
int	num;

	debuginfo = "VCR::NameToNum";
	if (svcr == NULL) {
		printf("%s: Table not loaded\n", debuginfo);
		return(-1);
	}
	num = 0;
	while ((num < 16) &&
		(strncasecmp(svcr->kname[num], name, 10) != 0))
			num++;
	if (num < 16)
		return(num);
	else
		printf("%s: Key not where: %s\n", debuginfo, name);
	return(-1);
};

/*
 * ���������� ����� � ��������
 * ������ ����������� ���������� �������������
 * �� ��������� ����������� ����������� �����
 * 0 - OK
 */
int VCR::Move(int fdout, int sec) {
int 	cmode;

	if (sec == 0)
		return(0);

	cmode = curmode;
	if (sec < 0) {
	/* ����� - REW */
		printf("Move: REW %d sec, ", sec);
		if (abs(sec) > svcr->a[0x8de]) {
		/* REW */
		printf("STOP");
			SetMode(fdout, "STOP");
		printf("-REW\n");
			SetMode(fdout, "REW");
		} else {
		/* Play_REW */
		printf("PLAY");
			SetMode(fdout, "PLAY");
		printf("-REW\n");
			SetMode(fdout, "REW");
		}
	} else {
	/* ������ - FF */
		printf("Move: FF %d sec\n", sec);
		if (abs(sec) > svcr->a[0x8dc]) {
		/* FF */
			SetMode(fdout, "STOP");
			SetMode(fdout, "FF");
		} else {
		/* Play_FF */
			SetMode(fdout, "PLAY");
			SetMode(fdout, "FF");
		}
	}
	sleep(abs(sec));
	SetMode(fdout, cmode);
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
