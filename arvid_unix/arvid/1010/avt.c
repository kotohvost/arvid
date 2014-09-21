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
	enum avttype controller;	/* type of controller */
	int	id;		/* ���� ��� ��������� */
	int	unit;
	u_short	port;		/* base i/o port */
	u_short	irq;		/* irq */
	u_short	maxrate;	/* controller density */
	u_char	maxphase;	/* */
	u_short	flags;		/* ���� ������� */
	u_short	cmdport;	/* copy CMDPORT */
#define READMASK10	0x2	/* 0000000000000010B */
#define WRITEMASK10	!READMASK10 /* 0x-3 1111111111111101B */
	u_char	commands;	/* command of tape drive */
#define VNONE	0		/* none command */
#define	VREAD	1		/* read */
#define VWRITE	2		/* write */
#define VDELAY	3		/* delay in kadr's */
#define VVCROUT	4		/* VCR command */
	u_char	time_out;
	u_short	rate;
	u_char	phase;
	u_long	countintr;	/* ������� ���������� �� ����� */

	void	*vcrtab;	/* VCR table admin videorecorder */
	void	*buf;		/* internal i/o buffer */

#define avtresetintr(tINTRPORT) outw (tINTRPORT, 0);	/* reset interrupt */
#define	avtsetcmd(tCMDPORT, data)	outw (tCMDPORT, data);	/*  */

	u_short DATAPORT, CMDPORT, STATPORT, INTRPORT, ADDRPORT;
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
/* ���� ���� �� ������� ? */
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
/* #ifdef AV1010 */
/* ������� Arvid-1010 */
/* ����������� ��������� */
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
#define AVT_NPORT	4*2	/* ���������� ������������ ������ */
		return (AVT_NPORT);
/* #endif AV1010 */
/* ���� ������������ */
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
	if (inw (t->STATPORT) == 0xffff)	/* �������� ������� ����� */
		{
		 dprintf ((" Arvid 1010 (port) not found\n"));
		 return (0);
		}

	if (irq < 11 || irq > 12)		/* ������������ ���������� */
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
	DELAY (1000000);	/* ������ ���������� ���������� */
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
	return 0;
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
/* #ifdef AV1010 */
	if (t->controller == Av1010)
	{
	 printf ("avt%d: type <Arvid 1010>\n", id->id_unit);
	 t->maxrate = MAXRATE10;		/* 100Kb/s */
	 t->maxphase = MAXPHASE10;
	 t->phase = DEFAULT_PHASE10;
	}
/* #endif AV1010 */
/* #ifdef AV1020 */
	if (t->controller == Av1020)
	{
	 printf ("avt%d: type <Arvid 1020>\n", id->id_unit);
	 t->maxrate = MAXRATE20;		/* 200Kb/s */
	 t->maxphase = MAXPHASE20;
	 t->phase = DEFAULT_PHASE20;
	}
/* #endif AV1020 */
	if (t->controller == Av1030)
		printf ("avt%d: type <Arvid 103x>\n", id->id_unit);
#if 0
	t->size = DataSize;	/* ������ ������ � ����������� */
/* ����� */
	t->buff = malloc (t->size, M_TEMP, M_WAITOK);
	if (! t->buff)
		{
		 dprintf ((" Arvid: buffer allocate error\n"));
		 return (0);
		}
#endif
/* ��� ���� */
	bzero (&avt_vcr, sizeof(struct avt_vcr));
	t->commands = VNONE;		/* tape is STOP */
	t->rate = t->maxrate;		/* ������������ �� ��������� */
	t->flags = VTCLOSE;		/* tape is close */
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
	 else return (0); /* ��������� ������ */
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
 * ������ ��� ����� ������ ����������.
 */
