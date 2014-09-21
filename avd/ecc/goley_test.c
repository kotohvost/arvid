
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>

#include "defines.h"
#include "dprint.h"
#include "mtime.h"
#include "arvid.h"
#include "ecc.h"
#include "goley.h"

word *TestTable = NULL;

static void ErrorExit (dword NumError)
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

static void TestError (void)
{
  dword i, j, k, EtalonCRC, CRC, Comb, ErrCounter, GlobErrCounter = 0;
  int hTesttabl;

  if (!(TestTable = malloc (sizeof (word) * NumElemErrorTable) ))
    ErrorExit (1);

  memset (TestTable, 0, NumElemErrorTable * sizeof (word));
  printf ("  Please wait. Code Goley restore test in progres...\r");

  time_t start_time = get_time ();
  while (start_time == get_time ());
  start_time = get_time ();

  for (j = 0; j < (1 << G1_VALUE_LEN); j++)
   {
    for (i = 0; i < NumElemErrorTable; i++)
     {
      TestTable [i] = eciGoleyDecoderWithErrStat (eciGoleyCoder (j) ^ ErrorTable[i], &ErrCounter);
      /* TestTable [i] = eciGoleyDecoder (eciGoleyCoder (j) ^ ErrorTable [i]); */
      /* GlobErrCounter += ErrCounter; */
     }

    EtalonCRC = j * NumElemErrorTable;

    for (k = 0, CRC = 0; k < NumElemErrorTable; k++)
      CRC += TestTable [k];

    if (EtalonCRC == CRC)
      Progress ();
      /* printf ("Data %X restore okay.\r", j); */
    else
      {
       printf ("Data %X restore badly!!!		       \n", j);
       hTesttabl = open (TestName, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);
       write (hTesttabl, TestTable, NumElemErrorTable * sizeof (word));
       close (hTesttabl);
       ErrorExit (2);
      }
   }

  printf ("\nSuccess!!!					       \n");
  Comb = NumElemErrorTable * (1 << G1_VALUE_LEN);
  time_t end_time = get_time ();

  printf ("Restored %d combination, speed rate = %.6f combinations/sec\n",
	  Comb, ((float)Comb * 1000) / (end_time - start_time));
  printf ("ErrorCounter: %d \n", GlobErrCounter);
  free (TestTable);
}

static void TestGoleyStringCoder (void)
{
  byte Date [] = TEST_DATA;
  byte String [100];

  printf ("Src date: %s\n", Date);
  GoleyStringCoder (String, Date, sizeof (fr_hdr)/3);
  StringMixer (String, sizeof (fr_hdr) * 2);
  
  printf ("Encode date: ");
  print_data (1, String, sizeof (fr_hdr) * 2);
  
  StringDemixer (String, sizeof (fr_hdr) * 2);
  GoleyStringDecoder (Date, String, sizeof (fr_hdr)/3);
  printf ("Restore date: %s\n", Date);
}

static void CountPulse (void)
{
  dword i, j, DimPulseCounts [16];
  memset (DimPulseCounts, 0, sizeof (DimPulseCounts));

  for (i = 0; i < 0x10000; i++)
   {
    word tmp = i, right_bit = tmp & 1;
    dword PulseCount = 0;

    for (j = 0; j < 15; j++)
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

static void CountRedundencyCode (void)
{
  dword NShRedundencyCount = 0, ShRedundencyCount = 0;
  dword i, j, tmpCount;

  /* Count phase not shifted codes */
  for (i = 0; i < 0x10000; i++)
   {
    word tmp = i;
    tmpCount = 0;

    for (j = 0; j < 8; j++)
     {
      if ((tmp & 3) == 3 || !(tmp & 3))
	tmpCount++;
      tmp >>= 2;
     }

    if (tmpCount == 8)
      NShRedundencyCount++;
   }

  printf ("Not shifted RedundencyCount = %d.\n", NShRedundencyCount);

  /* Count phase shifted codes */
  for (i = 0; i < 0x10000; i++)
   {
    word tmp = i >> 1;
    tmpCount = 0;

    for (j = 0; j < 7; j++)
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
  printf ("Goley ECC (%d,%d) coder/encoder test program.\n", G1_CODE_LEN, G1_VALUE_LEN);

  InitECCTable ();
  TestGoleyStringCoder ();

  CountPulse ();
  CountRedundencyCode ();
  TestError ();

  ECCTest (250, 500, GOLEY_FLAG | ARVID1031_FLAG | CLUSTER_BLOCK);
  ECCTest (G1_VALUE_LEN * 250, G1_VALUE_LEN * 500, GOLEY_FLAG | ARVID1031_FLAG);
  FreeECCTable ();

  return 0;
}
