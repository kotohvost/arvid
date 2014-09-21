
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

#define TEST_FRAMES 23

int avd_get_param (int hFile, avd_param *pavdp, int debug_flag)
{
  int rc = ioctl (hFile, AVT_GET_PARAM, pavdp);
  
  if (rc)
   {
    printf ("Cann't make ioctl request GET_PARAM, err %d\n!", rc);
    return rc;
   }    
  
  if (!debug_flag)
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

int avd_read_test (char *dev_path)
{
  avd_param avdp;
  int rc = 0, hFile = open (dev_path, O_RDONLY, FILE_ACCESS);
  
  if (hFile == -1)
   {
    printf ("Cann't open device file %s!\n", dev_path);
    return 1;
   }
   
  unlink ("./read.tmp");
  int hOutFile = open ("./read.tmp", O_WRONLY | O_CREAT, FILE_ACCESS);
  
  if (hOutFile == -1)
   {
    printf ("Cann't open device file %s!\n", "./read.tmp");
    return 1;
   }
   
  rc = avd_get_param (hFile, &avdp, 0);
   
  if (rc)
   {
    close (hFile);
    close (hOutFile);
    return rc;
   }    
  
  int frame_buffer_size = TEST_FRAMES * avdp.SizeArvFrame * 2;
  char *frame_buffer = malloc (frame_buffer_size);

  if (!frame_buffer)
   {
    close (hFile);
    close (hOutFile);
    return -ENOMEM;
   }

  rc = read (hFile, frame_buffer, frame_buffer_size);
  write (hOutFile, frame_buffer, rc);
  
  rc = avd_get_param (hFile, &avdp, 0);
   
  free (frame_buffer);
  close (hOutFile);
  close (hFile);
  
  return rc;
}

int avd_write_test (char *dev_path)
{
  avd_param avdp;
  int rc = 0, hFile = open (dev_path, O_WRONLY, FILE_ACCESS);
  
  if (hFile == -1)
   {
    printf ("Cann't open device file %s!\n", dev_path);
    return 1;
   }
   
  rc = avd_get_param (hFile, &avdp, 0);
   
  if (rc)
   {
    close (hFile);
    return rc;
   }    
  
  int frame_buffer_size = avdp.SizeArvFrame * 2;
  char *frame_buffer = malloc (frame_buffer_size);
  
  if (!frame_buffer)
   {
    close (hFile);
    return -ENOMEM;
   }
  
  int i = 0;
  
  while (i < frame_buffer_size) 
    frame_buffer [i++] = 0xAA;

  i = 50;
  
  while (i)
   {
    rc = write (hFile, frame_buffer, frame_buffer_size);
    i--;
   }

  sleep (1);
  rc = avd_get_param (hFile, &avdp, 1);
   
  free (frame_buffer);
  close (hFile);
  
  return rc;
}

int main (int argc, char **argv)
{
  char dev_path [MAX_PATH], mode = 'h';
  int unit = 0, rc = 0;
  
  sprintf (dev_path, "/dev/%s%1i%c", MOD_NAME, unit, mode);
  printf ("dev_path: %s\n", dev_path);
  
  rc = avd_read_test (dev_path);
  
  if (rc)
    return rc;

  mode = 'i';
  sprintf (dev_path, "/dev/%s%1i%c", MOD_NAME, unit, mode);
  printf ("dev_path: %s\n", dev_path);
    
  rc = avd_write_test (dev_path);
  
  if (rc)
    return rc;
    
  return 0;
}
