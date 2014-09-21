/*
 * tdr.h
 */
#if 0
/* For DOS */
typedef	unsigned char	BYTE;
typedef	unsigned int	WORD;
typedef	unsigned long	DWORD;
#else
/* For UNIX */
typedef	unsigned char	BYTE;
typedef	unsigned short	WORD;
typedef	unsigned int	DWORD;
#endif

/* ��������� ����� TDR */
typedef struct {
	DWORD	startf;		/* �������� ������� ������ */
	DWORD	startd;		/* �������� ������� ��������� */
	DWORD	startpt;	/* �������� ������� ���������������� */
	DWORD	sizef;		/* ������ ������� ������ */
	DWORD	sized;		/* ������ ������� ��������� */
	DWORD	sizept;		/* ������ ������� ���������������� */
	WORD	format;		/* ������ ����� */
	WORD	ID;		/* ID ����� */
	WORD	length;		/* ����� ����� � �������� */
	WORD	savelen;	/* ����� ����������� ������� ����� */
	DWORD	nextsect;	/* ��������� ��� ������ ������ */
} TDR_HEAD;

/* �������� ���������� ����� TDR ������ */
typedef struct {
	DWORD	startdes;	/* �������� ������� description's */
	DWORD	reserv1a;	/*  */
	DWORD	reserv2a;	/*  */
	DWORD	reserv3a;
	DWORD	reserv4a;
	DWORD	sizedes;	/* ������ ������� description */
	DWORD	reserv1b;	/*  */
	DWORD	reserv2b;	/*  */
	DWORD	reserv3b;
	DWORD	reserv4b;
	DWORD	lastsect;	/* ���������� ������ ��� ������ */
	DWORD	reserv[9];
} TDR_ADD_HEAD;

/* �������� ����� ������� ������ */
typedef	struct {
	BYTE	name[8];	/* ��� ����� */
	BYTE	ext[3];		/* ���������� ����� */
	BYTE	attr;		/* ������� ����� */
	DWORD	startsect;	/* ��������� ������ */
	DWORD	startdesc;	/* �������� �������� */
	WORD	reserv1;
	WORD	time;		/* ����� ����� � ������� DOS */
	WORD	date;		/* ���� ����� � ������� DOS */
	WORD	clastr;		/* �� ������������ */
	DWORD	size;		/* ������ � ������ */
} TDR_FILES;
typedef	struct {
	BYTE	name[8];	/* ��� ����� */
	BYTE	ext[3];		/* ���������� ����� */
	BYTE	attr;		/* ������� ����� */
	BYTE	startsect[4];	/* ��������� ������ */
	BYTE	startdesc[4];	/* �������� �������� */
	BYTE	reserv1[2];
	BYTE	time[2];	/* ����� ����� � ������� DOS */
	BYTE	date[2];	/* ���� ����� � ������� DOS */
	BYTE	clastr[2];	/* �� ������������ */
	BYTE	size[4];	/* ������ � ������ */
} TDR_FILESb;

/* �������� ����� ������� ��������� */
typedef	struct {
	WORD		level;		/* ������� �������� */
	TDR_FILES	d;		/* �������� �������� */
	WORD		startf;		/* ����� ������� ����� */
	WORD		lastf;		/* ����� ���������� ����� (��������) */
	WORD		numf;		/* ����� ������ */
} TDR_DIR;
typedef	struct {
	BYTE		level[2];
	TDR_FILESb	d;
	BYTE		startf[2];
	BYTE		lastf[2];
	BYTE		numf[2];
} TDR_DIRb;
