
#ifndef __BCHB_H__
#define __BCHB_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BCH1_CODE_LEN          63
#define BCH1_MAX_RESTORE_ERROR  2
#define BCH1_VALUE_LEN         51

void InitBCH1Table (void);
void FreeBCH1Table (void);

/* Frames BCH1 methods */
extern void CodeBCH1Frame   (byte *dest, byte *data, dword Flags);
extern void DecodeBCH1Frame (byte *dest, byte *data, dword Flags);
extern int  DecodeBCH1FrameWithErrStat (byte *dest, byte *data, estat *pes, dword Flags);

/* Clusters RS1 methods */
extern void CodeBCH1Cluster   (byte *dest, byte *data, dword Flags);
extern void DecodeBCH1Cluster (byte *dest, byte *data, dword Flags);
extern int  DecodeBCH1ClusterWithErrStat (byte *dest, byte *data, estat *pes, dword Flags);

/* Get raw data block size methods */
extern int  GetBCH1FrameDataSize   (dword Flags);
extern int  GetBCH1ClusterDataSize (dword Flags);

/* Get BCH1 encoded data block size methods */
extern int  GetBCH1FrameSize       (dword Flags);
extern int  GetBCH1ClusterSize     (dword Flags);

#ifdef __cplusplus
}
#endif

#endif
