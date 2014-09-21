
#include <string.h>

#include "defines.h"
#include "arvid.h"
#include "ecc.h"
#include "rs3b.h"
#include "rs3.h"

void InitRS3Table (void) { init_rs3 (); }
void FreeRS3Table (void) { /* for function compability */ }

static void CodeRS3Blocks (byte *dest, byte *data, int blocks)
{
  int i, j;
  
  /* Make (SIZE_DATA_FRAME_10XX / NN) blocks */
  for (i = 0; i < blocks; i++)
   {
    dtype block1[NN]; /* 15 */
    dtype block2[NN]; /* 15 */
    dtype* pdest;

    for (j = 0; j < KK; j++, data++)
     {
       block1[j] = *data & 0xf;
       block2[j] = *data >> MM;
     }

    encode_rs3 (block1, &block1[KK]); /* 11 */
    encode_rs3 (block2, &block2[KK]); /* 11 */

    /* Make 15 bytes */
    pdest = dest + i;
    for (j = 0; j < NN; j++)
     {
      *pdest = block1[j] | (block2[j] << MM);
      pdest += blocks;
     }
   }
}

static void DecodeRS3Blocks (byte *dest, byte *data, int blocks)
{
  int i, j;
  dtype block1[NN]; /* 15 */
  dtype block2[NN]; /* 15 */

  /* Make (SIZE_DATA_FRAME_10XX / NN) blocks */
  for (i = 0; i < blocks; i++)
   {
    dtype *pdata = data + i;
    
    for (j = 0; j < NN; j++)
     {
      block1[j] = *pdata & 0xf;
      block2[j] = *pdata >> MM;
      pdata += blocks;
     }

    eras_dec_rs3_wers (block1);
    eras_dec_rs3_wers (block2);

    /* Make 15 bytes */
    for (j = 0; j < KK; j++, dest++)
      *dest = block1[j] | (block2[j] << MM);
   }
}

static int DecodeRS3BlocksWithErrStat (byte *dest, byte *data, estat *pes, int blocks)
{
  dtype block1[NN]; /* 15 */
  dtype block2[NN]; /* 15 */
  int i, j, errnum = 0;
  dword ernum1, ernum2;
  
  if (pes)
    memset (pes, 0, sizeof (estat));

  /* Make (SIZE_DATA_FRAME_10XX / NN) blocks */
  for (i = 0; i < blocks; i++)
   {
    dtype *pdata = data + i;
    
    for (j = 0; j < NN; j++)
     {
      block1[j] = *pdata & 0xf;
      block2[j] = *pdata >> MM;
      pdata += blocks;
     }

    ernum1 = (dword) eras_dec_rs3_wers (block1);
    ernum2 = (dword) eras_dec_rs3_wers (block2);

    /* Make 15 bytes */
    for (j = 0; j < KK; j++, dest++)
      *dest = block1[j] | (block2[j] << MM);

    if (pes)
     {
      UpdateErrStat (&ernum1, pes);
      UpdateErrStat (&ernum2, pes);
     }
     
    if (ernum1 == (dword)-1 || ernum2 == (dword)-1)
      errnum = -1;
   }

  if (pes && !errnum)
    return (pes->OrdinaryErr + (pes->DupletErr * 2));
  else
    return errnum;
}

/* select frame type */

/* frames operation  */
void CodeRS3Frame (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    CodeRS3Blocks (dest, data, SIZE_DATA_FRAME_105X / NN);
  else
    CodeRS3Blocks (dest, data, SIZE_DATA_FRAME_1031 / NN);
}

void DecodeRS3Frame (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    DecodeRS3Blocks (dest, data, SIZE_DATA_FRAME_105X / NN);
  else
    DecodeRS3Blocks (dest, data, SIZE_DATA_FRAME_1031 / NN);
}

int DecodeRS3FrameWithErrStat (byte *dest, byte *data, estat *pes, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return DecodeRS3BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_105X / NN);
  else
    return DecodeRS3BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_1031 / NN);
}

/* clusters operation */
void CodeRS3Cluster (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    CodeRS3Blocks (dest, data, SIZE_DATA_FRAME_105X);
  else
    CodeRS3Blocks (dest, data, SIZE_DATA_FRAME_1031);
}

void DecodeRS3Cluster (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    DecodeRS3Blocks (dest, data, SIZE_DATA_FRAME_105X);
  else
    DecodeRS3Blocks (dest, data, SIZE_DATA_FRAME_1031);
}

int DecodeRS3ClusterWithErrStat (byte *dest, byte *data, estat *pes, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return DecodeRS3BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_105X);
  else
    return DecodeRS3BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_1031);
}

int GetRS3FrameDataSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return ((SIZE_DATA_FRAME_105X / RS3_CODE_LEN) * RS3_VALUE_LEN);
  else
    return ((SIZE_DATA_FRAME_1031 / RS3_CODE_LEN) * RS3_VALUE_LEN);
}

int GetRS3ClusterDataSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return (SIZE_DATA_FRAME_105X * RS3_VALUE_LEN);
  else
    return (SIZE_DATA_FRAME_1031 * RS3_VALUE_LEN);
}

int GetRS3FrameSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return ((SIZE_DATA_FRAME_105X / RS3_CODE_LEN) * RS3_CODE_LEN);
  else
    return ((SIZE_DATA_FRAME_1031 / RS3_CODE_LEN) * RS3_CODE_LEN);
}

int GetRS3ClusterSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return (SIZE_DATA_FRAME_105X * RS3_CODE_LEN);
  else
    return (SIZE_DATA_FRAME_1031 * RS3_CODE_LEN);
}
