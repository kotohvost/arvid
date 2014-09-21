/* $Id: avt.h,v 1.11 1999/09/13 06:09:38 vladimir Exp vladimir $
 *
 *	Файл-описание интерфейса АрВид V0.01beta (Arvid API)
 *  Содержит:
 *	- описание используемых структур данных
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

/* Про железо */
/*
 * Описание констант и структур используемых в программах работы с Арвид-1000
 */
#define	AV_SIZESTR100	 8			/* длина строки 100 K/s */
#define	AV_SIZE100	256*SIZESTR100*2	/* размер кадра 100 K/s */

#define	AV_SIZESTR200	 9			/* длина строки 200 K/s */
#define	AV_SIZE200	284*SIZESTR200*2	/* размер кадра 200 K/s */

#define	AV_SIZESTR325	13			/* длина строки 325 K/s */
#define	AV_SIZE325	300*SIZESTR325*2	/* размер кадра 325 K/s */

#define AV_SIZE1030	4096		/* размер кадра ОЗУ карты в словах */

#define	MAXKADRWSIZE	AV_SIZE1030

/* Все порты используемые устройством 16-ти разрядные
 * ArvidPort	= 270h | 320h
 *
 * чтение/запись
 */
#define	AV30_RDINC(base)	((base) + 0)
#define	AV30_RD(base)		((base) + 2)
/*
 * для записи
 */
#define AV30_RK(base)		((base) + 4)
#define AV30_RA(base)		((base) + 6)
/*
 * для чтение
 */
#define	AV30_RS(base)		((base) + 4)
#define AV30_RS1(base)		((base) + 6)
#define AV30_NPORT		2*4	/* количество используемых портов */

/*
 * это внутренние структуры
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
	u_short	port;			/* номер базового порта */
	u_short	drq;			/* DMA канал */
	u_short	irq;			/* прерывание */
	u_short	busy;
	u_char	reserv[8];
};

struct DPB {		/* Driver Parameters Block */
/* Неизменяемые характеристики в процессе работы */
	enum controller type;	/* тип контроллера */
	u_short	maxrate;	/* макс. скорость передачи [100-200] Кбайт/cек */
#define	MAXRATE10	100	/* 100 Kb/s */
#define	MAXRATE20	200	/* 200 Kb/s */
#define	MAXRATE30	200	/* 200 Kb/s */
#define	MAXRATE50	325	/* 325 Kb/s */
	u_char	maxphase;	/* макс. величина параметра phase */
#define MAXPHASE10	3	/* для 1010 (0-3) */
#define MAXPHASE20	7	/* для 1020 (0-7) */
#define MAXPHASE30	7	/* для 103x (0-7 + A0-A7) */
#define MAXPHASE50	7	/* для 103x (0-7 + A0-A7 + B0-B7) */
	u_char	isautotune;	/* автоподстройка 0/1 */
	u_char	maxtune;	/* макс. величина подстройки фазы */
#define	MAXTUNE10	0
#define	MAXTUNE20	0
#define	MAXTUNE30	1
#define	MAXTUNE50	2
};

struct CDPB {	/* текущие параметры */
	u_short	rate;		/* скорость передачи [100-200] Кбайт/cек */
	u_char	readphase;	/* текущая фаза приема */
#define DEFAULT_PHASE10	3
#define DEFAULT_PHASE20	3
#define DEFAULT_PHASE30 0
#define DEFAULT_PHASE50 1
	u_char	autotune;	/* текущая автоподстройка 0/1 */
	u_char	readtune;		/* текущая подстройка */
#define AUTOTUNE_A	1
#define AUTOTUNE_B	2
};

/*
 * используется для работы через ioctl режим GET_STILL и FREE_STILL
 */
struct still_t {
	U16 	ssize;		/* размер в словах */
	U16	*data;		/* указатель на буфер */
	/* возврашаемые поля */
	U16	rsize;		/* реально прочитано */
	U16	fields;		/* кол-во полей запол/своб для чтен/зап */
};

