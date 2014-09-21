
#include "idriver.h"

// Режим чтения данных из карты. Читаем по 100h слов. (256w/512b)
// Режим чтения не устанавливаем, считается что он установлен.
// Перед чтением устанавливаем номер кадра из t->RADDRES.
// Максимальный размер информационного кадра 1000h СЛОВ
void ReadArv3x (avt_unit *t, void *rawbuf, size_t len)
{
  u_short addres;
  size_t tmp = 0x100;

  if (len > 0x1000)
    return; // незнаю как тут быть ???
    
  for ( addres = t->RADDRES; len; addres++ )
   {
    outw (t->RA, addres);           // установим кадр
    inw (t->RD);                    // так называемый 'конвеер'
    if (len < tmp) tmp = len;       // лишнее не читаем
    insw (t->RDINC, rawbuf, tmp);   // прочитаем кусочек
    (char *)rawbuf += tmp * 2;      // увеличим адрес в буфере
    len -= tmp;                     // уменьшим длину чтения
   } // for
}

// Режим записи данных в карту. Пишем по 100h слов. (256w/512b)
// Режим не устанавливаем, считается что он установлен.
// Перед записью устанавливаем номер кадра из t->RADDRES.
// Максимальный размер информационного кадра 1000h СЛОВ
void WriteArv3x (avt_unit *t, void *rawbuf, size_t len)
{
  u_short addres;
  size_t tmp = 0x100;

  if (len > 0x1000)                 // не больше
    len = 0x1000;

  for (addres = t->RADDRES; len; addres++)
   {
    outw (t->RA, addres);           // установим кадр
    if (len < tmp) tmp = len;       // лишнее не пишем
    outsw (t->RDINC, rawbuf, tmp);  // запишем кусочек
    (char *)rawbuf += tmp * 2;      // увеличим адрес в буфере
    len -= tmp;                     // уменьшим длину писания
   } // for
}

// После чтения/записи данных надо увеличить номер буфера кадра
// путем выдачи "1" в бит 3 RK
void RWDone3x (avt_unit *t)
{
  #define INCBUFFMASK 0x8

  avtsetcmd ((t->cmdport | INCBUFFMASK));
  if ((t->RADDRES += 0x10) > t->MAXADDR)
    t->RADDRES = 0;
          
  // надо
  avtsetaddr (t->RADDRES);
}

