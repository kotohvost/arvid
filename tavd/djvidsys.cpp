/*
    DOS support for Crystal Space 3D library
    Copyright (C) 1998 by Jorrit Tyberghein
    Written by David N. Arnold <derek_arnold@fuse.net>
    Written by Andrew Zabolotny <bit@eltech.ru>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
 * Written by David N. Arnold. <derek_arnold@fuse.net>
 *   - Full featured VGA 320x200 support and various VESA
 *     resolutions at 256 colors.
 * 13-07-98:  Andrew Zabolotny <bit@eltech.ru>
 *   - Major rewrite
 *   - Added support for page-flipped VESA modes
 *   - Added hicolor support
 *   - Added support for 16 X-modes
 */

#include "compiler.h"
#include <_dpmi.h>

#include "mytypes.h"
#include "inline.h"
#include "render.h"
#include "dpmiserv.h"
#include "djvidsys.h"

bool OutToStdOut = true;

static struct
{
  int Mode, Width, Height, Depth;
} VesaModeDesc [] =
{
  { 0x100, 640, 400, 8 },
  { 0x101, 640, 480, 8 },
  { 0x102, 800, 600, 4 },
  { 0x103, 800, 600, 8 },
  { 0x104, 1024, 768, 4 },
  { 0x105, 1024, 768, 8 },
  { 0x106, 1280, 1024, 4 },
  { 0x107, 1280, 1024, 8 },
  { 0x10D, 320, 200, 15 },
  { 0x10E, 320, 200, 16 },
  { 0x10F, 320, 200, 24 },
  { 0x110, 640, 480, 15 },
  { 0x111, 640, 480, 16 },
  { 0x112, 640, 480, 24 },
  { 0x113, 800, 600, 15 },
  { 0x114, 800, 600, 16 },
  { 0x115, 800, 600, 24 },
  { 0x116, 1024, 768, 15 },
  { 0x117, 1024, 768, 16 },
  { 0x118, 1024, 768, 24 },
  { 0x119, 1280, 1024, 15 },
  { 0x11A, 1280, 1024, 16 },
  { 0x11B, 1280, 1024, 24 },
  { 0x120, 1600, 1200, 8 },
  { 0x121, 1600, 1200, 15 },
  { 0x122, 1600, 1200, 16 },
  { 0, 0, 0, 0 }
};

