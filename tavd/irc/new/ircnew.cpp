// Класс используемый для работы с таблицой управления ВМ.
#include <stdlib.h>
#include <fcntl.h>

#ifdef __GNUC__
#include <unistd.h>
#endif

#include <string.h>
#include <ctype.h>

#include "errormes.h" // сообщения об ошибках
#include "ircnew.h"
#include "mytypes.h"
#include "adriver.h"
#include "inline.h"

#if 0
#define DEBUG_IRCOUT
#endif

static	word IRC_ON [] = {
	0xffff, 0xfffe, 0xfffc, 0xfff8, 0xfff0, 0xffe0, 0xffc0, 0xff80,
	0xff00, 0xfe00, 0xfc00, 0xf800, 0xf000, 0xe000, 0xc000, 0x8000 };

static	word IRC_OFF[] = {
	0x0000, 0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f,
	0x00ff, 0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff };

static char *debuginfo;

// Constructor
VCR::VCR () {
#ifdef DEBUG_IRCOUT
	printf ("Constructor");
#endif
	svcr = NULL; skey = Sbuffer = bufkadr = Sbufkod = Nbufkod = NULL;
//	fvcr = 0;
	keynums = 0;
	curmode = 0;
	modesize = 0;
	kadrsize = 0x1000;
	sizeper = 0;
	vcr_34 = 0;
#ifdef DEBUG_IRCOUT
	printf ("\n");
#endif
};

// загрузка таблицы управления ВМ из файла (VCR)
// вызывает:
// KeyNum()
// возвращает:
// 0 - успешно
int VCR::LoadVCR (char *fname)
{
#ifdef DEBUG_IRCOUT
	printf ("LoadVCR: %s", fname);
	printf (" open");
#endif
	// open file
#ifdef UNIX
	int handle = open (fname, O_RDONLY);
#else
	int handle = open (fname, O_RDONLY | O_BINARY);
#endif
	if (handle < 0)
	 {
	  errinf (EOPEN, "LoadVCR: Error open file\n");
	  return (EOPEN);
	 }

	if (!svcr)
	 {
	  // выделим место под таблицу
//	  svcr = (u_char *)malloc(0x996);
	  svcr = (vcrfile *)malloc(sizeof(vcrfile));
	 }

	// считываем таблицу
#ifdef DEBUG_IRCOUT
	printf (" read");
//	if (read (handle, svcr, 0x996) != 0x996) {
#endif
	if (read (handle, svcr, sizeof(vcrfile)) != sizeof(vcrfile))
	 {
	  errinf (EREAD, "LoadVCR: Read error file\n");
	  free (svcr);
	  svcr = NULL;
	  return (EREAD);
	 }

	close (handle);
//	fvcr = 1;
#ifdef DEBUG_IRCOUT
	printf ("CRC: %X\n", svcr->tstsum);
//	printf ("%s\n", svcr->kname[0]);
#endif
	keynums = KeyNum ();
	return (0);
};

// Тест.
// 0 - ошибка
int VCR::KeyNum ()
{
  int key;

  if (svcr == NULL)
   {
    errinf (ETABLENONE, "KeyNum: Table not loaded\n");
    return (0);
   }

  // таблица ссылок на клавиши
  for (key = 0; (svcr->key [key] != 0xff & key < 49) ; key++);
    return (key);
};

