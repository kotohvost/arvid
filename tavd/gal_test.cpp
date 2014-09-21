#include "compiler.h"

#include "mtime.h"
#include "mytypes.h"
#include "adriver.h"
#include "galey.h"

word HDensity = 0;
//word HDensity = SET_325K;

// Logical parametrs
dword SizeDataFrame = SIZE_DATA_FRAME_1031;
dword SizeOrigCluster = SizeDataFrame * VALUE_LEN;
dword SizeGaleyCluster = SizeDataFrame * CODE_LEN;

void ErrorExit (dword NumError)
{
  switch (NumError)
   {
    case 1:  printf ("Memory allocation error!!!\n"); break;
    case 2:  printf ("Found restore error!!!\n");     break;
    case 3:  printf ("Not found file!!!\n");	      break;
    default: printf ("Unknown Error!!!\n");	      break;
   }

  exit (NumError);
}

char  AnimTable [] = "|/-\\";
dword AnimPhase = 0;
word *TestTable = NULL;

void TestError (void)
{
  dword EtalonCRC, CRC, Comb;
  dword i, j, k, GlobErrCounter = 0;
  int hTesttabl;

  if (!(TestTable = new word [NumElemErrorTable]))
    ErrorExit (1);

  memset (TestTable, 0, NumElemErrorTable * sizeof (word));
  printf ("  Please wait. Code Galey restore test is making...\r");

  time_t start_time = get_time ();
  while (start_time == get_time ());
  start_time = get_time ();

  for (j = 0; j < (1 << VALUE_LEN); j++)
   {
    dword ErrCounter;

    for (i = 0;i < NumElemErrorTable; i++)
     {
      TestTable [i] = eciGaleyDecoderWithErrStat (eciGaleyCoder (j) ^ ErrorTable[i], ErrCounter);
//	TestTable [i] = eciGaleyDecoder (eciGaleyCoder (j) ^ ErrorTable [i]);
//	GlobErrCounter += ErrCounter;
     }

    EtalonCRC =j * NumElemErrorTable;

    for (k = 0, CRC = 0; k < NumElemErrorTable; k++)
      CRC += TestTable [k];

    if (EtalonCRC == CRC)
      printf ("%c\r", AnimTable [(AnimPhase++) & 3]);
      //printf ("Data %X restore okay.\r", j);
     else
      {
       printf ("Data %X restore badly!!!		       \n", j);
       hTesttabl = open (TestName, O_CREAT | O_BINARY | O_RDWR, _A_NORMAL);
       write (hTesttabl, TestTable, NumElemErrorTable * sizeof (word));
       close (hTesttabl);
       ErrorExit (2);
      }
   }

  printf ("Success!!!					       \n");
  Comb = NumElemErrorTable * (1 << VALUE_LEN);
  time_t end_time = get_time ();

  printf ("Restored %d combination, speed rate = %.6f combinations/sec\n",
	  Comb, float (Comb * 1000) / (end_time - start_time));
  printf ("ErrorCounter: %d \n", GlobErrCounter);
  delete TestTable;
}

void TestGaleyStringCoder ()
{
  byte Date [3 * 9 + 1] = {'1','2','3','4','5','6','7','8','9','9','8','7','6','5','4','3','2','1','1','2','3','4','5','6','7','8','9',0};
  byte String [13 * 2 + 1];

  printf ("Src date: %s\n", Date);
  GaleyStringCoder (String, Date, 3);
  StringMixer (String, 18);
//    StringMixer (Date, 26);
  printf ("Encode date: %s\n", String);
  printf ("\n");
//    StringDemixer (Date, 26);
  StringDemixer (String, 18);
  GaleyStringDecoder (Date, String, 3);
  printf ("Restore date: %s\n", Date);
}

void CountPulse ()
{
  dword DimPulseCounts [16];
  memset (DimPulseCounts, 0, sizeof (DimPulseCounts));

  dword i;
  for (i = 0; i < 0x10000; i++)
   {
    word  tmp = i;
    word  right_bit  = tmp & 1;
    dword PulseCount = 0;

    for (dword j = 0; j < 15; j++)
     {
      tmp >>= 1;

      if ((tmp & 1) != right_bit)
       {
	PulseCount++;
	right_bit = tmp & 1;
       }
     }

    DimPulseCounts [PulseCount]++;
   }

  for (i = 0; i < 16; i++)
    printf ("%2d pulse consist in %d word.\n", i, DimPulseCounts [i]);
}

void CountRedundencyCode ()
{
  dword NShRedundencyCount = 0;
  dword ShRedundencyCount = 0;

  // Count phase not shifted codes
  dword i;
  for (i = 0; i < 0x10000; i++)
   {
    word tmp = i;
    dword tmpCount = 0;

    for (dword j = 0; j < 8; j++)
     {
      if ((tmp & 3) == 3 || !(tmp & 3))
	tmpCount++;
      tmp >>= 2;
     }

    if (tmpCount == 8)
      NShRedundencyCount++;
   }

  printf ("Not shifted RedundencyCount = %d.\n", NShRedundencyCount);

  // Count phase shifted codes
  for (i = 0; i < 0x10000; i++)
   {
    word tmp = i >> 1;
    dword tmpCount = 0;

    for (int j = 0; j < 7; j++)
     {
      if ((tmp & 3) == 3 || !(tmp & 3))
	tmpCount++;
      tmp >>= 2;
     }

    if (tmpCount == 7)
      ShRedundencyCount++;
   }

  printf ("Shifted RedundencyCount = %d.\n", ShRedundencyCount);
  printf ("Public RedundencyCount = %d.\n", ShRedundencyCount + NShRedundencyCount);
}

int main (void)
{
  printf ("Galey coder/encoder test program.\n");

  InitGaleyTable ();
  TestGaleyStringCoder ();

  CountPulse ();
  CountRedundencyCode ();
  TestError ();

  byte *data = new byte [SizeOrigCluster  + 1];
  byte *dest = new byte [SizeGaleyCluster + 1];
  strcpy ((char*)data, TEST_DATA);

  time_t start_time = get_time (), end_time;
  while (start_time == get_time ()) {};
  start_time = get_time () + 5000;

  size_t count = 0;
  do
  {
    CodeGaleyCluster (dest, data);
    count++;
  } while ((end_time = get_time ()) < start_time);

  printf ("Send rate: %.6f K/S\n", float (SizeOrigCluster * count) / 5);
  printf ("Facking data: %s\n", dest);

  start_time = get_time ();
  while (start_time == get_time ()) {};
  start_time = get_time () + 5000;

  printf ("\n");
  printf ("|--Cluster--|-----I----|----II----|----III---|--Summary--|\n");

  count = 0;
  do
  {
    estat ES;
    //DecodeGaleyCluster (data, dest);
    DecodeGaleyClusterWithErrStat (data, dest, &ES);
/*
    printf ("| %9d | %8d | %8d | %8d | %9d |\n", count,
	     ES.OrdinaryErr,
	     ES.DupletErr,
	     ES.TripletErr,
	     ES.OrdinaryErr +
	    (ES.DupletErr * 2) +
	    (ES.TripletErr * 3));
*/
    count++;
  } while ((end_time = get_time ()) < start_time);

  printf ("Receive rate: %.6f K/S\n", float (SizeOrigCluster * count) / 5);
  printf ("Restored data: %s\n", data);

  delete data;
  delete dest;
  FreeGaleyTable ();
  return 0;
}
