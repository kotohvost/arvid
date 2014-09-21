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
#define	FREEBSD222	1

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
#ifdef FREEBSD222
/* #include */
#else
#include <sys/devconf.h> 
#endif
#include <machine/clock.h>
#include <i386/isa/icu.h>
#include <i386/isa/isa_device.h>

/* For 1020 only */
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
	enum controller	arvid;	/* type of controller */
	int	id;		/* ���� ��� ��������� */
	int	unit;
	u_short	port;		/* base i/o port for 1010, 1030-1052 */
	u_short	irq;		/* irq for all */
	u_short	drq;		/* drq for 1020 */
	u_short	maxrate;	/* controller density */
	u_char	maxphase;	/* phase for read */
	u_short	flags;		/* ���� ������� */
	u_short	cmdport;	/* copy CMDPORT */
	u_short	emulate1030;	/* ��� ��������� �������� 1031 */
	u_char	commands;	/* command of tape drive */
#define VNONE	0		/* none command */
#define	VREAD	1		/* read data */
#define VVCRIN	4		/* VCR IN */
#define VWRITE	7		/* write data */
#define VVCROUT	8		/* VCR OUT */
#define VDELAY	9		/* delay in kadr's */
	u_char	time_out;	/* in sec - NOT WORKED */
	u_short	rate;
	u_char	phase;		/* current phase */
	u_long	countintr;	/* ������� ���������� �� ����� */

	u_short	sizekadr;	/* ������ ����� � ������ ��� ������/������ */
				/* ������ ��� ������� 200/325 � ��/�� */
	u_int	rawb;		/* ���������� ���������� ������� �� ����� */
	void	*rawbuf;	/* ��������� �� ������� ���������� ����� */
/* ����� �� ��    100C - 4108  / 13 =  316   (1 �����) */
/* ������ ������  2BF2 - 11250 /  9 = 1250   (4 ������) */
/* ������ ������  3236 - 12854 / 10 = 1285.4 (4 ������) */
	void	*dmavaddr;	/* ����� ������ DMA ��� 1020 */
	u_int	dmasize;	/* ������� ������ DMA ������ */
	u_int	dmaflags;	/* B_READ, B_WRITE, B_RAW */
	u_int	dmacount;	/* ����� DMA->ADDR */

#define	avtsetcmd(data)		outw (t->CMDPORT, data);	/*  */
#define avtsetaddr(data)	outw (t->RA, data);
/*
 * CMDPORT	- ������� �������
 * RA		- ������� ������
 * RS		- ������� �������
 * RD		- ������� ������
 * RDINC	- ������� ������ � ����������� ����������� ������
 */
	u_short CMDPORT, RA, RS;
	u_short	RDINC, RD;
	u_short	RADDRES;	/* ������� ����� � ����� Read/Write */
	u_short	MAXADDR;	/* ������������ ����� � ����� */
	u_char BUSY, NOEXCEP;
	u_char REQUEST;		/* ���� ������� �� ���������� */
	u_char OVERFLOW;	/* ���� ������ ������ */
} avt_unit;

avt_unit avttab[NAVT];			/* tape info by unit number */

static u_int avt_imask = HWI_MASK;	/* ����� ���������� ��� ����� */

/*
static int avtread(avt_unit *, void *, unsigned);
static int avtwrite(avt_unit *, void *, unsigned);
static int avtopen(dev_t, int, int, struct proc *);
static int avtclose(dev_t, int, int, struct proc *);
static int avtioctl(dev_t, int, caddr_t, int, struct proc *);
static int avtdump(dev_t);
static int avtsize(dev_t);
static int avtstrategy();
*/

int avttest (avt_unit *probe);
int waitkadr (u_short);
void avtintr (int);

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
/* id ��� ���� ���������� */
	if(id->id_unit)
		kdc_avt[id->id_unit] = kdc_avt[0];
	kdc_avt[id->id_unit].kdc_unit = id->id_unit;
	kdc_avt[id->id_unit].kdc_parentdata = id;
	dev_attach(&kdc_avt[id->id_unit]);
}
#endif	/* FREEBSD222 */

