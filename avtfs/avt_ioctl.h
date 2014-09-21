#ifndef __AVT_IOCTL_H__
#define __AVT_IOCTL_H__

typedef struct
{
       /* Hardware parametrs */
       byte ArvidInsert;
       word ArvidModel;
       word ArvidModelDetect;

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

       dword MemorySize;
       dword HArvFrame;
       dword WArvFrame;
       dword NumArvFrame;
       dword SizeArvFrame;

       /* interrupt */       
       vint countintr;       
       vint SubFrame;
       vint SendFrameCount;
       vint ReceiveFrameCount;
} avt_param;

/* Ioctl definitions */
#define AVT_IOC_MAGIC 'S'

#define AVT_SET_PHASE       _IO(AVT_IOC_MAGIC, 0)
#define AVT_SET_SUBPHASE    _IO(AVT_IOC_MAGIC, 1)
#define AVT_SET_AUTOPHASE   _IO(AVT_IOC_MAGIC, 2)
#define AVT_RESET_AUTOPHASE _IO(AVT_IOC_MAGIC, 3)
#define AVT_GET_PARAM       _IOR(AVT_IOC_MAGIC, 4, avt_param)

#define AVT_IOC_MAXNR 4

#endif
