
#include <string.h>

#include "defines.h"
#include "arvid.h"
#include "ecc.h"
#include "bchb.h"
#include "bch4836.h"

void InitBCH1Table (void)
{
  init_bch ();
}

void FreeBCH1Table (void)
{
  free_bch ();
}

static void CodeBCH1Blocks (byte *dest, byte *data, int blocks)
{
  int i, j, b, src_off, dst_off, bch [BCH1_CODE_LEN];
  memset (dest, 0, sizeof (byte) * blocks * BCH1_CODE_LEN);

  for (src_off = 0, i = 0; i < blocks; i++, src_off += BCH1_VALUE_LEN)
   {
    for (b = 0; b < 8; b++)
     {
      memset (bch, 0, sizeof (int) * RDNC);

      for (j = 0; j < BCH1_VALUE_LEN; j++)
        bch[j + RDNC] = (data[src_off + j] >> b) & 1;

      encode_bch (&bch[RDNC], bch);

      for (dst_off = 0, j = 0; j < BCH1_CODE_LEN; j++, dst_off += BCH1_CODE_LEN)
        dest[dst_off + i] |= bch[j] << b;
     }
   }
}

static void DecodeBCH1Blocks (byte *dest, byte *data, int blocks)
{
  int i, j, b, src_off, dst_off, bch [BCH1_CODE_LEN];
  memset (dest, 0, sizeof (byte) * blocks * BCH1_VALUE_LEN);

  for (dst_off = 0, i = 0; i < blocks; i++, dst_off += BCH1_VALUE_LEN)
   {
    for (b = 0; b < 8; b++)
     {
      for (src_off = 0, j = 0; j < BCH1_CODE_LEN; j++, src_off += BCH1_CODE_LEN)
        bch[j] = (data[src_off + i] >> b) & 1;

      decode_bch (bch);

      for (j = 0; j < BCH1_VALUE_LEN; j++)
        dest[dst_off + j] |= bch[j + RDNC] << b;
     }
   }
}

static int DecodeBCH1BlocksWithErrStat (byte *dest, byte *data, estat *pes, int blocks)
{
  int i, j, b, err, errnum = 0, src_off, dst_off, bch [BCH1_CODE_LEN];
  memset (dest, 0, sizeof (byte) * blocks * BCH1_VALUE_LEN);
  
  if (pes)
    memset (pes, 0, sizeof (estat));

  for (dst_off = 0, i = 0; i < blocks; i++, dst_off += BCH1_VALUE_LEN)
   {
    for (b = 0; b < 8; b++)
     {
      for (src_off = 0, j = 0; j < BCH1_CODE_LEN; j++, src_off += BCH1_CODE_LEN)
        bch[j] = (data[src_off + i] >> b) & 1;

      err = decode_bch (bch);
      UpdateErrStat (&err, pes);
      
      if (err == -1)
        errnum = -1;

      for (j = 0; j < BCH1_VALUE_LEN; j++)
        dest[dst_off + j] |= bch[j + RDNC] << b;
     }
   }

  if (pes && !errnum)
    return (pes->OrdinaryErr + (pes->DupletErr * 2));
  else
    return errnum;
}

/* Frames BCH1 methods */
void CodeBCH1Frame (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    CodeBCH1Blocks (dest, data, SIZE_DATA_FRAME_105X / BCH1_CODE_LEN);
  else
    CodeBCH1Blocks (dest, data, SIZE_DATA_FRAME_1031 / BCH1_CODE_LEN);
}

void DecodeBCH1Frame (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    DecodeBCH1Blocks (dest, data, SIZE_DATA_FRAME_105X / BCH1_CODE_LEN);
  else
    DecodeBCH1Blocks (dest, data, SIZE_DATA_FRAME_1031 / BCH1_CODE_LEN);
}

int DecodeBCH1FrameWithErrStat (byte *dest, byte *data, estat *pes, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return DecodeBCH1BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_105X / BCH1_CODE_LEN);
  else
    return DecodeBCH1BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_1031 / BCH1_CODE_LEN);
}

/* Clusters BCH1 methods */
void CodeBCH1Cluster (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    CodeBCH1Blocks (dest, data, SIZE_DATA_FRAME_105X);
  else
    CodeBCH1Blocks (dest, data, SIZE_DATA_FRAME_1031);
}

void DecodeBCH1Cluster (byte *dest, byte *data, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    DecodeBCH1Blocks (dest, data, SIZE_DATA_FRAME_105X);
  else
    DecodeBCH1Blocks (dest, data, SIZE_DATA_FRAME_1031);
}

int DecodeBCH1ClusterWithErrStat (byte *dest, byte *data, estat *pes, dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return DecodeBCH1BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_105X);
  else
    return DecodeBCH1BlocksWithErrStat (dest, data, pes, SIZE_DATA_FRAME_1031);
}

/* Get raw data block size methods */
int GetBCH1FrameDataSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return ((SIZE_DATA_FRAME_105X / BCH1_CODE_LEN) * BCH1_VALUE_LEN);
  else
    return ((SIZE_DATA_FRAME_1031 / BCH1_CODE_LEN) * BCH1_VALUE_LEN);
}

int GetBCH1ClusterDataSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return (SIZE_DATA_FRAME_105X * BCH1_VALUE_LEN);
  else
    return (SIZE_DATA_FRAME_1031 * BCH1_VALUE_LEN);
}

/* Get BCH1 encoded data block size methods */
int GetBCH1FrameSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return ((SIZE_DATA_FRAME_105X / BCH1_CODE_LEN) * BCH1_CODE_LEN);
  else
    return ((SIZE_DATA_FRAME_1031 / BCH1_CODE_LEN) * BCH1_CODE_LEN);
}

int GetBCH1ClusterSize (dword Flags)
{
  if (Flags & ARVID1052_FLAG)
    return (SIZE_DATA_FRAME_105X * BCH1_CODE_LEN);
  else
    return (SIZE_DATA_FRAME_1031 * BCH1_CODE_LEN);
}
