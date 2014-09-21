#include "compiler.h"
#include <_dpmi.h>

#include "mytypes.h"
#include "inline.h"
#include "scrctrl.h"
#include "render.h"

dword FlagFlipVMode, OffsetVPage, WidthPageFactor, XModeFlag, numXmode;
dword ClipMinX, ClipMinY, ClipMaxX, ClipMaxY, SizeScreenX, SizeScreenY;
bool OutToStdOut = true;
byte *WidthAdressTable [768];

void WaitVrt (dword loop)
{
  while (loop--)
   {
    //while((inp (INPUT_STATUS_1)&8)){}
    while (!(inp (INPUT_STATUS_1) & 8)) {}
   }
}

void RecalcWidthAdressTable (dword X, dword Y, byte *Memory)
{
  for (dword i = 0; i <= Y; i++)
    WidthAdressTable [i] = (byte*)((int)Memory + (i * X));
}

  dword SizeXTable[MaxXMode][2]={{256,200},{256,240},{320,200},{320,240},{320,400},
				 {320,480},{360,200},{360,240},{360,360},{360,400},
				 {360,480},{376,282},{376,308},{376,564},{360,350},
				 {320,350}};

  dword OffPageTable[MaxXMode] = {0x3200,0x3c00,0x3e80,0x4b00,0x7d00,0x0000,0x4650,
				  0x5460,0x7e90,0x0000,0x0000,0x678c,0x7118,0x0000,
				  0x7b0c,0x6d60};

void CopyShadowToScreen (byte *Memory)
{
  if (XModeFlag)
   {
    if (!OffPageTable [numXmode])
      CopyShadowHorRasterToCurVPage_NoFlip (Memory);
    else
     {
      CopyShadowHorRasterToCurVPage (Memory);
      SwitchVideoPage ();
     }

    return;
   }

//  if(VesaFlag) VESA_ScreenCopy(Memory);
}

void CopyShadowHorRasterToCurVPage (byte *Memory)
{
  dword CurOffPage, CurOffShadow;

  CurOffShadow = (int)Memory + (ClipMinY*SizeScreenX);
  CurOffPage = 0xa0000 + (ClipMinY*WidthPageFactor) + OffsetVPage;

  for (dword i = 0; i < 4; i++)
   {
    outpw (SC_INDEX, (1 << (i+8)) | CPWER);
    mover ((byte*)CurOffPage, (byte*)CurOffShadow, WidthPageFactor * (ClipMaxY - ClipMinY));
    CurOffShadow++;
   }
}

void CopyShadowHorRasterToCurVPage_NoFlip (byte *Memory)
{
  dword CurOffPage, CurOffShadow, count_plane;
  dword Y1, Y2 = SizeXTable[numXmode][1];

  CurOffPage = 0xa0000 + (ClipMinY*WidthPageFactor) + OffsetVPage;

  for (Y1 = 0; Y1 < Y2; Y1++)
   {
    CurOffShadow = (int)Memory+(Y1 * SizeXTable[numXmode][0]);

    for (count_plane = 0; count_plane < 4; count_plane++)
     {
      outpw (SC_INDEX, (1 << (count_plane + 8)) | CPWER);
      mover ((byte*)CurOffPage, (byte*)CurOffShadow, WidthPageFactor);
      CurOffShadow++;
     }
    CurOffPage += WidthPageFactor;
   }
}

void CopyXModeLine (dword X1, dword Y1, dword offset, void *src, dword ncopy)
{
  dword count_plane, plane = X1&3;

  offset = 0xA0000 + offset + OffsetVPage ^ OffPageTable [numXmode];

  for (count_plane = 0; count_plane < 4; count_plane++)
   {
    outpw (SC_INDEX, (1 << (plane + 8)) | CPWER);
    mover ((byte*)offset, (byte*)src, (ncopy - count_plane + 3) >> 2);
    src = (void*)((int)src + 1);
    plane = (plane + 1) & 3;
   }
}

