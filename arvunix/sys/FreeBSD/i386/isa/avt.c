/*
 * Streamer tape driver for FreeBSD.
 * Supports Arvid 102x boards.
 * Supports Arvid 103x boards.
 * Supports Arvid 105x boards.
 *
 * Copyright (C) 1996 by Vladimir Zarozhevsky
 *
 * E-mail:  <vladimir@c-press.udm.ru> <vladimir@mark-itt.ru>
 * Fidonet: 2:5050/3 Work, 2:5050/17 & 2:5050/2.10 Home
 *
 * This software is distributed with NO WARRANTIES, not even the implied
 * warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Authors grant any other persons or organisations permission to use
 * or modify this software as long as this message is kept with the software,
 * all derivative works or modified versions.
 *
 * Version 0.1 ALPHA
 *
 */
#define	FREEBSD222	1
#define	ARVID_1020	1

#include "avt.h"
#if NAVT > 0

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/buf.h>
#include <sys/malloc.h>
#include <sys/ioctl.h>		/* ioctl operations */
#include <sys/mtio.h>		/* mag tape io control commands */
#include <sys/proc.h>
#include <sys/conf.h>

#if FREEBSD222
/* #include */
#else
#include <sys/devconf.h> 
#endif

#include <machine/clock.h>
#include <i386/isa/icu.h>
#include <i386/isa/isa_device.h>
#include <i386/isa/isa.h>
#include <i386/isa/avtreg.h>

/*
 * Define to enable internal device tracing.
 */
#define DEBUG
#ifdef DEBUG
#define dprintf(s)		printf s
#else
#define dprintf(s)		/* remark */
#endif

static int avtprobe(struct isa_device *); 
static int avtattach(struct isa_device *);

typedef struct {
	enum controller	arvid;	/* type of controller */
	int	id;		/* надо для интерапта */
	int	unit;
	u_short	port;		/* base i/o port for 1010, 1030-1052 */
	u_short	irq;		/* irq for all */
	u_short	drq;		/* drq for 1020 */
	u_short	maxrate;	/* controller density */
	u_short	rate;
	u_char	maxphase;	/* phase for read */
	u_char	phase;		/* current phase */
	u_short	flags;		/* флаг статуса */
	u_short	cmdport;	/* copy CMDPORT */
	u_char	commands;	/* command of tape drive */
#define VNONE	0		/* none command */
#define	VREAD	1		/* read data */
#define VVCRIN	4		/* VCR IN */
#define VWRITE	7		/* write data */
#define VVCROUT	8		/* VCR OUT */
#define VDELAY	9		/* delay in kadr's */
	u_long	countintr;	/* счетчик прерываний от платы */

	u_int	rawb;		/* количество заполненых буферов из карты */
	void	*rawbuf;	/* указатель на текущий заполненый буфер */

	u_short	sizekadr;	/* размер кадра в СЛОВАХ для чтения/записи */
				/* разный для режимов 200/325 и ВМ/ИК */
#ifdef ARVID_1020
	u_short	sizebuf;	/* размер буфера для кадров в DMA для 1020 */
/* вывод на ИК    100C - 4108  / 13 =  316   (1 буфер) */
/* чтение данных  2BF2 - 11250 /  9 = 1250   (4 буфера) */
/* запись данных  3236 - 12854 / 10 = 1285.4 (4 буфера) */
	u_short	*vaddr, *paddr;	/* буфер DMA для 1020 */
	u_int	dmasize;	/* алокированный размер DMA буфера */
	u_int	dmaflags;	/* B_READ, B_WRITE, B_RAW */
	u_int	dmacount;	/* копия DMA->ADDR */
	int	dmasync;	/* */
	int	dmapar;		/* parity */
	u_char	rphase;		/* ??? */
	u_char	wphase;		/* ??? */
	int	start;		/* начало записи */
#endif

#define	avtsetcmd(data)		outw (t->CMDPORT, data);	/*  */
#define avtsetaddr(data)	outw (t->RA, data);
/*
 * CMDPORT	- регистр комманд
 * RA		- регистр адреса
 * RS		- регистр статуса
 * RD		- регистр данных
 * RDINC	- регистр данных с последующим увеличением адреса
 */
	u_short CMDPORT, RA, RS;
	u_short	RDINC, RD;
	u_short	RADDRES;	/* текущий адрес в карте Read/Write */
	u_short	MAXADDR;	/* максимальный адрес в карте */
	u_char BUSY, NOEXCEP;
	u_char REQUEST;		/* флаг запроса от прерывания */
	u_char OVERFLOW;	/* флаг потери данных */
} avt_unit;

avt_unit avttab[NAVT];			/* tape info by unit number */

static u_int avt_imask = HWI_MASK;	/* маска прерываний при тесте */

static int avtread(avt_unit *, void *, unsigned);
static int avtwrite(avt_unit *, void *, unsigned);
static int avttest(avt_unit *);
static void avtclearram(avt_unit *);
static int avtversion(avt_unit *);
static int ReadNumKadr(avt_unit *);
static void RWDone3x(avt_unit *);
static void ReadArv3x(avt_unit *, void *, size_t);
static void WriteArv3x(avt_unit *, void *, size_t);

static u_short isa_dmacount(u_short);
static void AvMode20(avt_unit *, u_int, u_int);
static u_short RW20(void *, u_int, unsigned);
u_short WriteSync(u_short *pbuf, char oper, char str, u_short cmd);
void PrepWD(u_short *pbuf);
void PrepWriteD(u_short *pbuf, int parity);
void PrepWriteS(u_short *pbuf, int parity);
u_short GetWDsize(int parity);
u_short GetRDsize(int parity);
int checkdma (avt_unit *t);
int checkram (avt_unit *t);
int checksizedma (avt_unit *t);
int checkirq (avt_unit *t);

#ifdef FREEBSD222
static	d_open_t	avtopen;
static	d_close_t	avtclose;
static	d_ioctl_t	avtioctl;
static	d_dump_t	avtdump;
static	d_psize_t	avtsize;
static	d_strategy_t	avtstrategy;

#define	CDEV_MAJOR	33
#define	BDEV_MAJOR	9

static struct cdevsw avt_cdevsw;
static struct bdevsw avt_bdevsw =
	{ avtopen,	avtclose,	avtstrategy,	avtioctl,	/* 9 */
	avtdump,	avtsize,	B_TAPE, "avt",	&avt_cdevsw,	-1 };

#else

static struct kern_devconf kdc_avt[NAVT] = { {
	0, 0, 0,		/* filled in by dev_attach */
	"avt", 0, { MDDT_ISA, 0 },
	isa_generic_externalize, 0, 0, ISA_EXTERNALLEN,
	&kdc_isa0,		/* parent */
	0,			/* parentdata */
	DC_UNCONFIGURED,	/* state */
	"Arvid tape controller",
	DC_CLS_TAPE		/* class */
} };

static inline void
avt_registerdev(struct isa_device *id)
{
/* id для всех одинаковый */
	if(id->id_unit)
		kdc_avt[id->id_unit] = kdc_avt[0];
	kdc_avt[id->id_unit].kdc_unit = id->id_unit;
	kdc_avt[id->id_unit].kdc_parentdata = id;
	dev_attach(&kdc_avt[id->id_unit]);
}
#endif	/* FREEBSD222 */

/*
 * Probe for the presence of the device.
 */
