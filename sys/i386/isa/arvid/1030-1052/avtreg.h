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

#define SIZE1030	4096	/* ������ ����� ��� ����� � ������ */

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
struct read_data {
	u_long	sect0[63];
	u_long	sect1[63];
	u_long	sect2[63];
	u_long	sect3[63];

	u_long	sect0e;
	u_long	sect1e;
	u_int	resrv0;
	u_short	crcS0S1;
	u_short	resrv0e;

	u_long	sect2e;
	u_long	sect3e;
	u_int	resrv1;
	u_short	crcS2S3;
	u_short	resrv1e;
};
/*
 * ������ ���� ��������� TPB �� ����� 24 �����
 */
typedef struct {
	u_char	TapeFormat;	/* ������ ����� */
/* ������� ���� ����� */
#define FORMAT_K4	0x1	/* ������ (reserved) */
#define FORMAT_K7	0x2	/* ������ 1010 100K/s (old) */
#define FORMAT_K7CRC	0x4	/* ������ 1010 100K/s c ���������� CRC32 */
#define FORMAT_RS	0x8	/* ������ 1020,103X 200K/s ��� ����-�������� */
#define FORMAT_RSC	0x10	/* ������ 1020,103X >200K/s ��� ����-��������
				   � ������������ ������ */
	u_char	TapeStartTPB;	/* ����������� ���� */
#define	StartTPB	0x33	/* */

	u_char	TapeName[8];	/* ��� ����� */
	u_short	TapeID;		/* ���������� �������������� ����� */
	u_short	TapeTime;	/* ����� ����� � ������� */

	u_short	Tapetime;	/* ������� ����� � �������� */
	u_char	TapeSectr[3];	/* ����� ������? ������� � ����� */
#define	SECTRNUM(a,b,c)	(a + (b << 8) + (c << 16))
	u_char	TapeFlag;	/* ���� ����� */
#define BLOCKDATA	0	/* ������ � ����� ��������� � ������ */
#define BLOCKTDR	0x80	/* ������ � ����� ��������� � TDR */
	u_char	TapeNsector;	/* ���������� �������� � �����? */
#define	EndTPB		!StartTPB
	u_char	TapeEndTPB;	/* ����������� ���� */
	u_short	TapeCRC;	/* ����������� ����� TPB */
} TPBInfo;

/*
 * ����� ���� ����������� ����� ������� �������
 * � � ����� �� ����� ����������� �����, ��� ���� 24 �����
 */
typedef struct {
	u_int	block0;
	u_int	block1;
	u_int	block2;
	u_int	block3;
	u_int	reserv;
	u_int	crc;
} TBcrc;

/* ����� ��������� �� ���� ���� */

#define SizeD100	1032	/* ������ � ������ (SizeData 100 Kb/s) */

typedef struct {
	u_short	data[SizeD100];	/* ������ � ������ */
	TPBInfo	TPB;		/* ��������� ����� */
	TBcrc	CRC;		/* ���� � ������ */
} avt_kadr100;

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

/*
 * ����� ��������� ��� ���������� ������ � �� ��������������
 * (������������ ���������������)
 */
struct avtvar {
	u_short	start;			/* 0 - ������ */
	u_short blk;			/* 2 - ���������� */
	u_short	raw [SIZE1030];		/* 4 - ����������� ���� */
	u_short	data [SizeK100];	/* 4100+0 - ���� ����� */
};

