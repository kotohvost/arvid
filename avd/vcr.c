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

#include <math.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "defines.h"
#include "dprint.h"
#include "arvid.h"
#include "vcr.h"
#include "ecc.h"
#include "tape.h"
#include "ini.h"
#include "pt.h"

#include "driver_thread.h"
#include "irc.h"

static  word IRC_ON [] =
{
  0xffff, 0xfffe, 0xfffc, 0xfff8, 0xfff0, 0xffe0, 0xffc0, 0xff80,
  0xff00, 0xfe00, 0xfc00, 0xf800, 0xf000, 0xe000, 0xc000, 0x8000
};

static  word IRC_OFF[] =
{
  0x0000, 0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f,
  0x00ff, 0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff
};

/* Constructor */
void InitVCR (vcr *pvcr, int unit, byte AutoPower, byte LPflag)
{
  dprintf (5, "MT:InitVCR:Constructor\n");

  pvcr->svcr = NULL; pvcr->skey = pvcr->Sbuffer = pvcr->bufkadr = pvcr->Sbufkod = pvcr->Nbufkod = NULL;
  pvcr->keynums = 0;
  pvcr->curmode = STOP;
  pvcr->modesize = 0;
  pvcr->kadrsize = 0x1000;
  pvcr->sizeper = 0;
  pvcr->vcr_34 = 0;
  pvcr->unit = unit;
  pvcr->LPflag = LPflag;
  pvcr->auto_power = AutoPower;
}

/* Check VHS on STOP */
int CheckStop (vcr *pvcr)
{
  if (STOP == CST[pvcr->unit].vcr_state)
    return true;
  else
    return false;
}

/* загрузка таблицы управления ВМ из файла (VCR)
 * вызывает:   KeyNum()
 * возвращает: 0 - успешно */

int LoadVCR (vcr *pvcr, char *name)
{
  int handle;
  dprintf (3, "MT:LoadVCR: Open %s\n", name);

  /* open file */
  handle = open (name, O_RDONLY | O_BINARY, FILE_ACCESS);
  
  if (handle < 0)
   {
    dprintf (1, "MT:LoadVCR: Error open %s file: %i\n", name, handle);
    return handle;
   }
   
  pvcr->vcr_size = lseek (handle, 0, SEEK_END);
  lseek (handle, 0, SEEK_SET);

  if (pvcr->svcr)
    free (pvcr->svcr);
  
  /* выделим место под таблицу */
  pvcr->svcr = (vcrfile*) malloc (pvcr->vcr_size);
  
  if (!pvcr->svcr)
   {
    close (handle);
    dprintf (1, "MT:LoadVCR: Can't alloc memory %i bytes for VCR table!\n", pvcr->vcr_size);
    return ERROR;
   }
   
  /* считываем таблицу */
  dprintf (5, "MT:LoadVCR: Read VCR table\n");
  if (read (handle, pvcr->svcr, pvcr->vcr_size) != pvcr->vcr_size)
   {
    dprintf (1, "MT:LoadVCR: Read error file!\n");
    free (pvcr->svcr);

    pvcr->svcr = NULL;
    pvcr->vcr_size = 0;
    return ERROR;
   }

  close (handle);
  dprintf (4, "MT:LoadVCR:VCR file size: %i\n", pvcr->vcr_size);
  dprintf (4, "MT:LoadVCR:CRC: %X\n", pvcr->svcr->tstsum);
  
  dump (5, (byte*)pvcr->svcr->kname, sizeof (pvcr->svcr->kname));

  if (!(pvcr->keynums = KeyNum (pvcr)))
    return ERROR;

  return E_OK;
}

/* 0 - ошибка */
int KeyNum (vcr *pvcr)
{
  int key;

  if (pvcr->svcr == NULL)
   {
    dprintf (1, "MT:KeyNum: Table not loaded!\n");
    return 0;
   }

  /* таблица ссылок на клавиши */
  for (key = 0; pvcr->svcr->key [key] != 0xff && key < 49; key++);
  return key;
}

/* Uncompress
 * разборка указателей:
 * XXXXxxxx - где
 *     XXXX - пауза
 *     xxxx - пакет
 * 0 - успешно */