int avtprobe(struct isa_device *id) {
int	rc=0;
/* структура probe для опробывания девайса на его наличие */
avt_unit *probe = avttab + id->id_unit;

#ifndef FREEBSD222
	avt_registerdev(id);
#endif
	/* нету контроллеров */
	/* Tape controller not found. */
	probe->arvid = UNKNOWN;
	probe->id   = id->id_id;
	probe->unit = id->id_unit;
	probe->port = id->id_iobase;
	probe->irq  = id->id_irq;
	probe->drq  = id->id_drq;
	dprintf(("avt:%d", probe->unit));

#ifdef ARVID_1020
	/* Если DRQ в пределах 5-7, значит это 1020 */
	if ((probe->drq > 4) && (probe->drq < 8)) {
		probe->drq = id->id_drq;
		probe->arvid = Av1020;
		probe->wphase = 0x79;
		probe->rphase = 0;
		isa_dmainit(probe->drq, 12996*2);
	/* DMA */
		rc = checkdma(probe);
	/* RAM board */
		if (!rc)
			rc = checkram(probe);
	/* DMA size */
		if (!rc)
			rc = checksizedma(probe);
	/* IRQ */
		if (!rc)
			rc = checkirq(probe);

		if (!rc)
			rc = 1;
		else
			rc = 0;
	} else
#endif
	{
/* #ifdef AV1030 */
/* пробуем Arvid-1030 */
		probe->RA	= AV30_RA(probe->port);
		probe->RS	= AV30_RS(probe->port);
		probe->CMDPORT	= AV30_RK(probe->port);
	/* 0000 0111  0000 0011 */
		probe->cmdport	= 0x0703;			/* Read BM */
		probe->RDINC	= AV30_RDINC(probe->port);
		probe->RD	= AV30_RD(probe->port);
		probe->RADDRES	= probe->MAXADDR = 0;
		probe->BUSY	= 1;
		probe->NOEXCEP	= 1;
		probe->REQUEST	= 0;
		rc = avttest(probe);
/* #endif AV1030 */
		}
	dprintf(("\n"));
	return (rc);
}

/*
 * Test's & prog's for 1020
 */
/*
 *  Register definitions for DMA controller 2 (channels 4..7):
 */
#define	DMA2_CHN(c)	(IO_DMA2 + 2*(2*(c)))	/* addr reg for channel c */
#define DMA2_REQ	(IO_DMA2 + 2*9)		/* soft request DMA start */
#define	DMA2_SMSK	(IO_DMA2 + 2*10)	/* single mask register */
#define	DMA2_MODE	(IO_DMA2 + 2*11)	/* mode register */
#define	DMA2_FFC	(IO_DMA2 + 2*12)	/* clear first/last FF */

/*
 * считывание регистра счетчика канала DMA
 */
u_short isa_dmacount(u_short chan) {
int	rport, fl;
u_short	h, l;

	rport = DMA2_CHN(chan - 4);
	fl = 0;
	do {
		l = inb(rport+2);
		h = inb(rport+2);
		if (h == 0xff)
			fl ^= 1;
	} while (fl);
	return (l + (h << 8) + 1);
}
/*
 * установка чтения/записи DMA канала
 * 0  - данные выведены
 * !0 - количество слов в очереди
 */
u_short RW20(void *buf, u_int count, unsigned chan) {
u_short	rc, nc;

	isa_dmastartq(B_WRITE, (caddr_t)buf, count*2, chan);
	/* wait done or timeout */
	for (nc = 0; (rc = isa_dmacount(chan)) && nc < 5000; nc++);
	isa_dmadone(B_WRITE, (caddr_t)buf, count*2, chan);

	if  (rc) {
		isa_dmastartb(B_WRITE, (caddr_t)buf, count*2, chan);
		if  (isa_dmacount(chan)) {
			outb (DMA2_REQ, chan);
			DELAY(100);
			outb (DMA2_REQ, chan & 3);
		}
		for (nc = 0; (rc = isa_dmacount(chan)) && nc < 5000; nc++);
		isa_dmadone(B_WRITE, (caddr_t)buf, count*2, chan);
	}
	return(rc);
}

#define memsetw(buf, ch, sizew) \
		asm("push %%edi; rep; stosw; pop %%edi;"\
		:\
		:"D" (buf), "a" (ch), "c" (sizew));

/*
 * comm - номер команды
 * reg  - параметр
 */
void AvMode20(avt_unit *t, u_int comm, u_int reg) {
static	u_short	commdma[24];
u_short	flag;
u_int	i, temp1, temp2, temp3, strsize;

	temp1 = reg & 0x8000;
	temp2 = reg & 0x7fff;
	if (temp2 != 0x79)	temp3 = temp2 & 0x6;
	else			temp3 = temp2;
	if ((temp3 == 0) || (temp3 == 0x6)) {
	/* чтение - установим фазу */
		t->cmdport = ((temp2 & 0xff1f) | ((t->phase<<5) & 0xe0));
		t->rphase = t->cmdport;
	} else {
	/* запись - какая-то другая константа */
		t->cmdport = ((temp2 & 0xff1f) | (t->wphase & 0xe0));
		t->wphase = t->cmdport;
	}

	/* выведем команду в карту без автоинициализации DMA */
	memsetw(commdma, 0x79, 10);
	RW20((caddr_t)commdma, 10, t->drq);
	/*
	 * старший бит в слове = 1
	 * и команды 8 или 4 - выход
	 */
	if ((!temp1) && ((comm == 4) || (comm == 8))) goto Commdone;
	memsetw(commdma+10, t->cmdport, 13);
	switch (comm) {
		/* чтение из карты */
		case 1:	i=1; strsize = 20; break;
		/* чтение ИК */
		case 2:	i=0; strsize = 20; break;
		case 3:	i=0; strsize = 20; break;
		/* вывод ИК */
		case 4:	i=1; strsize = 23; break;
		case 5:	i=1; strsize = 23; break;
		case 6:	i=1; strsize = 20; break;
		case 7:	i=1; strsize = 20; break;
		/* вывод ИК */
		case 8:	i=1; strsize = 23; break;
		case 0:
		default: i=0; strsize = 20; break;
	}
	/* выводим команды */
#if 0
	for (flag = 0; flag < 24; flag++)
		dprintf(("%x ", commdma[flag]));
	dprintf(("\n"));
#endif
	do {
		RW20((caddr_t) commdma, strsize, t->drq);
	} while (i--);
Commdone:
	/* set autoinitialize */
	t->dmaflags |= B_RAW;
}
/*
 * Тест наличия карты на указанном DMA
 * 0 - DMA работает
 */
int checkdma(avt_unit *t) {
static	u_short	chkdma[10];
int	i, dflag, count;

dprintf((" DMA"));
	count = 1;
	AvMode20(t, 7, 0x79);
	memsetw(chkdma, 0x79, 10);	/* 111 1001 */
	dflag = B_WRITE;
	do {
		isa_dmastartq(dflag, (caddr_t)chkdma, 10*2, t->drq);
		DELAY(1000000/18*2);		/* 2 сек. */
		isa_dmadone(dflag, (caddr_t)chkdma, 10*2, t->drq);
	} while (count-- && isa_dmacount(t->drq));
	return(isa_dmacount(t->drq));
}

/*
 * П/П проверки ОЗУ контроллера
 * 0 - Ok
 */