int checkdma (avt_unit *t);
int checkram (avt_unit *t);
int checksizedma (avt_unit *t);
/*
 * Probe for the presence of the device.
 */
int avtprobe (struct isa_device *id)
{
/* ��������� probe ��� ����������� ������� �� ��� ������� */
	avt_unit *probe = avttab + id->id_unit;

#ifndef FREEBSD222
	avt_registerdev(id);
#endif
	/* ���� ������������ */
	/* Tape controller not found. */
	probe->arvid = UNKNOWN;
	probe->id   = id->id_id;
	probe->unit = id->id_unit;
	probe->port = id->id_iobase;
	probe->irq  = id->id_irq;
	probe->drq  = id->id_drq;
	/* ���� DRQ � �������� 5-7, ������ ��� 1020 */
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
		probe->arvid = Av1020;
		return (1);
		}
	else	{
/* #ifdef AV1030 */
/* ������� Arvid-1030 */
/* ����������� ��������� */
/*	probe->arvid = Av1030; */
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

/*
 * �� ������ �������� ������ ������ DMA
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
 * ��������� ��������� ������/������ DMA ������
 * 0  - ������ ��������
 * !0 - ���������� ���� � �������
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
	/* ���������� ���������� ������ */
	outb (DMA2_REQ, chan);
	DELAY(100);
	outb (DMA2_REQ, chan & 3);
	b ^= 1;
/* dprintf(("%d, ", isa_dmacount(chan))); */
	if (b) goto NextRW20;
	return(isa_dmacount(chan));
}

/*
 * comm - ����� �������
 * reg  - ��������
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
	/* ������ - ��������� ���� */
		t->cmdport = ((temp2 & 0xff1f) | (t->phase << 5));
		}
	else	{
	/* ������ - �����-�� ������ ��������� */
		t->cmdport = ((temp2 & 0xff1f) | (t->phase << 5));
		}
	/* ������� ������� � ����� ��� ����������������� DMA */
	dflag = B_WRITE;
	for (i=0; i<10; i++)	commdma[i] = 0x79;
	RW20 (dflag, (caddr_t) commdma, 10, t->drq);
	/*
	 * ������� ��� � ����� = 1
	 * � ������� 8 ��� 4 - �����
	 */
	if ((!temp1) && ((comm == 4) || (comm == 8))) goto Commdone;
	for (i=10; i<24; i++)	commdma[i] = t->cmdport;
	switch (comm) {
		/* ������ �� ����� */
		case 1:	i=2; strsize = 20; break;
		/* ������ �� */
		case 2:	i=1; strsize = 20; break;
		case 3:	i=1; strsize = 20; break;
		/* ����� �� */
		case 4:	i=2; strsize = 23; break;
		case 5:	i=2; strsize = 23; break;
		case 6:	i=2; strsize = 20; break;
		case 7:	i=2; strsize = 20; break;
		/* ����� �� */
		case 8:	i=2; strsize = 23; break;
		case 0:
		default: i=2; strsize = 20; break;
		}
	/* ������� ������� */
	while (i > 0) {
		RW20 (dflag, (caddr_t) commdma, strsize, t->drq);
		i--;
		}
Commdone:
	/* set autoinitialize */
	t->dmaflags |= B_RAW;
}
/*
 * ���� ������� ����� �� ��������� DMA
 * 0 - DMA ����������
 * 1 - DMA ��������
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
		/* ��� ������� ���� !!! */
		DELAY(1000000/18*2);
/* dprintf((" blk:%d", isa_dmacount(t->drq))); */
		if (isa_dmacount(t->drq) == 0) return (1);
		else i--;
		}
	return (0);
}

