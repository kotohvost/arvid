
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/msg.h>

#include "defines.h"
#include "dprint.h"
#include "arvid.h"
#include "mtime.h"
#include "vcr.h"
#include "ecc.h"
#include "ini.h"
#include "pt.h"

#include "driver_thread.h"
#include "tape.h"

/* Fill cst.
 * Flags == IDENT_WITH_REW
 * 1. Rewind on -(pvcr->svcr->IdentRew >> 2) seconds
 * 2. Set play VHS mode
 * 3. Read sectors untill find data
 * 4. Set stop VHS mode
 * 5. Return frame id or
 *    E_WRITE_TO_MAIN_QUEUE,
 *    E_READ_FROM_DRV_QUEUE,
 *    E_NOT_POSITION
 *
 * Flags == IDENT_QUICK
 * 1. Set play VHS mode
 * 2. Read sectors untill find data
 * 3. Set stop VHS mode
 * 4. Return frame id or
 *    E_WRITE_TO_MAIN_QUEUE,
 *    E_READ_FROM_DRV_QUEUE,
 *    E_NOT_POSITION
 */
int IdentifyTape (vcr *pvcr, int Flags)
{
  word beg_mode = pvcr->curmode, avd_mode;
  int cnt, tray_count = retrain_count, probe_count = 10;
  
  if (Flags == IDENT_WITH_REW)
    MoveTape (pvcr, -(pvcr->svcr->IdentRew >> 2), PLAY); /* to do */
    //MoveTape (pvcr, -60, PLAY); /* to do */
  else
    SetVHSMode (pvcr, PLAY);

  SetReadMode (pvcr->unit, avdpar[pvcr->unit].phase, avdpar[pvcr->unit].sub_phase, avdpar[pvcr->unit].auto_phase);
  char msg[MAX_FRAME_SIZE + sizeof (command)];
  command *pcom = (command*)&msg;

  while (1)
   {
    pcom->mtype = 1;
    pcom->com  = O_RDONLY;
    pcom->unit = pvcr->unit;
    pcom->tray = 25;
    pcom->mode = avd_mode = 'l';
    pcom->hdr.size = ID_ECC_DATA << 12;

    if (msgsnd (msgmain, &msg, sizeof (command), 0) < 0)
     {
      dprintf (1, "MT:IdentifyTape:Write error to main queue, handle %i!\n", msgmain);
      SetVHSMode (pvcr, beg_mode);
      return E_WRITE_TO_MAIN_QUEUE;
     }

    if ((cnt = msgrcv (msgdrv, &msg, sizeof (msg), 0, 0)) <= 0)
     {
      dprintf (1, "MT:IdentifyTape:Read error from drv queue!\n");
      SetVHSMode (pvcr, beg_mode);
      return E_READ_FROM_DRV_QUEUE;
     }
    else
     {
      if (pcom->com == AVT_DATA_FRAME_NOT_FOUND_COMMAND)
       {
        pcom->mtype = 1;
        pcom->com  = O_RDONLY;
	pcom->unit = pvcr->unit;
	pcom->tray = 25;
	pcom->mode = avd_mode = 'h';
	pcom->hdr.size = ID_ECC_DATA << 12;

        if (msgsnd (msgmain, &msg, sizeof (command), 0) < 0)
         {
          dprintf (1, "MT:IdentifyTape:Write error to main queue, handle %i!\n", msgmain);
          SetVHSMode (pvcr, beg_mode);
          return E_WRITE_TO_MAIN_QUEUE;
         }

        if ((cnt = msgrcv (msgdrv, &msg, sizeof (msg), 0, 0)) <= 0)
         {
          dprintf (1, "MT:IdentifyTape:Read error from drv queue!\n");
          SetVHSMode (pvcr, beg_mode);
          return E_READ_FROM_DRV_QUEUE;
         }
        else
         {
          if (pcom->com == AVT_DATA_FRAME_NOT_FOUND_COMMAND)
           { /* out of write area */
            if (tray_count)
	     {
	      if (probe_count)
	       {
		probe_count--;
		continue;		
	       }
	      else
	        probe_count = 10; /* 10 seconds find data in two speeds */
	      
	      tray_count--;
	      
	      if (tray_count)
	        MoveTape (pvcr, -(pvcr->svcr->IdentRew >> 2), PLAY); /* to do */
		
	      continue;
	     }
	    else
	     {
              dprintf (3, "MT:IdentifyTape:Get AVT_DATA_FRAME_NOT_FOUND!\n");
              SetVHSMode (pvcr, beg_mode);
	      return E_NOT_POSITION;
	     }
	   }
	  else
	   { /* Found data frame on high speed */
	    /* dprintf (5, "Found data frame on high speed!\n"); */
            break;
	   }
	 } /* end else */
       }
      else
       { /* Found data frame on low speed */
        /* dprintf (5, "Found data frame on low speed!\n"); */
        break;
       }
     } /* end else */
   } /* end while */
   
  memset (&CST[pvcr->unit], 0, sizeof (cst));
  CST[pvcr->unit].trg_frame = CST[pvcr->unit].cur_frame = pcom->hdr.id;
  CST[pvcr->unit].mode = avd_mode;

  /* dprintf (5, "beg_mode %i.\n", beg_mode); */
  SetVHSMode (pvcr, beg_mode);
  CST[pvcr->unit].vcr_state = beg_mode;
  return pcom->hdr.id;
}