void XMode_UpdateScreen (dword X1, dword Y1, dword X2, dword Y2, void *src)
/****************************************************************************
 * Function:	 XMode_UpdateScreen
 * Description:  Copies a block of memory to a location on the screen
 *		 in X-mode
 ****************************************************************************/
{
  dword ncopy = X2 - X1;
  dword offset;

  for (Y1 = Y1; Y1 <= Y2; Y1++)
   {
    offset =Y1 * WidthPageFactor + (X1>>2);
    CopyXModeLine (X1, Y1, offset, src, ncopy);
    src = (void*)((int)src + SizeXTable[numXmode][0]); //Set on next line
   }
}

void UpdateScreen (dword X1, dword Y1, dword X2, dword Y2, void *src)
{
 //if(VesaFlag) {VESA_UpdateScreen(X1, Y1, X2, Y2, src); return;}
 if(XModeFlag) {XMode_UpdateScreen(X1, Y1, X2, Y2, src); return;}
}

void SwitchVideoPage()
{
  if (numXmode < MaxXMode && OffPageTable [numXmode])
   {
    outpw (CRTC_INDEX, (OffsetVPage & 0xff00) | 0xc);
    outpw (CRTC_INDEX, (OffsetVPage << 8) | 0xd);
    OffsetVPage ^= OffPageTable [numXmode]; //XOR
   }
}

