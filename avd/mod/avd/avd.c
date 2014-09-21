/* 
 *     Arvid 105x, 103x tape driver for Linux, project TeleArVid
 *     Author: Slava Levtchenko <sl@lgsoftlab.ru>, <sl@eltrast.ru>
 *
 *     This program is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU General Public License as
 *     published by the Free Software Foundation; either version 2, or 
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software Foundation,
 *     Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

//#if (__GNUC__ > 2)
//#  error Your compiler is bad for compile wait_queue_head_t!
//#endif

#include <linux/config.h>
#include <linux/module.h>

#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/delay.h>  /* udelay */
#include <linux/slab.h>   /* kmalloc (, GFP_KERNEL) */
#include <linux/ioport.h> /* ioctl */
#include <linux/interrupt.h>/* irq probing */
#include <linux/tqueue.h> /* queue */

#include "sysdep.h"
#include "defines.h"
#include "arvid.h"
#include "avd_ioctl.h"

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

       /* system specific */
       struct resource *presource;

       /* kernel buffer */
       word *cbuf;
       vint szbuf; /* used flag + size of buffer */
       
       /* interrupt */
       vint OpenUnit;
       vint countintr;
       struct semaphore FreeForApp;
       struct semaphore FreeForOpen;
       struct semaphore FreeForRelease;
       wait_queue_head_t wait;
       
       vint SubFrame;
       byte ResetSeqFlag;
       vint SendFrameCount;
       vint ReceiveFrameCount;
} avd_unit;

static avd_unit avdtab [MAX_AVT]; /* tape info by unit number */

/* Default hardware parametrs */
static byte insert[MAX_AVT];
static word model[MAX_AVT], defArvidModelDetect[MAX_AVT];

static word phase[MAX_AVT], subphase[MAX_AVT], autophase[MAX_AVT];
static int  port[MAX_AVT], irq[MAX_AVT];

static word defValueRK[MAX_AVT], defArv105X[MAX_AVT], defDataMode[MAX_AVT];
static word defHalfDupMode[MAX_AVT], defHDensity[MAX_AVT], defFrameMask[MAX_AVT];

static dword defMemorySize[MAX_AVT], defHArvFrame[MAX_AVT], defWArvFrame[MAX_AVT];
static dword defNumArvFrame[MAX_AVT], defSizeArvFrame[MAX_AVT];

MODULE_PARM(insert, "0-4b");
MODULE_PARM(model, "0-4h");
MODULE_PARM(irq, "0-4i");
MODULE_PARM(port, "0-4i");
MODULE_PARM(autophase, "0-4h");
MODULE_PARM(subphase, "0-4h");
MODULE_PARM(phase, "0-4h");

MODULE_AUTHOR("Vyacheslav I. Levtchenko");
MODULE_DESCRIPTION("ArVid 103X-105X driver by Sl");
MODULE_SUPPORTED_DEVICE(MOD_NAME);

static inline word GetArvidRawFrameCount (avd_unit *punit)
{
  word status = inw (RS1);

  /* 1110 0111 */
  if (punit->ArvidModelDetect >= 1051)
    status = ((status & LOW_COUNT_FRAME_MASK) | ((status & HIGH_COUNT_FRAME_MASK) >> 2)) & punit->FrameMask;
  else
    status &= LOW_COUNT_FRAME_MASK;

  return status;
}

static inline word GetArvidFrameCount (avd_unit *punit)
{
#if 0
  word ArvStatus = inw (RS1);
  return ArvStatus = ((ArvStatus & LOW_COUNT_FRAME_MASK) | ((ArvStatus & HIGH_COUNT_FRAME_MASK) >> 2)) & punit->FrameMask;
#endif

  word ArvStatus;
  
  while (1)
   {
    ArvStatus = GetArvidRawFrameCount (punit);
    
    if (GetArvidRawFrameCount (punit) == ArvStatus)
      break;
   }
   
  return ArvStatus;
}