// Uncompress
// разборка указателей:
// XXXXxxxx - где
//     XXXX - пауза
//     xxxx - пакет
// 0 - успешно
int VCR::Uncompress (int key)
{
  vcrfile *nvcr;	  // указатель на таблицу управления
  word	  *nkey;	  // указатель на последовательность
  word maxpaus = 0;    // максимальная длина паузы
  byte n, p;
  int	  i, keytmp;

  if (svcr == NULL)
   {
    errinf (ETABLENONE, "Uncompress: Table not loaded\n");
    return (1);
   }

  // проверим и извлечом номер последовательности
  if (key >= keynums)
   {
    errinf (ENUMKEY, "Uncompress: Key number error\n");
    return (ENUMKEY);
   }

  // резервируем область
  if (!skey)
    skey = (word*)malloc(0x100 * 2);

  nvcr = svcr;
  nkey = skey;
// key -> num buffer
// keytmp = svcr->key[key] * 0x68;
  keytmp = svcr->key[key];

#ifdef DEBUG_IRCOUT
  printf ("KeyTmp: %d/%d\n", keytmp, svcr->key[key]);
#endif
  for (i = 0; i < 0x68; i++)
   {
//  p = svcr->a[0x23A + keytmp + i];
    p = svcr->pack[keytmp][i];
#ifdef DEBUG_IRCOUT
    printf("%02X.",p);
#endif
    // пакет
    n = p & 0x0f;

    if (n == 0x0f)
      break;

    *nkey = svcr->type.paket[n];
#ifdef DEBUG_IRCOUT
    printf("%04X.", *nkey);
#endif
    nkey++;

    // пауза
    n = p >> 4;

    if (n == 0x0f)
      break;

    *nkey = svcr->type.pause[n];

    if (maxpaus < *nkey)
      maxpaus = *nkey;

#ifdef DEBUG_IRCOUT
    printf("%04X.", *nkey);
#endif
    nkey++;
   }

  *nkey = 0; // заполнили

// обработка модулированного сигнала
// тип модуляции
// if (!svcr->a[0x238]) {

  if (!svcr->mod)
   {
    int     tmp;

    // период в мкс.
    tmp = svcr->a[0x1BC] * maxpaus / 1000;

    if (tmp > 82)
     {
      word_23e = 3;
     }
    else if (tmp > 62)
     {
      word_23e = 2;
     }
    else if (tmp > 42)
     {
      word_23e = 1;
     }
    else
     {
      word_23e = 0;
     }

    //printf ("tmp %d\n", tmp);
   }

// подсчитаем количество бит в периоде
//if (svcr->a[0x238]) {
  if (svcr->mod)
   {
    // немодулированный сигнал
    sizeper = svcr->a[0x1BC] * 340 / 238;
   }
  else
   {
    // модулированный сигнал
    sizeper = svcr->a[0x1BC] * 3;
   }

  sizeper++;

  // установим четное количество периодов в буфере
  modesize = (kadrsize / (sizeper * 12)) * (sizeper * 12);
  // установим адрес буфера и флаг старта
  Sbufkod = skey;
  startflag = 1;
  // проверим наличие репетитора у клавиши
  IsRepet = FindRepetitor (key);
#ifdef DEBUG_IRCOUT
  printf ("Rep: %d\n", IsRepet);
#endif

  // 1795
  vcr_32 = 0;

  // указатель на буфер кадра вывода
  if (!Sbuffer)
    Sbuffer = (word*) malloc (0x1000*2);

  // обнулим
  bzero (Sbuffer, 0x1000*2);

  // Здесь сбрасывается адрес в карте
  // и выводиться 0xEA0 слов.
  // Затем устанавливается режим вывода IRC.
  // 1932
  // 18B7
  // 1887
  fEndKey = 0;
  Nbufkod = Sbufkod;
  pause = startflag;
  kodn = 0;
  vcr_2C = vcr_2 = 0;

  // end 1887
  numkadr = vcr_30 = 0;
  // MakeIRCbufbit
  bufbit = modesize * 16;
  bit = 0;
  bufkadr = Sbuffer;
  // end MakeIRCbufbit
  // устанавливаем адрес первого кадра в карте
  // end 18B7
  fStartP = pause = 1;
  // преобразовывается и выводится команда.
  // end 1932
  return (0);
};

// Поиск репетитора у клавиши. Возможна ошибка выхода за пределы. !!!
int VCR::FindRepetitor (int key)
{
  int i = 0;

  while (svcr->a[0x158 + i] != 0xff)
   if (svcr->a[0x158 + i] == key)
     return (1);
   else
     i++;

  return (0);
};

