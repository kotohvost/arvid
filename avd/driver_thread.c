
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <linux/errno.h>

#include "defines.h"
#include "dprint.h"
#include "arvid.h"
#include "mtime.h"
#include "avd_ioctl.h"
#include "vcr.h"
#include "chc.h"
#include "ecc.h"
#include "crc32.h"
#include "goley.h"

#include "driver_thread.h"
#include "main_thread.h"

static int avd_get_param (int hFile, avd_param *pavdp, int debug_flag)
{
  int rc = ioctl (hFile, AVT_GET_PARAM, pavdp);
  
  if (rc)
   {
    dprintf (1, "Cann't make ioctl request GET_PARAM, err %d\n!", rc);
    return rc;
   }    
  
  if (!debug_flag)
    return rc;
  
  dprintf (3, "Hardware parametrs:\n"
          "ArvidInsert: %i\n"
	  "ArvidModel: %i\n"
	  "ArvidModelDetect: %i\n"
	  "Phase: %Xh\n"
	  "SubPhase: %Xh\n"
	  "AutoPhase: %Xh\n"
	  "BasePort: %Xh\n"
	  "ArvidIRQ: %i\n"
          "ValueRK: %Xh\n"
	  "Arv105X: %Xh\n"
	  "DataMode: %Xh\n"
          "HalfDupMode: %Xh\n"
	  "HDensity: %Xh\n"
	  "FrameMask: %Xh\n"
	  "MemorySize: %i\n"
          "HArvFrame: %i\n"
	  "WArvFrame: %i\n"
	  "NumArvFrame: %i\n"
          "SizeArvFrame: %i\n\n"
          "countintr: %i\n"
          "SubFrame: %Xh\n"
          "SendFrameCount: %i\n"
          "ReceiveFrameCount: %i\n",
	  pavdp->ArvidInsert, pavdp->ArvidModel, pavdp->ArvidModelDetect,
	  pavdp->Phase, pavdp->SubPhase, pavdp->AutoPhase, pavdp->BasePort,
	  pavdp->ArvidIRQ, pavdp->ValueRK, pavdp->Arv105X, pavdp->DataMode,
	  pavdp->HalfDupMode, pavdp->HDensity, pavdp->FrameMask,
	  pavdp->MemorySize, pavdp->HArvFrame, pavdp->WArvFrame,
	  pavdp->NumArvFrame, pavdp->SizeArvFrame, pavdp->countintr,
	  pavdp->SubFrame, pavdp->SendFrameCount, pavdp->ReceiveFrameCount);

  return rc;
}

static int avd_open (int unit, int fmode, char mode)
{
  char dev_path [MAX_PATH];
  int hFile = ERROR;

  sprintf (dev_path, "/dev/%s%1i%c", MOD_NAME, unit, mode);
  dprintf (5, "DT:avd_open:dev_path %s\n", dev_path);

  if ((hFile = open (dev_path, fmode, FILE_ACCESS)) < 0)
    dprintf (1, "DT:avd_open:Cann't open device file %s, error %i!\n", dev_path, hFile);
  
  return hFile;
}

static int avd_reopen (int unit, int fmode, char mode, int hFile, avd_param *pavdp)
{
  int rc;
  
  if (hFile != ERROR)
    close (hFile);
    
  if ((hFile = avd_open (unit, fmode, mode)) < 0)
    return hFile;
   
  if ((rc = avd_get_param (hFile, pavdp, 0)) != 0)
   {
    close (hFile);
    return rc;
   }
   
  return hFile; 
}

int FrameIDToECCType (int fr_type)
{
  switch (fr_type & ID_FRAME_MASK)
   {
    case ID_G1_FRAME:   return GOLEY_FLAG;
    case ID_RS1_FRAME:  return RS1_FLAG;
    case ID_RS2_FRAME:  return RS2_FLAG;
    case ID_RS3_FRAME:  return RS3_FLAG;
    case ID_BCH1_FRAME: return BCH1_FLAG;
              default: dprintf (5, "DT:FrameIDToECCType:ECC frame type %xh not implemented!\n", fr_type);
   }

  return ERROR;
}

int ECCTypeToFrameID (int flags)
{
  switch (flags & ECC_MASK)
   {
    case GOLEY_FLAG: return ID_G1_FRAME;
    case   RS1_FLAG: return ID_RS1_FRAME;
    case   RS2_FLAG: return ID_RS2_FRAME;
    case   RS3_FLAG: return ID_RS3_FRAME;
    case  BCH1_FLAG: return ID_BCH1_FRAME;
	    default: dprintf (5, "MT:ECCTypeToFrameID:ECC flag %xh not implemented!\n", flags);
   }

  return ERROR;
}