static avd_param *avd_get_param (avd_param *pavdp, avd_unit *punit)
{
  /* Hardware parametrs */
  pavdp->ArvidInsert      = punit->ArvidInsert;
  pavdp->ArvidModel       = punit->ArvidModel;
  pavdp->ArvidModelDetect = punit->ArvidModelDetect;

  pavdp->Phase            = punit->Phase;
  pavdp->SubPhase         = punit->SubPhase;
  pavdp->AutoPhase        = punit->AutoPhase;
  pavdp->BasePort         = punit->BasePort;
  pavdp->ArvidIRQ         = punit->ArvidIRQ;

  pavdp->ValueRK          = punit->ValueRK;
  pavdp->Arv105X          = punit->Arv105X;
  pavdp->DataMode         = punit->DataMode;
  pavdp->HalfDupMode      = punit->HalfDupMode;
  pavdp->HDensity         = punit->HDensity;
  pavdp->FrameMask        = punit->FrameMask;

  pavdp->MemorySize       = punit->MemorySize;
  pavdp->HArvFrame        = punit->HArvFrame;
  pavdp->WArvFrame        = punit->WArvFrame;
  pavdp->NumArvFrame      = punit->NumArvFrame;
  pavdp->SizeArvFrame     = punit->SizeArvFrame;
  pavdp->FrameCounter     = GetArvidFrameCount (punit);  

  /* interrupt */       
  pavdp->countintr        = punit->countintr;
  pavdp->SubFrame         = punit->SubFrame;
  pavdp->SendFrameCount   = punit->SendFrameCount;
  pavdp->ReceiveFrameCount= punit->ReceiveFrameCount;
  
  return pavdp;
}

static void SetDefAvtSettings (avd_unit *punit, int num_dev)
{
  /* Hardware parametrs */
  punit->ArvidInsert = insert[num_dev]?insert[num_dev] & 1:1;
  punit->ArvidModel  = model[num_dev]?model[num_dev]:0;
  punit->ArvidModelDetect = defArvidModelDetect[num_dev]?defArvidModelDetect[num_dev]:1031;

  punit->Phase     = phase[num_dev]?(phase[num_dev] & 7) << 5:5 << 5;
  punit->SubPhase  = subphase[num_dev]?(subphase[num_dev] & 3) << 11:0;
  punit->AutoPhase = autophase[num_dev]?(autophase[num_dev] & 1) << 2:0; /* SET_AUTO_PHASE */
  punit->BasePort  = port[num_dev]?port[num_dev]:0x390;
  punit->ArvidIRQ  = irq[num_dev]?irq[num_dev]:11;

  punit->ValueRK   = defValueRK[num_dev]?defValueRK[num_dev]:0;
  punit->Arv105X   = defArv105X[num_dev]?defArv105X[num_dev]:0;
  punit->DataMode  = defDataMode[num_dev]?defDataMode[num_dev]:SET_DATA;
  punit->HalfDupMode = defHalfDupMode[num_dev]?defHalfDupMode[num_dev]:0; /* transmit */
  punit->HDensity  = defHDensity[num_dev]?defHDensity[num_dev]:0;
  punit->FrameMask = defFrameMask[num_dev]?defFrameMask[num_dev]:0;

  punit->MemorySize= defMemorySize[num_dev]?defMemorySize[num_dev]:0;
  punit->HArvFrame = defHArvFrame[num_dev]?defHArvFrame[num_dev]:HFRAME_1031;
  punit->WArvFrame = defWArvFrame[num_dev]?defWArvFrame[num_dev]:WFRAME_1031;
  punit->NumArvFrame = defNumArvFrame[num_dev]?defNumArvFrame[num_dev]:8;
  punit->SizeArvFrame = defSizeArvFrame[num_dev]?defSizeArvFrame[num_dev]:WFRAME_1031 * HFRAME_1031;

  /* system specific */
  punit->presource = NULL;

  /* kernel buffer */
  punit->szbuf = 0;
  punit->cbuf = NULL;
  
  /* interrupt */  
  punit->OpenUnit = 0;
  punit->countintr = 0;
  init_MUTEX (&punit->FreeForApp);
  /* init_MUTEX (&punit->FreeForOpen); */ /* make it early */
  init_MUTEX (&punit->FreeForRelease);
  init_waitqueue_head (&punit->wait);

  punit->SubFrame = 0;
  punit->ResetSeqFlag = 0;
  punit->SendFrameCount = 0;
  punit->ReceiveFrameCount = 0;
}