/*#define	NBUFRAW	40	*/	/* количество буферов */
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
#define	AVGETDHB	_IOR('A',  5, struct DHB)	/* читаем DHB */
#define	AV_6		_IOR('A',  6, unsigned int)	/* pParam */
#define	AV_7		_IOR('A',  7, unsigned)		/* NULL */
#define	AV_8		_IOR('A',  8, unsigned int)	/* pParam */
#define	AVSETPHASE	_IOW('A', 10, unsigned char)	/* Paramlen */
#define	AVSETBUF	_IOW('A', 11, unsigned int)	/* pParam, ParamLen */
/* начать работу с кадрами */
#define	AV_BEGIN_WORK		_IOW('A', 12, unsigned char)
/* получить (захватить) кадр из аппаратуры */
#define	AV_GET_STILL		_IOW('A', 13, struct still_t)
/* немедленно получить кадр (если есть, то получить) */
#define	AV_GET_STILL_IMM	_IOW('A', 14, struct still_t)
/* освободить кадр из аппаратуры */
#define	AV_FREE_STILL		_IOW('A', 15, struct still_t)
/* освободить кадр из аппаратуры (без обмена данными) */
#define	AV_FREE_STILL_EMPTY	_IOW('A', 16, struct still_t)
#define	AV_17		_IOR('A', 17, unsigned)		/* NULL */
#define	AVGETFILLKADR	_IOR('A', 18, unsigned)		/* NULL */
#define	AV_21		_IOR('A', 21, unsigned int)	/* pParam, ParamLen */
#define	AV_22		_IOR('A', 22, unsigned int)	/* ParamLen */
#define	AVVMTOPC	_IOW('A', 23, unsigned)		/* NULL */

/*-----------------05-18-94 11:51am-----------------
        описание регистров кенгуру 1030
        17.06.96 09:29 modified for 1051 (Actel)
--------------------------------------------------*/
#define __D1030REG__
#define __D1051REG__
/*
 *--------------- [ предопределенные константы: ]-------------------------
 *      __K90__  - определена только для XILINX версии 1030
 *      __K91__  - определена только для 1515XM версии 1030
 *      __1040H__ - HOSTовая часть 1040 ACTEL 286-12-512
 *      __1032__ - определена только для ACTEL версии 1030 ( байтовая )
 *      __1051__ - определена только для ACTEL версии 1051 (а также __1032__)
 */
#define	__1032__	1
#define	__1051__	1
/*
 *-----------------[ РЕГИСТР КОМАНД общий ]-----------------------
 *-----------------[      RK        ]-----------------------
 */
/*-------следующие биты используются в режиме 1030 */
#define ___DAN          0x1     /* ИК / ДАН */
#define ___RCV          0x2     /* передача / прием */
/* #define ___200          0x4     // 100 / 200 */
/* #define VIDEO_BUF_4     0x4     // внутренний видеобуфер размером в 4 кадра */
/* #define VIDEO_BUF_16    0       // внутренний видеобуфер размером в 16 кадра */
#define EXTERNAL        0x4     /* установка внешнего аналога */
#define INC_FIELD       0x8     /* инкрементировать счетчик полей */
#define RESET_FIELD     0x10    /* сброс буфера данных */
#define MODE_MASK       0x3     /* 2 бита для изменения режима */
#ifdef __1032__
 #define PHASE_MASK      0xe0   /* маска разрядов фазы */
 #define PHA             0x4    /* включить автоматическую подстройку фазы */
 #define RS_PHA          0x10   /* бит наличия автофазирования в RS */
#else
 #define PHASE_MASK      0x3e0   /* маска разрядов фазы */
#endif
#define MEM_50         0x100   /* 1051 бит увеличения кол-ва памяти */
#define T_250          0x200   /* 1051 бит варианта развертки (333ns/250ns) */
#define START_PHASE     0x10    /* старшие разряды фазы (chip only) */
#define RESET_RING	0x80	/* сброс контроля передачи кольца ( реж TRN ) */
#define FASE_MASK       PHASE_MASK
#define ___CVCR         0x400   /* ВМ работает с кенгуру */
#define FASE_MASK_TRN   0x60    /* маска разрядов фазы для контроля передачи */
#define PHASE_SHIFT     5       /* сдвиг до мл разрядов фазы */
#define APT_SHIFT       11      /* сдвиг до мл разрядов величины сдвига автоподстр фазы */
#define APT_MASK        0x1800  /* маска разрядов автоподстр фазы */
#define DIS_REGENERATE  0x20    /* запрещение регенерации для режима ИК */
/*------- режимы */
#define TRN_DX        (___DAN)          /* режим передачи */
#define RCV           (___RCV^___DAN)   /* режим приема */
#define RCV_RING      RCV               /* режим кольца */
#define RCV_IRC       (___RCV)          /* режим прием ИК */
#define TRN_IRC       (0)         /* режим передачи ИК */
#define NOT_RQST      TRN_DX

/*
 *-----------------[ РЕГИСТР КОМАНД для HOST ]-----------------------
 *-----------------[        RKHST            ]-----------------------
 */
