
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <linux/errno.h>

#include "defines.h"
#include "arvid.h"
#include "avd_ioctl.h"

#define GENERATE_TIME (1 * 60) /* 1 min */
#define VERBOSE 1    /* messaging output level */
#define NO_VERBOSE 0
#define CURRENT_MODE 'h'

int avd_get_param (int hFile, avd_param *pavdp, int verbose_flag)
{
  int rc = ioctl (hFile, AVT_GET_PARAM, pavdp);
  
  if (rc)
   {
    printf ("Cann't make ioctl request GET_PARAM, err %d\n!", rc);
    return rc;
   }    
  
  if (verbose_flag == NO_VERBOSE)
    return rc;
  
  printf ("Hardware parametrs:\n"
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

int avd_write_fields (char *dev_path)
{
  avd_param avdp;
  int rc = 0, hFile = open (dev_path, O_WRONLY, FILE_ACCESS);
  
  if (hFile == -1)
   {
    printf ("Cann't open device file %s!\n", dev_path);
    return 1;
   }
   
  rc = avd_get_param (hFile, &avdp, NO_VERBOSE);
   
  if (rc)
   {
    close (hFile);
    return rc;
   }    
  
  int frame_buffer_size = avdp.SizeArvFrame * 2;
  int max_x = ((CURRENT_MODE == 'l')?(WFRAME_1031 * 2):(WFRAME_105X * 2)),max_y = ((CURRENT_MODE == 'l')?(HFRAME_1031):(HFRAME_105X));
  int x,y;

  // if buffer len cannot equ x * y in bytes
  if (frame_buffer_size != max_x * max_y) {
    close (hFile);
    return -ENOMEM; // ;-)
  }

  char *frame_buffer = malloc (frame_buffer_size);
  
  if (!frame_buffer)
   {
    close (hFile);
    return -ENOMEM;
   }
  
  int i = 0, frame_counter = 0;
  
  while (i < frame_buffer_size)
   frame_buffer [i++] = 0x5;

#if 0 
  // clean high semiscreen
  for( y = 0; y < 12; y++) {
    frame_buffer[y * max_x] = 0x55;
    for (x = 1; x < max_x; x++)
      frame_buffer[(y * max_x) + x] = 0;
  }

  // set low semiscreen to vertical line
  for( ; y < max_y; y++) {
    frame_buffer[y * max_x] = 0x55;
    for (x = 1; x < max_x; x++)
      frame_buffer[(y * max_x) + x] = 0x3;
  }
#endif

  i = PAL_FPS * GENERATE_TIME; /* one minutes generate */

  while (i)
   {
    rc = write (hFile, frame_buffer, frame_buffer_size);
    frame_counter++;
    
    if (!(frame_counter % PAL_FPS))
      printf ("Write to ArVid %d frames(fields).\n", frame_counter);
      
    i--;
   }

  sleep (1);
  rc = avd_get_param (hFile, &avdp, VERBOSE);
   
  free (frame_buffer);
  close (hFile);
  
  return rc;
}

int main (int argc, char **argv)
{
  char dev_path [MAX_PATH], mode = CURRENT_MODE;
  int unit = 0;
  
  sprintf (dev_path, "/dev/%s%1i%c", MOD_NAME, unit, mode);
  printf ("dev_path: %s\n", dev_path);
    
  return avd_write_fields (dev_path);
}