dword GetNearXMode (dword *X, dword *Y)
{
  dword i, BestX = (dword)(-1), BestY = (dword)(-1), BestDXY = (dword)(-1), BestNum = 1, CurDXY, RastrFactor;
  int CurFactor;

  if ((*X - *Y) > 0)
    RastrFactor = 0;
  else
    RastrFactor = 1;

  for (i = 0; i < MaxXMode; i++)
   {
    CurFactor = SizeXTable[i][0] - SizeXTable[i][1];
    CurDXY = abs (*X - SizeXTable[i][0]) + abs (*Y - SizeXTable[i][1]);

    if (CurDXY < BestDXY)
     {
      if (CurFactor > 0 && RastrFactor == 0)
       {
	BestDXY = CurDXY;
	BestX	= SizeXTable[i][0];
	BestY	= SizeXTable[i][1];
	BestNum = i;
       }
	else
	 if (CurFactor <= 0 && RastrFactor == 1)
	  {
	   BestDXY = CurDXY;
	   BestX   = SizeXTable[i][0];
	   BestY   = SizeXTable[i][1];
	   BestNum = i;
	  }
     }
   }

   *X = BestX;
   *Y = BestY;
   return BestNum;
}

  char Sequencer[5] = {0x03,0x23,0x0f,0x00,0x06}; //For all modes
  char Graphics[9]  = {0x00,0x00,0x00,0x00,0x00,0x40,0x05,0x0f,0xff}; //For all modes
  char Attrib[5]    = {0x41,0x00,0x0f,0x00,0x00}; //For all modes
			  // 0	  1    2    3	 4    5    6	7    8	  9    10   11	 12   13   14	15
  char MORValue[MaxXMode] = {0xe3,0xe3,0x63,0xe3,0x63,0xe3,0x67,0xe7,0xa7,0x67,0xe7,0xe7,0xe7,0xe7,0x67,0x67};
  char CRTCValue[MaxXMode][25] ={
			  {0x5f,0x3f,0x42,0x9f,0x4c,0x00,0xcc,0x1f,
			   0x00,0x41,0x1e,0x00,0x00,0x00,0x00,0x00,	 //MOR = 0xe3
			   0xa0,0x2c,0x8f,0x20,0x00,0x96,0xc6,0xe3,0xff},//256*200*256c
			  {0x5f,0x3f,0x42,0x9f,0x4c,0x00,0x0d,0x3e,
			   0x00,0x41,0x1e,0x00,0x00,0x00,0x00,0x00,	 //MOR = 0xe3
			   0xea,0x2c,0xdf,0x20,0x00,0xe7,0x06,0xe3,0xff},//256*240*256c
			  {0x5f,0x4f,0x50,0x82,0x55,0x80,0xcc,0x1f,
			   0x00,0x41,0x1e,0x00,0x00,0x00,0x00,0x00,	 //MOR = 0x63
			   0xa0,0x2c,0x8f,0x28,0x00,0x96,0xc6,0xe3,0xff},//320*200*256c
			  {0x5f,0x4f,0x50,0x82,0x54,0x80,0x0d,0x3e,
			   0x00,0x41,0x00,0x00,0x00,0x00,0x00,0x00,	 //MOR = 0xe3
			   0xea,0x2c,0xdf,0x28,0x00,0xe7,0x06,0xe3,0xff},//320*240*256c
			  {0x5f,0x4f,0x50,0x82,0x55,0x80,0xcc,0x1f,
			   0x00,0x40,0x1e,0x00,0x00,0x00,0x00,0x00,	 //MOR = 0x63
			   0xa0,0x2c,0x8f,0x28,0x00,0x96,0xc6,0xe3,0xff},//320*400*256c
			  {0x5f,0x4f,0x50,0x82,0x55,0x80,0x0d,0x3e,
			   0x00,0x40,0x1e,0x00,0x00,0x00,0x00,0x00,	 //MOR = 0xe3
			   0xea,0x2c,0xdf,0x28,0x00,0xe7,0x06,0xe3,0xff},//320*480*256c
			  {0x6b,0x59,0x5a,0x8e,0x5e,0x8a,0xcc,0x1f,
			   0x00,0x41,0x1e,0x00,0x00,0x00,0x00,0x00,	 //MOR = 0x67
			   0xa0,0x2c,0x8f,0x2d,0x00,0x96,0xc6,0xe3,0xff},//360*200*256c
			  {0x6b,0x59,0x5a,0x8e,0x5e,0x8a,0x0d,0x3e,
			   0x00,0x41,0x1e,0x00,0x00,0x00,0x00,0x00,	 //MOR = 0xe7
			   0xea,0x2c,0xdf,0x2d,0x00,0xe7,0x06,0xe3,0xff},//360*240*256c
			  {0x6b,0x59,0x5a,0x8e,0x5e,0x8a,0xcc,0x1f,
			   0x00,0x40,0x1e,0x00,0x00,0x00,0x00,0x00,	 //MOR = 0x67
			   0x88,0x05,0x67,0x2d,0x00,0x6d,0xba,0xe3,0xff},//360*360*256c
			  {0x6b,0x59,0x5a,0x8e,0x5e,0x8a,0xcc,0x1f,
			   0x00,0x40,0x1e,0x00,0x00,0x00,0x00,0x00,	 //MOR = 0x67
			   0xa0,0x2c,0x8f,0x2d,0x00,0x96,0xc6,0xe3,0xff},//360*400*256c
			  {0x6b,0x59,0x5a,0x8e,0x5e,0x8a,0x0d,0x3e,
			   0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,	 //MOR = 0xe7
			   0xea,0x2c,0xdf,0x2d,0x00,0xe7,0x06,0xe3,0xff},//360*480*256c
			  {0x6e,0x5d,0x5e,0x91,0x62,0x8f,0x62,0xf0,
			   0x00,0x61,0x1e,0x00,0x00,0x00,0x00,0x31,	 //MOR = 0xe7
			   0x37,0x09,0x33,0x2f,0x00,0x3c,0x5c,0xe3,0xff},//376*282*256c
			  {0x6e,0x5d,0x5e,0x91,0x62,0x8f,0x62,0x0f,
			   0x00,0x40,0x1e,0x00,0x00,0x00,0x00,0x31,	 //MOR = 0xe7
			   0x37,0x09,0x33,0x2f,0x00,0x3c,0x5c,0xe3,0xff},//376*308*256c
			  {0x6e,0x5d,0x5e,0x91,0x62,0x8f,0x62,0xf0,
			   0x00,0x60,0x1e,0x00,0x00,0x00,0x00,0x31,	  //MOR = 0xe7
			   0x37,0x09,0x33,0x2f,0x00,0x3c,0x5c,0xe3,0xff},//376*564*256c
			  {0x6b,0x59,0x5a,0x8e,0x5e,0x8a,0xbf,0x1f,
			   0x00,0x40,0x1e,0x00,0x00,0x00,0x00,0x00,	 //MOR = 0xA7
			   0x83,0x85,0x5d,0x2d,0x00,0x63,0xba,0xe3,0xff},//360*350*256c
			  {0x5f,0x4f,0x50,0x82,0x55,0x80,0xbf,0x1f,
			   0x00,0x41,0x1e,0x00,0x00,0x00,0x00,0x00,	 //MOR = 0xA3
			   0x83,0x85,0x5d,0x28,0x00,0x63,0xba,0xe3,0xff}};//320*350*256c