int Uncompress (vcr *pvcr, int key)
{
  byte n, p;
  vcrfile *nvcr;    /* указатель на таблицу управления */
  word    *nkey;    /* указатель на последовательность */
  word maxpaus = 0; /* максимальная длина паузы */
  int  i, keytmp, tmp;  

  if (pvcr->svcr == NULL)
   {
    dprintf (1, "MT:Uncompress: Table not loaded!\n");
    return ERROR;
   }

  /* проверим и извлечом номер последовательности */
  if (key >= pvcr->keynums)
   {
    dprintf (1, "MT:Uncompress: Key number error!\n");
    return ERROR;
   }

  /* резервируем область */
  if (!pvcr->skey)
    pvcr->skey = (word*) malloc (0x100 * 2);

  nvcr = pvcr->svcr;
  nkey = pvcr->skey;
  
  /* key -> num buffer */
  /* keytmp = svcr->key[key] * 0x68; */
  keytmp = pvcr->svcr->key[key];
  dprintf (4, "MT:Uncompress:KeyTmp: %d/%d\n", keytmp, pvcr->svcr->key[key]);

  for (i = 0; i < 0x68; i++)
   {
    p = pvcr->svcr->pack[keytmp][i];
    dprintf (5, "%02X.", p);

    /* packet */
    n = p & 0x0f;

    if (n == 0x0f)
      break;

    *nkey = pvcr->svcr->type.paket[n];
    dprintf (5, "%04X.", *nkey);
    nkey++;

    /* pause */
    n = p >> 4;

    if (n == 0x0f)
      break;

    *nkey = pvcr->svcr->type.pause[n];

    if (maxpaus < *nkey)
      maxpaus = *nkey;

    dprintf (5, "%04X.", *nkey);
    nkey++;
   }

  *nkey = 0; /* заполнили */

  /* обработка модулированного сигнала тип модуляции*/
  if (!pvcr->svcr->mod)
   {
    /* период в мкс. period */
    tmp = pvcr->svcr->period * maxpaus / 1000;

    /* aproximal sheet! that's curved irc command (sl) */
    if (tmp > 82)
      pvcr->word_23e = 3;
    else
    if (tmp > 62)
      pvcr->word_23e = 2;
    else
    if (tmp > 42)
      pvcr->word_23e = 1;
    else
      pvcr->word_23e = 0;

    /* printf ("tmp %d\n", tmp); */
   }

  /* подсчитаем количество бит в периоде */
  if (pvcr->svcr->mod) /* немодулированный сигнал period */
    pvcr->sizeper = pvcr->svcr->period * 340 / 238;
  else                 /* модулированный сигнал period */
    pvcr->sizeper = pvcr->svcr->period * 3;

  pvcr->sizeper++;

  /* установим четное количество периодов в буфере */
  pvcr->modesize = (pvcr->kadrsize / (pvcr->sizeper * 12)) * (pvcr->sizeper * 12);
  
  /* установим адрес буфера и флаг старта */
  pvcr->Sbufkod = pvcr->skey;
  pvcr->startflag = 1;
  
  /* проверим наличие репетитора у клавиши */
  pvcr->IsRepet = FindRepetitor (pvcr, key);
  dprintf (5, "MT:Uncompress:Rep: %d\n", pvcr->IsRepet);

  /* 1795 */
  pvcr->vcr_32 = 0;

  /* указатель на буфер кадра вывода */
  if (!pvcr->Sbuffer)
    pvcr->Sbuffer = (word*) malloc (0x1000 * 2);

  /* обнулим */
  memset (pvcr->Sbuffer, 0, 0x1000 * 2);

  /* Здесь сбрасывается адрес в карте и выводиться 0xEA0 слов.
   * Затем устанавливается режим вывода IRC.
   * 1932 18B7 1887 */
  pvcr->fEndKey = 0;
  pvcr->Nbufkod = pvcr->Sbufkod;
  pvcr->pause   = pvcr->startflag;
  pvcr->kodn    = 0;
  pvcr->vcr_2C  = pvcr->vcr_2 = 0;
  /* end 1887 */

  pvcr->numkadr = pvcr->vcr_30 = 0;
  
  /* MakeIRCbufbit */
  pvcr->bufbit = pvcr->modesize << 4;
  pvcr->bit = 0;
  pvcr->bufkadr = pvcr->Sbuffer;
  /* end MakeIRCbufbit */
  /* устанавливаем адрес первого кадра в карте */
  /* end 18B7 */
  
  pvcr->fStartP = pvcr->pause = 1;
  /* преобразовывается и выводится команда. */
  /* end 1932 */
  return E_OK;
}

/* Поиск репетитора у клавиши. */
int FindRepetitor (vcr *pvcr, int key)
{
  int i;

  if (pvcr->svcr == NULL)
   {
    dprintf (1, "MT:FindRepetitor: Table not loaded!\n");
    return false;
   }

  for (i = 0; i < 0x2c && pvcr->svcr->keysrep[i] != 0xff; i++)
    if (pvcr->svcr->keysrep[i] == key)
      return true;

  return false;
}

