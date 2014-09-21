
#ifndef __RSB_H__
#define __RSB_H__

#define RS1_CODE_LEN         15
#define RS1_MAX_RESTORE_ERROR 3
#define RS1_VALUE_LEN (RS1_CODE_LEN - (RS1_MAX_RESTORE_ERROR * 2))

extern void InitRS1Table (void);
extern void FreeRS1Table (void);

/* Frames RS1 methods */
extern void CodeRS1Frame   (byte *dest, byte *data, dword Flags);
extern void DecodeRS1Frame (byte *dest, byte *data, dword Flags);
extern int  DecodeRS1FrameWithErrStat (byte *dest, byte *data, estat *pes, dword Flags);

/* Clusters RS1 methods */
extern void CodeRS1Cluster   (byte *dest, byte *data, dword Flags);
extern void DecodeRS1Cluster (byte *dest, byte *data, dword Flags);
extern int  DecodeRS1ClusterWithErrStat (byte *dest, byte *data, estat *pes, dword Flags);

/* Get raw data block size methods */
extern int  GetRS1FrameDataSize   (dword Flags);
extern int  GetRS1ClusterDataSize (dword Flags);

/* Get RS1 encoded data block size methods */
extern int  GetRS1FrameSize       (dword Flags);
extern int  GetRS1ClusterSize     (dword Flags);

typedef struct
{
  byte rs [RS1_CODE_LEN];
} rsw;

#endif