const int XModeCount = 16;
static struct
{
  int Width, Height;
  char MiscControlReg;
  char CRTCReg [0x19];
} XModeTable [XModeCount] =
{
  {256,200, 0xe3,
   {0x5f,0x3f,0x42,0x9f,0x4c,0x00,0xcc,0x1f,0x00,0x41,0x1e,0x00,
    0x00,0x00,0x00,0x00,0xa0,0x0c,0x8f,0x20,0x00,0x96,0xc6,0xe3,0xff}},
  {256,240, 0xe3,
   {0x5f,0x3f,0x42,0x9f,0x4c,0x00,0x0d,0x3e,0x00,0x41,0x1e,0x00,
    0x00,0x00,0x00,0x00,0xea,0x0c,0xdf,0x20,0x00,0xe7,0x06,0xe3,0xff}},
  {320,200, 0x63,
   {0x5f,0x4f,0x50,0x82,0x55,0x80,0xcc,0x1f,0x00,0x41,0x1e,0x00,
    0x00,0x00,0x00,0x00,0xa0,0x0c,0x8f,0x28,0x00,0x96,0xc6,0xe3,0xff}},
  {320,240, 0xe3,
   {0x5f,0x4f,0x50,0x82,0x54,0x80,0x0d,0x3e,0x00,0x41,0x00,0x00,
    0x00,0x00,0x00,0x00,0xea,0x0c,0xdf,0x28,0x00,0xe7,0x06,0xe3,0xff}},
  {320,400, 0x63,
   {0x5f,0x4f,0x50,0x82,0x55,0x80,0xcc,0x1f,0x00,0x40,0x1e,0x00,
    0x00,0x00,0x00,0x00,0xa0,0x0c,0x8f,0x28,0x00,0x96,0xc6,0xe3,0xff}},
  {320,480, 0xe3,
   {0x5f,0x4f,0x50,0x82,0x55,0x80,0x0d,0x3e,0x00,0x40,0x1e,0x00,
    0x00,0x00,0x00,0x00,0xea,0x0c,0xdf,0x28,0x00,0xe7,0x06,0xe3,0xff}},
  {360,200, 0x67,
   {0x6b,0x59,0x5a,0x8e,0x5e,0x8a,0xcc,0x1f,0x00,0x41,0x1e,0x00,
    0x00,0x00,0x00,0x00,0xa0,0x0c,0x8f,0x2d,0x00,0x96,0xc6,0xe3,0xff}},
  {360,240, 0xe7,
   {0x6b,0x59,0x5a,0x8e,0x5e,0x8a,0x0d,0x3e,0x00,0x41,0x1e,0x00,
    0x00,0x00,0x00,0x00,0xea,0x0c,0xdf,0x2d,0x00,0xe7,0x06,0xe3,0xff}},
  {360,360, 0xa7,
   {0x6b,0x59,0x5a,0x8e,0x5e,0x8a,0xcc,0x1f,0x00,0x40,0x1e,0x00,
    0x00,0x00,0x00,0x00,0x88,0x05,0x67,0x2d,0x00,0x6d,0xba,0xe3,0xff}},
  {360,400, 0x67,
   {0x6b,0x59,0x5a,0x8e,0x5e,0x8a,0xcc,0x1f,0x00,0x40,0x1e,0x00,
    0x00,0x00,0x00,0x00,0xa0,0x0c,0x8f,0x2d,0x00,0x96,0xc6,0xe3,0xff}},
  {360,480, 0xe7,
   {0x6b,0x59,0x5a,0x8e,0x5e,0x8a,0x0d,0x3e,0x00,0x40,0x00,0x00,
    0x00,0x00,0x00,0x00,0xea,0x0c,0xdf,0x2d,0x00,0xe7,0x06,0xe3,0xff}},
  {376,282, 0xe7,
   {0x6e,0x5d,0x5e,0x91,0x62,0x8f,0x62,0xf0,0x00,0x61,0x1e,0x00,
    0x00,0x00,0x00,0x31,0x37,0x09,0x33,0x2f,0x00,0x3c,0x5c,0xe3,0xff}},
  {376,308, 0xe7,
   {0x6e,0x5d,0x5e,0x91,0x62,0x8f,0x62,0x0f,0x00,0x40,0x1e,0x00,
    0x00,0x00,0x00,0x31,0x37,0x09,0x33,0x2f,0x00,0x3c,0x5c,0xe3,0xff}},
  {376,564, 0xe7,
   {0x6e,0x5d,0x5e,0x91,0x62,0x8f,0x62,0xf0,0x00,0x60,0x1e,0x00,
    0x00,0x00,0x00,0x31,0x37,0x09,0x33,0x2f,0x00,0x3c,0x5c,0xe3,0xff}},
  {360,350, 0x67,
   {0x6b,0x59,0x5a,0x8e,0x5e,0x8a,0xbf,0x1f,0x00,0x40,0x1e,0x00,
    0x00,0x00,0x00,0x00,0x83,0x05,0x5d,0x2d,0x00,0x63,0xba,0xe3,0xff}},
  {320,350, 0x67,
   {0x5f,0x4f,0x50,0x82,0x55,0x80,0xbf,0x1f,0x00,0x41,0x1e,0x00,
    0x00,0x00,0x00,0x00,0x83,0x05,0x5d,0x28,0x00,0x63,0xba,0xe3,0xff}}
};

static unsigned short *VESAModes;

static void SetXmode (int XMode)
{
  // Set up plain VGA 320x200x256 mode
  __dpmi_regs regs;
  regs.x.ax = 0x0013;
  __dpmi_int (0x10, &regs);

  // Reset attribute controller flip-flop
  inp (0x3DA);

  // Unlock CRT registers
  outpw (0x3D4, (int (XModeTable [XMode].CRTCReg [0x11]) << 8) | 0x0011);

  // Set up CRTC registers
  for (int r = 0; r < 0x19; r++)
    outpw (0x3D4, (int (XModeTable [XMode].CRTCReg [r]) << 8) | r);

  // Lock CRT registers
  outpw (0x3D4, (int (XModeTable [XMode].CRTCReg [0x11]) << 8) | 0x8011);

  // Initialize miscelaneous control register
  outpw (0x3C4, 0x0100);
  outp (0x3C2, XModeTable [XMode].MiscControlReg);
  outpw (0x3C4, 0x0300);

  // Initialize VRAM addressing mode
  outpw (0x3C4, 0x0604);
  // Initialize attribute controler
  outp (0x3C0, 0x30);
  outp (0x3C0, 0x61);
}

