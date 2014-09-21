/*
 * Copyright (C): Vladimir Zarozhevsky <vladimir@mark-itt.ru>
 * FidoNet:	  2:5050/3 Work, 2:5050/17 & 2:5050/2.10 Home
 * 
 * This software is distributed with NO WARRANTIES, not even the implied
 * warranties for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Authors grant any other persons or organisations permission to use
 * or modify this software as long as this message is kept with the software,
 * all derivative works or modified versions.
 */

#ifndef __VCR_H__
#define __VCR_H__

#pragma pack(push, 1)
/* must be packed */
/* .vcr file structure */
struct To
{
  byte key[6];
};

struct From
{
  byte st[6];
  byte pl[6];
};

struct Delay
{
  byte from[10]; /* Stop, Play, Play_FF, Play_REW, nPlay_FF, nPlay_REW, FF, REW, Rec, Pause */
};

typedef struct
{
  int  major, minor;  /* +0 Version Id = 2.0 */

  byte b[0x10];       /* +8 */
  byte PowerOn[32];   /* +18h smart sequience for PowerOn */

  struct To Sto[10];  /* +38h Stop to x */
  struct To Pto[10];  /* +74h Play to x */
  struct From Tox [10];/* +B0 From x to Stop/Play */

  byte z[0x10];       /* +128 */
  byte PowerOff[32];  /* +138 */
  byte keysrep[0x2c]; /* +158 keys with repeetors */
  word rep;           /* +184 repeetor signicha */
  word nrep;          /* +186 repeetors repeet */
  word nkey;          /* +188 used keys number */
  byte key[50];       /* +18A pointer to keys number buffer*/
  word period;        /* +1BC period in mks */

  struct type
   {
    word paket[11];
    word pause[11];
   } type;            /* +1BE */

  byte z1[78];        /* +1EA */
  word mod;           /* +238 */
  byte pack[0x10][0x68];
  word tstsum;        /* +8BA CRC16 */
  word tstsum1;       /* +8BC */

  struct SPLP         /* recovered by Sl */
   {
    word  keytokey;    /* +8BE Usual delay between keys,frame */
    word  speedFF;     /* +8C0 Angular tape speed in FF ,unit (f1 * 2 * pi) */
    word  speedREW;    /* +8C2 Angular tape speed in REW,unit (f2 * 2 * pi) */
    word  c_spPLAY_FF; /* +8C4 Coeff. of SP PLAY_FF , * 100 */
    word  c_spPLAY_REW;/* +8C6 Coeff. of SP PLAY_REW, * 100 */
    word  c_lpPLAY_FF; /* +8C8 Coeff. of LP PLAY_FF,  * 100 */
    word  c_lpPLAY_REW;/* +8CA Coeff. of LP PLAY_REW, * 100 */
    word  delayFF;     /* +8CC Transit. delay in FF  */
    word  delayREW;    /* +8CE Transit. delay in REW */
    short disp_spPLAY_FF;  /* +8D0 Displacement from target position in SP PLAY_FF ,frame */
    short disp_spPLAY_REW; /* +8D2 Displacement from target position in SP PLAY_REW,frame */
    short disp_lpPLAY_FF;  /* +8D4 Displacement from target position in LP PLAY_FF ,frame */
    short disp_lpPLAY_REW; /* +8D6 Displacement from target position in LP PLAY_REW,frame */
    short maxb;            /* +8D8 Maximal bound of using PLAY_FF in searching,sec */
    short minb;            /* +8DA Minimal bound of using PLAY_REW in searching,sec */
    short bound_spPLAY_FF; /* +8DC Bound of using SP PLAY_FF in searching,sec */
    short bound_spPLAY_REW;/* +8DE Bound of using SP PLAY_REW in searching,sec */
    short bound_lpPLAY_FF; /* +8E0 Bound of using LP PLAY_FF in searching,sec */
    short bound_lpPLAY_REW;/* +8E2 Bound of using LP PLAY_REW in searching,sec */
    byte  z2[6];       /* +8E4 */
    short LossREC;     /* +8EA Loss in transition to REC,frame */
    short dispREC;     /* +8EC Displacement in transition to REC,frame */
    byte  z3[8];       /* +8EE */
   } SPLP;

  struct Delay DelTo[10];/* +8F6 Stop, Play, Play_FF, Play_REW, nPlay_FF, nPlay_REW, FF, REW, Rec, Pause in ticks */

  word RECgap;         /* +95A Record gap,frame */
  byte z4[8];          /* +95C */
  word IdentRew;       /* +964 Distance for rewind before identification, sec */
  byte z5[264];        /* +966 */
  char kname[15][10];  /* +A6E */
  char firm[20];       /* +B04 */
  char model[16];      /* +B18 */
  char author[30];     /* +B28 */
  char email[150];     /* +B46 */
  char remark[0];      /* +BDC */
} vcrfile;
#pragma pack(pop)

