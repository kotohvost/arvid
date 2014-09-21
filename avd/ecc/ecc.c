
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "defines.h"
#include "dprint.h"
#include "arvid.h"
#include "mtime.h"
#include "ecc.h"

#include "goley.h"
#include "rsb.h"
#include "rs2b.h"
#include "rs3b.h"
#include "bchb.h"

/* Select of ECC codec */
void InitECCTable (void)
{
  InitGoleyTable ();
  InitRS1Table ();
  InitRS2Table ();
  InitRS3Table ();
  InitBCH1Table ();
}

void FreeECCTable (void)
{
  FreeBCH1Table ();
  FreeRS3Table ();
  FreeRS2Table ();
  FreeRS1Table ();
  FreeGoleyTable ();
}

/* frame operations */
void CodeFrame (byte *dest, byte *data, dword Flags)
{
  switch (Flags & ECC_MASK)
   {
    case GOLEY_FLAG: CodeGoleyFrame (dest, data, Flags); break;
    case   RS1_FLAG: CodeRS1Frame   (dest, data, Flags); break;
    case   RS2_FLAG: CodeRS2Frame   (dest, data, Flags); break;
    case   RS3_FLAG: CodeRS3Frame   (dest, data, Flags); break;
    case  BCH1_FLAG: CodeBCH1Frame  (dest, data, Flags); break;
    default: dprintf (1, "ECC:CodeFrame:Undefined ECC codec: %i!\n", Flags & ECC_MASK); break;
   }
}

void DecodeFrame (byte *dest, byte *data, dword Flags)
{
  switch (Flags & ECC_MASK)
   {
    case GOLEY_FLAG: DecodeGoleyFrame (dest, data, Flags); break;
    case   RS1_FLAG: DecodeRS1Frame   (dest, data, Flags); break;
    case   RS2_FLAG: DecodeRS2Frame   (dest, data, Flags); break;
    case   RS3_FLAG: DecodeRS3Frame   (dest, data, Flags); break;
    case  BCH1_FLAG: DecodeBCH1Frame  (dest, data, Flags); break;
    default: dprintf (1, "ECC:DecodeFrame:Undefined ECC codec: %i!\n", Flags & ECC_MASK); break;
   }
}

int DecodeFrameWithErrStat (byte *dest, byte *data, estat *pes, dword Flags)
{
  switch (Flags & ECC_MASK)
   {
    case GOLEY_FLAG: return DecodeGoleyFrameWithErrStat (dest, data, pes, Flags);
    case   RS1_FLAG: return DecodeRS1FrameWithErrStat   (dest, data, pes, Flags);
    case   RS2_FLAG: return DecodeRS2FrameWithErrStat   (dest, data, pes, Flags);
    case   RS3_FLAG: return DecodeRS3FrameWithErrStat   (dest, data, pes, Flags);
    case  BCH1_FLAG: return DecodeBCH1FrameWithErrStat  (dest, data, pes, Flags);
    default: dprintf (1, "ECC:DecodeFrameWithErrStat:Undefined ECC codec: %i!\n", Flags & ECC_MASK); break;
   }
   
  return ERROR;
}

/* cluster operations */
void CodeCluster (byte *dest, byte *data, dword Flags)
{
  switch (Flags & ECC_MASK)
   {
    case GOLEY_FLAG: CodeGoleyCluster (dest, data, Flags); break;
    case   RS1_FLAG: CodeRS1Cluster   (dest, data, Flags); break;
    case   RS2_FLAG: CodeRS2Cluster   (dest, data, Flags); break;
    case   RS3_FLAG: CodeRS3Cluster   (dest, data, Flags); break;
    case  BCH1_FLAG: CodeBCH1Cluster  (dest, data, Flags); break;
    default: dprintf (1, "ECC:CodeCluster:Undefined ECC codec: %i!\n", Flags & ECC_MASK); break;
   }
}

void DecodeCluster (byte *dest, byte *data, dword Flags)
{
  switch (Flags & ECC_MASK)
   {
    case GOLEY_FLAG: DecodeGoleyCluster (dest, data, Flags); break;
    case   RS1_FLAG: DecodeRS1Cluster   (dest, data, Flags); break;
    case   RS2_FLAG: DecodeRS2Cluster   (dest, data, Flags); break;
    case   RS3_FLAG: DecodeRS3Cluster   (dest, data, Flags); break;
    case  BCH1_FLAG: DecodeBCH1Cluster  (dest, data, Flags); break;
    default: dprintf (1, "ECC:DecodeCluster:Undefined ECC codec: %i!\n", Flags & ECC_MASK); break;
   }
}