static void MakeDataFrame (avd_param *pavdp, command *pcom, int fr_type, byte *phard_fr)
{
  byte SoftHdr [sizeof (fr_hdr) * 2 + 1];
  dword pSoftHdr = 0;

  byte ecc_fr[MAX_FRAME_SIZE], *pecc_fr;
  dword Flags = pavdp->HDensity, len = (pavdp->SizeArvFrame * 2) - (pavdp->WArvFrame * 2 * GAP_STR) - (sizeof (fr_hdr) * 2 * 2);
  dword HeadStep = len / ((sizeof (fr_hdr) * 2 * 2) + 1), cp_size, ecc_type;
  
  /* Prepare 2 strings */
  int i = pavdp->WArvFrame * 2 * GAP_STR;
  while (i--) *phard_fr++ = 0xAA;
	  
  /* Generate software header of frame. */
  pcom->hdr.cl_num = 0;
  pcom->hdr.crc_id = GetBlockCRC ((byte*)&pcom->hdr.id, sizeof (pcom->hdr.id));
  GoleyStringCoder (SoftHdr, (byte*)&pcom->hdr, sizeof (fr_hdr) / 3);
  StringMixer (SoftHdr, sizeof (fr_hdr) * 2);

  /* Get ECC type from frame ID */
  ecc_type = FrameIDToECCType (fr_type);
  if (ecc_type == (dword)ERROR)
    ecc_type = GOLEY_FLAG;
  Flags |= ecc_type;
  
  CodeBlock (ecc_fr, (byte*)(pcom + 1), Flags);
  pecc_fr = ecc_fr;
  cp_size = HeadStep;

  /* Mix frame header with ecc coded data */
  while (len)
   {
    if (len < HeadStep)
      cp_size = len;
	   
    memcpy (phard_fr, pecc_fr, cp_size);
    phard_fr += cp_size;
    pecc_fr += cp_size;
    len -= cp_size;
    
    if (pSoftHdr >= (sizeof (fr_hdr) * 2))
      continue;
	    
    *((word*)(phard_fr)) = SpeedLowTabl [SoftHdr [pSoftHdr++]];
    phard_fr += 2;
   }
}

static int GetDataFromFrame (avd_param *pavdp, command *pcom, int fr_type, byte *phard_fr)
{
  byte SoftHdr [sizeof (fr_hdr) * 2 + 1];
  dword pSoftHdr = 0;

  phard_fr += pavdp->WArvFrame * 2 * 2;
	
  byte ecc_fr[MAX_FRAME_SIZE], *pecc_fr;
  dword Flags = pavdp->HDensity, len = (pavdp->SizeArvFrame * 2) - (pavdp->WArvFrame * 2 * GAP_STR) - (sizeof (fr_hdr) * 2 * 2);
  dword HeadStep = len / ((sizeof (fr_hdr) * 2 * 2) + 1), cp_size, crc_id, ecc_type;
  fr_hdr hdr;
	  
  pecc_fr = ecc_fr;
  cp_size = HeadStep;

  /* Get frame header from ecc coded data */
  while (len)
   {
    if (len < HeadStep)
      cp_size = len;
	   
    memcpy (pecc_fr, phard_fr, cp_size);
    phard_fr += cp_size;
    pecc_fr += cp_size;
    len -= cp_size;
	    
    if (pSoftHdr >= (sizeof (fr_hdr) * 2))
      continue;
	    
    SoftHdr [pSoftHdr++] = SpeedHighTabl [*((word*)phard_fr) & 0xAAAA] & 0xFF;
    phard_fr += 2;
   }
	  
  /* Restore software header of frame. */
  StringDemixer (SoftHdr, sizeof (fr_hdr) * 2);
  GoleyStringDecoder ((byte*)&hdr, SoftHdr, sizeof (fr_hdr) / 3);
  crc_id = GetBlockCRC ((byte*)&hdr.id, sizeof (hdr.id));
	  
  if (crc_id != hdr.crc_id)
   {
    /* dprintf (5, "DT:GetDataFromFrame:Header corrupted, frame hasn't data.\n"); */
    return false; /* fault */
   }
	
  if (fr_type == hdr.size >> 12 || fr_type == ID_ECC_DATA)
   {
    /* Get ECC type from frame ID */
    ecc_type = FrameIDToECCType (hdr.size >> 12);
    if (ecc_type == (dword)ERROR)
      ecc_type = GOLEY_FLAG;
    Flags |= ecc_type;
    
    DecodeBlockWithErrStat ((byte*)(pcom + 1), ecc_fr, &pcom->err, Flags);
    memcpy (&pcom->hdr, &hdr, sizeof (fr_hdr));
    dprintf (5, "DT:GetDataFromFrame:Data found, Hdr type: %d, decoded.\n", hdr.size >> 12); 
   }

  return true; /* success */
}