int checkram(avt_unit *t) {
int	i, count, rc;
int	si;
short	tstbuf1[16];
short	tstbuf2[20];

dprintf((" RAM"));
	count = 0;
	rc = 0;
	do {
		si = 1 << (count & 0x0f);
		if ((count & 0x1f) >= 16)
			asm("not %%di": "=D" (si): "D" (si));
/*			si = -si-1; */		/* как получить not si ??? */
		AvMode20(t, 7, 0x79);
		for (i = 0; i < 16; i++) {
			tstbuf1[i] = si;
			asm("rol $1, %%di": "=D" (si): "D" (si));
		}
		tstbuf1[0] = t->cmdport;
		isa_dmastartq(B_WRITE, (caddr_t)tstbuf1, 10*2, t->drq);
		while (isa_dmacount(t->drq));
		isa_dmadone(B_WRITE, (caddr_t)tstbuf1, 10*2, t->drq);
/*	printf("\nWrite:\t"); for (i = 0; i < 11; i++) printf(" %04x", tstbuf1[i]);
*/
		isa_dmastartq(B_READ, (caddr_t)tstbuf2, 20*2, t->drq);
		while (isa_dmacount(t->drq));
		isa_dmadone(B_READ, (caddr_t)tstbuf2, 20*2, t->drq);

		if (memcmp(tstbuf1+1, tstbuf2+1, 9*2)) {
	printf("\nRead:\t"); for (i = 0; i < 11; i++) printf(" %04x", tstbuf2[i]);
			if (count)
				rc = 1;
		}
	} while (!rc && (count++ < 256));
	return(rc);
}

/*
 * П/П определения работоспособности ДМА канала
 * 0 - Ok
 */
int checksizedma(avt_unit *t) {
u_short	*tstdma;
int	i, rc;

dprintf((" SIZE"));
	tstdma = malloc(0x2000*2, M_DEVBUF, M_NOWAIT);
	memsetw(tstdma, 0x5aa5, 0x2000);

	isa_dmastartb(B_READ, (caddr_t)tstdma, 0x2000*2, t->drq);
	while (isa_dmacount(t->drq));
	isa_dmadone(B_READ, (caddr_t)tstdma, 0x2000*2, t->drq);

	for (i = 0, rc = 0; !rc && i < 0x2000; i++)
		if (tstdma[i] == 0x5aa5)
			rc = 1;

	free(tstdma, M_DEVBUF);
	return(rc);
}

#define	AV1020DMAREQ	0xFFBF
#define AV1020CLBIT6	0xFFBF
#define	AV1020DMAIRQ	0xFFDF
#define AV1020CLBIT5	0xFFDF

void PrepWriteD(u_short *pbuf, int parity) {
int	str, c;
u_short	cmd;

	cmd = 0x79;
	if (parity)
		str = 12;
	else
		str = 13;
	for (c = 0; c < str; c++) {
		*pbuf++ = cmd;
		*pbuf++ = 0xAAAA;
		memsetw(pbuf, 0x0000, 8);
		pbuf += 8;
	}
	if (parity)
		str = 9;
	else
		str = 8;
	for (c = 0; c < str; c++) {
		*pbuf++ = cmd;
		memsetw(pbuf, 0xAAAA, 9);
		pbuf += 9;
	}
	for (c = 0; c < 256; c++) {
		*pbuf = cmd;
		pbuf += 10;
	}
	*pbuf++ = cmd;
	memsetw(pbuf, 0xAAAA, 9);
	pbuf += 9;
	for (c = 0; c < 4; c++) {
		*pbuf++ = cmd;
		memsetw(pbuf, 0x0000, 9);
		pbuf += 9;
	}
	*pbuf++ = cmd;
	memsetw(pbuf, 0xFFFF, 9);
}

u_short WriteSync(u_short *pbuf, char oper, char str, u_short cmd) {
u_short	rc = str*13;

	while (str--) {
		*pbuf = cmd;
		memsetw(pbuf+1, 0xFFFF, 12);
		switch(oper) {
		case 0:
			pbuf[10] = 0x0F;
			pbuf[11] = 0xFFFC;
			break;
		case 1:
			pbuf[4] = pbuf[10] = 0xF80F;
			break;
		case 2:
			pbuf[4] = 0xF80F;
			pbuf[10] = 0x0F;
			pbuf[11] = pbuf[12] = 0;
			break;
		case 3:
			memsetw(pbuf+1, 0x0000, 12);
			pbuf[3] = pbuf[9] = 0xFFC0;
			pbuf[4] = pbuf[10] = 0x0F;
			break;
		case 4:
			pbuf[1] = pbuf[2] = 0;
			pbuf[3] = 0xFFC0;
			pbuf[4] = pbuf[10] = 0xF80F;
			break;
		case 5:
			pbuf[10] = 0xF80F;
			break;
		case 6:
			pbuf[4] = pbuf[10] = 0x0F;
			pbuf[9] = 0xFFC0;
			pbuf[5] = pbuf[6] = pbuf[7] = pbuf[8] = pbuf[11] = pbuf[12] = 0;
			break;
		case 7:
			pbuf[3] = pbuf[9] = 0xFFC0;
			pbuf[4] = 0x0F;
			pbuf[10] = 0xF80F;
			pbuf[1] = pbuf[2] = pbuf[5] = pbuf[6] = pbuf[7] = pbuf[8] = 0;
			break;
		default:
			break;
		}
		pbuf += 13;
	}
	return(rc);
}

void PrepWriteS(u_short *pbuf, int parity) {
char	*ppar;
int	psize;
static char	odd[]  = {4, 0, 1, 5, 2, 1, 1, 6, 1, 3, 1, 7,  2, 1, 18, 0};
static char	even[] = {4, 0, 2, 1, 1, 2, 2, 3, 1, 4, 2, 1, 17, 0};

	pbuf += 5660/2;
	if (parity) {
		ppar = odd;
		psize = 8;
	} else {
		ppar = even;
		psize = 7;
	}
	while (psize-- > 0)
		pbuf += WriteSync(pbuf, *ppar++, *ppar++, 0x7C);
}

u_short GetWDsize(int parity) {
	if (parity)
		return(3220);
	else
		return(3207);
}

u_short GetRDsize(int parity) {
	if (parity)
		return(313*9);
	else
		return(312*9);
}

void PrepWD(u_short *pbuf) {
int	parity, nk;

	parity = 1;
	for (nk = 0; nk < 4; nk++) {
		memsetw(pbuf, 0xAAAA, GetWDsize(parity));
		PrepWriteD(pbuf, parity);
		PrepWriteS(pbuf, parity);
		pbuf[0] &= AV1020DMAREQ;
		pbuf[5660/2] &= AV1020DMAREQ;
		pbuf[5660/2 + 13] &= AV1020DMAREQ;
		pbuf[5660/2 + 26] &= AV1020DMAIRQ;
		pbuf += GetWDsize(parity);
		parity ^= 1;
	}
};

/*
 * Инициализация буфера для вывода IRC
 * 249 * 13
 */
