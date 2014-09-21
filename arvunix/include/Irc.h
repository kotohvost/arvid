/*
 * ����� ������������ ��� ������ � �������� ���������� ��.
 */
#ifndef IRC
#define IRC

/*
 * ��������� VCR �����
 */
struct vcrfile {
	unsigned char	a[0];	/* ���� ��� ������������� */
	int	major, minor;	/* +8 � ����� ��� Version Id = 2.0*/
	unsigned char	b[0x10];
	unsigned char	PowerOn[32];	/* PowerOn */
	struct	To {
		unsigned char	key[6];
	};
	struct	To	Sto[10];	/* Stop to x */
	struct	To	Pto[10];	/* Play to x */
	struct	From {
		unsigned char	st[6];
		unsigned char	pl[6];
	};
	struct	From	Tox[10];	/* From x to Stop/Play */
	unsigned char	z[0x5c];
	unsigned short	rep;		/* +184 ������� ���������� */
	unsigned short	nrep;		/* ���-�� �������� ���������� */
	unsigned short	nkey;		/* ���-�� �������. ������ */
	unsigned char	key[50];	/* +18A ��������� �� ����� ������ ������ */
	unsigned short	period;		/* +1BC ������ ������� � ��� */
	struct	type {
		unsigned short	paket[11];
		unsigned short	pause[11];
	}	type;	/* +1BE */
	unsigned char	z1[78];		/* +1EA */
	unsigned short	mod;		/* 238 */
	unsigned char	pack[0x10][0x68];
	unsigned short	tstsum;		/* 8BA CRC16 */
	unsigned short	tstsum1;
	struct	SPLP {
		unsigned short	keytokey;	/* ����� ����� ��������� (����) */
		unsigned short	speedFF;	/* ������� �������� ����� */
		unsigned short	speedREW;
		unsigned short	cPLAY_FF;	/* ����������� �������� � PLAY */
		unsigned short	cPLAY_REW;
		unsigned short	cnPLAY_FF;
		unsigned short	cnPLAY_REW;
		unsigned short	delayFF;	/* �������� ��������� (����) */
		unsigned short	delayREW;
		unsigned short	skipPLAY_FF;	/* �������� ���� */
		unsigned short	skipPLAY_REW;
	} SPLP;
	unsigned char	z2[410];
	char	kname[16][10];	/* 2670 */
};

class VCR {
 public:
	vcrfile	*svcr;		// ��������� �� ������� ���������� ��
	int	keynums;	// ���������� ������
	int	curmode;	// ������� ��������� ��
	/* ������������ ��� ���������� */
	unsigned short	*skey;		// ��������� �� ����� ��� ������������������
	int	word_23e;	// ��������� (???)
	int	IsRepet;	// ���� ������� ����������
	int	vcr_2;		// +02h ??
	unsigned short	*Sbuffer;	// +06h ����� �����
	int	modesize;	// +08h ����. ����� Arvid'a ��� ������� ���. ��������
	int	kadrsize;	// +0Ah ����. ����� Arvid'a (4096)
	int	numkadr;	// +0Eh	...
	int	fEndKey;	// +12h ����� ������������������
	int	pause;		// +14h ���� ������� �����
	unsigned short	*Sbufkod;	// +16h ��� ���� ����� ������
	unsigned short	*Nbufkod;	// +1Ah ��������� �� ��������� ���
	unsigned short	kodn;		// +1Eh ��� ��� ��������������
	int	sizeper;	// +20h ����� ������ ������� � �����
	int	bufbit;		// +22h ������ modesize � �����
	int	startflag;	// +26h �����
	int	bit;		// +28h ������� ��� � ��������� �����
	unsigned short	*bufkadr;	// +2Ah ����� �����
	int	vcr_2C;		// +2Ch ??
	int	fStartP;	// +2Eh ������ ��������� �����
	int	vcr_30;		// +30h ??
	int	vcr_32;		// +32h ??
	int	vcr_34;		// +34h ?????? ��������������(?)
 public:
	VCR();			// constructor
	~VCR();			// destructor
 int	LoadVCR(char *);	// �������� �� �����
 int	KeyNum();		// ���������� ������ � VCR'e
 int	Uncompress (int);	// ���������� ������������������ �����
 int	FindRepetitor(int);	// ������� ���������� � �������
 int	VCR_1950();		// ���������� ��������
 void	IRCtoPause(int, int, unsigned short, unsigned short *);
 void	IRCtoMod(int, int, unsigned short, unsigned short *);
 void	IRCtoNMod(int, int, unsigned short, unsigned short *);
 void	CorrectBufBit(int bits);
	/* ���������� �� � ������������� �������� ��������� */
 int	SetMode(int, int);
 int	SetMode(int, char *);
	/* ���������� �� */
 int	IrcKeyTrn(int, int);
 int	IrcKeyTrn(int, char *);
	/* ������� ����������� ����� ������� � �������� */
 int	NameToNum(char *);
	/* ���������� ����� ������/����� � �������� */
 int	Move(int, int);
};

#endif