int DecodeClusterWithErrStat (byte *dest, byte *data, estat *pes, dword Flags)
{
  switch (Flags & ECC_MASK)
   {
    case GOLEY_FLAG: return DecodeGoleyClusterWithErrStat (dest, data, pes, Flags);
    case   RS1_FLAG: return DecodeRS1ClusterWithErrStat   (dest, data, pes, Flags);
    case   RS2_FLAG: return DecodeRS2ClusterWithErrStat   (dest, data, pes, Flags);
    case   RS3_FLAG: return DecodeRS3ClusterWithErrStat   (dest, data, pes, Flags);
    case  BCH1_FLAG: return DecodeBCH1ClusterWithErrStat  (dest, data, pes, Flags);
    default: dprintf (1, "ECC:DecodeClusterWithErrStat:Undefined ECC codec: %i!\n", Flags & ECC_MASK); break;
   }

  return ERROR;
}

/* block operations */
void CodeBlock (byte *dest, byte *data, dword Flags)
{
  if (Flags & CLUSTER_BLOCK)
    CodeCluster (dest, data, Flags);
  else
    CodeFrame (dest, data, Flags);
}

void DecodeBlock (byte *dest, byte *data, dword Flags)
{
  if (Flags & CLUSTER_BLOCK)
    DecodeCluster (dest, data, Flags);
  else
    DecodeFrame (dest, data, Flags);
}

int  DecodeBlockWithErrStat (byte *dest, byte *data, estat *pes, dword Flags)
{
  if (Flags & CLUSTER_BLOCK)
    return DecodeClusterWithErrStat (dest, data, pes, Flags);
  else
    return DecodeFrameWithErrStat (dest, data, pes, Flags);
}

/* frame & cluster sizes */
int  GetFrameDataSize (dword Flags)
{
  switch (Flags & ECC_MASK)
   {
    case GOLEY_FLAG: return GetGoleyFrameDataSize (Flags);
    case   RS1_FLAG: return GetRS1FrameDataSize   (Flags);
    case   RS2_FLAG: return GetRS2FrameDataSize   (Flags);
    case   RS3_FLAG: return GetRS3FrameDataSize   (Flags);
    case  BCH1_FLAG: return GetBCH1FrameDataSize  (Flags);
    default: dprintf (1, "ECC:GetFrameDataSize:Undefined ECC codec: %i!\n", Flags & ECC_MASK); break;
   }

  return false;
}

int  GetClusterDataSize (dword Flags)
{
  switch (Flags & ECC_MASK)
   {
    case GOLEY_FLAG: return GetGoleyClusterDataSize (Flags);
    case   RS1_FLAG: return GetRS1ClusterDataSize   (Flags);
    case   RS2_FLAG: return GetRS2ClusterDataSize   (Flags);
    case   RS3_FLAG: return GetRS3ClusterDataSize   (Flags);
    case  BCH1_FLAG: return GetBCH1ClusterDataSize  (Flags);
    default: dprintf (1, "ECC:GetClusterDataSize:Undefined ECC codec: %i!\n", Flags & ECC_MASK); break;
   }

  return false;
}

int  GetFrameSize (dword Flags)
{
  switch (Flags & ECC_MASK)
   {
    case GOLEY_FLAG: return GetGoleyFrameSize (Flags);
    case   RS1_FLAG: return GetRS1FrameSize   (Flags);
    case   RS2_FLAG: return GetRS2FrameSize   (Flags);
    case   RS3_FLAG: return GetRS3FrameSize   (Flags);
    case  BCH1_FLAG: return GetBCH1FrameSize  (Flags);
    default: dprintf (1, "ECC:GetFrameSize:Undefined ECC codec: %i!\n", Flags & ECC_MASK); break;
   }

  return false;
}

int  GetClusterSize (dword Flags)
{
  switch (Flags & ECC_MASK)
   {
    case GOLEY_FLAG: return GetGoleyClusterSize (Flags);
    case   RS1_FLAG: return GetRS1ClusterSize   (Flags);
    case   RS2_FLAG: return GetRS2ClusterSize   (Flags);
    case   RS3_FLAG: return GetRS3ClusterSize   (Flags);
    case  BCH1_FLAG: return GetBCH1ClusterSize  (Flags);
    default: dprintf (1, "ECC:GetClusterSize:Undefined ECC codec: %i!\n", Flags & ECC_MASK); break;
   }

  return false;
}