#if 0
static void dump (byte *buf, int len)
{
  printk (KERN_CRIT "_offset_|__0__1__2__3__4__5__6__7|_8__9__A__B__C__D__E__F|______ASCII_____|\n");

  while (len)
   {
    printk (KERN_CRIT "%8X| ", buf);

    if (len >= 16)
     {
      int count = 16;
      
      printk (KERN_CRIT "%02X %02X %02X %02X %02X %02X %02X %02X|%02X %02X %02X %02X %02X %02X %02X %02X|",
              buf[0], buf[1], buf[2],  buf[3],  buf[4],  buf[5],  buf[6],  buf[7], buf[8], buf[9], buf[10],
	      buf[11], buf[12], buf[13], buf[14], buf[15]);

      while (count--)
       {
        /*
        if (*buf >= 0x20)
          printk (KERN_CRIT "%c", *buf);
        else
          printk (KERN_CRIT "%c", '.');
         */
        buf++;
       }

      printk (KERN_CRIT "|\n");
      len -= 16;
     }
    else
     {
      int last = 16 - len;
      int count = 0, lastcount;

      while (len)
       {
        if (count != 7)
          printk (KERN_CRIT "%02X ", buf[count++]);
        else
          printk (KERN_CRIT "%02X|", buf[count++]);

        len--;
       }

      lastcount = count;

      while (last)
       {
        if (lastcount != 7 && lastcount != 15)
          printk (KERN_CRIT "   ");
        else
          printk (KERN_CRIT "  |");

        lastcount++;
        last--;
       }

      last = 16 - count + 1;
      lastcount = count - 1;

      while (count--)
       {
        /*
        if (*buf >= 0x20)
          printk (KERN_CRIT "%c", *buf);
        else
          printk (KERN_CRIT "%c", '.');
         */
        buf++;
       }

      while (last)
       {
        if (lastcount != 15)
          printk (KERN_CRIT " ");
        else
          printk (KERN_CRIT "|");

        lastcount++;
        last--;
       }

      printk (KERN_CRIT "\n");
     }
   }
}

static void SendFrameFromInt (avd_unit *punit, word *buf)
{
  int LastWordsOfBlock = 0x100 - 1 - punit->ResetSeqFlag;
  int LastWords = punit->SizeArvFrame - 1 - punit->ResetSeqFlag;
  buf += 1 + punit->ResetSeqFlag;
  punit->ResetSeqFlag = 0;
  /* dump ((byte*)buf, 0x100); */
  /* printk (KERN_CRIT "punit->ValueRK %04Xh\n", punit->ValueRK); */
  
  while (LastWords)
   {
    outw (punit->SubFrame++, RA);

    if (LastWords < LastWordsOfBlock)
      LastWordsOfBlock = LastWords;

    outsw (RD_INC, buf, LastWordsOfBlock);
    buf += LastWordsOfBlock;

    LastWords -= LastWordsOfBlock;
    LastWordsOfBlock = 0x100;
   }

  punit->SendFrameCount++;
  outw (punit->ValueRK | SET_INC_BUFFER, RK); /* increment frame count */
}

static void ReceiveFrameFromInt (avd_unit *punit, word *buf)
{
  int LastWordsOfBlock = 0x100;
  int LastWords = punit->SizeArvFrame;

  while (LastWords)
   {
    outw (punit->SubFrame++, RA);
    inw (RD); /* Arvid have gluck - point 2 of hints - that work */

    if (LastWords < LastWordsOfBlock)
      LastWordsOfBlock = LastWords;

    insw (RD_INC, buf, LastWordsOfBlock);
    buf += LastWordsOfBlock;

    LastWords -= LastWordsOfBlock;
   }

  punit->ReceiveFrameCount++;
  outw (punit->ValueRK | SET_INC_BUFFER, RK); /* increment frame count */
}
#endif

static void SendFrame (avd_unit *punit, word *buf)
{
  int LastWordsOfBlock = 0x100;
  int LastWords = punit->SizeArvFrame;
  /* dump ((byte*)buf, 0x100); */
  /* printk (KERN_CRIT "punit->ValueRK %04Xh\n", punit->ValueRK); */
  
  while (LastWords)
   {
    outw (punit->SubFrame++, RA);

    if (LastWords < LastWordsOfBlock)
      LastWordsOfBlock = LastWords;

    outsw (RD_INC, buf, LastWordsOfBlock);
    buf += LastWordsOfBlock;

    LastWords -= LastWordsOfBlock;
   }

  punit->SendFrameCount++;
  outw (punit->ValueRK | SET_INC_BUFFER, RK); /* increment frame count */
}

static void ReceiveFrame (avd_unit *punit, word *buf)
{
  int LastWordsOfBlock = 0x100 - 1, first_pass = 1;
  int LastWords = punit->SizeArvFrame;

  while (LastWords)
   {
    outw (punit->SubFrame++, RA);
    inw (RD); /* Arvid have gluck - point 2 of hints - that work */
    
    if (LastWords < LastWordsOfBlock)
      LastWordsOfBlock = LastWords;

    if (first_pass)
     {
      inw (RD_INC);
      first_pass = 0;
     }
     
    insw (RD_INC, buf, LastWordsOfBlock);
    buf += LastWordsOfBlock;
    
    LastWords -= LastWordsOfBlock;
    LastWordsOfBlock = 0x100;
   }

  punit->ReceiveFrameCount++;
  outw (punit->ValueRK | SET_INC_BUFFER, RK); /* increment frame count */
}

