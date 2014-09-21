/* $Id: avt.h,v 1.11 1999/09/13 06:09:38 vladimir Exp vladimir $
 *
 *	����-�������� ���������� ����� V0.01beta (Arvid API)
 *  ��������:
 *	- �������� ������������ �������� ������
 */
/*
 * $Log: avt.h,v $
 * Revision 1.11  1999/09/13 06:09:38  vladimir
 * Make GET_STILL.
 *
 * Revision 1.10  1999/09/11 12:07:55  vladimir
 * Make ioctl BEGIN_WORK.
 *
 * Revision 1.9  1999/09/05 06:53:24  vladimir
 * Make testMemory.
 *
 * Revision 1.8  1999/09/03 17:59:09  vladimir
 * Add headers from src.
 *
 * Revision 1.7  1999/05/10 15:14:21  vladimir
 * Add ioctl.
 * Add Vm to Tv, Pc
 * Add 'RBUF'.
 */
/* define */
typedef	unsigned char	U8;
typedef	unsigned short	U16;
typedef	unsigned int	U32;

/* ��� ������ */
/*
 * �������� �������� � �������� ������������ � ���������� ������ � �����-1000
 */
#define	AV_SIZESTR100	 8			/* ����� ������ 100 K/s */
#define	AV_SIZE100	256*SIZESTR100*2	/* ������ ����� 100 K/s */

#define	AV_SIZESTR200	 9			/* ����� ������ 200 K/s */
#define	AV_SIZE200	284*SIZESTR200*2	/* ������ ����� 200 K/s */

#define	AV_SIZESTR325	13			/* ����� ������ 325 K/s */
#define	AV_SIZE325	300*SIZESTR325*2	/* ������ ����� 325 K/s */

#define AV_SIZE1030	4096		/* ������ ����� ��� ����� � ������ */

#define	MAXKADRWSIZE	AV_SIZE1030

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
 * ��� ���������� ���������
 */
enum controller {
	UNKNOWN	= 0,	/* unknown type, driver disabled */
	Av1010	= 1010,	/* 1010 - 100 Kb/s (180min = 1Gb) */
	Av1020	= 1020,	/* 1020 - 200 Kb/s (180min = 2Gb) */
	Av1030	= 1030,	/* 1030 - 200 Kb/s */
	Av1031	= 1031,	/* 1031 - 200 Kb/s, 32 Kw, (300 Kb/s with compression) */
	Av1050	= 1050, /* ??? */
	Av1051	= 1051,	/* 1051 - 325 Kb/s, 64 Kw, (180min = 3.3Gb) */
	Av1052	= 1052,	/* 1052 - 325 Kb/s, 256 Kw, -//--//- */
};

struct DHB {		/* Device Hardvare Block */
	u_short	port;			/* ����� �������� ����� */
	u_short	drq;			/* DMA ����� */
	u_short	irq;			/* ���������� */
	u_short	busy;
	u_char	reserv[8];
};

struct DPB {		/* Driver Parameters Block */
/* ������������ �������������� � �������� ������ */
	enum controller type;	/* ��� ����������� */
	u_short	maxrate;	/* ����. �������� �������� [100-200] �����/c�� */
#define	MAXRATE10	100	/* 100 Kb/s */
#define	MAXRATE20	200	/* 200 Kb/s */
#define	MAXRATE30	200	/* 200 Kb/s */
#define	MAXRATE50	325	/* 325 Kb/s */
	u_char	maxphase;	/* ����. �������� ��������� phase */
#define MAXPHASE10	3	/* ��� 1010 (0-3) */
#define MAXPHASE20	7	/* ��� 1020 (0-7) */
#define MAXPHASE30	7	/* ��� 103x (0-7 + A0-A7) */
#define MAXPHASE50	7	/* ��� 103x (0-7 + A0-A7 + B0-B7) */
	u_char	isautotune;	/* �������������� 0/1 */
	u_char	maxtune;	/* ����. �������� ���������� ���� */
#define	MAXTUNE10	0
#define	MAXTUNE20	0
#define	MAXTUNE30	1
#define	MAXTUNE50	2
};

