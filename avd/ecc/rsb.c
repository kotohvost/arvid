
#include <string.h>

#include "defines.h"
#include "arvid.h"
#include "ecc.h"
#include "rsb.h"
#include "rs.h"

void InitRS1Table (void) { init_rs (); }
void FreeRS1Table (void) { /* for function compability */ }

static void CodeRS1Blocks (byte *dest, byte *data, int blocks)
{
  int i, j;
  
  /* Make (SIZE_DATA_FRAME_10XX / NN) blocks */
  for (i = 0; i < blocks; i++)
   {
    dtype block1[NN]; /* 15 */
    dtype block2[NN]; /* 15 */
    dtype* pdest;

    for (j = 0; j < KK; j++, data++)
     {
       block1[j] = *data & 0xf;
       block2[j] = *data >> MM;
     }

    encode_rs (block1, &block1[KK]); /* 9 */
    encode_rs (block2, &block2[KK]); /* 9 */

    /* Make 15 bytes */
    pdest = dest + i;
    for (j = 0; j < NN; j++)
     {
      *pdest = block1[j] | (block2[j] << MM);
      pdest += blocks;
     }
   }
}

static void DecodeRS1Blocks (byte *dest, byte *data, int blocks)
{
  int i, j;
  dtype block1[NN]; /* 15 */
  dtype block2[NN]; /* 15 */

  /* Make (SIZE_DATA_FRAME_10XX / NN) blocks */
  for (i = 0; i < blocks; i++)
   {
    dtype *pdata = data + i;
    
    for (j = 0; j < NN; j++)
     {
      block1[j] = *pdata & 0xf;
      block2[j] = *pdata >> MM;
      pdata += blocks;
     }

    eras_dec_rs_wers (block1);
    eras_dec_rs_wers (block2);

    /* Make 15 bytes */
    for (j = 0; j < KK; j++, dest++)
      *dest = block1[j] | (block2[j] << MM);
   }
}

static int DecodeRS1BlocksWithErrStat (byte *dest, byte *data, estat *pes, int blocks)
{
  dtype block1[NN]; /* 15 */
  dtype block2[NN]; /* 15 */
  int i, j, errnum = 0;
  dword ernum1, ernum2;
  
  if (pes)
    memset (pes, 0, sizeof (estat));

  /* Make (SIZE_DATA_FRAME_10XX / NN) blocks */
  for (i = 0; i < blocks; i++)
   {
    dtype *pdata = data + i;
    
    for (j = 0; j < NN; j++)
     {
      block1[j] = *pdata & 0xf;
      block2[j] = *pdata >> MM;
      pdata += blocks;
     }

    ernum1 = (dword) eras_dec_rs_wers (block1);
    ernum2 = (dword) eras_dec_rs_wers (block2);

    /* Make 15 bytes */
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
    return (pes->OrdinaryErr + (pes->DupletErr * 2) + (pes->TripletErr * 3));
  else
    return errnum;
}

/* select frame type */

/* frames operation  */
void CodeRS1Frame (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    CodeRS1Blocks (dest, data, SIZE_DATA_FRAME_105X / NN);
  else
    CodeRS1Blocks (dest, data, SIZE_DATA_FRAME_1031 / NN);
}

void DecodeRS1Frame (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    DecodeRS1Blocks (dest, data, SIZE_DATA_FRAME_105X / NN);
  else
    DecodeRS1Blocks (dest, data, SIZE_DATA_FRAME_1031 / NN);
}

int DecodeRS1FrameWithErrStat (byte *dest, byte *data, estat *pes, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return DecodeRS1BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_105X / NN);
  else
    return DecodeRS1BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_1031 / NN);
}

/* clusters operation */

void CodeRS1Cluster (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    CodeRS1Blocks (dest, data, SIZE_DATA_FRAME_105X);
  else
    CodeRS1Blocks (dest, data, SIZE_DATA_FRAME_1031);
}

void DecodeRS1Cluster (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    DecodeRS1Blocks (dest, data, SIZE_DATA_FRAME_105X);
  else
    DecodeRS1Blocks (dest, data, SIZE_DATA_FRAME_1031);
}

int DecodeRS1ClusterWithErrStat (byte *dest, byte *data, estat *pes, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return DecodeRS1BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_105X);
  else
    return DecodeRS1BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_1031);
}

int GetRS1FrameDataSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return ((SIZE_DATA_FRAME_105X / RS1_CODE_LEN) * RS1_VALUE_LEN);
  else
    return ((SIZE_DATA_FRAME_1031 / RS1_CODE_LEN) * RS1_VALUE_LEN);
}

int GetRS1ClusterDataSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return (SIZE_DATA_FRAME_105X * RS1_VALUE_LEN);
  else
    return (SIZE_DATA_FRAME_1031 * RS1_VALUE_LEN);
}

int GetRS1FrameSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return ((SIZE_DATA_FRAME_105X / RS1_CODE_LEN) * RS1_CODE_LEN);
  else
    return ((SIZE_DATA_FRAME_1031 / RS1_CODE_LEN) * RS1_CODE_LEN);
}

int GetRS1ClusterSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return (SIZE_DATA_FRAME_105X * RS1_CODE_LEN);
  else
    return (SIZE_DATA_FRAME_1031 * RS1_CODE_LEN);
}