VideoSystem::VideoSystem ()
{
  DPMIInit (0x1000);
  VRAM = NULL;
  VRAMBuffer = NULL;
  VideoMode = 0;
  VideoPage = 0;
  WaitVR = true;
  UseDoubleBuffering = false;

  // Sanity check
  if ((sizeof (VESAInformation) != 512)
   || (sizeof (VESAModeInfoBlock) != 256))
  {
    printf ("Your compiler does not handle properly packed structures!");
    abort ();
  }

  // Query VESA info
  VESAInformation vi;
  __dpmi_regs regs;
  regs.x.ax = 0x4f00;
  regs.x.es = __tb >> 4;
  regs.x.di = 0;
  dosmemput ("VBE2", 4, __tb);
  __dpmi_int (0x10, &regs);

  if (regs.x.ax == 0x004f)
  {
    // Retrieve mode info structure from lower megabyte of dos memory.
    dosmemget (__tb, sizeof (VESAInformation), &vi);
    VESAversion = vi.Version;
    VideoRAM = vi.VideoRAM * 64;

    // now get the list of supported VESA modes
    if (vi.VideoModeList)
    {
      int vmcount = 0;
      unsigned short mode;
      unsigned long vmaddr = ((vi.VideoModeList & 0xffff0000) >> 12) +
			      (vi.VideoModeList & 0xffff); // Convert to linear address
      while (1)
      {
	dosmemget (vmaddr + vmcount * 2, sizeof (unsigned short), &mode);
	vmcount++;

	if (mode == 0xffff)
	  break;
      }
      VESAModes = new unsigned short [vmcount];
      dosmemget (vmaddr, vmcount * sizeof (unsigned short), VESAModes);
    }
    else
    {
      // build a dummy videomode list
      VESAModes = new unsigned short [0x40 + 1];

      for (int i = 0; i < 0x40; i++)
	VESAModes [i] = 0x100 + i;

      VESAModes [0x40] = 0xffff;
    }
  }
  else
  {
    VESAversion = 0;
    VideoRAM = 0;
  } /* endif */

#ifdef DEBUG
  char __oem [200];
  __oem [sizeof (__oem)] = 0;
  dosmemget ((size_t (vi.OEM) >> 16) * 16 + (size_t (vi.OEM) & 0xffff),
    sizeof (__oem), &__oem);
  printf ("Videocard OEM: %s\n", __oem);
  printf ("Memory size: %dK\n", VideoRAM);
#endif
}

VideoSystem::~VideoSystem (void)
{ Close (); }

dword GetNearXMode (int &X, int &Y)
{
  dword BestX = (dword)(-1), BestY = (dword)(-1), BestDXY = (dword)(-1);
  dword BestNum = 1, CurDXY, RastrFactor;
  int CurFactor;

  if ((X - Y) > 0)
    RastrFactor = 0;
  else
    RastrFactor = 1;

  for (int i = 0; i < XModeCount; i++)
   {
    CurFactor = XModeTable [i].Width - XModeTable [i].Height;
    CurDXY = abs (X - XModeTable [i].Width) + abs (Y - XModeTable [i].Height);

    if (CurDXY < BestDXY
     && (CurFactor > 0 && !RastrFactor
      || CurFactor <= 0 && RastrFactor))
     {
      BestDXY = CurDXY;
      BestX   = XModeTable [i].Width;
      BestY   = XModeTable [i].Height;
      BestNum = i;
     }
   }

  X = BestX;
  Y = BestY;
  return BestNum;
}