/* handles of queues */
int msgmain = ERROR, msgdrv = ERROR;

/* Driver thread used it for notify main thread */
static int SendCommand (int unit, int com, int par)
{
  char msg[MAX_FRAME_SIZE + sizeof (command)];
  command *pcom = (command*)&msg;

  pcom->mtype = 1;
  pcom->com   = com;
  pcom->unit  = unit;
  pcom->tray  = par;

  if (msgsnd (msgmain, &msg, sizeof (msg), 0) < 0)
   {
    dprintf (1, "MT:SendCommand:Write error to main queue, handle %i!\n", msgmain);
    return false;
   }
  else
    return true;
}

/* phase: 0,1,2,3,4,5,6,7 */
extern int SetPhase (int unit, int phase)
{
  return SendCommand (unit, AVT_SET_PHASE_COMMAND, phase);
}

/* subphase: 0,1,2,3,4 */
extern int SetSubPhase (int unit, int subphase)
{
  return SendCommand (unit, AVT_SET_SUBPHASE_COMMAND, subphase);
}

/* autophase: 0,1 */
extern int SetAutoPhase (int unit, int autophase)
{
  if (autophase)
    return SendCommand (unit, AVT_SET_AUTOPHASE_COMMAND, 0);
  else
    return SendCommand (unit, AVT_RESET_AUTOPHASE_COMMAND, 0);
}

extern int SetReadMode (int unit, int phase, int subphase, int autophase)
{
  int rc = false;
  rc |= SetPhase (unit, phase);
  rc |= SetSubPhase (unit, subphase);
  rc |= SetAutoPhase (unit, autophase);
  
  return rc;
}

