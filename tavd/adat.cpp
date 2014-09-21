#include <conio.h>
#include <stdio.h>

#include "..\MIKM300\INCLUDE\mikmod.h"
#include "copyrit.h"
#include "mytypes.h"
#include "audio.h"
#include "adat.h"

#define ESCAPE 27

int main (int argc, char *argv [])
{
  bool quit = 0;
  SAMPLE *explode;

  printf (Rights);
  Audio AU;

  AU.Open (3);

  if ((explode = WAV_LoadFN ("explode.wav")) == NULL)
   {
    puts ("Error: Could not load EXPLODE.WAV!");
    return 1;
   }

  explode->flags |= SF_LOOP;
  explode->loopstart = 0;
  explode->loopend = explode->length;

  AU.BeginPlay ();

  puts ("Keys 1,2,3 - play EXPLOSION.WAV at left, middle, and right panning.");
  puts ("  Press 'ESCAPE' to quit.");

  while (!quit)
   {
    if (kbhit())
       {
	switch (getch())
	 {
	  case ESCAPE: quit = 1; break;

	  // Explosion Playback Triggers - 1, 2, 3
	  case '1': Voice_SetPanning (MikMod_PlaySample (explode,0,0), PAN_LEFT); break;
	  case '2': Voice_SetPanning (MikMod_PlaySample (explode,0,0), PAN_CENTER); break;
	  case '3': Voice_SetPanning (MikMod_PlaySample (explode,0,0), PAN_RIGHT); break;
	 }
       }
     printf ("Voice Position = %d\n", (dword)Voice_GetPosition (0));
     delay (10);
     MD_Update ();
   }

  AU.FinishPlay ();
  WAV_Free (explode);
  AU.Close ();
  return 0;
}