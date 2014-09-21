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
#include <i386/isa/avt.tbl>	/* корректирующий код */

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
	u_short	port;		/* base i/o port */
	u_short	irq;		/* irq */
	u_short	drq;
	u_short	maxrate;	/* controller density */
	u_char	maxphase;	/* */
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
	u_char	time_out;
	u_short	rate;
	u_char	phase;
	u_long	countintr;	/* счетчик прерываний от платы */

	void	*vcrtab;	/* VCR table admin videorecorder */

	u_short	sizekadr;	/* размер кадра в СЛОВАХ для чтения/записи */
				/* разный для режимов 1030/1052 и ВМ/ИК */
	u_int	rawb;		/* количество заполненых буферов из карты */
	void	*rawbuf;	/* указатель на текущий заполненый буфер */
	u_int	secb;		/* количество заполненых секторов данных */
	void	*secbuf;	/* указатель на текущий заполненый сектор */

#define	avtsetcmd(data)		outw (t->CMDPORT, data);	/*  */
#define avtsetaddr(data)	outw (t->RA, data);

	u_short DATAPORT, CMDPORT, RA, RS;
	u_short	RDINC, RD;
	u_short	RADDRES, MAXADDR;	/* текущий адрес в карте Read/Write */
	u_char	DECODE;			/* флаг декодирования */
#define	RAWREAD		0x0		/* - IORAW/IOIRC */
#define	DATAREAD	0x1		/* - IODATA */
	u_char BUSY, NOEXCEP, RESETMASK, RESETVAL;
	u_char ONLINE, RESET;
	u_char REQUEST;		/* флаг запроса от прерывания */
	u_char OVERFLOW;	/* флаг потери данных */
} avt_unit;

avt_unit avttab[NAVT];			/* tape info by unit number */

struct avtvar	avtvar;			/* все о арвиде */
struct avt_vcr	avt_vcr;		/* VCR таблица для управления видиком */

avt_fifo	fifo;		/* fifo для 1010 */
avt_fifo0	fifo0;		/* fifo new */

static u_int avt_imask = HWI_MASK;	/* маска прерываний при тесте */

struct TPB	TapeTPB;	/* текущая смонтированная лента */

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
 * процедура декодирования блока информации.
 * процедура генерации таблиц востановления
 * вызывается только при первом вызове.
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
/* id для всех одинаковый */
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
	probe->drq  = id->id_drq;
/* #ifdef AV1030 */
/* пробуем Arvid-1030 */
/* прописываем параметры */
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
/* нету контроллеров */
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
/* Для всех */
	bzero (&avt_vcr, sizeof(struct avt_vcr));
	t->commands = VNONE;		/* tape is STOP */
	t->rate = t->maxrate;		/* максимальная по умолчанию */
	t->flags = VTCLOSE;		/* tape is close */
	kdc_avt[id->id_unit].kdc_state = DC_IDLE;

	return (1);
}

struct isa_driver avtdriver = { avtprobe, avtattach, "avt", };

u_short crc16 (avt_kadr100 *kadr, int start, int c)
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

