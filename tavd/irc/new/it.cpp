#include "compiler.h"

#include "copyrit.h"
#include "mytypes.h"
#include "adriver.h"
#include "console.h"

#include "ircnew.h"
#include "avtreg.h"

//#define BEG_PAUSE 0x16c
#define BEG_PAUSE 0
//#define END_PAUSE SIZE_IRC_FRAME
#define END_PAUSE 0

bool PlayIRCCommandFromFile (char *name)
{
  int hCom = -1;

  if ((hCom = open (name, O_BINARY | O_RDONLY, FILE_ACCESS)) ==-1)
   {
    printf ("File %s not found.\n", name);
    return false;
   }
  else
   {
    dword sizeIRCCom = lseek (hCom, 0, SEEK_END);
    lseek (hCom, 0, SEEK_SET);

    byte *IRCCom = new byte [BEG_PAUSE + sizeIRCCom + END_PAUSE];

    if (!IRCCom)
     {
      close (hCom);
      printf ("Cannot alocate memory fo IRC command: %d\n", sizeIRCCom);
      return false;
     }

    bzero (IRCCom, BEG_PAUSE + sizeIRCCom + END_PAUSE);
    int bread = read (hCom, &IRCCom[BEG_PAUSE], sizeIRCCom);
    close (hCom);

/*
    int numwords = bread >> 1, testprint = 10;
    word *wIRCCom = (word*)&IRCCom[BEG_PAUSE];
    printf ("numword: %d\n", numwords);

    while (numwords--)
     {
      if (testprint)
	printf ("Data: %04X |", *wIRCCom);

      *wIRCCom = ((*wIRCCom & 0xFF) << 8) | ((*wIRCCom & 0xFF00) >> 8);

      if (testprint)
	printf (" %04X\n", *wIRCCom);

      if (testprint)
	testprint--;

      wIRCCom++;
     }
*/
    int crc32 = 0;
    for (dword i = 0; i < (BEG_PAUSE + sizeIRCCom + END_PAUSE); i++)
      crc32 += *((byte*)((int)IRCCom + i));

    printf ("Send IRC command, size: %d, CRC32: %X\n", bread, crc32);
    // SendIRCSCommand (IRCCom, BEG_PAUSE + sizeIRCCom + END_PAUSE);
    SendIRCCommand (IRCCom, BEG_PAUSE + sizeIRCCom + END_PAUSE);
    delete IRCCom;
   }

  return true;
}

bool RecordIRCCommand (char *name, dword NumFrames)
{
  int hCom = -1;

  unlink (name);
  if ((hCom = open (name, O_BINARY | O_WRONLY | O_CREAT, FILE_ACCESS)) ==-1)
   {
    printf ("Create file error: %s, check path!\n", name);
    return false;
   }
  else
   {
    printf ("Receive IRC command, %d frames:\n", NumFrames);
    ReceiveIRCSCommand (hCom, NumFrames);
    close (hCom);
   }

  return true;
}

int main (int argc, char *argv[])
{
  VCR	  vcr; // класс работы с VCR
  int	  rc, fdout = 1; //, key,  done;

  printf (Rights);
  // загрузим таблицу
//  if ((rc = vcr.LoadVCR ("AKAIR110.VCR")))
  if ((rc = vcr.LoadVCR ("PANANV-S.VCR")))
    exit (rc);

  LoadIniFile ();
  InitArvid ();
  GD.Initialize ();
  InitAsynCommand ();

  /// fdout = open ("/dev/ravt0", O_WRONLY, 0);
//SetRW.rate = 200;
//SetRW.flag = IODATA;
//ioctl (fdout, VSET_WRITE, &SetRW);
  /// SetRW.rate = 200;
  /// SetRW.flag = IOIRC;
  /// ioctl (fdout, VSET_WRITE, &SetRW);

///if (vcr.IrcKeyTrn (fdout, key)) {
  if (vcr.SetMode (fdout, 0))
   {
    printf ("Error key num: %s\n", "0");
    /// close (fdout);
    FreeAsynCommand ();
    FreeArvid ();
    return 1;
   };

//vcr.Move(fdout, -10);
  vcr.IrcKeyTrn (fdout, "PLAY");
  /// ioctl (fdout, VSET_WRITE, &SetRW);
///  vcr.IrcKeyTrn (fdout, "REW");
  /// ioctl (fdout, VSET_WRITE, &SetRW);
  sleep (2);
  vcr.IrcKeyTrn (fdout, "STOP");
  /// close (fdout);

//  PlayIRCCommandFromFile ("../dump/rpower.bin");
//  RecordIRCCommand ("../dump/rpower.bin", 50);

  while ((IComFlag || SComFlag || RComFlag || StartFlag) && !StopByUser)
    CheckOnFreeComBuffer ();

  FreeAsynCommand ();
  FreeArvid ();
  printf ("End operation.\n");
  return 0;
};
