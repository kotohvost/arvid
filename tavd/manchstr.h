#ifndef __MANCHSTR_H__
#define __MANCHSTR_H__

extern void  InitManchTable (void);
extern void  FreeManchTable (void);
extern void  CodeManchesterCluster (byte *data);
extern void  DecodeManchesterCluster (byte *data);

extern word *SpeedLowTabl;
extern char *SpeedHighTabl;

#define ManchCName    "TABLET/mancctbl.bin"
#define ManchDName    "TABLET/mancdtbl.bin"
#define SpeedHighName "TABLET/shightbl.bin"
#define SpeedLowName  "TABLET/slowtbl.bin"

#endif