/*
 * �/� �������� ��� �����������
 */
int checkram (avt_unit *t)
{
int	bp2;	/* bp-2 */
int	bp4;	/* bp-4 */
int	bp8;	/* bp-8 */
int	bpA;	/* bp-A */
int	i;
int	si;	/* si */
short	tstbuf1[0x14];
short	tstbuf2[0x14];
int	dflag;

	bp2 = 0;
ChTst1:
	bp8 = bp2 & 0x0f;
	bpA = bp2 & 0x1f;
	si = 1 << bp8;
	if (bpA >= 0x10) si = !si;
	bp4 = 1;
	Command20 (t, 7, 0x79);
	for (i=0; i<0x10; i++) {
		tstbuf1[i] = si;
	/* rol si (???) */
		si <<= 1;
		}
	/* ��������� ������� */
	tstbuf1[0] = t->cmdport;

printf("\nWrite:\t"); for (i=0; i<0x10; i++) printf(", %x", tstbuf1[i]);

	dflag = B_WRITE;
	isa_dmastart(dflag, (caddr_t)tstbuf1, sizeof(tstbuf1), t->drq);
	while (isa_dmacount(t->drq) > 0) {
dprintf((", DMAcount:%d", isa_dmacount(t->drq)));
		}
	dflag = B_READ;
	isa_dmastart(dflag, (caddr_t)tstbuf2, sizeof(tstbuf2), t->drq);
	while (isa_dmacount(t->drq) > 0) {
dprintf((", DMAcount:%d", isa_dmacount(t->drq)));
		}

printf("\nRead:\t"); for (i=0; i<0x10; i++) printf(", %x", tstbuf2[i]);

	return (1);
}

