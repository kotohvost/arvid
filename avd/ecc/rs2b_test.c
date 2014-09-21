
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
#include "rs2b.h"
#include "rs2.h"

#define ErrorName "tables/errstabl.bin"
#define TestName  "tables/testtabl.bin"

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

static rs2w *ErrorTable = NULL, *TestTable = NULL;
static dword NumElemErrorTable = 0, RecurCounter = 0, pErrorTable = 0;

static dword NumOne (rs2w *mask)
{
  dword i, num = 0;

  for (i = 0; i < RS2_CODE_LEN; i++)
    if (mask->rs [i])
      num++;

  return num;
}

static bool CompareMask (rs2w *src, rs2w *dst)
{
  dword i;
  
  for (i = 0; i < RS2_CODE_LEN; i++)
    if (src->rs [i] != dst->rs [i])
      return false;

  return true;
}

static void RecurProc (rs2w *mask, dword Nested)
{
  /* dword localmask = 1, addmask; */
  dword i, j, index = 0;
  rs2w loc, add;
  
  for (i = 0; i < RS2_CODE_LEN; i++)
    loc.rs [i] = add.rs [i] = 0;

  loc.rs [index] = 0xf;
  RecurCounter++;

  for (i = 0; i < RS2_CODE_LEN; i++)
   {
    /* addmask = localmask | mask; */
    for (j = 0; j < RS2_CODE_LEN; j++)
      add.rs [j] = loc.rs [j] | mask->rs [j];

    if (NumOne (&add) == RecurCounter)
     {
      if (RecurCounter == Nested)
       {
	bool found = false;

	for (j = 0; j < NumElemErrorTable; j++)
	  /* if (ErrorTable [j] == addmask) */
	  if (CompareMask (&ErrorTable [j], &add))
	    found = true;

	if (!found)
	 {
	  /* ErrorTable [pErrorTable++] = addmask; */
	  for (j = 0; j < RS2_CODE_LEN; j++)
	    ErrorTable [pErrorTable].rs [j] = add.rs [j];
	    
	  pErrorTable++;
	 }
       }
      else
	RecurProc (&add, Nested);
     }

    /* localmask <<= 1; */
    loc.rs [index++] = 0;
    loc.rs [index] = 0xf;
   }

  RecurCounter--;
}

static void RecalcErrorTable (void)
{
  dword i, j;
  int hFerrtabl = -1;

  NumElemErrorTable += RS2_CODE_LEN/1;
  NumElemErrorTable += ((RS2_CODE_LEN - 1) * (RS2_CODE_LEN))/2;
//  NumElemErrorTable += ((RS2_CODE_LEN - 2) * (RS2_CODE_LEN - 1) * (RS2_CODE_LEN))/6;
  /* NumElemErrorTable = 11155 + 1; */ /* without checking double */
  printf ("-NumElemErrorTable = %d\n", NumElemErrorTable);

  if (!(ErrorTable = malloc (sizeof (rs2w) * NumElemErrorTable)) )
    ErrorExit(1);

  if ((hFerrtabl = open (ErrorName, O_BINARY | O_RDWR, FILE_ACCESS)) == -1)
   {
    memset (ErrorTable, 0, NumElemErrorTable * sizeof (rs2w));

    for (i = 1; i <= RS2_MAX_RESTORE_ERROR; i++)
     {
      rs2w loc;
      
      for (j = 0; j < RS2_CODE_LEN; j++)
	loc.rs [j] = 0;

      RecurProc (&loc, i);
     }
     
    printf ("-pErrorTable = %d\n", pErrorTable);

    hFerrtabl = open (ErrorName, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);
    write (hFerrtabl, ErrorTable, NumElemErrorTable * sizeof (rs2w));
   }
  else
    read (hFerrtabl, ErrorTable, NumElemErrorTable * sizeof (rs2w));

  close (hFerrtabl);
}

static void TestError (void)
{
  dword i, j, k, EtalonCRC, CRC, Comb, GlobErrCounter = 0;
  int hTesttabl;

  if (!(TestTable = malloc (sizeof(rs2w) * NumElemErrorTable)))
    ErrorExit (1);

  printf ("  Please wait. Reed-Solomon code restore test in progres...\r");

  time_t start_time = get_time ();
  while (start_time == get_time ());
  start_time = get_time ();

  char errortempl = 0xf;
  while (errortempl--)
   {
    for (j = 0; j < (1 << RS2_VALUE_LEN); j++)
     {
      memset (TestTable, 0, NumElemErrorTable * sizeof (rs2w));

      /* fill data table */
      for (i = 0; i < NumElemErrorTable; i++)
       {
	dword mask = 1;
	
	for (k = 0; k < KK; k++)
	 {
	  if (j & mask)
	    TestTable [i].rs[k] = errortempl;

	  mask <<= 1;
	 }
       }

      /* EtalonCRC = j * NumElemErrorTable; */
      EtalonCRC = 0;
      for (i = 0; i < NumElemErrorTable; i++)
	for (k = 0; k < KK; k++)
	  EtalonCRC += TestTable [i].rs[k];

      /* printf ("%X|", EtalonCRC); */
      /* fflush (stdout); */

      for (i = 0; i < NumElemErrorTable; i++)
       {
	encode_rs2 (TestTable [i].rs, &TestTable [i].rs [KK]); /* 9 */
	/* application errors on encoded data */
	for (k = 0; k < NN; k++)
	  TestTable [i].rs[k] ^= ErrorTable [i].rs[k];

	/* dword ErrCounter;
	 * TestTable [i] = eciGaleyDecoderWithErrStat (eciGaleyCoder (j) ^ ErrorTable [i], ErrCounter);
	 * TestTable [i] = eciGaleyDecoder (eciGaleyCoder (j) ^ ErrorTable [i]);
	 * GlobErrCounter += ErrCounter;
	 * eras_dec_rs2 (TestTable [i].rs, NULL, 0); */
	eras_dec_rs2_wers (TestTable [i].rs);
       }

      CRC = 0;
      for (i = 0; i < NumElemErrorTable; i++)
	for (k = 0; k < KK; k++)
	  CRC += TestTable [i].rs[k];

      if (EtalonCRC == CRC)
	Progress ();
      else
	{
	 printf ("\nData %X restore badly!!!\n", j);
	 hTesttabl = open (TestName, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);
	 write (hTesttabl, TestTable, NumElemErrorTable * sizeof (rs2w));
	 close (hTesttabl);
	 ErrorExit (2);
	}
     } /* end for */
   } /* end while */

  printf ("\nSuccess!!!\n");
  Comb = NumElemErrorTable * (1 << RS2_VALUE_LEN) * 0xf;
  time_t end_time = get_time ();

  printf ("Restored %d combination, speed rate = %.3f Bps, time = %.3f sec\n",
	  Comb, (float) (Comb) * 4.5 * 1000 / (end_time - start_time),
	  (float) (end_time - start_time) / 1000);
  printf ("ErrorCounter: %d \n", GlobErrCounter);
  free (TestTable);
}

int main (void)
{
  printf ("Reed-Solomon ECC (%d,%d) coder/encoder test program.\n", RS2_CODE_LEN, RS2_VALUE_LEN);

  InitECCTable ();
  RecalcErrorTable ();
  TestError ();
  
  ECCTest (500, 500, RS2_FLAG | ARVID1031_FLAG | CLUSTER_BLOCK);
  ECCTest (500 * RS2_VALUE_LEN, 500 * RS2_VALUE_LEN, RS2_FLAG | ARVID1031_FLAG);
  
  free (ErrorTable);
  FreeECCTable ();
  
  return 0;
}
