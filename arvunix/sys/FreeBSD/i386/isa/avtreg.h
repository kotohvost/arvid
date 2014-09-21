/*
 * ������ ���������� �����������, �� ��������� arvid10.exe Ver. 3.46r4
 * � ����� arvidapi.h ����������� �� ��� SU.HARDW.SUPPORT.ARVID
 *	����-�������� ���������� ����� V0.01beta (Arvid API)
 *  ��������:
 *	- ������������� �������� ����� ����������
 *	- �������� ������������ �������� ������
 *
 * ��������� ��� Arvid 1031
 */

/* ��� ������ */
/*
 * �������� �������� � �������� ������������ � ���������� ������ � �����-1000
 */
#define	SIZESTR100	 8			/* ����� ������ 100 K/s */
#define	SIZE100		256*SIZESTR100*2	/* ������ ����� 100 K/s */

#define	SIZESTR200	 9			/* ����� ������ 200 K/s */
#define	SIZE200		284*SIZESTR200*2	/* ������ ����� 200 K/s */

#define	SIZESTR325	13			/* ����� ������ 325 K/s */
/*#define	SIZE325		296*SIZESTR325*2 */	/* ������ ����� 325 K/s */
#define	SIZE325		300*SIZESTR325*2	/* ������ ����� 325 K/s */

#define SIZE1030	4096		/* ������ ����� ��� ����� � ������ */

/* ��� ����� ������������ ����������� 16-�� ���������
 * ArvidPort	= 270h | 320h
 *
 * ������/������
 */
#define	AV30_RDINC(base)	((base) + 0)
#define	AV30_RD(base)		((base) + 2)
/*
 * ��� ������
 */
#define AV30_RK(base)		((base) + 4)
#define AV30_RA(base)		((base) + 6)
/*
 * ��� ������
 */
#define	AV30_RS(base)		((base) + 4)
#define AV30_RS1(base)		((base) + 6)
#define AV30_NPORT		2*4	/* ���������� ������������ ������ */

/*
 * ���� ����������� _���_ �������� ������ �� ����� ��� ������ 1010
 *
 * ����� ������� �� ����� ����������� ���� �� 1032 ���� � ��������� �������:
 * ----------------------------------------------
 * |	������ ������ - [512-8] ����		|
 * ----------------------------------------------
 * |	������ ������ - [512-8] ����		|
 * ----------------------------------------------
 * |	������ ������ - [512-8] ����		|
 * ----------------------------------------------
 * |	��������� ������ - [512-8] ����		|
 * ----------------------------------------------
 * |	8 ���� - �� ������� �������		|
 * ----------------------------------------------
 * |	8 ���� - �� ������� �������		|
 * ----------------------------------------------
 * |	8 ���� - ����������� ����� ���� 16 ����	| ������ Xor16
 * ----------------------------------------------
 * |	8 ���� - �� �������� �������		|
 * ----------------------------------------------
 * |	8 ���� - �� ��������� �������		|
 * ----------------------------------------------
 * |	8 ���� - ����������� �����		|
 * ----------------------------------------------
 */
/*
 * ������ ���� ��������� TPB �� ����� 24 �����
 */

/* ����� ��������� �� ���� ���� */

#define SizeD100	1032	/* ������ � ������ (SizeData 100 Kb/s) */

#define SizeK100	1056

/*
 * ����� �� �������� ����� 1056 ���� � ����� �����. ����� �����
 * ���� ���������� XOR ��������������� �� 24 -> 66. ����������
 * ���� ������ ������� ��� �����. ���� � ��� ��� ���������������
 * ������������ ������ ������ ������ (��� 1010 = 34 �����)
 * ��� �������� � ���������� ���������� �� �����. 
 */

/*
 * ��� ���������� ���������
 */

enum controller {
	UNKNOWN	= 0,	/* unknown type, driver disabled */
	Av1010	= 1010,	/* 1010 - 100 Kb/s (180min = 1Gb) */
	Av1020	= 1020,	/* 1020 - 200 Kb/s (180min = 2Gb) */
	Av1030	= 1030,	/* 1030 - 200 Kb/s */
	Av1031	= 1031,	/* 1031 - 200 Kb/s, 32 Kw, (300 Kb/s with compression) */
	Av1051	= 1051,	/* 1051 - 325 Kb/s, 64 Kw, (180min = 3.3Gb) */
	Av1052	= 1052,	/* 1052 - 325 Kb/s, 256 Kw, -//--//- */
};

