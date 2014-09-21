
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>

#include "defines.h"
#include "arvid.h"
#include "ecc.h"
#include "goley.h"

dword *ErrorTable   = NULL;
word  *RestoreTable = NULL;
word  *NumBitsTable = NULL;
word  *LastTable    = NULL;

dword RecurCounter = 0, pErrorTable = 0, NumElemErrorTable = 0;
dword NumElemLastTable = 1 << G1_VALUE_LEN, NumElemRestoreTable;
dword NumElemNumBits = 1 << G1_VALUE_LEN;

dword NumOne (dword mask)
{
  dword i, num = 0;

  for (i = 0; i < G1_CODE_LEN; i++)
   {
    num += mask & 1;
    mask >>= 1;
   }

  return num;
}

static void RecurProc (dword mask, dword Nested)
{
  dword i, j, localmask = 1, addmask, found;
  RecurCounter++;

  for (i = 0; i < G1_CODE_LEN; i++)
   {
    addmask = localmask | mask;

    if (NumOne (addmask) == RecurCounter)
     {
      if (RecurCounter == Nested)
       {
	found = 0;

	for (j = 0; j < NumElemErrorTable; j++)
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

static int RecalcErrorTable (void)
{
  dword i;
  int hFerrtabl = -1;

  NumElemErrorTable += G1_CODE_LEN/1;
  NumElemErrorTable += ((G1_CODE_LEN - 1) * (G1_CODE_LEN))/2;
  NumElemErrorTable += ((G1_CODE_LEN - 2) * (G1_CODE_LEN - 1) * (G1_CODE_LEN))/6;
  /* NumElemErrorTable = 11155 + 1; */ /* without checking double */
  /* printf ("-NumElemErrorTable = %d\n", NumElemErrorTable); */

  if (!(ErrorTable = malloc (sizeof(dword) * NumElemErrorTable) ))
    return 0;

  if ((hFerrtabl = open (ErrorName, O_BINARY | O_RDWR, FILE_ACCESS)) == -1)
   {
    memset (ErrorTable, 0, NumElemErrorTable * sizeof(dword));

    for (i = 1; i <= G1_MAX_RESTORE_ERROR; i++)
      RecurProc (0, i);
    /* printf ("-pErrorTable = %d\n", pErrorTable); */

    hFerrtabl = open (ErrorName, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);
    write (hFerrtabl, ErrorTable, NumElemErrorTable * sizeof (dword));
   }
  else
    read (hFerrtabl, ErrorTable, NumElemErrorTable * sizeof (dword));

  close (hFerrtabl);
  return 1;
}

static int RecalcLastTable (void)
{
  dword i, j, DataWord, Polynum, HighBitMask;
  int hLasttabl = -1;

  if (!(LastTable = malloc (sizeof(word) * NumElemLastTable) ))
    return 0;

  memset (LastTable, 0, NumElemLastTable * sizeof (word));

  if ((hLasttabl = open (LastName, O_BINARY | O_RDWR, FILE_ACCESS)) == -1)
   {
    for (i = 0; i < NumElemLastTable; i++)
     {
      DataWord	  = i << G1_LAST_LEN;
      Polynum	  = G1 << (G1_CODE_LEN - G1_POLYNUM_LEN);
      HighBitMask = 1 << (G1_CODE_LEN - 1);

      /* Evclid division */
      for (j = 0; j < G1_POLYNUM_LEN; j++)
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
  return 1;  
}

static int RecalcRestoreTable (dword *ErrorTable)
{
  dword i, j, ErrorTemplate, Polynum, HighBitMask;
  int hRestoretabl;

  NumElemRestoreTable = NumElemErrorTable + 1;

  if (!(RestoreTable = malloc (sizeof(word) * NumElemRestoreTable) ))
    return 0;

  memset(RestoreTable, 0, NumElemRestoreTable * sizeof (word));

  if ((hRestoretabl = open (RestName, O_BINARY | O_RDWR, FILE_ACCESS)) == -1)
   {
    for (i = 0; i < NumElemErrorTable; i++)
     {
      ErrorTemplate = ErrorTable[i];
      Polynum	    = G1 << (G1_CODE_LEN - G1_POLYNUM_LEN);
      HighBitMask   = 1 << (G1_CODE_LEN - 1);

      for (j = 0; j < G1_POLYNUM_LEN; j++)
       {
	if (HighBitMask & ErrorTemplate)
	  ErrorTemplate ^= Polynum;

	HighBitMask >>= 1;
	Polynum >>= 1;
       }

      RestoreTable[ErrorTemplate] = ErrorTable[i] >> G1_LAST_LEN;
     }

    hRestoretabl = open (RestName, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);
    write (hRestoretabl, RestoreTable, NumElemRestoreTable * sizeof (word));
   }
  else
    read (hRestoretabl, RestoreTable, NumElemRestoreTable * sizeof (word));

  close (hRestoretabl);
  return 1;  
}

static int RecalcNumBitsTable (void)
{
  dword i, j; 
  int hNumBits;

  if (!(NumBitsTable = malloc (sizeof(word) * NumElemNumBits) ))
    return 0;

  memset (NumBitsTable, 0, NumElemNumBits * sizeof (word));

  if ((hNumBits = open (NumBitsName, O_BINARY | O_RDWR, FILE_ACCESS)) == -1)
   {
    for (i = 0; i < NumElemNumBits; i++)
     {
      word Counter = 0;
      dword Mask = 1;

      for (j = 0; j < G1_VALUE_LEN; j++)
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
  return 1;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~End_recalce_table~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifndef __GNUC__
dword gcoder (word Data, word *LastTable);
#pragma aux gcoder parm [ax][ecx]=\
"movzx eax,ax"			  \
"movzx ebx,word ptr [ecx][eax*2]" \
"shl   eax,23-12"		  \
"or    eax,ebx" 		  \
modify [ebx]\
value  [eax];
#endif /* __GNUC__ */

static inline dword GoleyCoder (word Data)
{
/* #ifdef __GNUC__ */
  return ((dword)Data << G1_LAST_LEN) | LastTable [Data];
/* #else */
/*   return gcoder (Data, LastTable); */
/* #endif */
}

dword eciGoleyCoder (word Data) { return GoleyCoder (Data); }

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
#endif /* __GNUC__ */

static inline word GoleyDecoder (dword Code)
{
/* #ifdef __GNUC__ */
  word Q = Code >> G1_LAST_LEN; /* Q(x) */
  return Q ^= RestoreTable [LastTable [Q] ^ (Code & 0x7FF)]; /* choose e(x) from s(x) */
/* #else */
/*   return gdecoder (Crypt, LastTable, RestoreTable); */
/* #endif */
}

word eciGoleyDecoder (dword Code) { return GoleyDecoder (Code); }

#ifndef __GNUC__
word GDecoderWithErrStat (dword Code, word *LastTable, word *RestoreTable, word *NumBitsTable, dword *ErrCounter);
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
#endif

static inline word GoleyDecoderWithErrStat (dword Code, dword *ErrCounter)
{
/* #ifdef __GNUC__ */
  word Q = Code >> G1_LAST_LEN;
  dword I = Q ^= RestoreTable [LastTable [Q] ^ (Code & 0x7FF)]; /* choose e(x) from s(x) */

  I = (I << G1_LAST_LEN) | LastTable [I];
  I ^= Code;
  *ErrCounter = NumBitsTable [I & 0x7FF];
  *ErrCounter += NumBitsTable [I >> G1_LAST_LEN];
  return Q;
/* #else */
/*   return GDecoderWithErrStat (Code, LastTable, RestoreTable, ErrCounter); */
/* #endif */
}

word eciGoleyDecoderWithErrStat (dword Code, dword *ErrCounter)
{ return GoleyDecoderWithErrStat (Code, ErrCounter); }

/* fill 23 bits consist in low 23 bits of gcode
   fill one bit of byte, because needs 8 iterations for full byte,
   shift - destination bit number
   dest  - destination buffer */
static inline void Fill23For1031 (byte *dest, dword shift, dword gcode)
{
/* make offset from dword to byte */
#undef DO_BIT
#define DO_BIT(x)						         \
  if (shift >= x)                                                        \
    dest [SIZE_DATA_FRAME_1031 * x] |= (gcode & (1 << x)) << (shift - x);\
  else                                                                   \
    dest [SIZE_DATA_FRAME_1031 * x] |= (gcode & (1 << x)) >> (x - shift);

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

static void CodeGoleyClusterFor1031 (byte *dest, byte *data)
{
  dword i;
  memset (dest, 0, SIZE_DATA_FRAME_1031 * G1_CODE_LEN);

  /* code 12 source bytes SIZE_DATA_FRAME_1031 times */
  for (i = 0; i < SIZE_DATA_FRAME_1031; i++)
   {
    Fill23For1031 (dest, 0, GoleyCoder ( *((word*) &data [0]) & 0xfff));
    Fill23For1031 (dest, 1, GoleyCoder ( *((word*) &data [1]) >> 4));

    Fill23For1031 (dest, 2, GoleyCoder ( *((word*) &data [3]) & 0xfff));
    Fill23For1031 (dest, 3, GoleyCoder ( *((word*) &data [4]) >> 4));

    Fill23For1031 (dest, 4, GoleyCoder ( *((word*) &data [6]) & 0xfff));
    Fill23For1031 (dest, 5, GoleyCoder ( *((word*) &data [7]) >> 4));

    Fill23For1031 (dest, 6, GoleyCoder ( *((word*) &data [9]) & 0xfff));
    Fill23For1031 (dest, 7, GoleyCoder ( *((word*) &data [10]) >> 4));

    data += 12;
    dest++;
   }
}

/* fill 23 bits consist in low 23 bits of gcode
   fill one bit of byte, because needs 8 operations for full byte,
   shift - destination bit number
   dest  - destination buffer */
static inline void Fill23For105X (byte *dest, dword shift, dword gcode)
{
/* make offset from dword to byte */
#undef DO_BIT
#define DO_BIT(x)						         \
  if (shift >= x)                                                        \
    dest [SIZE_DATA_FRAME_105X * x] |= (gcode & (1 << x)) << (shift - x);\
  else                                                                   \
    dest [SIZE_DATA_FRAME_105X * x] |= (gcode & (1 << x)) >> (x - shift);

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

static void CodeGoleyClusterFor105X (byte *dest, byte *data)
{
  dword i;
  memset (dest, 0, SIZE_DATA_FRAME_105X * G1_CODE_LEN);

  /* code 12 source bytes SIZE_DATA_FRAME_105X times */
  for (i = 0; i < SIZE_DATA_FRAME_105X; i++)
   {
    Fill23For105X (dest, 0, GoleyCoder ( *((word*) &data [0]) & 0xfff));
    Fill23For105X (dest, 1, GoleyCoder ( *((word*) &data [1]) >> 4));

    Fill23For105X (dest, 2, GoleyCoder ( *((word*) &data [3]) & 0xfff));
    Fill23For105X (dest, 3, GoleyCoder ( *((word*) &data [4]) >> 4));

    Fill23For105X (dest, 4, GoleyCoder ( *((word*) &data [6]) & 0xfff));
    Fill23For105X (dest, 5, GoleyCoder ( *((word*) &data [7]) >> 4));

    Fill23For105X (dest, 6, GoleyCoder ( *((word*) &data [9]) & 0xfff));
    Fill23For105X (dest, 7, GoleyCoder ( *((word*) &data [10]) >> 4));

    data += 12;
    dest++;
   }
}

static inline dword Get23For1031 (byte *source, dword shift)
{
  dword ret = 0;

#undef DO_BIT
#define DO_BIT(x)							     \
  if (shift >= x)							     \
    ret |= (source [SIZE_DATA_FRAME_1031 * x] & (1 << shift)) >> (shift - x);\
  else                                                                       \
    ret |= (source [SIZE_DATA_FRAME_1031 * x] & (1 << shift)) << (x - shift);

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

static void DecodeGoleyClusterFor1031 (byte *dest, byte *data)
{
  dword s1, s2, i;
  
  for (i = 0; i < SIZE_DATA_FRAME_1031; i++)
   {
    s1 = GoleyDecoder (Get23For1031 (data, 0));
    s2 = GoleyDecoder (Get23For1031 (data, 1));
    *(dword*)&dest [0] = s1 | (s2 << G1_VALUE_LEN);

    s1 = GoleyDecoder (Get23For1031 (data, 2));
    s2 = GoleyDecoder (Get23For1031 (data, 3));
    *(dword*)&dest [3] = s1 | (s2 << G1_VALUE_LEN);

    s1 = GoleyDecoder (Get23For1031 (data, 4));
    s2 = GoleyDecoder (Get23For1031 (data, 5));
    *(dword*)&dest [6] = s1 | (s2 << G1_VALUE_LEN);

    s1 = GoleyDecoder (Get23For1031 (data, 6));
    s2 = GoleyDecoder (Get23For1031 (data, 7));
    *(dword*)&dest [9] = s1 | (s2 << G1_VALUE_LEN);

    dest += G1_VALUE_LEN;
    data++;
   }
}

static inline dword Get23For105X (byte *source, dword shift)
{
  dword ret = 0;

#undef DO_BIT
#define DO_BIT(x)							     \
  if (shift >= x)							     \
    ret |= (source [SIZE_DATA_FRAME_105X * x] & (1 << shift)) >> (shift - x);\
  else									     \
    ret |= (source [SIZE_DATA_FRAME_105X * x] & (1 << shift)) << (x - shift);

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

static void DecodeGoleyClusterFor105X (byte *dest, byte *data)
{
  dword s1, s2, i;
  
  for (i = 0; i < SIZE_DATA_FRAME_105X; i++)
   {
    s1 = GoleyDecoder (Get23For105X (data, 0));
    s2 = GoleyDecoder (Get23For105X (data, 1));
    *(dword*)&dest [0] = s1 | (s2 << G1_VALUE_LEN);

    s1 = GoleyDecoder (Get23For105X (data, 2));
    s2 = GoleyDecoder (Get23For105X (data, 3));
    *(dword*)&dest [3] = s1 | (s2 << G1_VALUE_LEN);

    s1 = GoleyDecoder (Get23For105X (data, 4));
    s2 = GoleyDecoder (Get23For105X (data, 5));
    *(dword*)&dest [6] = s1 | (s2 << G1_VALUE_LEN);

    s1 = GoleyDecoder (Get23For105X (data, 6));
    s2 = GoleyDecoder (Get23For105X (data, 7));
    *(dword*)&dest [9] = s1 | (s2 << G1_VALUE_LEN);

    dest += G1_VALUE_LEN;
    data++;
   }
}

static int DecodeGoleyClusterWithErrStatFor1031 (byte *dest, byte *data, estat *pes)
{
  dword s1, s2, i, ErrCounter;
  memset (pes, 0, sizeof (estat) - sizeof (dword));

  for (i = 0; i < SIZE_DATA_FRAME_1031; i++)
   {
    s1 = GoleyDecoderWithErrStat (Get23For1031 (data, 0), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23For1031 (data, 1), &ErrCounter);
    *(dword*)&dest [0] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    s1 = GoleyDecoderWithErrStat (Get23For1031 (data, 2), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23For1031 (data, 3), &ErrCounter);
    *(dword*)&dest [3] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    s1 = GoleyDecoderWithErrStat (Get23For1031 (data, 4), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23For1031 (data, 5), &ErrCounter);
    *(dword*)&dest [6] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    s1 = GoleyDecoderWithErrStat (Get23For1031 (data, 6), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23For1031 (data, 7), &ErrCounter);
    *(dword*)&dest [9] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    dest += G1_VALUE_LEN;
    data++;
   }

  return (pes->OrdinaryErr + (pes->DupletErr * 2) + (pes->TripletErr * 3));
}

static int DecodeGoleyClusterWithErrStatFor105X (byte *dest, byte *data, estat *pes)
{
  dword s1, s2, i, ErrCounter;
  memset (pes, 0, sizeof (estat) - sizeof (dword));

  for (i = 0; i < SIZE_DATA_FRAME_105X; i++)
   {
    s1 = GoleyDecoderWithErrStat (Get23For105X (data, 0), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23For105X (data, 1), &ErrCounter);
    *(dword*)&dest [0] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    s1 = GoleyDecoderWithErrStat (Get23For105X (data, 2), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23For105X (data, 3), &ErrCounter);
    *(dword*)&dest [3] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    s1 = GoleyDecoderWithErrStat (Get23For105X (data, 4), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23For105X (data, 5), &ErrCounter);
    *(dword*)&dest [6] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    s1 = GoleyDecoderWithErrStat (Get23For105X (data, 6), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23For105X (data, 7), &ErrCounter);
    *(dword*)&dest [9] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    dest += G1_VALUE_LEN;
    data++;
   }

  return (pes->OrdinaryErr + (pes->DupletErr * 2) + (pes->TripletErr * 3));
}

/* -------------- Goley frames ----------------- */
#define SIZE_G1_FRAME_BLOCK_105X (SIZE_DATA_FRAME_105X / G1_CODE_LEN)
#define SIZE_G1_FRAME_105X (SIZE_G1_FRAME_BLOCK_105X * G1_CODE_LEN)
#define SIZE_G1_FRAME_DATA_105X (SIZE_G1_FRAME_BLOCK_105X * G1_VALUE_LEN)

/* fill 23 bits consist in low 23 bits of gcode
   fill one bit of byte, because needs 8 operations for full byte,
   shift - destination bit number
   dest  - destination buffer */
static inline void Fill23ForFrame105X (byte *dest, dword shift, dword gcode)
{
/* make offset from dword to byte */
#undef DO_BIT
#define DO_BIT(x)						             \
  if (shift >= x)						             \
    dest [SIZE_G1_FRAME_BLOCK_105X * x] |= (gcode & (1 << x)) << (shift - x);\
  else								             \
    dest [SIZE_G1_FRAME_BLOCK_105X * x] |= (gcode & (1 << x)) >> (x - shift);

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

static void CodeGoleyFrameFor105X (byte *dest, byte *data)
{
  dword i;
  memset (dest, 0, SIZE_G1_FRAME_105X);

  /* code 12 source bytes SIZE_G1_FRAME_BLOCK_105X times */
  for (i = 0; i < SIZE_G1_FRAME_BLOCK_105X; i++)
   {
    Fill23ForFrame105X (dest, 0, GoleyCoder ( *((word*) &data [0]) & 0xfff));
    Fill23ForFrame105X (dest, 1, GoleyCoder ( *((word*) &data [1]) >> 4));

    Fill23ForFrame105X (dest, 2, GoleyCoder ( *((word*) &data [3]) & 0xfff));
    Fill23ForFrame105X (dest, 3, GoleyCoder ( *((word*) &data [4]) >> 4));

    Fill23ForFrame105X (dest, 4, GoleyCoder ( *((word*) &data [6]) & 0xfff));
    Fill23ForFrame105X (dest, 5, GoleyCoder ( *((word*) &data [7]) >> 4));

    Fill23ForFrame105X (dest, 6, GoleyCoder ( *((word*) &data [9]) & 0xfff));
    Fill23ForFrame105X (dest, 7, GoleyCoder ( *((word*) &data [10]) >> 4));

    data += 12;
    dest++;
   }
}

#define SIZE_G1_FRAME_BLOCK_1031 (SIZE_DATA_FRAME_1031 / G1_CODE_LEN)
#define SIZE_G1_FRAME_1031 (SIZE_G1_FRAME_BLOCK_1031 * G1_CODE_LEN)
#define SIZE_G1_FRAME_DATA_1031 (SIZE_G1_FRAME_BLOCK_1031 * G1_VALUE_LEN)

/* fill 23 bits consist in low 23 bits of gcode
   fill one bit of byte, because needs 8 operations for full byte,
   shift - destination bit number
   dest  - destination buffer */
static inline void Fill23ForFrame1031 (byte *dest, dword shift, dword gcode)
{
/* make offset from dword to byte */
#undef DO_BIT
#define DO_BIT(x)						             \
  if (shift >= x)						             \
    dest [SIZE_G1_FRAME_BLOCK_1031 * x] |= (gcode & (1 << x)) << (shift - x);\
  else								             \
    dest [SIZE_G1_FRAME_BLOCK_1031 * x] |= (gcode & (1 << x)) >> (x - shift);

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

static void CodeGoleyFrameFor1031 (byte *dest, byte *data)
{
  dword i;
  memset (dest, 0, SIZE_G1_FRAME_1031);

  /* code 12 source bytes SIZE_G1_FRAME_BLOCK_1031 times */
  for (i = 0; i < SIZE_G1_FRAME_BLOCK_1031; i++)
   {
    Fill23ForFrame1031 (dest, 0, GoleyCoder ( *((word*) &data [0]) & 0xfff));
    Fill23ForFrame1031 (dest, 1, GoleyCoder ( *((word*) &data [1]) >> 4));

    Fill23ForFrame1031 (dest, 2, GoleyCoder ( *((word*) &data [3]) & 0xfff));
    Fill23ForFrame1031 (dest, 3, GoleyCoder ( *((word*) &data [4]) >> 4));

    Fill23ForFrame1031 (dest, 4, GoleyCoder ( *((word*) &data [6]) & 0xfff));
    Fill23ForFrame1031 (dest, 5, GoleyCoder ( *((word*) &data [7]) >> 4));

    Fill23ForFrame1031 (dest, 6, GoleyCoder ( *((word*) &data [9]) & 0xfff));
    Fill23ForFrame1031 (dest, 7, GoleyCoder ( *((word*) &data [10]) >> 4));

    data += 12;
    dest++;
   }
}

static inline dword Get23ForFrame105X (byte *source, dword shift)
{
  dword ret = 0;

#undef DO_BIT
#define DO_BIT(x)							            \
  if (shift >= x)							            \
    ret |= (source [SIZE_G1_FRAME_BLOCK_105X * x] & (1 << shift)) >> (shift - x);\
  else									            \
    ret |= (source [SIZE_G1_FRAME_BLOCK_105X * x] & (1 << shift)) << (x - shift);

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

static void DecodeGoleyFrameFor105X (byte *dest, byte *data)
{
  dword s1, s2, i;

  for (i = 0; i < SIZE_G1_FRAME_BLOCK_105X; i++)
   {
    s1 = GoleyDecoder (Get23ForFrame105X (data, 0));
    s2 = GoleyDecoder (Get23ForFrame105X (data, 1));
    *(dword*)&dest [0] = s1 | (s2 << G1_VALUE_LEN);

    s1 = GoleyDecoder (Get23ForFrame105X (data, 2));
    s2 = GoleyDecoder (Get23ForFrame105X (data, 3));
    *(dword*)&dest [3] = s1 | (s2 << G1_VALUE_LEN);

    s1 = GoleyDecoder (Get23ForFrame105X (data, 4));
    s2 = GoleyDecoder (Get23ForFrame105X (data, 5));
    *(dword*)&dest [6] = s1 | (s2 << G1_VALUE_LEN);

    s1 = GoleyDecoder (Get23ForFrame105X (data, 6));
    s2 = GoleyDecoder (Get23ForFrame105X (data, 7));
    *(dword*)&dest [9] = s1 | (s2 << G1_VALUE_LEN);

    dest += G1_VALUE_LEN;
    data++;
   }
}

static inline dword Get23ForFrame1031 (byte *source, dword shift)
{
  dword ret = 0;

#undef DO_BIT
#define DO_BIT(x)							            \
  if (shift >= x)							            \
    ret |= (source [SIZE_G1_FRAME_BLOCK_1031 * x] & (1 << shift)) >> (shift - x);\
  else									            \
    ret |= (source [SIZE_G1_FRAME_BLOCK_1031 * x] & (1 << shift)) << (x - shift);

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

static void DecodeGoleyFrameFor1031 (byte *dest, byte *data)
{
  dword s1, s2, i;

  for (i = 0; i < SIZE_G1_FRAME_BLOCK_1031; i++)
   {
    s1 = GoleyDecoder (Get23ForFrame1031 (data, 0));
    s2 = GoleyDecoder (Get23ForFrame1031 (data, 1));
    *(dword*)&dest [0] = s1 | (s2 << G1_VALUE_LEN);

    s1 = GoleyDecoder (Get23ForFrame1031 (data, 2));
    s2 = GoleyDecoder (Get23ForFrame1031 (data, 3));
    *(dword*)&dest [3] = s1 | (s2 << G1_VALUE_LEN);

    s1 = GoleyDecoder (Get23ForFrame1031 (data, 4));
    s2 = GoleyDecoder (Get23ForFrame1031 (data, 5));
    *(dword*)&dest [6] = s1 | (s2 << G1_VALUE_LEN);

    s1 = GoleyDecoder (Get23ForFrame1031 (data, 6));
    s2 = GoleyDecoder (Get23ForFrame1031 (data, 7));
    *(dword*)&dest [9] = s1 | (s2 << G1_VALUE_LEN);

    dest += G1_VALUE_LEN;
    data++;
   }
}

static int DecodeGoleyFrameFor105XWithErrStat (byte *dest, byte *data, estat *pes)
{
  dword s1, s2, i, ErrCounter;
  memset (pes, 0, sizeof (estat) - sizeof (dword));

  for (i = 0; i < SIZE_G1_FRAME_BLOCK_105X; i++)
   {
    s1 = GoleyDecoderWithErrStat (Get23ForFrame105X (data, 0), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23ForFrame105X (data, 1), &ErrCounter);
    *(dword*)&dest [0] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    s1 = GoleyDecoderWithErrStat (Get23ForFrame105X (data, 2), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23ForFrame105X (data, 3), &ErrCounter);
    *(dword*)&dest [3] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    s1 = GoleyDecoderWithErrStat (Get23ForFrame105X (data, 4), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23ForFrame105X (data, 5), &ErrCounter);
    *(dword*)&dest [6] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    s1 = GoleyDecoderWithErrStat (Get23ForFrame105X (data, 6), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23ForFrame105X (data, 7), &ErrCounter);
    *(dword*)&dest [9] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    dest += G1_VALUE_LEN;
    data++;
   }

  return (pes->OrdinaryErr + (pes->DupletErr * 2) + (pes->TripletErr * 3));
}

static int DecodeGoleyFrameFor1031WithErrStat (byte *dest, byte *data, estat *pes)
{
  dword s1, s2, i, ErrCounter;
  memset (pes, 0, sizeof (estat) - sizeof (dword));

  for (i = 0; i < SIZE_G1_FRAME_BLOCK_1031; i++)
   {
    s1 = GoleyDecoderWithErrStat (Get23ForFrame1031 (data, 0), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23ForFrame1031 (data, 1), &ErrCounter);
    *(dword*)&dest [0] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    s1 = GoleyDecoderWithErrStat (Get23ForFrame1031 (data, 2), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23ForFrame1031 (data, 3), &ErrCounter);
    *(dword*)&dest [3] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    s1 = GoleyDecoderWithErrStat (Get23ForFrame1031 (data, 4), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23ForFrame1031 (data, 5), &ErrCounter);
    *(dword*)&dest [6] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    s1 = GoleyDecoderWithErrStat (Get23ForFrame1031 (data, 6), &ErrCounter);
    UpdateErrStat (&ErrCounter, pes);
    s2 = GoleyDecoderWithErrStat (Get23ForFrame1031 (data, 7), &ErrCounter);
    *(dword*)&dest [9] = s1 | (s2 << G1_VALUE_LEN);
    UpdateErrStat (&ErrCounter, pes);

    dest += G1_VALUE_LEN;
    data++;
   }

  return (pes->OrdinaryErr + (pes->DupletErr * 2) + (pes->TripletErr * 3));
}

/* select frame type */
/* frame operations */
void CodeGoleyFrame (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    CodeGoleyFrameFor105X (dest, data);
  else
    CodeGoleyFrameFor1031 (dest, data);
}

void DecodeGoleyFrame (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    DecodeGoleyFrameFor105X (dest, data);
  else
    DecodeGoleyFrameFor1031 (dest, data);
}

int DecodeGoleyFrameWithErrStat (byte *dest, byte *data, estat *pes, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return DecodeGoleyFrameFor105XWithErrStat (dest, data, pes);
  else
    return DecodeGoleyFrameFor1031WithErrStat (dest, data, pes);
}

/* cluster operations */
void CodeGoleyCluster (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    CodeGoleyClusterFor105X (dest, data);
  else
    CodeGoleyClusterFor1031 (dest, data);
}

void DecodeGoleyCluster (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    DecodeGoleyClusterFor105X (dest, data);
  else
    DecodeGoleyClusterFor1031 (dest, data);
}

int DecodeGoleyClusterWithErrStat (byte *dest, byte *data, estat *pes, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return DecodeGoleyClusterWithErrStatFor105X (dest, data, pes);
  else
    return DecodeGoleyClusterWithErrStatFor1031 (dest, data, pes);
}

/* get sizes */
int GetGoleyFrameDataSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return SIZE_G1_FRAME_DATA_105X;
  else
    return SIZE_G1_FRAME_DATA_1031;
}

int GetGoleyClusterDataSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return (SIZE_DATA_FRAME_105X * G1_VALUE_LEN);
  else
    return (SIZE_DATA_FRAME_1031 * G1_VALUE_LEN);
}

int GetGoleyFrameSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return SIZE_G1_FRAME_105X;
  else
    return SIZE_G1_FRAME_1031;
}

int GetGoleyClusterSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return (SIZE_DATA_FRAME_105X * G1_CODE_LEN);
  else
    return (SIZE_DATA_FRAME_1031 * G1_CODE_LEN);
}

/* Triplet is eqial 3 bytes.
 * Len string = (NumTriplets * 3 * 2) + 1,
 *  3 - sizeof (Triplet),
 *  2 - Goley richest,
 *  NumTriplets of Date */
void GoleyStringCoder (byte *String, byte *Date, int NumTriplets)
{
  int i;
  
  /* in 3 bytes - out 6 bytes */
  for (i = 0; i < NumTriplets; i++)
   {
    word Word = *Date | ((word)(Date [1] & 0xF) << 8);
    dword *pDString = (dword*)String;
    *pDString = GoleyCoder (Word);
    String +=3;
    
    pDString = (dword*)String;
    Word = Date [2] | ((word)(Date [1] & 0xF0) << 4);
    *pDString = GoleyCoder (Word);
    String +=3;
    
    Date +=3;
   }
}

/* NumTriplets of Date */
void GoleyStringDecoder (byte *Date, byte *String, int NumTriplets)
{
  int i;

  /* in 6 bytes - out 3 bytes */
  for (i = 0; i < NumTriplets; i++)
   {
    dword Dword = *(dword*)String & 0x7FFFFF;
    word Word = GoleyDecoder (Dword);
    *Date = Word;
    Date [1] = (Word >> 8) & 0xF;
    String +=3;
    
    Dword = *(dword*)String & 0x7FFFFF;
    Word = GoleyDecoder (Dword);
    Date [2] = Word;
    Date [1] |= (Word >> 4) & 0xF0;
    String +=3;
    
    Date +=3;
   }
}

/* Max lenght 48 bytes, valid lenght 6, 12, 18, 24, 30, 36, 42, 48 */
/* Len % 6 = 0 */
void StringMixer (byte *String, dword Len)
{
  dword NumTriplet = Len / 3, Mask = 1, i, j, shift;
  dword StepByte = NumTriplet >> 1; /* * num_bit_per_out_element / bytelen */
  ddword tmp;
  byte MixerBuff [1000];
  
  memset (MixerBuff, 0, Len);

  for (i = 0; i < G1_CODE_LEN; i++, Mask <<= 1)
   {
    for (j = 0; j < NumTriplet; j++)
     {
      /* Select next bit from triplet of string */
      tmp = *((dword*)(&String [j * 3])) & Mask;

      /* Shift correction */
      shift = (i & 3) * NumTriplet + j;

      if (shift > i)
	tmp <<= shift - i;
      else
	tmp >>= i - shift;

      *((ddword*)&MixerBuff [StepByte * (i >> 2)]) |= tmp;
     }
   }

  memcpy (String, MixerBuff, NumTriplet * 3);
}

/* Len % 6 = 0 */
void StringDemixer (byte *String, dword Len)
{
  dword NumTriplet = Len / 3, Mask = 1, i, j, shift;
  dword StepByte = NumTriplet >> 1; /* * numbitperoutelement / bytelen */
  ddword tmp;  
  byte MixerBuff [1000];
  
  memset (MixerBuff, 0, Len);

  for (i = 0; i < G1_CODE_LEN; i++, Mask <<= 1)
   {
    for (j = 0; j < NumTriplet; j++)
     {
      tmp = *((ddword*)&String [StepByte * (i >> 2)]);

      /* Shift correction */
      shift = (i & 3) * NumTriplet + j;

      if (shift > i)
	tmp >>= shift - i;
      else
	tmp <<= i - shift;

      *((dword*)(&MixerBuff [j * 3])) |= tmp & Mask;
     }
   }

  memcpy (String, MixerBuff, NumTriplet * 3);
}

void InitGoleyTable (void)
{
  RecalcErrorTable ();
  RecalcLastTable ();
  RecalcRestoreTable (ErrorTable);
  RecalcNumBitsTable ();
}

void FreeGoleyTable (void)
{
  free (ErrorTable);
  free (LastTable);
  free (RestoreTable);
  free (NumBitsTable);
}
