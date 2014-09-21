/*
 * Streamer tape driver for FreeBSD.
 * Supports Arvid 102x boards.
 * Supports Arvid 103x boards.
 * Supports Arvid 105x boards.
 *
 * Copyright (C) 1996 by Vladimir Zarozhevsky
 *
 * E-mail:  <vladimir@c-press.udm.ru>
 * Fidonet: 2:5050/17 Work, 2:5050/2.10 Home
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

#include "avt.h"
#if NAVT > 0

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/buf.h>
#include <sys/malloc.h>
#include <sys/ioctl.h>		/* ioctl operations */
#include <sys/mtio.h>		/* mag tape io control commands */
#include <sys/devconf.h> 

#include <machine/clock.h>
#include <i386/isa/icu.h>
#include <i386/isa/isa_device.h>

/* For 1020 only */
/* #include <sys/proc.h> */
/* #include <sys/conf.h> */
/* #include <sys/file.h> */
/* #include <sys/uio.h> */
/* #include <sys/syslog.h> */
/* #include <sys/rlist.h> */
/* #include <machine/segments.h> */
#include <vm/vm.h>		/* for pmap_extract() */
#include <i386/isa/isa.h>
#include <i386/isa/ic/i8237.h>
#define	DMA37MD_BLOCK	0x80	/* block pass mode */
/* end 1020 */
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

