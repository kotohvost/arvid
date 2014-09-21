#include <string.h>

#include "irc.h"

unsigned short irc_on [16] = { 0xFFFF, 0xFFFE, 0xFFFC, 0xFFF8,
                               0xFFF0, 0xFFE0, 0xFFC0, 0xFF80,
                               0xFF00, 0xFE00, 0xFC00, 0xF800,
                               0xF000, 0xE000, 0xC000, 0x8000 };

unsigned short irc_off [16] = { 0x0000, 0x0001, 0x0003, 0x0007,
                                0x000F, 0x001F, 0x003F, 0x007F,
                                0x00FF, 0x01FF, 0x03FF, 0x07FF,
                                0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF };

void unpackpause (int bit, int period, int kod, unsigned short *buf)
{
  unsigned int sizebit, nbyte;
  sizebit = period * kod;

  if (bit)
   {
    *buf &= irc_off[bit];
    buf++;
    sizebit -= 16 - bit;
   }

  nbyte = sizebit / 16;

  if (sizebit & 0x0F)
    nbyte++;

  memset (buf, 0, nbyte * 2);
}

void unpacknmod (int bit, int period, int kod, unsigned short *buf)
{
  unsigned int indx;

  if (bit)
   {
    if (bit >= 16)
     {
      buf++;
      bit -= 16;
     }

    if (bit)
     {
      bit &= 0x0F;
      *buf = irc_on [bit];
      bit -= 16;
     }
   }

  do
   {
    bit += period;

    for (indx = bit / 16; indx; indx--)
     {
      *buf = 0xFFFF;
      buf++;
     }

     bit &= 0x0F;
    } while (--kod);

  if (bit)
    *buf = irc_off [bit];
}

void unpackmod (int bit, int period, int kod, unsigned short *buf)
{
  unsigned char  perl, perh, _bit, indx;
  perh = period / 2;
  perl = period - perh;

  if (bit)
   {
    if (bit >= 16)
     {
      buf++;
      bit -= 16;
     }

     if (bit)
      {
       bit &= 0x0F;
       *buf = irc_on [bit];
       buf++;
       bit -= 16;
      }
   }

  _bit = bit;

  do
   {
    _bit += perl;

    for (indx = _bit / 16; indx; indx--)
     {
      *buf = 0xFFFF;
      buf++;
     }

    _bit &= 0x0F;

    if (_bit)
     {
      *buf = irc_off[_bit];
      buf++;
      _bit -= 16;
     }

    _bit += perh;

    for (indx = _bit / 16; indx; indx--)
     {
      *buf = 0;
      buf++;
     }

    _bit &= 0x0F;

    if (_bit)
     {
      *buf = irc_on [_bit];
      buf++;
      _bit -= 16;
     }

   } while (--kod);
}

// ------------ settings -----------

void irc_setkodbuf1 (struct IRC *irc, unsigned int *kodbuf)
{
  irc->kodbuf = kodbuf;
  irc->startflag = 1;
}

void irc_setsizeperbuf (struct IRC *irc)
{
  unsigned int m = irc->period * 12;   // слов в строке

  irc->sizekadrw = (unsigned int)(irc->maxsizekadrw / m) * m;
}

void irc_initwr (struct IRC *irc, unsigned int *kodbuf, U16 period, U16 irc_type)
{
  unsigned int ircperiod;

  if (irc_type)
    ircperiod = (period * 352 + 140) / 280;
  else
    ircperiod = period * 3 + 1;

  irc->period = ircperiod;
  irc_setsizeperbuf (irc);
  irc_setkodbuf1 (irc, kodbuf);
}

void irc_initbitbuf (struct IRC *irc)
{
  irc->sizebitbuf = irc->sizekadrw * 16;
  irc->curbit = 0;
  irc->pcurbuf = irc->buffer;
}

void irc_correctbitbuf (struct IRC *irc, unsigned int sizebit)
{
  irc->curbit += sizebit & 0x0F;
  irc->pcurbuf += sizebit / 16;

  if (irc->curbit >= 16)
   {
    irc->pcurbuf++;
    irc->curbit &= 0x0F;
   }
}

