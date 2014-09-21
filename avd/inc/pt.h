
#ifndef __PT_H__
#define __PT_H__

/* Tape Parametr Block Flags */
#define LP 0x8000 /* OR mask for load LP vcr file */
#define SP 0x7FFF /* AND mask for load SP vcr file */

/* Tape Parametr Block */
typedef struct
{
  dword volume;    /* ID */
  word time;       /* tapes length in seconds */
  word flags;      /* format flags (ECC, density, block/cluster and SP/LP) */
   /* ref. to ecc.h for choosing combination ECC and density flags */
  char name [MAX_PATH]; /* ASCIIZ tape name */
  dword beg_frame; /* set on tape create, >0 */
  dword end_frame; /* update after each write operation */
  float radius;    /* tape babins radius */
  float thickness; /* tape thickness */
  float tape_len;  /* tape lenght */
} tpb;

/* Current State of Tape, fill by IdentifyTape and SetTape */
typedef struct
{
  float cur_time;  /* current time at the tape begin, seconds */
  float trg_time;  /* target position, seconds */
  dword cur_frame; /* current frame > 0 */
  dword trg_frame; /* target frame */
  dword lastwr_time; /* last write operation time */
  word  vcr_state; /* current VHS state */
  byte  mode;      /* mode:h,l set after IdentifyTape */
} cst;

extern cst CST[]; /* Pointers on Position Tables for MAX_AVT boards */
extern tpb* PT[]; /* Current states of MAX_AVT tapes */

extern int GetPTLengthByTime (word time); /* time in seconds */
extern int GetPTLength (int unit);

/* Get filled PT length */
extern int GetActualPTLength (tpb *ptpb);

/* Check on tape mounting */
extern int IsTape (int unit);

extern cst *GetCST (int unit);

/* Create new PT for new tape */
extern tpb *CreatePT (int unit, word time, word flags, char *name); /* time in seconds */

/* Create new tape */
extern tpb *SetTape (int unit, tpb *ptpb, byte avd_mode);

/* Mount tape, untill tape reading */
extern tpb *SetPT (int unit, tpb *ptpb);

/* Save PT on dismount, after tape recording */
extern tpb *GetPT (int unit, tpb *ptpb);

/* Save PT to file */
extern void SavePT (int unit, char *filename);

/* Restore PT from file */
extern void LoadPT (int unit, char *filename);

/* Unmounting tape - reset CST[unit], PT[unit] */
extern void ResetTape (int unit);

/* Returns frames position time */
extern word GetFrameTime (int unit, dword frame);

/* Returns frames ID */
extern dword GetFrameByTime (int unit, word time); /* time in seconds */

/* Update after each write operation */
extern dword UpdatePT (int unit, dword frame, word time); /* time in seconds */

/* For debug purposes */
extern void PrintPT (int unit);
extern void PrintCST (int unit);

#endif
