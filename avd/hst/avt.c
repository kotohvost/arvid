/* $Id: avt.c,v 1.14 1999/09/13 06:09:38 vladimir Exp vladimir $
 *
 * Streamer tape driver for FreeBSD.
 * Supports Arvid 103x boards.
 * Supports Arvid 105x boards.
 *
 * FreeBSD support:	Vladimir Zarozhevsky
 * E-mail:		vladimir@mark-itt.ru
 * Fidonet:		2:5050/3 Work, 2:5050/17 & 2:5050/2.10 Home
 *
 * linux support:	Denis V. Dmitrienko
 * E-mail:		denis@null.net
 *
 * This software is distributed with NO WARRANTIES, not even the implied
 * warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Authors grant any other persons or organisations permission to use
 * or modify this software as long as this message is kept with the software,
 * all derivative works or modified versions.
 *
 */
/*
 * $Log: avt.c,v $
 * Revision 1.14  1999/09/13 06:09:38  vladimir
 * Make ioctl GET_STILL.
 *
 * Revision 1.13  1999/09/11 12:07:55  vladimir
 * Make ioctl BEGIN_WORK.
 *
 * Revision 1.12  1999/09/07 10:08:47  vladimir
 * Add interrupt.
 *
 * Revision 1.11  1999/09/05 09:34:58  vladimir
 * Make testInterr.
 *
 * Revision 1.10  1999/09/05 06:53:24  vladimir
 * Make testMemory.
 *
 * Revision 1.9  1999/09/03 17:59:09  vladimir
 * Add headers from src.
 *
 * Revision 1.8  1999/09/03 09:02:16  vladimir
 * patch linux
 *
 * Revision 1.7  1999/05/10 15:53:14  vladimir
 * Add ioctl.
 *
 * Revision 1.6  1999/05/03 10:56:06  vladimir
 * Add 'RBUF'.
 *
 */

/*
 * Define to enable internal device tracing.
 */
#define	DEBUG

/*
 * NAVT - Number of devices
 */
#ifdef __linux__
#define	NAVT	1
#endif
#ifdef __FreeBSD__
#include	"avt.h"
#endif

#if NAVT > 0		/* количество доступных "сесий" (железок) */

#ifdef __linux__
#include <linux/version.h>
#ifndef LINUX_VERSION_CODE
#error "This driver needs linux kernel 2.0.0 or greater!"
#endif

#if LINUX_VERSION_CODE < 0x020000
#error "This driver needs linux kernel 2.0.0 or greater!"
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/malloc.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/ptrace.h>
#include <asm/signal.h>
#include "avt.h"

#define	CDEV_MAJOR 67
#define	INTREN		enable_irq
#define	INTRDIS		disable_irq

/* Параметры драйвера */
static int irq = 0;
static int io  = 0;
/*static int drq = 0;*/

#if LINUX_VERSION_CODE > 0x020100
MODULE_SUPPORTED_DEVICE("avt");

/*spinlock_t avt_lock = SPIN_LOCK_UNLOCKED;*/

MODULE_PARM(irq, "i");
MODULE_PARM(io, "i");
/*MODULE_PARM(drq, "i");*/
#endif

#define io_outw(port, val) outw(val, port)

#ifdef DEBUG
#define debug(s)		printk s
#else
#define debug(s)		/* remark */
#endif /* DEBUG */

void avtintr(int irq, void *dev_id, struct pt_regs *regs);
#endif /* __linux__ */

#ifdef __FreeBSD__
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>		/* SYSINIT stuff */
#include <sys/conf.h>		/* cdevsw stuff */
#include <sys/malloc.h>		/* malloc region definitions */
#include <machine/clock.h>	/* DELAY() */
#include <i386/isa/isa.h>	/* ISA bus port definitions etc. */
#include <i386/isa/isa_device.h>/* ISA bus configuration structures */
#include <i386/isa/avt.h>	/* avt IOCTL definitions */
#ifdef DEVFS
#include <sys/devfsext.h>	/* DEVFS definitions */
#endif /* DEVFS */

#include <sys/buf.h>
#include <sys/ioctl.h>		/* ioctl operations */
#include <sys/mtio.h>		/* mag tape io control commands */
#include <sys/proc.h>
#include <i386/isa/icu.h>

#define io_outw(port, val) outw(port, val)

#ifdef DEBUG
#define debug(s)		printf s
#else
#define debug(s)		/* remark */
#endif /* DEBUG */

/* Function prototypes (these should all be static except for avtintr()) */
static	d_open_t	avtopen;
static	d_close_t	avtclose;
static	d_read_t	avtread;
static	d_write_t	avtwrite;
static	d_ioctl_t	avtioctl;
/*static	d_mmap_t	avtmmap;*/
/*static	d_select_t	avtselect;*/
static	int		avtprobe (struct isa_device *);
static	int		avtattach (struct isa_device *);
/* void avtintr(int unit);*//* actually defined in ioconf.h (generated file) */

#define	CDEV_MAJOR 20
static struct cdevsw avt_cdevsw = {
	avtopen,
	avtclose,
	avtread,
	avtwrite,
	avtioctl,
	nullstop,
	nullreset,
	nodevtotty,
#if 0
	avtselect,
	avtmmap,
#else
	NULL,
	NULL,
#endif
	NULL,
	"avt",
	NULL,
	-1 };

struct isa_driver avtdriver = {
	avtprobe,
	avtattach,
	"avt" };

static u_int avt_imask = HWI_MASK;	/* маска прерываний при тесте */
#endif /* __FreeBSD__ */

/*===================================================================*/
typedef	unsigned	PORT_ADDR;

typedef	U16	RcvBlock[RCV_STILL];
typedef	U16	RcvBlock50[RCV_STILL_50];

struct avt_unit {
	int	id;		/* надо для интерапта */
	int	unit;
	u_int	hmask;

	/* from Dwb30.h */
	int		fDetect50;	/* обнаружение выполняется перед тестом аппаратуры */
	int		contrNum;
	U32		avh;		/* PDD handler (not used in unix) */
	unsigned	RK_COPY;	/* рабочая копия RK */
	unsigned	RKHOST_COPY;	/* рабочая копия RKHST (1040 ???) */
	/* абстрактные адреса регистров */
	PORT_ADDR	RKHOST;		/* регистр команд HOST (1040 ???) */
	PORT_ADDR	RK;		/* регистр комманд */
	PORT_ADDR	RS;		/* регистр состояния */
	PORT_ADDR	RDINC;		/* регистр данных с инкрементом адреса */
	PORT_ADDR	RD;		/* регистр данных */
	PORT_ADDR	RA;		/* регистр адреса */
	unsigned	lastRA;
	unsigned	mode;		/* command of tape drive */
	U16		*still;		/* буфер под кадр */
	unsigned	wbSsize;		/* размер в словах */
	unsigned	maskRingIrc;	/* маска для трансляции ИК */
	unsigned	phaseR;		/* фаза для режима приема */
	unsigned	phaseT;		/* фаза для режима передачи */
	unsigned	aptR;		/* величина смещения при автоподстройке фазы */
	unsigned	refresh;	/* запрет регенерации */
	unsigned	tcount;		/* текущий счетчик тест-мультиплексора */
	unsigned	avail;
	unsigned	autoPhase;	/* автофазирование поддерживается */
	unsigned	maskRScnt;	/* маска для счетчика RS */
	/*------------------------------------------------------*/
	/* данные переменные изменяются в зависимости от режима */
	unsigned	nrFields;	/* кол-во полей приема (различно для 1051) */
	unsigned	nrBanks;	/* кол-во банком памяти */
	unsigned	memSectors;	/* number of memory sectors */
	/*-----------------------*/
	U32		numbuf;		/* must be before buf!!! (???) */
#if 1
	#define	LINE_RESERV	16
	#ifndef __NOT_ALLOC_WB__
		U16	buf[S_STILL_MAX+SEC_SIZE+LINE_RESERV*2];	/* hardware buf for still */
	#endif
#endif
	unsigned	wb_cur;		/* для теста */