void InitWriteIrcBuf(u_short *buf);
void InitWriteIrcBuf(u_short *buf) {
u_short	*pbuf;
int	nk, str;

	pbuf = buf;
	dprintf(("InitWriteIrcBuf();"));
	for (nk = 0; nk < 4; nk++) {
		dprintf(("."));
		memsetw(pbuf, 0xFFFF, 249 * 13);
		for (str = 0; str < 249; str++)
			pbuf[str*13] = 0x75;
		pbuf[13] &= AV1020DMAREQ;
		pbuf[249*13-13] &= AV1020DMAIRQ;
		pbuf += 249 * 13;
	}
	pbuf = buf;
	for (nk = 0; nk < 13; nk++) {
		*pbuf = 0x79;
		pbuf += 10;
	}
	*pbuf |= 0x08;
	buf[0] &= AV1020CLBIT6;
	buf[10] &= AV1020CLBIT5;
	dprintf(("\n"));
};

/*
 * Test IRQ.
 * 0 - Ok
 */
int checkirq(avt_unit *t) {
u_short	*tstdma;
u_char	irq = ffs(t->irq)-1;
int	s;

#define	KADR1020b	12854*2
dprintf((" IRQ"));

	AvMode20 (t, 7, 0x79);

	tstdma = malloc(KADR1020b, M_DEVBUF, M_NOWAIT);
/*	memsetw(tstdma, 0xa5a5, KADR1020b/4); */
	PrepWD(tstdma);

	register_intr(irq, t->id, 0, (inthand2_t *)avtintr, &avt_imask, t->unit);
	t->countintr = 0;
	t->commands = VNONE;
	t->flags = VTOPEN;
	isa_dmastartq(B_WRITE|B_RAW, (caddr_t)tstdma, KADR1020b, t->drq);
	isa_dmadone(B_WRITE|B_RAW, (caddr_t)tstdma, KADR1020b, t->drq);

	s = splbio();
	spl0();
	INTREN(t->irq);
	DELAY(1000000);
	AvMode20 (t, 0, 0x38);
	t->flags = VTCLOSE;
	INTRDIS(t->irq);
	splx(s);
	unregister_intr(irq, (inthand2_t *)avtintr);

	free(tstdma, M_DEVBUF);
dprintf((":%d", t->countintr));
	if (!t->countintr) {
		dprintf ((" Arvid not interrupt\n"));
		return(1);
	}
	return (0);
}

/*
 * Test for 1052, 1051, 1031
 * input:
 *	struct avt_unit
 * output:
 *	1031, 1051, 1052, 1030
 */
int avtversion (avt_unit *t) {
/* 1052 */
dprintf((" 1052 "));
	avtsetcmd((t->cmdport & 0xfcfb) | 0xfefb);	/* set bit 9 */
	t->sizekadr = 300 * 13;		/* размер кадра для 1051 & 1052 */
	t->MAXADDR = 0x3ff;
	inw(t->RS);
	if (inw(t->RS) & 0x10) return (1052);
/* 1051 */
dprintf((" 1051 "));
	avtsetcmd((t->cmdport & 0xfcfb) | 0xfdfb);	/* set bit 8 */
	t->MAXADDR = 0xff;
	inw(t->RS);
	if (inw(t->RS) & 0x10) return (1051);
/* 1031 */
dprintf((" 1031 "));
	avtsetcmd((t->cmdport & 0xfcfb) | 0xfcff);	/* set bit 2 */
	t->sizekadr = 284 * 9;		/* размер кадра для 1031 & 1030 */
	t->MAXADDR = 0x7f;
	inw(t->RS);
	if (inw(t->RS) & 0x10) return (1031);
/* 1030 ? */
dprintf((" 1030 "));
	return (Av1030);
}

/*
 * test the controller PORT and IRQ
 */
int avttest (avt_unit *t) {
u_char	irq = ffs(t->irq)-1;

	if (inw (t->RS) == 0xffff) {	/* проверим наличие порта */
		dprintf ((" Arvid port not found\n"));
		return (0);
	}
/* found kontroller, find version */
	t->arvid = avtversion(t);
/* test IRQ */
	register_intr(irq, t->id, 0, (inthand2_t *)avtintr, &avt_imask, t->unit);
	t->countintr = 0;
	t->commands = VNONE;
	splhigh();
	INTREN(t->irq);
	t->flags = VTOPEN;
	DELAY (1000000);	/* должно возникнуть прерывание */
	t->flags = VTCLOSE;
	INTRDIS(t->irq);
	spl0();
	unregister_intr(irq, (inthand2_t *)avtintr);
	if (!t->countintr) {
		 dprintf ((" Arvid not interrupt\n"));
		 return(0);
	}
	return (1);
}

/*
 * Device is found, configure it.
 */
int avtattach (struct isa_device *id) {
	avt_unit *t = avttab + id->id_unit;

	if (t->arvid == UNKNOWN) {
		printf ("avt%d: type UNKNOWN, abort.\n", id->id_unit);
		return 0;
	}

	printf ("avt%d: type <Arvid %d>\n", id->id_unit, t->arvid);

/* Для всех */
	t->commands = VNONE;		/* tape is STOP */
	t->flags = VTCLOSE;		/* tape is close */

	switch (t->arvid) {
	case Av1020:
	/* 200 Kb/s */
		t->maxrate = MAXRATE20;
		t->maxphase = MAXPHASE20;
		t->phase = DEFAULT_PHASE20;
		t->sizekadr = SIZE200;
		t->sizebuf = 12996;
		t->dmasize = t->sizebuf * 2;
		t->vaddr = malloc(t->dmasize, M_DEVBUF, M_NOWAIT);
		t->paddr = t->vaddr;
		t->dmaflags = B_READ;
		t->dmapar = 1;
		t->dmasync = 0;
		break;
	case Av1031:
	case Av1030:
	/* 200 Kb/s */
		t->maxrate = MAXRATE30;
		t->maxphase = MAXPHASE30;
		t->phase = DEFAULT_PHASE30;
		break;
	case Av1051:
	case Av1052:
	/* 325 Kb/s */
		t->maxrate = MAXRATE50;
		t->maxphase = MAXPHASE50;
		t->phase = DEFAULT_PHASE50;
		break;
	} /* switch */
	t->rate = t->maxrate;		/* максимальная по умолчанию */
#ifndef FREEBSD222
	kdc_avt[id->id_unit].kdc_state = DC_IDLE;
#endif
	return (1);
}

struct isa_driver avtdriver = { avtprobe, avtattach, "avt" };

/*
 * Режим чтения данных из карты. Читаем по 100h слов. (256w/512b)
 * Режим чтения не устанавливаем, считается что он установлен.
 * Перед чтением устанавливаем номер кадра из t->RADDRES.
 * Максимальный размер информационного кадра 1000h СЛОВ
 */
void ReadArv3x(avt_unit *t, void *rawbuf, size_t len) {
u_short	addres;
size_t	tmp = 0x100;

	if (len > 0x1000) return;		/* незнаю как тут быть ??? */
	for (addres = t->RADDRES; len; addres++) {
		outw (t->RA, addres);		/* установим кадр */
		inw (t->RD);			/* так называемый 'конвеер' */
		if (len < tmp) tmp = len;	/* лишнее не читаем */
		insw (t->RDINC, rawbuf, tmp);	/* прочитаем кусочек */
		(char *)rawbuf += tmp * 2;	/* увеличим адрес в буфере */
		len -= tmp;			/* уменьшим длину чтения */
	} /* for */
}