typedef struct {
	enum avttype	controller;	/* type of controller */
	int	id;		/* надо для интерапта */
	int	unit;
	u_short	port;		/* base i/o port for 1010, 1030-1052 */
	u_short	irq;		/* irq for all */
	u_short	drq;		/* drq for 1020 */
	u_short	maxrate;	/* controller density */
	u_char	maxphase;	/* phase for read */
	u_short	flags;		/* флаг статуса */
	u_short	cmdport;	/* copy CMDPORT */
	u_short	emulate1030;	/* для временной эмуляции 1031 */
	u_char	commands;	/* command of tape drive */
#define VNONE	0		/* none command */
#define	VREAD	1		/* read data */
#define VVCRIN	4		/* VCR IN */
#define VWRITE	7		/* write data */
#define VVCROUT	8		/* VCR OUT */
#define VDELAY	9		/* delay in kadr's */
	u_char	time_out;	/* in sec - NOT WORKED */
	u_short	rate;
	u_char	phase;
	u_long	countintr;	/* счетчик прерываний от платы */

	void	*vcrtab;	/* VCR table admin videorecorder */

	u_short	sizekadr;	/* размер кадра в СЛОВАХ для чтения/записи */
				/* разный для режимов 200/325 и ВМ/ИК */
	u_int	rawb;		/* количество заполненых буферов из карты */
	void	*rawbuf;	/* указатель на текущий заполненый буфер */
/* вывод на ИК    100C - 4108  / 13 =  316   (1 буфер) */
/* чтение данных  2BF2 - 11250 /  9 = 1250   (4 буфера) */
/* запись данных  3236 - 12854 / 10 = 1285.4 (4 буфера) */
	void	*dmavaddr;	/* адрес буфера DMA для 1020 */
	u_int	dmasize;	/* текущий размер DMA буфера */
	u_int	dmaflags;	/* B_READ, B_WRITE, B_RAW */
	u_int	dmacount;	/* копия DMA->ADDR */

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

struct avt_vcr	avt_vcr;		/* VCR таблица для управления видиком */

static u_int avt_imask = HWI_MASK;	/* маска прерываний при тесте */

struct TPB	TapeTPB;	/* текущая смонтированная лента */

/* static int atwait (atinfo_t *t, int catch, char *msg); */
/* static int atcmd (atinfo_t *t, int cmd); */

/*
 * процедуры декодирования и кодирования блока информации.
 */
/* Reed-Solomon Code/Decode */
/*
extern int Decoder200 (u_char *data, u_char *code, u_int start, u_int count, u_int group);
extern int Coder200 (u_char *data, u_char *code, u_int start, u_int count, u_int group);
extern int decode200sect (u_char *data, u_char *code, u_int sect);
extern int code200sect (u_char *data, u_char *code, u_int sect);
*/

static int avtread (avt_unit *t, void *data, unsigned len);
static int avtwrite (avt_unit *t, void *data, unsigned len);
/* static timeout_t atimer; */
/* static void atclock (atinfo_t *t); */
/* static int atsense (atinfo_t *t, int verb, int ignor); */
/* static int avtstatus (avt_unit *t); */
/* static void atrewind (atinfo_t *t); */
/* static int atpoll (atinfo_t *t, int mask, int bits); */

extern int xdescrembler();

int avttest (avt_unit *probe);
int waitkadr (u_short);
void avtintr (int);

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

int checkdma (avt_unit *t);
int checkram (avt_unit *t);
int checksizedma (avt_unit *t);
/*
 * Probe for the presence of the device.
 */
int avtprobe (struct isa_device *id)
{
/* структура probe для опробывания девайса на его наличие */
	avt_unit *probe = avttab + id->id_unit;

	avt_registerdev(id);

	/* нету контроллеров */
	/* Tape controller not found. */
	probe->controller = UNKNOWN;
	probe->id   = id->id_id;
	probe->unit = id->id_unit;
	probe->port = id->id_iobase;
	probe->irq  = id->id_irq;
	probe->drq  = id->id_drq;
	/* Если DRQ в пределах 5-7, значит это 1020 */
	if ((probe->drq > 4) && (probe->drq < 8)) {
		probe->drq = id->id_drq;
	/* DMA num */
		dprintf(("DMA:%d -", probe->drq));
		if (checkdma(probe))
			dprintf((" ok"));
		else	{
			dprintf((" bad num DMA\n"));
			return (0);
			}
	/* RAM board */
		dprintf((", RAM -"));
		if (checkram(probe))
			dprintf((" ok"));
		else	{
			dprintf((" bad RAM board\n"));
			return (0);
			}
	/* DMA size */
		dprintf((", DMA size -"));
		if (checksizedma(probe))
			dprintf((" ok"));
		else	{
			dprintf((" BAD\n"));
			return (0);
			}
	/* IRQ num */
		dprintf((", IRQ -"));
		if (1)
			dprintf((" ok"));
		dprintf(("\n"));
		probe->controller = Av1020;
		return (1);
		}
	else	{
/* #ifdef AV1030 */
/* пробуем Arvid-1030 */
/* прописываем параметры */
/*	probe->controller = Av1030; */
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
		if (avttest (probe))
			return (AV30_NPORT);
/* #endif AV1030 */
		}
	return (0);
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

#if 0
extern int isa_dmarangecheck(caddr_t va, unsigned length, unsigned chan);
/* region of physical memory known to be contiguous */
vm_offset_t isaphysmem;
static caddr_t dma_bounce[8];		/* XXX */
static char bounced[8];		/* XXX */
/* #define MAXDMASZ 512	*/	/* XXX */

/* high byte of address is stored in this port for i-th dma channel */
static short dmapageport[8] =
	{ 0x87, 0x83, 0x81, 0x82, 0x8f, 0x8b, 0x89, 0x8a };

/*
 * isa_dmastart(): program 8237 DMA controller channel, avoid page alignment
 * problems by using a bounce buffer.
 *
 * add set mask
 * add for block moving
 */
void isa_dmastartblk(int flags, caddr_t addr, unsigned nbytes, unsigned chan)
{	vm_offset_t phys;
	int waport;
	caddr_t newaddr;

	if (    chan > 7
	    || (chan < 4 && nbytes > (1<<16))
	    || (chan >= 4 && (nbytes > (1<<17) || (u_int)addr & 1)))
		panic("isa_dmastart: impossible request");

	if (isa_dmarangecheck(addr, nbytes, chan)) {
		if (dma_bounce[chan] == 0)
			dma_bounce[chan] =
				/*(caddr_t)malloc(MAXDMASZ, M_TEMP, M_WAITOK);*/
				(caddr_t) isaphysmem + NBPG*chan;
		bounced[chan] = 1;
		newaddr = dma_bounce[chan];
		*(int *) newaddr = 0;	/* XXX */

		/* copy bounce buffer on write */
		if (!(flags & B_READ))
			bcopy(addr, newaddr, nbytes);
		addr = newaddr;
	}

	/* translate to physical */
	phys = pmap_extract(pmap_kernel(), (vm_offset_t)addr);

#define	DMA37MD_BLOCK	0x80
	/*
	 * Program one of DMA channels 4..7.  These are
	 * word mode channels.
	 */
	/* set mask chanel */
	outb (DMA2_SMSK, chan);
	/* set dma channel mode, and reset address ff */

	/* If B_RAW flag is set, then use autoinitialise mode */
	if (flags & B_RAW) {
	  if (flags & B_READ)
		outb(DMA2_MODE, DMA37MD_AUTO|DMA37MD_WRITE|(chan&3));
	  else
		outb(DMA2_MODE, DMA37MD_AUTO|DMA37MD_READ|(chan&3));
	}
	else
	if (flags & B_READ)
		outb(DMA2_MODE, DMA37MD_BLOCK|DMA37MD_WRITE|(chan&3));
	else
		outb(DMA2_MODE, DMA37MD_BLOCK|DMA37MD_READ|(chan&3));
	outb(DMA2_FFC, 0);

	/* send start address */
	waport = DMA2_CHN(chan - 4);
	outb(waport, phys>>1);
	outb(waport, phys>>9);
	outb(dmapageport[chan], phys>>16);

	/* send count */
	nbytes >>= 1;
	outb(waport + 2, --nbytes);
	outb(waport + 2, nbytes>>8);

	/* unmask channel */
	outb(DMA2_SMSK, chan & 3);
}

/* high byte of address is stored in this port for i-th dma channel */
short dmapageport[8];

/*
 * isa_dmastartblk(): program 8237 DMA controller channel, avoid page alignment
 * problems by using a bounce buffer.
 *
 * add set mask
 * add for block moving
 */
void isa_dmastartblk(int flags, caddr_t addr, unsigned nbytes, unsigned chan)
{	vm_offset_t phys;
	int waport;

	if ((nbytes > (1<<17)) || ((u_int)addr & 1))
		panic("avt_dmastart: impossible request");

	/* translate to physical */
	phys = pmap_extract(pmap_kernel(), (vm_offset_t)addr);

	/*
	 * Program one of DMA channels 4..7.  These are
	 * word mode channels.
	 */
	/* set mask chanel */
	outb (DMA2_SMSK, chan);
	/* set dma channel mode, and reset address ff */

	/* If B_RAW flag is set, then use autoinitialise mode */
	if (flags & B_RAW) {
	  if (flags & B_READ)
		outb(DMA2_MODE, DMA37MD_AUTO|DMA37MD_WRITE|(chan&3));
	  else
		outb(DMA2_MODE, DMA37MD_AUTO|DMA37MD_READ|(chan&3));
	}
	else
	if (flags & B_READ)
		outb(DMA2_MODE, DMA37MD_BLOCK|DMA37MD_WRITE|(chan&3));
	else
		outb(DMA2_MODE, DMA37MD_BLOCK|DMA37MD_READ|(chan&3));
	outb(DMA2_FFC, 0);

	/* send start address */
	waport = DMA2_CHN(chan - 4);
	outb(waport, phys>>1);
	outb(waport, phys>>9);
	outb(dmapageport[chan], phys>>16);

	/* send count */
	nbytes >>= 1;
	outb(waport + 2, --nbytes);
	outb(waport + 2, nbytes>>8);

	/* unmask channel */
	outb(DMA2_SMSK, chan & 3);
}
#endif

/*
 * ПП чтения регистра адреса канала DMA
 */
u_short isa_dmacount (u_short chan)
{
int	rport;
u_short	count;

	rport = DMA2_CHN(chan - 4);
	count = inb(rport+2);
	count += (inb(rport+2) << 8) + 1;
	return (count);
}
/*
 * Программа установки чтения/записи DMA канала
 * 0  - данные выведены
 * !0 - количество слов в очереди
 */
u_short RW20 (int dmaflags, void *buf, u_int count, unsigned chan)
{
u_short	c, b;

/* dprintf(("put:%d -> %d, ", count, chan)); */
/*	isa_dmastartblk(dmaflags, (caddr_t)buf, count * 2, chan); */
	isa_dmastart(dmaflags, (caddr_t)buf, count * 2, chan);
/* dprintf(("%d, ", isa_dmacount(chan))); */
	b = 0;
NextRW20:
	for (c=0; c < 5000; c++)
		if (isa_dmacount(chan) == 0)
			return(isa_dmacount(chan));
/*			return (0); */
	/* выставляем програмный запрос */
	outb (DMA2_REQ, chan);
	DELAY(100);
	outb (DMA2_REQ, chan & 3);
	b ^= 1;
/* dprintf(("%d, ", isa_dmacount(chan))); */
	if (b) goto NextRW20;
	return(isa_dmacount(chan));
}

/*
 * comm - номер команды
 * reg  - параметр
 */
void Command20 (avt_unit *t, u_int comm, u_int reg)
{
static	u_short	commdma[24];
u_short	flag;
u_int	i, temp1, temp2, temp3, strsize;
int	dflag;

	temp1 = reg & 0x8000;
	temp2 = reg & 0x7fff;
	if (temp2 != 0x79)	temp3 = temp2 & 0x6;
	else			temp3 = temp2;
	if ((temp3 == 0) || (temp3 == 0x6)) {
	/* чтение - установим фазу */
		t->cmdport = ((temp2 & 0xff1f) | (t->phase << 5));
		}
	else	{
	/* запись - какая-то другая константа */
		t->cmdport = ((temp2 & 0xff1f) | (t->phase << 5));
		}
	/* выведем команду в карту без автоинициализации DMA */
	dflag = B_WRITE;
	for (i=0; i<10; i++)	commdma[i] = 0x79;
	RW20 (dflag, (caddr_t) commdma, 10, t->drq);
	/*
	 * старший бит в слове = 1
	 * и команды 8 или 4 - выход
	 */
	if ((!temp1) && ((comm == 4) || (comm == 8))) goto Commdone;
	for (i=10; i<24; i++)	commdma[i] = t->cmdport;
	switch (comm) {
		/* чтение из карты */
		case 1:	i=2; strsize = 20; break;
		/* чтение ИК */
		case 2:	i=1; strsize = 20; break;
		case 3:	i=1; strsize = 20; break;
		/* вывод ИК */
		case 4:	i=2; strsize = 23; break;
		case 5:	i=2; strsize = 23; break;
		case 6:	i=2; strsize = 20; break;
		case 7:	i=2; strsize = 20; break;
		/* вывод ИК */
		case 8:	i=2; strsize = 23; break;
		case 0:
		default: i=2; strsize = 20; break;
		}
	/* выводим команды */
	while (i > 0) {
		RW20 (dflag, (caddr_t) commdma, strsize, t->drq);
		i--;
		}
Commdone:
	/* set autoinitialize */
	t->dmaflags |= B_RAW;
}
/*
 * Тест наличия карты на указанном DMA
 * 0 - DMA неработает
 * 1 - DMA работает
 */
int checkdma (avt_unit *t)
{
static	u_short	chkdma[10];
int	i, dflag;


	Command20 (t, 7, 0x79);
	for (i=0; i < sizeof(chkdma); i++)
		chkdma[i] = 0x79;
	dflag = B_WRITE;
	i = 2;
	while (i) {
		isa_dmastart(dflag, (caddr_t) chkdma, sizeof(chkdma), t->drq);
		/* два досовых тика !!! */
		DELAY(1000000/18*2);
/* dprintf((" blk:%d", isa_dmacount(t->drq))); */
		if (isa_dmacount(t->drq) == 0) return (1);
		else i--;
		}
#if 0
	/* сбросим светодиод (??? - не проверенно) */
	Command20 (t, 0, 0x38);
/*	for (i=0; i<10; i++)	chkdma[i] = 0x79;
	for (i=10; i<20; i++)	chkdma[i] = 0x38;
dprintf(("d:%d, ", RW20 (dflag, (caddr_t) chkdma, 20, t->drq)));
*/	return (1);
#endif
	return (0);
}

/*
 * П/П проверки ОЗУ контроллера
 */
int checkram (avt_unit *t)
{
	Command20 (t, 7, 0x79);
	return (1);
}

/*
 * П/П определения работоспособности ДМА канала
 */
int checksizedma (avt_unit *t)
{
u_short	*ststdma, *ntstdma;
int	i, dflag;

	ststdma = ntstdma = malloc(0x1000*2, M_DEVBUF, M_NOWAIT);
	for (i=0; i < 0x1000; i++)
		*ntstdma++ = 0x5aa5;
	dflag = B_READ;
	isa_dmastart(dflag, (caddr_t)ststdma, 0x1000*2, t->drq);
	DELAY(10000);
	while (isa_dmacount(t->drq) > 0) {
/* dprintf((", DMAcount:%d", isa_dmacount(t->drq))); */
		if (isa_dmacount(t->drq) == 1000)
			return (0);
		}
	ntstdma = ststdma;
	for (i=0; i < 0x1000; i++)
		if (*ntstdma++ == 0x5aa5) return (0);
	/* free memory ??? */
	free(ststdma, M_DEVBUF);
	return (1);
}

/*
 * Test for 1052, 1051, 1031
 * input:
 *	struct avt_unit
 * output:
 *	1031, 1051, 1052, 1030
 */
int avtversion (avt_unit *t)
{
/* 1052 */
dprintf((" 1052 "));
	avtsetcmd((t->cmdport & 0xfcfb) | 0xfefb);	/* set bit 9 */
	t->sizekadr = 296 * 13;		/* размер кадра для 1051 & 1052 */
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
int avttest (avt_unit *t)
{
u_char	irq = ffs(t->irq)-1;

	if (inw (t->RS) == 0xffff)	/* проверим наличие порта */
		{
		 dprintf ((" Arvid port not found\n"));
		 return (0);
		}
/* found kontroller, find version */
	t->controller = avtversion(t);
/* test IRQ */
	register_intr(irq, t->id, 0, (inthand2_t *)avtintr, &avt_imask, t->unit);
	t->countintr = 0;
	t->commands = VNONE;
	splhigh();
	INTREN(t->irq);
	t->flags = VTOPEN;
	DELAY (1000000);	/* должно возникнуть прерывание */
	t->flags = VTCLOSE;
	spl0();
	INTRDIS(t->irq);
	unregister_intr(irq, (inthand2_t *)avtintr);
	if (!t->countintr) {
		 dprintf ((" Arvid not interrupt\n"));
		 return(0);
		 }
	return (1);
}

/*
 * эмулируем 1031.
 */
int setemulate1030 (avt_unit *t)
{
	t->cmdport &= t->emulate1030 = 0xfcff;
	avtsetcmd(t->cmdport);
	inw(t->RS);
	if (inw(t->RS) & 0x0020) return (0);
	t->sizekadr = 284 * 9;		/* размер кадра для 200K/s */
	t->MAXADDR = 0x7f;
	return (1);
}

/*
 * Device is found, configure it.
 */
int avtattach (struct isa_device *id)
{
	avt_unit *t = avttab + id->id_unit;

	if (t->controller == UNKNOWN) {
		printf ("avt%d: type UNKNOWN, abort.\n", id->id_unit);
		return 0; }

	printf ("avt%d: type <Arvid %d>\n", id->id_unit, t->controller);

/* Для всех */
	bzero (&avt_vcr, sizeof(struct avt_vcr));
	t->commands = VNONE;		/* tape is STOP */
	t->rate = t->maxrate;		/* максимальная по умолчанию */
	t->flags = VTCLOSE;		/* tape is close */

	if (t->controller == Av1020) {
	/* 200 Kb/s */
		t->maxrate = MAXRATE20;
		t->maxphase = MAXPHASE20;
		t->phase = DEFAULT_PHASE20;
		t->dmavaddr = malloc(22500, M_DEVBUF, M_NOWAIT);
		t->dmaflags = B_READ;
		}
	if (t->controller == 1031) {
	/* 200 Kb/s */
		t->maxrate = MAXRATE30;
		t->maxphase = MAXPHASE30;
		t->phase = DEFAULT_PHASE30;
		}
	if (t->controller == 1052) {
	/* 325 Kb/s */
		t->maxrate = MAXRATE50;
		t->maxphase = MAXPHASE50;
		t->phase = DEFAULT_PHASE50;
		}
	kdc_avt[id->id_unit].kdc_state = DC_IDLE;

	return (1);
}

struct isa_driver avtdriver = { avtprobe, avtattach, "avt", };

/*
 * Режим чтения данных из карты. Читаем по 100h слов. (256w/512b)
 * Режим чтения не устанавливаем, считается что он установлен.
 * Перед чтением устанавливаем номер кадра из t->RADDRES.
 * Максимальный размер информационного кадра 1000h СЛОВ
 */
void ReadArvHardw (avt_unit *t, void *rawbuf, size_t len)
{
u_short	addres;
size_t	tmp = 0x100;

	if (len > 0x1000) return;		/* незнаю как тут быть ??? */
	for (addres = t->RADDRES; len; addres++) {
		outw (t->RA, addres);		/* установим кадр */
		inw (t->RD);			/* так называемый 'конвеер' */
		if (len < tmp) tmp = len;	/* лишнее не читаем */
		insw (t->RDINC, rawbuf, tmp);	/* прочитаем кусочек */
		rawbuf += tmp * 2;		/* увеличим адрес в буфере */
		len -= tmp;			/* уменьшим длину чтения */
	} /* for */
}

/*
 * Режим записи данных в карту. Пишем по 100h слов. (256w/512b)
 * Режим не устанавливаем, считается что он установлен.
 * Перед записью устанавливаем номер кадра из t->RADDRES.
 * Максимальный размер информационного кадра 1000h СЛОВ
 */
void WriteArvHardw (avt_unit *t, void *rawbuf, size_t len)
{
u_short	addres;
size_t	tmp = 0x100;

	if (len > 0x1000) return;		/* незнаю как тут быть ??? */
	for (addres = t->RADDRES; len; addres++) {
		outw (t->RA, addres);		/* установим кадр */
/*		inw (t->RD); */			/* так называемый 'конвеер' */
		if (len < tmp) tmp = len;	/* лишнее не пишем */
		outsw (t->RDINC, rawbuf, tmp);	/* запишем кусочек */
		rawbuf += tmp * 2;		/* увеличим адрес в буфере */
		len -= tmp;			/* уменьшим длину писания */
	} /* for */
}

/*
 * После чтения/записи данных надо увеличить номер буфера кадра
 * путем выдачи "1" в бит 3 RK
 */
void ReadWriteDone (avt_unit *t)
{
#define INCBUFFMASK	0x8
	avtsetcmd((t->cmdport | INCBUFFMASK));
	if ((t->RADDRES += 0x10) > t->MAXADDR) {
		t->RADDRES = 0;
		avtsetaddr(t->RADDRES);
		}
}

/*
 * Читаем номер кадра
 */
int ReadNumKadr (avt_unit *t)
{
register u_short	a, b;
NextNumKadr0:
	a=inw(t->RS);
/* 1110 0111 */
	if (t->controller > 1031)	a = (a & 0x7) | ((a & 0xe0) >> 2);
	else				a &= 0x07;
	b=inw(t->RS);
/* dprintf(("Kadr:%x,", b)); */
	if (t->controller > 1031)	b = (b & 0x7) | ((b & 0xe0) >> 2);
	else				b &= 0x07;
	if (a != b) goto NextNumKadr0;
	return (a);
}

/*
 * Interrupt routine.
 * делать как можно меньше вычислений.
 */
void avtintr (int u)
{
	avt_unit	*t = avttab + u;
	u_int		clear;
	int		numk;
	struct buf_raw	*bufraw;
	struct buf_raw	*bufsec;

	if (u >= NAVT || t->controller == UNKNOWN) {
		dprintf (("avtintr() -- device not configured\n"));
		return;
	}

	t->countintr ++;	/* увеличим счетчик прерываний */
 	if (!(t->flags & VTOPEN)) goto avintrexit;
	switch (t->commands)
	{
	/*
	 * READ - data and IRC command
	 */
	case VREAD:
	 {
		/* небудем читать, некуда */
		if (t->rawb >= NBUFRAW) goto avintrexit;
		bufraw = t->rawbuf;	/* указатель на буфер */
		bufraw = bufraw->next;	/* следующий */
		disable_intr();
		switch (t->controller)
		{
	case Av1020:
dprintf(("Av:%d, ", isa_dmacount(t->drq)));
		Command20(t, 1, 0x7e);
		break;	/* 1020 */
/*
 * 0x0008   - инкремент номера буфера
 * 0x0010   - сброс очереди
 * 0x0400   - подключение ВМ к ПС
 */
	case Av1031:
	case Av1052:
		avtsetcmd(t->cmdport);	/* установить режим */
		if (numk = ReadNumKadr (t))	 {	/* если есть кадры */
			avtsetaddr(t->RADDRES);		/* установим адрес буфера */
/* dprintf((" Addr:%x", t->RADDRES)); */
		/* читаем данные */
		 	ReadArvHardw (t, bufraw->data, t->sizekadr);
		 	t->rawbuf = bufraw;	/* запомним */
			t->rawb++;		/* прочитали еще один */
		 	ReadWriteDone (t);	/* кадр прочитан */
		 	numk--;
		 	if ((numk) && (t->rawb < NBUFRAW)) {
		 		bufraw = bufraw->next;
				avtsetaddr(t->RADDRES);	/* установим адрес буфера */
		 		ReadArvHardw (t, bufraw->data, t->sizekadr);
		 		t->rawbuf = bufraw;	/* запомним */
				t->rawb++;		/* прочитали еще один */
		 		ReadWriteDone (t);	/* кадр прочитан */
		 		}
			}
		break; /* 1052 */
		}	/* t-> controller */
		enable_intr();
	/*
	 * DECODE - decode kadr
	 */
#if 0
	if (t->DECODE) {
		bzero (avtvar.data, sizeof(avtvar.data));	/* ???????? */

		descrembler( 0, 88);	/* дешифруем данные */

		if (load_tpb (t)) {	/* загружаем параметры ленты */
			disable_intr();
			move_data();
			if (fifo.count == 8) { fifo.count = 0; t->OVERFLOW = 1; };
			bcopy ( avtvar.data, &fifo.buf[fifo.count], sizeof(avtvar.data));
			fifo.count++;
			enable_intr();
			} /* if (load_tpb) */
		}; /* if (t->DECODE) */
#endif /* 0 */
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
		/* найдем первый из записанных кадров */
		bufraw = t->rawbuf;
		/* используем переменную numk */
		for (numk=NBUFRAW - t->rawb; numk; numk--)
			bufraw = bufraw->next;
		/* bufraw - найденный кадр */
		disable_intr();
		switch (t->controller)
		{
	case Av1031:
	case Av1052:
		avtsetcmd(t->cmdport);	/* установить режим */
		if ((numk=ReadNumKadr(t)) < (t->MAXADDR>>4)) {	/* если есть кадры */
			avtsetaddr(t->RADDRES);		/* установим адрес буфера */
			/* пишем данные */
			WriteArvHardw (t, bufraw->data, t->sizekadr);
			t->rawb--;		/* записали еще один */
			ReadWriteDone (t);	/* кадр записан */
			numk++;
			if ((numk < (t->MAXADDR>>4)) && (t->rawb)) {
				bufraw = bufraw->next;	/* следующий */
				avtsetaddr(t->RADDRES);	/* установим адрес буфера */
				WriteArvHardw (t, bufraw->data, t->sizekadr);
				t->rawb--;		/* записали еще один */
				ReadWriteDone (t);	/* кадр записан */
				}
			} /* numk */
		break; /* 1052 */
		} /* t-> controller */
		enable_intr();
		break; } /* case VWRITE: */
	 case VDELAY:
	 case VVCRIN:
	 case VVCROUT:
	 	break;
	 case VNONE:
	 default:
	 	break;
/*
 * если много пустых прерываний, их надо будет остановить
 */
	} /* t->command */
avintrexit:
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
 * Open routine, called on every device open.
 */
int avtopen (dev_t dev, int flag, int fmt, struct proc *p)
{
	/* минор девайса */
	int	u = minor (dev) & T_UNIT;
	avt_unit *t = avttab + u;
	int 	error, buf, i;
	struct buf_raw	*rtempbuf;

	/* неправильный девайс */
	if (u >= NAVT || t->controller == UNKNOWN)
		return (-1);

	/* Check that device is not in use */
	if (t->flags & VTOPEN)
		return (-1);

	/* резервируем и инициализируем кольцевой буфер для raw чтения */
	rtempbuf = t->rawbuf = malloc(sizeof(struct buf_raw), M_TEMP, M_WAITOK);
	for (buf = 0; buf < NBUFRAW-1; buf++) {
		rtempbuf->next = malloc(sizeof(struct buf_raw), M_TEMP, M_WAITOK);
		rtempbuf = rtempbuf->next; /* текущий буфер */
		}
	rtempbuf->next = t->rawbuf;	/* кольцо :) */
	t->rawb = 0;			/* обнулили */

	t->flags = VTOPEN;		/* открываем девайс */
	t->commands = VREAD;		/* по default чтение */
	if (t->controller == Av1020) {
		Command20(t, 1, 0x0e);
dprintf(("Start DMA read - "));
		RW20(t->dmaflags, t->dmavaddr, 22500/2, t->drq);
dprintf(("OK!\n"));
		}
/* #ifdef AV1030 */
	if (t->controller >= Av1030) {
		avtsetcmd(t->cmdport | 0x10);	/* сбросить очередь */
	/* глюкодром */
		for (i = 0; i < t->MAXADDR; i += 10)
			avtsetcmd(t->cmdport | 0x08);
		t->RADDRES = 0;
		}
/* #endif AV1030 */
	INTREN(t->irq);			/* разрешить прерывания */
	kdc_avt[u].kdc_state = DC_BUSY;
	return (0);
}

/*
 * Close routine, called on last device close.
 */
int avtclose (dev_t dev, int flags, int fmt, struct proc *p)
{
/* аналогично */
	int u = minor (dev) & T_UNIT;
	avt_unit *t = avttab + u;
	int		buf;
	struct buf_raw	*rtempbuf;

	if (u >= NAVT || t->controller == UNKNOWN)
		return (-1);

	t->commands = VNONE;
	t->flags = VTCLOSE;
	INTRDIS(t->irq);		/* запретить прерывания */

	if (t->controller == Av1020) {
		Command20(t, 0, 0x38);
		}
/* #ifdef AV1030 */
	if (t->controller >= Av1030) {
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

	kdc_avt[u].kdc_state = DC_IDLE;
	return (0);
}

/*
 * Ioctl routine.
 */
int avtioctl (dev_t dev, int cmd, caddr_t data, int flags, struct proc *p)
{
/* тудаже */
	int u = minor (dev) & T_UNIT;
	avt_unit *t = avttab + u;

	char	buffer[DEV_BSIZE];

/*	int error=0; */

	if (u >= NAVT || t->controller == UNKNOWN)
		return (ENXIO);
	switch (cmd)
	{
	case VGET_INFO:	/* get driver info */
	{
	struct	avt_info *ptr;
		bzero(buffer, sizeof(buffer));
		ptr = (struct avt_info *)buffer;
/* тип контроллера */
		ptr->controller = t->controller;
		ptr->port = t->port;
		ptr->irq = t->irq;
		ptr->flags = t->flags;
		if (avt_vcr.vcrload == 1) ptr->flags |= VCRLOAD;
/* версия 0.1 */
		ptr->ver_major = 0;
		ptr->ver_minor = 1;
/* поддерживаемый формат :)) */
		ptr->formats = FORMAT_K7CRC;
/* максимальная скорость в Кбайт/сек. */
		ptr->maxrate = t->maxrate;
/* максимальная фаза */
		ptr->maxphase = t->maxphase;
/* время тайм-аут в сек.*/
		ptr->time_out = 13;
/* текущая скорость */
		ptr->rate = t->rate;
/* текущая фаза */
		ptr->phase = t->phase;
/* проверка связи */
		ptr->check_rec = 1;
/* текущий режим авто-фазирования */
		ptr->auto_phase = 0;
		*(struct avt_info *)data = *ptr;
		return 0;
	}
	/*
	 * читаем данные. в будущем определяем по флагу ВМ/ИК
	 */
	case VSET_READ: {
		struct set_rw	*ptr = NULL;

/*		ptr = (struct set_rw *)data; */
		t->commands = VREAD;
		t->cmdport |= 0x0002;	/* 0000000000000010B */
		goto ioctlvset_rw;
		break;
	}
	/*
	 * пишем данные. ВМ/ИК
	 */
	case VSET_WRITE: {
		struct set_rw	*ptr = NULL;

		t->commands = VWRITE;
		t->cmdport &= 0xfffd;	/* 1111111111111101B */
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
			if (ptr->flag == IOIRC) {
			/* 1111 1111  1111 1110 */
				t->cmdport &= 0xfffe;
				t->sizekadr = 312*12;
			/* no decode */
				} /* IOIRC */
			else {
			/* 0000 0000  0000 0001 */
				t->cmdport |= 0x0001;
				switch (ptr->rate) {
				case 100: {
					t->sizekadr = 284*9;
			/* 1111 1101  1111 1111 */
					t->cmdport |= 0x0300;
					t->cmdport &= 0xfdff;
					break;
					}
				case 200: {
					t->sizekadr = 284*9;
			/* 1111 1101  1111 1111 */
					t->cmdport |= 0x0300;
					t->cmdport &= 0xfdff;
					break;
					}
				case 325: {
					t->sizekadr = 296*13;
			/* 0000 0011  0000 0000 */
					t->cmdport |= 0x0300;
					break;
					}
				default: break;
					} /* switch (ptr->rate) */
				} /* IORAW/IODATA */
		/* установить режим и сбросить очередь */
			avtsetcmd(t->cmdport | 0x10);
			t->RADDRES = 0;
			t->rawb = 0;
			return 0;
			}
		return (-1);
	}
/*
 * загрузка таблицы управления видеомагнитофоном, должна выполнятся
 * хоть один раз перед управлением магнитофоном. Иначе будет отлуп.
 */
	case VSET_VCR:
	{
		bcopy ((struct avt_vcr *)data, &avt_vcr, SIZE_VCR);
		avt_vcr.vcrload = 1;
		return 0;
	}
/*
 * чтение таблицы параметров ленты
 */
	case VGET_TPB:
	{
		*(struct TPB *)data = TapeTPB;
		return 0;
	}
/*
 * чтение фазы чтения
 */
	case VGET_PHASE:
	{
		*(u_char *)data = t->phase;
		return 0;
	}
/*
 * установка фазы чтения
 */
	case VSET_PHASE:
	{
	u_short	tmphase;
		if (*(u_char *)data <= t->maxphase) {
			t->phase = *(u_char *)data;
	/* 0000 0000  1110 0000 */
			tmphase = (t->phase << 5) & 0x0e0;
			if (t->phase > 0x07) {
	/* 0000 0000  0000 0100 */
				tmphase |= 0x04;
	/* 0001 1000  0000 0000 */
				tmphase |= (((t->phase - 0x08) << 8) & 0x1800);
				}
	/* 1110 0111  0001 1011 */
			t->cmdport = (t->cmdport & 0xe71b) | tmphase;
/* dprintf(("phase = %x, cmd = %x\n", tmphase, t->cmdport)); */
/* Sorry, not worked */
/*			t->cmdport = t->cmdport & t->emulate1030; */
			return 0;
			}
		else	return (EINVAL);
	}

	default:
		break;
	}
/*	return (error); */
	return (EINVAL);
}

/*
 * Strategy routine.
 * Обработка запроса на чтение и запись.
 */
void avtstrategy (struct buf *bp)
{
	int u = minor (bp->b_dev) & T_UNIT;
	avt_unit *t = avttab + u;

	if (u >= NAVT || t->controller == UNKNOWN)
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
	disable_intr();
	if (bp->b_flags & B_READ) {
	/* читаем данные */
		int	count;
	/* расчитаем количество имеющихся данных */
		count = t->rawb * t->sizekadr * 2;
	/*
	 * если количество читаемых данных меньше имеющихся
	 * то читаем сколько нужно
	 */
		if (count > bp->b_bcount) count = bp->b_bcount;
	/* должны что-то делать ??????????????????????? */

		avtread (t, bp->b_un.b_addr, count);
		bp->b_resid = bp->b_bcount - count;
		} /* B_READ */
	else {
	/* пишем данные */
		int	count;
	/* расчитаем количество оставшегося места для данных */
		count = (NBUFRAW - t->rawb) * t->sizekadr * 2;
	/*
	 * если количество записываемых данных меньше имеющегося места
	 * то пишем сколько надо
	 */
		if (count > bp->b_bcount) count = bp->b_bcount;
	/* должны что-то делать ??????????????????????? */

		avtwrite (t, bp->b_un.b_addr, count);
		bp->b_resid = bp->b_bcount - count;
		} /* B_WRITE */
	enable_intr();
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
static int avtread (avt_unit *t, void *data, unsigned len)
{
int		count;
int		search;
int		tmp;
struct buf_raw	*bufraw;
	
	/* найдем первый считанный кадр */
	bufraw = t->rawbuf;
	for (search=NBUFRAW - t->rawb; search > 0; search--)
		bufraw = bufraw->next;
	tmp = t->sizekadr * 2;
	/*
	 * bufraw теперь указывает на первый из t->rawb прочитанных буферов
	 * теперь копируем буфера в data
	 */
	for (count = 0; count < len; count += tmp) {
		/* лишнее не читаем */
		if ((len - count) < tmp) tmp = len - count;
		bcopy(bufraw->data, data + count, tmp);
		bufraw = bufraw->next;
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
static int avtwrite (avt_unit *t, void *data, unsigned len)
{
int		count;
int		tmp;
struct buf_raw	*bufraw;
	
	bufraw = t->rawbuf;
	/*
	 * bufraw теперь указывает на первый из t->rawb свободных буферов
	 * теперь копируем data в буфера
	 */
	tmp = t->sizekadr * 2;
	for (count = 0; count < len; count += tmp) {
		/* лишнее не пишем */
		if ((len - count) < tmp) tmp = len - count;
/* а может их сначала обнулять? */
		bcopy(data + count, bufraw->data, tmp);
		bufraw = bufraw->next;	/* следующий */
		t->rawb++;		/* еще добавили */
		}
	t->rawbuf = bufraw;		/* запомнили */
	t->flags &= ~VTBUSY;			/* clear exception flag */
	return (1);
}
#endif /* NAVT */
