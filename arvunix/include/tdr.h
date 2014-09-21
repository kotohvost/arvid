/*
 * tdr.h
 */

/*
 * ��������� ����� TDR
 */
struct	TDR_HEAD {
	unsigned int	startf;		/* �������� ������� ������ */
	unsigned int	startd;		/* �������� ������� ��������� */
	unsigned int	startpt;	/* �������� ������� ���������������� */
	unsigned int	sizef;		/* ������ ������� ������ */
	unsigned int	sized;		/* ������ ������� ��������� */
	unsigned int	sizept;		/* ������ ������� ���������������� */
	unsigned short	format;		/* ������ ����� */
	unsigned short	ID;		/* ID ����� */
	unsigned short	length;		/* ����� ����� � �������� */
	unsigned short	savelen;	/* ����� ����������� ������� ����� */
	unsigned int	nextsect;	/* ��������� ��� ������ ������ */
};

/*
 * �������� ���������� ����� TDR ������.
 * �������� � ������ 3.05
 */
struct	TDR_ADD_HEAD {
	unsigned int	startdes;	/* �������� ������� description's */
	unsigned int	reserv1a;	/*  */
	unsigned int	reserv2a;	/*  */
	unsigned int	reserv3a;
	unsigned int	reserv4a;
	unsigned int	sizedes;	/* ������ ������� description */
	unsigned int	reserv1b;	/*  */
	unsigned int	reserv2b;	/*  */
	unsigned int	reserv3b;
	unsigned int	reserv4b;
	unsigned int	lastsect;	/* ���������� ������ ��� ������ */
	unsigned int	reserv[9];
};

/*
 * �������� ����� ������� ������
 */
struct	TDR_FILE {
	char	name[8];	/* ��� ����� */
	char	ext[3];		/* ���������� ����� */
	char	attr;		/* ������� ����� */
	unsigned int	startsect;	/* ��������� ������ */
	unsigned int	startdesc;	/* �������� �������� */
	unsigned short	reserv1;
	unsigned short	dt;		/* ����� ����� � ������� DOS */
	unsigned short	dd;		/* ���� ����� � ������� DOS */
	unsigned short	clastr;		/* �� ������������ */
	unsigned int	size;		/* ������ � ������ */
};

/*
 * �������� ����� ������� ���������
 */
struct	TDR_DIR {
//	unsigned short		a;	/* ������ */
	unsigned short		level;		/* ������� �������� */
#if 0
	TDR_FILES	f;		/* �������� �������� */
#else
	char	name[8];	/* ��� ����� */
	char	ext[3];		/* ���������� ����� */
	char	attr;		/* ������� ����� */
#if 0
	unsigned int	startsect;	/* ��������� ������ */
	unsigned int	startdesc;	/* �������� �������� */
#else
	unsigned short	startsect[2];	/* ��������� ������ */
	unsigned short	startdesc[2];	/* �������� �������� */
#endif
	unsigned short	reserv1;
#if 1
	unsigned short	dt;		/* ����� ����� � ������� DOS */
	unsigned short	dd;		/* ���� ����� � ������� DOS */
#else
	unsigned int	time;
#endif
	unsigned short	clastr;		/* �� ������������ */
#if 0
	unsigned int	size;		/* ������ � ������ */
#else
	unsigned short	size[2];		/* ������ � ������ */
#endif
#endif
	unsigned short		startf;		/* ����� ������� ����� */
	unsigned short		lastf;		/* ����� ���������� ����� (��������) */
	unsigned short		numf;		/* ����� ������ */
//	unsigned short		b;	/* ������ */
};
#define	SIZETDRDIR	40