// преобразование последовательности в сигнал
int VCR::VCR_1950 ()
{
  int tmpkod;

  // 1602
  while (!fEndKey && bufbit)
   {
    // pause for start
    if (fStartP)
     {
      fStartP = 0;
      kodn = 3000 / sizeper;
     }

    if (!kodn)
     {
      kodn = *Nbufkod++;
      pause ^= 1;
     }

#ifdef DEBUG_IRCOUT
    printf ("kod: %04X/%08X ", kodn, bufbit);
#endif

    if (kodn)
     {
      if (bufbit < (kodn * sizeper))
	tmpkod = bufbit / sizeper;
      else
	tmpkod = kodn;

#ifdef DEBUG_IRCOUT
      printf ("%04X ", tmpkod);
#endif
      // kod to signal
      if (pause)
	IRCtoPause (bit, sizeper, tmpkod, bufkadr);
      else
       {
	if (!svcr->a[0x238])
	  IRCtoMod (bit, sizeper, tmpkod, bufkadr);
	else
	  IRCtoNMod (bit, sizeper, tmpkod, bufkadr);
       } // pause

      kodn -= tmpkod;
      bufbit -= tmpkod * sizeper;
      CorrectBufBit (tmpkod *sizeper);
     }
    else
     {
      fEndKey = 1;
     } // kodn
   } // while

  // 170D
  if (fEndKey <= 3)
   {
    if (bufbit)
     {
      if (vcr_34)
	goto c_1745;

      tmpkod = bufbit / sizeper;
#ifdef DEBUG_IRCOUT
      printf ("kod: %04X/%08X %04X ", kodn, bufbit, tmpkod);
#endif
      IRCtoPause (bit, sizeper, tmpkod, bufkadr);
     } // bufbit

    // MakeIRCbufbit
    bufbit = modesize << 4; // *16
    bit = 0;
    bufkadr = Sbuffer;

    // end MakeIRCbufbit
c_1745:
    if (fEndKey)
     {
      if (!vcr_34)
	fEndKey++;
      else
	fEndKey = 3;
     }
   }

#ifdef DEBUG_IRCOUT
  printf ("VCR_1950 done (%d/%d)\n", bufbit, bufbit/8);
#endif

  // 175B
  if (fEndKey == 3)
    return (1);

  return (0);
};

// Распаковка сигнала Pause.
void VCR::IRCtoPause (int bit, int sizeper, word kod, word *buffer)
{
  int tmpb, tmpk;

#ifdef DEBUG_IRCOUT
  printf ("Pause\n");
#endif
  tmpb = kod * sizeper;

  if (bit)
   {
    *buffer++ &= IRC_OFF[bit];
    tmpb -= 0x10 - bit;
   }

  tmpk = tmpb >> 4;

  if (tmpb & 0x0f)
    tmpk++;

  bzero (buffer, tmpk << 1);
};

// Распаковка в модулированный сигнал
void VCR::IRCtoMod (int bit, int sizeper, word kod, word *buffer)
{
  int perdh, perdl, tmpb;

#ifdef DEBUG_IRCOUT
  printf ("Signal-M\n");
#endif
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
   } // bit

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
   } // kod
};

void VCR::IRCtoNMod (int bit, int sizeper, word tmpkod, word *bufkadr)
{
  printf ("Signal-N\n");
};

void VCR::CorrectBufBit (int bits)
{
#ifdef DEBUG_IRCOUT
  printf ("bufk: %X/%X+%X=", bufkadr, bit, bits/16);
#endif
  bit += bits & 0x0f;
  bufkadr += bits/16;

  if (bit >= 0x10)
   {
    bufkadr++;
    bit &= 0x0f;
   }

#ifdef DEBUG_IRCOUT
  printf ("%X/%X\n", bufkadr, bit);
#endif
};

