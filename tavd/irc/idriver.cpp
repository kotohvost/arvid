
#include "idriver.h"

// ����� ������ ������ �� �����. ������ �� 100h ����. (256w/512b)
// ����� ������ �� �������������, ��������� ��� �� ����������.
// ����� ������� ������������� ����� ����� �� t->RADDRES.
// ������������ ������ ��������������� ����� 1000h ����
void ReadArv3x (avt_unit *t, void *rawbuf, size_t len)
{
  u_short addres;
  size_t tmp = 0x100;

  if (len > 0x1000)
    return; // ������ ��� ��� ���� ???
    
  for ( addres = t->RADDRES; len; addres++ )
   {
    outw (t->RA, addres);           // ��������� ����
    inw (t->RD);                    // ��� ���������� '�������'
    if (len < tmp) tmp = len;       // ������ �� ������
    insw (t->RDINC, rawbuf, tmp);   // ��������� �������
    (char *)rawbuf += tmp * 2;      // �������� ����� � ������
    len -= tmp;                     // �������� ����� ������
   } // for
}

// ����� ������ ������ � �����. ����� �� 100h ����. (256w/512b)
// ����� �� �������������, ��������� ��� �� ����������.
// ����� ������� ������������� ����� ����� �� t->RADDRES.
// ������������ ������ ��������������� ����� 1000h ����
void WriteArv3x (avt_unit *t, void *rawbuf, size_t len)
{
  u_short addres;
  size_t tmp = 0x100;

  if (len > 0x1000)                 // �� ������
    len = 0x1000;

  for (addres = t->RADDRES; len; addres++)
   {
    outw (t->RA, addres);           // ��������� ����
    if (len < tmp) tmp = len;       // ������ �� �����
    outsw (t->RDINC, rawbuf, tmp);  // ������� �������
    (char *)rawbuf += tmp * 2;      // �������� ����� � ������
    len -= tmp;                     // �������� ����� �������
   } // for
}

// ����� ������/������ ������ ���� ��������� ����� ������ �����
// ����� ������ "1" � ��� 3 RK
void RWDone3x (avt_unit *t)
{
  #define INCBUFFMASK 0x8

  avtsetcmd ((t->cmdport | INCBUFFMASK));
  if ((t->RADDRES += 0x10) > t->MAXADDR)
    t->RADDRES = 0;
          
  // ����
  avtsetaddr (t->RADDRES);
}