// ----------- gettings ------------
unsigned int irc_getperiod (struct IRC *irc)
{
  return (irc->period);
}

unsigned int irc_unpack (struct IRC *irc)
{
  unsigned int ckod, csizebit;

  // незнаю
  if (irc->f84 == 0)
    irc->f80++;

  // распаковка
  while (irc->done == 0 && irc->sizebitbuf)
   {
    // пауза перед выводом
    if (irc->fstartpause)
     {
      irc->fstartpause = 0;
      irc->curkod = 3000 / irc->period;
     }

    // следующий код
    if (irc->curkod == 0)
     {
      irc->curkod = *irc->pkodbuf;
      irc->pkodbuf++;
      irc->fpause ^= 1;
     }

    // распакуем код если он есть
    if (irc->curkod)
     {
      // проверим на помещаемость в буфере вывода
      if (irc->period * irc->curkod > irc->sizebitbuf)
        ckod = irc->sizebitbuf / irc->period;
      else
        ckod = irc->curkod;

      // распаковка пакета или паузы
      if (irc->fpause)
        // распаковка паузы
        unpackpause (irc->curbit, irc->period, ckod, irc->pcurbuf);
      else
       {
        if (irc->ik && irc->ik->irc_type)
          // модулированный сигнал
          unpackmod (irc->curbit, irc->period, ckod, irc->pcurbuf);
        else
          // немодулированный сигнал
          unpacknmod (irc->curbit, irc->period, ckod, irc->pcurbuf);

        irc->f80 = 0;
       }

      irc->curkod -= ckod;
      irc->sizebitbuf -= csizebit = ckod * irc->period;
      irc_correctbitbuf (irc, csizebit);
    }
   else
     irc->done = 1;
  } // while (irc->done == 0 && irc->sizebitbuf)

  // непонятно
  if (irc->done < 3)
   {
    if (irc->sizebitbuf)
     {
      if (irc->f78 == 0)
       {
        unpackpause (irc->curbit,
                     irc->period,
                     irc->sizebitbuf / irc->period,
                     irc->pcurbuf);
        irc_initbitbuf (irc);
       }
     }
    else
      irc_initbitbuf (irc);

    if (irc->done)
     {
      if (irc->f78 == 0)
        irc->done++;
      else
        irc->done = 3;
     }
   }

  return (irc->done == 3);
}

#if 0

unsigned int irc_write_start (struct IRC *irc)
{
  unsigned int rc, count;

  rc = 0;
  count = 0;

  if (buf_write_irc (irc))
   {
    do
     {
      rc = irc_unpack (irc);
      buf_writel (irc);
     } while (++count < 4 && rc == 0 && buf_write_irc (irc));
    }

  return rc;
}

unsigned int irc_isfreekadr (struct IRC *irc)
{
  if (irc->done == 0)
   {
   }
  else
    return 0;
}

unsigned int irc_writehl (struct IRC *irc)
{
  unsigned int rc;

  if (irc->done == 3)
   {
    rc = irc_isfreekadr (irc);

    if (rc)
     {
      if (irc->f74 == 0)
        rc = 0;
      irc->f74++;
     }
   }
  else
   {
    irc_write_start (irc);
    rc = 0;
   }

  return rc;
}

//----------------------------------------------

void irc_init_start (struct IRC *irc)
{
  irc->done = 0;
  irc->pkodbuf = irc->kodbuf;
  irc->fpause = irc->startflag;
  irc->curkod = 0;
  irc->f14 = irc->f68 = 0;
}

void irc_wrinit_start (struct IRC *irc)
{
  irc_init_start (irc);
  irc->f38 = 0;
  irc->f70 = 0;
  irc_initbitbuf (irc);
}

// оно когда нибудь вызавается?

void irc_write_init (struct IRC *irc)
{
  irc_wrinit_start (irc);
  irc->fstartpause = 1;
  irc->fpause = 1;
  irc_write_start (irc);
}

#endif