struct CDPB {	/* ������� ��������� */
	u_short	rate;		/* �������� �������� [100-200] �����/c�� */
	u_char	readphase;	/* ������� ���� ������ */
#define DEFAULT_PHASE10	3
#define DEFAULT_PHASE20	3
#define DEFAULT_PHASE30 0
#define DEFAULT_PHASE50 1
	u_char	autotune;	/* ������� �������������� 0/1 */
	u_char	readtune;		/* ������� ���������� */
#define AUTOTUNE_A	1
#define AUTOTUNE_B	2
};

/*
 * ������������ ��� ������ ����� ioctl ����� GET_STILL � FREE_STILL
 */
struct still_t {
	U16 	ssize;		/* ������ � ������ */
	U16	*data;		/* ��������� �� ����� */
	/* ������������ ���� */
	U16	rsize;		/* ������� ��������� */
	U16	fields;		/* ���-�� ����� �����/���� ��� ����/��� */
};

/*#define	NBUFRAW	40	*/	/* ���������� ������� */
#define HMODEVMPC	0x400	/* 00000100 00000000 */

/*
 * SETMODE
 */
#define	CLASS_TRN_DATA	1
#define	CLASS_TRN_IRC	2
#define	CLASS_RCV_DATA	3
#define	CLASS_RCV_IRC	4

/*#define AVINSTALL	_IOR('A',  1, unsigned)*/
#define	AVVMTOTV	_IOW('A',  4, unsigned)		/* NULL */
#define	AVGETDHB	_IOR('A',  5, struct DHB)	/* ������ DHB */
#define	AV_6		_IOR('A',  6, unsigned int)	/* pParam */
#define	AV_7		_IOR('A',  7, unsigned)		/* NULL */
#define	AV_8		_IOR('A',  8, unsigned int)	/* pParam */
#define	AVSETPHASE	_IOW('A', 10, unsigned char)	/* Paramlen */
#define	AVSETBUF	_IOW('A', 11, unsigned int)	/* pParam, ParamLen */
/* ������ ������ � ������� */
#define	AV_BEGIN_WORK		_IOW('A', 12, unsigned char)
/* �������� (���������) ���� �� ���������� */
#define	AV_GET_STILL		_IOW('A', 13, struct still_t)
/* ���������� �������� ���� (���� ����, �� ��������) */
#define	AV_GET_STILL_IMM	_IOW('A', 14, struct still_t)
/* ���������� ���� �� ���������� */
#define	AV_FREE_STILL		_IOW('A', 15, struct still_t)
/* ���������� ���� �� ���������� (��� ������ �������) */
#define	AV_FREE_STILL_EMPTY	_IOW('A', 16, struct still_t)
#define	AV_17		_IOR('A', 17, unsigned)		/* NULL */
#define	AVGETFILLKADR	_IOR('A', 18, unsigned)		/* NULL */
#define	AV_21		_IOR('A', 21, unsigned int)	/* pParam, ParamLen */
#define	AV_22		_IOR('A', 22, unsigned int)	/* ParamLen */
#define	AVVMTOPC	_IOW('A', 23, unsigned)		/* NULL */

/*-----------------05-18-94 11:51am-----------------
        �������� ��������� ������� 1030
        17.06.96 09:29 modified for 1051 (Actel)
--------------------------------------------------*/
#define __D1030REG__
#define __D1051REG__
/*
 *--------------- [ ���������������� ���������: ]-------------------------
 *      __K90__  - ���������� ������ ��� XILINX ������ 1030
 *      __K91__  - ���������� ������ ��� 1515XM ������ 1030
 *      __1040H__ - HOST���� ����� 1040 ACTEL 286-12-512
 *      __1032__ - ���������� ������ ��� ACTEL ������ 1030 ( �������� )
 *      __1051__ - ���������� ������ ��� ACTEL ������ 1051 (� ����� __1032__)
 */
#define	__1032__	1
#define	__1051__	1
/*
 *-----------------[ ������� ������ ����� ]-----------------------
 *-----------------[      RK        ]-----------------------
 */