static int DriverThread (void)
{
  avd_param avdp;
  long long fr_id = 0;
  int phase = 3, subphase = 0, autophase = 1;
  int unit = 0, rc = 0, fmode = 0, hFile = ERROR, cnt;
  byte mode = 0, eop = 0, msg[MAX_FRAME_SIZE + sizeof (command)], hard_fr[MAX_FRAME_SIZE];
  command *pcom = (command*)&msg;
  
  InitECCTable ();
  InitCHCTable ();

/*  unlink ("./test.tmp");
  int hDebFile = open ("./test.tmp", O_CREAT | O_WRONLY | O_BINARY, FILE_ACCESS); */
  
  while (!eop)
   {
    if ((cnt = msgrcv (msgmain, &msg, sizeof (msg), 0, 0)) <= 0)
     {
      dprintf (1, "DT:Read error from main queue!\n");
      continue;
     }
    else
      dprintf (5, "DT:New msg, size %i\n", cnt);
     
    if (pcom->com == EOP_COMMAND)
     {
      eop = 1;
      dprintf (3, "DT:EOP_COMMAND\n");
      break;
     }
    else
    if (pcom->com == AVT_SET_PHASE_COMMAND)
     {
      rc = ioctl (hFile, AVT_SET_PHASE, pcom->tray);
      
      if (rc)
        break;
      else
        phase = pcom->tray;
      continue;	
     }
    else
    if (pcom->com == AVT_SET_SUBPHASE_COMMAND)
     {
      rc = ioctl (hFile, AVT_SET_SUBPHASE, pcom->tray);
      
      if (rc)
        break;
      else
        subphase = pcom->tray;
      continue;	
     }
    else
    if (pcom->com == AVT_SET_AUTOPHASE_COMMAND)
     {
      rc = ioctl (hFile, AVT_SET_AUTOPHASE, 0);
      
      if (rc)
        break;
      else
        autophase = 1;
      continue;	
     }
    else
    if (pcom->com == AVT_RESET_AUTOPHASE_COMMAND)
     {
      rc = ioctl (hFile, AVT_RESET_AUTOPHASE, 0);
      
      if (rc)
        break;
      else
        autophase = 0;
      continue;	
     }
    else /* arvid must be open - send read or write command before */ 
    if (pcom->com == AVT_LOOP_TEST_COMMAND)
     {
      if (mode != 'i') /* data mode */
       {
        /* send stage test */
        int fr_type = pcom->hdr.size >> 12;
	
        if (fr_type > ID_IRC_FRAME)
	  MakeDataFrame (&avdp, pcom, fr_type, hard_fr);
	else
	if (fr_type == ID_RAW_FRAME)
          memcpy (hard_fr, pcom + 1, avdp.SizeArvFrame * 2);
	  
        /* receive stage test */
        if (fr_type > ID_IRC_FRAME)
	 {
	  if (!GetDataFromFrame (&avdp, pcom, fr_type, hard_fr))
	   {
            pcom->com = AVT_DATA_FRAME_NOT_FOUND_COMMAND;
            pcom->mtype = 1;
		
            if (msgsnd (msgdrv, pcom, sizeof (command), 0) < 0)
              dprintf (1, "DT:Write error to driver queue!\n");
	      
            continue; /* command uncomplete */
           } /* end if data header is corrupt */
         }    
	else /* end if pcom->hdr.fr_type > ID_IRC_FRAME */
        if (fr_type == ID_RAW_FRAME)
	  memcpy (pcom + 1, hard_fr, avdp.SizeArvFrame * 2);
       }
      else
      if (mode == 'i') /* irc_mode */
       {
        /* send stage test */
        memcpy (hard_fr, pcom + 1, avdp.SizeArvFrame * 2);
	
        /* receive stage test */
	memcpy (pcom + 1, hard_fr, avdp.SizeArvFrame * 2);
       }
       
      /* send msg to main thread */
      pcom->com = 0;
      pcom->mtype = 1;
      /* dump (1, (char*)(pcom + 1), MAX_FRAME_SIZE); */
      
      if (msgsnd (msgdrv, &msg, sizeof (msg), 0) < 0)
        dprintf (1, "DT:Write error to driver queue!\n");
	
      continue;	/* command complete */
     }

    /* begin normal command handling */
    if (pcom->com  != fmode
     || pcom->mode != mode
     || pcom->unit != unit
     || hFile == ERROR)
     {
      if ((hFile = avd_reopen (pcom->unit, pcom->com, pcom->mode, hFile, &avdp)) < 0)
        return hFile;
      else
       { /* succesfull reopen */
       
        if (pcom->mode != 'i' && pcom->com == O_RDONLY)
	 {
          ioctl (hFile, AVT_SET_PHASE, phase);
          ioctl (hFile, AVT_SET_SUBPHASE, subphase);
	
	  if (autophase)
	    ioctl (hFile, AVT_SET_AUTOPHASE, 0);
	  else
	    ioctl (hFile, AVT_RESET_AUTOPHASE, 0);
	 }    
	
        fmode = pcom->com;
	mode  = pcom->mode;
	unit  = pcom->unit;
	fr_id = 0;
       }
     }
     
    if (pcom->com == O_WRONLY)
     {
      if (mode != 'i') /* data mode */
       {
        int fr_type = pcom->hdr.size >> 12;
	
        if (fr_type > ID_IRC_FRAME)
	 {
	  MakeDataFrame (&avdp, pcom, fr_type, hard_fr);
	  /* write (hDebFile, hard_fr, avdp.SizeArvFrame * 2); */
	  /* dprintf (5, "Send data frame.\n"); */
	 }
	else
	if (fr_type == ID_RAW_FRAME)
	 {
          memcpy (hard_fr, pcom + 1, avdp.SizeArvFrame * 2);
	  /* write (hDebFile, hard_fr, avdp.SizeArvFrame * 2); */
	  /* dprintf (5, "Send raw data frame.\n"); */
	 }
       }
      else /* irc mode */
        memcpy (hard_fr, pcom + 1, avdp.SizeArvFrame * 2);

      /* dump (1, hard_fr, avdp.SizeArvFrame * 2); */
      rc = write (hFile, hard_fr, avdp.SizeArvFrame * 2);
      
      if (mode != 'i'
       && (pcom->hdr.size >> 12) > ID_IRC_FRAME)
       {
        /* send msg to main thread */
	pcom->com   = 0;
        pcom->mtype = 1;
	pcom->tray  = get_time ();
	  
        if (msgsnd (msgdrv, pcom, sizeof (command), 0) < 0)
         {
    	  dprintf (1, "DT:Write error to driver queue!\n");
	  break;
	 }
       }
     }
    else
    if (pcom->com == O_RDONLY)
     {
      if (mode == 'i')
        pcom->tray = 1;
      else
      if (!pcom->tray)
        pcom->tray = 1;
      
      while (pcom->tray)
       {
        rc = read (hFile, hard_fr, avdp.SizeArvFrame * 2);

        if (mode != 'i') /* data receive mode */
         {
          int fr_type = pcom->hdr.size >> 12;
	  
	  if (fr_type == ID_RAW_FRAME)
	    pcom->tray = 1;
	
          if (fr_type > ID_IRC_FRAME)
	   {
	    /* write (hDebFile, hard_fr, rc); */
	    
	    if (!GetDataFromFrame (&avdp, pcom, fr_type, hard_fr))
	     {
	      if (pcom->tray == 1)  /* send msg to main thread */ 
	       {
	        pcom->com = AVT_DATA_FRAME_NOT_FOUND_COMMAND;
                pcom->mtype = 1;
		
                if (msgsnd (msgdrv, &msg, sizeof (command), 0) < 0)
                 {
                  dprintf (1, "DT:Write error to driver queue!\n");
	          break;
                 }
	       }
		
	      if (pcom->tray)
	        pcom->tray--;
		
              continue;
	     } /* end if data header is corrupt */
	    else /* data found - clear tray counter */
	      pcom->tray = 0;
	   }    
	  else /* end if pcom->hdr.fr_type > ID_IRC_FRAME */
          if (fr_type == ID_RAW_FRAME)
	   {
	    /* write (hDebFile, hard_fr, rc); */
	    memcpy (pcom + 1, hard_fr, avdp.SizeArvFrame * 2);
	    
	    if (pcom->tray) 
              pcom->tray--;
	   }
         }
        else /* irc receive mode */
	 {
          memcpy (pcom + 1, hard_fr, avdp.SizeArvFrame * 2);

          if (pcom->tray)
            pcom->tray--;
	 }
     
        /* send msg to main thread */
	pcom->com = 0;
        pcom->mtype = 1;
        if (msgsnd (msgdrv, &msg, sizeof (msg), 0) < 0)
         {
          dprintf (1, "DT:Write error to driver queue!\n");
	  break;
         }
	 
       } /* end while tray count */
     } /* end O_RDONLY */
    else
      dprintf (1, "DT:Unavaible command detected!\n");
   } /* end while */

  rc = avd_get_param (hFile, &avdp, 0);
  close (hFile);
  
  /* Driver thread destroy all queues */
  if (msgctl (msgmain, IPC_RMID, 0) < 0)
   {
    dprintf (1, "DT:Cann't remove main queue!\n");
    exit (1);
   }
  
  if (msgctl (msgdrv, IPC_RMID, 0) < 0)
   {
    dprintf (1, "DT:Cann't remove driver queue!\n");
    exit (1);
   }
   
/*  close (hDebFile); */
  FreeCHCTable ();
  FreeECCTable ();
  
  return rc;
}