static void ApplyMode (avd_unit *punit)
{
  if (punit->HalfDupMode & SET_RECIVE)
    punit->SubFrame = 0; /* set subframe on current frame */
  else
    punit->SubFrame = 0x10; /* set subframe on next frame */
    
  /* connect PC to VM, reset seq, set operate "data" */
  outw (punit->ValueRK = punit->DataMode    |
                         punit->HalfDupMode |
			 punit->AutoPhase   |
			 punit->Phase       |
			 punit->SubPhase    |
			 punit->Arv105X     |
			 punit->HDensity    |
			 SET_RESET_SEQ      |
			 LINK_VM2PC, RK);
			 
  punit->ResetSeqFlag = 1;
  outw (punit->ValueRK &= RESET_RESET_SEQ, RK);
}

static void avd_proc_tasklet (unsigned long data)
{
  word fr_count;
  avd_unit *punit = (avd_unit *)data;
  
  if (!waitqueue_active (&punit->wait))
   {
/*    printk (KERN_CRIT "avd:handler:sizeof(wait) %i\n", sizeof(punit->wait) ); */
/*    printk (KERN_CRIT "avd:compiler_version: %i\n", __GNUC__); */
    return;
   }
   
  fr_count = GetArvidFrameCount (punit);
  
  if (punit->ValueRK & SET_RECIVE)
   {
    if (fr_count)
      wake_up_interruptible (&punit->wait); /* wake any reading process */
   }    
  else
   {
    if (fr_count < punit->NumArvFrame - 1)
      wake_up_interruptible (&punit->wait); /* wake any writing process */
   }
}

DECLARE_TASKLET (avd_tasklet, avd_proc_tasklet, 0);

static void Handler (int irq, void *dev_id, struct pt_regs *regs)
{
  avd_unit *punit = dev_id;
  punit->countintr++;
  
  if (!punit->OpenUnit)
    return;

  avd_tasklet.data = (unsigned long)punit;
  tasklet_schedule(&avd_tasklet);
}

static int InitInterrupt (avd_unit *punit)
{
  int rc = 0;
  
  /* unsigned long mask = probe_irq_on (); int probe_irq_off (mask); */
  rc = request_irq (punit->ArvidIRQ, Handler, SA_INTERRUPT, MOD_NAME, punit);

  if (rc)
   {
    printk (KERN_CRIT "request_irq: can't get assigned irq %i \n", punit->ArvidIRQ);
    return rc;
   }

  return rc;
}

static int FreeInterrupt (avd_unit *punit)
{
  int rc = 0;

  free_irq (punit->ArvidIRQ, punit);
  return rc;
}

static void SetArvMemory (avd_unit *punit, int subframes, word WordMask)
{
  dword i, j;
  
  /* connect PC to VM, reset seq, set operate "data" */
  punit->ValueRK = (punit->ValueRK | SET_DATA | LINK_VM2PC | SET_RESET_SEQ | SET_1052) & SET_TRANSMIT;
  outw (punit->ValueRK, RK);
  
  punit->ValueRK &= RESET_RESET_SEQ;
  outw (punit->ValueRK, RK);

  for (i = 0; i <= subframes; i++) /* subcadr count */
   {
    outw (i, RA); /* set subcadr */
    
    for (j = 0; j <= 0xff; j++)
      outw (WordMask, RD_INC);
   }
}

#if 0
static dword FullTestArvMemory (avd_unit *punit)
{
  dword i, j, MemCount = 0;
  
  SetArvMemory (punit, MAX_AVT_SUBFRAMES, 0xCCCC);
  for (i = 0; i <= MAX_AVT_SUBFRAMES; i++) /* subcadr count */
   {
    outw (i, RA); /* set subcadr */
    inw (RD);     /* Arvid have gluck - point 2 of hints - that work */
    
    for (j = 0; j <= 0xff; j++, MemCount++)
     {
      if (inw (RD) != 0xCCCC)
       {
        MemCount <<= 1;
        return punit->MemorySize = MemCount;
       }	

      outw (0, RD_INC);
     }
   }

  MemCount <<= 1;
  return punit->MemorySize = MemCount;
}
#endif

static dword TestArvMemory (avd_unit *punit)
{
  dword i, MemCount = 0;
  
  /* connect PC to VM, reset seq, set operate "data" */
  punit->ValueRK = (punit->ValueRK | SET_DATA | LINK_VM2PC | SET_RESET_SEQ | SET_1052) & SET_TRANSMIT;
  outw (punit->ValueRK, RK);
  
  punit->ValueRK &= RESET_RESET_SEQ;
  outw (punit->ValueRK, RK);

  for (i = 0; i <= MAX_AVT_SUBFRAMES; i += 16) /* subcadr count */
   {
    outw (i, RA); /* set subcadr */
    outw (0xCCCC, RD);
   }

  for (i = 0; i <= MAX_AVT_SUBFRAMES; i += 16) /* subcadr count */
   {
    outw (i, RA); /* set subcadr */
    inw (RD);     /* Arvid have gluck - point 2 of hints - that work */

    if (inw (RD) != 0xCCCC)
     {
      MemCount <<= 1;
      return punit->MemorySize = MemCount;
     }

    MemCount += 0x100 << 4;
    outw (0, RD);
   }

  MemCount <<= 1;
  return punit->MemorySize = MemCount;
}