void avtintr (int u)
{
	avt_unit *t = avttab + u;
	u_int clear;

	if (u >= NAVT || t->controller == UNKNOWN) {
		dprintf (("avtintr() -- device not configured\n"));
		return;
	}

	t->countintr ++;	/* �������� ������� ���������� */

	switch (t->commands)
	{
	 case VREAD:
	 {
	 	if (t->flags & VTOPEN)
	 	 {
		disable_intr();
		switch (t->controller)
		{
/* #ifdef AV1010 */
		case Av1010: {
		 t->cmdport = (SETPHASE10(t->phase) | READMASK10 );
		 while (hardready(t->STATPORT));
		 outw (t->ADDRPORT, 0);			/* reset addres */
		 avtsetcmd(t->CMDPORT, t->cmdport);	/* ���������� ����� */
		 inw (t->DATAPORT);			/* ������, �� ��� ������ */
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
		descrembler( 0, 88);	/* ��������� ������ */
#else
		for (avtvar.row = 0; avtvar.row < 88; avtvar.row++)
		 {
		  xdescrembler();	/* ��������� ������ */
		 }
#endif
		if (load_tpb (&avtvar.data))	/* ��������� ��������� ����� */
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
 * ���� ����� ������ ����������, �� ���� ����� ����������
 */
	 /* return(0);	*/ /* not clear - stop interrupt */
	}
	/* ���� ���������� �� ��������������� - ���������������? */
/*	if (countintr == 50) { dprintf(("\n")); countintr=0; } */
/*	dprintf (("A\n")); */
	t->REQUEST = 1;		/* ���������� ��������� */
	if (t->controller == Av1010)
	 if (t->flags & VTOPEN)			/* ���� ������� ������ */
	  avtresetintr(t->INTRPORT);	/* �������� ���� ���������� */
					/* ��� ���������� ������ ������� */
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
	int u = minor (dev) & T_UNIT;
	avt_unit *t = avttab + u;
	int error;
/* ������������ ������ */
	if (u >= NAVT || t->controller == UNKNOWN)
		return (-1);

	/* Check that device is not in use */
	if (t->flags & VTOPEN)
		return (-1);

	t->flags = VTOPEN;		/* ��������� ������ */
	t->commands = VREAD;		/* �� default ������ */
/* #ifdef AV1010 */
	if (t->controller == Av1010)
	 avtresetintr(t->INTRPORT);	/* �������� ���� ���������� */
/* #endif AV1010 */
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

	if (u >= NAVT || t->controller == UNKNOWN)
		return (-1);

	t->commands = VNONE;
	if (t->flags & VCRLOAD) free (t->vcrtab, M_TEMP);
	t->flags = VTCLOSE;
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
/*		break; */
	}
/*
 * �������� ������� ���������� �����������������, ������ ����������
 * ���� ���� ��� ����� ����������� ������������. ����� ����� �����.
 */
	case VSET_VCR:
	{
		bcopy (&avt_vcr, (struct avt_vcr *)data, SIZE_VCR);
		avt_vcr.vcrload = 1;
	}
	case VGET_TPB:
	{
	*(struct TPB *)data = TapeTPB;
	return 0;
	}
/*
 * �������� ���� ����������, ���� ��������� (�������) ������,
 * �������� ������, �������
 */
	case VGET_KADR:
	{
/* dprintf (("avt ioctl VGET_DATA")); */
/* ���� ���������� ��� ���� �� �������� ����� */
	t->REQUEST = 0;
	 while (t->REQUEST == 0 );
/* �������� ������, ���� ��� ��� ������ ���������� */	
	disable_intr();
	 *(avt_kadr *)data = *(avt_kadr *)avtvar.data;
	enable_intr();
/* dprintf ((" OK\n")); */
	 return 0;
	}
	case VGET_RAW:
	{
/* dprintf (("avt ioctl VGET_RAW")); */
	t->REQUEST = 0;		/* �������� ���� ������� */
/* ���� ���������� */
	 while (t->REQUEST == 0 );
	disable_intr();
/* �������� ������, ���� ��� ��� ������ ���������� */	
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

	if (! bp->b_bcount)
		goto exit;

	t->flags &= ~VTBUSY;

	if (bp->b_flags && B_READ)
	 {
	  int	count;
	   disable_intr();
/* ��������� ���������� ��������� ������ */
	   count = (sizeof(fifo.buf) / 8) * fifo.count;
/*
 * ���� ���������� �������� ������ ������ ���������
 * �� ������ ������� �����
 */
	   if (count > bp->b_bcount) count = bp->b_bcount;
/* ������ ���-�� ������ ??????????????????????? */

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