bool VideoSystem::FindMode (int &Width, int &Height, int Depth, int &PaletteSize,
  long &RedMask, long &GreenMask, long &BlueMask)
{
  __dpmi_regs regs;
  VESAModeInfoBlock mb;
  int i, bestmode = 0;
  BytesPerPixel = (Depth + 7) >> 3;

  // Default values
  PaletteSize = 256;
  RedMask = GreenMask = BlueMask = 0;
  static int maskbits [9] = { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };

  // Check if plain 320x200x256 is good enough
  if ((Width == 320)
   && (Height == 200)
   && (Depth == 8))
    bestmode = 0x13;

  // Check VESA mode list
  if ((!bestmode)
   && (VESAversion >= 0x0100)
   && VESAModes)
    for (i = 0; VESAModes [i] != 0xffff; i++)
    {
      regs.x.ax = 0x4f01;
      regs.x.cx = VESAModes [i];
      regs.x.es = __tb >> 4;
      regs.x.di = 0;

      // Get mode information.
      __dpmi_int (0x10, &regs);
      if (regs.x.ax != 0x004f)
	continue;

      // Retrieve mode info structure from lower megabyte of dos memory.
      dosmemget (__tb, sizeof (VESAModeInfoBlock), &mb);
      if (((mb.ModeAttributes & 0x0001) == 0)
       && ((mb.ModeAttributes & 0x0010) != 0))
	continue;
      VESAFillModeBlock (mb);

      if ((mb.BitsPerPixel == Depth)
       && (mb.XResolution == Width)
       && (mb.YResolution == Height)
       && ((mb.MemoryModel == 4)
	|| (mb.MemoryModel == 5)
	|| (mb.MemoryModel == 6)))
      {
	PaletteSize = (Depth == 8) ? 256 : 0;
	RedMask = maskbits [mb.RedMaskBits] << mb.RedMaskShift;
	GreenMask = maskbits [mb.GreenMaskBits] << mb.GreenMaskShift;
	BlueMask = maskbits [mb.BlueMaskBits] << mb.BlueMaskShift;
	bestmode = VESAModes [i];
	break;
      } /* endif */
    } /* endfor */

  // If still no suitable mode found, check X-modes
  if ((!bestmode)
   && (Depth == 8))
    bestmode = GetNearXMode (Width, Height) | 0x1000;

  // If we did not found a suitable mode, return failure
  if (!bestmode)
    return false;

  VideoMode = bestmode;
  ScanLinesPerPage = Height;
  return true;
}

