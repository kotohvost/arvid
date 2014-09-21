
#ifndef __ECC_H__
#define __ECC_H__

/* ECC functions flags */
/* Use one Code Definition flag, Block/Cluster definition flag and
 * density definition flag */
#define GOLEY_FLAG	0 /* ECC Goley (23,12,7), restore triplet errors */
#define RS1_FLAG	1 /* ECC Reed-Solomon (15,9), restore triplet errors */
#define RS2_FLAG	2 /* ECC Reed-Solomon (15,11), restore duplet errors */
#define RS3_FLAG	3 /* ECC Reed-Solomon (15,13), restore ordinary errors */
#define RS4_FLAG	4 /* ECC Reed-Solomon (63,55), restore quadro errors */
#define BCH1_FLAG	5 /* ECC BCH (63,51), restore duplet errors, fastest RS */

#define ECC_MASK	0xF

#define CLUSTER_BLOCK	0x10 /* interframe ECC coding */
#define FRAME_BLOCK     0    /* frame ECC */

#define ARVID1052_FLAG	SET_325K /* setup native density for 105X boards */
#define ARVID1031_FLAG	0        /* setup native density for 103X boards */

#define TEST_DATA  "0123456789abcdefghiklmnoprtquwxyzABCDEFGHIKLMNOPRSTQUWXYZ"
#define TEST_DATA2 "ABCDEFGHIKLMNOPRS"

typedef struct
{
  dword  OrdinaryErr;  /* Ordinary errors counter */
  dword  DupletErr;    /* Duplet errors counter   */
  dword  TripletErr;   /* Triplet errors counter  */
  dword  QuadroErr;    /* Quadro errors counter   */
  ddword MapBadFrames; /* Bit mask of bad frames in cluster */
  dword	 BadFrames;    /* Bad frames in cluster counter */
} estat;

extern void InitECCTable (void);
extern void FreeECCTable (void);

/* Blocks ECC methods (independiding block) */
extern void CodeBlock (byte *dest, byte *data, dword Flags);
extern void DecodeBlock (byte *dest, byte *data, dword Flags);
extern int  DecodeBlockWithErrStat (byte *dest, byte *data, estat *pes, dword Flags);

/* Block methods union frames methods and clusters methods */
/* Frames ECC methods */
extern void CodeFrame (byte *dest, byte *data, dword Flags);
extern void DecodeFrame (byte *dest, byte *data, dword Flags);
extern int  DecodeFrameWithErrStat (byte *dest, byte *data, estat *pes, dword Flags);

/* Clusters ECC methods */
extern void CodeCluster (byte *dest, byte *data, dword Flags);
extern void DecodeCluster (byte *dest, byte *data, dword Flags);
extern int  DecodeClusterWithErrStat (byte *dest, byte *data, estat *pes, dword Flags);

/* Get raw data block size methods */
extern int  GetBlockDataSize (dword Flags);
extern int  GetFrameDataSize (dword Flags);
extern int  GetClusterDataSize (dword Flags);

/* Get ECC encoded data block size methods */
extern int  GetBlockSize (dword Flags);
extern int  GetFrameSize (dword Flags);
extern int  GetClusterSize (dword Flags);

/* ECC independent ECC block test */
extern void ECCTest (int enc_test_blocks, int dec_test_blocks, dword Flags);

/* Update estat after block recovering */
static inline void UpdateErrStat (dword *ErrCounter, estat *ErrStat)
{
  switch (*ErrCounter)
   {
    case 0: return;
    case 1: ErrStat->OrdinaryErr++; return;
    case 2: ErrStat->DupletErr++;   return;
    case 3: ErrStat->TripletErr++;  return;
    case 4: ErrStat->QuadroErr++;   return;
    case -1: ErrStat->BadFrames++;  return;
   }
}

#endif