/*-------��������� ���� ������������ � ������ 1030 */
#define ___DAN          0x1     /* �� / ��� */
#define ___RCV          0x2     /* �������� / ����� */
/* #define ___200          0x4     // 100 / 200 */
/* #define VIDEO_BUF_4     0x4     // ���������� ���������� �������� � 4 ����� */
/* #define VIDEO_BUF_16    0       // ���������� ���������� �������� � 16 ����� */
#define EXTERNAL        0x4     /* ��������� �������� ������� */
#define INC_FIELD       0x8     /* ���������������� ������� ����� */
#define RESET_FIELD     0x10    /* ����� ������ ������ */
#define MODE_MASK       0x3     /* 2 ���� ��� ��������� ������ */
#ifdef __1032__
 #define PHASE_MASK      0xe0   /* ����� �������� ���� */
 #define PHA             0x4    /* �������� �������������� ���������� ���� */
 #define RS_PHA          0x10   /* ��� ������� ��������������� � RS */
#else
 #define PHASE_MASK      0x3e0   /* ����� �������� ���� */
#endif
#define MEM_50         0x100   /* 1051 ��� ���������� ���-�� ������ */
#define T_250          0x200   /* 1051 ��� �������� ��������� (333ns/250ns) */
#define START_PHASE     0x10    /* ������� ������� ���� (chip only) */
#define RESET_RING	0x80	/* ����� �������� �������� ������ ( ��� TRN ) */
#define FASE_MASK       PHASE_MASK
#define ___CVCR         0x400   /* �� �������� � ������� */
#define FASE_MASK_TRN   0x60    /* ����� �������� ���� ��� �������� �������� */
#define PHASE_SHIFT     5       /* ����� �� �� �������� ���� */
#define APT_SHIFT       11      /* ����� �� �� �������� �������� ������ ���������� ���� */
#define APT_MASK        0x1800  /* ����� �������� ���������� ���� */
#define DIS_REGENERATE  0x20    /* ���������� ����������� ��� ������ �� */
/*------- ������ */
#define TRN_DX        (___DAN)          /* ����� �������� */
#define RCV           (___RCV^___DAN)   /* ����� ������ */
#define RCV_RING      RCV               /* ����� ������ */
#define RCV_IRC       (___RCV)          /* ����� ����� �� */
#define TRN_IRC       (0)         /* ����� �������� �� */
#define NOT_RQST      TRN_DX

/*
 *-----------------[ ������� ������ ��� HOST ]-----------------------
 *-----------------[        RKHST            ]-----------------------
 */
#ifdef __K90__
  #define E_IRQ_20        0x1000  /* ���������� ������� �� ���������� 20�� (�����) */
#else
  #define E_IRQ_20        0x800  /* ���������� ������� �� ���������� 20��  (��������) */
#endif
#ifdef __1032__         /* RKHST ����������� */
  #undef E_IRQ_20
#endif
#define INC_TRGSH       0x200  /* ��������� �������� ���� ������� (��������) */
#define RESET_TRGSH     0x400  /* ����� �������� ���� ������� (��������) */
#define SYNC_MASK       0x30   /* ( �������� only) */
#define SYNC_SHIFT      0x4    /* ( �������� only) */
#define TEST_MASK       0xf    /* ( �������� only) */

#define E_IRQ_SLAVE     0x2000  /* ���������� ������� �� SLAVE (1040) */
#define RQST_IRQ_SLAVE  0x400   /* ������ �� ����������� ���������� ��� SLAVE (1040) */
#define INIT_SLAVE      0x4000  /* ������������� ���������� SLAVE */
#define SET_BUSY_SLAVE  0x8000  /* ����������, ��� SLAVE ����� */

/*
 *-----------------[ ������� ������ ��� SLAVE ]-----------------------
 *-----------------[        RKSLV (1040)      ]-----------------------
 */
#define E_IRQ_20S       0x1000  /* ���������� ������� �� ���������� 20�� */
#define E_IRQ_HOST      0x800   /* ���������� ������� �� HOST (1040) */
#define RQST_IRQ_HOST   0x2000  /* ������ �� ����������� ���������� ��� HOST (1040) */
#define INIT_SLAVE      0x4000  /* ������������� ���������� SLAVE */
#define CLR_BUSY_SLAVE  0x8000  /* ����������, ��� SLAVE (��� HOST) */