int VideoSystem::Open ()
{
  SaveTextScreen ();
  unsigned long AppertureAddress;
  unsigned long AppertureSize;
  __dpmi_regs regs;

  if (VideoMode == 0x13)
  {
    // plain VGA mode
    IsBanked = false;
    IsXmode = false;
    AppertureAddress = 0xa0000;
    AppertureSize = 0x10000;
    ScreenSize = PageSize = 320 * 200 * 1;
    BytesPerScanLine = 320;
    VideoPages = 1;
    regs.x.ax = VideoMode;
    __dpmi_int (0x10, &regs);
  }
  else if (VideoMode & 0x1000)
  {
    // X-mode
    IsBanked = false;
    IsXmode = true;
    AppertureAddress = 0xa0000;
    AppertureSize = 0x10000;
    int xmode = VideoMode & 0xfff;
    ScreenSize = XModeTable [xmode].Width * XModeTable [xmode].Height * 1;

    if (ScreenSize / 4 <= 0x8000)
    {
      PageSize = 0x8000;
      VideoPages = 2;
    }
    else
    {
      PageSize = 0xffff;
      VideoPages = 1;
    }

    BytesPerScanLine = XModeTable [xmode].Width / 4;
    SetXmode (xmode);
  }
  else
  {
    // SVGA mode
    VESAModeInfoBlock mb;
    regs.x.ax = 0x4f01;
    regs.x.cx = VideoMode;
    regs.x.es = __tb >> 4;
    regs.x.di = 0;

    // clear the mode info buffer
    memset (&mb, 0, sizeof (mb));
    dosmemput (&mb, sizeof (VESAModeInfoBlock), __tb);

    // Get mode information.
    __dpmi_int (0x10, &regs);
    if (regs.x.ax != 0x004f)
      return -1;

    // Retrieve mode info structure from lower megabyte of dos memory.
    dosmemget (__tb, sizeof (VESAModeInfoBlock), &mb);

    if ((mb.ModeAttributes & 0x0001) == 0)
      return -1;

    VESAFillModeBlock (mb);

    // Compute size of one video page
    ScreenSize = (mb.XResolution * mb.YResolution * BytesPerPixel);
    IsBanked = !(mb.ModeAttributes & 0x80);
    IsXmode = (mb.MemoryModel == 5);
    BytesPerScanLine = mb.BytesPerScanLine;
    VideoPages = mb.VideoPages;
    WindowGranularity = mb.WindowGranularity;
    WindowSize = mb.WindowSize;
    PageSize = BytesPerScanLine * mb.YResolution;

    if ((VESAversion < 0x0101)
     || (VideoPages < 1)
     || IsXmode
     || IsBanked)
      VideoPages = 1;

    // Sanity check: don't allow less videoram than is needed for VideoPages
    // pages. Matrox OS/2 driver has a bug in VDD that makes VESA bios report
    // 2M VRAM instead of 4M: the following overcomes it.
    size_t rqVRAM = (VideoPages * PageSize + 1023) / 1024;

    if (VideoRAM < rqVRAM)
      VideoRAM = ((rqVRAM - 1) | 0x3ff) + 1;

    if (IsBanked)
    {
      AppertureAddress = ((int)mb.WindowA_Seg) << 4;
      AppertureSize = ((int)mb.WindowSize) * 1024;
    }
    else
    {
      AppertureAddress = mb.PhysicalAddress;
      AppertureSize = VideoRAM * 1024;
    } /* endif */

    // Set mode.
    regs.x.ax = 0x4f02;
    regs.x.bx = VideoMode | (IsBanked ? 0 : 0x4000);
    __dpmi_int (0x10, &regs);

    if (regs.x.ax != 0x004f)
      return -1;
  } /* endif */

  __dpmi_meminfo info;
  info.address = AppertureAddress;
  info.size = AppertureSize;

  // Map frame buffer into our linear memory space
  if (__dpmi_physical_address_mapping (&info))
  {
    if (AppertureAddress + AppertureSize <= 0x100000)
    {
      VRAMSelector = _dos_ds;
      VRAM = (unsigned char *) AppertureAddress;
      goto allocate;
    }
    Close ();
    return -2;
  }

  VRAMSelector = 0;
  VRAM = NULL;

  // Trick: since CS,DS and ES also points into our linear address space,
  // we can compute the address of LFB as LFBaddress-base(DS); a problem
  // that can arise is that limit(DS) can be less than resulting value;
  // this is resolved by trying to rise the limit of DS
  unsigned long flatsel, flatbase;
  flatsel = _my_ds ();

  if (__dpmi_get_segment_base_address (flatsel, &flatbase) == 0)
  {
    VRAM = (unsigned char *)(info.address - flatbase);
    unsigned long minlimit = ((unsigned long)VRAM) + AppertureSize - 1;

    if (__dpmi_get_segment_limit (flatsel) < minlimit)
      if (__dpmi_set_segment_limit (flatsel, minlimit | 0xfff))
	VRAM = NULL;
      else
      {
	extern int __djgpp_selector_limit;
	__djgpp_selector_limit = minlimit;
      }
  } /* endif */

  if (!VRAM)
  {
    // Our trick failed so we should process as usual by allocating a selector
    // Create our selector.
    VRAMSelector = __dpmi_allocate_ldt_descriptors (1);

    if (!VRAMSelector)
    {
      Close ();
      return -3;
    }

    if (__dpmi_set_segment_base_address (VRAMSelector, info.address)
     || __dpmi_set_segment_limit (VRAMSelector, AppertureSize | 0xfff))
    {
      Close ();
      return -3;
    }
  } /* endif */

allocate:

  if (!AllocateBackBuffer ())
  {
    Close ();
    return -4;
  }

  DoubleBuffer (true);
#ifdef DEBUG
  printf("IsBanked: %d\n" "IsXmode: %d\n" "VideoPages: %d\n"
	 "VRAMSelector: %08lX\n" "VRAMBuffer: %08lX\n" "VRAM: %08lX\n",
	 IsBanked, IsXmode, VideoPages, (unsigned long)VRAMSelector,
	 (unsigned long)VRAMBuffer, (unsigned long)VRAM);
  fflush (stdout);
#endif
  return 0;
}