	/* CDriverSession  */
	U32		*numBuff;	/* for record number of buffer (for read) */
	int		inRead;		/* флаг что находимся в операции чтения данных */
	int		inBegin;	/* флаг что находимся в запуске операции */
	int		ds_cur;		/* текущий номер в стеке CRegClass */

	/* from CCQueue.h */
#define	QBlock		RcvBlock
	QBlock		*begin;
	QBlock		*fend;
	QBlock		*fpget;
	QBlock		*fpins;
#define	NR_Q_ELEM	60
	unsigned	nelem;
	unsigned	typeQueue;		/* 0/1 ---- 1031/1051 режимы */
	U32		hMem;

	/* CStillB */
#define	_SHIFT_WORDS	2
	unsigned	sbStoff;	/* текущее смещение в буфере RA */
	U16		*sbStill;	/* указатель на кадр */
	unsigned	sbSsize;	/* размер кадра */
	unsigned	sbSfield;	/* размер поля под кадр >= ssize */
	unsigned	sbCurs;		/* текущий номер кадра в буфере (условно) */
	unsigned	sbhwBufN;	/* number of hardware buffer in HW(Trn) */
	unsigned	sbNumClass;	/* номер класса */
	int		sbRazvert;	/* */

	/* CTrnB */
	unsigned	lastField;

	/* CIrcB */
	unsigned	diffIrcB;	/* кол-во слов необходимое до конца поля */
	unsigned	fnextFieldIrcB;	/* нужно записать в другое поле */
	unsigned	ibFwaitIrq;	/* дождались прерывания */
	unsigned	cnt_emptyIrcB;	/* number Empty IRC stills */

	/* CIrcBRcv */
	unsigned	firstWord;
	unsigned	frcv;		/* признак что ПДП запущен (???) */

	struct	DHB	dhb;
	struct	DPB	dpb;
	struct	CDPB	cdpb;

	u_short	flags;		/* флаг статуса */
#define	VTCLOSE	0x0000
#define	VTOPEN	0x0001

#define	READ200		1
#define	READIRC		4
#define	WRITE200	7
#define	WRITEIRC	8
#define	WRITE325	9
#define	READ325		10
#define VNONE	0		/* none command */
	u_long	countintr;	/* счетчик прерываний от платы */

	u_short	sizekadr;	/* размер кадра в СЛОВАХ для чтения/записи */
				/* разный для режимов 200/325 и ВМ/ИК */
	u_short	datasizew;	/* */
	u_int	nbuf;		/* количество заполненых буферов из карты */

	u_short	RADDRES;	/* текущий адрес в карте Read/Write */
	u_short	MAXADDR;	/* максимальный адрес в карте */
	u_char	myBUSY, NOEXCEP;
	u_char	REQUEST;		/* флаг запроса от прерывания */
	u_char	OVERFLOW;	/* флаг потери данных */

	u_short	maxkadr;	/* max kadr in hardw */
	u_short	curkadr;	/* cur kadr in hardw */
	u_short	ckadr;
	u_int	freekadr;	/* ??? */
	u_int	busy;

/*	struct RBUF	rbuf;*/	/* read buffer */
};

struct avt_unit avttab[NAVT];	/* tape info by unit number */

enum {	M_LOCK, M_RCV, M_QUEST, M_NUMBER, M_RCVIRC, M_RINGIRC, M_RING,
	M_TRN, M_TRNIRC, M_TRN50, M_RCV50, M_MEM_TEST, M_REFRESH_OFF };

/* from src */
/*
 * Class CWorkBuf
 */
#define	bankRA(bank)	(bank * S_FIELD_S)
#define	wbIsReady	wbGetField

/* used */
static unsigned getRS(struct avt_unit *punit)
{	return (unsigned)inw(punit->RS); }

/* used */
static int wbIsMemMax(struct avt_unit *punit)
{	return (punit->RK_COPY & MEM_50); }

/* used */
static unsigned wbGetCurField(struct avt_unit *punit)
{
	unsigned	tmp = getRS(punit);
	if ( !wbIsMemMax(punit) ) {	/* 1031 */
		return (tmp & CNT_FIELD_MASK);
	} else {			/* 1051 */
		return ((tmp & CNT_FIELD_MASK) |
			((tmp & punit->maskRScnt) >> SHIFT_ADD_CNT_FIELD));
	}
	return (tmp);
}

/* used */
static unsigned wbGetField(struct avt_unit *punit)
{
	unsigned	tmp;
	do {
		tmp = wbGetCurField(punit);
		if ( tmp == wbGetCurField(punit))
			break;
	} while ( 1 );
	return (tmp);
}

/* used перевести аппаратуру на след-ее поле */
static void wbIncField(struct avt_unit *punit)
{	io_outw(punit->RK, punit->RK_COPY | INC_FIELD); }

/* used */
static unsigned wbNextNField(struct avt_unit *punit, unsigned n)
{	return ((++n >= punit->nrFields) ? 0 : n); }

/* used записать RA */
static void setRA(struct avt_unit *punit, unsigned dt)
{
	io_outw(punit->RA, punit->lastRA = dt);
	punit->avail = 256;
}

/* used */
static unsigned getRD(struct avt_unit *punit)
{	return (inw(punit->RD)); }

/* used */
static void setRD(struct avt_unit *punit, unsigned dt)
{	io_outw(punit->RD, dt); }

/* used */
static void wbSetBank(struct avt_unit *punit, unsigned bank)
{	setRA(punit, bankRA(bank)); }

/* used считать с текущей позиции RA nr слов */
static void wbRd(struct avt_unit *punit, U16 *addr, unsigned nr)
{
	register unsigned	cnt;

	while( nr ) {
		inw(punit->RD);
		if ( nr < (cnt = punit->avail))
			cnt = nr;
		insw(punit->RDINC, addr, cnt);
		addr += cnt;
		nr -= cnt;
		punit->avail -= cnt;
		if (punit->avail == 0) {
			setRA(punit, ++punit->lastRA);
		}
	}
}

/* used записать с текущей позиции RA nr слов */
static void wbWr(struct avt_unit *punit, U16 *addr, unsigned nr)
{
	register unsigned	cnt;

	while( nr ) {
		/* максимальное кол-во доступных слов */
		if (nr < (cnt = punit->avail))
			cnt = nr;
		outsw(punit->RDINC, addr, cnt);
		addr += cnt;
		nr -= cnt;
		punit->avail -= cnt;
		if ( punit->avail == 0) {
			setRA(punit, ++punit->lastRA);
		}
	}
}

/* used считать с текущей позиции RA nr слов для 1051 */
static void wbRdBank(struct avt_unit *punit, unsigned nbank, U16 *addr, unsigned nr)
{
	unsigned	cnt;

	wbSetBank(punit, nbank);
	cnt = (nr >= S_STILL_MAX) ? S_STILL_MAX : nr;
	nr -= cnt;
	wbRd(punit, addr, cnt);
	if ( nr ) {
		/* размер сектора в последнем спец банке */
		setRA(punit, SPEC_RA + (nbank & (SECT_IN_STILL - 1)));
		wbRd(punit, addr + S_STILL_MAX, nr);
	}
}

/* used */
static int wbIsDetect50(struct avt_unit *punit)
{	return (punit->fDetect50); }

/* used */
static void wbSetMem(struct avt_unit *punit, int set)
{	(set) ? (punit->RK_COPY |= MEM_50) : (punit->RK_COPY &= ~MEM_50); }

/* used */
static void wbSetMemMax(struct avt_unit *punit)
{	wbSetMem(punit, 1); }

/* used default */
static void wbSetMemStd(struct avt_unit *punit)
{	wbSetMem(punit, 0); }

/* used */
static void wbParam31(struct avt_unit *punit)
{	punit->nrFields = 8; punit->nrBanks = 8; }

/* used */
static void wbParam50(struct avt_unit *punit)
{
	punit->nrFields = 16; punit->nrBanks = 16;
	if (punit->contrNum == Av1052)
		{ punit->nrFields = 64; punit->nrBanks = 64; }
}