// управление ВМ, key - номер клавиши пульта
// 0 - OK
int VCR::IrcKeyTrn (int fdout, int key)
{
  int stat, delay = 0, crc32 = 0;
  word *sbuf, *nbuf, *pbuf;

#define IKTBNUM 11

  if ((stat = Uncompress (key)) != 0)
    return (stat);

  stat = 1;
//write (fdout, Sbuffer, modesize * 2);
  sbuf = nbuf = pbuf = (word*) malloc (SSIZE_IRC_FRAME * IKTBNUM);
  bzero (sbuf, SSIZE_IRC_FRAME * IKTBNUM);

  while (stat)
   {
    if (VCR_1950 ())
      stat = 0;

    bcopy (Sbuffer, nbuf, modesize * 2); // copy first part
    //imemcpy (nbuf, Sbuffer, modesize * 2); // copy first part
    nbuf += modesize; // move pointer to begin second part

    // если следующий буфер будет больше или конец
    if (nbuf+modesize > sbuf + (SSIZE_IRC_FRAME/2) * IKTBNUM || stat == 0)
     {
      // новый размер будет больше размера буфера
      // int bsend = 0;
      // расчитаем количество заполненых буферов
      int fullfr = (nbuf - sbuf) / (SSIZE_IRC_FRAME/2);
      // размер для вывода
      int szfullfr = SSIZE_IRC_FRAME * fullfr;

#ifdef DEBUG_IRCOUT
      printf ("fullfr: %d, szfullfr: %d\n", fullfr, szfullfr);
#endif
      // выводим
      /*
      for (pbuf = sbuf; szfullfr && (bsend != -1);)
       {
	do
	 {
	  /// bsend = write (fdout, pbuf, 312*12*2);
	  bsend = SendIRCCommand ((byte*)pbuf, SSIZE_IRC_FRAME);
	 } while (bsend != SSIZE_IRC_FRAME && bsend != -1);

	pbuf += bsend/2;
	szfullfr -= bsend;
       }
      */

      pbuf = sbuf;
      SendIRCCommand ((byte*)pbuf, szfullfr);

      for (int i = 0; i < szfullfr; i++)
	crc32 += *((byte*)((int)pbuf + i));

      pbuf += szfullfr/2;

      // оставшуюся информацию
      int last = nbuf - sbuf - (fullfr * (SSIZE_IRC_FRAME/2));
      last *= 2;
#ifdef DEBUG_IRCOUT
      printf ("last: %d\n", last);
#endif

      // если конец выводим
      if (stat == 0 && last)
       {
	/*
	do
	 {
	  /// bsend = write (fdout, pbuf, last);
	  bsend = SendIRCCommand ((byte*)pbuf, last);
	 }
	while (bsend != last && bsend != -1);
	*/

	SendIRCCommand ((byte*)pbuf, last);

	for (int i = 0; i < last; i++)
	  crc32 += *((byte*)((int)pbuf + i));

#ifdef DEBUG_IRCOUT
	printf ("send last\n");
#endif
	nbuf = sbuf;
       }
      else
       {
	// перенесем в начало
	bcopy (pbuf, sbuf, last);
	//imemcpy (sbuf, pbuf, last);
	// и запомним
	nbuf = sbuf + last/2;
       }
     } // end if
   } // end while

  free (sbuf);
  // подождем
  delay = svcr->a[0x8be] / 50;

  if (!delay)
    delay++;

  printf ("IrcKeyTrn: Sleep %d/%d sec\n", svcr->a[0x8be], delay);
  printf ("crc32: %X\n", crc32);
  sleep (delay);
  return 0;
};

#if 0
// управление ВМ, key - номер клавиши пульта
// 0 - OK
int VCR::IrcKeyTrn (int fdout, int key)
{
  int stat, buf, delay;
  word *sbuf, *nbuf, *pbuf;

#define IKTBNUM 5
  stat = Uncompress(key);

  if (stat)
    return (stat);

  stat = 1;
//write (fdout, Sbuffer, modesize * 2);
  sbuf = nbuf = pbuf = (word*) malloc (312*12 * 2 * IKTBNUM);
  bzero (sbuf, 312*12 * 2 * IKTBNUM);
  buf = 0;
  delay = 0;

  while (stat)
   {
    if (VCR_1950 ())
      stat = 0;

    bcopy (Sbuffer, nbuf, modesize * 2); // copy first part
    nbuf += modesize; // move pointer to begin second part

    // если следующий буфер будет больше или конец
    if (nbuf+modesize > sbuf+312*12*IKTBNUM || stat == 0)
     {
      // новый размер будет больше размера буфера
      int ctmp = 0, wtmp, cbuf;
      // расчитаем количество заполненых буферов
      cbuf = (nbuf - sbuf) / (312 * 12);
      // размер для вывода
      wtmp = 312*12*2 * cbuf;
#ifdef DEBUG_IRCOUT
      printf ("cbuf: %d, wtmp: %d\n", cbuf, wtmp);
#endif
      // выводим
      for (pbuf = sbuf; wtmp && (ctmp != -1);)
       {
	do
	 {
	  /// ctmp = write (fdout, pbuf, 312*12*2);
	  ctmp = SendIRCCommand ((byte*)pbuf, 312*12*2);
	 } while (ctmp != 312*12*2 && ctmp != -1);

	pbuf += ctmp/2;
	wtmp -= ctmp;
       }

      // оставшуюся информацию
      wtmp = nbuf - sbuf - cbuf*312*12;
      wtmp *= 2;
#ifdef DEBUG_IRCOUT
      printf ("wtmp: %d\n", wtmp);
#endif
      // если конец выводим
      if (stat == 0 && wtmp)
       {
	do
	 {
	  /// ctmp = write (fdout, pbuf, wtmp);
	  ctmp = SendIRCCommand ((byte*)pbuf, wtmp);
	 }
	while (ctmp != wtmp && ctmp != -1);

	nbuf = sbuf;
       }
      else
       {
	// перенесем в начало
	bcopy (pbuf, sbuf, wtmp);

	// и запомним
	nbuf = sbuf + wtmp/2;
       }
     } // end if
   } // end while

  free (sbuf);
  // подождем
  delay = svcr->a[0x8be] / 50;

  if (!delay)
    delay++;

  printf ("IrcKeyTrn: Sleep %d/%d sec\n", svcr->a[0x8be], delay);
  sleep (delay);
  return (0);
};
#endif

