/*
 * Streamer tape driver for FreeBSD.
 * Supports Arvid 103x boards.
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

#include <i386/isa/avtreg.h>
#include <i386/isa/avt.tbl>	/* �������������� ��� */

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
	int	id;		/* ���� ��� ��������� */
	int	unit;
	u_short	port;		/* base i/o port */
	u_short	irq;		/* irq */
	u_short	drq;
	u_short	maxrate;	/* controller density */
	u_char	maxphase;	/* */
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
	u_char	time_out;
	u_short	rate;
	u_char	phase;
	u_long	countintr;	/* ������� ���������� �� ����� */

	void	*vcrtab;	/* VCR table admin videorecorder */

	u_short	sizekadr;	/* ������ ����� � ������ ��� ������/������ */
				/* ������ ��� ������� 1030/1052 � ��/�� */
	u_int	rawb;		/* ���������� ���������� ������� �� ����� */
	void	*rawbuf;	/* ��������� �� ������� ���������� ����� */
	u_int	secb;		/* ���������� ���������� �������� ������ */
	void	*secbuf;	/* ��������� �� ������� ���������� ������ */

#define	avtsetcmd(data)		outw (t->CMDPORT, data);	/*  */
#define avtsetaddr(data)	outw (t->RA, data);

	u_short DATAPORT, CMDPORT, RA, RS;
	u_short	RDINC, RD;
	u_short	RADDRES, MAXADDR;	/* ������� ����� � ����� Read/Write */
	u_char	DECODE;			/* ���� ������������� */
#define	RAWREAD		0x0		/* - IORAW/IOIRC */
#define	DATAREAD	0x1		/* - IODATA */
	u_char BUSY, NOEXCEP, RESETMASK, RESETVAL;
	u_char ONLINE, RESET;
	u_char REQUEST;		/* ���� ������� �� ���������� */
	u_char OVERFLOW;	/* ���� ������ ������ */
} avt_unit;

avt_unit avttab[NAVT];			/* tape info by unit number */

struct avtvar	avtvar;			/* ��� � ������ */
struct avt_vcr	avt_vcr;		/* VCR ������� ��� ���������� ������� */

avt_fifo	fifo;		/* fifo ��� 1010 */
avt_fifo0	fifo0;		/* fifo new */

static u_int avt_imask = HWI_MASK;	/* ����� ���������� ��� ����� */

struct TPB	TapeTPB;	/* ������� �������������� ����� */

/* static int atwait (atinfo_t *t, int catch, char *msg); */
/* static int atcmd (atinfo_t *t, int cmd); */
static int avtread (avt_unit *t, void *data, unsigned len);
static int avtwrite (avt_unit *t, void *data, unsigned len);
/* static timeout_t atimer; */
/* static void atclock (atinfo_t *t); */
/* static int atsense (atinfo_t *t, int verb, int ignor); */
/* static int avtstatus (avt_unit *t); */
/* static void atrewind (atinfo_t *t); */
/* static int atpoll (atinfo_t *t, int mask, int bits); */

extern int xdescrembler();
/*
 * ��������� ������������� ����� ����������.
 * ��������� ��������� ������ �������������
 * ���������� ������ ��� ������ ������.
 */
extern int DecoderRS(u_char *data, u_char *code, u_int start, u_int count, u_int group);

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
/* id ��� ���� ���������� */
	if(id->id_unit)
		kdc_avt[id->id_unit] = kdc_avt[0];
	kdc_avt[id->id_unit].kdc_unit = id->id_unit;
	kdc_avt[id->id_unit].kdc_parentdata = id;
	dev_attach(&kdc_avt[id->id_unit]);
}

/*
 * Probe for the presence of the device.
 */