/* used */
static void wbSetMemParam(struct avt_unit *punit)
{
	if (wbIsDetect50(punit)) {	/* 1031 compatible */
		wbSetMemMax(punit);
		wbParam50(punit);
	} else {			/* 1031 */
		wbSetMemStd(punit);
		wbParam31(punit);
	}
}

/* used */
static void wbSet250(struct avt_unit *punit)
{	punit->RK_COPY |= T_250; }

/* used default */
static void wbSet333(struct avt_unit *punit)
{	punit->RK_COPY &= ~T_250; }

/* used установка соответствующего режима */
static int wbSetMode(struct avt_unit *punit, u_short mod)
{
	/* mask1 содержит только биты режима и фазы */
	register unsigned	mask1 = mod & MODE_MASK;

	/*
	 * на время модификации RK_COPY shared resource
	 * запретим прерывания
	 */
	INTRDIS(punit->dhb.irq);

	/* название режима */
	switch(punit->mode) {
	/*
	 * в зависимости от режима установим следующие параметры
	 * 2 бита режима
	 * кол-во доступных банков памяти + соответствующий бит размера
	 * кол-во доступных полей
	 * тип развертки
	 * впишем значение фазы (если нужно)
	 */
	case M_REFRESH_OFF:	/* used for test */
		mask1 |= DIS_REGENERATE;
		wbSetMemParam(punit);
		wbSet333(punit);
		break;
	default:
	/* остальные режимы иентичны передаче 1030 */
	case M_RINGIRC:
	case M_LOCK:
	case M_QUEST:
	case M_NUMBER:
	case M_TRN:	/* передача с разверткой 1030 */
	old_trn:
		wbSetMemParam(punit);
		wbSet333(punit);
		mask1 |= punit->phaseT & FASE_MASK_TRN;	/* используем фазу передачи */
		break;
	case M_TRN50:
		if ( !wbIsDetect50(punit) )	/* 1031 */
			goto old_trn;
		wbSetMemMax(punit);
		wbParam50(punit);
		wbSet250(punit);
		mask1 |= punit->phaseT & FASE_MASK_TRN;	/* используем фазу передачи */
		break;
	case M_RING:
		mask1 |= punit->phaseR & FASE_MASK;	/* возьмем фазу приема */
	case M_RCVIRC:
	case M_TRNIRC:
		wbSetMemStd(punit);	/* все параметры аналогичны 1031 */
		wbParam31(punit);
		wbSet333(punit);
		break;
	case M_RCV:
	old_rcv:
		wbSetMemParam(punit);
		wbSet333(punit);
		mask1 |= (punit->phaseR & FASE_MASK) | (punit->aptR & APT_MASK);
				/* возьмем фазу приема */
		break;

	case M_RCV50:
		if ( !wbIsDetect50(punit) )	/* 1031 */
			goto old_rcv;
		wbSetMemMax(punit);
		wbParam50(punit);
		wbSet250(punit);
		mask1 |= (punit->phaseR & FASE_MASK) | (punit->aptR & APT_MASK);
				/* возьмем фазу приема */
		break;
	case M_MEM_TEST:
		wbSetMemParam(punit);
		wbSet333(punit);
		break;
	} /* switch (punit->mode) */
	/*
	 * впишем режим и ставим соответствующую фазу (mask1)
	 * остальные биты оставим
	 */
	punit->RK_COPY = (mask1 | (punit->RK_COPY & ~FASE_MASK & ~MODE_MASK & ~APT_MASK));
	io_outw(punit->RK, punit->RK_COPY);
	INTREN(punit->dhb.irq);
	return (0);
}

/* used */
static void wbSetMemTest( struct avt_unit *punit)
{	punit->mode = M_MEM_TEST; wbSetMode(punit, TRN_DX); }

/* used */
static void wbSetTrn(struct avt_unit *punit, int razvert)
{
	punit->mode = (razvert) ? M_TRN50 : M_TRN;
	wbSetMode(punit, TRN_DX);
}

/* used установить адреса */
static void wbInit(struct avt_unit *punit, u_short adr)
{
	punit->fDetect50 = 0;	/* работаем как с 1030 */
	wbParam31(punit);
	punit->RK_COPY = 0;
	if ( adr ) {
		punit->RDINC	= AV30_RDINC(adr);
		punit->RD	= AV30_RD(adr);
		punit->RK	= AV30_RK(adr);
		punit->RA	= AV30_RA(adr);
		punit->RS	= AV30_RS(adr);
		punit->RK_COPY |= TRN_DX;
		punit->RK_COPY |= ___CVCR;
	} else
		punit->RK = punit->RD = punit->RDINC = punit->RA = punit->RS = 0;
}

/* used */
static void setDetect50(struct avt_unit *punit)
{
	punit->fDetect50 = 1;
	punit->maskRScnt = ADD_CNT_FIELD_BIT;
	punit->contrNum = Av1051;
	punit->dpb.type = Av1051;	/* my */
}

/* used */
static void setDetect52(struct avt_unit *punit)
{
	punit->fDetect50 = 1;
	punit->maskRScnt = ADD_CNT_FIELD_BIT_52;
	punit->contrNum = Av1052;
	punit->dpb.type = Av1052;	/* my */
}

/* used определить поддерживается ли Автофазирование */
static int wbIsPHA(struct avt_unit *punit)
{	return (getRS(punit) & RS_PHA); }

/* used включить автоматическую подстройку фазы */
static void wbSetPHA(struct avt_unit *punit)
{	io_outw(punit->RK, punit->RK_COPY |= PHA); }

/* used выключить автоматическую подстройку фазы */
static void wbClrPHA(struct avt_unit *punit)
{	io_outw(punit->RK, punit->RK_COPY &= ~PHA); }

/* used */
/* определить поддерживается ли Автофазирование */
static int existPHA(struct avt_unit *punit)
{
int	ret, i;

	ret = TRUE;
	for (i = 0; i < 50; i++) {
		wbSetPHA(punit);
		if (!wbIsPHA(punit)) { ret = FALSE; break; }
		wbClrPHA(punit);
		if (wbIsPHA(punit)) { ret = FALSE; break; }
	}
	wbClrPHA(punit);
	return (punit->autoPhase = ret);
}

/*
 * обнаружить -- установлена плата 1051 или 1031  fDetect50
 * манипулирую битами RS_PHA = PHA | MEM_50 | T_250
 */
/* used */
static int wbDetect50(struct avt_unit *punit)
{
	int	f1031 = 0;
	int	f1050 = 0;

	wbSet333(punit);
	wbSetMemStd(punit);	/* 0 0 */
	punit->contrNum = Av1031;
	punit->dpb.type = Av1031;	/* my */
	if (!existPHA(punit))
		return (punit->fDetect50 = 0);	/* firstest 1031 */

	/*
	 * должен поддерживать автофазирование
	 * проверим теперь трансляцию бита T_250
	 * 1031 not translate
	 * проверим теперь трансляцию бита MEM_50
	 */
	wbSet250(punit);
	wbSetMemMax(punit);
	wbClrPHA(punit);		/* 1 1 */
	if (!wbIsPHA(punit))
		{ f1031 = 1; }	/* 1031 not translate */
	if (f1031 == 0) {
		punit->contrNum = Av1051;
		punit->dpb.type = Av1051;	/* my */
	}
	wbSetMemStd(punit);
	wbSet250(punit);
	wbClrPHA(punit);		/* 1 0 */
	if (wbIsPHA(punit))
		{ f1050 = 1; }	/* first 1050 as 1031 or 1052 */

	/* для 1050 выполним доп. проверку */
	if (f1050) {
		if (!f1031) {
			punit->contrNum = Av1050;
			punit->dpb.type = Av1050;	/* my */
		}
		wbSet333(punit);
		wbSetMemMax(punit);
		wbClrPHA(punit);	/* 0 1 */
		if (!wbIsPHA(punit)) {
			f1050 = 0;
			punit->contrNum = Av1052;	/* detected */
			punit->dpb.type = Av1052;	/* my */
		}
	}
	wbSet333(punit);
	wbSetMemStd(punit);
	wbClrPHA(punit);		/* 0 0 */
	if ( f1031 || f1050 )	/* first 1050 as 1031 */
		return (punit->fDetect50 = 0);
	if (punit->contrNum == Av1052)
		setDetect52(punit);
	else
		setDetect50(punit);
	return (punit->fDetect50 = 1);
}