static int AvtProbe (avd_unit *punit)
{
  punit->NumArvFrame = TestArvMemory (punit)?punit->MemorySize / 0x2000:0;

  if (punit->NumArvFrame > 32
   && punit->NumArvFrame <= 64)
    punit->ArvidModelDetect = 1052;
  else
  if (punit->NumArvFrame > 8
   && punit->NumArvFrame <= 32)
    punit->ArvidModelDetect = 1051;
  else
  if (punit->NumArvFrame
   && punit->NumArvFrame <= 8)
    punit->ArvidModelDetect = 1031;
  else
    punit->ArvidModelDetect = 0;

  return punit->ArvidModelDetect;
}

static void SetMode (avd_unit *punit, word flags)
{
  int i;
  word mask = 1;

  punit->HalfDupMode = flags & SET_RECIVE?SET_RECIVE:0;
  punit->ArvidModel = punit->ArvidModel?punit->ArvidModel:punit->ArvidModelDetect;

  if (flags & SET_DATA)
   {
    punit->DataMode = SET_DATA;

    if (punit->ArvidModel == 1031)
     {
      punit->Arv105X = 0;
      punit->HDensity = 0;

      /* Override detected value! */
      punit->NumArvFrame = 8;
      punit->HArvFrame = HFRAME_1031;
      punit->WArvFrame = WFRAME_1031;
     }
    else
     {
      punit->Arv105X = SET_1052;
      punit->HDensity = flags & SET_325K;

      if (punit->HDensity)
       {
        punit->HArvFrame = HFRAME_105X;
        punit->WArvFrame = WFRAME_105X;
       }
      else
       {
        punit->HArvFrame = HFRAME_1031;
        punit->WArvFrame = WFRAME_1031;
       }
     }

    if (!(flags & SET_RECIVE))
     {
      punit->Phase     = 0; /* if phase set to 1 receive mode not set */
      punit->SubPhase  = 0; /* protect for undocuments modes */
      punit->AutoPhase = 0; /* protect for undocuments modes */
     }
   } /* end if data */
  else 
   { /* irc mode */
    punit->DataMode = 0;
    
    if (punit->ArvidModel == 1031)
      punit->Arv105X = 0;
    else
     {
      if (flags & SET_RECIVE)
        punit->Arv105X = SET_1052;
      else /* in to irc send mode set emulation */
       {   /* for perfomance optimization */
        punit->Arv105X = 0;
	
        /* Overrided detected value! */
        punit->NumArvFrame = 8;
       }
     }
    
    punit->HDensity  = 0;
    punit->Phase     = 0; /* if phase set to 1 receive mode not set */
    punit->SubPhase  = 0; /* protect for undocuments modes */
    punit->AutoPhase = 0; /* protect for undocuments modes */
    
    punit->HArvFrame = HFRAME_IRC;
    punit->WArvFrame = WFRAME_IRC;
   }

  /* Calculate FrameMask */
  punit->FrameMask = 0;
  for (i = 0; i < 8; i++, mask <<= 1)
   {
    if (punit->NumArvFrame & mask)
      break;
    else
      punit->FrameMask |= mask;
   }

  punit->SizeArvFrame = punit->WArvFrame * punit->HArvFrame;

  /* undocuments correction */
  /* irc && send -> increment frame lenght on 12 bytes */
  if (!(flags & SET_DATA) && !(flags & SET_RECIVE))
    punit->SizeArvFrame += 6;
}

static void FreeRegion (avd_unit *punit)
{
  if (punit->presource)
   {
    release_region (punit->BasePort, PORT_RANGE);
    punit->presource = NULL;
   }
}

