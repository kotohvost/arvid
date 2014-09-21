#include "compiler.h"
#include <_dpmi.h>

#include "mytypes.h"
#include "dpmiserv.h"
#include "adriver.h"
#include "inline.h"
#include "mtime.h"
#include "galey.h"

dword *ErrorTable   = NULL;
word  *RestoreTable = NULL;
word  *NumBitsTable = NULL;
word  *LastTable    = NULL;

dword RecurCounter = 0, pErrorTable = 0, NumElemErrorTable = 0;
dword NumElemLastTable = 1 << VALUE_LEN, NumElemRestoreTable;
dword NumElemNumBits = 1 << VALUE_LEN;

dword NumOne (dword mask)
{
  dword num = 0;

  for (dword i = 0; i < CODE_LEN; i++)
   {
    num += mask & 1;
    mask >>= 1;
   }

  return num;
}

void RecurProc (dword mask, dword Nested)
{
  dword localmask = 1, addmask, found;
  RecurCounter++;

  for (dword i = 0; i < CODE_LEN; i++)
   {
    addmask = localmask | mask;

    if (NumOne (addmask) == RecurCounter)
     {
      if (RecurCounter == Nested)
       {
	found = 0;

	for (dword j = 0; j < NumElemErrorTable; j++)
	  if (ErrorTable [j] == addmask)
	    found = 1;

	if (!found)
	  ErrorTable [pErrorTable++] = addmask;
       }
      else
	RecurProc (addmask, Nested);
     }
    localmask <<= 1;
   }

  RecurCounter--;
}

void RecalcErrorTable (void)
{
  dword i;
  int hFerrtabl = -1;

  NumElemErrorTable += CODE_LEN/1;
  NumElemErrorTable += ((CODE_LEN - 1) * (CODE_LEN))/2;
  NumElemErrorTable += ((CODE_LEN - 2) * (CODE_LEN - 1) * (CODE_LEN))/6;
  // NumElemErrorTable = 11155 + 1; //without checking double
  //printf ("-NumElemErrorTable = %d\n", NumElemErrorTable);

  if (!(ErrorTable = new dword [NumElemErrorTable]))
    ErrorExit(1);

  LockMemory (ErrorTable, sizeof (dword) * NumElemErrorTable, LOCK_MEMORY);

  if ((hFerrtabl = open (ErrorName, O_BINARY | O_RDWR, FILE_ACCESS)) ==-1)
   {
    memset (ErrorTable, 0, NumElemErrorTable * sizeof(dword));

    for (i = 1; i <= MAX_RESTORE_ERROR; i++)
      RecurProc (0, i);
    //printf ("-pErrorTable = %d\n", pErrorTable);

    hFerrtabl = open (ErrorName, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);
    write (hFerrtabl, ErrorTable, NumElemErrorTable * sizeof (dword));
   }
  else
    read (hFerrtabl, ErrorTable, NumElemErrorTable * sizeof (dword));

  close (hFerrtabl);
}

#define G1 0x0C75
#define G2 0x0AE3
#define PolynumLen 12

void RecalcLastTable (void)
{
  dword DataWord, Polynum, HighBitMask;
  int hLasttabl = -1;

  if (!(LastTable = new word [NumElemLastTable]))
    ErrorExit (1);

  LockMemory (LastTable, sizeof (word) * NumElemLastTable, LOCK_MEMORY);
  memset (LastTable, 0, NumElemLastTable * sizeof (word));

  if ((hLasttabl = open (LastName, O_BINARY | O_RDWR, FILE_ACCESS)) ==-1)
   {
    for (dword i = 0; i < NumElemLastTable; i++)
     {
      DataWord	  = i << LAST_LEN;
      Polynum	  = G1 << (CODE_LEN - PolynumLen);
      HighBitMask = 1 << (CODE_LEN - 1);

      //Evklid division
      for (dword j = 0; j < PolynumLen; j++)
       {
	if (HighBitMask & DataWord)
	  DataWord ^= Polynum;

	HighBitMask >>= 1;
	Polynum >>= 1;
       }

      LastTable [i] = (word)DataWord;
     }

    hLasttabl = open (LastName, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);

    if (hLasttabl != -1)
      write (hLasttabl, LastTable, NumElemLastTable * sizeof (word));
   }
  else
    read (hLasttabl, LastTable, NumElemLastTable * sizeof (word));

  close (hLasttabl);
}