/* used сбросим аппаратный счетчик полей */
static void wbClrField(struct avt_unit *punit)
{	io_outw(punit->RK, punit->RK_COPY | RESET_FIELD); }

/* used кадр можно получить от аппаратуры */
static int isWorkingStillRcv(struct avt_unit *punit)
{
	int	ret = wbIsReady(punit);
	if ( ret <= 1 )
		ret = 0;
	return (ret);
}

/* used следующее поле */
static int nextFStillB(struct avt_unit *punit)
{
	wbIncField(punit);
	punit->sbStoff = wbNextNField(punit, punit->sbStoff);
	wbSetBank(punit, punit->sbStoff);
	return 0;
}

/* used определить кол-во секторов памяти */
static unsigned wbGetNrSectors (struct avt_unit *punit)
{
	unsigned	i;

	wbSetMemTest(punit);
	for (i = punit->memSectors = 0; i < 1024; i++) {
		setRA(punit, i);
		setRD(punit, i+1); setRD(punit, i+1);
		getRD(punit);	/* конвеер */
		if ( getRD(punit) != i+1 )
			return (0);
		setRA(punit, 0);
		getRD(punit);	/* конвеер */
		if ( getRD(punit) != 1 )
			return (punit->memSectors = i);	/* заворот памяти */
	}
	return (i);
}

/* CStillB */
#define	getSSizeStillB(punit)	punit->sbSsize
#define	getSTStillB(punit)	punit->sbStoff
#define	nextSStillRcv	nextFStillB
#define	nextSStillTrn	nextFStillB
#define	nextSIrcB	nextSStillTrn

/* used перезапуск аппаратуры */
static void sbReset(struct avt_unit *punit)
{
	wbClrField(punit);
	wbSetBank(punit, punit->sbStoff);
}

/* used перезапуск аппаратуры */
static void stReset(struct avt_unit *punit)
{
	punit->sbStoff = 1;	/* начнем передачу с 1-го поля */
	sbReset(punit);
}

/* used */
static u_short* wbGetStill(struct avt_unit *punit)
{	return (punit->still); }

/* used разметка буфера и начала передачи */
static void beginTrnB(struct avt_unit *punit)
{
	U16		*p;
	unsigned	i;
	int		cnt;

	punit->sbStoff = 1;
	wbSetTrn(punit, punit->sbRazvert);
	stReset(punit);
	wbIncField(punit);
	punit->sbCurs = punit->lastField = 0;
	p = wbGetStill(punit);
	i = 0;
	for ( i = 0; i < _SHIFT_WORDS + 24 * RCV_NR; i++)
		p[i] = 0xAAAA;
	p += i;
	for ( i = 0; i < (256 * RCV_NR); i += 9)
		p[i] = 0xAAAA;
	p += i;
	for ( i = 0; i < 8 * RCV_NR; i++)
		p[i] = 0xAAAA;
	cnt = 0;
	while ( wbGetField(punit) )
		if ( cnt++ > 30000 )
			break;
	stReset(punit);			/* reset again ! */
}

/* used */
static void wbSetTrnIrc(struct avt_unit *punit)
{
	punit->mode = M_TRNIRC;
	wbSetMode(punit, TRN_IRC);
}

/* used очистить кадровый буфер */
#define	wbClrStill(p)	bzero(p->still, p->wbSsize * 2)
#define	sbErase	wbClrStill	/* used прописать кадр нулями */

/* used инициализация передачи ИК-клавиши */
static void beginIrcB(struct avt_unit *punit)
{
	U16	*still_ = wbGetStill(punit);

	punit->cnt_emptyIrcB = 0;
	punit->ibFwaitIrq = 0;
	sbErase(punit);
	stReset(punit);			/* before setBank */
	wbSetTrn(punit, 0);
	wbSetBank(punit, 0);		/* очистим 0 банк */
	wbWr(punit, still_, SIZE_FIELD);
	wbSetTrnIrc(punit);
	stReset(punit);
}

/* CStillQueue */
#define	getFirstQueue(punit)	(U16 *)punit->fpget
#define	getLastQueue(punit)	(U16 *)punit->fpins
#define	insertQueue(punit)	(punit->fpins = nextQueue(punit, punit->fpins))
#define	freeQueue(punit)	(punit->fpget = nextQueue(punit, punit->fpget))
#define	isEmptyQueue(punit)	(punit->fpget == punit->fpins)
#define	isFullQueue(punit)	(punit->fpget == nextQueue(punit, punit->fpins))

/* used */
static void initQueue (struct avt_unit *punit, int Qtype) {
	punit->fpget = punit->fpins = punit->begin;
	punit->typeQueue = Qtype;
	if ( punit->typeQueue ) {	/* 1051 */
		punit->fend = (QBlock *)((char *)punit->begin +
			((punit->nelem * sizeof(QBlock)) / sizeof(RcvBlock50))
			* sizeof(RcvBlock50));
	} else {		/* 1031 */
		punit->fend = (punit->begin + punit->nelem);
	}
}

/* used */
static QBlock *nextQueue(struct avt_unit *punit, QBlock *p)
{
	if ( punit->typeQueue ) {	/* 1051 */
		register QBlock *px = (QBlock *)((char *)p + sizeof(RcvBlock50));
		return (( px >= punit->fend) ? punit->begin : px);
	} else {	/* 1031 */
		return (((++p) >= punit->fend) ? punit->begin : p);
	}
}

/* used */
static void allocQueue(struct avt_unit *punit, unsigned nel)
{
	if (punit->begin == NULL) {
	next:
		punit->begin = (QBlock *)malloc((sizeof(QBlock)+8)*nel, M_DEVBUF, M_NOWAIT);
		if (punit->begin) {
			punit->nelem = nel;
			initQueue(punit, 0);
		} else {
			nel /= 2;
			goto next;
		}
	}
}

/* used */
static void wbSetRcv(struct avt_unit *punit, int razvert)
{
	punit->mode = (razvert) ? M_RCV50 : M_RCV;
	wbSetMode(punit, RCV);
}

/* used */
static unsigned wbGetNrFields(struct avt_unit *punit)
{	return (punit->nrFields); }

/* used */
static void wbWrBank(struct avt_unit *punit, unsigned nbank, U16 *addr, unsigned nr)
{
	unsigned	cnt;

	wbSetBank(punit, nbank);
	cnt = (nr >= S_STILL_MAX) ? S_STILL_MAX : nr;
	nr -= cnt;
	wbWr(punit, addr, cnt);
	if ( nr ) {
/*		setRA(punit, (kadr & 0x0f) + 0xf0);*/
		setRA(punit, SPEC_RA + nbank);	/* номер сектора в последнем спец банке */
		wbWr(punit, addr + S_STILL_MAX, nr);
	}
}

/* used записать кадр */
static void sbWr(struct avt_unit *punit, unsigned bank)
{	wbWrBank(punit, bank, wbGetStill(punit), punit->sbSsize); }

/* used перезапуск аппаратуры */
static void rbReset(struct avt_unit *punit)
{
	int	nrf, i;

	/* _enable() */
	sbErase(punit);
	punit->sbStoff = 0;
	nrf = wbGetNrFields(punit);
	for ( i = 0; i < nrf; i++) {
		if ( i < 2 )
			sbWr(punit, i);
		else
			wbWrBank(punit, i, wbGetStill(punit), punit->sbSsize/4);
	}
	sbReset(punit);
}

/* used */
static void rcvBstartR(struct avt_unit *punit)
{
	wbSetRcv(punit, punit->sbRazvert);
	rbReset(punit);
}

