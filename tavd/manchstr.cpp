#include "compiler.h"
#include <_dpmi.h>

#include "mytypes.h"
#include "console.h"
#include "adriver.h"
#include "dpmiserv.h"
#include "manchstr.h"
#include "mtime.h"

char *ManchTableC  = NULL;
char *ManchTableD  = NULL;
word *SpeedLowTabl = NULL;
char *SpeedHighTabl= NULL;

#define ManWidth 8
#define ManLen	 (1 << (ManWidth))
#define ManSize  ((ManWidth + 7) >> 3)
#define ManNum	 (ManLen * ManSize)

static inline char ManchCoder (char Byte, dword LastLevel)
{
  if (LastLevel) return ManchTableC[Byte];
   else 	 return ~ManchTableC[Byte];
}

static inline char ManchDecoder(char Byte, dword LastLevel)
{
  if (LastLevel) return ManchTableD[Byte];
   else 	 return ManchTableD[~Byte];
}

void RecalcManchesterTable(void)
{
  dword SelectMask, LastBit, LastLevel, LevelCount, CurBit;
  int hManchCtabl = -1, hManchDtabl = -1;

  if (!(ManchTableC = new char [ManNum]))
    ErrorExit (1);

  if (!(ManchTableD = new char [ManNum]))
    ErrorExit (1);

  LockMemory (ManchTableC, ManNum, LOCK_MEMORY);
  LockMemory (ManchTableD, ManNum, LOCK_MEMORY);

  if (((hManchCtabl = open (ManchCName, O_BINARY | O_RDWR, FILE_ACCESS)) == -1)
    ||((hManchDtabl = open (ManchDName, O_BINARY | O_RDWR, FILE_ACCESS)) == -1))
   {
    memset (ManchTableC, 0, ManNum);
    memset (ManchTableD, 0, ManNum);

    for (dword i = 0; i < ManLen; i++)
     {
      SelectMask = 1;
      LastBit	 = 0;
      LastLevel  = 0;
      LevelCount = 0;

      for (dword j = 0; j < ManWidth; j++)
       {
	CurBit = (SelectMask & i) >> j;
	//если битовая последовательность не изменилась, то сменить уровень
	//на инверсный
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
	//если битовая последовательность не изменилась, то сменить уровень
	//на инверсный
	if (!(CurBit ^ LastBit)) LevelCount++;
	LastBit = CurBit;
	SelectMask <<= 1;
       }
*/

      ManchTableD[ManchTableC[i]] = i;
     }

    hManchCtabl = open (ManchCName, O_CREAT|O_BINARY|O_RDWR, _A_NORMAL);
    write (hManchCtabl, ManchTableC, ManNum);
    hManchDtabl = open (ManchDName, O_CREAT|O_BINARY|O_RDWR, _A_NORMAL);
    write (hManchDtabl, ManchTableD, ManNum);
   }
    else {read (hManchCtabl, ManchTableC, ManNum);
	  read (hManchDtabl, ManchTableD, ManNum);}

  close (hManchCtabl);
  close (hManchDtabl);
}

#define LowSize  (0x100 * sizeof (word))
#define HighSize ((0xAAAA + 1) * sizeof (char))

void RecalcSpeedTable (void)
{
  dword SelectMask, SetMask;
  int hSpeedLowTabl = -1, hSpeedHighTabl = -1;
  word Word;

  if (!(SpeedLowTabl  = (word*) new char [LowSize]))
    ErrorExit (1);

  if (!(SpeedHighTabl = (char*) new char [HighSize]))
    ErrorExit (1);

  LockMemory (SpeedLowTabl, LowSize, LOCK_MEMORY);
  LockMemory (SpeedHighTabl, HighSize, LOCK_MEMORY);

  if (((hSpeedLowTabl  = open (SpeedLowName,  O_BINARY | O_RDWR, FILE_ACCESS)) == -1)
    ||((hSpeedHighTabl = open (SpeedHighName, O_BINARY | O_RDWR, FILE_ACCESS)) == -1))
   {
    memset (SpeedLowTabl,  0, LowSize);
    memset (SpeedHighTabl, 0, HighSize);

    for (dword i = 0; i < 0x100; i++)
     {
      SelectMask = 1;
      SetMask	 = 3;
      Word	 = 0;

      for (dword j = 0; j < 8; j++)
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
}

void CodeManchesterCluster (char *data)
{
  dword LastLevel;
  char Byte;

  for (dword row = 0; row < HDataFrame * 23; row++)
   {
    LastLevel = 1;

    for (dword col = 0; col < WDataFrame; col++)
     {
      Byte = *data;
      Byte = ManchCoder (Byte, LastLevel);
      LastLevel = (Byte & 0x80) >> 7;
      data++;
     }
   }
}

void DecodeManchesterCluster (char *data)
{
  dword LastLevel;
  char Byte;

  for (dword row = 0; row < HDataFrame * 23; row++)
   {
    LastLevel = 1;

    for (dword col = 0; col < WDataFrame; col++)
     {
      Byte = *data;
      Byte = ManchDecoder (Byte, LastLevel);
      LastLevel = (Byte & 0x80) >> 7;
      data++;
     }
   }
}

void InitManchTable (void)
{
  RecalcManchesterTable ();
  RecalcSpeedTable ();
}

void FreeManchTable (void)
{
  delete SpeedHighTabl;
  delete SpeedLowTabl;
  delete ManchTableC;
  delete ManchTableD;

  LockMemory (SpeedLowTabl, LowSize, UNLOCK_MEMORY);
  LockMemory (SpeedHighTabl, HighSize, UNLOCK_MEMORY);
  LockMemory (ManchTableC, ManNum, UNLOCK_MEMORY);
  LockMemory (ManchTableD, ManNum, UNLOCK_MEMORY);
}
