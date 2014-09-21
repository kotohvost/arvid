#ifndef __AFTP_H__
#define __AFTP_H__

//-----------------------------TAVDFTP--------------------------------
// always must be in begin of cluster
struct FileHdr
{
  dword FileID; // may us file lenght plus current time
  word	HdrLen; // Include variable part
  word	NumBlock; // in cluster
};

struct BlockHdr
{
  dword Number; // 0 - file attr, > 0 - data of file
  dword CRC;	// CRC32
  dword FileOffset;
  dword ClusterOffset;
  dword Len;
};

struct FileAttr
{
  dword Size;
  dword Attr;
  dword Date;
  dword Time;
};
// On this place must be file name

#define BLOCK_LEN 0x1000
#define LEN_SEQ 10 // Cashe clusters on receive.

extern int SendFile (char *FileName);
extern int ReceiveFile (char *FileName);
#endif