typedef struct
{
  vcrfile *svcr;      /* указатель на таблицу управления ВМ */
	
  int keynums;        /* количество клавиш */
  int curmode;        /* текущее состояние ВМ */

  /* используется при распаковке */
  word *skey;         /* указатель на буфер для последовательности */
  int word_23e;       /* непонятно (???) */
  int IsRepet;        /* флаг наличия репетитора */
  int vcr_2;          /* +02h ?? */
  word *Sbuffer;      /* +06h буфер кадра */
  int modesize;       /* +08h разм. кадра Arvid'a для ровного кол. периодов */
  int kadrsize;       /* +0Ah разм. кадра Arvid'a (4096) */
  int numkadr;        /* +0Eh ... */
  int fEndKey;        /* +12h конец последовательности */
  int pause;          /* +14h флаг сигнала паузы */
  word *Sbufkod;      /* +16h еще один адрес буфера */
  word *Nbufkod;      /* +1Ah указатель на следующий код */
  word kodn;          /* +1Eh код для преобразования */
  int sizeper;        /* +20h длина одного периода в битах */
  int bufbit;         /* +22h размер modesize в битах */
  int startflag;      /* +26h старт */
  int bit;            /* +28h текущий бит в последнем слове */
  word *bufkadr;      /* +2Ah буфер кадра */
  int vcr_2C;         /* +2Ch ?? */
  int fStartP;        /* +2Eh первой выводится пауза */
  int vcr_30;         /* +30h ?? */
  int vcr_32;         /* +32h ?? */
  int vcr_34;         /* +34h ?????? неиспользуется(?) */
  
  /* local variables for avd */
  int unit;           /* board number */
  int LPflag;         /* LP flag */
  int auto_power;     /* Auto Power On/Off */
  int vcr_size;       /* .vcr file size */
} vcr;

/* Part of original high level API definition arvidapi.h */
/* Implemented for old software portability        */

/****************************************************************************
        Файл-описание интерфейса АрВид V1.03 (Arvid API)
****************************************************************************/

/* VHS modes */
/* original keys defines */
enum {
   VCR_STOP = 0 ,
   VCR_PLAY     ,
   VCR_PLAY_FF  ,
   VCR_PLAY_REW ,
   VCR_nPLAY_FF ,    /* reserved */
   VCR_nPLAY_REW,    /* reserved */
   VCR_FF       ,
   VCR_REW      ,
   VCR_REC      ,
   VCR_MAX = VCR_REC /* maximal mode */
};

/* our keys defines */
enum {
   STOP = 0 ,
   PLAY     ,
   PLAY_FF  ,
   PLAY_REW ,
   nPLAY_FF , /* reserved */
   nPLAY_REW, /* reserved */
   FF       ,
   REW      ,
   REC      ,
   MAX = REC  /* maximal mode */
};

extern void InitVCR (vcr *pvcr, int unit, byte AutoPower, byte LPflag); /* constructor */
extern void FreeVCR (vcr *pvcr);                    /* destructor */
extern int  LoadVCR (vcr *pvcr, char *vcr_file); /* загрузка из файла */

extern int  CheckStop (vcr *pvcr);
extern int  KeyNum    (vcr *pvcr);              /* количество кнопок в VCR'e */
extern int  Uncompress (vcr *pvcr, int key);    /* заполнение последовательности кодов */
extern int  FindRepetitor (vcr *pvcr, int key); /* наличие репетитора у клавиши */
extern int  VCR_1950      (vcr *pvcr);          /* распаковка сигналов */
extern void CorrectBufBit (vcr *pvcr, int bits);

extern void IRCtoPause (int bit, int sizeper, word kod, word *buffer);
extern void IRCtoMod   (int bit, int sizeper, word kod, word *buffer);
extern void IRCtoNMod  (int bit, int sizeper, word kod, word *buffer);
 
/* управление ВМ с отслеживанием текущего состояния */
extern int  SetVHSMode       (vcr *pvcr, int key_num);    /* key by number */
 
/* управление ВМ */
extern int  IrcKeyTrn       (vcr *pvcr, int key_num);    /* key by number */
extern int  IrcKeyTrnByName (vcr *pvcr, char *key_name); /* key by name */
 
/* Перевод имени клавиши в ее индекс */
extern int  NameToNum (vcr *pvcr, char *key_name);

/* Перемещать ленту вперед/назад в секундах */
extern int  MoveTape (vcr *pvcr, int sec, int end_mode);
extern int  VHSopen  (vcr *pvcr, int unit, char *name, byte AutoPower, byte LPflag);
extern int  VCRTest  (int unit, char *name);
extern int  VHSclose (vcr *pvcr);

extern void PrintVCRfile (vcr *pvcr);

#endif