/* used запустить в режиме прием */
#define	beginRcvB	rcvBstartR
#define	getController(punit)	punit->contrNum

/* used */
static void wbSetRcvIrc(struct avt_unit *punit)
{
	punit->mode = M_RCVIRC;
	wbSetMode(punit, RCV_IRC);
}

/* used */
static void sbInitSPart(struct avt_unit *punit)
{
	punit->sbSfield = SIZE_FIELD;	/* размер поля под кадр одинаковый */
	punit->sbStill = wbGetStill(punit);
}

#define	srInitS	sbInitS
/* used инициализировать буфер кадров */
static int sbInitS(struct avt_unit *punit)
{
	/* переменная часть (не всегда инициализировать) */
	punit->sbCurs =		/* текущий номер кадра в буфере (условно) */
	punit->sbStoff = 0;	/* текущее смещение в буфере в словах */
	sbInitSPart(punit);
	return (0);
}

/* used инициализировать буфер кадров */
static int ibRcvInitS(struct avt_unit *punit)
{
	srInitS(punit);
	punit->sbSsize = RCV_STILL_IRC;
	return (0);
}

/* used */
static void beginIrcBRcv(struct avt_unit *punit)
{
	punit->firstWord = 0;
	ibRcvInitS(punit);
	wbSetRcvIrc(punit);
	rbReset(punit);
	punit->frcv = TRUE;	/* setWork(punit) */
}

#define	wbGetMaxField(punit)	(punit->nrFields - 1)
/* used возвращает количество свободных полей */
static unsigned wbIsNrField(struct avt_unit *punit)
{
	unsigned	dat;
	unsigned	x;
	int		dt;
	dat = wbGetMaxField(punit);
	x = wbGetField(punit);
	dt = dat - x - 1;		/* old: dat-x */
	return ((dt < 0) ? 0 : dt);	/* (x >= dat) */
}

/* used кадр можно получить от аппаратуры */
static int isWorkingStillTrn(struct avt_unit *punit)
{	return (punit->sbhwBufN = wbIsNrField(punit)); }

#define	getSStillTrn	isWorkingStillTrn
#define	getSTrnB	getSStillTrn

/* used захватить кадр */
static int getSIrcB(struct avt_unit *punit)
{
	U16	*still_ = wbGetStill(punit);
	int	dis_rec = ( punit->cnt_emptyIrcB > 18 );	/* */
repeat:
	if ( getSStillTrn(punit) == 0 )
		return (0);
	if ( punit->fnextFieldIrcB ) {	/* запишем остаток в начало след кадра */
		if ( punit->diffIrcB < TRN_STILL_IRC ) {
			if ( !dis_rec )
				wbWr(punit,
					still_+punit->sbSsize-punit->diffIrcB,
					punit->diffIrcB);
			punit->fnextFieldIrcB = FALSE;
		} else {	/* случай возникновения избыточного кадра ssize>TRN_STILL_IRC */
			if ( !dis_rec )
				wbWr(punit, still_+punit->sbSsize-punit->diffIrcB,
				TRN_STILL_IRC);
			punit->diffIrcB -= TRN_STILL_IRC;
			if ( !punit->diffIrcB )
				punit->fnextFieldIrcB = FALSE;
			nextSStillTrn(punit);
			goto repeat;
		}
	}
	return (1);
}

/* used считать кадр */
static void sbRd(struct avt_unit *punit, unsigned bank)
{
	U16	*st = wbGetStill(punit);
	if ( punit->sbSsize && st )
		wbRdBank(punit, bank, st, punit->sbSsize);
}

/* used захватить кадр */
static int getSIrcBRcv(struct avt_unit *punit)
{
	int		x;
	U16		*st;
	unsigned	fW;

	if ( !(x = isWorkingStillRcv(punit)) )
		return (0);
	/* _enable() */
	sbRd(punit, punit->sbStoff);
	st = wbGetStill(punit);
	fW = st[0];
	if ( wbIsDetect50(punit) )	/* > 1050 */
		st[0] = punit->firstWord;
	punit->firstWord = fW;

	nextSStillRcv(punit);

	return (x);
}

/* used захватить кадр */
static int getSStillRcv(struct avt_unit *punit)
{
	int	rc;
	if ( !(rc = isWorkingStillRcv(punit)))
		return (0);

	/* _enable() */
	sbRd(punit, punit->sbStoff);
	nextSStillRcv(punit);	/* переведем указатель */
	return (rc);
}

#define	getSRcvB	getSStillRcv

#define	freeSTrnB	freeSStillTrn
#define	freeSEmptyTrnB	freeSEmptyStillTrn
#define	freeSRcvB(x)		/* return */ 0
#define	freeSEmptyRcvB(x)	/* return */ 0
#define	freeSIrcBRcv(x)		/* return */ 0
#define	freeSEmptyIrcBRcv(x)	/* return */ 0
#define	freeSEmptyIrcB	freeSEmptyStillTrn

/* used освободить кадр */
static int freeSStillTrn(struct avt_unit *punit)
{
	/* _enable() */
	sbWr(punit, punit->sbStoff);
	nextSStillTrn(punit);	/* переместим указатель */
	return (0);
}

/* used освободить кадр */
static int freeSEmptyStillTrn(struct avt_unit *punit)
{
	wbSetBank(punit, punit->sbStoff);
	/* _enable() */
	nextSStillTrn(punit);	/* переместим указатель */
	return (0);
}

/* used освободить кадр */
static int freeSIrcB(struct avt_unit *punit)
{
	U16	*still_ = wbGetStill(punit);	/* not still different addr space! (???) */
	unsigned	newdiff = punit->diffIrcB + punit->sbSsize;
	int	dis_rec = (punit->cnt_emptyIrcB > 18);
	if ( newdiff >= TRN_STILL_IRC ) {	/* накопилось достаточно данных */
		/* запишем, сколько осталось места в Hw-буфере */
		if ( !dis_rec )
			wbWr(punit, still_, TRN_STILL_IRC - punit->diffIrcB);
		punit->diffIrcB = newdiff - TRN_STILL_IRC;
		nextSIrcB(punit);	/* переместим указаьель */
		if ( punit->diffIrcB )
			punit->fnextFieldIrcB = TRUE;	/* надо писать в след поле */
	} else {
		if ( !dis_rec )
			wbWr(punit, still_, punit->sbSsize);
		punit->diffIrcB = newdiff;
	}
	return (0);
}

/*======================== from arvid32.asm ============================*/
#if 0

static void
DHBVmToPc(struct avt_unit *punit)
{
	punit->RK_COPY |= HMODEVMPC;
	io_outw(punit->RK, punit->RK_COPY);
}

static void
DHBVmToTv(struct avt_unit *punit)
{
	punit->RK_COPY &= ~HMODEVMPC;
	io_outw(punit->RK, punit->RK_COPY);
}

static void
DHBAutoPhase(struct avt_unit *punit, u_int autophase)
{
	punit->cdpb.readtune = (autophase << 11) & 0x1800;
	if (punit->mode == READ200
		|| punit->mode == READ325) {
		punit->RK_COPY &= ~(0x1800);	/* 1110 0111 0000 0000 */
		punit->RK_COPY |= punit->cdpb.readtune;
		io_outw(punit->RK, punit->RK_COPY);
	}
}

static void
DHBReadKadr(struct avt_unit *punit, u_short kadr, u_short *buf, u_short nr)
{
char	*func = "DHBReadKadr";
u_short	lword, lwordread;

	debug(("%s\n", func));
	wbSetBank(punit, kadr);
	if (nr > MAXKADRWSIZE)
		lword = MAXKADRWSIZE;
	else
		lword = nr;
	lwordread = lword;
	nr -= lwordread;
	wbRd(punit, buf, lwordread);
	if (nr) {
		setRA(punit, (kadr & 0x0f) + 0xf0);
		wbRd(punit, buf + MAXKADRWSIZE, nr);
	}
}