bool VideoSystem::AllocateBackBuffer ()
{
  FreeBackBuffer ();

  if (! (VRAMBuffer = new unsigned char [ScreenSize]))
    return false;
  else
    return true;
}

void VideoSystem::FreeBackBuffer ()
{
  if (VRAMBuffer)
  {
    delete [] VRAMBuffer;
    VRAMBuffer = NULL;
  }
}

void VideoSystem::Close ()
{
  FreeBackBuffer ();

  __dpmi_regs regs;
  regs.x.ax = 0x0f00;
  __dpmi_int (0x10, &regs);

  if (regs.h.al != 3)
  {
    regs.x.ax = 0x03;
    __dpmi_int (0x10, &regs);
  }

  RestoreTextScreen ();
  DPMIFree ();
}

void VideoSystem::Flush (int x, int y, int w, int h)
{
  // Copy image from backbuffer to visible page
  if (IsXmode)
  {
    // Image Refresh for X-modes
    bool linetransfer = ((w * BytesPerPixel) >> 2) == BytesPerScanLine;
    w = (w + (x & 3) + 3) & ~3;
    x &= ~3;

    if (VRAMSelector)
      _farsetsel (VRAMSelector);

    // Copy backbuffer in X-mode
    if ((VideoPages > 1) && UseDoubleBuffering)
    {
      // We have enough space for two videopages
      // In this case we would want to fast blit memory to invisible page
      // then to flip video pages rather than slowly copying memory to VRAM
      dword vpoffset = VideoPage * PageSize;
      byte *src = VRAMBuffer + x + y * BytesPerScanLine * 4;
      byte *dest = VRAM + (x >> 2) + y * BytesPerScanLine + vpoffset;
      dword length = linetransfer ? h * BytesPerScanLine : w / 4;

      for (int page = 0; page < 4; page++)
      {
	outpw (0x3C4, (0x0100 << page) | 0x0002);

	if (linetransfer)
	  if (VRAMSelector)
	    XFmove (src + page, dest, length);
	  else
	    Xmove (src + page, dest, length);
	else
	{
	  byte *_src = src + page;
	  byte *_dest = dest;

	  if (VRAMSelector)
	    for (int _h = h; _h; _h--)
	    {
	      XFmove (_src, _dest, length);
	      _src += BytesPerScanLine * 4;
	      _dest += BytesPerScanLine;
	    } /* endfor */
	  else
	    for (int _h = h; _h; _h--)
	    {
	      Xmove (_src, _dest, length);
	      _src += BytesPerScanLine * 4;
	      _dest += BytesPerScanLine;
	    } /* endfor */
	} /* endif */
      } /* endfor */

      // Show video page
      XSetPage (VideoPage);
      VideoPage ^= 1;
    }
    else
    {
      byte  *src = VRAMBuffer + x + y * BytesPerScanLine * 4;
      byte  *dest = VRAM + (x >> 2) + y * BytesPerScanLine;
      dword length = w / 4;

      // We have only one page; refresh visible page from VRAMBuffer
      for (int _h = h; _h; _h--)
      {
	for (int page = 0; page < 4; page++)
	{
	  outpw (0x3C4, (0x0100 << page) | 0x0002);

	  if (VRAMSelector)
	    XFmove (src + page, dest, length);
	  else
	    Xmove (src + page, dest, length);
	} /* endfor */
	src += BytesPerScanLine * 4;
	dest += BytesPerScanLine;
      } /* endfor */
    }
  }
  else
  {
    // Transform width into bytes
    w *= BytesPerPixel;

    // Image Refresh for VESA modes
    bool linetransfer = ((unsigned)w == BytesPerScanLine);
    unsigned char *src, *dest, *last;
    unsigned int ws = WindowSize * 1024;
    unsigned int wg = WindowGranularity * 1024;
    int curx = 0, Bank = 0;

    src = VRAMBuffer + x * BytesPerPixel + y * BytesPerScanLine;
    last = src + w + h * BytesPerScanLine;
    dest = VRAM + VideoPage * PageSize + x * BytesPerPixel + y * BytesPerScanLine;

    if (IsBanked)
      VESASetBank (Bank);

    if (VRAMSelector)
      _farsetsel (VRAMSelector);

    while (src < last)
    {
      byte *bankend;
      if (IsBanked)
      {
	int bank = (src - VRAMBuffer) / wg;

	while (Bank != bank)
	{
	  Bank++;
	  dest -= wg;

	  if (Bank == bank)
	    VESASetBank (Bank);
	} /* endif */
	bankend = VRAMBuffer + bank * wg + ws;
	if (!linetransfer)
	  if (bankend - src > w)
	    bankend = src + w;
      }
      else
	bankend = linetransfer ? last : src + w;

      if (VRAMSelector)
	VESAFmove (src, dest, bankend);
      else
	VESAmove (src, dest, bankend);

      curx += bankend - src;
      dest += bankend - src;
      src = bankend;

      if (!linetransfer)
	if (curx >= w)
	{
	  int delta = BytesPerScanLine - curx;
	  src += delta; dest += delta; curx = 0;
	} /* endif */
    } /* endwhile */

    // Flip pages
    if ((VideoPages > 1) && UseDoubleBuffering)
    {
      VESASetPage (VideoPage);
      VideoPage ^= 1;
    } /* endif */
  } /* endif */
}