/* преобразование последовательности в сигнал */
int VCR_1950 (vcr *pvcr)
{
  int tmpkod;

  /* 1602 */
  while (!pvcr->fEndKey && pvcr->bufbit)
   {
    /* pause for start */
    if (pvcr->fStartP)
     {
      pvcr->fStartP = 0;
      pvcr->kodn = 3000 / pvcr->sizeper;
     }

    if (!pvcr->kodn)
     {
      pvcr->kodn = *pvcr->Nbufkod++;
      pvcr->pause ^= 1; /* inversion pause flag: click-clap :-)) */
     }

    dprintf (5, "MT:VCR_1950:kod: %04X/%08X ", pvcr->kodn, pvcr->bufbit);

    if (pvcr->kodn)
     {
      if (pvcr->bufbit < (pvcr->kodn * pvcr->sizeper))
        tmpkod = pvcr->bufbit / pvcr->sizeper;
      else
        tmpkod = pvcr->kodn;

      dprintf (5, "%04X ", tmpkod);

      /* kod to signal */
      if (pvcr->pause)
        IRCtoPause (pvcr->bit, pvcr->sizeper, tmpkod, pvcr->bufkadr);
      else
       {
        if (!pvcr->svcr->mod)
          IRCtoMod (pvcr->bit, pvcr->sizeper, tmpkod, pvcr->bufkadr);
        else
          IRCtoNMod (pvcr->bit, pvcr->sizeper, tmpkod, pvcr->bufkadr);
       } /* pause */

      pvcr->kodn -= tmpkod;
      pvcr->bufbit -= tmpkod * pvcr->sizeper;
      CorrectBufBit (pvcr, tmpkod * pvcr->sizeper);
     }
    else
      pvcr->fEndKey = 1; /* kodn */
   } /* end while */

  /* 170D */
  if (pvcr->fEndKey <= 3)
   {
    if (pvcr->bufbit)
     {
      if (pvcr->vcr_34)
        goto c_1745;

      tmpkod = pvcr->bufbit / pvcr->sizeper;
      dprintf (5, "MT:VCR_1950:kod: %04X/%08X %04X ", pvcr->kodn, pvcr->bufbit, tmpkod);
      IRCtoPause (pvcr->bit, pvcr->sizeper, tmpkod, pvcr->bufkadr);
     } /* bufbit */

    /* MakeIRCbufbit */
    pvcr->bufbit = pvcr->modesize << 4;
    pvcr->bit = 0;
    pvcr->bufkadr = pvcr->Sbuffer;

    /* end MakeIRCbufbit */
c_1745:
    if (pvcr->fEndKey)
     {
      if (!pvcr->vcr_34)
        pvcr->fEndKey++;
      else
        pvcr->fEndKey = 3;
     }
   }

  dprintf (5, "MT:VCR_1950 done (%d/%d)\n", pvcr->bufbit, pvcr->bufbit >> 3);

  /* 175B */
  if (pvcr->fEndKey == 3)
    return 1;

  return 0;
}

/* Распаковка сигнала Pause. */
void IRCtoPause (int bit, int sizeper, word kod, word *buffer)
{
  int tmpb, tmpk;

  dprintf (5, "MT:IRCtoPause\n");
  tmpb = kod * sizeper;

  if (bit)
   {
    *buffer++ &= IRC_OFF[bit];
    tmpb -= 0x10 - bit;
   }

  tmpk = tmpb >> 4;

  if (tmpb & 0x0f)
    tmpk++;

  memset (buffer, 0, tmpk << 1);
}

/* Распаковка в модулированный сигнал */
void IRCtoMod (int bit, int sizeper, word kod, word *buffer)
{
  int perdh, perdl, tmpb;

  dprintf (5, "MT:IRCtoMod\n");
  perdh = sizeper >> 1;
  perdl = sizeper - perdh;

  if (bit)
   {
    if (bit >= 0x10)
     {
      buffer++;
      bit -= 0x10;
     }
     
    if (bit)
     {
      bit &= 0x0f;
      *buffer++ = IRC_ON[bit];
      bit -= 0x10;
     }
   } /* bit */

  for (; kod; kod--)
   {
    bit += perdl;
    tmpb = bit >> 4;

    if (tmpb)
      for (; tmpb; tmpb--)
        *buffer++ = 0xffff;

    bit &= 0x0f;

    if (bit)
     {
      *buffer++ = IRC_OFF[bit];
      bit -= 0x10;
     }

    bit += perdh;
    tmpb = bit >> 4;

    if (tmpb)
      for (; tmpb; tmpb--)
        *buffer++ = 0;

    bit &= 0x0f;

    if (bit)
     {
      *buffer++ = IRC_ON[bit];
      bit -= 0x10;
     }
   } /* kod */
}

void IRCtoNMod (int bit, int sizeper, word tmpkod, word *bufkadr)
{
  dprintf (1, "MT:IRCtoNMod - not imlemented!\n");
}

void CorrectBufBit (vcr *pvcr, int bits)
{
  dprintf (5, "MT:CorrectBufBit:bufkadr %X/%X+%X=", pvcr->bufkadr, pvcr->bit, bits >> 4);

  pvcr->bit += bits & 0x0f;
  pvcr->bufkadr += bits >> 4;

  if (pvcr->bit >= 0x10)
   {
    pvcr->bufkadr++;
    pvcr->bit &= 0x0f;
   }

  dprintf (5, "%X/%X\n", pvcr->bufkadr, pvcr->bit);
}

