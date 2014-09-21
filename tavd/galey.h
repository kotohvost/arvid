#ifndef __GALEY_H__
#define __GALEY_H__

extern void  InitGaleyTable (void);
extern void  FreeGaleyTable (void);
extern dword eciGaleyCoder (word Data);
extern word  eciGaleyDecoder (dword Code);
extern word  eciGaleyDecoderWithErrStat (dword Code, dword &ErrCounter);
extern void  CodeGaleyCluster (byte *data, byte *dest);
extern void  DecodeGaleyCluster (byte *data, byte *dest);
extern void  DecodeGaleyClusterWithErrStat (byte *data, byte *dest, estat *ErrStat);
extern void  BeginGALEYLockedCode ();
// Len string = (NumTriplets * 3 * 2) + 1
extern void  GaleyStringCoder (byte *String, byte *Date, int NumTriplets);
extern void  GaleyStringDecoder (byte *Date, byte *String, int NumTriplets);
extern void  StringMixer (byte *String, dword Len);
extern void  StringDemixer (byte *String, dword Len);
extern void  EndGALEYLockedCode ();
extern void  TestError (void);
extern void  ErrorExit (dword NumError);

extern dword *ErrorTable, NumElemErrorTable;

#define MAX_RESTORE_ERROR 3
#define CODE_LEN  23
#define VALUE_LEN 12
#define LAST_LEN  (CODE_LEN - VALUE_LEN)
#define TEST_DATA "0123456789abcdefghiklmnoprtquwxyzABCDEFGHIKLMNOPRSTQUWXYZ"
#define TEST_DATA2 "ABCDEFGHIKLMNOPRS"

#define ErrorName     "TABLES/errtabl.bin"
#define NumBitsName   "TABLES/numbits.bin"
#define TestName      "TABLES/testtabl.bin"
#define LastName      "TABLES/lasttabl.bin"
#define RestName      "TABLES/resttabl.bin"

#endif