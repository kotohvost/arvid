
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>

#include "defines.h"
#include "dprint.h"
#include "arvid.h"
#include "mtime.h"
#include "vcr.h"
#include "ecc.h"
#include "ini.h"
#include "tape.h"
#include "pt.h"

/* modify after each write frame */
tpb* PT[MAX_AVT] = {NULL, NULL, NULL, NULL}; /* Pointers on Position Tables for MAX_AVT boards */
cst CST[MAX_AVT]; /* Current states of MAX_AVT tapes */

int GetPTLengthByTime (word sec)
{
  return (sizeof (word) * sec * PAL_FPS + sizeof (tpb));
}

int GetPTLength (int unit)
{
  if (PT[unit])
    return (sizeof (word) * PT[unit]->time * PAL_FPS + sizeof (tpb));
  else
    return false;
}

/* Get filled PT length */
int GetActualPTLength (tpb *ptpb)
{
  return (sizeof (word) * (ptpb->end_frame - ptpb->beg_frame) + sizeof (tpb));
}

/* Check on tape mounting */
int IsTape (int unit)
{
  if (PT[unit])
    return true;
  else
    return false;
}

/* Create PT for new tape */
tpb* CreatePT (int unit, word time, word flags, char *name)
{
  tpb *ptpb = PT[unit];

  if (ptpb)
   {
    free (ptpb);
    PT[unit] = NULL;
   }
    
  int PTlen = GetPTLengthByTime (time);
  PT[unit] = ptpb = malloc (PTlen);
  
  if (!ptpb)
    return NULL;
    
  memset (ptpb, 0xFF, PTlen);
  memset (ptpb, 0, sizeof (tpb));
  
  ptpb->time = time;
  ptpb->flags = flags;
  ptpb->volume = get_time () ^ time ^ flags;
  ptpb->radius = 0.0254 / 2; /* tape babins radius */
  ptpb->thickness = 0.0000175; /* tape thickness */
  ptpb->tape_len = time * avdpar[unit].velosity; /* tape lenght */
  ptpb->beg_frame = 1;
  ptpb->end_frame = 1;
  
  if (strlen (name) < MAX_PATH)
    strcpy (ptpb->name, name);
  else
   {
    memcpy (ptpb->name, name, MAX_PATH - 1);
    ptpb->name[MAX_PATH - 1] = 0;
   }    

  return PT[unit]; /* success */
}

/* Create tape: tpb, pt, cst */
tpb* SetTape (int unit, tpb *ptpb, byte avd_mode)
{
  memset (&CST[unit], 0, sizeof (cst));
  CST[unit].trg_frame = CST[unit].cur_frame = 1;
  CST[unit].mode = avd_mode;
  
  return CreatePT (unit, ptpb->time, ptpb->flags, ptpb->name);
}

cst *GetCST (int unit)
{
  if (CST[unit].cur_frame || CST[unit].mode)
    return &CST[unit];
  else
    return false;
}

/* Unmounting tape */
void ResetTape (int unit)
{
  if (PT[unit])
   {
    free (PT[unit]);
    PT[unit] = NULL;
   }
    
  memset (&CST[unit], 0, sizeof (cst));
}

/* Mount tape, untill tape reading */
tpb *SetPT (int unit, tpb *ptpb)
{
  if (PT[unit])
    free (PT[unit]);

  PT[unit] = malloc (GetPTLengthByTime (ptpb->time));
  if (!PT[unit])
    return NULL;

//  memset (ptpb, 0xFF, GetPTLengthByTime (ptpb->time)); ??? what the fuck???
  memset (PT[unit], 0xFF, GetPTLengthByTime (ptpb->time));
  memcpy (PT[unit], ptpb, GetActualPTLength (ptpb));
  
  /* fill cst, CST[unit].cur_frame set after tape identify */
  CST[unit].trg_time = CST[unit].cur_time = GetFrameTime (unit, CST[unit].cur_frame);
  return PT[unit]; /* success */
}

/* Save PT on dismount, after tape recording */
tpb *GetPT (int unit, tpb *ptpb)
{
  if (!PT[unit])
    return NULL;
    
  memcpy (ptpb, PT[unit], GetActualPTLength (ptpb));
  return ptpb; /* success */
}

