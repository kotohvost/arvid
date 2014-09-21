
#ifndef __CHC_H__
#define __CHC_H__

extern void InitCHCTable (void);
extern void FreeCHCTable (void);

/* Scramblers methods */
extern void Scramble   (byte* data, dword len);
extern void DeScramble (byte* data, dword len);

/* Manchester methods */
extern void CodeManchesterBlock   (byte *data, dword Len);
extern void DecodeManchesterBlock (byte *data, dword Len);

extern word *SpeedLowTabl;
extern byte *SpeedHighTabl;

/* If folder TABLES exists, tables be loaded from there */
#define ManchCName    "TABLET/mancctbl.bin"
#define ManchDName    "TABLET/mancdtbl.bin"
#define SpeedHighName "TABLET/shightbl.bin"
#define SpeedLowName  "TABLET/slowtbl.bin"

#endif