static void
DHBSetPhase(struct avt_unit *punit, u_int phase)
{
	punit->cdpb.readphase = (phase << 5) & 0xE0;
	if (punit->mode == READ200
		|| punit->mode == READ325) {
		punit->RK_COPY &= 0x1F;
		punit->RK_COPY |= punit->cdpb.readphase;
		io_outw(punit->RK, punit->RK_COPY);
	}
}

static u_int
DHBGetFreeKadr(struct avt_unit *punit)
{
	u_int freekadr = wbGetNrFields(punit) - wbGetField(punit) - 1;

	if (freekadr >= 0)
		return (freekadr);
	return (0);
}

#endif	/* 0 */
/*====================== end from arvid32.asm ============================*/

/* used */
static void Srand (struct avt_unit *punit, unsigned x)
{	punit->wb_cur = x; }

/* used */
static U16 Rand (struct avt_unit *punit)
{
#define	POLINOM16	0x100b	/* X^16+X^12+X^3+X+1 */
	if (punit->wb_cur & 0x8000) {
		return (punit->wb_cur = (((punit->wb_cur<<1) ^ POLINOM16) & 0xffff));
	}
	return (punit->wb_cur = ((punit->wb_cur<<1) & 0xffff));
}

/* тесты аппаратуры */
static int testMemory( struct avt_unit *punit )
{
	U16		buf[SEC_SIZE];
	U16		et [SEC_SIZE];
	unsigned	noerr = TRUE;
	int		nsect;
	int		i;
	int		cnt;

	wbSetMemTest(punit);
	nsect = wbGetNrSectors(punit);
	if ( nsect == 0)
		{ noerr = FALSE; }
	Srand(punit, 0xCCCC);
	for (i = 0; i < SEC_SIZE; i++)
		et[i] = Rand(punit);
	for (cnt = 0; cnt < nsect; cnt++) {
		setRA(punit, cnt);
		wbWr(punit, et, SEC_SIZE);
		setRA(punit, cnt);
		wbRd(punit, buf, SEC_SIZE);

		if (memcmp(et, buf, 2*SEC_SIZE))
			{ noerr = FALSE; break; }
	}
	return (noerr);
}

/*
 * тест прерываний
 */
static int testInterr( struct avt_unit *punit ) {
#ifdef __linux__
u_char	irq = punit->dhb.irq;
/* test IRQ */
#if 0
	if(io==0) {
		printk(" I/O base address not set.\n");
		return 0;
	}
	if(irq==0) {
		printk(" IRQ not set.\n");
		return 0;
	}
#endif
	/* это надо вынести в другое место */
	if(check_region(io, 8)) {
		printk(" I/O region in use.\n");
		return 0;
	}
	request_region(io, 8, "avt");

	request_irq(irq, avtintr, SA_INTERRUPT, "avt", punit);

	punit->countintr = 0;
	punit->mode = VNONE;

/*  spin_lock_irqsave(&avt_lock, flags);*/
/*  enable_irq(t->dhb.irq); */
/*  Ругается на несбалансированность enable_irq */
/*  т.е. прерывание уже разрешено и вызывается еще enable_irq */
/*  spin_lock_irqsave, который делает cli, не помогает... */

	punit->flags = VTOPEN;
	udelay(1000000); /* Это в микросекундах? */
			/* Да, должно получиться 1 секунда всего */
	punit->flags = VTCLOSE;

/*  disable_irq(t->dhb.irq); */
/*  spin_unlock_irqrestore(&avt_lock, flags); */

	free_irq(irq, punit);
#endif	/* __linux__ */
#ifdef __FreeBSD__
	int		ret;
	unsigned	count;
	U32		time, t;
	unsigned	cnt = 0;
	unsigned	typeE, type1;
	u_char	irq = ffs(punit->dhb.irq)-1; /* номер irq из битовой маски */

	register_intr(irq, punit->id, 0, (inthand2_t *)avtintr, &avt_imask, punit->unit);

	type1 = typeE = 0;

	(void) splhigh();		/* _disable() */

	wbSetTrn(punit, 0);
	wbClrField(punit);
	/* hi->clrIntCount() */
	punit->countintr = 0;
	/* time = GetRealTime(); */

	INTREN(punit->dhb.irq);		/* enable this irq */
	DELAY (1000000);	/* должно возникнуть прерывание */
	INTRDIS(punit->dhb.irq);	/* disable this irq */

	(void) spl0();			/* _enable() */

	unregister_intr(irq, (inthand2_t *)avtintr);
#endif	/* __FreeBSD__ */

finit1:
	wbSetTrn(punit, 0);
	wbClrField(punit);
	if (!punit->countintr) {
		 debug((" Arvid not interrupt\n"));
		 return(0);
	}
/*debug(("Arvid interrupt\n"));*/
	return (1);
}

/*
 * Probe for the presence of the device.
 * Return the number of ports needed
 */
static int
#ifdef __linux__
avtprobe (int unit)
#endif
#ifdef __FreeBSD__
avtprobe (struct isa_device *dev)
#endif
{
	/* инициализируем порты */
#ifdef __linux__
  struct avt_unit *probe = &avttab[unit];
/*  debug(("avtprobe(%i)\n", unit));*/

	/* Tape controller not found. */
	probe->contrNum = UNKNOWN;

	probe->id   = 0; /* ??? Что такое id? */
		/* номер девайса в системе */
	probe->unit = unit;

/* Сделать через параметры драйвера... Сделано */
/*  debug(("io=%x, irq=%i\n", io, irq));*/
	probe->dhb.port = io;
/*	probe->drq	= drq;*/
	probe->dhb.irq  = irq;

#endif
#ifdef __FreeBSD__
	int unit = dev->id_unit;
	struct avt_unit *probe = avttab + unit;

	/*
	 * Check the unit makes sense.
	 */
	if (unit > NAVT) {
		printf("bad unit (%d)\n", unit);
		return (0);
	}

	/* Tape controller not found. */
	probe->contrNum = UNKNOWN;

	probe->id   = dev->id_id;
	probe->unit = dev->id_unit;

	probe->dhb.port = dev->id_iobase;
/*	probe->drq	= dev->id_drq; */
	probe->dhb.irq  = dev->id_irq;
#endif

/*	debug(("avt:%d", probe->unit));*/

	probe->flags = VTCLOSE;		/* для прерывания */

	/* CDriverSession() */
	probe->inBegin = 0;
	probe->inRead = 0;
	/* CWorkBuf() */
	probe->contrNum = Av1031;
	probe->memSectors = probe->fDetect50 = 0;	/* работаем как с 1030 */
	probe->numbuf = 0;
	probe->still = (U16 *)probe->buf + LINE_RESERV;
	probe->wbSsize = S_STILL_MAX + SEC_SIZE;

	/* CStillsQueueB() */
	probe->nelem = NR_Q_ELEM;	/* уже наверное не надо */
	probe->begin = 0;
	initQueue(probe, 0);

	wbParam31(probe);
	/* allocResource() */
	wbInit(probe, probe->dhb.port);
	/* устанавливаем обработчик прерываний */

	/* initResource() */
	wbDetect50(probe);
	/* Eint() */

	/* testResource() { full(); } */
	/* dma() */
	if ( !testMemory(probe) ) {
		printf ("E_HW_MEM_TEST");
		return (0);
	}
	/* correctDma() */
/*
	probe->RADDRES	= probe->MAXADDR = 0;
	probe->myBUSY	= 1;
	probe->NOEXCEP	= 1;
	probe->REQUEST	= 0;
*/
	if ( !testInterr(probe) ) {
		printf ("E_HW_INT");
		return (0);
	}

	return (1);
}

/*
 * Called if the probe succeeded.
 * We can be destructive here as we know we have the device.
 * we can also trust the unit number.
 */