/* SET_DATA, SET_RECIVE, SET_325K */
static int InitUnit (int unit, word flags)
{
  avd_unit *punit = &avdtab[unit];
  int rc = 0;
  
  SetDefAvtSettings (punit, unit);

  if (punit->ArvidInsert)
   {
    if ((rc = check_region (punit->BasePort, PORT_RANGE)) < 0)
     {
      printk (KERN_CRIT "I'm crashed, cann't check_region (BasePort = %Xh, Ports = %d), err = %d\n", punit->BasePort, PORT_RANGE, rc);
      return rc;
     }

    punit->presource = request_region (punit->BasePort, PORT_RANGE, MOD_NAME);

    if (!punit->presource)
     {
      printk (KERN_CRIT "I'm crashed, cann't request_region (BasePort = %Xh, Ports = %d, mod_name = %s), err = %d\n", punit->BasePort, PORT_RANGE, MOD_NAME, rc);
      return -EINVAL;
     }

    if (!AvtProbe (punit))
     {
      FreeRegion (punit);
      return -ENODEV;
     }
   }

  SetMode (punit, flags);

  if (punit->cbuf)
   {
    punit->szbuf = 0;
    kfree (punit->cbuf);
    punit->cbuf = NULL;
   }

  punit->szbuf = punit->SizeArvFrame << 1; /* maximum 128 kbytes allocate (0x80 * 0x400) */
  printk (KERN_CRIT "avd:punit->szbuf %d bytes\n", punit->szbuf);
  punit->cbuf = kmalloc (punit->szbuf, GFP_KERNEL);
  
  if (!punit->cbuf)
   {
    FreeRegion (punit);
    return -ENOMEM;
   }
  
  if (punit->ArvidInsert)
   {
    /* clear arvid memory */
    if (flags & SET_TRANSMIT)
     {
      if (flags & SET_DATA)
        SetArvMemory (punit, 1 << 4, 0xAAAA); /* set first data frame to 0xAAAA */
      else
        SetArvMemory (punit, 1 << 4, 0); /* set first irc frame to 0 */
     }
    else /* receive mode */
     {
      /* if set irc mode clear arvid memory */
      /* in receive mode skip 1st frame, he has previos data */
      if (!flags & SET_DATA)
        SetArvMemory (punit, punit->NumArvFrame << 4, 0);
     }
   
    InitInterrupt (punit);
    ApplyMode (punit);    
    punit->OpenUnit = 1;
   }

  /* printk (KERN_CRIT "avd:board %d init, memory on board %d bytes\n", unit, punit->MemorySize); */
  return rc;
}

static void FreeUnit (int unit)
{
  avd_unit *punit = &avdtab[unit];
 
  if (punit->ArvidInsert)
   { /* wait for clearing arvid cache in send mode */
    while (!(punit->ValueRK & SET_RECIVE) && GetArvidFrameCount (punit))
      interruptible_sleep_on (&punit->wait);
   
    punit->OpenUnit = 0; /* lock handler */
    FreeInterrupt (punit);

    punit->ValueRK &= LINK_VM2TV; /* Disconnect */
    outw (punit->ValueRK, RK);

    FreeRegion (punit);

    if (punit->cbuf)
     {
      punit->szbuf = 0;
      kfree (punit->cbuf);
      punit->cbuf = NULL;
     }
   }    
}

/*loff_t (*llseek)(struct file *,loff_t,int);
  ssize_t (*read)(struct file *,char *,size_t,loff_t *);
  ssize_t (*write)(struct file *,const char *,size_t,loff_t *);
  int (*readdir)(struct file *,void *,filldir_t);
  int (*ioctl)(struct inode *,struct file *,unsigned int,unsigned long);
  int (*open)(struct inode *,struct file *);
  int (*release)(struct inode *,struct file *);
  int (*flush)(struct file *);
  ssize_t (*readv)(struct file *,const struct iovec *,unsigned long,loff_t *);
  ssize_t (*writev)(struct file *,const struct iovec *,unsigned long,loff_t *);*/

#define TYPE(dev) (MINOR(dev)>>4) /* high nibble */
#define NUM(dev) (MINOR(dev)&0xf) /* low nibble */

#define AVT_LOW_SPEED   0
#define AVT_HIGH_SPEED  1
#define AVT_IRC_CONTROL 2

/* Open routine, called on every device open. */
static int avd_open (struct inode *inode, struct file *file)
{
  int unit = NUM(inode->i_rdev);
  int type = TYPE(inode->i_rdev);
  word flags = 0;

  if (unit > MAX_AVT)
    return -ENODEV;

  avd_unit *punit = &avdtab[unit];

  if (punit->OpenUnit)
    return -EFAULT;

  /* set read/write mode */
  if ((file->f_flags & O_ACCMODE) == O_WRONLY)
    flags &= SET_TRANSMIT;
  else
  if ((file->f_flags & O_ACCMODE) == O_RDONLY)
    flags |= SET_RECIVE;

  /* set speed/data/irc mode */
  if (type == AVT_LOW_SPEED)
    flags = (flags | SET_DATA) & SET_200K;
  else
  if (type == AVT_HIGH_SPEED)
    flags |= SET_325K | SET_DATA;
  else
  if (type == AVT_IRC_CONTROL)
    flags &= SET_IR;
  else
    return -ENODEV;
    
  init_MUTEX (&punit->FreeForOpen);
  if (down_interruptible (&punit->FreeForOpen))
    return -ERESTARTSYS;
    
  if (InitUnit (unit, flags) < 0)
   {
    up (&punit->FreeForOpen);
    return -EFAULT;
   }
  
  MOD_INC_USE_COUNT;
  file->private_data = punit;
  up (&punit->FreeForOpen);
  return 0;
}

