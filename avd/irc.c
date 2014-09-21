
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>

#include <sys/msg.h>

#include "defines.h"
#include "dprint.h"
#include "arvid.h"
#include "vcr.h"
#include "ecc.h"

#include "driver_thread.h"
#include "tape.h"
#include "irc.h"

int SendIRCCommand (int unit, byte *IRCCom, int size_buf)
{
  char msg[MAX_FRAME_SIZE + sizeof (command)];
  command *pcom = (command*)&msg;

  pcom->mtype = 1;
  pcom->com  = O_WRONLY;
  pcom->unit = unit;
  pcom->mode = 'i';
  pcom->tray = 1;
  pcom->hdr.size = 0;
  
  int full_frames = size_buf / SSIZE_IRC_FRAME;

  /* change mode and send irc command */
  while (full_frames--)
   {
    memcpy (pcom + 1, IRCCom, SSIZE_IRC_FRAME);
    IRCCom += SSIZE_IRC_FRAME;

    if (msgsnd (msgmain, pcom, sizeof (command) + MAX_FRAME_SIZE, 0) < 0)
     {
      dprintf (1, "MT:SendIRCCommand:Write error to main queue, handle %i!\n", msgmain);
      continue;
     }
   }
   
  int last = size_buf % SSIZE_IRC_FRAME;

  /* send last */  
  if (last)
   {
    memset (pcom + 1, 0, SSIZE_IRC_FRAME);
    memcpy (pcom + 1, IRCCom, last);
    
    if (msgsnd (msgmain, pcom, sizeof (command) + MAX_FRAME_SIZE, 0) < 0)
     {
      dprintf (1, "MT:SendIRCCommand:Write error to main queue, handle %i!\n", msgmain);
      return E_WRITE_TO_MAIN_QUEUE;
     }
   }
  
  return E_OK;
}

int PlayIRCCommandFromFile (command *pcom, char *name)
{
  int i, cnt, itr, hCom = ERROR, sizeIRCCom, size_buf;

  if ((hCom = open (name, O_BINARY | O_RDONLY, FILE_ACCESS)) == ERROR)
   {
    dprintf (1, "MT:PlayIRCCommandFromFile:File %s not found.\n", name);
    return E_FILE_NOT_FOUND;
   }
   
  sizeIRCCom = lseek (hCom, 0, SEEK_END);
  lseek (hCom, 0, SEEK_SET);

  size_buf = (BEG_PAUSE + sizeIRCCom + END_PAUSE + (SSIZE_IRC_FRAME - 1)) / SSIZE_IRC_FRAME * SSIZE_IRC_FRAME;
  byte *IRCCom = malloc (sizeof (byte) * size_buf);
  
  if (!IRCCom)
   {
    close (hCom);
    dprintf (1, "MT:PlayIRCCommandFromFile:Cannot alocate memory fo IRC command: %d\n", size_buf);
    return E_OUT_OF_MEMORY;
   }
  
  memset (IRCCom, 0, size_buf);
  cnt = read (hCom, &IRCCom[BEG_PAUSE], sizeIRCCom);
  close (hCom);
  
  pcom->mtype = 1;
  pcom->com = O_WRONLY;
/*  pcom->unit = 0; */ /* external */
  pcom->mode = 'i';
  pcom->tray = 1;  
  pcom->hdr.size = 0;
  
  itr = size_buf / SSIZE_IRC_FRAME;
  i = 0;
  /* change mode and send irc command */
  while (itr--)
   {
    memcpy (pcom + 1, &IRCCom[i], SSIZE_IRC_FRAME);
    i += SSIZE_IRC_FRAME;

    if (msgsnd (msgmain, pcom, sizeof (command) + MAX_FRAME_SIZE, 0) < 0)
     {
      dprintf (1, "MT:PlayIRCCommandFromFile:Write error to main queue, handle %i!\n", msgmain);
      continue;
     }
   }
  
  free (IRCCom);
  dprintf (1, "MT:PlayIRCCommandFromFile:Send IRC command, size: %d\n", cnt);
  return E_OK;
}

int SaveIRCCommandToFile (command *pcom, char *name, int empty_frames)
{
  int i, cnt, crc32, hCom = ERROR, frame_count = 0;
  
  unlink (name);
  if ((hCom = open (name, O_BINARY | O_WRONLY | O_CREAT, FILE_ACCESS)) == ERROR)
   {
    dprintf (1, "MT:SaveIRCCommandToFile:Create file error: %s, check path!\n", name);
    return E_FILE_NOT_FOUND;
   }

  while (1)
   {
    pcom->mtype = 1;
    pcom->com = O_RDONLY;
    /*  pcom->unit = 0; */ /* external */
    pcom->mode = 'i';
    pcom->tray = 1;
    pcom->hdr.size = 0;
   
    if (msgsnd (msgmain, pcom, sizeof (command), 0) < 0)
     {
      dprintf (1, "MT:SaveIRCCommandToFile:Write error to main queue, handle %i!\n", msgmain);
      return E_WRITE_TO_MAIN_QUEUE;
     }
  
    if ((cnt = msgrcv (msgdrv, pcom, sizeof (command) + MAX_FRAME_SIZE, 0, 0)) <= 0)
     {
      dprintf (1, "MT:SaveIRCCommandToFile:Read error from drv queue!\n");
      return E_READ_FROM_DRV_QUEUE;
     }

    crc32 = 0;
    for (i = 0; i < SIZE_IRC_FRAME; i++)
      crc32 += *((byte*)((int)(pcom + 1) + i));

    if (crc32 > 0)
     {
      write (hCom, (byte*)(pcom + 1), SIZE_IRC_FRAME);
      frame_count++;
      break;
     }
   }
   
  dprintf (3, "MT:SaveIRCCommandToFile:Found IRC Data - wait for %i empty_frames.\n", empty_frames);
  
  int empty_count = 0;
  while (1)
   {
    pcom->mtype = 1;
    pcom->com = O_RDONLY;
    /*  pcom->unit = 0; */ /* external */
    pcom->mode = 'i';
    pcom->tray = 1;
    pcom->hdr.size = 0;
    
    if (msgsnd (msgmain, pcom, sizeof (command), 0) < 0)
     {
      dprintf (1, "MT:SaveIRCCommandToFile:Write error to main queue, handle %i!\n", msgmain);
      return E_WRITE_TO_MAIN_QUEUE;
     }
  
    if ((cnt = msgrcv (msgdrv, pcom, sizeof (command) + MAX_FRAME_SIZE, 0, 0)) <= 0)
     {
      dprintf (1, "MT:SaveIRCCommandToFile:Read error from drv queue!\n");
      return E_READ_FROM_DRV_QUEUE;
     }

    crc32 = 0;
    for (i = 0; i < SIZE_IRC_FRAME; i++)
      crc32 += *((byte*)((int)(pcom + 1) + i));

    write (hCom, (byte*)(pcom + 1), SIZE_IRC_FRAME);
    frame_count++;
    
    if (crc32 > 0)
      empty_count = 0; 
    else
     {
      empty_count++;
      
      if (empty_count >= empty_frames)
        break;
     }
   } /* end while */
   
  close (hCom);
  dprintf (3, "MT:SaveIRCCommandToFile:Receive IRC command, %d frames:\n", frame_count);
  return E_OK;
}