static int
#ifdef __linux__
avtattach(int unit)
#endif
#ifdef __FreeBSD__
avtattach (struct isa_device *dev)
#endif
{
#ifndef __linux__
	int unit = dev->id_unit;
#endif

	struct avt_unit *punit = avttab + unit;

	/* stop interrupt */
#ifdef __linux__
	diasble_irq(punit->dpb.irq);
#endif
#ifdef __FreeBSD__
	INTRDIS(punit->dhb.irq);
#endif

/*debug(("avtattach()\n"));*/
#if 0	/* next */
	/*
	 * Allocate storage for this instance.
	 */
	scp = malloc(sizeof(*scp), M_DEVBUF, M_NOWAIT);
	if (scp == NULL) {
		printf("avt%d failed to allocate driver storage\n", unit);
		return (0);
	}
	bzero(scp, sizeof(*scp));
	sca[unit] = scp;

	/*
	 * Store whatever seems wise.
	 */
	scp->dev = dev;
#if DEVFS
	scp->devfs_token = devfs_add_devswf(&avt_cdevsw, unit, DV_CHR,
		UID_ROOT, GID_KMEM, 0600, "avt%d", unit);
#endif
#endif	/* next */

	if (punit->contrNum == UNKNOWN) {
		debug(("avt%d: type UNKNOWN, abort.\n", unit));
		return 0;
	}
	debug(("avt%d: type <Arvid %d>\n", unit, punit->contrNum));

/* Для всех */
#if 0
	punit->mode = VNONE;		/* tape is STOP */
	punit->flags = VTCLOSE;		/* tape is close */
	punit->busy = 0;

	switch (punit->dpb.type) {
	case Av1030:
	case Av1031:
	/* 200 Kb/s */
		punit->dpb.maxrate = MAXRATE30;
		punit->dpb.maxphase = MAXPHASE30;
		punit->cdpb.readphase = DEFAULT_PHASE30;
		break;
	case Av1050:
	case Av1051:
	case Av1052:
	/* 325 Kb/s */
		punit->dpb.maxrate = MAXRATE50;
		punit->dpb.maxphase = MAXPHASE50;
		punit->cdpb.readphase = DEFAULT_PHASE50;
		break;
  default:
	/* 100 Kb/s */
		punit->dpb.maxrate = MAXRATE10;
		punit->dpb.maxphase = MAXPHASE10;
		punit->cdpb.readphase = DEFAULT_PHASE10;
		break;
	} /* switch */
	punit->cdpb.rate = punit->dpb.maxrate;
		/* максимальная по умолчанию */

	punit->wbSsize = MAXKADRWSIZE;
#ifdef __linux__
	punit->still = kmalloc(punit->wbSsize * 2, GFP_KERNEL);
#endif
#ifdef __FreeBSD__
	punit->still = malloc(punit->wbSsize * 2, M_DEVBUF, M_NOWAIT);
#endif
#endif

	return (1);
}

/*
 * процедура выполняется во время обработки прерывания
 */