/*
 * ���� ����������, ������������ ��������� API

#define OK_VCR  0               // �����
enum{
	E_OK=0,                 // �����
	E_DRIVER_NOT_INSTALLED=1,      // ������� �� ����������
	E_DEVICE_INVALID_OR_ALREADY_EXIST=2,// �������� AVHANDLE
	E_SYSTEM_ERROR=3,       // ��������� ��������� ������ ( Windows: ��� � GetLastError() )
	E_NOT_TPB=4,            // �� ��������� TPB
	E_TPB_EXIST=5,          // TPB ��� ���������
	E_NO_FUNCTION=6,        // ������� ��� ������� �� �������������� (�������� �����)
	E_UNKNOWN_FORMAT=7,     // ������ ����������� ������ �����
	E_NOT_FOUND_TAPE_DIR=8, // ���������� ����� �� �������
	E_NOT_AVAIL_FORMAT=9,   // ���������������� ������ ����� ��� ������
	E_NOT_POSITION=10,      // ������ �� ������ (����-��� �����)
	E_NOT_IDENTIC_TPB=11,   // ������������ TPB (���������� �� �� �� �����)
	E__ABORT=12,            // ������� �������� �������������
	E_OLD_DIR=13,           // ���������� ��������, ������ ������ "�����" �����
				// ����� �������, � �������� ������ ������ ������
				// � ������ �������� ����������
	E_NOT_DIR=14,           // �� ������� ���������� ����� ��� ������
				// � ������ �������� ����������
	E_ABORT_DISABLE=15,     // ����� � ������ ������ ��������
	E_ABORT_RECORD=16,      // �����,  ����� �� ��� � ������
	E_TRANSL_RECORD=17,     // ������ �������� "������" � ������ REC
	E_NO_IRC_RCV=18,        // �� ���� ������ �� ������� (��� ������� ����� ��)
	// ������ ���� �������� AvdCheckStop
	OFF_VCR=19,             // �� �������� ��������
	TUNER_VCR=20,           // �� ���� ��������� ���� ����������� �� ������ TV, ����
				// ��������� ������ � �����
	PICTURE_VCR=21,         // ����������, �� ����� ������������� ��������
	REC_OK_VCR=PICTURE_VCR,   // ���������������

	E_TAPE_FULL=22,         // ������ �������������� � ����� �����
				// ��� ������ (�������� �� 1 ���. �� �����)
				// �������� ��� ������
	E_FIND_DIR=23,          // ��� ������ ������� �� ����. ����������
				// ���������� ���������� �����
	E_INVALID_PARAM=24,     // �������� ��������� � �������
        E_INVALID_SEQUENCE=25,  // �������� ������������������ ������ (������ ��� ������)
	E_REACH_END_TAPE=26,    // ���������� ����� ����� ����� (��� ������)
	E_ERROR_OR_SKIP_DATA=27,// �� ��� ������ ������� ������ �������
	E_BAD_VCR_TABLE=28,     // ����������� �������� ������� ��
	E_INCOMPATIBLE_LOW_LEVEL_DRIVER=29, // ������������� ������� ������� ������� ������ ( .VxD or .SYS )
	E_UNKNOWN_FORMAT_PT=30, // ����������� ������ ������� ����������������
	E_BAD_PT=31,            // �������� ������� ���������������� (��)
	E_MISMATCH_FORMAT_PT=32,// �������������� �������� ����� � ��
	E_INTERNAL_DRIVE_ERROR=33, // internal drive error
        E_SECTOR_NOT_ACCESSIBLE=34,// ������ ��� ������ ����������( ����� ������� ������� )
	E_UNKNOWN_ERROR         // ����������� ������, ���� >= E_UNKNOWN_ERROR
};

// ��������� ��� ������� ������ AvdIsTape
#define NOT_TAPE        0       // ��� �����
#define TPB_TAPE        1       // ��������� ����� ���������


// ������ ����������� ����� �� ( ��������� �� )
enum {
   VCR_STOP=0           ,          // ��������� STOP
   VCR_PLAY             ,          // ��������� PLAY
   VCR_PLAY_FF          ,          // ���������� ��������������� ������
   VCR_PLAY_REW         ,          // ���������� ��������������� �����
   VCR_nPLAY_FF         ,          // ���������������, ���������� ��������������� ������ (������ ��������)
   VCR_nPLAY_REW        ,          // ���������������, ���������� ��������������� �����  (������ ��������)
   VCR_FF               ,          // ���������� ��������� ������
   VCR_REW              ,          // ���������� ��������� �����
   VCR_REC              ,          // ����� ������ �� �����
   VCR_MAX= VCR_REC                // ���� �������� ������
};



#define VER_PROTO_API   0x103   //

// ����� ������� ����������� �������� ������, ������������ API

 *	1. ���� ���������� �������� �����
 * ���������� �������� ��������������, ���������� ��� ������ ������ ��������
 */

/* ���� ���������� (� ���� attributs) */

#define USED_AUTO_PHASE 0x1	/* ������� ������������ ��������������� */
#define USED_COMPRESS   0x2	/* ������� ������������ ����� �� ������� */
#define USED_XOR        0x4	/* ������� ������������ ���������� ����� XOR */

enum avttype {
	UNKNOWN	= 0,	/* unknown type, driver disabled */
	Av1010	= 1010,	/* 1010 - 100 Kb/s (180min = 1Gb) */
	Av1020	= 1020,	/* 1020 - 200 Kb/s (180min = 2Gb) */
	Av1030	= 1030,	/* 1030 - 200 Kb/s */
	Av1031	= 1031,	/* 1031 - 200 Kb/s, 32 Kw, (300 Kb/s with compression) */
	Av1051	= 1051,	/* 1051 - 325 Kb/s, 64Kw (180min = 3.3Gb) */
	Av1052	= 1052,	/* 1052 - 325 Kb/s, 256 Kw */
};