#ifdef __K90__
  #define E_IRQ_20        0x1000  /* разрешение запроса на прерывание 20мс (макет) */
#else
  #define E_IRQ_20        0x800  /* разрешение запроса на прерывание 20мс  (кристалл) */
#endif
#ifdef __1032__         /* RKHST отсутствует */
  #undef E_IRQ_20
#endif
#define INC_TRGSH       0x200  /* инкремент счетчика тест сигнала (кристалл) */
#define RESET_TRGSH     0x400  /* сброс счетчика тест сигнала (кристалл) */
#define SYNC_MASK       0x30   /* ( кристалл only) */
#define SYNC_SHIFT      0x4    /* ( кристалл only) */
#define TEST_MASK       0xf    /* ( кристалл only) */

#define E_IRQ_SLAVE     0x2000  /* разрешение запроса от SLAVE (1040) */
#define RQST_IRQ_SLAVE  0x400   /* запрос на неожиданное прерывание для SLAVE (1040) */
#define INIT_SLAVE      0x4000  /* инициализация процессора SLAVE */
#define SET_BUSY_SLAVE  0x8000  /* установить, что SLAVE занят */

/*
 *-----------------[ РЕГИСТР КОМАНД для SLAVE ]-----------------------
 *-----------------[        RKSLV (1040)      ]-----------------------
 */
#define E_IRQ_20S       0x1000  /* разрешение запроса на прерывание 20мс */
#define E_IRQ_HOST      0x800   /* разрешение запроса от HOST (1040) */
#define RQST_IRQ_HOST   0x2000  /* запрос на неожиданное прерывание для HOST (1040) */
#define INIT_SLAVE      0x4000  /* инициализация процессора SLAVE */
#define CLR_BUSY_SLAVE  0x8000  /* установить, что SLAVE (для HOST) */

/*
 *-----------------[        RS            ]-----------------------
 *                      общая часть
 */
#define ADDR_LOW_MASK   0x3e0   /* маска младших разрядов адреса из РА */
#define ADDR_LOW_SHIFT  5       /* параметр сдвига для младших разрядов адреса */
#define BUSY            0x8000  /* контроллер занят (1040) */
#define IRQ_HOST        0x2000  /* запрос на неожиданное прерывание для HOST (1040) */
#define IRQS            0x800   /* аналог 20 мс прерывания */
/*
 *-----------------[ РЕГИСТР СОСТОЯНИЯ  для HOST ]--------------------
 */
#ifdef __1032__
 #define CNT_FIELD_MASK  0x7     /* маска сколько кадров должен обработать контроллер */
 #define ADD_CNT_FIELD_BIT    0x20 /* __1051__ доп разряд для счетчика кадров */
 #define ADD_CNT_FIELD_BIT_52 0xe0 /* __1052__ доп разряды для счетчика кадров */
 #define SHIFT_ADD_CNT_FIELD  2
#else
 #define CNT_FIELD_MASK  0xf     /* маска сколько кадров должен обработать контроллер */
#endif

#ifdef __1032__
 #define CHECK_RING      0x8    /* бит контроля кольца данных */
#else
 #define CHECK_RING      0x10    /* бит контроля кольца данных */
#endif
#define READY_20        0x2000  /* 20-мс готовность */
#define READY_SLAVE     0x800   /* готовность от SLAVE */

/*
 *-----------------[ РЕГИСТР СОСТОЯНИЯ  для SLAVE ]--------------------
 *-----------------[        RSSLV            ]-----------------------
 */
#define READY_HOST      0x800   /* готовность от HOST */

/*
 *-----------------[ РЕГИСТР АДРЕСА ]--------------------
 *                      RA
 */
#ifdef __1040H__
 #define ADDR_RA_MASK    0xffff                  /* 1040 */
 #define CELL_SHIFT      4
 #define CELL_MASK       0xf
#else
 #ifdef __1032__

  #if !defined(__1051__)
        /* вариант с байтовой памятью (1031) */
   #define ADDR_RA         0x80                   /* кол-во секторов для памати 64К */
   #define ADDR_RA_MASK    (ADDR_RA-1)            /* 1030 */
   #define CELL_SHIFT      9
   #define CELL_MASK       0x1FF
  #else
        /* вариант со словной памятью (1051) */
   #define ADDR_RA         0x100                  /* кол-во секторов для памати 128К */
   #define ADDR_RA_MASK    (ADDR_RA-1)            /* 1051 */
   #define CELL_SHIFT      9
   #define CELL_MASK       0x1FF
  #endif	/* __1051__ */
 #else
        /* вариант со словной памятью */
  #define ADDR_RA         0x100                   /* кол-во секторов для памати 128К */
  #define ADDR_RA_MASK    (ADDR_RA-1)            /* 1030 */
  #define CELL_SHIFT      9
  #define CELL_MASK       0x1FF
 #endif	/* __1032__ */