void
#ifdef __linux__
avtintr(int irq, void *dev_id, struct pt_regs *regs)
{
	struct avt_unit *t = (struct avt_unit*)dev_id;
#endif
#ifdef __FreeBSD__
avtintr(int unit)
{
	struct avt_unit	*punit = avttab + unit;
#endif

	/*
	 * well we got an interupt, now what?
	 * Theoretically we don't need to check the unit.
	 */

	punit->countintr ++;	/* увеличим счетчик прерываний */

	if ( !punit->inBegin ) {
		if ( punit->inRead ) {
/*			if ( !(punit->countintr % 100) ) {
				debug(("avtintr %d, isFull=%d, getLast=%08lx, getSsize=%d\n",
					punit->countintr, isFullQueue(punit),
					getLastQueue(punit), getSSizeStillB(punit)));
			}
*/			while (!isFullQueue(punit) && isWorkingStillRcv(punit)) {
/*				INTREN(punit->dhb.irq);*/

				wbRdBank(punit, getSTStillB(punit), getLastQueue(punit),
					getSSizeStillB(punit));

				INTRDIS(punit->dhb.irq);

				nextFStillB(punit);
				insertQueue(punit);
				/* rcvB.freeS() {} */
/*
				debug(("avtintr %d, getST=%d, isFull=%d, getLast=%08lx, getSsize=%d\n",
					punit->countintr,
					getSTStillB(punit),
					isFullQueue(punit),
					getLastQueue(punit),
					getSSizeStillB(punit)));
*/
				INTREN(punit->dhb.irq);
			}
		} /* punit->inRead */
	} /* !punit->inBegin */
	punit->REQUEST = 1;		/* прерывание закончено */
}

/*
 * Ioctl routine.
 */
#ifdef __linux__
int avtioctl(struct inode *inode, struct file *file, u_int cmd, u_long arg) {
#endif
#ifdef __FreeBSD__
int avtioctl (dev_t dev, int cmd, caddr_t data, int flag, struct proc *p) {
#endif

#ifndef DEV_BSIZE
#define DEV_BSIZE 512
#endif /* DEV_BSIZE */

	struct still_t	*st;	/* указатель для STILL */
	int unit = minor (dev);
	struct avt_unit *punit = avttab + unit;
	int	rc = 0;		/* return code */

	switch(cmd) {
#if 0
/*========================= arvid32.sys ======================*/
	case AVVMTOTV:		/* 4 */
		DHBVmToTv(punit);
		punit->busy = 0;
		break;
	case AVSETBUF: {
/*		DHBSetBuf(punit, *(u_short *)data*/
	} /* case AVSETBUF */
		rc = 0;
		break;
	case AVGETFILLKADR:
		rc = wbGetField(punit);
		break;
	case AVGETDHB:
		bcopy(&punit->dhb, (struct DHB *)data, sizeof(punit->dhb));
		rc = 0;
		break;

	case AVSETPHASE: {
		unsigned int	phase, autotune, autophase;

	debug(("AVSETPHASE\n"));
		phase = *(u_char *)data & 0x3f;		/* 0011 1111 */
		autotune = *(u_char *)data & 0x80;	/* 1000 0000 */
		INTRDIS(punit->dhb.irq);
		if (autotune) {
			if (wbIsDetect50(punit)) {
				if (*(u_char *)data & 0x40)
					autophase = 1;	/* phase 'A' ??? */
				else
					autophase = 0;	/* phase 'B' ??? */
				DHBAutoPhase(punit, autophase);
			}
			wbSetPHA(punit);
		} else {
			DHBAutoPhase(punit, 0);
			wbClrPHA(punit);
		}
		DHBSetPhase(punit, phase);
		INTREN(punit->dhb.irq);
	} /* case AVSETPHASE */
		break;
#endif	/* 0 */
	case AV_BEGIN_WORK:		/* начать работу с кадрами */
		/* запретим прерывания */
		INTRDIS(punit->dhb.irq);
		/* установим флаг начала инициализации */
		punit->inBegin = TRUE;
		punit->inRead = FALSE;
		switch(*(u_char *)data) {
		case CLASS_TRN_DATA:
			beginTrnB(punit);
			break;
		case CLASS_TRN_IRC:
			beginIrcB(punit);
			break;
		case CLASS_RCV_DATA:
			beginRcvB(punit);
			if (getController(punit) < 1052) {
				allocQueue(punit, NR_Q_ELEM);
				/* очистим очередь */
				initQueue(punit, punit->sbRazvert);
				punit->inRead = TRUE;
			}
			break;
		case CLASS_RCV_IRC:
			beginIrcBRcv(punit);
			break;
		}
		punit->sbNumClass = *(u_char *)data;
		/* очистим флаг начала инициализации */
		punit->inBegin = FALSE;
		/* разрешим прерывания */
		INTREN(punit->dhb.irq);
		rc = 0;
		break;
	case AV_GET_STILL:
	/* захватить кадр */
		st = (struct still_t *)data;
/*		debug(("GET_STILL::CLASS_RCV_DATA st=%08lx, st.size=%d, st.data=%08lx\n",
			 st, st->ssize, st->data));*/
		switch(punit->sbNumClass) {
		/* ->getS() */
		case CLASS_TRN_DATA:
			rc = getSTrnB(punit);
			break;
		case CLASS_TRN_IRC:
			rc = getSIrcB(punit);
			break;
		case CLASS_RCV_DATA:
#if 0
			if ( st && st->data && st->ssize && !isEmptyQueue(punit)) {
/*				wbRdBank(punit, punit->sbStoff, st->data, st->size); */
				bcopy(getFirstQueue(punit), st->data, st->ssize * 2);
				freeQueue(punit);
#else
			rc = getSRcvB(punit);
#endif
			if ( rc && st && st->data && st->ssize ) {
				bcopy(wbGetStill(punit), st->data, st->ssize * 2);
				st->rsize = st->ssize;
				rc = 0;
			} else
				rc = -1;
			break;
		case CLASS_RCV_IRC:
			rc = getSIrcBRcv(punit);
			if ( rc && st && st->data && st->ssize ) {
				bcopy(wbGetStill(punit), st->data, st->ssize * 2);
				st->rsize = st->ssize;
				rc = 0;
			} else
				rc = -1;
			break;
		}
		break;
	case AV_GET_STILL_IMM:
		st = (struct still_t *)data;
		switch(punit->sbNumClass) {
		/* ->getSimm() */
		case CLASS_TRN_DATA:
			rc = getSTrnB(punit);
			break;
		case CLASS_TRN_IRC:
			rc = getSIrcB(punit);
			break;
		case CLASS_RCV_DATA:
			rc = getSRcvB(punit);
			if ( rc && st && st->data && st->ssize ) {
				bcopy(wbGetStill(punit), st->data, st->ssize * 2);
				st->rsize = st->ssize;
				rc = 0;
			} else
				rc = -1;
			break;
		case CLASS_RCV_IRC:
			rc = getSIrcBRcv(punit);
			if ( rc && st && st->data && st->ssize ) {
				bcopy(wbGetStill(punit), st->data, st->ssize * 2);
				st->rsize = st->ssize;
				rc = 0;
			} else
				rc = -1;
			break;
		}
		break;
	case AV_FREE_STILL:
		st = (struct still_t *)data;
		switch(punit->sbNumClass) {
		/* ->freeS() */
		case CLASS_TRN_DATA:
			rc = freeSTrnB(punit);
			break;
		case CLASS_TRN_IRC:
			rc = freeSIrcB(punit);
			break;
		case CLASS_RCV_DATA:
			rc = freeSRcvB(punit);
			break;
		case CLASS_RCV_IRC:
			rc = freeSIrcBRcv(punit);
			break;
		}
		break;
	case AV_FREE_STILL_EMPTY:
		st = (struct still_t *)data;
		switch(punit->sbNumClass) {
		/* ->freeSempty() */
		case CLASS_TRN_DATA:
			rc = freeSEmptyTrnB(punit);
			break;
		case CLASS_TRN_IRC:
			rc = freeSEmptyIrcB(punit);
			break;
		case CLASS_RCV_DATA:
			rc = freeSEmptyRcvB(punit);
			break;
		case CLASS_RCV_IRC:
			rc = freeSEmptyIrcBRcv(punit);
			break;
		}
		break;
#if 0
	case AVVMTOPC:		/* 23 */
		DHBVmToPc(punit);
		break;
	case AV_8:
	case AV_6:
	case AV_7:
	case AV_21:
		rc = 0;
		break;
#endif	/* 0 */
/*====================== end arvid32.sys ======================*/
	} /* switch(cmd) */

	return (rc);
}

/*
 * Open routine, called on every device open.
 */
static int
#ifdef __linux__
avtopen(struct inode *inode, struct file *file)
#endif
#ifdef __FreeBSD__
avtopen(dev_t dev, int oflags, int devtype, struct proc *p)
#endif
{
	int unit = minor (dev);
	struct avt_unit *punit = avttab + unit;
#if 0
	sc_p scp = sca[unit];

	CHECKUNIT(ENXIO);
#endif
	/*
	 * Do processing
	 */

	/* установим размер */
	punit->sbSsize = (punit->sbRazvert) ? /* 296 */
			(RCV_STILL_50 - 2 * RCV_NR_50) : (RCV_STILL);

	return (0);
}

/*
 * Close routine, called on last device close.
 * Вообще-то, на _каждое_ закрытие драйвера...
 */
#ifdef __linux__
#if LINUX_VERSION_CODE < 0x020100
static void
#else
static int
#endif
avtclose(struct inode *inode, struct file *file)
#endif
#ifdef __FreeBSD__
static int
avtclose(dev_t dev, int fflag, int devtype, struct proc *p)
#endif
{
	int unit = minor (dev);
	struct avt_unit *punit = avttab + unit;
#if 0
	sc_p scp = sca[unit];

	CHECKUNIT(ENXIO);
#endif
	/*
	 * Do processing
	 */
	INTRDIS(punit->dhb.irq);

#ifdef __FreeBSD__
	return (0);
#endif
}

static int
#ifdef __linux__
#if LINUX_VERSION_CODE < 0x020100
avtread(struct inode *inode, struct file *file, char *BUFFER, size_t count)
#else
avtread(struct file *file, char *BUFFER, size_t count, loff_t *ppos)
#endif
#endif
#ifdef __FreeBSD__
avtread(dev_t dev, struct uio *uio, int ioflag)
#endif
{
#if 0
	int unit = UNIT (dev);
	sc_p scp = sca[unit];
	int	toread;

	CHECKUNIT_DIAG(ENXIO);

	/*
	 * Do processing
	 */
	toread = (min(uio->uio_resid, sizeof(scp->buffer)));
	return(uiomove(scp->buffer, toread, uio));
#else
	return (0);
#endif
}

static int
#ifdef __linux__
#if LINUX_VERSION_CODE < 0x020100
avtwrite(struct inode *inode, struct file *file, char *BUFFER, size_t count)
#else
avtwrite(struct file *file, char *BUFFER, size_t count, loff_t *ppos)
#endif
#endif
#ifdef __FreeBSD__
avtwrite(dev_t dev, struct uio *uio, int ioflag)
#endif
{
#if 0
	int unit = UNIT (dev);
	sc_p scp = sca[unit];
	int	towrite;

	CHECKUNIT_DIAG(ENXIO);

	/*
	 * Do processing
	 * write to buffer
	 */
	towrite = (min(uio->uio_resid, sizeof(scp->buffer)));
	return(uiomove(scp->buffer, towrite, uio));
#else
	return (0);
#endif
}
#if 0
static int
avtmmap(dev_t dev, int offset, int nprot)
{
	return (-1);
}

static int
avtselect(dev_t dev, int which, struct proc *p)
{
	return (0);
}
#endif

#ifdef __linux__
static struct file_operations avt_fops = {
  read:    avtread,
  write:   avtwrite,
  open:    avtopen,
  release: avtclose,
  ioctl:   avtioctl,
};

static avt_devsw_installed = 0;

int init_module(void) {
  int i, res = 0;
	if (!avt_devsw_installed) {

/*debug(("init_module()\n"));*/
     for(i=0; i<NAVT; i++) {
       res = avtprobe(i);
/*       debug(("avtprobe()=%i\n",res));*/
       if(res) {
         res = avtattach(i);
/*         debug(("avtattach()=%i\n", res));*/
         if(res) {
           register_chrdev(CDEV_MAJOR, "avt", &avt_fops);
           debug(("avt device registered!\n"));
           res = 1;
         }
       }
     }
		avt_devsw_installed = 1;
	}
  if(res)
    return 0;
  else {
    release_region(io, 8);
    return -EINVAL;
  }
}

void cleanup_module(void) {
  release_region(io, 8);
  unregister_chrdev(CDEV_MAJOR, "avt");
}
#endif /* __linux__ */

#ifdef __FreeBSD__
/*
 * Now for some driver initialisation.
 * Occurs ONCE during boot (very early).
 */
static void
avt_drvinit(void *unused)
{
	dev_t	dev;

	dev = makedev(CDEV_MAJOR, 0);
	cdevsw_add(&dev, &avt_cdevsw, NULL);
}

SYSINIT(avtdev, SI_SUB_DRIVERS, SI_ORDER_MIDDLE+CDEV_MAJOR,
		avt_drvinit, NULL)
#endif /* __FreeBSD__ */

#endif /* NAVT */
