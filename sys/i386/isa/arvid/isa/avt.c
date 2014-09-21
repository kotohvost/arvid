/*
 * Streamer tape driver for FreeBSD.
 * Supports Arvid 1010 boards.
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
/* #include <sys/fcntl.h> */
#include <sys/malloc.h>
#include <sys/ioctl.h>	/* ioctl operations */
#include <sys/mtio.h>	/* mag tape io control commands */
/* #include <sys/proc.h> */
#include <sys/devconf.h> 
/* #include <vm/vm_param.h> */

#include <machine/clock.h>
#include <i386/isa/icu.h>
#include <i386/isa/isa_device.h>

#include <i386/isa/avtreg.h>
#include <i386/isa/avt.tbl>	/* корректирующий код */

/* #ifdef AV1020 */
#include <i386/isa/isa.h>
#define	DMA2_CHN(c)	(IO_DMA2 + 2*(2*(c)))	/* Addr reg for channel c */
#define DMA2_REQ	(IO_DMA2 + 2*9)		/* Soft request DMA start */
#define DMA2_SMSK	(IO_DMA2 + 2*10)	/* single mask register */
/* #endif AV1020 */

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
	enum avttype controller;	/* type of controller */
	int	id;		/* надо для интерапта */
	int	unit;
	u_short	port;		/* base i/o port */
	u_short	irq;		/* irq */
	u_short	drq;		/* DMA ch */
	u_short	maxrate;	/* controller density */
#define FORMAT10	100		/* 100Kb/s */
#define FORMAT20	200		/* 200Kb/s */
	u_char	maxphase;	/* */
	u_short	flags;		/* флаг статуса */
#define	VTCLOSE	0x0000	/* дайвер свободен */
#define VTOPEN	0x0001	/* драйвер используется */
#define	VTBUSY	0x8000	/* драйвер что-то делает */
	u_short	cmdport;	/* copy CMDPORT */
#define READMASK10	0x2	/* 0000000000000010B */
#define WRITEMASK10	NOT READMASK10 /* 0x-3 1111111111111101B */
	u_char	commands;	/* command of tape drive */
#define VNONE	0		/* none command */
#define	VREAD	1		/* read */
#define VWRITE	2		/* write */
#define VDELAY	3		/* delay in kadr's */
#define VVCROUT	4		/* VCR command */
	u_char	time_out;
	u_short	rate;
	u_char	phase;
	u_long	countintr;	/* счетчик прерываний от платы */

	void	*buf;		/* internal i/o buffer */
	void	*dmavaddr;	/* virtual address of dma i/o buffer  */
	unsigned dmatotal;	/* size of i/o buffer */
	unsigned dmaflags;	/* B_READ, B_WRITE, B_RAW */
	unsigned dmacount;	/* resulting length of dma i/o */

#define avtresetintr(tINTRPORT) outw (tINTRPORT, 0);	/* reset interrupt */
#define	avtsetcmd(tCMDPORT, data)	outw (tCMDPORT, data);	/*  */

	u_short DATAPORT, CMDPORT, STATPORT, INTRPORT, ADDRPORT;
	u_char BUSY, NOEXCEP, RESETMASK, RESETVAL;
	u_char ONLINE, RESET;
	u_char REQUEST;		/* флаг запроса от прерывания */
	u_char OVERFLOW;	/* флаг потери данных */
} avt_unit;

avt_unit avttab[NAVT];				/* tape info by unit number */

struct avtvar	avtvar;			/* все о арвиде */

avt_fifo	fifo;		/* fifo для 1010 */
avt_fifo0	fifo0;		/* fifo new */

static u_int avt_imask = HWI_MASK;	/* маска прерываний при тесте */
struct TPB	TapeTPB;	/* текущая смонтированная лента */

/* static int atwait (atinfo_t *t, int catch, char *msg); */
/* static int atcmd (atinfo_t *t, int cmd); */
static int avtread (avt_unit *t, void *data, unsigned len);
/* static timeout_t atimer; */
/* static void atclock (atinfo_t *t); */
/* static int atsense (atinfo_t *t, int verb, int ignor); */
/* static int avtstatus (avt_unit *t); */
/* static void atrewind (atinfo_t *t); */
/* static int atwritefm (atinfo_t *t); */
/* static int atpoll (atinfo_t *t, int mask, int bits); */

extern int xdescrembler();