#define IKTBNUM 11
/* управление ВМ, key - номер клавиши пульта */
/* 0 - OK */
int IrcKeyTrn (vcr *pvcr, int key)
{
  word *sbuf, *nbuf, *pbuf;
  __useconds_t mks_delay;
  int stat;
  
  dprintf (4, "MT:IrcKeyTrn:key %d\n", key);

  if ((stat = Uncompress (pvcr, key)) == ERROR)
    return (stat);

  stat = 1;
  sbuf = nbuf = pbuf = (word*) malloc (SSIZE_IRC_FRAME * IKTBNUM);
  memset (sbuf, 0, SSIZE_IRC_FRAME * IKTBNUM);

  while (stat)
   {
    if (VCR_1950 (pvcr))
      stat = 0;

    memcpy (nbuf, pvcr->Sbuffer, pvcr->modesize * 2); /* copy first part */
    /* imemcpy (nbuf, pVCR->Sbuffer, pVCR->modesize * 2); */ /* copy first part */
    nbuf += pvcr->modesize; /* move pointer to begin second part */

    /* если следующий буфер будет больше или конец */
    if (nbuf + pvcr->modesize > sbuf + (SSIZE_IRC_FRAME / 2) * IKTBNUM || stat == 0)
     {
      /* новый размер будет больше размера буфера
       * int bsend = 0;
       * расчитаем количество заполненых буферов */
      int fullfr = (nbuf - sbuf) / (SSIZE_IRC_FRAME / 2);
      
      /* размер для вывода */
      int szfullfr = SSIZE_IRC_FRAME * fullfr;
      dprintf (5, "MT:IrcKeyTrn:fullfr: %d, szfullfr: %d\n", fullfr, szfullfr);

      pbuf = sbuf;
      SendIRCCommand (pvcr->unit, (byte*)pbuf, szfullfr);
      pbuf += szfullfr / 2;

      /* выводим  оставшуюся информацию */
      int last = nbuf - sbuf - (fullfr * (SSIZE_IRC_FRAME / 2));
      last *= 2;
      dprintf (5, "MT:IrcKeyTrn:last: %d\n", last);

      /* если конец выводим */
      if (stat == 0 && last)
       {
        SendIRCCommand (pvcr->unit, (byte*)pbuf, last);
        dprintf (5, "MT:IrcKeyTrn: send last\n");
        nbuf = sbuf;
       }
      else
       {
        /* перенесем в начало */
        memcpy (sbuf, pbuf, last);
        /* и запомним */
        nbuf = sbuf + last / 2;
       }
     } /* end if */
   } /* end while */

  free (sbuf);

  /* wait untill send irc seqience */
  mks_delay = (__useconds_t)pvcr->svcr->SPLP.keytokey * ONE_SECOND / 50;

  if (!mks_delay)
    mks_delay++;

  dprintf (4, "MT:IrcKeyTrn: Sleep %i mks\n", mks_delay);
  usleep (mks_delay);

  return E_OK;
}

/* Перевод имени клавиши в ее индекс */
int NameToNum (vcr *pvcr, char *name)
{
  int key = 0;

  if (pvcr->svcr == NULL)
   {
    dprintf (1, "MT:NameToNum: Table not loaded!\n");
    return ERROR;
   }

  while ((key < pvcr->keynums) &&
         (strncasecmp (pvcr->svcr->kname [key], name, 10) != 0))
   {
    dprintf (5, "MT:NameToNum:keys: |%s|\n", pvcr->svcr->kname [key]);
    key++;
   }

  if (key < pvcr->keynums)
    return (pvcr->svcr->key [key] & 0x3f);
  else
    dprintf (1, "MT:NameToNum: Key not where: %s\n", name);

  return ERROR;
}

/* управление ВМ, key - символьное описание клавиши */
/* 0 - OK */
int IrcKeyTrnByName (vcr *pvcr, char *key)
{
  int keynum = E_OK;

  if (isdigit (*key))
   {
    /* number */
    keynum = atoi (key);
    return IrcKeyTrn (pvcr, keynum);
   }
  else
   {
    if ((keynum = NameToNum (pvcr, key)) != ERROR)
      return IrcKeyTrn (pvcr, keynum);
   }

  return ERROR;
}

/* Play VHS phisical keys sequience from table */
int PlayKeysSeq (vcr *pvcr, byte *seq)
{
  int i, rc = E_OK;

  for (i = 0; i < 6 && seq[i] != 0xff; i++)
   {
    dprintf (4, "MT:PlayKeysSeq: key index %d\n", seq[i]);
    rc = IrcKeyTrn (pvcr, seq[i] & 0x3f);
   }

  return rc;
}

/* 1. PlayKeysSeq (); */
/* 2. Cinematic usleep (mks_delay); */
/* return: 0 - OK */
static int ChangeVHSMode (vcr *pvcr, byte* key_seq, int new_mode)
{
  int rc = PlayKeysSeq (pvcr, key_seq);
  
  if (E_OK == rc)
   {
    byte delay;

    if ((delay = pvcr->svcr->DelTo[new_mode].from[pvcr->curmode]))
     {
      __useconds_t mks_delay = (__useconds_t)delay * ONE_SECOND / 50;

      if (!mks_delay)
        mks_delay++;

      dprintf (4, "MT:ChangeVHSMode: Cinematic delay %i mks\n", mks_delay);
      usleep (mks_delay);
     }

    pvcr->curmode = new_mode;
   }
  
  return rc;
}

