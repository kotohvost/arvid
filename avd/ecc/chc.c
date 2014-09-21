
/* Channal coding */

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#include "defines.h"
#include "arvid.h"
#include "crc32.h"
#include "chc.h"

byte *ManchTableC   = NULL;
byte *ManchTableD   = NULL;

word *SpeedLowTabl  = NULL;
byte *SpeedHighTabl = NULL;

#define ManWidth 8
#define ManLen	 (1 << (ManWidth))
#define ManSize  ((ManWidth + 7) >> 3)
#define ManNum	 (ManLen * ManSize)

/* dword CRC32Table [256]; */
void Scramble (byte* data, dword len)
{
  dword i, blen = len & 3, dlen = len >> 2, *ddata = (dword*)data;
  byte* bdata = (byte*)&data [dlen << 2];
  
  for (i = 0; i < dlen; i++)
    ddata [i] ^= CRC32Table [i & 0xFF];

  for (i = 0; i < blen; i++)
    bdata [i] ^= (byte) CRC32Table [i & 0xFF];
}

void DeScramble (byte* data, dword len)
{
  Scramble (data, len);
}

static inline byte ManchCoder (byte Byte, dword LastLevel)
{
  if (LastLevel)
    return ManchTableC [Byte];
  else
    return ~ManchTableC [Byte];
}

static inline byte ManchDecoder (byte Byte, dword LastLevel)
{
  if (LastLevel)
    return ManchTableD [Byte];
  else
    return ManchTableD [~Byte];
}

static int RecalcManchesterTable (void)
{
  dword i, j, SelectMask, LastBit, LastLevel, LevelCount, CurBit;
  int hManchCtabl = -1, hManchDtabl = -1;

  if (!(ManchTableC = malloc (sizeof(char) * ManNum) ))
    return 0;

  if (!(ManchTableD = malloc (sizeof(char) * ManNum) ))
    return 0;  

  if (((hManchCtabl = open (ManchCName, O_BINARY | O_RDWR, FILE_ACCESS)) == -1)
    ||((hManchDtabl = open (ManchDName, O_BINARY | O_RDWR, FILE_ACCESS)) == -1))
   {
    memset (ManchTableC, 0, ManNum);
    memset (ManchTableD, 0, ManNum);

    for (i = 0; i < ManLen; i++)
     {
      SelectMask = 1;
      LastBit	 = 0;
      LastLevel  = 0;
      LevelCount = 0;

      for (j = 0; j < ManWidth; j++)
       {
	CurBit = (SelectMask & i) >> j;
	
	if (!(CurBit ^ LastBit))
	 {
	  LastLevel ^= 1;
	  LevelCount++;
	 }

	ManchTableC [i] |= LastLevel << j;
	LastBit = CurBit;
	SelectMask <<= 1;
       }

/*
      LastLevelCount = LevelCount;
      SelectMask = 1;
      LastBit	 = 0;
      LevelCount = 0;

      for (j = 0;j < ManWidth; j++)
       {
	CurBit = (SelectMask & ManchTableC[i]) >> j;
	
	if (!(CurBit ^ LastBit)) LevelCount++;
	LastBit = CurBit;
	SelectMask <<= 1;
       }
*/

      ManchTableD [ManchTableC[i]] = i;
     }

    hManchCtabl = open (ManchCName, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);
    write (hManchCtabl, ManchTableC, ManNum);
    hManchDtabl = open (ManchDName, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);
    write (hManchDtabl, ManchTableD, ManNum);
   }
  else
   {
    read (hManchCtabl, ManchTableC, ManNum);
    read (hManchDtabl, ManchTableD, ManNum);
   }

  close (hManchCtabl);
  close (hManchDtabl);
  return 1;
}

#define LowSize  (0x100 * sizeof (word))
#define HighSize ((0xAAAA + 1) * sizeof (char))

static int RecalcSpeedTable (void)
{
  dword i, j, SelectMask, SetMask;
  int hSpeedLowTabl = -1, hSpeedHighTabl = -1;
  word Word;

  if (!(SpeedLowTabl  = (word*) malloc (sizeof (char) * LowSize) ))
    return 0;

  if (!(SpeedHighTabl = (char*) malloc (sizeof (char) * HighSize) ))
    return 0;

  if (((hSpeedLowTabl  = open (SpeedLowName,  O_BINARY | O_RDWR, FILE_ACCESS)) == -1)
    ||((hSpeedHighTabl = open (SpeedHighName, O_BINARY | O_RDWR, FILE_ACCESS)) == -1))
   {
    memset (SpeedLowTabl,  0, LowSize);
    memset (SpeedHighTabl, 0, HighSize);

    for (i = 0; i < 0x100; i++)
     {
      SelectMask = 1;
      SetMask	 = 3;
      Word	 = 0;

      for (j = 0; j < 8; j++)
       {
	if (i & SelectMask)
	  Word |= SetMask;

	SelectMask <<= 1;
	SetMask    <<= 2;
       }

      SpeedLowTabl [i] = Word;
      SpeedHighTabl [Word & 0xAAAA] = i;
     }

    hSpeedLowTabl = open (SpeedLowName, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);
    write (hSpeedLowTabl, SpeedLowTabl, LowSize);
    
    hSpeedHighTabl = open (SpeedHighName, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);
    write (hSpeedHighTabl, SpeedHighTabl, HighSize);
   }
  else
   {
    read (hSpeedLowTabl, SpeedLowTabl, LowSize);
    read (hSpeedHighTabl, SpeedHighTabl, HighSize);
   }

  close (hSpeedLowTabl);
  close (hSpeedHighTabl);
  return 1;  
}

void CodeManchesterBlock (byte *data, dword Len)
{
  dword i, LastLevel = 1;

  for (i = 0; i < Len; i++)
   {
    *data = ManchCoder (*data, LastLevel);
    LastLevel = (*data & 0x80) >> 7;
    data++;
   }
}

void DecodeManchesterBlock (byte *data, dword Len)
{
  dword i, LastLevel = 1;

  for (i = 0; i < Len; i++)
   {
    *data = ManchDecoder (*data, LastLevel);
    LastLevel = (*data & 0x80) >> 7;
    data++;
   }
}

void InitCHCTable (void)
{
  RecalcManchesterTable ();
  RecalcSpeedTable ();
}

void FreeCHCTable (void)
{
  free (SpeedHighTabl);
  free (SpeedLowTabl);
  free (ManchTableC);
  free (ManchTableD);
}