int avttest (avt_unit *probe);
int hardready (u_short);
/* void avtresetintr (avt_unit *t); */
void avtintr (int);

/* #ifdef AV1020 */
int checkdma (avt_unit *t);
int testdma (avt_unit *t);
/* #endif AV1020 */

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
/* если нету то нулевой ? */
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
/* структура probe для опробывания девайса на его наличие */
	avt_unit *probe = avttab + id->id_unit;

	avt_registerdev(id);

	probe->id   = id->id_id;
	probe->unit = id->id_unit;
	probe->port = id->id_iobase;
	probe->irq  = id->id_irq;
/* #ifdef AV1010 */
/* пробуем Arvid-1010 */
/* прописываем параметры */
	probe->controller = Av1010;
	probe->INTRPORT = AV10_INTRPORT (probe->port);
	probe->ADDRPORT = AV10_ADDRPORT (probe->port);
	probe->STATPORT = AV10_STATPORT (probe->port);
	probe->CMDPORT = AV10_CMDPORT (probe->port);
	probe->DATAPORT = AV10_DATAPORT (probe->port);
	probe->BUSY = 1;
	probe->NOEXCEP = 1;
	probe->RESETMASK = 1;
	probe->RESETVAL = 1;
	probe->ONLINE = 1;
	probe->RESET = 1;
	probe->REQUEST = 0;
	if (avttest (probe))
#define AVT_NPORT	4*2	/* количество используемых портов */
		return (AVT_NPORT);
/* #endif AV1010 */
/* #ifdef AV1020 */
/* пробуем Arvid-1020 */
	probe->controller = Av1020;
	probe->drq  = id->id_drq;
	probe->dmaflags = B_READ;
	if (avttest (probe))
#define PROBE_SUCCES	1
		return PROBE_SUCCES;
/* #endif AV1020 */
/* нету контроллеров */
	/* Tape controller not found. */
	probe->controller = UNKNOWN;
	return (0);
}

/* test the controller PORT and IRQ */
int avttest (avt_unit *t)
{
	u_long s;
	u_char	irq = ffs(t->irq)-1;
/* #ifdef AV1010 */
	if (t->controller == Av1010)
	{
	if (inw (t->STATPORT) == 0xffff)	/* проверим наличие порта */
		{
		 dprintf ((" Arvid 1010 (port) not found\n"));
		 return (0);
		}

	if (irq < 11 || irq > 12)		/* используемое прерывание */
		{
		printf (" avt%d: Bad irq=%d, should be 11 or 12\n", t->unit, ffs(irq)-1);
		return (0);
		}

	register_intr(irq, t->id, 0,
			 (inthand2_t *)avtintr, &avt_imask, t->unit);
	s = t->countintr;
	splhigh();
	INTREN(t->irq);
	t->flags = VTOPEN;
	avtresetintr(t->INTRPORT);
	DELAY (1000000);	/* должно возникнуть прерывание */
	t->flags = VTCLOSE;
	INTRDIS(t->irq);
	spl0();
	unregister_intr(irq, (inthand2_t *)avtintr);
	if (s == t->countintr)
		{
		 dprintf ((" Arvid 1010 not interrupt\n"));
		 return(0);
		 }
	return (t->port);
	}
/* #endif AV1010 */
/* #ifdef AV1020 */
	if (t->controller == Av1020)
	{
	if (t->drq < 5 || t->drq > 7 ) {
		printf ("avt%d: Bad drq=%d, should be 5..7\n", t->unit, t->drq);
		return 0;
	}
	dprintf (("check DMA "));
	if (checkdma (t)) dprintf ((" - found!\n"));
	else { dprintf ((" - not found :(\n")); return 0; }
	dprintf (("check size DMA "));
	if (testdma (t)) dprintf ((" - Ok!\n"));
	else { dprintf ((" - bad :(\n")); return 0; }
	return (1);
	}
/* #endif AV1020 */
	return 0;
}

/* #ifdef AV1020 */
int isa_dmaCount (u_short chan)
{
int	rport;
u_short	count;

	rport = DMA2_CHN(chan - 4);
	count = inb(rport+2);
	count = count + (inb(rport+2) << 8) + 1;
/* dprintf((" %x", count)); */
	return (count);
}

void isa_dmareqon (u_short chan)
{
	outb (DMA2_REQ, chan);
}
void isa_dmareqoff (u_short chan)
{
	outb (DMA2_REQ, (chan & 3));
}