#endif	/* __1040H__ */
#define ADDR_RA_64      ADDR_RA
#define ADDR_RA_256     (ADDR_RA_MASK+1)        /* (мах РА)+1 для памати 512К */

/*---------[ общие константы для данной аппаратуры ]----------- */
#define SEC_SIZE        256     /* слов в секторе */
#define S_STILL_MAX     4096     /* макс размер кадра в словах */
#define SECT_IN_STILL  (S_STILL_MAX/SEC_SIZE) /* секторов в кадре
			(для 1051 в кадр данных входит еще кусочек) */
#ifdef __1032__
 #define NR_STILL_MAX      (ADDR_RA/SECT_IN_STILL)  /* кадров максимальной длины для 1030 ACTEL */
 #define SEC_NR            (ADDR_RA)     /* кол-во секторов для 1030 ACTEL */
#else
 #define NR_STILL_MAX      (ADDR_RA/SECT_IN_STILL)  /* кадров максимальной длины для 1030 */
 #define SEC_NR            (ADDR_RA)     /* кол-во секторов для 1030 */
#endif	/* __1032__ */
#ifdef __1040H__
 #define NR_STILL_MAX_1040 (256/4) /* кадров максимальной длины для 1040 */
#endif
/*----------  кол-во слов в строке передаваемой аппаратно */

#define TRN_NR          9       /* слов в строке */
#define TRN_IRC_NR      12      /* слов в строке */
#define RCV_IRC_NR      12      /* слов в строке */
#define RCV_NR          9       /* слов в строке */
#define TRN_NR_50      13      /* слов данных в строке для 1051 (250нс) */
#define RCV_NR_50      13      /* слов данных в строке для 1051 (250нс) */

/*----------  кол-во строк передаваемых аппаратно */

#define TRN_LINES       282     /* передаваемых строк */
#define TRN_LINES_IRC   312     /* передаваемых строк ИК !!!! реально 312.5 */
#define RCV_LINES_IRC   312     /* принимаемых  строк ИК !!!! */
#define RCV_LINES       (TRN_LINES+4)  /* принимаемых  строк данных в режиме 1030 */
#define RCV_LINES_K7    256     /* необходимо строk для K7 */
#define RCV_LINES_DIFF_K7_K8 20 /* строк разницы в начале кадра */
/* отличия для 1051: */
#define TRN_LINES_50    296     /* передаваемых строк */
#define RCV_LINES_50    (TRN_LINES_50+4) /* принимаемых  строк данных в режиме 1051 */
/*----------  кол-во слов в кадре передаваемых аппаратно */

#define TRN_STILL_IRC   (TRN_IRC_NR*TRN_LINES_IRC) /* 312 строк */
#define RCV_STILL_IRC   (RCV_IRC_NR*RCV_LINES_IRC) /* 312 строк */
#define TRN_STILL       (TRN_NR*TRN_LINES)
#define RCV_STILL       (RCV_NR*RCV_LINES)
/* отличия для 1051: */
#define TRN_STILL_50    (TRN_NR_50*TRN_LINES_50)
#define RCV_STILL_50    (RCV_NR_50*RCV_LINES_50)

/*
 *----------  кол-во слов в кадре для перехода на следующий кадр
 *----------  ( для коррекции адреса ) длина поля
 */
#define SIZE_FIELD      (4096)  /* кадры выровнены на 8K */
#define S_FIELD_S       (SIZE_FIELD/SEC_SIZE) /* размер поля в секторах */
#define RCV_FIELD       SIZE_FIELD
#define TRN_FIELD       RCV_FIELD
#define TRN_FIELD_IRC   RCV_FIELD /* 312.5 строк */
#define RCV_FIELD_IRC   RCV_FIELD /* 312.5 строк */

/*
 *----------  1030 only
 *---------- максимальное кол-во полей которое может быть
 *---------- получено при считывании RS из CNT_FIELD_MASK
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
/* для 1051 в кадр еще входят сектор из специального кадра */
#define SPEC_RA  ((16-1)*SECT_IN_STILL) /* адрес спец кадра ,
				от которого берется смещение в секторах */