void VideoSystem::GetPalette (RGBpaletteEntry *Palette, int Colors)
{
  outp (0x3c7, 0);

  for (int i = 0; i < Colors; i++)
  {
    Palette [i].red   = inp (0x3c9);
    Palette [i].green = inp (0x3c9);
    Palette [i].blue  = inp (0x3c9);
  }
}

void VideoSystem::SetPalette (RGBpaletteEntry *Palette, int Colors)
{
  outp (0x3c8, 0);

  for (int i = 0; i < Colors; i++)
  {
    outp (0x3c9, Palette [i].red >> 2);
    outp (0x3c9, Palette [i].green >> 2);
    outp (0x3c9, Palette [i].blue >> 2);
  }
}

unsigned char *VideoSystem::BackBuffer ()
{ return VRAMBuffer; }

void VideoSystem::Clear (int Color)
{
  register unsigned *ptr, *last, color, s = 1;

  if (BytesPerPixel == 1)
  {
    Color &= 0xff;
    color = Color | Color << 8 | Color << 16 | Color << 24;
  }
  else if (BytesPerPixel == 2)
  {
    Color &= 0xffff;
    color = Color | Color << 16;
  }
  else
  {
    Color &= 0xffffff;
    color = Color;
  }

  ptr = (unsigned *)BackBuffer ();
  last = (unsigned *) ((unsigned char *)ptr + ScreenSize);

  while (ptr < last)
  {
    // Uses s instead of incrementing because
    // registers are faster than immediate
    // values on pentium.
    *ptr = color;
    ptr += s;
    *ptr = color;
    ptr += s;
    *ptr = color;
    ptr += s;
    *ptr = color;
    ptr += s;
    *ptr = color;
    ptr += s;
    *ptr = color;
    ptr += s;
    *ptr = color;
    ptr += s;
    *ptr = color;
    ptr += s;
  } /* endwhile */
}

bool VideoSystem::DoubleBuffer (bool Enable)
{
  if (Enable)
  {
    if (VideoPages < 2)
      return false;
    UseDoubleBuffering = true;
  }
  else
  {
    if (IsXmode)
      XSetPage (VideoPage = 0);
    else
      VESASetPage (VideoPage = 0);
    UseDoubleBuffering = false;
  } /* endif */
  return true;
}

bool VideoSystem::DoubleBuffer ()
{ return (UseDoubleBuffering); }

//-------------------------------------------------// Private functions //----//