int ReadWriteArv (avt_unit *t, void *buf, u_int count)
{
u_int	c;
/* Внимательно следить за флагом dmaflags */
	isa_dmastartblk (t->dmaflags, (caddr_t)buf, count, t->drq);
/* вставить проверку на тайм-аут по записи/чтению */
	for (c=0; c < 0x1388; c++) if (isa_dmaCount(t->drq) == 0) return (0);
	return (c);
} 

/*
 * comm - номер команды
 * reg  - параметр для команды
 */
void CommandArv (avt_unit *t, u_int comm, u_int reg)
{
u_short	flag;
u_short	commdma[24];
u_int	i, temp1, temp2, temp3;

	temp1 = reg & 0x8000;
	temp2 = reg & 0x7fff;
	if (temp2 != 0x79) temp3 = temp2 & 0x6;
	if (temp2 == 0x79) temp3 = temp2;
	if ((temp3 == 0) || (temp3 == 0x6))
/* чтение - установим фазу */
	 { t->cmdport = ((t->cmdport & 0xe0) | (t->phase << 5)); }
	else
/* запись - здесь другая константа ???????????????? */
	 { t->cmdport = ((t->cmdport & 0xe0) | (t->phase << 5)); }
/* запись без автоинициализации */
	t->dmaflags = B_WRITE;
	for (i=0; i<10; i++) commdma[i] = 0x79;
	ReadWriteArv (t, (caddr_t)commdma, 10);
	if ((!temp1) && ((comm == 4) || (comm == 8))) goto commdone;
	for (i=10; i<24; i++) commdma[i] = t->cmdport;
	switch (comm)
	{
/* Чтение данных из платы */
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 0:
	dafault:
	 break;
	}
/* общий выход */
commdone:
	t->dmaflags |= B_RAW;
}

int checkdma (avt_unit *t)
{
int	i;
u_short	chkdma[20];

	for (i=0; i<20; i++) chkdma[i] = 0x79;
	t->dmaflags = B_WRITE;
	isa_dmastartblk (t->dmaflags, (caddr_t)chkdma, 20, t->drq);
	isa_dmareqon (t->drq);
	DELAY(100);
	isa_dmareqoff (t->drq);
	ReadWriteArv (t, (caddr_t)chkdma, 40);
	DELAY(100);
	ReadWriteArv (t, (caddr_t)chkdma, 40);
	for (i=0; i<2; i++)
	{
	 if (ReadWriteArv (t, (caddr_t)chkdma, 40) > 0) return (0);
/*	 DELAY (100); */	/* это от балда :) */
/* А во FreeBSD это как ? :( */
/*	 if ((isa_dmaCount (chan)) > 0 ) return (0); */
	 dprintf ((".%d-%d.", t->drq, i));
	}
/* Сброс, чтоб светодиод не горел зря */
	for (i=0; i<10; i++) chkdma[i] = 0x79;
	for (i=10; i<20; i++) chkdma[i] = 0x38;
	ReadWriteArv (t, (caddr_t)chkdma, 40);
	return (1);
};

int testdma (avt_unit *t)
{
int	i;
u_short	tstdma[1000];
/* главное чтоб в памяти небыло таких слов */
	for (i=0; i<1000; i++) tstdma[i]=0xa5a5;
	t->dmaflags = B_READ;
	if (ReadWriteArv (t, (caddr_t)tstdma, 1000*2) > 0)
	 dprintf(("avt: time out\n"));
	for (i=0; i<1000; i++) if (tstdma[i] == 0xa5a5) return (0);
	return (1);
}
/* #endif AV1020 */

/*
 * Device is found, configure it.
 */