static int avd_release (struct inode *inode, struct file *file)
{
  int unit = NUM(inode->i_rdev);
/*  int type = TYPE(inode->i_rdev); */

  if (unit > MAX_AVT)
    return -ENODEV;

  avd_unit *punit = &avdtab[unit];

  if (!punit->OpenUnit)
    return -EFAULT;
    
  if (down_interruptible (&punit->FreeForRelease))
    return -ERESTARTSYS;

  if (!file->private_data)
   {
    up (&punit->FreeForRelease);
    return -EFAULT;
   }
  else
    file->private_data = NULL;

  FreeUnit (unit);

  MOD_DEC_USE_COUNT;
  up (&punit->FreeForRelease);
  return 0;
}

static int avd_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
  int unit = NUM(inode->i_rdev);
/*  int type = TYPE(inode->i_rdev); */

  if (unit > MAX_AVT)
    return -ENODEV;
    
  avd_unit *punit = &avdtab[unit];

  if (!punit->OpenUnit)
    return -EFAULT;

  if (!file->private_data)
    return -EFAULT;
    
  int rc = 0;

  /* don't even decode wrong cmds: better returning  ENOTTY than EFAULT */
  if (_IOC_TYPE(cmd) != AVT_IOC_MAGIC
   || _IOC_NR(cmd) > AVT_IOC_MAXNR)
    return -ENOTTY;

  /* the type is a bitmask, and VERIFY_WRITE catches R/W
   * transfers. Note that the type is user-oriented, while
   * verify_area is kernel-oriented, so the concept of "read" and
   * "write" is reversed */
  if (_IOC_DIR(cmd) & _IOC_READ)
    rc = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
  else
  if (_IOC_DIR(cmd) & _IOC_WRITE)
    rc = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
    
  if (rc)
    return -EFAULT;

  switch (cmd)
   {
    case AVT_SET_PHASE:
      punit->Phase = (arg & 7) << 5;
      punit->ValueRK = (punit->ValueRK & ~PHASE_MASK) | punit->Phase;
      break;
    case AVT_SET_SUBPHASE:
      if (punit->Arv105X)
       {
        punit->SubPhase = (arg & 3) << 11;
        punit->ValueRK = (punit->ValueRK & ~SUBPHASE_MASK) | punit->SubPhase;
       }
      break;
    case AVT_SET_AUTOPHASE:
      punit->AutoPhase = SET_AUTO_PHASE;
      punit->ValueRK |= punit->AutoPhase;
      break;
    case AVT_RESET_AUTOPHASE:
      punit->AutoPhase = 0;
      punit->ValueRK &= ~SET_AUTO_PHASE;
      break;
    case AVT_GET_PARAM:
     {
      avd_param avdp;     
      avd_get_param (&avdp, punit);
/*      rc = __PUT_USER (avdp, (avd_param *)arg); */ /* compiler 3.0.2 crash here */
      rc = copy_to_user ((avd_param *)arg, &avdp, sizeof (avd_param));
      break;
     } 
    default:  /* redundant, as cmd was checked against MAXNR */
      return -ENOTTY;
   }

  return rc;
}

