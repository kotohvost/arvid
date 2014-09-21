/*
 * �������� �������� � �������� ������������ � ���������� ������ � �����-1000
 */
#include "stdio.h"
/* ��� ������ */
/* #include "avtreg.h" */

/* ������� ���� ����� */
#define FORMAT_ERROR	0x00	/* ���������� ������� */
#define FORMAT_K4	0x01	/* ������ (reserved) */
#define FORMAT_K7	0x02	/* ������ 1010 100K/s (old) */
#define FORMAT_K7CRC	0x04	/* ������ 1010 100K/s c ���������� CRC32 */
#define FORMAT_RS	0x08	/* ������ 1020,103X 200K/s ��� ����-�������� */
#define FORMAT_RSC	0x10	/* ������ 1020,103X >200K/s ��� ����-��������
				   � ������������ ������ */
#define FORMAT_RSC_50	0x20	/* ������ 105� >325K/s */

#define PASSWD		0x33	/* ������ ������ ����� 100K/s */
#define PASSWD1		0xCC	/* ������ ������ ����� 100K/s */

/* ����� � ����� ����� 100/200 K/s */
#define SPEC_ATTR	0x1000	/* ���� log */
#define TAIL_ATTR	0x2000	/* ����� ���������� ������ */
#define HEAD_ATTR	0x4000	/* ������ ���������� ������ */
#define	LENGTHMASK	0x03ff	/* ����� ��� ����� ����� */
#define	DIR_MARK_C	0x0800	/* ���� � ����������� � ������� Compress */

/* ����� � ������ ������� 100/200 K/s */
#define DIR_MARK	0x80000000L	/* ���� � ����������� */
#define SPEC_MARK	0x40000000L	/* ���� �� ���� ��������� */
#define	SECTORMASK	0x3fffffffL	/* ����� ��� ������� */

/* ����� ������ ������������ ������ � ������ */
#define	SIZE_G_BLOCK1	21		/* 100K/s */
#define SIZE_G_BLOCK2	1		/* 100K/s */
#define	SIZE_RS_BLOCK1	18		/* 200K/s */
#define SIZE_RS_BLOCK2	1		/* 200K/s */

/* ���������� �������� � ����� */
#define NUM_G_SECT	4		/* 100K/s */
#define NUM_RS_SECT	8		/* 200K/s */

/* �������� �������������� ����� � ����� */
#define ADD_G_BEG	84		/* 100K/s */
#define ADD_RS_BEG	144		/* 200K/s */

/* ���� lenCompress */
#define	NO_COMPRESS	0x8000		/* ������� ���������� ���������
					   ���������� ����� ������ */
#define	COMPRESSMASK	0x03ff		/* ����� �� ����������
					   �������������� ����� */
/*
 *------------------------------------------------------------------
 * ��������� ���������� ������
 *------------------------------------------------------------------
 */

/* FORMAT_K7CRC - 1010 */
typedef struct {
	u_char	format;		/* ������ ����� */
	u_char	parol1;		/* ������ */
	u_char	name[8];	/* ��� ����� */
	u_short	ident;		/* �������������� ����� */
	u_short	length;		/* ����� ����� � ������� � �������� */
	u_short	tapetime;	/* ������� ����� � �������� */
	u_int	sector;		/* ��������� ������ � �������� */
	u_char	nsect;		/* ���������� �������� � ����� */
	u_char	parol;		/* ������ */
	u_short	tstsum;		/* ����������� ����� ��������� */
} HEADER_G;

/* FORMAT_RS - 1020, 103x - Rid-Solomon, ���� nsect */
typedef struct  {
	u_char	format;		/* ������ ����� */
	u_char	nsect;		/* ����� �������� � ����� */
	u_char	name[8];	/* ��� �����, � ��������� I8 (?) */
	u_short	ident;		/* �������������� ����� */
	u_short	length;		/* ����� ����� � ������� � ��������� */
	u_short	tapetime;	/* ����� �� ������ �����, sec */
	u_int	sector;		/* ��������� ������ */
	u_char	upassword;	/* ������ ������� � ���� usr */
	u_char	distance;	/* ��������� �� Log � ���. (255-������) */
	u_short	usr;		/* �����. ����: ������ ������ �������� */
	u_int	tstsum;		/* ����������� ����� ��������� */
} HEADER_RS;