struct avt_info {		/* Driver Parameters Block */
/* ������������ �������������� � �������� ������ */
	enum controller arvid;	/* ��� ����������� */
	u_short	port;			/* ����� �������� ����� */
	u_short	irq;			/* ���������� */
	u_short	drq;			/* DMA ����� */
	u_char	ver_major,
		ver_minor;		/* ������ �������� */
	u_short	maxrate;	/* ����. �������� �������� [100-200] �����/c�� */
#define	MAXRATE10	100	/* 100 Kb/s */
#define	MAXRATE20	200	/* 200 Kb/s */
#define	MAXRATE30	200	/* 200 Kb/s */
#define	MAXRATE50	325	/* 325 Kb/s */
	u_char	maxphase;	/* ����. �������� ��������� phase */
#define MAXPHASE10	3	/* ��� 1010 (0-3) */
#define MAXPHASE20	7	/* ��� 1020 (0-7) */
#define MAXPHASE30	15	/* ��� 103x (0-7 + A0-A7) */
#define MAXPHASE50	23	/* ��� 103x (0-7 + A0-A7 + B0-B7) */

/*	2. ���� ������� �������� �����
 * ���������� ��������������, ������� ����� ���������� ��� ���������� Device
 */
/*     struct CDPB {           // Current Device Parameters Block */
	u_short	flags;		/* ������ �������� */
#define	VTCLOSE	0x0000		/* ������� �������� */
#define	VTOPEN	0x0001		/* ������� ������������ */
#define	VCRLOAD	0x0002		/* ���� �������� VCR ������� */
#define	VTBUSY	0x8000		/* ������� �������� */
	u_char	commands;	/* ������� ������� */
	u_char	time_out;	/* ����� ����-����, ������, �.�. �����,
				   �� ��������� �������� �������� ������������,
				   ���� �� ����� �� ������� ������� ����������
				   (�� ��������� 13 ������) ??? */
	u_short	rate;		/* �������� �������� [100-200] �����/c�� */
	u_char	phase;		/* ������� ���� ������ */
#define DEFAULT_PHASE10	3
#define DEFAULT_PHASE20	3
#define DEFAULT_PHASE30 0
#define DEFAULT_PHASE50 1
	u_char	check_rec;	/* ���� �������� ������ � ���. Record */
/*	u_short	lp; */		/* ���� ������ � ������ LongPlay
				   ( �.�. 0 ) reserved
				   � ���� ������ ���� � ��������� ������ */
	u_char	auto_phase;	/* ����: ������� ������ � ������
				   �������������� ���� */
/* ONLY 1030 and above */
/* #define AUTOTUNON	0000100B */
/* #define AUTOTUNOFF	!AUTOTUNON */
/*	U8      compress_enable; // ����: ��� ������ ������ � ������� FORMAT_RSC
				 // ������ ���������
	U8      xor_enable;  //  ��������� ����������� XOR ����� ��� ������
	U8      xor_block_size;   // ���-�� ������ � XOR ����� ��� ������
	U8      xor_block_number; // ���-�� ������ � XOR ������� ��� ������
	U32     reserv;     // reserv
*/
};
typedef	struct	avt_info avt_info_t;

/*
 *======================================================
 * �������� ������� ��������� ����� ��� ������ ������ ��
 * �����, ��� ���� �� �� �������� �� ��������, � �����
 * ������ ����� ������ ������������ ��� �������������.
 *======================================================
 */
/* ������� */
#if 0
#define	SIZEKADR	296 * 13	/* ������������ ���������� ����
					   ������ � ����� ����� 1052 */
struct buf_raw {
	struct buf_raw	*next;	/* ��������� �� ��������� ����� */
	u_short	data[SIZEKADR];	/* ������ �� ����� */
};
#else
#define SIZEBUFBYTE	8192
struct buf_raw {
	struct buf_raw	*next;
	u_char		data[SIZEBUFBYTE];
};
#endif

#define	NBUFRAW	20		/* ���������� ������� */

#define	SIZE_VCR	0x996	/* ������ VCR ������� */

struct	set_rw {
	u_int	rate;		/* �������� ������: 100/200/325 */
#define	IORAW	0x0
#define	IODATA	0x1
#define	IOIRC	0x2
	u_char	mode;		/* RAW/IRC/DATA */
};
/* ioctl ������� */
#define	VGET_INFO	_IOR('A',  1, struct avt_info)	/* ������ ������ �������� */
#define	VSET_READ	_IOW('A',  2, struct set_rw)	/* ������ ������ */
#define	VSET_WRITE	_IOW('A',  3, struct set_rw)	/* ����� ������ */
#define	VGET_PHASE	_IOR('A',  4, unsigned char)	/* ������ ���� ������ */
#define	VSET_PHASE	_IOW('A',  5, unsigned char)	/* ��������� ���� ������ */