int LoopBackTest (command *pcom, dword Flags)
{
  int cnt, fr_type, i, itr = 1;

  pcom->mtype = 1;   /* unix message signature */
  pcom->com   = O_WRONLY;
//  pcom->unit  = 0; /* board number - upper controled */
  pcom->mode  = 'l'; /* density - 'l' - low (200kbps) 'h' - high (325kbps) */
  pcom->hdr.id = 0;  /* to do */

  if (ARVID1052_FLAG & Flags) pcom->mode = 'h';

  /* Take frame type by ecc type */
  fr_type = ECCTypeToFrameID (Flags);

  if (fr_type < 0)
    fr_type = GOLEY_FLAG;
    
  pcom->hdr.size = (fr_type << 12) | (GetBlockDataSize (Flags) >> 1);
  dprintf (5, "MT:LoopBackTest:pcom->hdr.size: %Xh\n", pcom->hdr.size & 0xFFF);

  memset (pcom + 1, 0xBB, GetBlockDataSize (Flags));
  
  /* change mode and send data command */
  while (itr)
   {
    if (msgsnd (msgmain, pcom, MAX_FRAME_SIZE + sizeof (command), 0) < 0)
     {
      dprintf (1, "MT:LoopBackTest:Write error to main queue, handle %i!\n", msgmain);
      continue;
     }

    itr--;
   }

  itr = 1;
  
  /* read completion command */
  while (itr)
   {
    command cmd;
    cmd.mtype = 1;
    
    if ((i = msgrcv (msgdrv, &cmd, sizeof (cmd), 0, 0)) <= 0)
     {
      dprintf (1, "MT:LoopBackTest:Read error from drv queue, handle %i!\n", msgdrv);
      continue;
     }

    itr--;
   }
  
  dword src_crc = GetBlockCRC ((byte*)(pcom + 1), GetBlockDataSize (Flags));
  
  pcom->mtype = 1;
  pcom->com = AVT_LOOP_TEST_COMMAND;
  
  if (msgsnd (msgmain, pcom, sizeof (command) + MAX_FRAME_SIZE, 0) < 0)
    dprintf (1, "MT:LoopBackTest:Write error to main queue, handle %i!\n", msgmain);

  memset (pcom + 1, 0xCC, MAX_FRAME_SIZE); /* set contrast backchar */
  
  if ((cnt = msgrcv (msgdrv, pcom, sizeof (command) + MAX_FRAME_SIZE, 0, 0)) <= 0)
    dprintf (1, "MT:LoopBackTest:Read error from drv queue!\n");
  else
   {
    if (pcom->com == AVT_DATA_FRAME_NOT_FOUND_COMMAND)
      dprintf (3, "MT:LoopBackTest:Get AVT_DATA_FRAME_NOT_FOUND!\n");
    else
     {
      if (src_crc == GetBlockCRC ((byte*)(pcom + 1), GetBlockDataSize (Flags)) )
       {
        dprintf (3, "MT:LoopBackTest:Data loop test succesed!\n");
	return true;	
       }
      else
       {
        dprintf (1, "MT:LoopBackTest:Data loop test failed! Data corrupted!\n");
        dump (1, (char*)(pcom + 1), GetBlockDataSize (Flags));
        // dump (1, (char*)(pcom + 1), MAX_FRAME_SIZE);
       }
     }
   }
   
  return false;
}