int load_tpb (avt_unit *t)
{
int		i;
u_char		data[32];	/* временный буфер */
HEADER_G	*header_g;	/* заголовок при 100К/с */
HEADER_RS	*header_rs;	/* заголовок при 200К/с */
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
*/	 else return (0); /* увеличить ошибку */
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
 * путем выдачи 1 в 3 бит RK
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
	 case VREAD:
	 {
		/* небудем читать, некуда */
		if (t->rawb >= NBUFRAW) goto avintrexit;
		bufraw = t->rawbuf;	/* указатель на буфер */
		bufraw = bufraw->next;	/* следующий */
		disable_intr();
		switch (t->controller)
		{
/* #ifdef AV1030 */
	case Av1031:
/*
 * SETPHASE - установка фазы
 * READMASK - маска для чтения
 * 0x0008   - инкремент номера буфера
 * 0x0010   - сброс очереди
 * 0x0400   - подключение ВМ к ПС
 */
/*		 t->cmdport = (SETPHASE30(t->phase) | READMASK30); */
/*		 goto IntRead52; */
/*		 break; */	/* case Av1030 */
/* #endif AV1030 */
	case Av1052:
/*		t->cmdport = (SETPHASE30(t->phase) | READMASK30) & t->emulate1030; */
/* IntRead52: */
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
	if (t->DECODE) {	/* DATAREAD */
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
		} /* if (t->DECODE) */
	break;
	}	/* case VREAD: */
	case VWRITE: {
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
/* #ifdef AV1030 */
	case Av1031:
/*
 * SETPHASE - установка фазы
 * READMASK - маска для чтения
 * 0x0008   - инкремент номера буфера
 * 0x0010   - сброс очереди
 * 0x0400   - подключение ВМ к ПС
 */
/*			t->cmdport = (SETPHASE30(t->phase) | WRITEMASK30); */
/*			goto IntRead52; */
/*			break; */	/* case Av1030 */
/* #endif AV1030 */
	case Av1052:
/*		t->cmdport = (SETPHASE30(t->phase) | WRITEMASK30) & t->emulate1030; */
/* IntWrite52: */
		avtsetcmd(t->cmdport);	/* установить режим */
		if ((numk=ReadNumKadr(t)) < 7) {	/* если есть кадры */
			avtsetaddr(t->RADDRES);		/* установим адрес буфера */
			/* пишем данные */
			WriteArvHardw (t, bufraw->data, t->sizekadr);
			bufraw = bufraw->next;	/* следующий */
			t->rawb--;		/* записали еще один */
			ReadWriteDone (t);	/* кадр записан */
			numk++;
			if ((numk<7) && (t->rawb > 0)) {
				bufraw = bufraw->next;
				avtsetaddr(t->RADDRES);	/* установим адрес буфера */
				ReadArvHardw (t, bufraw->data, t->sizekadr);
/*				bufraw=bufraw->next; */	/* запомним */
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
	int 	error, buf;
	struct buf_raw	*rtempbuf;
	struct buf_sec	*stempbuf;

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

	/* резервируем и инициализируем кольцевой буфер для данных */
	stempbuf = t->secbuf = malloc(sizeof(struct buf_sec), M_TEMP, M_WAITOK);
	for (buf = 0; buf < NBUFSEC-1; buf++) {
		stempbuf->next = malloc(sizeof(struct buf_sec), M_TEMP, M_WAITOK);
		stempbuf = stempbuf->next; /* текущий буфер */
		}
	stempbuf->next = t->secbuf;	/* кольцо :) */
	t->secb = 0;			/* обнулили */

	t->flags = VTOPEN;		/* открываем девайс */
	t->commands = VREAD;		/* по default чтение */
/* #ifdef AV1030 */
	avtsetcmd(t->cmdport | 0x10);	/* сбросить очередь */
	t->RADDRES = 0;
/*	if (t->controller >= Av1030) */
		INTREN(t->irq);		/* разрешить прерывания */
/* #endif AV1030 */
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
	struct buf_sec	*stempbuf;

	if (u >= NAVT || t->controller == UNKNOWN)
		return (-1);

	t->commands = VNONE;
	t->flags = VTCLOSE;
/* #ifdef AV1030 */
/*	if (t->controller >= Av1030) */
		INTRDIS(t->irq);		/* запретить прерывания */
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

	/* освободили зарезервированные буфера с данными */
	stempbuf = t->secbuf;
	for (buf = 0; buf < NBUFSEC; buf++) {
		stempbuf = stempbuf->next;
		free(t->secbuf, M_TEMP);
		t->secbuf = stempbuf; /* следующий буфер */
		}
	/* в t->secbuf остался старый адрес первого буфера,
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
		/* установить режим и сбросить очередь */
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
	/*
	 * bufraw теперь указывает на первый из t->rawb прочитанных буферов
	 * теперь копируем буфера в data
	 */
	tmp = t->sizekadr * 2;
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
	t->rawbuf = bufraw;		/* ???? */
	t->flags &= ~VTBUSY;			/* clear exception flag */
	return (1);
}
#endif /* NAVT */