int avtattach (struct isa_device *id)
{
	avt_unit *t = avttab + id->id_unit;

	if (t->controller == UNKNOWN) {
		printf ("avt%d: type UNKNOWN, abort.\n", id->id_unit);
		return 0; }
/* #ifdef AV1010 */
	if (t->controller == Av1010)
	{
	 printf ("avt%d: type <Arvid 1010>\n", id->id_unit);
	 t->maxrate = 100;		/* 100Kb/s */
	 t->maxphase = 3;
	 t->phase = DEFAULT_PHASE10;
	}
/* #endif AV1010 */
/* #ifdef AV1020 */
	if (t->controller == Av1020)
	{
	 printf ("avt%d: type <Arvid 1020>\n", id->id_unit);
	 t->maxrate = 200;		/* 200Kb/s */
	 t->maxphase = 4;
	 t->phase = DEFAULT_PHASE20;
	}
/* #endif AV1020 */
	if (t->controller == Av1030)
		printf ("avt%d: type <Arvid 103x>\n", id->id_unit);
#if 0
	t->size = DataSize;	/* размер данных с информацией */
/* буфер */
	t->buff = malloc (t->size, M_TEMP, M_WAITOK);
	if (! t->buff)
		{
		 dprintf ((" Arvid: buffer allocate error\n"));
		 return (0);
		}
#endif
/* Для всех */
	t->commands = VNONE;		/* tape is STOP */
	t->rate = t->maxrate;		/* максимальная по умолчанию */
	t->commands = VREAD;		/* tape is read */
	kdc_avt[id->id_unit].kdc_state = DC_IDLE;

	return (1);
}

struct isa_driver avtdriver = { avtprobe, avtattach, "avt", };

int hardready (u_short port)
{
 if (inw(port) && HARDREADY)
  return 0;
 else
  return 1;
}

