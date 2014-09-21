
#include <string.h>

#include "defines.h"
#include "arvid.h"
#include "ecc.h"
#include "rs2b.h"
#include "rs2.h"

void InitRS2Table (void) { init_rs2 (); }
void FreeRS2Table (void) { /* for function compability */ }

static void CodeRS2Blocks (byte *dest, byte *data, int blocks)
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

    encode_rs2 (block1, &block1[KK]); /* 11 */
    encode_rs2 (block2, &block2[KK]); /* 11 */

    /* Make 15 bytes */
    pdest = dest + i;
    for (j = 0; j < NN; j++)
     {
      *pdest = block1[j] | (block2[j] << MM);
      pdest += blocks;
     }
   }
}

static void DecodeRS2Blocks (byte *dest, byte *data, int blocks)
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

    eras_dec_rs2_wers (block1);
    eras_dec_rs2_wers (block2);

    /* Make 15 bytes */
    for (j = 0; j < KK; j++, dest++)
      *dest = block1[j] | (block2[j] << MM);
   }
}

static int DecodeRS2BlocksWithErrStat (byte *dest, byte *data, estat *pes, int blocks)
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

    ernum1 = (dword) eras_dec_rs2_wers (block1);
    ernum2 = (dword) eras_dec_rs2_wers (block2);

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

/* frame operations  */
void CodeRS2Frame (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    CodeRS2Blocks (dest, data, SIZE_DATA_FRAME_105X / NN);
  else
    CodeRS2Blocks (dest, data, SIZE_DATA_FRAME_1031 / NN);
}

void DecodeRS2Frame (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    DecodeRS2Blocks (dest, data, SIZE_DATA_FRAME_105X / NN);
  else
    DecodeRS2Blocks (dest, data, SIZE_DATA_FRAME_1031 / NN);
}

int DecodeRS2FrameWithErrStat (byte *dest, byte *data, estat *pes, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return DecodeRS2BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_105X / NN);
  else
    return DecodeRS2BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_1031 / NN);
}

/* clusters operation */
void CodeRS2Cluster (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    CodeRS2Blocks (dest, data, SIZE_DATA_FRAME_105X);
  else
    CodeRS2Blocks (dest, data, SIZE_DATA_FRAME_1031);
}

void DecodeRS2Cluster (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    DecodeRS2Blocks (dest, data, SIZE_DATA_FRAME_105X);
  else
    DecodeRS2Blocks (dest, data, SIZE_DATA_FRAME_1031);
}

int DecodeRS2ClusterWithErrStat (byte *dest, byte *data, estat *pes, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return DecodeRS2BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_105X);
  else
    return DecodeRS2BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_1031);
}

int GetRS2FrameDataSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return ((SIZE_DATA_FRAME_105X / RS2_CODE_LEN) * RS2_VALUE_LEN);
  else
    return ((SIZE_DATA_FRAME_1031 / RS2_CODE_LEN) * RS2_VALUE_LEN);
}

int GetRS2ClusterDataSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return (SIZE_DATA_FRAME_105X * RS2_VALUE_LEN);
  else
    return (SIZE_DATA_FRAME_1031 * RS2_VALUE_LEN);
}

int GetRS2FrameSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return ((SIZE_DATA_FRAME_105X / RS2_CODE_LEN) * RS2_CODE_LEN);
  else
    return ((SIZE_DATA_FRAME_1031 / RS2_CODE_LEN) * RS2_CODE_LEN);
}

int GetRS2ClusterSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return (SIZE_DATA_FRAME_105X * RS2_CODE_LEN);
  else
    return (SIZE_DATA_FRAME_1031 * RS2_CODE_LEN);
}