/*
 * Режим записи данных в карту. Пишем по 100h слов. (256w/512b)
 * Режим не устанавливаем, считается что он установлен.
 * Перед записью устанавливаем номер кадра из t->RADDRES.
 * Максимальный размер информационного кадра 1000h СЛОВ
 */
void WriteArv3x(avt_unit *t, void *rawbuf, size_t len) {
u_short	addres;
size_t	tmp = 0x100;

	if (len > 0x1000)		/* не больше */
		len = 0x1000;
	for (addres = t->RADDRES; len; addres++) {
		outw (t->RA, addres);		/* установим кадр */
		if (len < tmp) tmp = len;	/* лишнее не пишем */
		outsw (t->RDINC, rawbuf, tmp);	/* запишем кусочек */
		(char *)rawbuf += tmp * 2;	/* увеличим адрес в буфере */
		len -= tmp;			/* уменьшим длину писания */
	} /* for */
}

/*
 * После чтения/записи данных надо увеличить номер буфера кадра
 * путем выдачи "1" в бит 3 RK
 */
void RWDone3x(avt_unit *t) {
#define INCBUFFMASK	0x8

	avtsetcmd((t->cmdport | INCBUFFMASK));
	if ((t->RADDRES += 0x10) > t->MAXADDR) {
		t->RADDRES = 0;
	}
	/* надо */
	avtsetaddr(t->RADDRES);
}

/*
 * Читаем номер кадра
 */
int ReadNumKadr (avt_unit *t) {
register u_short	a, b;

	do {
		a = inw(t->RS);
	/* 1110 0111 */
		if (t->arvid >= Av1051)
			a = (a & 0x7) | ((a & 0xe0) >> 2);
		else	
			a &= 0x07;
		b = inw(t->RS);
		if (t->arvid >= Av1051)
			b = (b & 0x7) | ((b & 0xe0) >> 2);
		else	
			b &= 0x07;
	} while (a != b);
/* dprintf(("Kadr:%x,", b)); */
	return (a);
}

/*
 * Interrupt routine.
 * делать как можно меньше вычислений.
 */
void avtintr(int u) {
avt_unit	*t = avttab + u;
u_int		clear;
int		numk, tmaxad, n_di, o_size;
u_short		cdmas, *pvaddr;
struct buf_raw	*bufraw;
struct buf_raw	*bufsec;

	if (u >= NAVT || t->arvid == UNKNOWN) {
		dprintf (("avtintr() -- device not configured\n"));
		return;
	}

	INTRDIS(t->irq);
	t->countintr ++;	/* увеличим счетчик прерываний */
 	if (!(t->flags & VTOPEN))
 		goto avintrexit;

	switch (t->commands) {
	/*
	 * READ - data and IRC command
	 */
	case VREAD: {
		/* небудем читать, некуда */
		if (t->rawb >= NBUFRAW) t->rawb--; /* goto avintrexit; */
		bufraw = t->rawbuf;	/* указатель на буфер */
		bufraw = bufraw->next;	/* следующий */
		switch (t->arvid) {
		case Av1020: {
/* #define	READSIZEBUF	11250 */
		if (t->dmasync < 5) {
			t->paddr = t->vaddr + (t->sizebuf - isa_dmacount(t->drq));
			t->paddr -= 294*9;
			if (t->paddr < t->vaddr)
				t->paddr += t->sizebuf;
			t->dmasync++;
		} else {
		/* di */
			n_di = t->sizebuf - isa_dmacount(t->drq);
		/* dx */
			o_size = t->paddr - t->vaddr;
			if (o_size < n_di) {
			/* адрес увеличился */
				if (o_size + 286*9 > n_di)
				/* сбой */
					numk = 0;
				else if (o_size >= 46*9)
					numk = 1;
				else if (o_size + t->sizebuf - 46*9 < n_di) {
					t->paddr += GetRDsize(t->dmapar);
					if (t->paddr > t->vaddr + t->sizebuf)
						t->paddr -= t->sizebuf;
					t->dmapar ^= 1;
					numk = 1;
				} else
					numk = 1;
/*				if ((o_size + 313*9 < n_di) && (o_size + 333*9 < n_di))
					t->paddr += 9*9;
*/			} else if (n_di + 46*9 < o_size) {
			/* адрес уменьшился */
				if (o_size + 286*9 < t->sizebuf)
				/* есть данные до конца буфера */
					numk = 1;
				else if (o_size + 286*9 - t->sizebuf < n_di)
				/* есть данные в начале буфера */
					numk = 1;
				else
				/* сбой */
					numk = 0;
			} else {
		/* 4DEE */
				t->paddr += GetRDsize(t->dmapar);
				if (t->paddr > t->vaddr + t->sizebuf)
					t->paddr -= t->sizebuf;
				t->dmapar ^= 1;
				numk = 1;
			}
/*		dprintf(("."));
*/		/* есть кадры */
			if (numk) {
				o_size = t->paddr + 285*9 - t->vaddr;
/*			dprintf(("p:%d, o:%x\n", t->dmapar, o_size));
*/				n_di = 285*9;
				isa_dmadone(B_READ|B_RAW, (caddr_t)t->vaddr, t->sizebuf * 2, t->drq);
				numk = 0;
				if (o_size > t->sizebuf) {
					numk = o_size - t->sizebuf;
					n_di -= numk;
				}
				bcopy(t->paddr, bufraw->data, n_di*2);
				if (numk)
					bcopy(t->vaddr, bufraw->data+n_di, numk*2);

				t->rawbuf = bufraw;
				t->rawb++;
			} /* numk */
		} /* t->dmasync */
			break;
		}
/*
 * 0x0008   - инкремент номера буфера
 * 0x0010   - сброс очереди
 * 0x0400   - подключение ВМ к ПС
 */
		case Av1030:
		case Av1031:
		case Av1051:
		case Av1052:
			avtsetcmd(t->cmdport);	/* установить режим */
			if (numk = ReadNumKadr (t))	 {	/* если есть кадры */
				avtsetaddr(t->RADDRES);		/* установим адрес буфера */
/* dprintf((" Addr:%x", t->RADDRES)); */
			/* читаем данные */
			 	ReadArv3x(t, bufraw->data, t->sizekadr);
			 	t->rawbuf = bufraw;	/* запомним */
				t->rawb++;		/* прочитали еще один */
			 	RWDone3x(t);	/* кадр прочитан */
			 	numk--;
			 	if ((numk) && (t->rawb < NBUFRAW)) {
			 		bufraw = bufraw->next;
					avtsetaddr(t->RADDRES);	/* установим адрес буфера */
			 		ReadArv3x(t, bufraw->data, t->sizekadr);
			 		t->rawbuf = bufraw;	/* запомним */
					t->rawb++;		/* прочитали еще один */
			 		RWDone3x(t);	/* кадр прочитан */
		 		}
			}
			break; /* 1052 */
		}	/* t->arvid */
		break;
	}	/* case VREAD: */
	/*
	 * WRITE - data and IRC command
	 */
	case VWRITE: {
	/*
	 * CODE - code kadr
	 */
		/* небудем писать, нечего */
		/* неплохо бы очистить буфер !!! */
		if (t->rawb == 0) goto avintrexit;
/* dprintf((" rawb: %d", t->rawb)); */
		/* найдем первый из записанных кадров */
		bufraw = t->rawbuf;
		/* используем переменную numk */
		for (numk=NBUFRAW - t->rawb; numk; numk--)
			bufraw = bufraw->next;
		/* bufraw - найденный кадр */
		switch (t->arvid) {
		case Av1020: {
		int	str;
		register u_short	*src, *dst;

			src = (short *)bufraw->data;
			dst = (short *)t->paddr;
			if (t->rate == 100) {
				dst += 210;
				for (str = 256; str; str--) {
					*(++dst) = 0xAAAA;
					bcopy(src, ++dst, 8*2);
					src += 8;
					dst += 8;
				}
			} else {
				dst += 10;
				for (str = 282; str; str--) {
					bcopy(src, ++dst, 9*2);
					src += 9;
					dst += 9;
				}
			}
			bufraw = bufraw->next;
			t->paddr += GetWDsize(t->dmapar);
			if (t->paddr > t->vaddr + t->sizebuf)
				t->paddr -= t->sizebuf;
			t->dmapar ^= 1;
			t->rawb--;
			break;	/* Av1020 */
		}
		case Av1030:
		case Av1031:
		case Av1051:
		case Av1052:
#if 0
			avtsetcmd(t->cmdport);	/* установить режим */
#endif
			/* количество записанных кадров в ОЗУ */
			numk = ReadNumKadr(t);
			tmaxad = t->MAXADDR >> 4;
/* dprintf((" numk: %d-%d/", numk, tmaxad)); */
			if (numk < tmaxad) {
			/* есть свободные кадры */
				if ((tmaxad - numk) > 4) {
					/* не больше 4 за раз */
					numk = 4;
				} else {
					numk = tmaxad - numk;
				};
/* dprintf(("%d,", numk)); */
				while (numk && t->rawb) {
				/* выводим кадры */
				/* пишем данные */
					WriteArv3x(t, bufraw->data, t->sizekadr);
				/* неплохо бы очистить буфер */
					bzero(bufraw->data, sizeof(bufraw->data));
					t->rawb--;		/* записали еще один */
					bufraw = bufraw->next;	/* следующий */
					RWDone3x(t);	/* кадр записан */
					numk--;
				};
			};
			break; /* Av1052 */
		} /* t->arvid */
		break;
	} /* case VWRITE: */
	case VDELAY:
	case VVCRIN:
	case VVCROUT:
		/* найдем первый из записанных кадров */
		bufraw = t->rawbuf;
		/* используем переменную numk */
		for (numk=NBUFRAW - t->rawb; numk; numk--)
			bufraw = bufraw->next;
		/* bufraw - найденный кадр */
		switch (t->arvid) {
		case Av1020: {
		int	str;
		register u_short	*src, *dst;

			src = (short *)bufraw->data;
			dst = (short *)t->paddr;
			for (str = 249; str; str--) {
				bcopy(src, ++dst, 12*2);
				src += 12;
				dst += 12;
			}
			bufraw = bufraw->next;
			t->paddr += 249*13*2;
			if (t->paddr >= t->vaddr + t->sizebuf)
				t->paddr -= t->sizebuf;
			t->dmapar ^= 1;
			t->rawb--;
/*			dprintf(("VVCROUT - intr\n"));
*/			break;
		};
		}; /* switch */
		break;
	case VNONE:
	default:
		break;
/*
 * если много пустых прерываний, их надо будет остановить
 */
	} /* t->command */
avintrexit:
	INTREN(t->irq);
	t->REQUEST = 1;		/* прерывание закончено */
}