// управление ВМ, key - символьное описание клавиши
// 0 - OK
int VCR::IrcKeyTrn (int fdout, char *key)
{
  int keynum = 0;

  if (isdigit (*key))
   {
    // цифра
    keynum = atoi(key);
    return(IrcKeyTrn(fdout, keynum));
   }
  else
   {
    if ((keynum = NameToNum (key)) != -1)
      return (IrcKeyTrn (fdout, keynum));
   }

  return(1);
};

// Перевод ВМ из текущего (curmode) состояния в состояние 'key'.
// 0 - OK
int VCR::SetMode (int fdout, int key)
{
  if (curmode == key)
  // состояния совпадают
    return 0;

  curmode = key;
  return (IrcKeyTrn (fdout, key));
};

// Перевод ВМ из текущего (curmode) состояния в состояние 'key'.
// 0 - OK
int VCR::SetMode (int fdout, char *keyname)
{
  int key;

  if ((key = NameToNum (keyname)) == -1)
    return(1);

  return (SetMode (fdout, key));
};

// Перевод имени клавиши в ее число
int VCR::NameToNum (char *name)
{
  int num;

  debuginfo = "VCR::NameToNum";

  if (svcr == NULL)
   {
    printf ("%s: Table not loaded\n", debuginfo);
    return (-1);
   }

  num = 0;
  while ((num < 16) &&
	 (strncasecmp (svcr->kname [num], name, 10) != 0))
   {
#ifdef DEBUG_IRCOUT
    printf ("keys: |%s|\n", svcr->kname [num]);
#endif
    num++;
   }

  if (num < 16)
    return(num);
  else
    printf ("%s: Key not where: %s\n", debuginfo, name);

  return (-1);
};

// перемещать ленту в секундах
// способ перемещения выбирается автоматически
// по окончании перемещения востановить режим
// 0 - OK
int VCR::Move (int fdout, int sec)
{
  int cmode;

  if (sec == 0)
    return 0;

  cmode = curmode;

  if (sec < 0)
   {
    // назад - REW
    printf ("Move: REW %d sec, ", sec);

    if (abs (sec) > svcr->a[0x8de])
     {
      // REW
      printf ("STOP");
      SetMode (fdout, "STOP");
      printf ("-REW\n");
      SetMode (fdout, "REW");
     }
    else
     {
      // Play_REW
      printf ("PLAY");
      SetMode (fdout, "PLAY");
      printf ("-REW\n");
      SetMode (fdout, "REW");
     }
   }
  else
   {
    // вперед - FF
    printf ("Move: FF %d sec\n", sec);

    if (abs (sec) > svcr->a[0x8dc])
     {
      // FF
      SetMode (fdout, "STOP");
      SetMode (fdout, "FF");
     }
    else
     {
      // Play_FF
      SetMode (fdout, "PLAY");
      SetMode (fdout, "FF");
     }
   }

  sleep (abs (sec));
  SetMode (fdout, cmode);
  return 0;
};

// Destructor
VCR::~VCR ()
{
//printf("Destructor");
  // буфер кадра
  if (Sbuffer)
    free (Sbuffer);

  // последовательность кодов
  if (skey)
    free (skey);

  // таблица управления
  if (svcr)
    free(svcr);
//printf("\n");
};
