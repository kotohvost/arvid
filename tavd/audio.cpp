#include "..\MIKM300\INCLUDE\mikmod.h"
#include "mytypes.h"
#include "audio.h"

void errorhandler(void)
// When called, the following global variables will have been set:
//  _mm_errno	  - INTEGER; the error that occured [listed in mmio.h]
//  _mm_critical  - BOOLEAN; set to TRUE if an error occurs within
//		    MikMod_Init() or when trying to start playing of a
//		    module - in which case MikMod automatically shuts
//		    down and impliments the NoSound driver.
{
    printf("MikWav Error: %s\n\n",_mm_errmsg[_mm_errno]);

    if(_mm_critical)
      exit(_mm_errno);	   // Mod player not much use without sound, so
			   // just exit the program.
}

Audio::Audio ()
{
  // Register the MikMod error handler.  If any errors occur within the
  // MikMod libraries (including the ngetopt and all _mm_ functions),
  // the given error handler will be called and _mm_errno set. [see
  // errorhandler(void) above]
  MikMod_RegisterErrorHandler (errorhandler);

  // ===================================================================
  // Initialize MikMod (initializes soundcard and associated mixers, etc)
  md_mixfreq	  = 44100;	      // standard mixing freq
  //md_dmabufsize   = 4000;		// DMA buffer size of 4 seconds (default = 50 millseconds)
  md_device	  = 0;		      // standard device: autodetect
  md_volume	  = 96; 	      // driver volume (max 128)
  md_musicvolume  = 128;	      // music volume (max 128)
  md_sndfxvolume  = 128;	      // sound effects volume (max 128)
  md_pansep	  = 128;	      // panning separation (0 = mono, 128 = full stereo)
  md_reverb	  = 0;		      // Reverb (max 15)

  // =======================================
  // Register all std loaders and drivers for use.
  MikMod_RegisterAllDrivers ();
  printf ("Load sound drivers.\n");
  AudioInitialize = true;
}

Audio::~Audio ()
{ Close (); }

bool Audio::Open (int Voices)
{
  if (!AudioInitialize)
    return false;

  MikMod_Init ();
  MikMod_SetNumVoices (0, Voices);   // 0 music voices; 4 sound effects voices!

  printf ("Using %s for %d bit %s %s sound at %u Hz\n\n",
	   md_driver->Name,
	   (md_mode&DMODE_16BITS) ? 16:8,
	   (md_mode&DMODE_INTERP) ? "interpolated" : "normal",
	   (md_mode&DMODE_STEREO) ? "stereo" : "mono",
	   md_mixfreq);

  return true;
}

void Audio::BeginPlay ()
{
  if (!AudioInitialize)
    return;

  MikMod_EnableOutput ();
}

void Audio::FinishPlay ()
{
  if (!AudioInitialize)
    return;

  MikMod_DisableOutput ();
}

void Audio::Close ()
{
  if (!AudioInitialize)
    return;

  MikMod_Exit();
}

bool Audio::StreamInit (dword speed, dword flags)
{
  if (!AudioInitialize)
    return false;

  return true;
}

void Audio::StreamExit (void)
{
  if (!AudioInitialize)
    return;

}

void Audio::Stream_Update (void)
{
  if (!AudioInitialize)
    return;
}

void Audio::StreamSetSpeed (dword speed)
{
  if (!AudioInitialize)
    return;
}

dword Audio::StreamGetPosition (void)
{
  if (!AudioInitialize)
    return (dword)-1;

  return 0;
}