int avtdump (dev_t dev)
{
	/* Not implemented */
	return (-1);
}

int avtsize (dev_t dev)
{
	/* Not implemented */
	return (-1);
}

/*
 * очистим ОЗУ в карте
 */
void avtclearram (avt_unit *t) {
void	*freebuf;
int	i;

/* dprintf(("avtclearram - ")); */
#if 0
	freebuf = malloc (SIZEKADR * 2, M_TEMP, M_WAITOK);
	bzero (freebuf, SIZEKADR * 2);
	for (i = 0; i < t->MAXADDR; i += 0x10) {
		WriteArv3x(t, freebuf, SIZEKADR * 2);
#else
	freebuf = malloc (8192, M_TEMP, M_WAITOK);
	bzero (freebuf, 8192);
	for (i = 0; i < t->MAXADDR; i += 0x10) {
		WriteArv3x(t, freebuf, 8192);
#endif
#if 0
		if ((t->RADDRES += 0x10) > t->MAXADDR) {
			t->RADDRES = 0;
			}
		/* надо */
		avtsetaddr(t->RADDRES);
#else
		RWDone3x(t);
#endif
		};
/* dprintf(("%x\n", i)); */
	free (freebuf, M_TEMP);
};

void clearbuf(avt_unit *t);
void clearbuf(avt_unit *t) {
int		i;
struct buf_raw	*bufraw;

	bufraw = t->rawbuf;
	for (i = 0; i < NBUFRAW; i++) {
		bzero(bufraw->data, sizeof(bufraw->data));
		bufraw = bufraw->next;	/* следующий буфер */
	}
	t->rawb = 0;			/* обнулили */
};

/*
 * Open routine, called on every device open.
 */
int avtopen (dev_t dev, int flag, int fmt, struct proc *p) {
/* минор девайса */
int		u = minor (dev) & T_UNIT;
avt_unit	*t = avttab + u;
int		error, buf, i;
struct buf_raw	*rtempbuf;
void		*freebuf;	/* буфер для очистки памяти */

	/* неправильный девайс */
	if (u >= NAVT || t->arvid == UNKNOWN)
		return (-1);

	/* Check that device is not in use */
	if (t->flags & VTOPEN)
		return (-1);

	INTRDIS(t->irq);		/* запретить прерывания */

	/* резервируем и инициализируем кольцевой буфер для raw чтения */
	rtempbuf = t->rawbuf = malloc(sizeof(struct buf_raw), M_TEMP, M_WAITOK);
	bzero (rtempbuf, sizeof(struct buf_raw));
	for (buf = 0; buf < NBUFRAW-1; buf++) {
		rtempbuf->next = malloc(sizeof(struct buf_raw), M_TEMP, M_WAITOK);
		bzero (rtempbuf->next, sizeof(struct buf_raw));
		rtempbuf = rtempbuf->next; /* текущий буфер */
	}
	rtempbuf->next = t->rawbuf;	/* кольцо :) */
	t->rawb = 0;			/* обнулили */

/* ??? #ifdef AV1030 */
	if (t->arvid >= Av1030) {
		t->cmdport = 3;	/* прием от ВМ */
		avtsetcmd(t->cmdport | 0x10);	/* сбросим очередь */
	/* очистим ОЗУ в карте? А нахера? */
	/*	avtclearram(t); */
		t->RADDRES = 0;
		avtsetaddr(t->RADDRES);
	}
/* #endif AV1030 */

	t->flags = VTOPEN;		/* открываем девайс */
	t->commands = VREAD;		/* по default чтение */

	INTREN(t->irq);			/* разрешить прерывания */
#ifndef FREEBSD222
	kdc_avt[u].kdc_state = DC_BUSY;
#endif
	return (0);
}

/*
 * Close routine, called on last device close.
 */
int avtclose (dev_t dev, int flags, int fmt, struct proc *p) {
/* аналогично */
	int		u = minor (dev) & T_UNIT;
	avt_unit	*t = avttab + u;
	int		buf;
	struct buf_raw	*rtempbuf;

	if (u >= NAVT || t->arvid == UNKNOWN)
		return (-1);

	/* подождем окончание вывода */
	if (t->commands == VWRITE)
		while (t->rawb)
			DELAY(100000);

	t->commands = VNONE;
	t->flags = VTCLOSE;
	INTRDIS(t->irq);		/* запретить прерывания */

	if (t->arvid == Av1020) {
		AvMode20(t, 0, 0x38);
	} else
/* #ifdef AV1030 */
	if (t->arvid >= Av1030) {
	}
/* #endif AV1030 */

	/* освободили зарезервированные буфера для raw чтения */
	rtempbuf = t->rawbuf;
	for (buf = 0; buf < NBUFRAW; buf++) {
		rtempbuf = rtempbuf->next;
		free(t->rawbuf, M_TEMP);
		t->rawbuf = rtempbuf; /* следующий буфер */
	}
	/* в t->rawbuf остался старый адрес первого буфера,
	   но мы ведь все равно его при открытии сменим.
	   Ну можно его еще занулить, а зачем? :) */

#ifndef FREEBSD222
	kdc_avt[u].kdc_state = DC_IDLE;
#endif
	return (0);
}

/*
 * Ioctl routine.
 */
int avtioctl (dev_t dev, int cmd, caddr_t data, int flags, struct proc *p) {
/* тудаже */
int		u = minor (dev) & T_UNIT;
avt_unit	*t = avttab + u;
struct set_rw	*ptr = NULL;
char		buffer[DEV_BSIZE];
int		rc;

	rc = EINVAL;
	if (u >= NAVT || t->arvid == UNKNOWN)
		return (ENXIO);
	switch (cmd) {
	case VGET_INFO: {
		struct	avt_info *ptr;

		bzero(buffer, sizeof(buffer));
		ptr = (struct avt_info *)buffer;
/* тип контроллера */
		ptr->arvid = t->arvid;
		if (t->arvid == Av1020) {
			ptr->drq = t->drq;
			ptr->port = 0;
		} else {
			ptr->drq = 0;
			ptr->port = t->port;
		}
		ptr->irq = t->irq;
		ptr->flags = t->flags;
	/* версия 0.1 */
		ptr->ver_major = 0;
		ptr->ver_minor = 1;
	/* максимальная скорость в Кбайт/сек. */
		ptr->maxrate = t->maxrate;
	/* максимальная фаза */
		ptr->maxphase = t->maxphase;
	/* текущая скорость */
		ptr->rate = t->rate;
	/* текущая фаза */
		ptr->phase = t->phase;
		*(struct avt_info *)data = *ptr;
		rc = 0;
		break;
	}
	case VSET_READ: {
		ptr = (struct set_rw *)data;
/*	dprintf(("avt: VSET_READ\n")); */
		INTRDIS(t->irq);	/* запретить прерывания */
		clearbuf(t);
		if (t->arvid == Av1020) {
			t->cmdport = 0x7E;	/* 0000 0000 0111 1110B */
			t->sizebuf = 625 * 9 * 2;	/* 11250 word */
			AvMode20(t, 1, t->cmdport);
			isa_dmastartq(B_READ|B_RAW, (caddr_t)t->vaddr, t->sizebuf * 2, t->drq);
			isa_dmadone(B_READ|B_RAW, (caddr_t)t->vaddr, t->sizebuf * 2, t->drq);
			t->dmapar = 1;
			t->dmasync = 0;
		} else {
			t->cmdport |= 0x0002;	/* 0000 0000 0000 0010B */
		}
		t->commands = VREAD;
		goto ioctlvset_rw;
		break;
	}
	case VSET_WRITE: {
		ptr = (struct set_rw *)data;
	dprintf(("avt: VSET_WRITE\n"));
		INTRDIS(t->irq);	/* запретить прерывания */
		clearbuf(t);
		if (t->arvid == Av1020) {
			if (ptr->mode == IOIRC) {
				t->cmdport = 0x75;	/* 0000 0000 0111 1001B */
				t->sizebuf = 249 * 13 * 4;	/* 12948 */
				AvMode20(t, 7, 0x79);
				InitWriteIrcBuf(t->vaddr);
				t->commands = VVCROUT;
				AvMode20(t, 8, 0x3D);
			} else {
				t->cmdport = 0x79;	/* 0000 0000 0111 1001B */
				t->sizebuf = 6427 * 2;	/* 12854; */
				AvMode20(t, 7, t->cmdport);
				PrepWD(t->vaddr);
				t->commands = VWRITE;
			}
			isa_dmastartq(B_WRITE|B_RAW, (caddr_t)t->vaddr, t->sizebuf * 2, t->drq);
			isa_dmadone(B_WRITE|B_RAW, (caddr_t)t->vaddr, t->sizebuf * 2, t->drq);
			t->dmapar = 1;
			t->paddr = t->vaddr;
		} else {
			t->cmdport &= 0xfffd;	/* 1111 1111 1111 1101B */
			t->commands = VWRITE;
		}
ioctlvset_rw:
		ptr = (struct set_rw *)data;
	/*
	 * устанавливаем скорость для карты
	 * проверяем возможность скорости на данной плате
	 */
		if ((ptr->rate <= t->maxrate) && (ptr->rate)) {
		/* set rate */
			t->rate = ptr->rate;
		/* set command & sizekadr */
			if (t->arvid == Av1020) {
			} else {
				if (ptr->mode == IOIRC) {
				struct	buf_raw	*tmpbuf;

					tmpbuf = t->rawbuf;
				/* 1111 1111  1111 1110 */
					t->cmdport &= 0xfffe;
					t->sizekadr = 312*12;
					t->rawb = 0;
					bzero(tmpbuf->data, t->sizekadr);
					t->RADDRES = 0;
					avtsetaddr(t->RADDRES);
					WriteArv3x(t, tmpbuf->data, t->sizekadr);
					avtsetcmd(t->cmdport);
				/* no decode */
					t->RADDRES = 0x10;
			/* установить режим и сбросить очередь */
					avtsetcmd(t->cmdport | 0x10);
					avtsetaddr(t->RADDRES);
				} else {
				/* 0000 0000  0000 0001 */
					t->cmdport |= 0x0001;
					switch (ptr->rate) {
					case 100: {
						t->sizekadr = (284+9)*9;
				/* 1111 1101  1111 1111 */
						t->cmdport |= 0x0300;
						t->cmdport &= 0xfdff;
						break;
						}
					case 200: {
						t->sizekadr = (284+9)*9;
				/* 1111 1101  1111 1111 */
						t->cmdport |= 0x0300;
						t->cmdport &= 0xfdff;
						break;
						}
					case 325: {
						t->sizekadr = (300+9)*13;
				/* 0000 0011  0000 0000 */
						t->cmdport |= 0x0300;
						break;
						}
					default: break;
					} /* switch (ptr->rate) */
					t->RADDRES = 0;
				/* установить режим и сбросить очередь */
					avtsetcmd(t->cmdport | 0x10);
					avtsetaddr(t->RADDRES);
					t->rawb = 0;
				} /* IORAW/IODATA */
			/* очистим ОЗУ */
				avtclearram(t);
			} /* av1020 */
		/* разрешим прерывания */
			rc = 0;
		} else
			rc = -1;
		INTREN(t->irq);		/* разрешить прерывания */
		break;
	}
	case VGET_PHASE: {
		*(u_char *)data = t->phase;
		rc = 0;
		break;
	}
	case VSET_PHASE: {
		u_short	tmphase;

		if ((*(u_char *)data <= t->maxphase) && t->commands == VREAD) {
			INTRDIS(t->irq);
			t->phase = *(u_char *)data;
		/* 0000 0000  1110 0000 */
			if (t->arvid == Av1020) {
				AvMode20(t, t->commands, 0x7E);
				isa_dmastartq(B_READ|B_RAW, (caddr_t)t->vaddr, t->sizebuf * 2, t->drq);
				isa_dmadone(B_READ|B_RAW, (caddr_t)t->vaddr, t->sizebuf * 2, t->drq);
				t->dmapar = 1;
				t->dmasync = 0;
			} else {
				tmphase = t->phase << 5;
				tmphase &= 0x0e0;
			/* 1111 1111  0001 1111 */
				t->cmdport &= 0xff1f;
				t->cmdport |= tmphase;
				avtsetcmd(t->cmdport);
			}
			INTREN(t->irq);
			rc = 0;
		} else
			rc = EINVAL;
		break;
	}
	default:
		break;
	}
	return (rc);
}

/*
 * Strategy routine.
 * Обработка запроса на чтение и запись.
 */
void avtstrategy (struct buf *bp) {
	int u = minor (bp->b_dev) & T_UNIT;
	avt_unit *t = avttab + u;

	if (u >= NAVT || t->arvid == UNKNOWN)
		goto errxit;

	/* at file marks and end of tape, we just return '0 bytes available' */
/*	if (t->flags & TPVOL)
		goto xit; */

	/* 0 count - not read */
	if (! bp->b_bcount) goto exit;

	t->flags &= ~VTBUSY;
	/*
	 * запретим прерывание на время работы ???
	 * надо бы поставить по флагу занятости
	 */
	if (bp->b_flags & B_READ) {
	/* Read */
		int	count;

/*	disable_intr(); */
	/* расчитаем количество имеющихся данных */
		count = t->rawb * SIZEBUFBYTE;
	/*
	 * если количество читаемых данных меньше имеющихся
	 * то читаем сколько нужно
	 */
		if (count > bp->b_bcount) count = bp->b_bcount;
	/* должны что-то делать ??????????????????????? */

		avtread(t, bp->b_un.b_addr, count);
		bp->b_resid = bp->b_bcount - count;
/*	enable_intr(); */
	} else {
	/* Write */
		int	count;

	/* расчитаем количество оставшегося места для данных */
		count = (NBUFRAW - t->rawb) * SIZEBUFBYTE;
/* dprintf((" count: %d", count)); */
	/*
	 * если количество записываемых данных меньше имеющегося места
	 * то пишем сколько надо
	 */
		if (count > bp->b_bcount) count = bp->b_bcount;
	/* должны что-то делать ??????????????????????? */

		avtwrite(t, bp->b_un.b_addr, count);
		bp->b_resid = bp->b_bcount - count;
	} /* B_WRITE */
	if (t->flags & VTBUSY) {
errxit:         bp->b_flags |= B_ERROR;
		bp->b_error = EIO;
	}
exit:	biodone (bp);
	return;
}

/*
 * start i/o operation
 * читаем байты из буфера, пока из bufraw.
 * надо зделать флаг.
 */
int avtread(avt_unit *t, void *data, unsigned len) {
int		count, search, tmp;
struct buf_raw	*bufraw;
	
	INTRDIS(t->irq);
	/* найдем первый считанный кадр */
	bufraw = t->rawbuf;
	for (search=NBUFRAW - t->rawb; search > 0; search--)
		bufraw = bufraw->next;
	INTREN(t->irq);
	tmp = SIZEBUFBYTE;
	/*
	 * bufraw теперь указывает на первый из t->rawb прочитанных буферов
	 * теперь копируем буфера в data
	 */
	for (count = 0; count < len; count += tmp) {
		/* лишнее не читаем */
		if ((len - count) < tmp) tmp = len - count;
		bcopy(bufraw->data, (char *)data + count, tmp);
		bufraw = bufraw->next;
		/* one processors */
		t->rawb--;
	}
	t->flags &= ~VTBUSY;			/* clear exception flag */
	return (1);
}

/*
 * start i/o operation
 * записываем байты в буфер, пока в bufraw.
 * надо зделать флаг.
 */
int avtwrite(avt_unit *t, void *data, unsigned len) {
int		count, tmp;
struct buf_raw	*bufraw;
	
/*	INTRDIS(t->irq); */
	/*
	 * rawbuf указывает на первый из t->rawb свободных буферов
	 * теперь копируем data в буфера
	 */
	tmp = SIZEBUFBYTE;
	for (count = 0; count < len; count += tmp) {
		/* лишнее не пишем */
		if ((len - count) < tmp) tmp = len - count;
/* а может их сначала обнулять? */
		bufraw = t->rawbuf;
		bcopy((char *)data + count, bufraw->data, tmp);
		t->rawbuf = bufraw->next;	/* следующий */
		t->rawb++;			/* еще добавили */
	}
/*	INTREN(t->irq); */
	t->flags &= ~VTBUSY;			/* clear exception flag */
	return (1);
}

#ifdef FREEBSD222
static avt_devsw_installed = 0;

static void avt_drvinit(void *unused) {

	if (!avt_devsw_installed) {
		bdevsw_add_generic(BDEV_MAJOR, CDEV_MAJOR, &avt_bdevsw);
		avt_devsw_installed = 1;
		}
}

SYSINIT(avtdev, SI_SUB_DRIVERS, SI_ORDER_MIDDLE+CDEV_MAJOR, avt_drvinit, NULL)

/*
 * По мотивам qcam
 */
#ifdef AVT_MODULE
#include "sys/lkm.h"

static struct isa_device avt_mod_dev =
	{0, &avtdriver, 0x270, IRQ11, 4, (caddr_t) 0, avtintr, 0, 0, 0, 0, 0, 0,
	 0, 1, 0, 0};
MOD_DEV(avt, LM_DT_BLOCK, BDEV_MAJOR, &avt_bdevsw);

static int avt_load(struct lkm_table *lkmtp, int cmd) {
	if (avtprobe(&avt_mod_dev)) {
		avtattach(&avt_mod_dev);

		avt_drvinit(NULL);

		uprintf("avt: driver loaded\n");
		return(0);
	} else {
		uprintf("avt: probe failed\n");
		return(1);
	}
}

static int avt_unload(struct lkm_table *lkmtp, int cmd) {
avt_unit	*av;
int		i;

	for (i = 0; i < NAVT; i++) {
		av = &avttab[i];
		if (av->flags & VTOPEN) {
			uprintf("avt%d: cannot unload, device busy", av->unit);
			return(1);
		}
	}
	uprintf("avt: driver unloaded\n");
	return(0);
}

static int avt_stat(struct lkm_table *lkmtp, int cmd) {
	return (0);
}

int avt_mod(struct lkm_table *lkmtp, int cmd) {
#define _module avt_module
	DISPATCH(lkmtp, cmd, ver,
		avt_load, avt_unload, avt_stat);
}
#endif /* AVT_MODULE */
#endif /* FREEBSD222 */
#endif /* NAVT */