short SetGraphXMode (dword X, dword Y, byte *Memory)
{
  dword i, h;

  SaveTextScreen ();
  numXmode = GetNearXMode (&X, &Y);

  //Set global variable
  FlagFlipVMode = 1;
  XModeFlag	= 1;
  OffsetVPage	= OffPageTable [numXmode];

  WidthPageFactor = X >> 2;
  ClipMinX = ClipMinY = 0;
  ClipMaxX = SizeScreenX = X;
  ClipMaxY = SizeScreenY = Y;
  RecalcWidthAdressTable (X, Y, Memory);

  WaitVrt (1);
  outp (SC_INDEX, 1);
  outp (SC_DATA, (inp (SC_DATA) | 0x20));

  for (i = 5; i > 0; i--)
   {
    outp (ACW_FF, 0x2f + i);
    outp (ACW_FF, Attrib [i - 1]);
   }

  for (i = 0x10; i > 0; i--)
   {
    outp (ACW_FF, i - 1);
    outp (ACW_FF, i - 1);
   }

  outp	(ACW_FF, 0x30);
  outpw (SC_INDEX, 0x604);
  outpw (SC_INDEX, 0x100);
  outp	(MISC_OUTPUT, MORValue [numXmode]);

  for (i = 5; i > 0; i--)
   {
    h = Sequencer [i - 1] << 8;
    h |= (i - 1);
    outpw (SC_INDEX, h);
   }

  for (i = 0x19; i > 0; i--)
   {
    h = CRTCValue[numXmode][i-1] << 8;
    h |= (i - 1);
    outpw (CRTC_INDEX, h);
   }

  for (i = 9; i > 0; i--)
   {
    h = Graphics [i - 1] << 8;
    h |= (i - 1);
    outpw (GC_INDEX, h);
   }

  outpw (SC_INDEX, 0xf02);
  imemset ((byte*)0xA0000, 0, 0xFFFF);
  outpw (SC_INDEX, 0x101);

  return 3; //Xmode installed success
}

void SetPalette (dword i, dword r, dword g, dword b)
{
  //WaitVrt(1);
  outp (PEL_MASK, 0xff);
  outp (PEL_SET,  i);
  outp (PEL_DATA, r >> 2);
  outp (PEL_DATA, g >> 2);
  outp (PEL_DATA, b >> 2);
}

byte StringHorRaster [1024];
byte ColumnHorRaster [768];

//without clipping
void SetDotHorRaster (dword XPos, dword YPos, dword Color)
{
  //if((ClipMinX <XPos< ClipMaxX)&&(ClipMinY <YPos< ClipMaxY))
  *((byte*)(WidthAdressTable [YPos] + XPos)) = Color;
}

//with clipping
void MoveColumn2HorRaster (dword XBeg, dword YBeg, dword Hiest)
{
  if ((YBeg > ClipMaxY) || (XBeg > ClipMaxX))
    return;
  if (YBeg + Hiest > ClipMaxY)
    Hiest = ClipMaxY - YBeg;

  movecol (WidthAdressTable [YBeg] + XBeg, ColumnHorRaster, Hiest, SizeScreenX);
}

//with clipping
void MoveString2HorRaster (dword XBeg, dword YBeg, dword Lenght)
{
  if ((YBeg > ClipMaxY) || (XBeg > ClipMaxX))
    return;
  if (XBeg + Lenght > ClipMaxX)
    Lenght = ClipMaxX - XBeg;

  Lenght & 0xfffffffc ? imemcpy (WidthAdressTable [YBeg] + XBeg, StringHorRaster, Lenght) : imemcpyb (WidthAdressTable[YBeg] + XBeg, StringHorRaster, Lenght);
}

