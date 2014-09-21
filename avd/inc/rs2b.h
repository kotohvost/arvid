
#ifndef __RS2B_H__
#define __RS2B_H__

#define RS2_CODE_LEN         15
#define RS2_MAX_RESTORE_ERROR 2
#define RS2_VALUE_LEN (RS2_CODE_LEN - (RS2_MAX_RESTORE_ERROR * 2))

extern void InitRS2Table (void);
extern void FreeRS2Table (void);

/* Frames RS2 methods */
extern void CodeRS2Frame   (byte *dest, byte *data, dword Flags);
extern void DecodeRS2Frame (byte *dest, byte *data, dword Flags);
extern int  DecodeRS2FrameWithErrStat (byte *dest, byte *data, estat *pes, dword Flags);

/* Clusters RS2 methods */
extern void CodeRS2Cluster   (byte *dest, byte *data, dword Flags);
extern void DecodeRS2Cluster (byte *dest, byte *data, dword Flags);
extern int  DecodeRS2ClusterWithErrStat (byte *dest, byte *data, estat *pes, dword Flags);

/* Get raw data block size methods */
extern int  GetRS2FrameDataSize   (dword Flags);
extern int  GetRS2ClusterDataSize (dword Flags);

/* Get RS2 encoded data block size methods */
extern int  GetRS2FrameSize       (dword Flags);
extern int  GetRS2ClusterSize     (dword Flags);

typedef struct
{
  byte rs [RS2_CODE_LEN];
} rs2w;

#endif