/* On mounted tape do this
 * used PT, CST and modify it
 * 1. Move tape on target frame position
 * 2. Set play VHS mode 
 * 3. Read data frame untill find data
 * 4. Set end_mode VHS mode
 * 5. Return target frame id or
 *    E_INVALID_PARAM,
 *    E_BAD_PT,
 *    E_OUT_WRITE_AREA,
 *    E_WRITE_TO_MAIN_QUEUE,
 *    E_READ_FROM_DRV_QUEUE,
 *    E_NOT_POSITION,
 *    E_NOT_IDENTIC_TPB
 */
int PositionTape (vcr *pvcr, dword cur_frame, dword trg_frame, int end_mode)
{
  if (!cur_frame || !trg_frame)
    return E_INVALID_PARAM;

  /* check on mount tape */
  if (!PT[pvcr->unit] || !CST[pvcr->unit].cur_frame)
    return E_BAD_PT;

  /* if previous read sector currently on the target position */
  if (CST[pvcr->unit].cur_frame == trg_frame) return CST[pvcr->unit].cur_frame;

  word cur_time = GetFrameTime (pvcr->unit, cur_frame);
  word trg_time = GetFrameTime (pvcr->unit, trg_frame);

  /* check on out write area */
  if (cur_time == (word)ERROR || trg_time == (word)ERROR)
    return E_OUT_WRITE_AREA;
  
  word beg_mode = pvcr->curmode;
  int cnt, tray_count = retrain_count;

  short delta_time = trg_time - cur_time;
  printf("\nDeltaTime = %d\n", delta_time);
  if (delta_time > 5 || delta_time < 0)
    MoveTape (pvcr, delta_time - 5, PLAY);
    
  SetVHSMode (pvcr, PLAY);
  SetReadMode (pvcr->unit, avdpar[pvcr->unit].phase, avdpar[pvcr->unit].sub_phase, avdpar[pvcr->unit].auto_phase);
  char msg[MAX_FRAME_SIZE + sizeof (command)];
  command *pcom = (command*)&msg;

  while (1)
   {
    pcom->mtype = 1;
    pcom->com = O_RDONLY;
    pcom->unit = pvcr->unit;
    pcom->tray = PAL_FPS * timeout;
    pcom->mode = CST[pvcr->unit].mode;
    pcom->hdr.size = ID_ECC_DATA << 12;

    if (msgsnd (msgmain, &msg, sizeof (command), 0) < 0)
     {
      dprintf (1, "MT:PositionTape:Write error to main queue, handle %i!\n", msgmain);
      SetVHSMode (pvcr, beg_mode);
      return E_WRITE_TO_MAIN_QUEUE;
     }

    if ((cnt = msgrcv (msgdrv, &msg, sizeof (msg), 0, 0)) <= 0)
     {
      dprintf (1, "MT:PositionTape:Read error from drv queue!\n");
      SetVHSMode (pvcr, beg_mode);
      return E_READ_FROM_DRV_QUEUE;
     }
    else
     {
      if (pcom->com == AVT_DATA_FRAME_NOT_FOUND_COMMAND)
       {
        if (tray_count)
	 { /* out of write area */
	  MoveTape (pvcr, -30, PLAY); /* move back */
	  tray_count--;
	  continue;
	 }
	else
	 {
          dprintf (3, "MT:PositionTape:Get AVT_DATA_FRAME_NOT_FOUND!\n");
          SetVHSMode (pvcr, beg_mode);
	  return E_NOT_POSITION;
	 }
       }
      else
       { /* found old record or new tape */
	if (pcom->hdr.volume != PT[pvcr->unit]->volume)
         {
	  MoveTape (pvcr, -30, PLAY); /* move back */
	  tray_count--;
	 }
	 
        /* find data -> set tray count */
        tray_count = retrain_count;
	
        CST[pvcr->unit].cur_frame = pcom->hdr.id;
	CST[pvcr->unit].cur_time = GetFrameTime (pvcr->unit, pcom->hdr.id);
	dprintf (3, "MT:PositionType:hdr id: %i\n", pcom->hdr.id);

        /* target found */
        printf("trg_frame %d\n", trg_frame);
        if (pcom->hdr.id == trg_frame)
	 {
	  CST[pvcr->unit].trg_frame = CST[pvcr->unit].cur_frame;
	  CST[pvcr->unit].trg_time  = CST[pvcr->unit].cur_time;
          break;
	 } 
	
        delta_time = trg_time - (short)CST[pvcr->unit].cur_time;
        printf("\nDeltaTime = %d\n", delta_time);
        if (delta_time > 5 || delta_time < 0)
          MoveTape (pvcr, delta_time - 5, PLAY);
       }
     }
   }
   
  if (pcom->hdr.volume != PT[pvcr->unit]->volume)
   {
    dprintf (1, "MT:PositionTape:Volume: %Xh not be mounted!\n", pcom->hdr.volume);
    SetVHSMode (pvcr, beg_mode);
    return E_NOT_IDENTIC_TPB;
   }

  SetVHSMode (pvcr, end_mode);
  CST[pvcr->unit].vcr_state = pvcr->curmode;
  return CST[pvcr->unit].trg_frame;
}

