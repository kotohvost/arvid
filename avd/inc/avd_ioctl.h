
#ifndef __AVT_IOCTL_H__
#define __AVT_IOCTL_H__

/* structure for determinate ArVid module status, used in ioctl requests */
typedef struct
{
       /* Hardware parametrs */
       byte ArvidInsert; /* debug mode avaible without hardware */
       word ArvidModel;  /* model set by applet according .ini file */
       word ArvidModelDetect; /* module detects boards marks 1031, 1051, 1052 */

       word Phase;
       word SubPhase;
       word AutoPhase;
       int  BasePort;
       int  ArvidIRQ;

       word ValueRK;
       word Arv105X;
       word DataMode;
       word HalfDupMode;
       word HDensity;
       word FrameMask;

       dword MemorySize;   /* detected memory size */
       dword HArvFrame;
       dword WArvFrame;
       dword NumArvFrame;  /* MemorySize / SizeArvFrame */
       dword SizeArvFrame; /* HArvFrame * WArvFrame */
       dword FrameCounter; /* Free/busy frames */

       /* interrupt */       
       vint countintr;     /* Interrupts counter */
       vint SubFrame;      /* SubFrames counter */
       vint SendFrameCount; /* Sending Frames counter */
       vint ReceiveFrameCount; /* Receiving Frames counter */
} avd_param;

/* Ioctl definitions */
#define AVT_IOC_MAGIC 'S'  /* ioctl command base number */

#define AVT_SET_PHASE       _IO(AVT_IOC_MAGIC, 0)
#define AVT_SET_SUBPHASE    _IO(AVT_IOC_MAGIC, 1)
#define AVT_SET_AUTOPHASE   _IO(AVT_IOC_MAGIC, 2)
#define AVT_RESET_AUTOPHASE _IO(AVT_IOC_MAGIC, 3)
#define AVT_GET_PARAM       _IOR(AVT_IOC_MAGIC, 4, avd_param)

#define AVT_IOC_MAXNR 4

#endif