/* blocks sizes */
int  GetBlockDataSize (dword Flags)
{
  if (Flags & CLUSTER_BLOCK)
    return GetClusterDataSize (Flags);
  else
    return GetFrameDataSize (Flags);
}

int  GetBlockSize (dword Flags)
{
  if (Flags & CLUSTER_BLOCK)
    return GetClusterSize (Flags);
  else
    return GetFrameSize (Flags);
}

void ECCTest (int enc_test_blocks, int dec_test_blocks, dword Flags)
{
  dprintf (1, "Encode %i %s", enc_test_blocks, Flags & CLUSTER_BLOCK ? "clusters " : "frames ");
  
  switch (Flags & ECC_MASK)
   {
    case GOLEY_FLAG: dprintf (1, "Goley (%i,%i), ", G1_CODE_LEN, G1_VALUE_LEN); break;
    case   RS1_FLAG: dprintf (1, "RS (%i,%i), ", RS1_CODE_LEN, RS1_VALUE_LEN);  break;
    case   RS2_FLAG: dprintf (1, "RS (%i,%i), ", RS2_CODE_LEN, RS2_VALUE_LEN);  break;
    case   RS3_FLAG: dprintf (1, "RS (%i,%i), ", RS3_CODE_LEN, RS3_VALUE_LEN);  break;
    case  BCH1_FLAG: dprintf (1, "BCH (%i,%i), ", BCH1_CODE_LEN, BCH1_VALUE_LEN);  break;
    default: dprintf (1, "\nECC:ECCTest:Undefined ECC codec: %i!\n", Flags & ECC_MASK); break;
   }
  
  dprintf (1, "ArVid %s mode.\n", Flags & ARVID1052_FLAG ? "1052" : "1031(1020)");
    
  dword SizeOrigBlock = GetBlockDataSize (Flags);
  dword SizeCodeBlock = GetBlockSize (Flags);
  dprintf (1, "Size original block: %i, size encoded block: %i.\n", SizeOrigBlock, SizeCodeBlock);

  byte *data = malloc (sizeof (byte) * (SizeOrigBlock + 1));
  byte *dest = malloc (sizeof (byte) * (SizeCodeBlock + 1));
  strcpy ((char*)data, TEST_DATA);

  size_t count = enc_test_blocks;
  time_t start_time = get_time ();
  while (start_time == get_time ());
  start_time = get_time ();
  
  while (count--)
  {
    CodeBlock (dest, data, Flags);
    Progress ();
  }
  
  time_t end_time = get_time ();
  estat es;

  dprintf (1, "\nSend rate: %.4f Bps\n", (float) SizeOrigBlock * enc_test_blocks * 1000 / (float) (end_time - start_time));
  
  dprintf (1, "Facking data: ");
  print_data (1, dest, sizeof (TEST_DATA));
/*  dump (1, dest, sizeof (TEST_DATA)); */
  /* dprintf (1, "Decode %i %s...\n", dec_test_blocks, Flags & CLUSTER_BLOCK ? "clusters" : "frames"); */
  
  dprintf (5, "\n|--Cluster--|-----I----|----II----|----III---|--Summary--|\n");
  
  count = dec_test_blocks;
  start_time = get_time ();
  
  while (start_time == get_time ());
  start_time = get_time ();
  
  while (count--)
   {
/*    DecodeBlock (data, dest, Flags); */
    DecodeBlockWithErrStat (data, dest, &es, Flags);
    Progress ();

/*    dprintf (5, "| %9d | %8d | %8d | %8d | %9d |\n", (dword)(dec_test_frames - count),
	     es.OrdinaryErr, es.DupletErr, es.TripletErr, es.OrdinaryErr +
	    (es.DupletErr * 2) + (es.TripletErr * 3)); */
   }
   
  end_time = get_time ();

  dprintf (1, "\nReceive rate: %.4f Bps\n", (float) SizeOrigBlock * dec_test_blocks * 1000 / (float) (end_time - start_time));
/*  printf ("Restored data: %s\n", data); */
  dprintf (1, "Restored data: ");
  print_data (1, data, sizeof (TEST_DATA));
/*  dump (1, data, sizeof (TEST_DATA)); */

  free (data);
  free (dest);
}