/* FORMAT_RSC - 1020, 103x - Rid-Solomon, compression */
typedef struct  {
	u_char	format;		/* ������ ����� */
	u_char	nsect;		/* ����� �������� � ����� */
	u_short	lenCompress;	/* ����� ������ ����������, ���� ������ */
	u_int	tapeID;		/* �������������� ����� */
	/* ������ ��� XOR */
	u_char	num_xor;	/* ����� �������� ����� � XOR ����� */
	u_char	num_block;	/* ����� �������� ����� � XOR ������� */
	u_char	nr_xor;		/* ���������� XOR ������ � XOR ����� */
	u_char	nr_block;	/* ���������� XOR ������ � XOR ������� */
	u_short	length;		/* ����� ����� � ������� � ��������� */
	u_short	tapetime;	/* ����� �� ������ �����, sec */
	u_int	sector;		/* ��������� ������ */
	u_char	numSpec;	/* ������ ����-�������� (���� � �����) */
	u_char	x0, x1, x2;
	u_int	tstsum;		/* ����������� ����� ��������� */
} HEADER_RSC;

/* FORMAT_RSC_50 - 105x - Rid-Solomon, compression */
typedef struct  {
	u_char	format;		/* ������ ����� */
	u_char	nsect;		/* ����� �������� � ����� */
	u_short	lenCompress;	/* ����� ������ ����������, ���� ������ */
	u_int	tapeID;		/* �������������� ����� */
	/* ������ ��� XOR */
	u_char	num_xor;	/* ����� �������� ����� � XOR ����� */
	u_char	num_block;	/* ����� �������� ����� � XOR ������� */
	u_char	nr_xor;		/* ���������� XOR ������ � XOR ����� */
	u_char	nr_block;	/* ���������� XOR ������ � XOR ������� */
	u_short	length;		/* ����� ����� � ������� � ��������� */
	u_short	tapetime;	/* ����� �� ������ �����, sec */
	u_int	sector;		/* ��������� ������ */
	u_char	numSpec;	/* ������ ����-�������� (���� � �����) */
	u_char	x0, x1, x2;
	u_int	CRC;		/* ����������� ����� ������ */
	u_char	info[12];	/* ������ 12 ���� ������ */
	u_int	tstsum;		/* ����������� ����� ��������� */
} HEADER_RSC_50;

typedef union {
	HEADER_G	g;	/* ����� */
	HEADER_RS	rs;	/* ���-������� */
	HEADER_RSC	rsc;	/* */
	HEADER_RSC_50	rsc50;	/* */
} UHEADER;

/*
 *------------------------------------------------
 * �������������� ������ ��� �����
 *------------------------------------------------
 */
typedef struct {
	u_int	sect[2][2];	/* ������� �� ������� */
	u_int	csum[2];	/* �� ������� */
	u_short	reserv[2];	/* ���������� */
} ADD_RS_BLOCK;

typedef union {
/*	ADD_G_BLOCK	g; */			/* ��� ���� Goley */
	ADD_RS_BLOCK	rs[NUM_RS_SECT/2];	/* 4 � ���� RS */
} UADD_BLOCK;

/* �������� ����� */
#define	TAPE_DIR	0x00040000
#define	TAPE_RWXU	0x00000700
#define	TAPE_RUSR	0x00000400
#define	TAPE_WUSR	0x00000200
#define	TAPE_XUSR	0x00000100
#define	TAPE_RWXG	0x00000070
#define	TAPE_RGRP	0x00000040
#define	TAPE_WGRP	0x00000020
#define	TAPE_XGRP	0x00000010
#define	TAPE_RWXO	0x00000007
#define	TAPE_ROTH	0x00000004
#define	TAPE_WOTH	0x00000002
#define	TAPE_XOTH	0x00000001