struct avt_info {		/* Driver Parameters Block */
/* ������������ �������������� � �������� ������ */
	enum avttype controller;	/* ��� ����������� */
	u_short	port;			/* ����� �������� ����� */
	u_short	irq;			/* ���������� */
	u_short	drq;			/* DMA ����� */
	u_char	ver_major,
		ver_minor;		/* ������ �������� */
	u_char	formats;	/* ��������� ������ �������. �������� ����� */
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
/* #define WRITEMASK30	0x01 */	/* ����� ������ */
/* #define READMASK30	0x03 */	/* ����� ������ */
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
/* #define PHASEMASK10	0x00C */	/* 0000000000001100B */
/* #define PHASEMASK20	0x0E0 */	/* 0000000011100000B */
/* #define PHASEMASK30	0x0E0 */	/* 0000000011100000B */
/* #define SETPHASE10(phase)	((phase << 2) & PHASEMASK10) */
/* #define SETPHASE20(phase)	((phase << 5) & PHASEMASK20) */
/* #define SETPHASE30(phase)	((phase << 5) & PHASEMASK30) */
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
 *	3. ���� ���������� �����:
 *
 * ���������� �������������� �����, � ������� ������ �������� �����
 */
struct TPB {
	u_short	format;		/* ������ ����� */
	char	name[8];	/* ��� ����� */
	u_short	ident;		/* ������������� ����� (���������� �����) */
	u_short	length;		/* ����� ����� � ������� */
	u_short	time;		/* ������� ����� �� ����� */
	u_int	sector;		/* ������� ������ �� ����� */
	u_char	Nsector;	/* ���������� �������� � ����� �� ����� */
};

#if 0
     struct TPBNEW {       // Tape Parameters Block ��� ������� FORMAT_RSC
	U16     format;		// ������ �����
	U16     length;		// ����� ����� � �������
	U32     tapeID;		// ��� ����� ( �������������-���������� ����� )
	U8      reserv[sizeof(TPBOLD)-8];     // reserv
     };