/* unsigned long copy_to_user (void *to, const void *from, unsigned long count); */
/* unsigned long copy_from_user (void *to, const void *from, unsigned long count); */
static ssize_t avd_read (struct file *file, char *buffer, size_t count, loff_t *ppos)
{
  avd_unit *punit = file->private_data;

  if (!file->private_data
   || !(punit->ValueRK & SET_RECIVE) )
    return -EFAULT;

  if (down_interruptible (&punit->FreeForApp))
    return -ERESTARTSYS;

  int eop = 0;
  int rd_count = 0;
  
  while (!eop)
   {
    if (!GetArvidFrameCount (punit))
     {
      if (file->f_flags & O_NONBLOCK)
       {
        up (&punit->FreeForApp);
        return -EAGAIN;
       }

/*      printk (KERN_CRIT "avd:reading %d bytes\n", rd_count); */
/*      interruptible_sleep_on_timeout (&punit->wait, HZ/50); */
/*      printk (KERN_CRIT "punit->ValueRK %04Xh\n", punit->ValueRK); */
      interruptible_sleep_on (&punit->wait);

      if (signal_pending (current))  /* a signal arrived */
       {
        up (&punit->FreeForApp);
        return -ERESTARTSYS; /* tell the fs layer to handle it */
       }
     }
    else /* read frame */
     {
      if ((punit->SizeArvFrame << 1) <= count)
       {
        rd_count += punit->SizeArvFrame << 1;
	
	if (rd_count <= count)
	 {
/*	  printk (KERN_CRIT "avd:receive frame, set SubFrame %Xh\n", punit->SubFrame); */
/*	  printk (KERN_CRIT "punit->ValueRK %04Xh\n", punit->ValueRK); */
          
          /* if (punit->ValueRK & SET_DATA) */
	  ReceiveFrame (punit, punit->cbuf);
	  /* dump ((byte*)punit->cbuf, 0x100); */
	  
          /* Set on limit of frame */
          punit->SubFrame += (4096 >> 8) - ((punit->SizeArvFrame + 0xFF) >> 8);

          if (punit->SubFrame >= (4096 >> 8) * punit->NumArvFrame)
            punit->SubFrame = 0;
	  
          if (copy_to_user (buffer, punit->cbuf, punit->SizeArvFrame << 1))
	   {
	    up (&punit->FreeForApp);
            return -EFAULT;
	   }
	    
	  buffer += punit->SizeArvFrame << 1;
	 }
	else
	 {
          rd_count -= punit->SizeArvFrame << 1;
          eop = 1;
	 } 
       }
      else
        eop = 1;
     } /* end else */
   }

  *ppos += rd_count;
  up (&punit->FreeForApp);
  return rd_count;
}

static ssize_t avd_write (struct file *file, const char *buffer, size_t count, loff_t *ppos)
{
  avd_unit *punit = file->private_data;

  if (!file->private_data
   || punit->ValueRK & SET_RECIVE)
    return -EFAULT;

  if (down_interruptible (&punit->FreeForApp))
    return -ERESTARTSYS;

  int eop = 0;
  int wr_count = 0;
      
  while (!eop)
   {
    if (GetArvidFrameCount (punit) >= punit->NumArvFrame - 1)
     {
      if (file->f_flags & O_NONBLOCK)
       {
        up (&punit->FreeForApp);
        return -EAGAIN;
       }

/*      printk (KERN_CRIT "avd:writing %d bytes\n", wr_count); */
/*      interruptible_sleep_on_timeout (&punit->wait, HZ/50); */
      interruptible_sleep_on (&punit->wait);

      if (signal_pending (current))  /* a signal arrived */
       {
        up (&punit->FreeForApp);
        return -ERESTARTSYS; /* tell the fs layer to handle it */
       }
     }
    else /* write frame */
     {
      if (count)
       {
        wr_count += punit->SizeArvFrame << 1;
	
	if (wr_count <= count)
	 {
          if (copy_from_user (punit->cbuf, buffer, punit->SizeArvFrame << 1))
	   {
	    up (&punit->FreeForApp);
            return -EFAULT;
	   }
	    
	  buffer += punit->SizeArvFrame << 1;
/*	  printk (KERN_CRIT "avd:send frame, set SubFrame %Xh\n", punit->SubFrame); */
/*        printk (KERN_CRIT "punit->ValueRK & SET_DATA %Xh\n", punit->ValueRK & SET_DATA); */
	  SendFrame (punit, punit->cbuf);
	  
          /* Set on limit of frame */
          punit->SubFrame += (4096 >> 8) - ((punit->SizeArvFrame + 0xFF) >> 8);

          if (punit->SubFrame >= (4096 >> 8) * punit->NumArvFrame)
            punit->SubFrame = 0;
	  
	  /* if (punit->ValueRK & SET_DATA)
	    printk (KERN_CRIT "punit->SubFrame %Xh\n", punit->SubFrame); */
	 }
	else
	 {
          wr_count -= (punit->SizeArvFrame << 1);
          eop = 1;
	 } 
       }
      else
        eop = 1;
     } 
   }

  *ppos += wr_count;
  up (&punit->FreeForApp);
  return wr_count;
}

static struct file_operations avd_fops = {
  read:    avd_read,
  write:   avd_write,
  open:    avd_open,
  release: avd_release,
  ioctl:   avd_ioctl
};

static int major = 0;
static int __init InitModule (void)
{
  major = register_chrdev (AVT_MAJOR, MOD_NAME, &avd_fops);
  
  if (major < 0)
   {
    printk (KERN_WARNING "avd:can't get major %d\n", major);
    return major;
   }

  return 0;
}

static void __exit FreeModule (void)
{
  if (major >= 0)
    unregister_chrdev (AVT_MAJOR, MOD_NAME);
}

module_init (InitModule);
module_exit (FreeModule);
