/*
 * Tdr.h
 */

/*
 * ��������� ����� TDR
 */
struct	tdr_head {
	u_int	startf;		/* �������� ������� ������ */
	u_int	startd;		/* �������� ������� ��������� */
	u_int	startpt;	/* �������� ������� ���������������� */
	u_int	sizef;		/* ������ ������� ������ */
	u_int	sized;		/* ������ ������� ��������� */
	u_int	sizept;		/* ������ ������� ���������������� */
	u_short	format;		/* ������ ����� */
	u_short	id;		/* ID ����� */
	u_short	length;		/* ����� ����� � �������� */
	u_short	savelen;	/* ����� ����������� ������� ����� */
	u_int	nextsect;	/* ��������� ��� ������ ������ */
};

/*
 * �������� ���������� ����� TDR ������.
 * �������� � ������ 3.05
 */
struct	tdr_head_new {
	u_int	startdes;	/* �������� ������� description's */
	u_int	reserv1[4];
	u_int	sizedes;	/* ������ ������� description */
	u_int	reserv2[4];
	u_int	lastsect;	/* ���������� ������ ��� ������ */
	u_int	reserv3[9];
};

/*
 * �������� ����� ������� ������
 */
struct	tdr_file {
	char	name[8];	/* ��� ����� */
	char	ext[3];		/* ���������� ����� */
	char	attr;		/* ������� ����� */
	u_int	startsect;	/* ��������� ������ */
	u_int	startdesc;	/* �������� �������� */
	u_short	reserv1;
	u_short	dt;		/* ����� ����� � ������� DOS */
	u_short	dd;		/* ���� ����� � ������� DOS */
	u_short	clastr;		/* �� ������������ */
	u_int	size;		/* ������ � ������ */
};

/*
 * �������� ����� ������� ���������
 */
struct	tdr_dir {
//	u_short		a;	/* ������ */
	u_short		level;		/* ������� �������� */
#if 0
	TDR_FILES	f;		/* �������� �������� */
#else
	char	name[8];	/* ��� ����� */
	char	ext[3];		/* ���������� ����� */
	char	attr;		/* ������� ����� */
#if 0
	u_int	startsect;	/* ��������� ������ */
	u_int	startdesc;	/* �������� �������� */
#else
	u_short	startsect[2];	/* ��������� ������ */
	u_short	startdesc[2];	/* �������� �������� */
#endif
	u_short	reserv1;
#if 1
	u_short	dt;		/* ����� ����� � ������� DOS */
	u_short	dd;		/* ���� ����� � ������� DOS */
#else
	u_int	time;
#endif
	u_short	clastr;		/* �� ������������ */
#if 0
	u_int	size;		/* ������ � ������ */
#else
	u_short	size[2];		/* ������ � ������ */
#endif
#endif
	u_short		startf;		/* ����� ������� ����� */
	u_short		lastf;		/* ����� ���������� ����� (��������) */
	u_short		numf;		/* ����� ������ */
//	u_short		b;	/* ������ */
};
#define	SIZETDRDIR	40

struct	tdr_pt {
	u_short		size;
	u_short		h[0x8d];
	u_char		l[0x1114];
};