int avtprobe (struct isa_device *id)
{
/* ��������� probe ��� ����������� ������� �� ��� ������� */
	avt_unit *probe = avttab + id->id_unit;

	avt_registerdev(id);

	probe->id   = id->id_id;
	probe->unit = id->id_unit;
	probe->port = id->id_iobase;
	probe->irq  = id->id_irq;
	probe->drq  = id->id_drq;
/* #ifdef AV1030 */
/* ������� Arvid-1030 */
/* ����������� ��������� */
/*	probe->controller = Av1030; */
	probe->RA	= AV30_RA(probe->port);
	probe->RS	= AV30_RS(probe->port);
	probe->CMDPORT	= AV30_RK(probe->port);
	probe->cmdport	= 0x3;			/* Read BM */
	probe->DATAPORT	= probe->RDINC = AV30_RDINC(probe->port);
	probe->RD	= AV30_RD(probe->port);
	probe->RADDRES	= probe->MAXADDR = 0;
	probe->DECODE	= RAWREAD;
	probe->BUSY = 1;
	probe->NOEXCEP = 1;
	probe->RESETMASK = 1;
	probe->RESETVAL = 1;
	probe->ONLINE = 1;
	probe->RESET = 1;
	probe->REQUEST = 0;
	if (avttest (probe))
		return (AV30_NPORT);
/* #endif AV1030 */
/* ���� ������������ */
	/* Tape controller not found. */
	probe->controller = UNKNOWN;
	return (0);
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
	t->MAXADDR = 0x7;
	inw(t->RS);
	if (inw(t->RS) & 0x10) return (1031);
/* 1030 ? */
dprintf((" 1030 "));
	return (1030);
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
	t->controller = avtversion(t);
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
 * ��������� 1031.
 */
int setemulate1030 (avt_unit *t)
{
	t->cmdport &= t->emulate1030 = 0xfcff;
	avtsetcmd(t->cmdport);
	inw(t->RS);
	if (inw(t->RS) & 0x0020) return (0);
	t->sizekadr = 284 * 9;		/* ������ ����� ��� 200K/s */
	t->MAXADDR = 0x7;
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
/*	if (t->controller > 1031) {
		printf("avt%d: Sorry, Arvid > 1031 not worked, ", id->id_unit);
		if (setemulate1030(t))
			printf("set emulation 1031\n");
		else	{
			printf("set emulation aborted, sorry.\n");
			return (0);
			}
		} */ /* > 1031 */
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
/* ��� ���� */
	bzero (&avt_vcr, sizeof(struct avt_vcr));
	t->commands = VNONE;		/* tape is STOP */
	t->rate = t->maxrate;		/* ������������ �� ��������� */
	t->flags = VTCLOSE;		/* tape is close */
	kdc_avt[id->id_unit].kdc_state = DC_IDLE;

	return (1);
}

struct isa_driver avtdriver = { avtprobe, avtattach, "avt", };

u_short crc16 (avt_kadr100 *kadr, int start, int c)
{
u_short	crc = 0;
register	byte, i;	/* ����������� ��� */

	for (i = start; c; i++, c--)
	{
	byte = kadr->data[i];
	crc +=crc;
	asm ("jnc	1f");
	byte ^= 0x100B;
	asm ("1:");
	crc ^= byte;
	}
	return (crc);
}

int load_tpb (avt_unit *t)
{
int		i;
u_char		data[32];	/* ��������� ����� */
HEADER_G	*header_g;	/* ��������� ��� 100�/� */
HEADER_RS	*header_rs;	/* ��������� ��� 200�/� */
#define	NGR	149

	if (t->rate == 100) {	/* 100K/s */
		}
	if (t->rate == 200) {	/* 200K/s */
		DecoderRS(data, t->rawbuf+46, 148, 1, NGR);
/*		if (crc32(data, sizeof(HEADER_RS)) == header_rs->tstsum) {
			} */
		header_rs = (HEADER_RS *)data;
		TapeTPB.format  = header_rs->format;
		for (i=0; i<8; i++)
			TapeTPB.name[i] = header_rs->name[i];
		TapeTPB.ident   = header_rs->ident;
		TapeTPB.length  = header_rs->length;
		TapeTPB.time    = header_rs->tapetime;
		TapeTPB.sector  = header_rs->sector;
		TapeTPB.Nsector = header_rs->nsect;
		}
/*	if (crc16 (kadr, 1032, 11) == kadr->TPB.TapeCRC)
	{
	 TapeTPB.format  = kadr->TPB.TapeFormat;
	 for (i=0; i<8; i++) TapeTPB.name[i] = kadr->TPB.TapeName[i];
	 TapeTPB.ident   = kadr->TPB.TapeID;
	 TapeTPB.length  = kadr->TPB.TapeTime;
	 TapeTPB.time    = kadr->TPB.Tapetime;
	 TapeTPB.sector  = SECTRNUM(	kadr->TPB.TapeSectr[0],\
	 				kadr->TPB.TapeSectr[1],\
	 				kadr->TPB.TapeSectr[2]);
	 TapeTPB.Nsector = kadr->TPB.TapeNsector;
	}
*/	 else return (0); /* ��������� ������ */
	return (1);
};

void move_data()
{
ushort	kadr[1056];
int	a, k=0;
#define	kadrk	kadr.data

fifo0.count = 0;
/* 1 */
	for (a=0; a<((512-8)/2); a++, k++) kadr[k] = avtvar.data[a];
	for (a=1008; a<1012; a++, k++) kadr[k] = avtvar.data[a];

	bcopy( kadr, fifo0.buf[fifo0.count].data, 512);
	if (fifo0.count >= 60) 
	 { return ; }
	else fifo0.count++;

/* 2 */
	for (a=0; a<((512-8)/2); a++, k++) kadr[k] = avtvar.data[a+((512-8)/2)];
	for (a=1012; a<1016; a++, k++) kadr[k] = avtvar.data[a];
/* 3 */
	for (a=0; a<((512-8)/2); a++, k++) kadr[k] = avtvar.data[a+(512-8)];
	for (a=1020; a<1024; a++, k++) kadr[k] = avtvar.data[a];
/* 4 */
	for (a=0; a<((512-8)/2); a++, k++) kadr[k] = avtvar.data[a+(((512-8)/2)*3)];
	for (a=1024; a<1028; a++, k++) kadr[k] = avtvar.data[a];
/* !!! */
	for (a=0; a<1028; a++) avtvar.data[a] = kadr[a];
}

int move_sector (u_char Nsector)
{
return (0);
}

void descrembler (u_short start, u_short blk)
{
	avtvar.start = start;
	avtvar.blk = blk;
	xdescrembler();
}

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
 * ����� ������ 1 � 3 ��� RK
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
 * ������ ����� �����
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
 * ������ ��� ����� ������ ����������.
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

	t->countintr ++;	/* �������� ������� ���������� */
 	if (!(t->flags & VTOPEN)) goto avintrexit;
	switch (t->commands)
	{
	 case VREAD:
	 {
		/* ������� ������, ������ */
		if (t->rawb >= NBUFRAW) goto avintrexit;
		bufraw = t->rawbuf;	/* ��������� �� ����� */
		bufraw = bufraw->next;	/* ��������� */
		disable_intr();
		switch (t->controller)
		{
/* #ifdef AV1030 */
	case Av1031:
/*
 * SETPHASE - ��������� ����
 * READMASK - ����� ��� ������
 * 0x0008   - ��������� ������ ������
 * 0x0010   - ����� �������
 * 0x0400   - ����������� �� � ��
 */
/*		 t->cmdport = (SETPHASE30(t->phase) | READMASK30); */
/*		 goto IntRead52; */
/*		 break; */	/* case Av1030 */
/* #endif AV1030 */
	case Av1052:
/*		t->cmdport = (SETPHASE30(t->phase) | READMASK30) & t->emulate1030; */
/* IntRead52: */
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
		}	/* t-> controller */
		enable_intr();
	if (t->DECODE) {	/* DATAREAD */
		bzero (avtvar.data, sizeof(avtvar.data));	/* ???????? */

		descrembler( 0, 88);	/* ��������� ������ */

		if (load_tpb (t)) {	/* ��������� ��������� ����� */
			disable_intr();
			move_data();
			if (fifo.count == 8) { fifo.count = 0; t->OVERFLOW = 1; };
			bcopy ( avtvar.data, &fifo.buf[fifo.count], sizeof(avtvar.data));
			fifo.count++;
			enable_intr();
			} /* if (load_tpb) */
		} /* if (t->DECODE) */
	break;
	}	/* case VREAD: */
	case VWRITE: {
		/* ������� ������, ������ */
		/* ������� �� �������� ����� !!! */
		if (t->rawb == 0) goto avintrexit;
		/* ������ ������ �� ���������� ������ */
		bufraw = t->rawbuf;
		/* ���������� ���������� numk */
		for (numk=NBUFRAW - t->rawb; numk; numk--)
			bufraw = bufraw->next;
		/* bufraw - ��������� ���� */
		disable_intr();
		switch (t->controller)
		{
/* #ifdef AV1030 */
	case Av1031:
/*
 * SETPHASE - ��������� ����
 * READMASK - ����� ��� ������
 * 0x0008   - ��������� ������ ������
 * 0x0010   - ����� �������
 * 0x0400   - ����������� �� � ��
 */
/*			t->cmdport = (SETPHASE30(t->phase) | WRITEMASK30); */
/*			goto IntRead52; */
/*			break; */	/* case Av1030 */
/* #endif AV1030 */
	case Av1052:
/*		t->cmdport = (SETPHASE30(t->phase) | WRITEMASK30) & t->emulate1030; */
/* IntWrite52: */
		avtsetcmd(t->cmdport);	/* ���������� ����� */
		if ((numk=ReadNumKadr(t)) < 7) {	/* ���� ���� ����� */
			avtsetaddr(t->RADDRES);		/* ��������� ����� ������ */
			/* ����� ������ */
			WriteArvHardw (t, bufraw->data, t->sizekadr);
			bufraw = bufraw->next;	/* ��������� */
			t->rawb--;		/* �������� ��� ���� */
			ReadWriteDone (t);	/* ���� ������� */
			numk++;
			if ((numk<7) && (t->rawb > 0)) {
				bufraw = bufraw->next;
				avtsetaddr(t->RADDRES);	/* ��������� ����� ������ */
				ReadArvHardw (t, bufraw->data, t->sizekadr);
/*				bufraw=bufraw->next; */	/* �������� */
				t->rawb--;		/* �������� ��� ���� */
				ReadWriteDone (t);	/* ���� ������� */
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
	int	u = minor (dev) & T_UNIT;
	avt_unit *t = avttab + u;
	int 	error, buf;
	struct buf_raw	*rtempbuf;
	struct buf_sec	*stempbuf;

	/* ������������ ������ */
	if (u >= NAVT || t->controller == UNKNOWN)
		return (-1);

	/* Check that device is not in use */
	if (t->flags & VTOPEN)
		return (-1);

	/* ����������� � �������������� ��������� ����� ��� raw ������ */
	rtempbuf = t->rawbuf = malloc(sizeof(struct buf_raw), M_TEMP, M_WAITOK);
	for (buf = 0; buf < NBUFRAW-1; buf++) {
		rtempbuf->next = malloc(sizeof(struct buf_raw), M_TEMP, M_WAITOK);
		rtempbuf = rtempbuf->next; /* ������� ����� */
		}
	rtempbuf->next = t->rawbuf;	/* ������ :) */
	t->rawb = 0;			/* �������� */

	/* ����������� � �������������� ��������� ����� ��� ������ */
	stempbuf = t->secbuf = malloc(sizeof(struct buf_sec), M_TEMP, M_WAITOK);
	for (buf = 0; buf < NBUFSEC-1; buf++) {
		stempbuf->next = malloc(sizeof(struct buf_sec), M_TEMP, M_WAITOK);
		stempbuf = stempbuf->next; /* ������� ����� */
		}
	stempbuf->next = t->secbuf;	/* ������ :) */
	t->secb = 0;			/* �������� */

	t->flags = VTOPEN;		/* ��������� ������ */
	t->commands = VREAD;		/* �� default ������ */
/* #ifdef AV1030 */
	avtsetcmd(t->cmdport | 0x10);	/* �������� ������� */
	t->RADDRES = 0;
/*	if (t->controller >= Av1030) */
		INTREN(t->irq);		/* ��������� ���������� */
/* #endif AV1030 */
	kdc_avt[u].kdc_state = DC_BUSY;
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
	struct buf_sec	*stempbuf;

	if (u >= NAVT || t->controller == UNKNOWN)
		return (-1);

	t->commands = VNONE;
	t->flags = VTCLOSE;
/* #ifdef AV1030 */
/*	if (t->controller >= Av1030) */
		INTRDIS(t->irq);		/* ��������� ���������� */
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

	/* ���������� ����������������� ������ � ������� */
	stempbuf = t->secbuf;
	for (buf = 0; buf < NBUFSEC; buf++) {
		stempbuf = stempbuf->next;
		free(t->secbuf, M_TEMP);
		t->secbuf = stempbuf; /* ��������� ����� */
		}
	/* � t->secbuf ������� ������ ����� ������� ������,
	   �� �� ���� ��� ����� ��� ��� �������� ������.
	   �� ����� ��� ��� ��������, � �����? :) */

	kdc_avt[u].kdc_state = DC_IDLE;
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
/* ��� ����������� */
		ptr->controller = t->controller;
		ptr->port = t->port;
		ptr->irq = t->irq;
		ptr->flags = t->flags;
		if (avt_vcr.vcrload == 1) ptr->flags |= VCRLOAD;
/* ������ 0.1 */
		ptr->ver_major = 0;
		ptr->ver_minor = 1;
/* �������������� ������ :)) */
		ptr->formats = FORMAT_K7CRC;
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
	}
	/*
	 * ������ ������. � ������� ���������� �� ����� ��/��
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
	 * ����� ������. ��/��
	 */
	case VSET_WRITE: {
		struct set_rw	*ptr = NULL;

		t->commands = VWRITE;
		t->cmdport &= 0xfffd;	/* 1111111111111101B */
ioctlvset_rw:
		ptr = (struct set_rw *)data;
	/*
	 * ������������� �������� ��� �����
	 * ��������� ����������� �������� �� ������ �����
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
				t->DECODE = 0;
				} /* IOIRC */
			else {
			/* 0000 0000  0000 0001 */
				t->cmdport |= 0x0001;
				switch (ptr->rate) {
				case 100: {
					t->sizekadr = 284*9;
			/* 1111 1101  1111 1111 */
					t->cmdport &= 0xfdff;
					break;
					}
				case 200: {
					t->sizekadr = 284*9;
			/* 1111 1101  1111 1111 */
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
		/* ���������� ����� � �������� ������� */
			avtsetcmd(t->cmdport | 0x10);
			t->RADDRES = 0;
		/* set decode? */
			if (ptr->flag == IODATA)	t->DECODE = 1;
			else				t->DECODE = 0;
			return 0;
			}
		return (-1);
	}
/*
 * �������� ������� ���������� �����������������, ������ ����������
 * ���� ���� ��� ����� ����������� ������������. ����� ����� �����.
 */
	case VSET_VCR:
	{
		bcopy ((struct avt_vcr *)data, &avt_vcr, SIZE_VCR);
		avt_vcr.vcrload = 1;
		return 0;
	}
/*
 * ������ ������� ���������� �����
 */
	case VGET_TPB:
	{
		*(struct TPB *)data = TapeTPB;
		return 0;
	}
/*
 * ������ ���� ������
 */
	case VGET_PHASE:
	{
		*(u_char *)data = t->phase;
		return 0;
	}
/*
 * ��������� ���� ������
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
 * ��������� ������� �� ������ � ������.
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
	/*
	 * bufraw ������ ��������� �� ������ �� t->rawb ����������� �������
	 * ������ �������� ������ � data
	 */
	tmp = t->sizekadr * 2;
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
	
	bufraw = t->rawbuf;
	/*
	 * bufraw ������ ��������� �� ������ �� t->rawb ��������� �������
	 * ������ �������� data � ������
	 */
	tmp = t->sizekadr * 2;
	for (count = 0; count < len; count += tmp) {
		/* ������ �� ����� */
		if ((len - count) < tmp) tmp = len - count;
/* � ����� �� ������� ��������? */
		bcopy(data + count, bufraw->data, tmp);
		bufraw = bufraw->next;	/* ��������� */
		t->rawb++;		/* ��� �������� */
		}
	t->rawbuf = bufraw;		/* ???? */
	t->flags &= ~VTBUSY;			/* clear exception flag */
	return (1);
}
#endif /* NAVT */