void RecalcRestoreTable (dword *ErrorTable)
{
  dword ErrorTemplate, Polynum, HighBitMask;
  int hRestoretabl;

  NumElemRestoreTable = NumElemErrorTable + 1;

  if (!(RestoreTable = new word [NumElemRestoreTable]))
    ErrorExit (1);

  LockMemory (RestoreTable, sizeof (word) * NumElemRestoreTable, LOCK_MEMORY);
  memset(RestoreTable, 0, NumElemRestoreTable * sizeof (word));

  if ((hRestoretabl = open (RestName, O_BINARY | O_RDWR, FILE_ACCESS)) == -1)
   {
    for (dword i = 0; i < NumElemErrorTable; i++)
     {
      ErrorTemplate = ErrorTable[i];
      Polynum	    = G1 << (CODE_LEN - PolynumLen);
      HighBitMask   = 1 << (CODE_LEN - 1);

      for (dword j = 0; j < PolynumLen; j++)
       {
	if (HighBitMask & ErrorTemplate)
	  ErrorTemplate ^= Polynum;

	HighBitMask >>= 1;
	Polynum >>= 1;
       }

      RestoreTable[ErrorTemplate] = ErrorTable[i] >> LAST_LEN;
     }

    hRestoretabl = open (RestName, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);
    write (hRestoretabl, RestoreTable, NumElemRestoreTable * sizeof (word));
   }
  else
    read (hRestoretabl, RestoreTable, NumElemRestoreTable * sizeof (word));

  close (hRestoretabl);
}