static char* kname[] = { "Stop\t", "Play\t", "Play_FF", "Play_REW", "nPlay_FF", "nPlay_REW", "FF\t", "REW\t", "Rec\t", "Pause" };

/* Перевод ВМ из текущего (curmode) состояния в состояние 'new_mode'. */
/* sl: 'new_mode' is virtual key. PlayKeysSeq operates with real key index. */
/* sl: using additional delay for VHS cinematic operation. */
/* return: 0 - OK */
int SetVHSMode (vcr *pvcr, int new_mode)
{
  int rc = E_OK;
  dprintf (3, "MT:SetVHSMode: cur_mode: %s -> new_mode: %s\n", kname [pvcr->curmode], kname [new_mode]);

  if (pvcr->svcr == NULL)
   {
    dprintf (1, "MT:SetVHSMode: Table not loaded!\n");
    return ERROR;
   }

  if (pvcr->curmode == STOP)
    rc = ChangeVHSMode (pvcr, &pvcr->svcr->Sto[new_mode].key[0], new_mode);
  else
  if (pvcr->curmode == PLAY)
    rc = ChangeVHSMode (pvcr, &pvcr->svcr->Pto[new_mode].key[0], new_mode);
  else
   {
    if (new_mode == STOP)
      rc = ChangeVHSMode (pvcr, &pvcr->svcr->Tox[pvcr->curmode].st[0], new_mode);
    else
    if (new_mode == PLAY)
      rc = ChangeVHSMode (pvcr, &pvcr->svcr->Tox[pvcr->curmode].pl[0], new_mode);
    else
     {
      dprintf (4, "MT:SetVHSMode: change mode crossing STOP!\n");
      rc = ChangeVHSMode (pvcr, &pvcr->svcr->Tox[pvcr->curmode].st[0], STOP);
      
      if (E_OK != rc)
	return rc;

      rc = ChangeVHSMode (pvcr, &pvcr->svcr->Sto[new_mode].key[0], new_mode);
     }
   }

  return rc;
}

/* перемещать ленту в секундах, способ перемещения выбирается автоматически
 * 0 - OK */