/* Pause in microseconds(mks) */
void WritePause (vcr *pvcr, __useconds_t pause_mks)
{
  char msg[MAX_FRAME_SIZE + sizeof (command)];
  command *pcom = (command*)&msg;
  
  /* Prepare raw frame */
  memset (pcom + 1, 0xAA, MAX_FRAME_SIZE);
  
  pcom->mtype = 1;
  pcom->com   = O_WRONLY;
  pcom->unit  = pvcr->unit;
  pcom->hdr.size = ID_RAW_FRAME << 12;
  
  if (CST[pvcr->unit].mode != 'h'
   && CST[pvcr->unit].mode != 'l')
    pcom->mode = 'l';
  else
    pcom->mode = CST[pvcr->unit].mode;

  /* dprintf (5, "Write pause mode: %c\n", CST[pvcr->unit].mode); */
  if (msgsnd (msgmain, &msg, sizeof (msg), 0) < 0)
    dprintf (1, "MT:WritePause:Write error to main queue, handle %i!\n", msgmain);

  usleep (pause_mks);
  CST[pvcr->unit].cur_time += (float)pause_mks / (float)ONE_SECOND;
  CST[pvcr->unit].lastwr_time = get_time ();
}

/* RecMode == REC_NO_DIR_CHECK
 * 1. PositionTape afterframe
 * 2. Set "Record" VHS mode
 * 3. Write gap frames with afterframe + 1
 * 4. Return true on success or
 *    E_BAD_PT,
 *    PositionTape errors codes
 *
 * RecMode == REC_CURRENT_PLACE
 * 1. Set "Record" VHS mode
 * 2. Write gap frames with afterframe + 1
 * After every write frame update PT: cur_tpb->cur_time + 1/PAL_FPS
 * After play-record add cur_tpb->cur_time + timePlayRecord
 * 3. Return true on success or
 *    E_BAD_PT,
 *    PositionTape errors codes
 */
