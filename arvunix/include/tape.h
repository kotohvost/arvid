/*
 * Описание констант и структур используемых в программах работы с Арвид-1000
 */
#include "stdio.h"
/* Про железо */
/* #include "avtreg.h" */

/* форматы лент Арвид */
#define FORMAT_ERROR	0x00	/* отсутствие формата */
#define FORMAT_K4	0x01	/* формат (reserved) */
#define FORMAT_K7	0x02	/* формат 1010 100K/s (old) */
#define FORMAT_K7CRC	0x04	/* формат 1010 100K/s c улучшенным CRC32 */
#define FORMAT_RS	0x08	/* формат 1020,103X 200K/s код Рида-Соломона */
#define FORMAT_RSC	0x10	/* формат 1020,103X >200K/s код Рида-Соломона
				   с возможностью сжатия */
#define FORMAT_RSC_50	0x20	/* формат 105Х >325K/s */

#define PASSWD		0x33	/* пароль внутри кадра 100K/s */
#define PASSWD1		0xCC	/* пароль внутри кадра 100K/s */

/* флаги в длине ленты 100/200 K/s */
#define SPEC_ATTR	0x1000	/* кадр log */
#define TAIL_ATTR	0x2000	/* конец физический записи */
#define HEAD_ATTR	0x4000	/* начало физической записи */
#define	LENGTHMASK	0x03ff	/* маска для длины ленты */
#define	DIR_MARK_C	0x0800	/* кадр с оглавлением в формате Compress */

/* флаги в номере сектора 100/200 K/s */
#define DIR_MARK	0x80000000L	/* кадр с оглавлением */
#define SPEC_MARK	0x40000000L	/* кадр со спец атрибутом */
#define	SECTORMASK	0x3fffffffL	/* маска для сектора */

/* длины блоков кодированных данных в кадрах */
#define	SIZE_G_BLOCK1	21		/* 100K/s */
#define SIZE_G_BLOCK2	1		/* 100K/s */
#define	SIZE_RS_BLOCK1	18		/* 200K/s */
#define SIZE_RS_BLOCK2	1		/* 200K/s */

/* количество секторов в кадре */
#define NUM_G_SECT	4		/* 100K/s */
#define NUM_RS_SECT	8		/* 200K/s */

/* смещения дополнительных групп в кадре */
#define ADD_G_BEG	84		/* 100K/s */
#define ADD_RS_BEG	144		/* 200K/s */

/* поле lenCompress */
#define	NO_COMPRESS	0x8000		/* пирзнак отсутствия компресии
					   количество групп данных */
#define	COMPRESSMASK	0x03ff		/* маска на количество
					   информационных групп */
/*
 *------------------------------------------------------------------
 * Структуры заголовков кадров
 *------------------------------------------------------------------
 */

/* FORMAT_K7CRC - 1010 */
typedef struct {
	u_char	format;		/* формат ленты */
	u_char	parol1;		/* пароль */
	u_char	name[8];	/* имя ленты */
	u_short	ident;		/* индентификатор ленты */
	u_short	length;		/* длина ленты в минутах и атрибуты */
	u_short	tapetime;	/* текущее время в секундах */
	u_int	sector;		/* начальный сектор и атрибуты */
	u_char	nsect;		/* количество секторов в кадре */
	u_char	parol;		/* пароль */
	u_short	tstsum;		/* контрольная сумма заголовка */
} HEADER_G;

/* FORMAT_RS - 1020, 103x - Rid-Solomon, поле nsect */
typedef struct  {
	u_char	format;		/* формат ленты */
	u_char	nsect;		/* число секторов в кадре */
	u_char	name[8];	/* имя ленты, в оригинале I8 (?) */
	u_short	ident;		/* индентификатор ленты */
	u_short	length;		/* длина ленты в минутах и аттрибуты */
	u_short	tapetime;	/* время от начала ленты, sec */
	u_int	sector;		/* начальный сектор */
	u_char	upassword;	/* пароль доступа к полю usr */
	u_char	distance;	/* растояное до Log в сек. (255-далеко) */
	u_short	usr;		/* польз. поле: сектор начала каталога */
	u_int	tstsum;		/* контрольная сумма заголовка */
} HEADER_RS;

/* FORMAT_RSC - 1020, 103x - Rid-Solomon, compression */
typedef struct  {
	u_char	format;		/* формат ленты */
	u_char	nsect;		/* число секторов в кадре */
	u_short	lenCompress;	/* длина сжатой информации, флаг сжатия */
	u_int	tapeID;		/* индентификатор ленты */
	/* резерв под XOR */
	u_char	num_xor;	/* номер текущего кадра в XOR блоке */
	u_char	num_block;	/* номер текущего блока в XOR регионе */
	u_char	nr_xor;		/* количество XOR кадров в XOR блоке */
	u_char	nr_block;	/* количество XOR блоков в XOR регионе */
	u_short	length;		/* длина ленты в минутах и аттрибуты */
	u_short	tapetime;	/* время от начала ленты, sec */
	u_int	sector;		/* начальный сектор */
	u_char	numSpec;	/* номера спец-секторов (один в кадре) */
	u_char	x0, x1, x2;
	u_int	tstsum;		/* контрольная сумма заголовка */
} HEADER_RSC;

/* FORMAT_RSC_50 - 105x - Rid-Solomon, compression */
typedef struct  {
	u_char	format;		/* формат ленты */
	u_char	nsect;		/* число секторов в кадре */
	u_short	lenCompress;	/* длина сжатой информации, флаг сжатия */
	u_int	tapeID;		/* индентификатор ленты */
	/* резерв под XOR */
	u_char	num_xor;	/* номер текущего кадра в XOR блоке */
	u_char	num_block;	/* номер текущего блока в XOR регионе */
	u_char	nr_xor;		/* количество XOR кадров в XOR блоке */
	u_char	nr_block;	/* количество XOR блоков в XOR регионе */
	u_short	length;		/* длина ленты в минутах и аттрибуты */
	u_short	tapetime;	/* время от начала ленты, sec */
	u_int	sector;		/* начальный сектор */
	u_char	numSpec;	/* номера спец-секторов (один в кадре) */
	u_char	x0, x1, x2;
	u_int	CRC;		/* контрольная сумма данных */
	u_char	info[12];	/* первые 12 байт данных */
	u_int	tstsum;		/* контрольная сумма заголовка */
} HEADER_RSC_50;

typedef union {
	HEADER_G	g;	/* Голей */
	HEADER_RS	rs;	/* Рид-Соломон */
	HEADER_RSC	rsc;	/* */
	HEADER_RSC_50	rsc50;	/* */
} UHEADER;

/*
 *------------------------------------------------
 * Дополнительные группы для кодов
 *------------------------------------------------
 */
typedef struct {
	u_int	sect[2][2];	/* остатки от сектора */
	u_int	csum[2];	/* КС сектора */
	u_short	reserv[2];	/* дополнение */
} ADD_RS_BLOCK;

typedef union {
/*	ADD_G_BLOCK	g; */			/* для кода Goley */
	ADD_RS_BLOCK	rs[NUM_RS_SECT/2];	/* 4 в коде RS */
} UADD_BLOCK;

/* атрибуты входа */
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