int MoveTape (vcr *pvcr, int sec, int end_mode)
{
/*  if (!(CST[pvcr->unit].cur_frame || CST[pvcr->unit].mode))
    return ERROR;*/

  if (!sec)
   {
    /* set VHS mode */
    SetVHSMode (pvcr, end_mode);
    return E_OK;
   }

  if (pvcr->svcr == NULL)
   {
    dprintf (1, "MT:MoveTape: Table not loaded!\n");
    return ERROR;
   }

  float time = 0, cur_time = 0, trg_time = 0, p1 = 0, p2 = 0, delta = 0;
  float cur_pos = CST[pvcr->unit].cur_time * avdpar[pvcr->unit].velosity, key_delay;
  float trg_pos = cur_pos + (sec * avdpar[pvcr->unit].velosity);

  if (sec < 0) /* backside motion */ /* REW */ /* STOP_REW */
   {
    dprintf (3, "MT:MoveTape: REW %d sec, ", sec);

    if ((!pvcr->LPflag && abs (sec) > pvcr->svcr->SPLP.bound_spPLAY_REW)
      || (pvcr->LPflag && abs (sec) > pvcr->svcr->SPLP.bound_lpPLAY_REW))
     {
      p1 = pvcr->svcr->SPLP.speedREW * PT[pvcr->unit]->radius;
      p2 = pvcr->svcr->SPLP.speedREW * PT[pvcr->unit]->thickness;

      if (!pvcr->LPflag)
        delta = pvcr->svcr->SPLP.bound_spPLAY_REW * avdpar[pvcr->unit].velosity;
      else
        delta = pvcr->svcr->SPLP.bound_lpPLAY_REW * avdpar[pvcr->unit].velosity;
      
      cur_time = (-p1 + sqrt (p1 * p1 - (p2 * 2 * (PT[pvcr->unit]->tape_len - (cur_pos + delta))))) / p2;
      trg_time = (-p1 + sqrt (p1 * p1 - (p2 * 2 * (PT[pvcr->unit]->tape_len - trg_pos)))) / p2;
      time = trg_time - cur_time;
      
      if (!pvcr->LPflag)
        time += (pvcr->svcr->SPLP.bound_spPLAY_REW * 100.0) / (float)pvcr->svcr->SPLP.c_spPLAY_REW;
      else
        time += (pvcr->svcr->SPLP.bound_lpPLAY_REW * 100.0) / (float)pvcr->svcr->SPLP.c_lpPLAY_REW;
	
      SetVHSMode (pvcr, REW);
     } 
    else /* t = (t * v * 100) / coff * v */ /* PLAY_REW */
     {
      if (!pvcr->LPflag)
        time = (abs (sec) * 100.0) / (float)pvcr->svcr->SPLP.c_spPLAY_REW + 1.0;
      else
        time = (abs (sec) * 100.0) / (float)pvcr->svcr->SPLP.c_lpPLAY_REW + 1.0;

      SetVHSMode (pvcr, PLAY_REW);
     }
   }
  else /* forwardside motion */ /* FF */ /* STOP_FF */
   {
    dprintf (3, "MT:MoveTape: FF %d sec\n", sec);

    if ((!pvcr->LPflag && abs (sec) > pvcr->svcr->SPLP.bound_spPLAY_FF)
      || (pvcr->LPflag && abs (sec) > pvcr->svcr->SPLP.bound_lpPLAY_FF))
     {
      p1 = pvcr->svcr->SPLP.speedFF * PT[pvcr->unit]->radius;
      p2 = pvcr->svcr->SPLP.speedFF * PT[pvcr->unit]->thickness;

      if (!pvcr->LPflag)
        delta = pvcr->svcr->SPLP.bound_spPLAY_FF * avdpar[pvcr->unit].velosity;
      else
        delta = pvcr->svcr->SPLP.bound_lpPLAY_FF * avdpar[pvcr->unit].velosity;
	
      cur_time = (-p1 + sqrt (p1 * p1 - (p2 * 2 * (cur_pos + delta)))) / p2;
      trg_time = (-p1 + sqrt (p1 * p1 - (p2 * 2 * trg_pos))) / p2;
      time = trg_time - cur_time;
      
      if (!pvcr->LPflag)
        time += (pvcr->svcr->SPLP.bound_spPLAY_FF * 100.0) / (float)pvcr->svcr->SPLP.c_spPLAY_FF;
      else
        time += (pvcr->svcr->SPLP.bound_lpPLAY_FF * 100.0) / (float)pvcr->svcr->SPLP.c_lpPLAY_FF;
	
      SetVHSMode (pvcr, FF);
     }
    else /* PLAY_FF */
     {
      if (!pvcr->LPflag)
        time = (abs (sec) * 100.0) / (float)pvcr->svcr->SPLP.c_spPLAY_FF + 1.0;
      else
        time = (abs (sec) * 100.0) / (float)pvcr->svcr->SPLP.c_lpPLAY_FF + 1.0;
     
      SetVHSMode (pvcr, PLAY_FF);
     }
   }

  /* wait untill send irc seqience */
  dprintf (3, "MT:MoveTape:time: %f\n", time);
  key_delay = pvcr->svcr->SPLP.keytokey * ONE_SECOND / 50;
  time *= ONE_SECOND;
  dprintf (3, "MT:MoveTape:key_delay: %f\n", key_delay);

  if (time > key_delay)
    time -= key_delay;

  dprintf (3, "MT:MoveTape:time totally: %f\n", time);

  /* wait VHS */
  usleep (time);
  
  /* set ending VHS mode */
  SetVHSMode (pvcr, end_mode);
  return E_OK;
}

/* Destructor */
void FreeVCR (vcr *pvcr)
{
  dprintf (5, "MT:Destructor\n");

  /* буфер кадра */
  if (pvcr->Sbuffer)
    free (pvcr->Sbuffer);

  /* последовательность кодов */
  if (pvcr->skey)
    free (pvcr->skey);

  /* таблица управления */
  if (pvcr->svcr)
    free (pvcr->svcr);
}

int VHSopen (vcr *pvcr, int unit, char *name, byte AutoPower, byte LPflag)
{
  int i, rc;
  InitVCR (pvcr, unit, AutoPower, LPflag);

  rc = LoadVCR (pvcr, name);

  if (E_OK != rc)
    exit (rc);

  if (pvcr->auto_power)
    for (i = 1; i < sizeof (pvcr->svcr->PowerOn) && pvcr->svcr->PowerOn[i] != 0xff; i += 3)
     {
      rc = IrcKeyTrn (pvcr, pvcr->svcr->PowerOn [i]);
      
      if (E_OK == rc
       && pvcr->svcr->PowerOn [i + 1])
       {
	 __useconds_t mks_delay = (__useconds_t)pvcr->svcr->PowerOn [i + 1] * ONE_SECOND / 50;

	if (!mks_delay)
          mks_delay++;

	dprintf (4, "MT:VHSopen: Cinematic delay %i mks\n", mks_delay);
	usleep (mks_delay);
       }
     }

  return rc;
}

int VHSclose (vcr *pvcr)
{
  int i, rc = E_OK;

  if (pvcr->svcr == NULL)
   {
    dprintf (1, "MT:VHSclose: Table not loaded!\n");
    return ERROR;
   }

  if (pvcr->auto_power)
    for (i = 1; i < sizeof (pvcr->svcr->PowerOff) && pvcr->svcr->PowerOff [i] != 0xff; i += 3)
     {
      rc = IrcKeyTrn (pvcr, pvcr->svcr->PowerOn [i]);
      
      if (E_OK == rc
       && pvcr->svcr->PowerOn [i + 1])
       {
	__useconds_t mks_delay = (__useconds_t)pvcr->svcr->PowerOn [i + 1] * ONE_SECOND / 50;

	if (!mks_delay)
          mks_delay++;

	dprintf (4, "MT:VHSclose: Cinematic delay %i mks\n", mks_delay);
	usleep (mks_delay);
       }
     }

  FreeVCR (pvcr);
  return rc;
}