/*
 * �/� ����������� ����������������� ��� ������
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
	t->sizekadr = 296 * 13;		/* ������ ����� ��� 1051 & 1052 */
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
	t->sizekadr = 284 * 9;		/* ������ ����� ��� 1031 & 1030 */
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

	if (inw (t->RS) == 0xffff)	/* �������� ������� ����� */
		{
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
	DELAY (1000000);	/* ������ ���������� ���������� */
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
 * Device is found, configure it.
 */
int avtattach (struct isa_device *id)
{
	avt_unit *t = avttab + id->id_unit;

	if (t->arvid == UNKNOWN) {
		printf ("avt%d: type UNKNOWN, abort.\n", id->id_unit);
		return 0; }

	printf ("avt%d: type <Arvid %d>\n", id->id_unit, t->arvid);

/* ��� ���� */
	t->commands = VNONE;		/* tape is STOP */
	t->rate = t->maxrate;		/* ������������ �� ��������� */
	t->flags = VTCLOSE;		/* tape is close */

	switch (t->arvid) {
	case Av1020:
	/* 200 Kb/s */
		t->maxrate = MAXRATE20;
		t->maxphase = MAXPHASE20;
		t->phase = DEFAULT_PHASE20;
		t->dmavaddr = malloc(22500, M_DEVBUF, M_NOWAIT);
		t->dmaflags = B_READ;
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
		}; /* switch */
#ifndef FREEBSD222
	kdc_avt[id->id_unit].kdc_state = DC_IDLE;
#endif
	return (1);
}

struct isa_driver avtdriver = { avtprobe, avtattach, "avt", };

/*
 * ����� ������ ������ �� �����. ������ �� 100h ����. (256w/512b)
 * ����� ������ �� �������������, ��������� ��� �� ����������.
 * ����� ������� ������������� ����� ����� �� t->RADDRES.
 * ������������ ������ ��������������� ����� 1000h ����
 */
void ReadArvHardw (avt_unit *t, void *rawbuf, size_t len)
{
u_short	addres;
size_t	tmp = 0x100;

	if (len > 0x1000) return;		/* ������ ��� ��� ���� ??? */
	for (addres = t->RADDRES; len; addres++) {
		outw (t->RA, addres);		/* ��������� ���� */
		inw (t->RD);			/* ��� ���������� '�������' */
		if (len < tmp) tmp = len;	/* ������ �� ������ */
		insw (t->RDINC, rawbuf, tmp);	/* ��������� ������� */
		rawbuf += tmp * 2;		/* �������� ����� � ������ */
		len -= tmp;			/* �������� ����� ������ */
	} /* for */
}

/*
 * ����� ������ ������ � �����. ����� �� 100h ����. (256w/512b)
 * ����� �� �������������, ��������� ��� �� ����������.
 * ����� ������� ������������� ����� ����� �� t->RADDRES.
 * ������������ ������ ��������������� ����� 1000h ����
 */
void WriteArvHardw (avt_unit *t, void *rawbuf, size_t len)
{
u_short	addres;
size_t	tmp = 0x100;

	if (len > 0x1000) return;		/* ������ ��� ��� ���� ??? */
	for (addres = t->RADDRES; len; addres++) {
		outw (t->RA, addres);		/* ��������� ���� */
/*		inw (t->RD); */			/* ��� ���������� '�������' */
		if (len < tmp) tmp = len;	/* ������ �� ����� */
		outsw (t->RDINC, rawbuf, tmp);	/* ������� ������� */
		rawbuf += tmp * 2;		/* �������� ����� � ������ */
		len -= tmp;			/* �������� ����� ������� */
	} /* for */
}

/*
 * ����� ������/������ ������ ���� ��������� ����� ������ �����
 * ����� ������ "1" � ��� 3 RK
 */
void ReadWriteDone (avt_unit *t)
{
#define INCBUFFMASK	0x8
	avtsetcmd((t->cmdport | INCBUFFMASK));
	if ((t->RADDRES += 0x10) > t->MAXADDR) {
		t->RADDRES = 0;
		}
	/* ���� */
	avtsetaddr(t->RADDRES);
}

/*
 * ������ ����� �����
 */
int ReadNumKadr (avt_unit *t)
{
register u_short	a, b;
NextNumKadr0:
	a=inw(t->RS);
/* 1110 0111 */
	if (t->arvid > 1031)	a = (a & 0x7) | ((a & 0xe0) >> 2);
	else				a &= 0x07;
	b=inw(t->RS);
	if (t->arvid > 1031)	b = (b & 0x7) | ((b & 0xe0) >> 2);
	else				b &= 0x07;
	if (a != b) goto NextNumKadr0;
/* dprintf(("Kadr:%x,", b)); */
	return (a);
}

/*
 * Interrupt routine.
 * ������ ��� ����� ������ ����������.
 */
void avtintr (int u)
{
	avt_unit	*t = avttab + u;
	u_int		clear;
	int		numk, tmaxad;
	struct buf_raw	*bufraw;
	struct buf_raw	*bufsec;

	if (u >= NAVT || t->arvid == UNKNOWN) {
		dprintf (("avtintr() -- device not configured\n"));
		return;
	}

	t->countintr ++;	/* �������� ������� ���������� */
 	if (!(t->flags & VTOPEN)) goto avintrexit;
	switch (t->commands)
	{
	/*
	 * READ - data and IRC command
	 */
	case VREAD:
	 {
		/* ������� ������, ������ */
		if (t->rawb >= NBUFRAW) goto avintrexit;
		bufraw = t->rawbuf;	/* ��������� �� ����� */
		bufraw = bufraw->next;	/* ��������� */
		disable_intr();
		switch (t->arvid)
		{
	case Av1020:
dprintf(("Av:%d, ", isa_dmacount(t->drq)));
		Command20(t, 1, 0x7e);
		break;	/* 1020 */
/*
 * 0x0008   - ��������� ������ ������
 * 0x0010   - ����� �������
 * 0x0400   - ����������� �� � ��
 */
	case Av1030:
	case Av1031:
	case Av1051:
	case Av1052:
		avtsetcmd(t->cmdport);	/* ���������� ����� */
		if (numk = ReadNumKadr (t))	 {	/* ���� ���� ����� */
			avtsetaddr(t->RADDRES);		/* ��������� ����� ������ */
/* dprintf((" Addr:%x", t->RADDRES)); */
		/* ������ ������ */
		 	ReadArvHardw (t, bufraw->data, t->sizekadr);
		 	t->rawbuf = bufraw;	/* �������� */
			t->rawb++;		/* ��������� ��� ���� */
		 	ReadWriteDone (t);	/* ���� �������� */
		 	numk--;
		 	if ((numk) && (t->rawb < NBUFRAW)) {
		 		bufraw = bufraw->next;
				avtsetaddr(t->RADDRES);	/* ��������� ����� ������ */
		 		ReadArvHardw (t, bufraw->data, t->sizekadr);
		 		t->rawbuf = bufraw;	/* �������� */
				t->rawb++;		/* ��������� ��� ���� */
		 		ReadWriteDone (t);	/* ���� �������� */
		 		}
			}
		break; /* 1052 */
		}	/* t->arvid */
		enable_intr();
	break;
	}	/* case VREAD: */
	/*
	 * WRITE - data and IRC command
	 */
	case VWRITE: {
	/*
	 * CODE - code kadr
	 */
		/* ������� ������, ������ */
		/* ������� �� �������� ����� !!! */
		if (t->rawb == 0) goto avintrexit;
/* dprintf((" rawb: %d", t->rawb)); */
		/* ������ ������ �� ���������� ������ */
		bufraw = t->rawbuf;
		/* ���������� ���������� numk */
		for (numk=NBUFRAW - t->rawb; numk; numk--)
			bufraw = bufraw->next;
		/* bufraw - ��������� ���� */
		disable_intr();
		switch (t->arvid)
		{
	case Av1030:
	case Av1031:
	case Av1051:
	case Av1052:
#if 0
		avtsetcmd(t->cmdport);	/* ���������� ����� */
#endif
#if 0
		if ((numk=ReadNumKadr(t)) < (t->MAXADDR>>4)) {	/* ���� ���� ����� */
			avtsetaddr(t->RADDRES);		/* ��������� ����� ������ */
			/* ����� ������ */
			WriteArvHardw (t, bufraw->data, t->sizekadr);
			t->rawb--;		/* �������� ��� ���� */
			bufraw = bufraw->next;	/* ��������� */
			ReadWriteDone (t);	/* ���� ������� */
			numk++;
			if ((numk < (t->MAXADDR>>4)) && (t->rawb)) {
				avtsetaddr(t->RADDRES);	/* ��������� ����� ������ */
				WriteArvHardw (t, bufraw->data, t->sizekadr);
				t->rawb--;		/* �������� ��� ���� */
				ReadWriteDone (t);	/* ���� ������� */
				}
			} /* numk */
#else
		/* ���������� ���������� ������ � ��� */
		numk = ReadNumKadr(t);
		tmaxad = t->MAXADDR >> 4;
/* dprintf((" numk: %d-%d/", numk, tmaxad)); */
		if (numk < tmaxad) {
		/* ���� ��������� ����� */
			if ((tmaxad - numk) > 4) {
				/* �� ������ 4 �� ��� */
				numk = 4;
				}
			else	{
				numk = tmaxad - numk;
				};
/* dprintf(("%d,", numk)); */
			while (numk && t->rawb) {
				/* ������� ����� */
				/* ����� ������ */
				WriteArvHardw (t, bufraw->data, t->sizekadr);
				t->rawb--;		/* �������� ��� ���� */
				bufraw = bufraw->next;	/* ��������� */
				ReadWriteDone (t);	/* ���� ������� */
				numk--;
				};
			};
#endif
		break; /* 1052 */
		} /* t->arvid */
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
 * ���� ����� ������ ����������, �� ���� ����� ����������
 */
	} /* t->command */
avintrexit:
	t->REQUEST = 1;		/* ���������� ��������� */
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
/* ����� ������� */
int		u = minor (dev) & T_UNIT;
avt_unit	*t = avttab + u;
int		error, buf, i;
struct buf_raw	*rtempbuf;
void		*freebuf;	/* ����� ��� ������� ������ */

	/* ������������ ������ */
	if (u >= NAVT || t->arvid == UNKNOWN)
		return (-1);

	/* Check that device is not in use */
	if (t->flags & VTOPEN)
		return (-1);

	/* ����������� � �������������� ��������� ����� ��� raw ������ */
	rtempbuf = t->rawbuf = malloc(sizeof(struct buf_raw), M_TEMP, M_WAITOK);
	bzero (rtempbuf, sizeof(struct buf_raw));
	for (buf = 0; buf < NBUFRAW-1; buf++) {
		rtempbuf->next = malloc(sizeof(struct buf_raw), M_TEMP, M_WAITOK);
	/* ������� ��� */
		bzero (rtempbuf->next, sizeof(struct buf_raw));
		rtempbuf = rtempbuf->next; /* ������� ����� */
		}
	rtempbuf->next = t->rawbuf;	/* ������ :) */
	t->rawb = 0;			/* �������� */

	t->cmdport = 3;	/* ����� �� �� */

	t->flags = VTOPEN;		/* ��������� ������ */
	t->commands = VREAD;		/* �� default ������ */
	if (t->arvid == Av1020) {
		Command20(t, 1, 0x0e);
dprintf(("Start DMA read - "));
		RW20(t->dmaflags, t->dmavaddr, 22500/2, t->drq);
dprintf(("OK!\n"));
		}
/* #ifdef AV1030 */
	if (t->arvid >= Av1030) {
		avtsetcmd(t->cmdport | 0x10);	/* ������� ������� */
		avtsetaddr(t->RADDRES);
#if 1
	/* ������� ��� � ����� */
		freebuf = malloc (0x1000*2, M_TEMP, M_WAITOK);
		bzero (freebuf, 0x1000*2);
		for (i = 0; i < t->MAXADDR; i += 10) {
			WriteArvHardw (t, freebuf, 0x1000);
			ReadWriteDone (t);
			};
		free (freebuf, M_TEMP);
#endif
		avtsetcmd(t->cmdport | 0x10);	/* ������� ������� */
		t->RADDRES = 0;
		avtsetaddr(t->RADDRES);
		}
/* #endif AV1030 */
	INTREN(t->irq);			/* ��������� ���������� */
#ifndef FREEBSD222
	kdc_avt[u].kdc_state = DC_BUSY;
#endif
	return (0);
}

/*
 * Close routine, called on last device close.
 */
int avtclose (dev_t dev, int flags, int fmt, struct proc *p)
{
/* ���������� */
	int u = minor (dev) & T_UNIT;
	avt_unit *t = avttab + u;
	int		buf;
	struct buf_raw	*rtempbuf;

	if (u >= NAVT || t->arvid == UNKNOWN)
		return (-1);

	/* �������� ��������� ������ */
	if (t->commands == VWRITE)
		while (t->rawb)
			DELAY(100000);

	t->commands = VNONE;
	t->flags = VTCLOSE;
	INTRDIS(t->irq);		/* ��������� ���������� */

	if (t->arvid == Av1020) {
		Command20(t, 0, 0x38);
		}
/* #ifdef AV1030 */
	if (t->arvid >= Av1030) {
		}
/* #endif AV1030 */
	/* ���������� ����������������� ������ ��� raw ������ */
	rtempbuf = t->rawbuf;
	for (buf = 0; buf < NBUFRAW; buf++) {
		rtempbuf = rtempbuf->next;
		free(t->rawbuf, M_TEMP);
		t->rawbuf = rtempbuf; /* ��������� ����� */
		}
	/* � t->rawbuf ������� ������ ����� ������� ������,
	   �� �� ���� ��� ����� ��� ��� �������� ������.
	   �� ����� ��� ��� ��������, � �����? :) */
#ifndef FREEBSD222
	kdc_avt[u].kdc_state = DC_IDLE;
#endif
	return (0);
}

/*
 * Ioctl routine.
 */
int avtioctl (dev_t dev, int cmd, caddr_t data, int flags, struct proc *p)
{
/* ������ */
	int u = minor (dev) & T_UNIT;
	avt_unit *t = avttab + u;
	struct set_rw	*ptr = NULL;

	char	buffer[DEV_BSIZE];

/*	int error=0; */

	if (u >= NAVT || t->arvid == UNKNOWN)
		return (ENXIO);
	switch (cmd)
	{
	case VGET_INFO: {
	struct	avt_info *ptr;
		bzero(buffer, sizeof(buffer));
		ptr = (struct avt_info *)buffer;
/* ��� ����������� */
		ptr->arvid = t->arvid;
		ptr->port = t->port;
		ptr->irq = t->irq;
		ptr->flags = t->flags;
/* ������ 0.1 */
		ptr->ver_major = 0;
		ptr->ver_minor = 1;
/* ������������ �������� � �����/���. */
		ptr->maxrate = t->maxrate;
/* ������������ ���� */
		ptr->maxphase = t->maxphase;
/* ����� ����-��� � ���.*/
		ptr->time_out = 13;
/* ������� �������� */
		ptr->rate = t->rate;
/* ������� ���� */
		ptr->phase = t->phase;
/* �������� ����� */
		ptr->check_rec = 1;
/* ������� ����� ����-����������� */
		ptr->auto_phase = 0;
		*(struct avt_info *)data = *ptr;
		return 0;
		};
	/*
	 * ������ ������. � ������� ���������� �� ����� ��/��
	 */
	case VSET_READ: {
		t->commands = VREAD;
		t->cmdport |= 0x0002;	/* 0000 0000 0000 0010B */
		goto ioctlvset_rw;
		}; /* VSET_READ */
	/*
	 * ����� ������. ��/��
	 */
	case VSET_WRITE: {
		t->commands = VWRITE;
		t->cmdport &= 0xfffd;	/* 1111 1111 1111 1101B */
ioctlvset_rw:
		ptr = (struct set_rw *)data;
	/*
	 * ������������� �������� ��� �����
	 * ��������� ����������� �������� �� ������ �����
	 */
		if ((ptr->rate <= t->maxrate) && (ptr->rate)) {
		/* �������� ���������� */
			disable_intr();
		/* set rate */
			t->rate = ptr->rate;
		/* set command & sizekadr */
			if (ptr->flag == IOIRC) {
			struct	buf_raw	*tmpbuf;

				tmpbuf = t->rawbuf;
			/* 1111 1111  1111 1110 */
				t->cmdport &= 0xfffe;
				t->sizekadr = 312*12;
				t->rawb = 0;
				bzero(tmpbuf->data, t->sizekadr);
				t->RADDRES = 0;
				avtsetaddr(t->RADDRES);
				WriteArvHardw(t, tmpbuf->data, t->sizekadr);
				avtsetcmd(t->cmdport);
			/* no decode */
				t->RADDRES = 0x10;
		/* ���������� ����� � �������� ������� */
				avtsetcmd(t->cmdport | 0x10);
				avtsetaddr(t->RADDRES);
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
				t->RADDRES = 0;
		/* ���������� ����� � �������� ������� */
			avtsetcmd(t->cmdport | 0x10);
			avtsetaddr(t->RADDRES);
			t->rawb = 0;
				} /* IORAW/IODATA */
		/* �������� ���������� */
			enable_intr();
			return 0;
			}
		return (-1);
		};
/*
 * ������ ���� ������
 */
	case VGET_PHASE: {
		*(u_char *)data = t->phase;
		return 0;
		}; /* VGET_PHASE */
/*
 * ��������� ���� ������
 */
	case VSET_PHASE: {
	u_short	tmphase;
		if (*(u_char *)data <= t->maxphase) {
			disable_intr();
			t->phase = *(u_char *)data;
		/* 0000 0000  1110 0000 */
			tmphase = t->phase << 5;
			tmphase &= 0x0e0;
		/* 1111 1111  0001 1111 */
			t->cmdport &= 0xff1f;
			t->cmdport |= tmphase;
			avtsetcmd(t->cmdport);
			enable_intr();
			return 0;
			}
		else	return (EINVAL);
		}; /* VSET_PHASE */
	default:
		break;
	}
	return (EINVAL);
}

/*
 * Strategy routine.
 * ��������� ������� �� ������ � ������.
 */
void avtstrategy (struct buf *bp)
{
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
	 * �������� ���������� �� ����� ������ ???
	 * ���� �� ��������� �� ����� ���������
	 */
	disable_intr();
	if (bp->b_flags & B_READ) {
	/* ������ ������ */
		int	count;
	/* ��������� ���������� ��������� ������ */
		count = t->rawb * t->sizekadr * 2;
	/*
	 * ���� ���������� �������� ������ ������ ���������
	 * �� ������ ������� �����
	 */
		if (count > bp->b_bcount) count = bp->b_bcount;
	/* ������ ���-�� ������ ??????????????????????? */

		avtread (t, bp->b_un.b_addr, count);
		bp->b_resid = bp->b_bcount - count;
		} /* B_READ */
	else {
	/* ����� ������ */
		int	count;
	/* ��������� ���������� ����������� ����� ��� ������ */
		count = (NBUFRAW - t->rawb) * t->sizekadr * 2;
/* dprintf((" count: %d", count)); */
	/*
	 * ���� ���������� ������������ ������ ������ ���������� �����
	 * �� ����� ������� ����
	 */
		if (count > bp->b_bcount) count = bp->b_bcount;
	/* ������ ���-�� ������ ??????????????????????? */

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
 * ������ ����� �� ������, ���� �� bufraw.
 * ���� ������� ����.
 */
static int avtread (avt_unit *t, void *data, unsigned len)
{
int		count;
int		search;
int		tmp;
struct buf_raw	*bufraw;
	
	/* ������ ������ ��������� ���� */
	bufraw = t->rawbuf;
	for (search=NBUFRAW - t->rawb; search > 0; search--)
		bufraw = bufraw->next;
	tmp = t->sizekadr * 2;
	/*
	 * bufraw ������ ��������� �� ������ �� t->rawb ����������� �������
	 * ������ �������� ������ � data
	 */
	for (count = 0; count < len; count += tmp) {
		/* ������ �� ������ */
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
 * ���������� ����� � �����, ���� � bufraw.
 * ���� ������� ����.
 */
static int avtwrite (avt_unit *t, void *data, unsigned len)
{
int		count;
int		tmp;
struct buf_raw	*bufraw;
	
	/*
	 * rawbuf ��������� �� ������ �� t->rawb ��������� �������
	 * ������ �������� data � ������
	 */
	tmp = t->sizekadr * 2;
	for (count = 0; count < len; count += tmp) {
		/* ������ �� ����� */
		if ((len - count) < tmp) tmp = len - count;
/* � ����� �� ������� ��������? */
		bufraw = t->rawbuf;
		bcopy(data + count, bufraw->data, tmp);
		t->rawbuf = bufraw->next;	/* ��������� */
		t->rawb++;			/* ��� �������� */
		};
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

#endif /* FREEBSD222 */

#endif /* NAVT */