int WriteStart (vcr *pvcr, int RecMode, dword afterframe)
{
  int rc = true;

  /* check on mount tape */
  if (!PT[pvcr->unit] || !CST[pvcr->unit].cur_frame)
    return E_BAD_PT;

  if (RecMode != REC_CURRENT_PLACE)
   {
    if ((rc = PositionTape (pvcr, CST[pvcr->unit].cur_frame, afterframe, STOP)) < 0)
     {
      dprintf (3, "MT:WriteStart:Cann't position on frame %d.\n", afterframe);
      return rc;
     }
   }
    
  SetVHSMode (pvcr, REC);
  CST[pvcr->unit].vcr_state = pvcr->curmode;
  CST[pvcr->unit].lastwr_time = get_time ();
  dprintf (1, "MT:WriteStart:CST[unit].lastwr_time %lu\n", CST[pvcr->unit].lastwr_time);
  
  WritePause (pvcr, ONE_SECOND / PAL_FPS); /* write one frame pause for setup arvid to data mode */
  return rc; /* success */
}

/* Return last frame id */
int WriteData (vcr *pvcr, void *buf, dword beg_frame, dword num)
{
  dword Flags = 0, frame_counter = 0, data_size = 0, cur_time;
  char msg[MAX_FRAME_SIZE + sizeof (command)];
  command *pcom = (command*)&msg;
  int fr_type;
  
  pcom->mtype = 1;
  pcom->com   = O_WRONLY;
  pcom->unit  = pvcr->unit;
  pcom->mode  = CST[pvcr->unit].mode;
  pcom->hdr.id = beg_frame;
  pcom->hdr.volume = PT[pvcr->unit]->volume;
  
  if (CST[pvcr->unit].mode == 'h')
    Flags |= ARVID1052_FLAG;

  /* Take frame type by ecc type */
  fr_type = ECCTypeToFrameID (PT[pvcr->unit]->flags);
  
  if (fr_type == ERROR)
    fr_type = GOLEY_FLAG;

  data_size = GetBlockDataSize (Flags | (PT[pvcr->unit]->flags & ECC_MASK));
  pcom->hdr.size = (fr_type << 12) | (data_size >> 1);
  
  /* correct time */
  cur_time = get_time ();
  CST[pvcr->unit].trg_time = CST[pvcr->unit].cur_time += (float)(cur_time - CST[pvcr->unit].lastwr_time) / 1000.0;
  CST[pvcr->unit].lastwr_time = cur_time;

  while (frame_counter < num)
   {
    memcpy (pcom + 1, buf, data_size); /* buf corrected bottom after successfull msgsnd () */
    
    if (msgsnd (msgmain, &msg, sizeof (msg), 0) < 0)
     {
      dprintf (1, "MT:WriteData:Write error to main queue, handle %i!\n", msgmain);
      continue;
     }
     
    buf = (char*)buf + data_size;
    pcom->hdr.id++;
    frame_counter++;
   }

  frame_counter = 0;

  /* receive completion commands */
  while (frame_counter < num)
   {
    int cnt;
    pcom->mtype = 1;
   
    if ((cnt = msgrcv (msgdrv, pcom, sizeof (command), 0, 0)) <= 0)
     {
      dprintf (1, "MT:WriteData:Read error from drv queue, handle %i!\n", msgdrv);
      continue;
     }

    /* correct time */
    CST[pvcr->unit].trg_time = CST[pvcr->unit].cur_time += (float)(pcom->tray - CST[pvcr->unit].lastwr_time) / 1000.0;
    CST[pvcr->unit].lastwr_time = pcom->tray;
    
    printf ("CST[pvcr->unit].cur_time %f sec\n", CST[pvcr->unit].cur_time);
    
    CST[pvcr->unit].trg_frame = CST[pvcr->unit].cur_frame = beg_frame;
    UpdatePT (pvcr->unit, beg_frame++, (dword)CST[pvcr->unit].cur_time);
    frame_counter++;
   }

  return --beg_frame; /* success */
}