int VCRTest (int unit, char *name)
{
  vcr VCR;
  int rc = VHSopen (&VCR, unit, name, 1, 0);
  
  if (rc)
    exit (rc);
    
/*  IrcKeyTrn (&VCR, 1);
  sleep (1);
  IrcKeyTrn (&VCR, 0);

  IrcKeyTrn (&VCR, 2);
  sleep (1);
  IrcKeyTrn (&VCR, 0);

  IrcKeyTrn (&VCR, 3);
  sleep (1);
  IrcKeyTrn (&VCR, 0);

  IrcKeyTrn (&VCR, 4);
  sleep (1);
  IrcKeyTrn (&VCR, 0);
  sleep (1); */
  
  SetVHSMode (&VCR, PLAY);
  sleep (1);
  SetVHSMode (&VCR, FF);
  sleep (1);
  SetVHSMode (&VCR, REW);
  sleep (1);
  SetVHSMode (&VCR, PLAY_FF);
  sleep (1);
  SetVHSMode (&VCR, PLAY_REW);
  sleep (1);
  SetVHSMode (&VCR, REC);
  sleep (1);

  return rc = VHSclose (&VCR);
}

void PrintVCRfile (vcr *pvcr)
{
  int x, y;

  printf ("vcrfile.keytokey        (1/50 sec): %d\n", pvcr->svcr->SPLP.keytokey);
  printf ("vcrfile.speedPLAY_FF  (f * 2 * pi): %d\n", pvcr->svcr->SPLP.speedFF);
  printf ("vcrfile.speedPLAY_REW (f * 2 * pi): %d\n", pvcr->svcr->SPLP.speedREW);
  printf ("vcrfile.c_spPLAY_FF      (k * 100): %d\n", pvcr->svcr->SPLP.c_spPLAY_FF);
  printf ("vcrfile.c_spPLAY_REW     (k * 100): %d\n", pvcr->svcr->SPLP.c_spPLAY_REW);
  printf ("vcrfile.c_lpPLAY_FF      (k * 100): %d\n", pvcr->svcr->SPLP.c_lpPLAY_FF);
  printf ("vcrfile.c_lpPLAY_REW     (k * 100): %d\n", pvcr->svcr->SPLP.c_lpPLAY_REW);
  printf ("vcrfile.delayFF         (1/50 sec): %d\n", pvcr->svcr->SPLP.delayFF);
  printf ("vcrfile.delayREW        (1/50 sec): %d\n", pvcr->svcr->SPLP.delayREW);
  printf ("vcrfile.disp_spPLAY_FF  (1/50 sec): %d\n", pvcr->svcr->SPLP.disp_spPLAY_FF);
  printf ("vcrfile.disp_spPLAY_REW (1/50 sec): %d\n", pvcr->svcr->SPLP.disp_spPLAY_REW);
  printf ("vcrfile.disp_lpPLAY_FF  (1/50 sec): %d\n", pvcr->svcr->SPLP.disp_lpPLAY_FF);
  printf ("vcrfile.disp_lpPLAY_REW (1/50 sec): %d\n", pvcr->svcr->SPLP.disp_lpPLAY_REW);
  printf ("vcrfile.maxb               (1 sec): %d\n", pvcr->svcr->SPLP.maxb);
  printf ("vcrfile.minb               (1 sec): %d\n", pvcr->svcr->SPLP.minb);
  printf ("vcrfile.bound_spPLAY_FF    (1 sec): %d\n", pvcr->svcr->SPLP.bound_spPLAY_FF);
  printf ("vcrfile.bound_spPLAY_REW   (1 sec): %d\n", pvcr->svcr->SPLP.bound_spPLAY_REW);
  printf ("vcrfile.bound_lpPLAY_FF    (1 sec): %d\n", pvcr->svcr->SPLP.bound_lpPLAY_FF);
  printf ("vcrfile.bound_lpPLAY_REW   (1 sec): %d\n", pvcr->svcr->SPLP.bound_lpPLAY_REW);
  printf ("vcrfile.LossREC         (1/50 sec): %d\n", pvcr->svcr->SPLP.LossREC);
  printf ("vcrfile.dispREC         (1/50 sec): %d\n", pvcr->svcr->SPLP.dispREC);
  printf ("vcrfile.RECgap          (1/50 sec): %d\n", pvcr->svcr->RECgap);
  printf ("vcrfile.IdentRew           (1 sec): %d\n", pvcr->svcr->IdentRew);

  printf ("\nDelays\nFrom:\t\t|St|Pl|PF|PR|nF|nR|FF|RW|Rc|Ps|\n");
  
  for (y = 0; y < 10; y++)
   {
    printf ("To %s\t|", kname[y]);
    
    for (x = 0; x < 10; x++)
      printf ("%02x|", pvcr->svcr->DelTo[y].from[x]);
      
    printf ("\n");
   }
   
  for (y = 0; y < 15; y++)
   if (*pvcr->svcr->kname[y] != 0)
     printf ("Key %i name: %s\n", y, pvcr->svcr->kname[y]);
     
  printf ("Used keys: %d\n", pvcr->svcr->nkey);
  printf ("Repeetors repeet: %d\n", pvcr->svcr->nrep);
  printf ("Repeetors signicha: %d\n", pvcr->svcr->rep);

  printf ("\nKey number buffer: |");
  for (y = 0; y < sizeof (pvcr->svcr->key) && pvcr->svcr->key[y] != 0xff; y++)
    printf ("%02x|", pvcr->svcr->key[y]);

  printf ("\nKeys repeetors: |");
  for (y = 0; y < 0x2c && pvcr->svcr->keysrep[y] != 0xff; y++)
    printf ("%02x|", pvcr->svcr->keysrep[y]);
 
  printf ("\nPower On Sequence: |");
  for (y = 0; y < 32 && pvcr->svcr->PowerOn[y] != 0xff; y++)
    printf ("%02x|", pvcr->svcr->PowerOn[y]);

  /* stop-to-x */     
  printf ("\n\nSTOP->STOP:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Sto[0].key[y]);
	  
  printf ("\nSTOP->PLAY:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Sto[1].key[y]);
	  
  printf ("\nSTOP->PLAY_FF:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Sto[2].key[y]);
  
  printf ("\nSTOP->PLAY_REW:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Sto[3].key[y]);

  printf ("\nSTOP->nPLAY_FF:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Sto[4].key[y]);

  printf ("\nSTOP->nPLAY_REW:|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Sto[5].key[y]);

  printf ("\nSTOP->FF:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Sto[6].key[y]);

  printf ("\nSTOP->REW:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Sto[7].key[y]);

  printf ("\nSTOP->Rec:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Sto[8].key[y]);

  printf ("\nSTOP->Pause:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Sto[9].key[y]);

  /* play-to-x */
  printf ("\n\nPLAY->STOP:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Pto[0].key[y]);
	  
  printf ("\nPLAY->PLAY:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Pto[1].key[y]);
	  
  printf ("\nPLAY->PLAY_FF:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Pto[2].key[y]);

  printf ("\nPLAY->PLAY_REW:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Pto[3].key[y]);

  printf ("\nPLAY->nPLAY_FF:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Pto[4].key[y]);

  printf ("\nPLAY->nPLAY_REW:|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Pto[5].key[y]);

  printf ("\nPLAY->FF:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Pto[6].key[y]);

  printf ("\nPLAY->REW:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Pto[7].key[y]);

  printf ("\nPLAY->Rec:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Pto[8].key[y]);

  printf ("\nPLAY->Pause:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Pto[9].key[y]);

  /* from x-to-stop */
  printf ("\n\nSTOP->STOP:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[0].st[y]);
	  
  printf ("\nPLAY->STOP:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[1].st[y]);
	  
  printf ("\nPLAY_FF->STOP:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[2].st[y]);

  printf ("\nPLAY_REW->STOP:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[3].st[y]);

  printf ("\nnPLAY_FF->STOP:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[4].st[y]);

  printf ("\nnPLAY_REW->STOP:|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[5].st[y]);

  printf ("\nFF->STOP:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[6].st[y]);

  printf ("\nREW->STOP:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[7].st[y]);

  printf ("\nRec->STOP:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[8].st[y]);

  printf ("\nPause->STOP:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[9].st[y]);
	  
  /* from x-to-play */
  printf ("\n\nSTOP->PLAY:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[0].pl[y]);
	  
  printf ("\nPLAY->PLAY:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[1].pl[y]);
	  
  printf ("\nPLAY_FF->PLAY:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[2].pl[y]);

  printf ("\nPLAY_REW->PLAY:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[3].pl[y]);

  printf ("\nnPLAY_FF->PLAY:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[4].pl[y]);

  printf ("\nnPLAY_REW->PLAY:|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[5].pl[y]);

  printf ("\nFF->PLAY:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[6].pl[y]);

  printf ("\nREW->PLAY:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[7].pl[y]);

  printf ("\nRec->PLAY:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[8].pl[y]);

  printf ("\nPause->PLAY:\t|");
  for (y = 0; y < 6; y++) printf ("%02x|", pvcr->svcr->Tox[9].pl[y]);
    
  printf ("\nVHSs firm: %s\nVHSs model: %s\n", pvcr->svcr->firm, pvcr->svcr->model);
  printf ("\nAuthor: %s\nAuthors e-mail: %s\n", pvcr->svcr->author, pvcr->svcr->email);
  printf ("\nRemark: %s\n", pvcr->svcr->remark);
}