     struct TPB {	// ����� ��������� TPB
	U16	format;
	U16	length;
	union {
	  struct {
	    char	name[8];
	    U16		ident;
		U16		resrv;
	    U32		reserv;
	  };
	  U32	tapeID;
	};
     };

//     4. ���� ������� ���������� ���������� ��� ������ � ������
// ����� �������������� ��� ����������� �������� ���������
     struct CurTPB {       // Current Tape Parameters Block
	U16     cur_time;  // ������� ����� �� ������ �����, ���
	U16     targ_time; // ����� ����, ���
	U32     cur_sect;  // ������� ������
	U32     targ_sect; // ������ ����
	U16     state_vcr; // ������� ��������� �� (��������� �� ����)
	// ����� ���� �����, ������������ AVSETUP
	U32     pure_st;   // ���-�� ������ ������ ( ��� ������ )
			   // �� ������ �������� ������
        U32     reserv[4];     // reserv
     };

//      5. ���� ���������� ��������� ����������
     struct DHB {       // Device Hardware parameters Block
	U16     port;   // ������� ����
	U16     dma;    // ����� ������ DMA
	U16     irq;    // ����� ������ IRQ
	U16     busy;   // 0, ���� ����� ���� ������ AvdOpen
	U8      reserv[8];
     };

// �������� ����: ������ ������
#define SIZE_SECTOR_BYTES       512    // ������ ������� ��� ArvidApi
#define SIZE_SECTOR_WORDS       (SIZE_SECTOR_BYTES/2)
#define SIZE_SECTOR_DWORDS      (SIZE_SECTOR_BYTES/4)
typedef char SectData[SIZE_SECTOR_BYTES];
typedef SectData _FAR_ *SectDataBlock; // ��������� �� �������
#endif /* if 0 */

/*
 *======================================================
 * �������� ������� ��������� ����� ��� ������ ������ ��
 * �����, ��� ���� �� �� �������� �� ��������, � �����
 * ������ ����� ������ ������������ ��� �������������.
 *======================================================
 */
#define	SIZEKADR	296 * 13	/* ������������ ���������� ����
					   ������ � ����� ����� 1052 */
struct buf_raw {
	struct buf_raw	*next;	/* ��������� �� ��������� ����� */
	u_short	data[SIZEKADR];	/* ������ �� ����� */
};
#define	NBUFRAW	20		/* ���������� ������� */
/*
 *==================================================
 * ������ ��������� ����� ��� �������������� ������.
 *==================================================
 */
struct buf_sec {
	struct buf_sec	*next;	/* ��������� �� ��������� ����� */
	u_short	data[512/2];	/* ������ ������ ������� */
	u_int	nsector;	/* (00h) ����� ������� �� ����� */
	u_int	time;		/* (04h) ����� ������� �� ����� � ���. */
	u_long	reserv[2];	/* (08h) ������ �� ������� */
	u_int	attr;		/* (18h) ������� �������, ��� �������������� */
#define	DIRECTORY	0x80000000
#define	ERRSECTOR	0xFFFFFFFF
	u_int	crc32;		/* (1ch) crc ������� */
};
#define	NBUFSEC	20*8		/* ���������� ������� � ��������� � ������ */

/* ��������� ������ ������� */
struct avsector {
	u_long	data[512/8];	/* ������ */
	u_int	sector;		/* ����� ������� */
	u_int	CRC;		/* ����������� ����� ��� ������������� */
	u_char	reserv[24];	/* ������ �� �������� */
};

/* fifo new */
typedef struct {
	u_int	count;			/* ����������/����� �������� */
	struct avsector	buf[15*4];	/* 4 ������� � �����, 15 ������ */
} avt_fifo0;

/* fifo old */
typedef struct {
	u_int		count;	/* ���������� ������ */
	avt_kadr100	buf[8]; /* ����� */
} avt_fifo;

/* ��� ������� ������ �� ����� */
typedef struct {
	u_short	data[SIZE1030];
} avt_raw;

#define	SIZE_VCR	0x996	/* ������ VCR ������� */
struct	avt_vcr {
	char	a[SIZE_VCR];	/* ������ ��� ���������� */
	char	vcrload;	/* ����=1 - ������� ��������� */
};

/* ioctl ������� */
#define	VGET_INFO	_IOR('A',  1, struct avt_info)	/* ������ ������ �������� */
struct	set_rw {
	u_int	rate;		/* �������� ������: 100/200/325 */
#define	IORAW	0x0
#define	IODATA	0x1
#define	IOIRC	0x2
	u_char	flag;		/* RAW/IRC/DATA */
};
#define	VSET_READ	_IOW('A',  2, struct set_rw)	/* ������ ������ */
#define	VSET_WRITE	_IOW('A',  3, struct set_rw)	/* ����� ������ */
#define	VSET_VCR	_IOW('A',  4, struct avt_vcr)	/* ��������� VCR ������� */
#define	VGET_TPB	_IOR('A',  6, struct TPB)	/* ������ ���-�� ����� */
#define	VGET_PHASE	_IOR('A',  8, unsigned char)	/* ������ ���� ������ */
#define	VSET_PHASE	_IOW('A',  9, unsigned char)	/* ��������� ���� ������ */

/*
 *------------------------------------------------------------------
 * ��������� ��������� �����
 *------------------------------------------------------------------
 */
#define	U8	unsigned char
#define	U16	unsigned short
#define I32	int
#define U32	unsigned int

/* 1010 */
typedef struct  {
	U8	format;		/* ������ ����� */
	U8	parol1;		/* �������������� ������ */
	U8	name[8];	/* ��� �����, � ��������� I8 (?) */
	U16	ident;		/* �������������� ����� */
	U16	length;		/* ����� �����, ��������� ������/����� ������ */
	U16	tapetime;	/* ����� �� ������ �����, sec */
	I32	sector;		/* ��������� ������ */
	U8	nsect;		/* ����� �������� � ����� */
	U8	parol;		/* ������ */
	U16	tstsum;		/* ����������� ����� */
} HEADER_G;

/* 1020 - Rid-Solomon, ���� nsect */
typedef struct  {
	U8	format;		/* ������ ����� */
	U8	nsect;		/* ����� �������� � ����� */
	U8	name[8];	/* ��� �����, � ��������� I8 (?) */
	U16	ident;		/* �������������� ����� */
	U16	length;		/* ����� �����, ��������� ������/����� ������ */
	U16	tapetime;	/* ����� �� ������ �����, sec */
	I32	sector;		/* ��������� ������ */
	U8	upassword;	/* ������ ������� � ���� usr */
	U8	distance;	/* ��������� �� Log � ���. (255-������) */
	U16	usr;		/* �����. ����: ������ ������ �������� */
	U32	tstsum;		/* ����������� ����� */
} HEADER_RS;
