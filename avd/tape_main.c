
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/msg.h>
#include <signal.h>

#include "defines.h"
#include "dprint.h"
#include "arvid.h"
#include "vcr.h"
#include "ecc.h"
#include "goley.h"
#include "pt.h"

#include "driver_thread.h"
#include "tape.h"
#include "ini.h"
#include "irc.h"
#include "main_thread.h"

void PrintVCR (int unit, char *vcr_filename)
{
  vcr VCR;
  
  VHSopen (&VCR, unit, vcr_filename, 0, 0);
  PrintVCRfile (&VCR);
  VHSclose (&VCR);
}

/* disconnect child process and destroy all queue */
static void Disconnect (command *pcom)
{
  pcom->mtype = 1;
  pcom->com = EOP_COMMAND;
  
  if (msgsnd (msgmain, pcom, sizeof (command), 0) < 0)
    dprintf (1, "MT:Write error to main queue, handle %i!\n", msgmain);
    
  sleep (1); /* wait child */
}

static void exit_sighandler (int x)
{
  char msg[MAX_FRAME_SIZE + sizeof (command)];
  command *pcom = (command*)&msg;
  static int sig_count = 0;
  ++sig_count;
  
  if (sig_count == 2) 
    exit (1);
    
  if (sig_count > 2)
   {
    /* can't stop :( */
    kill (getpid(), SIGKILL);
   }

  Disconnect (pcom);
  exit (1);
}

/* Migrate from MPlayer */
static void SignalsHook (void)
{
  /* ========= Catch terminate signals: ================ */
  /* terminate requests: */
  signal (SIGTERM, exit_sighandler); // kill
  signal (SIGHUP,  exit_sighandler); // kill -HUP  /  xterm closed
  signal (SIGINT,  exit_sighandler); // Interrupt from keyboard
  signal (SIGQUIT, exit_sighandler); // Quit from keyboard
  
  /* fatal errors: */
  signal (SIGBUS,  exit_sighandler); // bus error
  signal (SIGSEGV, exit_sighandler); // segfault
  signal (SIGILL,  exit_sighandler); // illegal instruction
  signal (SIGFPE,  exit_sighandler); // floating point exc.
  signal (SIGABRT, exit_sighandler); // abort()
}