int main (int argc, char** argv)
{
  key_t drv_key  = ftok ("/dev/avd0l", AVT_MAJOR);
  key_t main_key = ftok ("/dev/avd0h", AVT_MAJOR);
  
  if (drv_key < 0 
  || main_key < 0)
   {
    dprintf (1, "Main:Cann't get process key!\n");
    exit (1);
   }
   
  /* Destroy all exist queues */
  if ((msgmain = msgget (main_key, 0)) >= 0)
   {
    dprintf (1, "Main:Main msg queue alive! It will be destroyed!\n");
    
    if (msgctl (msgmain, IPC_RMID, 0) < 0)
     {
      dprintf (1, "Main:Cann't remove main queue!\n");
      exit (1);
     }
   }
   
  if ((msgdrv = msgget (drv_key, 0)) >= 0)
   {
    dprintf (1, "Main:Driver msg queue alive! It will be destroyed!\n");
    
    if (msgctl (msgdrv, IPC_RMID, 0) < 0)
     {
      dprintf (1, "Main:Cann't remove driver queue!\n");
      exit (1);
     }
   }

  /* main thread split */
  pid_t pid = fork ();
  if (pid < 0)
   {
    perror ("fork");
    exit (1);
   }
  else
  if (!pid)
   {
    dprintf (3, "DT:Child alive!\n");
    
    if ( (msgdrv = msgget (drv_key, FILE_ACCESS | IPC_CREAT)) < 0)
     {
      dprintf (1, "DT:Cann't create driver queue!\n");
      exit (1);
     }
    
    dprintf (3, "DT:msgdrv %i\n", msgdrv);

    int trg = 0;
    /* wait for launch main thread */
    while ( (msgmain = msgget (main_key, 0)) < 0)
     {
      if (!trg)
       {
        dprintf (3, "DT:Cann't open main queue!\n");
	trg++;
       }
     }
     
    dprintf (3, "DT:msgmain %i\n", msgmain);
    return DriverThread ();
   }
  else
   {
    dprintf (3, "MT:Parent alive!\n");
    
    if ( (msgmain = msgget (main_key, FILE_ACCESS | IPC_CREAT)) < 0)
     {
      dprintf (1, "MT:Cann't create main queue!\n");
      exit (1);
     }
     
    dprintf (3, "MT:msgmain %i\n", msgmain);
     
    int trg = 0;
    /* wait for launch driver thread */
    while ( (msgdrv = msgget (drv_key, 0)) < 0)
     {
      if (!trg)
       {
        dprintf (3, "MT:Cann't open driver queue!\n");
	trg++;
       }
     }
     
    dprintf (3, "MT:msgdrv %i\n", msgdrv);     
    return MainThread ();
   }
}