short TestVMode (dword X, dword Y, byte *Memory)
{
  dword XPos = 0, YPos = 0, Color = 0;

  //if(i =SetGraphMode(X,Y,8,Memory)) return i;

  for (XPos = ClipMinX; XPos < ClipMaxX; XPos++)
   {
    for (YPos = ClipMinY; YPos < ClipMaxY; YPos++)
      SetDotHorRaster (XPos, YPos, Color);
    Color++;
   }

  CopyShadowToScreen (Memory);

  for (dword i = 0; i < 0xffffff; i++)

  RestoreVMode();
  return 0;
}

void RestoreVMode(void)
{
  if (XModeFlag)
   {
    outp (SC_INDEX, 1);
    outp (SC_DATA, (inp (SC_DATA) | 0x20));
    WaitVrt (1);
    RestoreScreen ();
   }

//   if(VesaFlag) VESA_EndGraphics();
  RestoreTextScreen ();
}

void BeginSCRCTRLLockedCode () {}
  scr_par spar;

void GetScreenParametrs (scr_par &spar)
{
#ifdef __GNUC__
  spar.mode = ScreenMode ();
  spar.rows = ScreenRows ();
  spar.cols = ScreenCols ();
#else
  spar.mode = *((byte*)0x449);
  spar.rows = *((byte*)0x484) + 1;
  spar.cols = *((word*)0x44A);
#endif
}

void PutChar (dword _ch, dword _attr, dword _x, dword _y)
{
#ifdef __GNUC__
  _farpokew (_dos_ds, ScreenPrimary + (_x << 1) + (spar.cols << 1) * _y, (_attr << 8) | _ch);
#else
  *((word*)(ScreenPrimary + (_x << 1) + (spar.cols << 1) * _y)) = (_attr << 8) | _ch;
#endif
}

void PutString (char *_ch, dword _attr, dword _x, dword _y)
{
  word *VideoMem = (word*)(ScreenPrimary + (_x << 1) + (spar.cols << 1) * _y);
  _attr <<= 8;

  while (*_ch)
   {
#ifdef __GNUC__
    _farpokew (_dos_ds, (dword)VideoMem++, _attr | *_ch++);
#else
    *VideoMem++ = _attr | *_ch++;
#endif
   }
}

void GetChar (dword *_ch, dword *_attr, dword _x, dword _y)
{
  word sym;
#ifdef __GNUC__
  sym = _farpeekw (_dos_ds, ScreenPrimary + (_x << 1) + (spar.cols << 1) * _y);
#else
  sym = *((word*)(ScreenPrimary + (_x << 1) + (spar.cols << 1) * _y));
#endif
  *_ch = sym & 0xff;
  *_attr = sym >> 8;
}

void EndSCRCTRLLockedCode () {}

void RestoreScreen (void)
{
  __dpmi_regs r;
  r.x.ax = spar.mode;
  __dpmi_int (0x10, &r);
}

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

#ifdef __GNUC__
  gettext (0, 0, spar.cols - 1, spar.rows - 1, TextScreen);
#else
  imemcpy (TextScreen, (void*)ScreenPrimary, TextScrSize);
#endif

  return TextScreen;
}

void RestoreTextScreen (void)
{
  if (!TextScreen || !OutToStdOut)
    return;

#ifdef __GNUC__
  puttext (0, 0, spar.cols - 1, spar.rows - 1, TextScreen);
#else
  imemcpy ((void*)ScreenPrimary, TextScreen, TextScrSize);
#endif

  delete TextScreen;
  TextScreen = NULL;
}

bool vgaDetect (void)
{
  __dpmi_regs r;
  r.x.ax = 0x1a00;
  __dpmi_int (0x10, &r);

  if ((r.h.al == 0x1a && r.h.bl == 7) || (r.h.al == 0x1a && r.h.bl == 8))
    return true;
  else
    return false;
}