/*
 *-----------------[        RS            ]-----------------------
 *                      ����� �����
 */
#define ADDR_LOW_MASK   0x3e0   /* ����� ������� �������� ������ �� �� */
#define ADDR_LOW_SHIFT  5       /* �������� ������ ��� ������� �������� ������ */
#define BUSY            0x8000  /* ���������� ����� (1040) */
#define IRQ_HOST        0x2000  /* ������ �� ����������� ���������� ��� HOST (1040) */
#define IRQS            0x800   /* ������ 20 �� ���������� */
/*
 *-----------------[ ������� ���������  ��� HOST ]--------------------
 */
#ifdef __1032__
 #define CNT_FIELD_MASK  0x7     /* ����� ������� ������ ������ ���������� ���������� */
 #define ADD_CNT_FIELD_BIT    0x20 /* __1051__ ��� ������ ��� �������� ������ */
 #define ADD_CNT_FIELD_BIT_52 0xe0 /* __1052__ ��� ������� ��� �������� ������ */
 #define SHIFT_ADD_CNT_FIELD  2
#else
 #define CNT_FIELD_MASK  0xf     /* ����� ������� ������ ������ ���������� ���������� */
#endif

#ifdef __1032__
 #define CHECK_RING      0x8    /* ��� �������� ������ ������ */
#else
 #define CHECK_RING      0x10    /* ��� �������� ������ ������ */
#endif
#define READY_20        0x2000  /* 20-�� ���������� */
#define READY_SLAVE     0x800   /* ���������� �� SLAVE */

/*
 *-----------------[ ������� ���������  ��� SLAVE ]--------------------
 *-----------------[        RSSLV            ]-----------------------
 */
#define READY_HOST      0x800   /* ���������� �� HOST */

/*
 *-----------------[ ������� ������ ]--------------------
 *                      RA
 */
#ifdef __1040H__
 #define ADDR_RA_MASK    0xffff                  /* 1040 */
 #define CELL_SHIFT      4
 #define CELL_MASK       0xf
#else
 #ifdef __1032__

  #if !defined(__1051__)
        /* ������� � �������� ������� (1031) */
   #define ADDR_RA         0x80                   /* ���-�� �������� ��� ������ 64� */
   #define ADDR_RA_MASK    (ADDR_RA-1)            /* 1030 */
   #define CELL_SHIFT      9
   #define CELL_MASK       0x1FF
  #else
        /* ������� �� ������� ������� (1051) */
   #define ADDR_RA         0x100                  /* ���-�� �������� ��� ������ 128� */
   #define ADDR_RA_MASK    (ADDR_RA-1)            /* 1051 */
   #define CELL_SHIFT      9
   #define CELL_MASK       0x1FF
  #endif	/* __1051__ */
 #else
        /* ������� �� ������� ������� */
  #define ADDR_RA         0x100                   /* ���-�� �������� ��� ������ 128� */
  #define ADDR_RA_MASK    (ADDR_RA-1)            /* 1030 */
  #define CELL_SHIFT      9
  #define CELL_MASK       0x1FF
 #endif	/* __1032__ */
#endif	/* __1040H__ */
#define ADDR_RA_64      ADDR_RA
#define ADDR_RA_256     (ADDR_RA_MASK+1)        /* (��� ��)+1 ��� ������ 512� */

/*---------[ ����� ��������� ��� ������ ���������� ]----------- */
#define SEC_SIZE        256     /* ���� � ������� */
#define S_STILL_MAX     4096     /* ���� ������ ����� � ������ */
#define SECT_IN_STILL  (S_STILL_MAX/SEC_SIZE) /* �������� � �����
			(��� 1051 � ���� ������ ������ ��� �������) */
#ifdef __1032__
 #define NR_STILL_MAX      (ADDR_RA/SECT_IN_STILL)  /* ������ ������������ ����� ��� 1030 ACTEL */
 #define SEC_NR            (ADDR_RA)     /* ���-�� �������� ��� 1030 ACTEL */
#else
 #define NR_STILL_MAX      (ADDR_RA/SECT_IN_STILL)  /* ������ ������������ ����� ��� 1030 */
 #define SEC_NR            (ADDR_RA)     /* ���-�� �������� ��� 1030 */