u_short crc16 (avt_kadr *kadr, int start, int c)
{
u_short	crc = 0;
register	byte, i;	/* обязательно так */

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

int load_tpb (avt_kadr *kadr)
{
int	i;

	if (crc16 (kadr, 1032, 11) == kadr->TPB.TapeCRC)
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
	 else return (0); /* увеличить ошибку */
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
 * Interrupt routine.
 * делать как можно меньше вычислений.
 */
void avtintr (int u)
{
	avt_unit *t = avttab + u;
	u_int clear;

	if (u >= NAVT || t->controller == UNKNOWN) {
		dprintf (("avtintr() -- device not configured\n"));
		return;
	}

	t->countintr ++;	/* увеличим счетчик прерываний */

	switch (t->commands)
	{
	 case VREAD:
	 {
	 	if (t->flags & VTOPEN)
	 	 {
		t->cmdport = (((t->phase << 2 ) & PHASEMASK ) | READMASK10 );
		disable_intr();
		switch (t->controller)
		{
/* #ifdef AV1010 */
		case Av1010: {
		 while (hardready(t->STATPORT));
		 outw (t->ADDRPORT, 0);			/* reset addres */
		 avtsetcmd(t->CMDPORT, t->cmdport);	/* установить режим */
		 inw (t->DATAPORT);			/* незнаю, но так всегда */
		 insw (t->DATAPORT, &avtvar.raw, 0x0800);
		 break; }
/* #endif AV1010 */
/* #ifdef AV1020 */
		case Av1020: {
		 break; }
/* #endif AV1020 */
		}	/* t-> controller */
		enable_intr();

		bzero ( avtvar.data, sizeof(avtvar.data));	/* ???????? */
#if 1
		descrembler( 0, 88);	/* дешифруем данные */
#else
		for (avtvar.row = 0; avtvar.row < 88; avtvar.row++)
		 {
		  xdescrembler();	/* дешифруем данные */
		 }
#endif
		if (load_tpb (&avtvar.data))	/* загружаем параметры ленты */
		{
		disable_intr();
		move_data();
		if (fifo.count == 8)
		 { fifo.count = 0; t->OVERFLOW = 1; };
		bcopy ( avtvar.data, &fifo.buf[fifo.count], sizeof(avtvar.data));
		fifo.count++;
		enable_intr();
		}
		}
	 break;
	 }
	 case VWRITE:
	 case VDELAY:
	 case VVCROUT:
	 	break;
	 case VNONE:
	 default:
	 	break;
/*
 * если много пустых прерываний, их надо будет остановить
 */
	 /* return(0);	*/ /* not clear - stop interrupt */
	}
	/* если прерывания не обрабатываються - останавливаемся? */
/*	if (countintr == 50) { dprintf(("\n")); countintr=0; } */
/*	dprintf (("A\n")); */
	t->REQUEST = 1;		/* прерывание закончено */
	if (t->controller == Av1010)
	 if (t->flags & VTOPEN)			/* если драйвер открыт */
	  avtresetintr(t->INTRPORT);	/* сбросить флаг прерывания */
					/* или прерываний больше небудет */
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
	int u = minor (dev) & T_UNIT;
	avt_unit *t = avttab + u;
	int error;
/* неправильный девайс */
	if (u >= NAVT || t->controller == UNKNOWN)
		return (-1);

	/* Check that device is not in use */
	if (t->flags & VTOPEN)
		return (-1);

	t->flags = VTOPEN;		/* открываем девайс */
	t->commands = VREAD;		/* по default чтение */
/* #ifdef AV1010 */
	if (t->controller == Av1010)
	 avtresetintr(t->INTRPORT);	/* очистить флаг прерывания */
/* #endif AV1010 */
/* #ifdef AV1020 */
	if (t->controller == Av1020) {
/*	 t->dmacount = SIZE1020; */
	 t->dmacount = 284 * 9 * 8;
	 t->buf = malloc (t->dmacount, M_TEMP, M_WAITOK);
	  if (!t->buf) return (EAGAIN);
/*	 isa_dmastartblk (B_READ, t->buf, t->dmacount, t->drq);
	 isa_dmareqon (t->drq);
	 DELAY(100);
	 isa_dmareqoff (t->drq); */
	}
/* #endif AV1020 */
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

	if (u >= NAVT || t->controller == UNKNOWN)
		return (-1);

	t->flags = VTCLOSE;
	t->commands = VNONE;
/* #ifdef AV1020 */
	if (t->controller == Av1020) {
	 outb (DMA2_SMSK, t->drq);
	 free (t->buf, M_TEMP); }
/* #endif AV1020 */
	kdc_avt[u].kdc_state = DC_IDLE;
	return (0);
}

/*
 * Ioctl routine.  Compatible with BSD ioctls.
 * There are three possible ioctls:
 * ioctl (int avt, MTIOCGET, struct mtget *buf)  -- get status
 * ioctl (int avt, MTIOCTOP, struct mtop *buf)   -- do BSD-like op
 * ioctl (int avt, ATQICMD, int qicop)           -- do QIC op
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
		ptr->drq = t->drq;
		ptr->flags = t->flags;
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
/*		break; */
	}
	case VGET_TPB:
	{
	*(struct TPB *)data = TapeTPB;
	return 0;
	}
/*
 * очисчаем флаг прерывания, ждем появления (очистки) флажка,
 * копируем данные, выходим
 */
	case VGET_KADR:
	{
/* dprintf (("avt ioctl VGET_DATA")); */
/* ждем результата или пока не кончится время */
	t->REQUEST = 0;
	 while (t->REQUEST == 0 );
/* копируем данные, ведь они так быстро изменяются */	
	disable_intr();
	 *(avt_kadr *)data = *(avt_kadr *)avtvar.data;
	enable_intr();
/* dprintf ((" OK\n")); */
	 return 0;
	}
	case VGET_RAW:
	{
/* dprintf (("avt ioctl VGET_RAW")); */
	t->REQUEST = 0;		/* очистить флаг запроса */
/* ждем результата */
	 while (t->REQUEST == 0 );
	disable_intr();
/* копируем данные, ведь они так быстро изменяются */	
	 *(avt_raw *)data = *(avt_raw *)avtvar.raw;
	enable_intr();
/* dprintf ((" OK\n")); */
	 return 0;
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

	if (! bp->b_bcount)
		goto exit;

	t->flags &= ~VTBUSY;

	if (bp->b_flags && B_READ)
	 {
	  int	count;
	   disable_intr();
/* расчитаем количество имеющихся данных */
	   count = (sizeof(fifo.buf) / 8) * fifo.count;
/*
 * если количество читаемых данных меньше имеющихся
 * то читаем сколько нужно
 */
	   if (count > bp->b_bcount) count = bp->b_bcount;
/* должны что-то делать ??????????????????????? */

	   avtread (t, bp->b_un.b_addr, count);
	   bp->b_resid = bp->b_bcount - count;
	   fifo.count = 0;
	   enable_intr();
	 }

	if (t->flags & VTBUSY) {
errxit:         bp->b_flags |= B_ERROR;
		bp->b_error = EIO;
	}
exit:	biodone (bp);
	return;
}

/* start i/o operation */
static int avtread (avt_unit *t, void *data, unsigned len)
{
	t->flags &= ~VTBUSY;			/* clear exception flag */

	 bcopy ( &fifo.buf, data, len);
	return (1);
}

#endif /* NAVT */
