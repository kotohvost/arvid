
#ifndef __GOLEY_H__
#define __GOLEY_H__

#define G1 0x0C75 /* first base polynum */
#define G2 0x0AE3 /* second base polynum */
#define G1_CODE_LEN  23 /* encoded data length */
#define G1_VALUE_LEN 12 /* raw data length */
#define G1_MAX_RESTORE_ERROR 3 /* recovery any 3 errors */
#define G1_POLYNUM_LEN G1_VALUE_LEN
#define G1_LAST_LEN  (G1_CODE_LEN - G1_VALUE_LEN) /* syndromus length*/

extern void  InitGoleyTable (void);
extern void  FreeGoleyTable (void);

/* binary Goley methods */
extern dword eciGoleyCoder (word Data);
extern word  eciGoleyDecoder (dword Code);
extern word  eciGoleyDecoderWithErrStat (dword Code, dword *ErrCounter);

/* Frames Goley methods */
extern void  CodeGoleyFrame (byte *dest, byte *data, dword Flags);
extern void  DecodeGoleyFrame (byte *dest, byte *data, dword Flags);
extern int   DecodeGoleyFrameWithErrStat (byte *dest, byte *data, estat *pes, dword Flags);

/* Clusters Goley methods */
extern void  CodeGoleyCluster (byte *dest, byte *data, dword Flags);
extern void  DecodeGoleyCluster (byte *dest, byte *data, dword Flags);
extern int   DecodeGoleyClusterWithErrStat (byte *dest, byte *data, estat *pes, dword Flags);

/* Auxilary methods */
extern int   GetGoleyFrameDataSize (dword Flags);
extern int   GetGoleyClusterDataSize (dword Flags);
extern int   GetGoleyFrameSize (dword Flags);
extern int   GetGoleyClusterSize (dword Flags);

/* Len string = (NumTriplets * 3 * 2) + 1 */
extern void  GoleyStringCoder (byte *String, byte *Date, int NumTriplets);
extern void  GoleyStringDecoder (byte *Date, byte *String, int NumTriplets);
extern void  StringMixer (byte *String, dword Len);
extern void  StringDemixer (byte *String, dword Len);

/* For external tests */
extern dword *ErrorTable, NumElemErrorTable;

/* If folder TABLES exists, tables be loaded from there */
#define ErrorName     "TABLES/errtabl.bin"
#define NumBitsName   "TABLES/numbits.bin"
#define TestName      "TABLES/testtabl.bin"
#define LastName      "TABLES/lasttabl.bin"
#define RestName      "TABLES/resttabl.bin"

#endif