#endif	/* __1032__ */
#ifdef __1040H__
 #define NR_STILL_MAX_1040 (256/4) /* ������ ������������ ����� ��� 1040 */
#endif
/*----------  ���-�� ���� � ������ ������������ ��������� */

#define TRN_NR          9       /* ���� � ������ */
#define TRN_IRC_NR      12      /* ���� � ������ */
#define RCV_IRC_NR      12      /* ���� � ������ */
#define RCV_NR          9       /* ���� � ������ */
#define TRN_NR_50      13      /* ���� ������ � ������ ��� 1051 (250��) */
#define RCV_NR_50      13      /* ���� ������ � ������ ��� 1051 (250��) */

/*----------  ���-�� ����� ������������ ��������� */

#define TRN_LINES       282     /* ������������ ����� */
#define TRN_LINES_IRC   312     /* ������������ ����� �� !!!! ������� 312.5 */
#define RCV_LINES_IRC   312     /* �����������  ����� �� !!!! */
#define RCV_LINES       (TRN_LINES+4)  /* �����������  ����� ������ � ������ 1030 */
#define RCV_LINES_K7    256     /* ���������� ����k ��� K7 */
#define RCV_LINES_DIFF_K7_K8 20 /* ����� ������� � ������ ����� */
/* ������� ��� 1051: */
#define TRN_LINES_50    296     /* ������������ ����� */
#define RCV_LINES_50    (TRN_LINES_50+4) /* �����������  ����� ������ � ������ 1051 */
/*----------  ���-�� ���� � ����� ������������ ��������� */

#define TRN_STILL_IRC   (TRN_IRC_NR*TRN_LINES_IRC) /* 312 ����� */
#define RCV_STILL_IRC   (RCV_IRC_NR*RCV_LINES_IRC) /* 312 ����� */
#define TRN_STILL       (TRN_NR*TRN_LINES)
#define RCV_STILL       (RCV_NR*RCV_LINES)
/* ������� ��� 1051: */
#define TRN_STILL_50    (TRN_NR_50*TRN_LINES_50)
#define RCV_STILL_50    (RCV_NR_50*RCV_LINES_50)

/*
 *----------  ���-�� ���� � ����� ��� �������� �� ��������� ����
 *----------  ( ��� ��������� ������ ) ����� ����
 */
#define SIZE_FIELD      (4096)  /* ����� ��������� �� 8K */
#define S_FIELD_S       (SIZE_FIELD/SEC_SIZE) /* ������ ���� � �������� */
#define RCV_FIELD       SIZE_FIELD
#define TRN_FIELD       RCV_FIELD
#define TRN_FIELD_IRC   RCV_FIELD /* 312.5 ����� */
#define RCV_FIELD_IRC   RCV_FIELD /* 312.5 ����� */

/*
 *----------  1030 only
 *---------- ������������ ���-�� ����� ������� ����� ����
 *---------- �������� ��� ���������� RS �� CNT_FIELD_MASK
 */
#ifdef __1032__
 #if defined(__1051__)
  /* 1051 */
  /*#define TRN_MAX_FIELD_50           14 */
  /*#define TRN_IRC_MAX_FIELD_50        7 */
  /*#define RCV_MAX_FIELD_50           14 */
  /*#define RCV_IRC_MAX_FIELD_50       14 */
 #endif	/* __1051__ */
  /* 1031 */
  #define TRN_MAX_FIELD           7
  #define TRN_IRC_MAX_FIELD       7
  #define RCV_MAX_FIELD           7
  #define RCV_IRC_MAX_FIELD       7
#else
 #define TRN_MAX_FIELD           15
 #define TRN_IRC_MAX_FIELD       15
 #define RCV_MAX_FIELD           15
 #define RCV_IRC_MAX_FIELD       15
#endif	/* __1032__ */
/* ��� 1051 � ���� ��� ������ ������ �� ������������ ����� */
#define SPEC_RA  ((16-1)*SECT_IN_STILL) /* ����� ���� ����� ,
				�� �������� ������� �������� � �������� */