/* Returns frames position time */
word GetFrameTime (int unit, dword frame)
{
  word *pPT = (word*)(PT[unit] + 1);
  
  if (!PT[unit] 
    || frame < PT[unit]->beg_frame
    || PT[unit]->end_frame < frame)
    return ERROR;
  else
  {
/*    printf("\nPT[unit]->beg_frame = %d, frame - PT[unit]->beg_frame = %d\n",
           PT[unit]->beg_frame, frame - PT[unit]->beg_frame);*/

    return pPT[(frame - (PT[unit]->beg_frame))];
  }
}

/* Returns frames ID */
dword GetFrameByTime (int unit, word time)
{
  word *pPT = (word*)(PT[unit] + 1);
  
  if (!PT[unit])
    return ERROR;

  dword frame = 0;
  while (pPT[frame] != (word)ERROR && frame <= (PT[unit]->end_frame - PT[unit]->beg_frame))
   {
    if (pPT[frame] == time)
      return frame + PT[unit]->beg_frame;
    
    frame++;
   }
   
  return ERROR;
}

/* Update after each write operation */
dword UpdatePT (int unit, dword frame, word time)
{
  word *pPT = (word*)(PT[unit] + 1);
  dprintf (1, "UpdatePT:Frame %lu Time %d\n",frame,time);

  if (!PT[unit]
    || frame <  PT[unit]->beg_frame
    || frame > (PT[unit]->end_frame + 1)
    || (frame - PT[unit]->beg_frame) / PAL_FPS > PT[unit]->time)
    return ERROR;

  if (frame == PT[unit]->end_frame + 1)
    PT[unit]->end_frame++;
    
  pPT[frame - PT[unit]->beg_frame] = time;
  return frame;
}

/* For debug purposes */
void PrintPT (int unit)
{
  if (!PT[unit])
   {
    dprintf (1, "PrintPT:Tape not mounted!\n");
    return;
   }
   
  dprintf (3, "PT:volume:\t%08Xh\n"
	  "PT:time:\t%d\n"
	  "PT:flags:\t%04Xh\n"
	  "PT:name:\t%s\n"
	  "PT:beg_frame:\t%d\n"
	  "PT:end_frame:\t%d\n"
	  "PT:radius:\t%f\n"
	  "PT:thickness:\t%f\n"
	  "PT:tape_len:\t%f\n"
	  "PT: ",
	  PT[unit]->volume, PT[unit]->time,      PT[unit]->flags,
	  PT[unit]->name,   PT[unit]->beg_frame, PT[unit]->end_frame,
	  PT[unit]->radius, PT[unit]->thickness, PT[unit]->tape_len);

  short *pPT = (short*)(PT[unit] + 1);
  int i = 0;
  
  while (*pPT != ERROR && i < PT[unit]->time * PAL_FPS)
   {
    printf ("%i ", *pPT++);
    i++;
   }
   
  dprintf (3, "\n===================== END PT ========================\n");
}

void PrintCST (int unit)
{
  dprintf (3, "CST:cur_time:\t%f\n"
	  "CST:trg_time:\t%f\n"
	  "CST:cur_frame:\t%d\n"
	  "CST:trg_frame:\t%d\n"
	  "CST:lastwr_time:\t%d\n"
	  "CST:vcr_state:\t%d\n"
	  "CST:mode:\t%c\n",
	  CST[unit].cur_time,  CST[unit].trg_time,    CST[unit].cur_frame,
	  CST[unit].trg_frame, CST[unit].lastwr_time, CST[unit].vcr_state,
	  CST[unit].mode);
}

/* Save PT to file */
void SavePT (int unit, char *filename)
{
  if (!PT[unit])
   {
    dprintf (1, "SavePT:Tape not mounted!\n");
    return;
   }

  int hFile = open (filename, O_CREAT | O_WRONLY | O_BINARY, FILE_ACCESS);
  if (hFile == ERROR)
    return;

  write (hFile, PT[unit], GetActualPTLength (PT[unit]));
  close (hFile);
}

/* Restore PT from file */
void LoadPT (int unit, char *filename)
{
  int hFile = open (filename, O_RDWR | O_BINARY, FILE_ACCESS);
  if (hFile == ERROR)
    return;
    
  int PTLen = lseek (hFile, 0, SEEK_END);
  lseek (hFile, 0, SEEK_SET);
   
  tpb *ptpb = (tpb*)malloc (PTLen);
  if (!ptpb)
   {
    close (hFile);
    return;    
   }

  read (hFile, ptpb, PTLen);
  close (hFile);
  
  SetPT (unit, ptpb);
  free (ptpb);
}