void VideoSystem::VESAFillModeBlock (VESAModeInfoBlock &mb)
{
  // If mode has no full description, try to fill the gaps
  if ((mb.ModeAttributes & 0x0002) == 0)
  {
    mb.XResolution = mb.YResolution = mb.BitsPerPixel = mb.MemoryModel = 0;
    mb.VideoPages = 1;

    for (int mode = 0; VesaModeDesc [mode].Mode; mode++)
      if (mode == VesaModeDesc [mode].Mode)
      {
	mb.XResolution = VesaModeDesc [mode].Width;
	mb.YResolution = VesaModeDesc [mode].Height;
	mb.BitsPerPixel = VesaModeDesc [mode].Depth;
	mb.MemoryModel = 4;
	break;
      } /* endif */
  } /* endif */

  if (mb.BitsPerPixel == 16)
  {
    // VESA BIOS (at least on my Matrox Mystique) claims 16 bpp
    // even for 15bpp modes
    if ((mb.RedMaskBits + mb.GreenMaskBits + mb.BlueMaskBits) == 15)
      mb.BitsPerPixel = 15;
  } /* endif */
}

void VideoSystem::VESASetBank (int Bank)
{
  __dpmi_regs regs;
  regs.x.ax = 0x4f05;
  regs.x.bx = 0;
  regs.x.dx = Bank;
  __dpmi_int (0x10, &regs);
}

void VideoSystem::VESASetPage (int Page)
{
  __dpmi_regs regs;
  if ((VESAversion < 0x0200) || !WaitVR)
  {
    // Wait for VSync.
    if (WaitVR)
    {
      while (inp (0x3DA) & 0x08);
      while (!inp (0x3DA) & 0x08);
    } /* endif */
    regs.x.bx = 0;
  }
  else
    regs.x.bx = 0x0080;

  regs.x.ax = 0x4f07;
  regs.x.cx = 0;
  regs.x.dx = Page * ScanLinesPerPage;
  __dpmi_int (0x10, &regs);
}

void VideoSystem::XSetPage (int Page)
{
  int vpoffset = Page * 0x8000;
  outpw (0x3D4, (vpoffset & 0xff00) | 0x000c);
  outpw (0x3D4, (vpoffset << 8) | 0x000d);

  // Wait for VSync.
  if (WaitVR)
    while (!(inp (0x3DA) & 0x08));
}

void BeginSCRCTRLLockedCode () {}
  scr_par spar;

void GetScreenParametrs (scr_par &spar)
{
  spar.mode = ScreenMode ();
  spar.rows = ScreenRows ();
  spar.cols = ScreenCols ();
  ScreenGetCursor (&spar.row_cursor, &spar.col_cursor);
}

void PutChar (dword _ch, dword _attr, dword _x, dword _y)
{
  _farpokew (_dos_ds, ScreenPrimary + (_x << 1) + (spar.cols << 1) * _y, (_attr << 8) | _ch);
}

void PutString (char *_ch, dword _attr, dword _x, dword _y)
{
  word *VideoMem = (word*)(ScreenPrimary + (_x << 1) + (spar.cols << 1) * _y);
  _attr <<= 8;

  while (*_ch)
    _farpokew (_dos_ds, (dword)VideoMem++, _attr | *_ch++);
}

void GetChar (dword *_ch, dword *_attr, dword _x, dword _y)
{
  word sym = _farpeekw (_dos_ds, ScreenPrimary + (_x << 1) + (spar.cols << 1) * _y);
  *_ch = sym & 0xff;
  *_attr = sym >> 8;
}

void EndSCRCTRLLockedCode () {}

static byte *TextScreen = NULL;
static dword TextScrSize = 0;

byte *SaveTextScreen (void)
{
  fflush (stdout);
  GetScreenParametrs (spar);
  TextScrSize = spar.rows * spar.cols * 2;
  //printf ("TextScrSize = %d\n", TextScrSize);
  if (!OutToStdOut)
    return NULL;

  TextScreen = new byte [TextScrSize];

  if (!TextScreen)
    return NULL;

  gettext (0, 0, spar.cols - 1, spar.rows - 1, TextScreen);
  return TextScreen;
}

void RestoreTextScreen (void)
{
  if (!TextScreen || !OutToStdOut)
    return;

  puttext (0, 0, spar.cols - 1, spar.rows - 1, TextScreen);
  delete TextScreen;
  TextScreen = NULL;
  ScreenSetCursor (spar.row_cursor, spar.col_cursor);
}


