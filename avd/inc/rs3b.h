
#ifndef __RS3B_H__
#define __RS3B_H__

#define RS3_CODE_LEN         15
#define RS3_MAX_RESTORE_ERROR 1
#define RS3_VALUE_LEN (RS3_CODE_LEN - (RS3_MAX_RESTORE_ERROR * 2))

extern void InitRS3Table (void);
extern void FreeRS3Table (void);

/* Frames RS3 methods */
extern void CodeRS3Frame   (byte *dest, byte *data, dword Flags);
extern void DecodeRS3Frame (byte *dest, byte *data, dword Flags);
extern int  DecodeRS3FrameWithErrStat (byte *dest, byte *data, estat *pes, dword Flags);

/* Clusters RS3 methods */
extern void CodeRS3Cluster   (byte *dest, byte *data, dword Flags);
extern void DecodeRS3Cluster (byte *dest, byte *data, dword Flags);
extern int  DecodeRS3ClusterWithErrStat (byte *dest, byte *data, estat *pes, dword Flags);

/* Get raw data block size methods */
extern int  GetRS3FrameDataSize   (dword Flags);
extern int  GetRS3ClusterDataSize (dword Flags);

/* Get RS3 encoded data block size methods */
extern int  GetRS3FrameSize       (dword Flags);
extern int  GetRS3ClusterSize     (dword Flags);

typedef struct
{
  byte rs [RS3_CODE_LEN];
} rs3w;

#endif