void RecalcNumBitsTable (void)
{
  int hNumBits;

  if (!(NumBitsTable = new word [NumElemNumBits]))
    ErrorExit (1);

  LockMemory (NumBitsTable, sizeof (word) * NumElemNumBits, LOCK_MEMORY);
  memset (NumBitsTable, 0, NumElemNumBits * sizeof (word));

  if ((hNumBits = open (NumBitsName, O_BINARY | O_RDWR, FILE_ACCESS)) == -1)
   {
    for (dword i = 0; i < NumElemNumBits; i++)
     {
      word Counter = 0;
      dword Mask = 1;

      for (dword j = 0; j < VALUE_LEN; j++)
       {
	Counter += (i & Mask) >> j;
	Mask <<= 1;
       }

      NumBitsTable [i] = Counter;
     }

    hNumBits = open (NumBitsName, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);
    write (hNumBits, NumBitsTable, NumElemNumBits * sizeof (word));
   }
  else
    read (hNumBits, NumBitsTable, NumElemNumBits * sizeof (word));

  close (hNumBits);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~End_recalce_table~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __GNUC__
dword gcoder (word Data, word *LastTable);
#pragma aux gcoder parm [ax][ecx]=\
"movzx eax,ax"			  \
"movzx ebx,word ptr [ecx][eax*2]" \
"shl   eax,23-12"		  \
"or    eax,ebx" 		  \
modify [ebx]\
value  [eax];
#endif // __GNUC__

static inline dword GaleyCoder (word Data)
{
//#ifdef __GNUC__
  return ((dword)Data << LAST_LEN) | LastTable [Data];
//#else
//  return gcoder (Data, LastTable);
//#endif
}

dword eciGaleyCoder (word Data) { return GaleyCoder (Data); }

#ifndef __GNUC__
word gdecoder (dword Crypt, word *LastTable, word *RestoreTable);
#pragma aux gdecoder parm [eax][esi][edi]=\
"mov ebx,eax"				  \
"and eax,0x7FF" 			  \
"shr ebx,23-12" 			  \
"mov edx,ebx"				  \
"movzx ebx,word ptr [esi][ebx*2]"	  \
"xor ebx,eax"				  \
"xor dx,[edi][ebx*2]"			  \
modify [eax ebx edx]			  \
value  [dx];
#endif // __GNUC__

static inline word GaleyDecoder (dword Code)
{
//#ifdef __GNUC__
  word Q = Code >> LAST_LEN; // Q(x)
  return Q ^= RestoreTable [LastTable [Q] ^ (Code & 0x7FF)]; //choose e(x) from s(x)
//#else
//  return gdecoder (Crypt, LastTable, RestoreTable);
//#endif
}

word eciGaleyDecoder (dword Code) { return GaleyDecoder (Code); }

word GDecoderWithErrStat (dword Code, word *LastTable, word *RestoreTable, word *NumBitsTable, dword &ErrCounter);
#pragma aux GDecoderWithErrStat parm [eax][esi][edi][ebx][ecx]=\
"push  eax"			  \
"push  ebx"			  \
"mov   ebx,eax" 		  \
"and   eax,0x7FF"		  \
"shr   ebx,23-12"		  \
"mov   edx,ebx" 		  \
"movzx ebx,word ptr [esi][ebx*2]" \
"xor   ebx,eax" 		  \
"xor   dx,[edi][ebx*2]" 	  \
"movzx ebx,dx"			  \
"movzx eax,word ptr [esi][ebx*2]" \
"shl   ebx,23-12"		  \
"or    ebx,eax" 		  \
"pop   eax"			  \
"xor   eax,ebx" 		  \
"mov   ebx,eax" 		  \
"and   eax,0x7FF"		  \
"shr   ebx,23-12"		  \
"movzx edi,word ptr [ebp][ebx*2]" \
"mov   ebx,eax" 		  \
"add   di, word ptr [ebp][ebx*2]" \
"mov   [ecx],edi"		  \
modify [eax ebx edx edi]	  \
value  [dx];

static inline word GaleyDecoderWithErrStat (dword Code, dword &ErrCounter)
{
//#ifdef __GNUC__
  word Q = Code >> LAST_LEN;
  dword I = Q ^= RestoreTable [LastTable [Q] ^ (Code & 0x7FF)]; //choose e(x) from s(x)

  I = (I << LAST_LEN) | LastTable [I];
  I ^= Code;
  ErrCounter = NumBitsTable [I & 0x7FF];
  ErrCounter += NumBitsTable [I >> LAST_LEN];
  return Q;
//#else
//  return GDecoderWithErrStat (Code, LastTable, RestoreTable, ErrCounter);
//#endif
}

word eciGaleyDecoderWithErrStat (dword Code, dword &ErrCounter)
{ return GaleyDecoderWithErrStat (Code, ErrCounter); }

static inline void Fill23For1031 (byte *dest, dword shift, dword gcode)
{
#undef DO_BIT
#define DO_BIT(x)						       \
  if (shift >= x)						       \
    dest [SIZE_DATA_FRAME_1031 * x] |= (gcode & (1 << x)) << shift - x;\
  else								       \
    dest [SIZE_DATA_FRAME_1031 * x] |= (gcode & (1 << x)) >> x - shift;

  DO_BIT (0);
  DO_BIT (1);
  DO_BIT (2);
  DO_BIT (3);
  DO_BIT (4);
  DO_BIT (5);
  DO_BIT (6);
  DO_BIT (7);
  DO_BIT (8);
  DO_BIT (9);
  DO_BIT (10);
  DO_BIT (11);
  DO_BIT (12);
  DO_BIT (13);
  DO_BIT (14);
  DO_BIT (15);
  DO_BIT (16);
  DO_BIT (17);
  DO_BIT (18);
  DO_BIT (19);
  DO_BIT (20);
  DO_BIT (21);
  DO_BIT (22);
}

static inline void Fill23For105X (byte *dest, dword shift, dword gcode)
{
#undef DO_BIT
#define DO_BIT(x)						       \
  if (shift >= x)						       \
    dest [SIZE_DATA_FRAME_105X * x] |= (gcode & (1 << x)) << shift - x;\
  else								       \
    dest [SIZE_DATA_FRAME_105X * x] |= (gcode & (1 << x)) >> x - shift;

  DO_BIT (0);
  DO_BIT (1);
  DO_BIT (2);
  DO_BIT (3);
  DO_BIT (4);
  DO_BIT (5);
  DO_BIT (6);
  DO_BIT (7);
  DO_BIT (8);
  DO_BIT (9);
  DO_BIT (10);
  DO_BIT (11);
  DO_BIT (12);
  DO_BIT (13);
  DO_BIT (14);
  DO_BIT (15);
  DO_BIT (16);
  DO_BIT (17);
  DO_BIT (18);
  DO_BIT (19);
  DO_BIT (20);
  DO_BIT (21);
  DO_BIT (22);
}

void CodeGaleyClusterFor1031 (byte *dest, byte *data)
{
  dword LastBit, LastLevel;

  imemset (dest, 0, SIZE_DATA_FRAME_1031 * 23);
  for (dword row = 0; row < HDATAFRAME_1031; row++)
  {
    LastBit   = 1; //white first line
    LastLevel = 1; //white first line

    for (dword col = 0; col < WDATAFRAME_1031; col++)
    {
      Fill23For1031 (dest, 0, GaleyCoder ( *((word*) &data [0]) & 0xfff));
      Fill23For1031 (dest, 1, GaleyCoder ( *((word*) &data [1]) >> 4));

      Fill23For1031 (dest, 2, GaleyCoder ( *((word*) &data [3]) & 0xfff));
      Fill23For1031 (dest, 3, GaleyCoder ( *((word*) &data [4]) >> 4));

      Fill23For1031 (dest, 4, GaleyCoder ( *((word*) &data [6]) & 0xfff));
      Fill23For1031 (dest, 5, GaleyCoder ( *((word*) &data [7]) >> 4));

      Fill23For1031 (dest, 6, GaleyCoder ( *((word*) &data [9]) & 0xfff));
      Fill23For1031 (dest, 7, GaleyCoder ( *((word*) &data [10]) >> 4));

      data += 12;
      dest++;
    } /* endfor */
  } /* endfor */
}

void CodeGaleyClusterFor105X (byte *dest, byte *data)
{
  dword LastBit, LastLevel;

  imemset (dest, 0, SIZE_DATA_FRAME_105X * 23);
  for (dword row = 0; row < HDATAFRAME_105X; row++)
  {
    LastBit   = 1; //white first line
    LastLevel = 1; //white first line

    for (dword col = 0; col < WDATAFRAME_105X; col++)
    {
      Fill23For105X (dest, 0, GaleyCoder ( *((word*) &data [0]) & 0xfff));
      Fill23For105X (dest, 1, GaleyCoder ( *((word*) &data [1]) >> 4));

      Fill23For105X (dest, 2, GaleyCoder ( *((word*) &data [3]) & 0xfff));
      Fill23For105X (dest, 3, GaleyCoder ( *((word*) &data [4]) >> 4));

      Fill23For105X (dest, 4, GaleyCoder ( *((word*) &data [6]) & 0xfff));
      Fill23For105X (dest, 5, GaleyCoder ( *((word*) &data [7]) >> 4));

      Fill23For105X (dest, 6, GaleyCoder ( *((word*) &data [9]) & 0xfff));
      Fill23For105X (dest, 7, GaleyCoder ( *((word*) &data [10]) >> 4));

      data += 12;
      dest++;
    } /* endfor */
  } /* endfor */
}

static inline dword Get23For1031 (byte *source, dword shift)
{
  dword ret = 0;

#undef DO_BIT
#define DO_BIT(x)							\
  if (shift >= x)							\
    ret |= (source [SIZE_DATA_FRAME_1031 * x] & (1 << shift)) >> shift - x;\
  else									\
    ret |= (source [SIZE_DATA_FRAME_1031 * x] & (1 << shift)) << x - shift;

  DO_BIT (0);
  DO_BIT (1);
  DO_BIT (2);
  DO_BIT (3);
  DO_BIT (4);
  DO_BIT (5);
  DO_BIT (6);
  DO_BIT (7);
  DO_BIT (8);
  DO_BIT (9);
  DO_BIT (10);
  DO_BIT (11);
  DO_BIT (12);
  DO_BIT (13);
  DO_BIT (14);
  DO_BIT (15);
  DO_BIT (16);
  DO_BIT (17);
  DO_BIT (18);
  DO_BIT (19);
  DO_BIT (20);
  DO_BIT (21);
  DO_BIT (22);

  return ret;
}

static inline dword Get23For105X (byte *source, dword shift)
{
  dword ret = 0;

#undef DO_BIT
#define DO_BIT(x)							\
  if (shift >= x)							\
    ret |= (source [SIZE_DATA_FRAME_105X * x] & (1 << shift)) >> shift - x;\
  else									\
    ret |= (source [SIZE_DATA_FRAME_105X * x] & (1 << shift)) << x - shift;

  DO_BIT (0);
  DO_BIT (1);
  DO_BIT (2);
  DO_BIT (3);
  DO_BIT (4);
  DO_BIT (5);
  DO_BIT (6);
  DO_BIT (7);
  DO_BIT (8);
  DO_BIT (9);
  DO_BIT (10);
  DO_BIT (11);
  DO_BIT (12);
  DO_BIT (13);
  DO_BIT (14);
  DO_BIT (15);
  DO_BIT (16);
  DO_BIT (17);
  DO_BIT (18);
  DO_BIT (19);
  DO_BIT (20);
  DO_BIT (21);
  DO_BIT (22);

  return ret;
}

void DecodeGaleyClusterFor1031 (byte *dest, byte *data)
{
  for (dword row = 0; row < HDATAFRAME_1031; row++)
  {
    for (dword col = 0; col < WDATAFRAME_1031; col++)
    {
      dword s1, s2;

      s1 = GaleyDecoder (Get23For1031 (data, 0));
      s2 = GaleyDecoder (Get23For1031 (data, 1));
      *(dword*)&dest [0] = s1 | (s2 << VALUE_LEN);

      s1 = GaleyDecoder (Get23For1031 (data, 2));
      s2 = GaleyDecoder (Get23For1031 (data, 3));
      *(dword*)&dest [3] = s1 | (s2 << VALUE_LEN);

      s1 = GaleyDecoder (Get23For1031 (data, 4));
      s2 = GaleyDecoder (Get23For1031 (data, 5));
      *(dword*)&dest [6] = s1 | (s2 << VALUE_LEN);

      s1 = GaleyDecoder (Get23For1031 (data, 6));
      s2 = GaleyDecoder (Get23For1031 (data, 7));
      *(dword*)&dest [9] = s1 | (s2 << VALUE_LEN);

      dest += VALUE_LEN;
      data++;
    } /* endfor */
  } /* endfor */
}

void DecodeGaleyClusterFor105X (byte *dest, byte *data)
{
  for (dword row = 0; row < HDATAFRAME_105X; row++)
  {
    for (dword col = 0; col < WDATAFRAME_105X; col++)
    {
      dword s1, s2;

      s1 = GaleyDecoder (Get23For105X (data, 0));
      s2 = GaleyDecoder (Get23For105X (data, 1));
      *(dword*)&dest [0] = s1 | (s2 << VALUE_LEN);

      s1 = GaleyDecoder (Get23For105X (data, 2));
      s2 = GaleyDecoder (Get23For105X (data, 3));
      *(dword*)&dest [3] = s1 | (s2 << VALUE_LEN);

      s1 = GaleyDecoder (Get23For105X (data, 4));
      s2 = GaleyDecoder (Get23For105X (data, 5));
      *(dword*)&dest [6] = s1 | (s2 << VALUE_LEN);

      s1 = GaleyDecoder (Get23For105X (data, 6));
      s2 = GaleyDecoder (Get23For105X (data, 7));
      *(dword*)&dest [9] = s1 | (s2 << VALUE_LEN);

      dest += VALUE_LEN;
      data++;
    } /* endfor */
  } /* endfor */
}

static inline void UpdateErrStat (dword &ErrCounter, estat *ErrStat)
{
  switch (ErrCounter)
   {
    case 0: return;
    case 1: ErrStat->OrdinaryErr++; return;
    case 2: ErrStat->DupletErr++;   return;
    case 3: ErrStat->TripletErr++;  return;
   }
}

void DecodeGaleyClusterWithErrStatFor1031 (byte *dest, byte *data, estat *pES)
{
  imemset (pES, 0, sizeof (estat) - sizeof (dword));
  for (dword row = 0; row < HDATAFRAME_1031; row++)
  {
    for (dword col = 0; col < WDATAFRAME_1031; col++)
    {
      dword s1, s2, ErrCounter;

      s1 = GaleyDecoderWithErrStat (Get23For1031 (data, 0), ErrCounter);
      UpdateErrStat (ErrCounter, pES);
      s2 = GaleyDecoderWithErrStat (Get23For1031 (data, 1), ErrCounter);
      *(dword*)&dest [0] = s1 | (s2 << VALUE_LEN);
      UpdateErrStat (ErrCounter, pES);

      s1 = GaleyDecoderWithErrStat (Get23For1031 (data, 2), ErrCounter);
      UpdateErrStat (ErrCounter, pES);
      s2 = GaleyDecoderWithErrStat (Get23For1031 (data, 3), ErrCounter);
      *(dword*)&dest [3] = s1 | (s2 << VALUE_LEN);
      UpdateErrStat (ErrCounter, pES);

      s1 = GaleyDecoderWithErrStat (Get23For1031 (data, 4), ErrCounter);
      UpdateErrStat (ErrCounter, pES);
      s2 = GaleyDecoderWithErrStat (Get23For1031 (data, 5), ErrCounter);
      *(dword*)&dest [6] = s1 | (s2 << VALUE_LEN);
      UpdateErrStat (ErrCounter, pES);

      s1 = GaleyDecoderWithErrStat (Get23For1031 (data, 6), ErrCounter);
      UpdateErrStat (ErrCounter, pES);
      s2 = GaleyDecoderWithErrStat (Get23For1031 (data, 7), ErrCounter);
      *(dword*)&dest [9] = s1 | (s2 << VALUE_LEN);
      UpdateErrStat (ErrCounter, pES);

      dest += VALUE_LEN;
      data++;
    } /* endfor */
  } /* endfor */
}

void DecodeGaleyClusterWithErrStatFor105X (byte *dest, byte *data, estat *pES)
{
  imemset (pES, 0, sizeof (estat) - sizeof (dword));
  for (dword row = 0; row < HDATAFRAME_105X; row++)
  {
    for (dword col = 0; col < WDATAFRAME_105X; col++)
    {
      dword s1, s2, ErrCounter;

      s1 = GaleyDecoderWithErrStat (Get23For105X (data, 0), ErrCounter);
      UpdateErrStat (ErrCounter, pES);
      s2 = GaleyDecoderWithErrStat (Get23For105X (data, 1), ErrCounter);
      *(dword*)&dest [0] = s1 | (s2 << VALUE_LEN);
      UpdateErrStat (ErrCounter, pES);

      s1 = GaleyDecoderWithErrStat (Get23For105X (data, 2), ErrCounter);
      UpdateErrStat (ErrCounter, pES);
      s2 = GaleyDecoderWithErrStat (Get23For105X (data, 3), ErrCounter);
      *(dword*)&dest [3] = s1 | (s2 << VALUE_LEN);
      UpdateErrStat (ErrCounter, pES);

      s1 = GaleyDecoderWithErrStat (Get23For105X (data, 4), ErrCounter);
      UpdateErrStat (ErrCounter, pES);
      s2 = GaleyDecoderWithErrStat (Get23For105X (data, 5), ErrCounter);
      *(dword*)&dest [6] = s1 | (s2 << VALUE_LEN);
      UpdateErrStat (ErrCounter, pES);

      s1 = GaleyDecoderWithErrStat (Get23For105X (data, 6), ErrCounter);
      UpdateErrStat (ErrCounter, pES);
      s2 = GaleyDecoderWithErrStat (Get23For105X (data, 7), ErrCounter);
      *(dword*)&dest [9] = s1 | (s2 << VALUE_LEN);
      UpdateErrStat (ErrCounter, pES);

      dest += VALUE_LEN;
      data++;
    } /* endfor */
  } /* endfor */
}

void CodeGaleyCluster (byte *data, byte *dest)
{
  if (HDensity)
    CodeGaleyClusterFor105X (data, dest);
  else
    CodeGaleyClusterFor1031 (data, dest);
}

void DecodeGaleyCluster (byte *data, byte *dest)
{
  if (HDensity)
    DecodeGaleyClusterFor105X (data, dest);
  else
    DecodeGaleyClusterFor1031 (data, dest);
}

void DecodeGaleyClusterWithErrStat (byte *data, byte *dest, estat *ErrStat)
{
  if (HDensity)
    DecodeGaleyClusterWithErrStatFor105X (data, dest, ErrStat);
  else
    DecodeGaleyClusterWithErrStatFor1031 (data, dest, ErrStat);
}

void BeginGALEYLockedCode () {}

// Len string = (NumTriplets * 3 * 2) + 1
void GaleyStringCoder (byte *String, byte *Date, int NumTriplets)
{
  for (int i = 0; i < NumTriplets; i++)
   {
    word Word = *Date | ((word)(Date [1] & 0xF) << 8);
    dword *pDString = (dword*)String;
    *pDString = GaleyCoder (Word);

    String +=3;
    pDString = (dword*)String;
    Word = Date [2] | ((word)(Date [1] & 0xF0) << 4);
    *pDString = GaleyCoder (Word);
    String +=3;
    Date +=3;
   }
}

void GaleyStringDecoder (byte *Date, byte *String, int NumTriplets)
{
  for (int i = 0; i < NumTriplets; i++)
   {
    dword Dword = *(dword*)String & 0x7FFFFF;
    word Word = GaleyDecoder (Dword);
    *Date = Word;
    Date [1] = (Word >> 8) & 0xF;

    String +=3;
    Dword = *(dword*)String & 0x7FFFFF;
    Word = GaleyDecoder (Dword);
    Date [2] = Word;
    Date [1] |= (Word >> 4) & 0xF0;
    String +=3;
    Date +=3;
   }
}

  byte MixerBuff [13 * 2 + 1];

void StringMixer (byte *String, dword Len)
{
  dword NumTriplet = Len / 3, Mask = 1, i, j, tmp, shift;
  dword StepByte = NumTriplet >> 1; // * numbitperoutelement / bytelen
  imemset (MixerBuff, 0, sizeof (MixerBuff));

  for (i = 0; i < CODE_LEN; i++, Mask <<= 1)
   {
    for (j = 0; j < NumTriplet; j++)
     {
      // get triplet's bit from string
      tmp = *((dword*)(&String [j * 3])) & Mask;

      // Shift correction
      shift = (i & 3) * NumTriplet + j;

      if (shift > i)
	tmp <<= shift - i;
      else
	tmp >>= i - shift;

      *((dword*)&MixerBuff [StepByte * (i >> 2)]) |= tmp;
     }
   }

  imemcpy (String, MixerBuff, NumTriplet * 3);
}

void StringDemixer (byte *String, dword Len)
{
  dword NumTriplet = Len / 3, Mask = 1, i, j, tmp, shift;
  dword StepByte = NumTriplet >> 1; // * numbitperoutelement / bytelen
  imemset (MixerBuff, 0, sizeof (MixerBuff));

  for (i = 0; i < CODE_LEN; i++, Mask <<= 1)
   {
    for (j = 0; j < NumTriplet; j++)
     {
      tmp = *((dword*)&String [StepByte * (i >> 2)]);

      // Shift correction
      shift = (i & 3) * NumTriplet + j;

      if (shift > i)
	tmp >>= shift - i;
      else
	tmp <<= i - shift;

      *((dword*)(&MixerBuff [j * 3])) |= tmp & Mask;
     }
   }

  imemcpy (String, MixerBuff, NumTriplet * 3);
}
void EndGALEYLockedCode () {}

void InitGaleyTable (void)
{
  RecalcErrorTable ();
  RecalcLastTable ();
  RecalcRestoreTable (ErrorTable);
  RecalcNumBitsTable ();
}

void FreeGaleyTable (void)
{
  delete ErrorTable;
  delete LastTable;
  delete RestoreTable;
  delete NumBitsTable;

  LockMemory (ErrorTable, sizeof (dword) * NumElemErrorTable, UNLOCK_MEMORY);
  LockMemory (LastTable, sizeof (word) * NumElemLastTable, UNLOCK_MEMORY);
  LockMemory (RestoreTable, sizeof (word) * NumElemRestoreTable, UNLOCK_MEMORY);
  LockMemory (NumBitsTable, sizeof (word) * NumElemNumBits, UNLOCK_MEMORY);
}