void WriteEnd (vcr *pvcr)
{
  /* my stupid VHS player doesn't stoped */
  SetVHSMode (pvcr, STOP);
  CST[pvcr->unit].vcr_state = pvcr->curmode;
}

/* Return number reading frames or
 *    E_WRITE_TO_MAIN_QUEUE,
 *    E_READ_FROM_DRV_QUEUE,
 *    E_NOT_POSITION,
 *    E_NOT_IDENTIC_TPB,
 *    PositionTape errors codes
 */
int ReadData (vcr *pvcr, void *buf, dword beg_frame, dword num, estat *err)
{
  int rc = 0, cnt, tray_count = retrain_count, reading_frames = 0, Flags = 0, fr_type;
  char msg[MAX_FRAME_SIZE + sizeof (command)];
  command *pcom = (command*)&msg;

  printf("Read data : Entry\n");

  SetReadMode (pvcr->unit, avdpar[pvcr->unit].phase, avdpar[pvcr->unit].sub_phase, avdpar[pvcr->unit].auto_phase);
  
  if (beg_frame)
   {
    if ((rc = PositionTape (pvcr, CST[pvcr->unit].cur_frame, beg_frame - 1, PLAY)) < 0)
     {
      dprintf (3, "MT:ReadData:Cann't position on frame %d.Error %d\n", beg_frame,rc);
      return rc;
     }

    //MoveTape (pvcr, -5, PLAY); /* move back */
    memset (err, 0, sizeof (estat));
   }

  while (num)
  {
    printf("Read data : Read loop\n");
    pcom->mtype = 1;
    pcom->com  = O_RDONLY;
    pcom->unit = pvcr->unit;
    pcom->tray = PAL_FPS * timeout;
    pcom->mode = CST[pvcr->unit].mode;
    pcom->hdr.id = beg_frame;
    pcom->hdr.volume = PT[pvcr->unit]->volume;
    
    if (CST[pvcr->unit].mode == 'h')
      Flags = ARVID1052_FLAG;

    /* Take frame type by ecc type */
    fr_type = ECCTypeToFrameID (PT[pvcr->unit]->flags);
    if (fr_type == ERROR)
      fr_type = GOLEY_FLAG;
    pcom->hdr.size = (fr_type << 12) | (GetBlockDataSize (Flags | (PT[pvcr->unit]->flags & ECC_MASK)) >> 1);
      
    if (msgsnd (msgmain, &msg, sizeof (command), 0) < 0)
     {
      dprintf (1, "MT:ReadData:Write error to main queue, handle %i!\n", msgmain);
      SetVHSMode (pvcr, STOP);
      return E_WRITE_TO_MAIN_QUEUE;
     }

    if ((cnt = msgrcv (msgdrv, &msg, sizeof (msg), 0, 0)) <= 0)
     {
      dprintf (1, "MT:ReadData:Read error from drv queue!\n");
      SetVHSMode (pvcr, STOP);
      return E_READ_FROM_DRV_QUEUE;
     }
    else
     {
      if (pcom->com == AVT_DATA_FRAME_NOT_FOUND_COMMAND)
      {
        printf("Read data : AVT_DATA_FRAME_NOT_FOUND_COMMAND\n");
        if (tray_count)
	 { /* out of write area */
	  MoveTape (pvcr, -30, PLAY); /* move back */
	  tray_count--;
	  continue;
	 }
	else
	 {
          dprintf (3, "MT:ReadData:Get AVT_DATA_FRAME_NOT_FOUND!\n");
	  SetVHSMode (pvcr, STOP);
	  return E_NOT_POSITION;
	 }
       }
      else
       { /* found old record or new tape */
	if (pcom->hdr.volume != PT[pvcr->unit]->volume)
        {
          printf("Read data : pcom->hdr.volume != PT[pvcr->unit]->volume\n");
	  MoveTape (pvcr, -30, PLAY); /* move back */
	  tray_count--;
	 }
	 
        /* find data -> set tray count */
        tray_count = retrain_count;

        CST[pvcr->unit].cur_frame = pcom->hdr.id;
        CST[pvcr->unit].cur_time  = GetFrameTime (pvcr->unit, pcom->hdr.id);

        printf("Read data : CST[pvcr->unit].cur_time %f CST[pvcr->unit].cur_frame %d\n",
               CST[pvcr->unit].cur_time,CST[pvcr->unit].cur_frame);

        /* target found */
	if (pcom->hdr.id == beg_frame)
        {
          printf("Read data : success find frame\n");
	  CST[pvcr->unit].trg_frame = CST[pvcr->unit].cur_frame;
	  CST[pvcr->unit].trg_time  = CST[pvcr->unit].cur_time;

	  memcpy (buf, pcom + 1, GetBlockDataSize (Flags));
	  err->OrdinaryErr += pcom->err.OrdinaryErr;
	  err->DupletErr   += pcom->err.DupletErr;
	  err->TripletErr  += pcom->err.TripletErr;
	  err->BadFrames   += pcom->err.BadFrames;
	  buf += GetBlockDataSize (Flags);

	  reading_frames++;
	  beg_frame++;
	  num--;
	 } 
	
            word trg_time = GetFrameTime (pvcr->unit, beg_frame);
            printf("Read data : trg_time %d\n",trg_time);
	if (trg_time == (word)ERROR)
	 {
	  SetVHSMode (pvcr, STOP);
	  return reading_frames;
	 }

        short delta_time = trg_time - CST[pvcr->unit].cur_time;
        printf("Read data : delta_time %d\n",delta_time);
        if (delta_time > 5 || delta_time < 0)
          MoveTape (pvcr, delta_time - 5, PLAY);
       }
     }
   }

  if (pcom->hdr.volume != PT[pvcr->unit]->volume)
   {
    dprintf (3, "MT:ReadData:Volume: %Xh not be mounted!\n", pcom->hdr.volume);
    SetVHSMode (pvcr, STOP);
    return E_NOT_IDENTIC_TPB;
   }

  return reading_frames; /* success */
}

void ReadEnd (vcr *pvcr)
{
  SetVHSMode (pvcr, STOP);
  CST[pvcr->unit].vcr_state = pvcr->curmode;
}

void Rewind (vcr *pvcr)
{
  /* protect for set play-rewind mode */
  if (!CheckStop (pvcr))
    SetVHSMode (pvcr, STOP);

  /* set stop mode */    
  CST[pvcr->unit].vcr_state = pvcr->curmode;
  SetVHSMode (pvcr, REW);
}

void Forward (vcr *pvcr)
{
  /* protect for set play-rewind mode */
  if (!CheckStop (pvcr))
    SetVHSMode (pvcr, STOP);

  /* set stop mode */    
  CST[pvcr->unit].vcr_state = pvcr->curmode;
  SetVHSMode (pvcr, FF);
}
