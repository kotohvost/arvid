#include "compiler.h"

#include "copyrit.h"
#include "mytypes.h"
#include "adriver.h"
#include "console.h"
#include "inline.h"
#include "galey.h"
#include "mtime.h"
#include "aftp.h"
#include "tavd.h"

word *Buffer = NULL;

void SendATest (void)
{
  WaitLoop (DelayToTransmit);

  for (int i = 0; i < 1; i++)
    SendACluster (CLUSTERS, (byte*)Buffer, get_time ());

  while ((IComFlag || SComFlag || RComFlag || StartFlag) && !StopByUser)
    CheckOnFreeComBuffer ();
}

void SendSTest (void)
{
  for (int i = 0; i < 1; i++)
    SendSCluster (CLUSTERS, (byte*)Buffer, get_time ());
}

void ReceiveATest (void)
{
  ReceiveACluster (CLUSTERS);

  while ((IComFlag || SComFlag || RComFlag || StartFlag) && !StopByUser)
    CheckOnFreeComBuffer ();

  DecodeANDFreeSemaphor ();
  byte *pBuffer = (byte*)Buffer;

  if (ErrAudit)
    printf ("|--Cluster--|-----I----|----II----|----III---|--Summary--|\n");

  for (int i = 0; i < MAX_AS_COM; i++)
   {
    if (ComTable [i].Semaphore == END_DEC_COM)
     {
      imemcpy (pBuffer, ComTable [i].Buffer, SizeOrigCluster);

      if (ErrAudit)
       {
        printf ("|  %8d | %8d | %8d | %8d |  %8d |\n", i,
                 ComTable [i].ES.OrdinaryErr,
                 ComTable [i].ES.DupletErr,
                 ComTable [i].ES.TripletErr,
                 ComTable [i].ES.OrdinaryErr +
                (ComTable [i].ES.DupletErr * 2) +
                (ComTable [i].ES.TripletErr * 3));
       }

      pBuffer += SizeOrigCluster;
      FreeSemaphor (i);
     }
   }

  PrintRFStatisics ();

  if (!StopByUser)
    WaitPushKey ();
}

void ParseCommandLineParametrs (int argc, char *argv[])
{
  PrintWorkStatusBegin ("Working mode is");

  if (argc > 1)
   {
    ReceiveAct = !!stricmp (argv [1], "s");
    TVSetFlag = !stricmp (argv [1], "t");
   }
  else
    ReceiveAct = true;

  if (TVSetFlag)
    PrintWorkStatusEnd ("TVSet");
  else
  if (ReceiveAct)
    PrintWorkStatusEnd (" Receive");
  else
    PrintWorkStatusEnd (" Transmit");
}

#define TEST_FILE_NAME  "..\\labuda"

int main (int argc, char *argv [])
{
  printf (Rights);
  LoadIniFile ();
  ParseCommandLineParametrs (argc, argv);
  InitArvid ();
  GD.Initialize ();

  if (TVSet ())
   {
    FreeArvid ();
    return 0;
   }

  Buffer = (word*) new byte [BUFF_FRAMES * SizeDataFrame];

  if (Buffer)
    InitBuffer (0xAAAA, Buffer, BUFF_FRAMES); //0xCCCC

  if (ArvidInsert && AsyncFlag && !ReceiveAct)
   {
    InitSyncroCommand ();
    SendSTest ();
    FreeSyncroCommand ();
    delete Buffer;
    FreeArvid ();
    return 0;
   }

  InitAsynCommand ();

  if (ArvidInsert)
    if (ReceiveAct)
      //ReceiveFile (TEST_FILE_NAME);
      ReceiveATest ();
    else
      //SendFile (TEST_FILE_NAME);
      SendATest ();

  if (!StopByUser & ViewBufferFlag)
    ViewBuffer (Buffer, BUFF_FRAMES);

  FreeAsynCommand ();
  delete Buffer;
  FreeArvid ();
  return 0;
}