int MainThread (void)
{
  int rc = 0, itr, i;
  char msg[MAX_FRAME_SIZE + sizeof (command)];
  /* ref. to driver_thread.h ID_XXX combination ECC and density flags */
  dword Flags = ARVID1052_FLAG | ID_G1_FRAME; /* ID_RAW_FRAME, ID_RS1_FRAME, ID_RS2_FRAME, ID_RS3_FRAME */
  command *pcom = (command*)&msg;
  int res;

  char* ini_name = DEFAULT_INI_FILE;
  char* hostname = getenv ("HOSTNAME");
  char name [256];
  
  if (hostname)
   {
    sprintf (name, "./%s.ini", hostname);
    ini_name = name;
   }

  SignalsHook ();
  if (!LoadIniFile (ini_name, avdpar, MAX_AVT))
   {
    if (!LoadIniFile (DEFAULT_INI_FILE, avdpar, MAX_AVT))
      return rc;
   }

  pcom->unit = 0; /* boards number */
  
  /* driver thread send/receive main thread test without real operations with board */
  LoopBackTest (pcom, Flags);
  
  /* Мой тестовый кусок
   Так как пока ничего не понятно, буду добавлять сюда по одной команде
   */
  /* VCRTest (pcom->unit, avdpar[pcom->unit].sp_vcr_filename); */
  vcr VCR;
  res = VHSopen (&VCR, pcom->unit, avdpar[pcom->unit].sp_vcr_filename, avdpar[pcom->unit].auto_power, 0);
  printf ("VHSopen: %d\n", res);
  
  PrintVCRfile (&VCR);
  sleep(2);

  res = VHSclose (&VCR);
  printf ("VHSclose: %d\n", res);
  /* disconnect child process from DT and destroy all queue */
  Disconnect (pcom);
  return rc;
  
  goto read_tape;

  /* создаем новое TPB */
  tpb TBP;
  TBP.time = 180 * 60; /* convert minutes to seconds */
   /* ref. to ecc.h for choosing combination ECC and density flags */
  TBP.flags = GOLEY_FLAG; /* ECC, density, block/cluster and SP/LP */
  strcpy (TBP.name, "Test.Volume.1");

  /* устанавливаем новую ленту */
  tpb *ptpb = SetTape (pcom->unit, &TBP, 'l');
  
  if (!ptpb) printf("SetTape: Tape not set!\n");

  //Rewind (&VCR);
  printf ("Press ENTER after tape rewind.\n");
  getchar ();

  res = WriteStart (&VCR, REC_CURRENT_PLACE, 0);
  printf("WriteStart %d\n",res);
  
  WritePause (&VCR, ONE_SECOND * 100);

  memset (pcom + 1, 0x5A, MAX_FRAME_SIZE);
  
  printf("\nWrite sectors : ");
  for (i = PT[pcom->unit]->end_frame; i < (4000 + PT[pcom->unit]->beg_frame); i++)
  {
    res = WriteData (&VCR, pcom + 1, i, 1);
    memset (pcom + 1, 0x5A, MAX_FRAME_SIZE);
    printf ("%d [%d] ",i,res);
    fflush (stdout);
  }

  WriteEnd (&VCR);
//  getchar ();
  SavePT (pcom->unit, "pt.bin");
  PrintPT (pcom->unit);
  goto queue_end;

  read_tape:
  LoadPT (pcom->unit, "pt.bin");

  printf ("\n===================== I D E N T I F Y =========================\n");

  res = IdentifyTape (&VCR, IDENT_WITH_REW);
  printf ("IdentifyTape %d\n",res);
  if (res < 0) goto queue_end;

//  goto queue_end;
  
//  Rewind (&VCR);
//  printf ("Press ENTER after tape rewind.\n");
//  getchar ();

  estat err;
  char *buf = malloc (MAX_FRAME_SIZE);
  memset (buf, 0, MAX_FRAME_SIZE);

  int out = open ("test.bin", O_CREAT | O_RDWR);
  
  if (out != -1)
   {
    for (i = 1000; i < 2000; i++)
     {
      ReadData (&VCR, buf, i, 1, &err);
      
      printf("%d<%lu %lu %lu %lu %lu> ",
             i,
             err.OrdinaryErr,
             err.DupletErr,
             err.TripletErr,
             err.QuadroErr,
             err.BadFrames);
	     
      fflush (stdout);
      write (out, buf, MAX_FRAME_SIZE);
     }
    
    close(out);
   }

  ReadEnd (&VCR);
  free (buf);

  queue_end:

  printf ("VHSclose %d\n", VHSclose (&VCR));

  /* test to send irc data - doesn't requied for tape operation */
  pcom->mtype = 1;
  pcom->com   = O_WRONLY;
  pcom->unit  = 0;
  pcom->mode  = 'i';
  pcom->hdr.size = 0;

  itr = 50;
  memset (pcom + 1, 0xAA, MAX_FRAME_SIZE);

  /* change mode and send irc command */
  while (itr)
   {
    if (msgsnd (msgmain, &msg, sizeof (msg), 0) < 0)
     {
      dprintf (1, "MT:Write error to main queue, handle %i!\n", msgmain);
      continue;
     }

    itr--;
   }

  /* disconnect child process from DT and destroy all queue */
  Disconnect (pcom);
  return rc;
  
#if 0
  tpb TBP;
  TBP.time = 180 * 60; /* convert minutes to seconds */
  TBP.flags = GOLEY_FLAG;
  strcpy (TBP.name, "Test.Volume.1");
  tpb *ptpb = SetTape (pcom->unit, &TBP, 'l');
  
  if (!ptpb)
    dprintf (1, "MT:SetTape: Tape not set!\n");

  Rewind (&VCR);
  //dprintf (1, "Press ENTER after tape rewind.\n");
  printf ("Press ENTER after tape rewind.\n");
  getchar ();
    
  WriteStart (&VCR, REC_CURRENT_PLACE, 0);
/*  WritePause (&VCR, ONE_SECOND * 15);
  MoveTape (&VCR, -60, PLAY); */
  
  WritePause (&VCR, ONE_SECOND * 5);
  memset (pcom + 1, 0xF, MAX_FRAME_SIZE);
  
  printf ("\nWrite sectors : ");
  for (i = PT[pcom->unit]->end_frame; i < (100 + PT[pcom->unit]->beg_frame); i++)
   {
    WriteData (pcom->unit, pcom + 1, i, 1);
    printf ("%d ",i);
    fflush (stdout);
   }

  WriteEnd (pcom->unit, &VCR);
  IdentifyTape (&VCR, IDENT_WITH_REW);

  /* fr_hdr hdr;
  printf ("fr_hdr size: %i, hdr.id size: %i\n", sizeof (hdr), sizeof (hdr.id)); */

  /* PrintVCR (0, avdpar[pcom->unit].sp_vcr_filename); */
  /* VCRTest  (0, avdpar[pcom->unit].sp_vcr_filename);
  Disconnect (pcom);
  return 0; */

  pcom->unit  = 0;

  /* driver thread <-> main thread test without real operations with boards */
  LoopBackTest (pcom, Flags);

#if 0  
  int rec = 0;
  if (rec)
    SaveIRCCommandToFile (pcom, "./play.dmp", 4); /* 3 is optimal */
  else    
    PlayIRCCommandFromFile (pcom, "/c:/work/dump/eject.bin");

  /* PlayIRCCommandFromFile (pcom, "/c:/work/dump/eject.bin"); */
  /* PlayIRCCommandFromFile (pcom, "/c:/work/dump/power.bin"); */
#endif
 
  /* VCRTest (pcom->unit, avdpar[pcom->unit].sp_vcr_filename); */
  vcr VCR;
  VHSopen (&VCR, pcom->unit, avdpar[pcom->unit].sp_vcr_filename, avdpar[pcom->unit].auto_power, 0);
  /* PrintVCRfile (&VCR); */
  
  tpb TBP;
  TBP.time = 180 * 60; /* convert minutes to seconds */
  TBP.flags = GOLEY_FLAG;
  strcpy (TBP.name, "Test.Volume.1");
  tpb *ptpb = SetTape (pcom->unit, &TBP, 'l');
  
  if (!ptpb)
    dprintf (1, "MT:SetTape: Tape not set!\n");

  Rewind (&VCR);
  //dprintf (1, "Press ENTER after tape rewind.\n");
  printf ("Press ENTER after tape rewind.\n");
  getchar ();
    
  WriteStart (&VCR, REC_CURRENT_PLACE, 0);
/*  WritePause (&VCR, ONE_SECOND * 15);
  MoveTape (&VCR, -60, PLAY); */
  
  WritePause (&VCR, ONE_SECOND * 5);
  memset (pcom + 1, 0xF, MAX_FRAME_SIZE);
  
  printf ("\nWrite sectors : ");

  for (i = PT[pcom->unit]->end_frame; i < (100 + PT[pcom->unit]->beg_frame); i++)
   {
    WriteData (pcom->unit, pcom + 1, i, 1);
    printf ("%d ", i);
    fflush (stdout);
   }

  WriteEnd (pcom->unit, &VCR);
  IdentifyTape (&VCR, IDENT_WITH_REW);
  
/*if (PositionTape (&VCR, CST[pcom->unit].cur_frame, PAL_FPS, "STOP") < 0)
    dprintf (3, "MT:Cann't position on frame %d.\n", PAL_FPS); */

  dprintf (3, "MT:Write again from %i\n", PT[pcom->unit]->end_frame);
  WriteStart (&VCR, REC_NO_DIR_CHECK, PT[pcom->unit]->end_frame);
  WritePause (&VCR, ONE_SECOND * 5);
  
  int end_frame = 100 + PT[pcom->unit]->end_frame + 1;

  printf("\nWrite sectors : ");
  for (i = PT[pcom->unit]->end_frame + 1; i < end_frame; i++)
   {
    WriteData (pcom->unit, pcom + 1, i, 1);
    printf ("%d ", i);
    fflush (stdout);
   }
  
  WriteEnd (pcom->unit, &VCR);
  
  estat err;
  char *buf = malloc (MAX_FRAME_SIZE * 200);
  memset (buf, 0, MAX_FRAME_SIZE * 200);
  
  ReadData (&VCR, buf, PT[pcom->unit]->beg_frame, 200, &err);
  int out = open ("test.bin",O_CREAT | O_RDWR);
  
  if (out != -1)
   {
    printf ("\nwrite %d bytes estst<%lu %lu %lu %lu %lu>\n",
           MAX_FRAME_SIZE * 200,
           err.OrdinaryErr,
           err.DupletErr,
           err.TripletErr,
           err.QuadroErr,
           err.BadFrames);

    write (out, buf, MAX_FRAME_SIZE * 200);
    close (out);
   }
  
  ReadEnd (&VCR);
  free (buf);

#if 0
  MoveTape (&VCR, -60, PLAY);

  pcom->mtype = 1;
  pcom->com   = O_RDONLY;
  pcom->unit  = 0;
  pcom->mode  = 'l';
  pcom->tray  = 1;
  pcom->hdr.size = ID_RAW_FRAME << 12;
   
  itr = 50; /* 300 */
  /* to book itr number blocks */
  while (itr--)
   {
    if (msgsnd (msgmain, pcom, sizeof (command), 0) < 0)
     {
      dprintf (1, "MT:Write error to main queue, handle %i!\n", msgmain);
      continue;
     }
   }

  itr = 50; /* 300 */
  /* receive itr number blocks */
  while (itr)
   {
    int cnt;
    pcom->mtype = 1;
   
    if ((cnt = msgrcv (msgdrv, &msg, sizeof (msg), 0, 0)) <= 0)
     {
      dprintf (1, "MT:Read error from drv queue!\n");
      continue;
     }
    else
     {
      if (pcom->com == AVT_DATA_FRAME_NOT_FOUND_COMMAND)
        dprintf (3, "MT:Get AVT_DATA_FRAME_NOT_FOUND!\n");
	
      itr--;
     } 
   }
#endif

  PrintCST (pcom->unit);
  PrintPT  (pcom->unit);
  
  char filename[MAX_PATH + 1];
  sprintf (filename, "./%s.avs", PT[pcom->unit]->name);
  SavePT (pcom->unit, filename);
  
  ResetTape (pcom->unit);
  VHSclose (&VCR);

#if 0
  pcom->mtype = 1;
  pcom->com   = O_WRONLY;
  pcom->unit  = 0;
  pcom->mode  = 'i';
  pcom->hdr.size = 0;

  itr = 50;
  memset (pcom + 1, 0xAA, MAX_FRAME_SIZE);
  /* change mode and send irc command */
  while (itr--)
   {
    if (msgsnd (msgmain, &msg, sizeof (msg), 0) < 0)
     {
      dprintf (1, "MT:Write error to main queue, handle %i!\n", msgmain);
      continue;
     }
   }
#endif

  /* disconnect child process from DT and destroy all queue */
  Disconnect (pcom);
  return rc;
#endif
}
