#include "compiler.h"
#include <bios.h>

#include "mytypes.h"
#include "console.h"
#include "adriver.h"
#include "inifile.h"
#include "inline.h"
#include "mtime.h"
#include "galey.h"
#include "djvidsys.h"
#include "tavd.h"

#define SizeFont (16 * 256)

  int X = WIDT_SCREEN, Y = HIEST_SCREEN, Yoffset [HIEST_SCREEN];
  dword Square, Square9, Delta;

  dword WidthTable [300], FrameTable [BUFF_FRAMES];
  byte *Font, *Memory, DimWord [16];
  bool TVSetFlag = false, ShowByTime = false, ViewBufferFlag = false;

  int PalEntries;

void PrintWorkStatusBegin (char *String)
{
  dword Len = strlen (String);

  if (Len >= STR_LIMIT)
   {
    byte EndChar = String [STR_LIMIT - 1];
    String [STR_LIMIT - 1] = 0;
    printf ("-%s.", String);
    String [STR_LIMIT - 1] = EndChar;
   }
  else
   {
    printf ("-%s", String);
    Len = STR_LIMIT - Len;

    for (dword i = 0; i < Len; i++)
      printf ("%c", '.');
   }

  fflush (stdout);
}

#include <stdarg.h>

void PrintWorkStatusEnd (const char *str, ...)
{
  char buf [200];
  va_list arg;

  va_start (arg, str);
  vsprintf (buf, str, arg);
  va_end (arg);

  printf ("[%s]\n", buf);
}

byte *InitFont (char *NameFont)
{
  int hFont = -1;
  byte BitString, LowBit, HighBit, LowMask, HighMask;

  Font = new byte [SizeFont];

  if ((hFont = open (NameFont, O_BINARY | O_RDWR, FILE_ACCESS)) == -1)
    ErrorExit (3);
  else
    read (hFont, Font, SizeFont);
  close (hFont);

  dword i;
  for (i = 0; i < SizeFont; i++)
   {
    BitString = Font[i];
    LowMask   = 1;
    HighMask  = 0x80;

    for (dword j = 0; j < 4; j++)
     {
      LowBit  = BitString & LowMask;
      HighBit = BitString & HighMask;

      if (LowBit)
	BitString |= HighMask;
      else
	BitString &= ~HighMask;

      if(HighBit)
	BitString |= LowMask;
      else
	BitString &= ~LowMask;

      LowMask  <<= 1;
      HighMask >>= 1;
     }
    Font [i] = BitString;
   }

  for (i = 0; i < HDataFrame; i++)
    WidthTable [i] = i * WDataFrame;

  for (i = 0; i < BUFF_FRAMES; i++)
    FrameTable [i] = i * SizeDataFrame;
  return Font;
}

static VideoSystem VS;
Graphics GD;

Graphics::~Graphics ()
{ Close (); }

bool Graphics::Initialize (void)
{
  PrintWorkStatusBegin ("Load font");
  InitFont (ArvFont);
  PrintWorkStatusEnd (ArvFont);

  PrintWorkStatusBegin ("Text screen size");
  GetScreenParametrs (spar);
  PrintWorkStatusEnd ("%dx%d", spar.cols, spar.rows);

  VS.WaitVRetrace (false);
  PrintWorkStatusBegin ("Select graphics size");

  long rm, gm, bm;
  if (!VS.FindMode (X, Y, 8, PalEntries, rm, gm, bm))
   {
    PrintWorkStatusEnd ("Failed");
    ErrorExit (4);
   }
  else
    PrintWorkStatusEnd ("%dx%d", X, Y);

  PrintWorkStatusBegin ("Graphics subsystem initialized");
  Square  = X * Y;
  Square9 = WArvFrame * Y;
  PrintWorkStatusEnd ("Success");
  return true;
}

bool Graphics::Open ()
{
  PaletteChanged = false;

  switch (VS.Open ())
  {
    case 0:
      break;
    case -1:
      printf ("VESA ERROR: Incompatible VESA BIOS detected!\n");
      return false;
    case -2:
      printf ("DPMI ERROR: Cannot map videobuffer into linear address space\n");
      return false;
    case -3:
      printf ("DPMI ERROR: Cannot set up a selector for accessing video memory\n");
      return false;
    case -4:
      printf ("MEM ERROR: Not enough memory for allocating a back buffer\n");
      return false;
    default:
      printf ("UNKNOWN ERROR ON VIDEO SUBSYSTEM INITIALIZATION\n");
      return false;
  }

  VS.Clear (0);
  VS.Flush (0, 0, X, Y);
  return true;
}

void Graphics::Close ()
{ VS.Close (); }

void Graphics::Print (csRect *area)
{
  if (PaletteChanged)
  {
    //VS.SetPalette (Palette, 256);
    PaletteChanged = false;
  }

  if (area)
    VS.Flush (area->xmin, area->ymin, area->width, area->height);
  else
    VS.Flush (0, 0, X, Y);
}

bool Graphics::BeginDraw ()
{
  Memory = VS.BackBuffer ();

  for (int i = 0; i < Y; i++)
    Yoffset [i] = (i * X) + (dword)Memory;

  imemset (DimWord, 0, sizeof (DimWord));
  return (Memory != NULL);
}

void Graphics::Clear (int color)
{ VS.Clear (color); }

int Graphics::GetPage ()
{ return VS.GetPage (); }

bool Graphics::DoubleBuffer ()
{ return VS.DoubleBuffer (); }

bool Graphics::DoubleBuffer (bool Enable)
{ return VS.DoubleBuffer (Enable); }

void ErrorExit (dword NumError)
{
  switch (NumError)
   {
    case 1:  GD.Close (); printf ("Memory allocation error!!!\n"); break;
    case 2:  GD.Close (); printf ("Found restore error!!!\n"); break;
    case 3:  GD.Close (); printf ("Not found file!!!\n"); break;
    case 4:  printf ("Cannot find a suitable videomode match!\n"); break;
    default: GD.Close (); printf ("Unknown Error!!!\n"); break;
   }

  exit (NumError);
}

// Make word to vector of 16 bytes
void MakeGraphString (byte *DimWord, word DataWord)
{
#define DO_BIT(x)					 \
  if (x < 4)						 \
    DimWord [x] = ((DataWord & (1 << x)) << 4 - x) - 1;  \
  else							 \
    DimWord [x] = ((DataWord & (1 << x)) >> x - 4) - 1;

  DO_BIT (0)
  DO_BIT (1)
  DO_BIT (2)
  DO_BIT (3)
  DO_BIT (4)
  DO_BIT (5)
  DO_BIT (6)
  DO_BIT (7)
  DO_BIT (8)
  DO_BIT (9)
  DO_BIT (10)
  DO_BIT (11)
  DO_BIT (12)
  DO_BIT (13)
  DO_BIT (14)
  DO_BIT (15)
}

// TVset :-)). Show hardware frame
static void ReceiveAndShowFrame (void)
{
  //connect PC to VM, reset seq, set operate "data"
  outpw (RK, ValueRK = ValueRK | SET_DATA | LINK_VM2PC | SET_RESET_SEQ
   | SET_RECIVE | AutoPhase | Phase | SubPhase | HDensity | Arv105X);

  dword Offset = 0, NumSubFrame = (HArvFrame * WArvFrame) / 0x100 + 1;
  int Xpos, Ypos;

  for (dword i = 0; i < NumSubFrame; i++)
   {
    outpw (RA, i); //RA - set subcadr
    inpw  (RD); //point 2 of hints - that work

    for (dword j = 0; j < 0x100; j++)
     {
      Ypos = Offset / WArvFrame;

      if (Ypos >= Y)
       {
	Delta = Offset - Square9;

	if (Delta >= Square9)
	  Delta -= Square9;

	Ypos = Delta / WArvFrame;
	Xpos = ((Delta % WArvFrame) + (WArvFrame + 1)) << 4;
       }
      else
	Xpos = (Offset % WArvFrame) << 4;

      outpw (RK, ValueRK |= SET_RECIVE);
      MakeGraphString (DimWord, inpw (RD_INC));
      // Copy vector
      imemcpy ((void*)(Yoffset [Ypos] + Xpos), DimWord, 16);
      Offset++;
     }
   }
}

bool TVSet (void)
{
  if (!TVSetFlag)
    return false;

  GD.Open ();
  GD.BeginDraw ();
  GD.Clear (0);

  while (!kbhit ())
   {
    ReceiveAndShowFrame ();
    GD.Print ();
   }

  _bios_keybrd (_KEYBRD_READ); //clear kbd buffer
  GD.FinishDraw ();
  GD.Close ();
  return true;
}

// Print char Litera in Buffer.Frame.Y.X position
void PrintChar (word *Buffer, dword Frame, dword X, dword Y, byte Litera)
{
  byte *dest = (byte*)((int)Buffer + FrameTable [Frame] + WidthTable [Y] + X);
  byte *src  = (byte*)((int)Font   + ((int)Litera << 4));

  //print litera
  for (Y = 0; Y < 16; Y++)
   {
    *dest = src [Y];
    dest  = (byte*)((int)dest + WDataFrame);
   }
}

// Print String in Buffer.Frame.Y.X position
void PrintString (word *Buffer, dword Frame, dword X, dword Y, char* String)
{
  byte Litera;
  dword i = 0;

  while ((Litera = String [i++]))
    PrintChar (Buffer, Frame, X++, Y << 4, Litera);
}

// Show Frame of data
void ShowFrame (word *Frame)
{
  int Xpos = 0, Ypos;

  for (dword Offset = 0; Offset < (SizeDataFrame >> 1); Offset++)
   {
    Ypos = Offset / WArvFrame;

    if (Ypos >= Y)
     {
      Delta = Offset - Square9;

      if (Delta >= Square9)
	Delta -= Square9;

      Ypos = Delta / WArvFrame;
      Xpos = ((Delta % WArvFrame) + (WArvFrame + 1)) << 4;
     }
    else
      Xpos = (Offset % WArvFrame) << 4;

    MakeGraphString (DimWord, Frame [Offset]);
    imemcpy ((void*)(Yoffset [Ypos] + Xpos), DimWord, 16);
   }

  GD.Print ();
}

// Show data buffer
void ShowBuffer (word *Buffer, int Frames)
{
  while (--Frames && !StopByUser)
   {
    ShowFrame (Buffer);
    Buffer = (word*)((int)Buffer + SizeDataFrame);

    if (ShowByTime)
     { //Delay loop
      dword BeginTime = get_time () + 100, EndTime;
      while (BeginTime >= (EndTime = get_time ()) ) {};
     }
    else
      while (!kbhit ()) {}

    if (kbhit ())
     {
      byte KeyCode = (_bios_keybrd (_KEYBRD_READ) >> 8) & 0x7f; //clear kbd buffer

      if (KeyCode == 1)
	StopByUser = true;
      else
      if (KeyCode == 0x39)
	ShowByTime = true;
     }
   }
}

void MakeTextInBuffer (char *Text, word *Buffer, int Frames)
{
  for (int i = 0; i < Frames; i++)
   {
    char NumBuff[20];
    PrintString (Buffer, i, 0, 0, "-=UltraVision=-");
    PrintString (Buffer, i, 0, 1, " Frame:");
    PrintString (Buffer, i, 7, 1, itoa (i, NumBuff, 10));

    for (int j = 2; j < 16; j++)
      PrintString (Buffer, i, 0, j, Text);
   }
}

void MakeHexGridInBuffer (word *Buffer, int Frames)
{
  for (int i = 0; i < Frames; i++)
   {
    word *dest = (word*)((dword)Buffer + FrameTable [i]);
    word Mask1 = 0, Mask2;

    for (dword Y = 0; Y < HDataFrame; Y++)
     {
      if (!(Y & 0xF))
	Mask1 = ~Mask1;
      Mask2 = Mask1;

      for (dword X = 0; X < WArvFrame; X++)
       {
	*dest = Mask2;
	Mask2 = ~Mask2;
	dest++;
       }
     }
   }
}

void MakeGridInBuffer (word InitWord, word *Buffer, int Frames)
{
  for (int i = 0; i < Frames; i++)
   {
    word *dest = (word*)((dword)Buffer + FrameTable [i]);
    word Mask = InitWord;

    for (dword Y = 0; Y < HDataFrame; Y++)
     {
      for (dword X = 0; X < WArvFrame; X++, dest++)
	*dest = Mask;
      Mask = ~Mask;
     }
   }
}

void InitBuffer (word Mask, word *Buffer, int Frames)
{
  PrintWorkStatusBegin ("Init buffer mask");
  //MakeTextInBuffer (TEST_DATA2, Buffer, Frames);
  MakeGridInBuffer (Mask, Buffer, Frames);
  PrintWorkStatusEnd ("%Xh", Mask);
}

void ViewBuffer (word *Buffer, int Frames)
{
  GD.Open ();
  GD.BeginDraw ();
  GD.Clear (0);

  StopByUser = false;

  while (!StopByUser)
    ShowBuffer (Buffer, Frames);

  GD.FinishDraw ();
  GD.Close ();
}

void WaitLoop (int Seconds)
{
  while (Seconds)
   {
    printf ("Last %d seconds", Seconds);
    fflush (stdout);
    dword BeginTime = get_time () + 1000, EndTime;
    while (BeginTime >= (EndTime = get_time ()) ) {};
    printf ("\r");
    Seconds--;
   }
  printf ("\n");
}

void WaitPushKey (void)
{
  while (!kbhit ()) {}
  _bios_keybrd (_KEYBRD_READ); //clear kbd buffer
}

word BasePortTable [] = {0x210,0x230,0x250,0x270,0x290,0x2B0,0x2D0,0x2F0,
			 0x310,0x330,0x350,0x370,0x390,0x3B0,0x3D0,0x3F0,0x00};

char ArvFont [MAX_PATH] = "tavd.fnt";

bool LoadIniFile (void)
{
  PrintWorkStatusBegin ("Initial settings loading & checked");
  csIniFile ini (';');

  ini.Load ("tavd.ini");
  ArvidInsert = !stricmp (ini.GetStr ("hardware settings", "Arvid insert"), "true");

  if (ArvidInsert)
   {
    ArvidModel = atoi (ini.GetStr ("hardware settings", "Model"));
    BasePort   = strtol (ini.GetStr ("hardware settings", "Base Port"), NULL, 16);
    ArvidIRQ   = atoi (ini.GetStr ("hardware settings", "IRQ"));
    Phase      = atoi (ini.GetStr ("hardware settings", "Receive phase")) << 5;
    SubPhase   = atoi (ini.GetStr ("hardware settings", "Receive subphase")) << 11;

    if (!stricmp (ini.GetStr ("hardware settings", "Auto phase"), "true"))
      AutoPhase = SET_AUTO_PHASE;
    else
      AutoPhase = 0;

    if (!stricmp (ini.GetStr ("hardware settings", "Density"), "high"))
      HDensity = SET_325K;
    else
      HDensity = 0;

    DelayToTransmit = atoi (ini.GetStr ("hardware settings", "Delay on transmit"));
   }

  X	   = atoi (ini.GetStr ("screen resolution", "abciss"));
  Y	   = atoi (ini.GetStr ("screen resolution", "ordinat"));
  ErrAudit = !stricmp (ini.GetStr ("common", "Decode with error stat"), "true");
  AsyncFlag = !stricmp (ini.GetStr ("common", "Syncro mode"), "sync");
  ViewBufferFlag = !stricmp (ini.GetStr ("common", "View buffer"), "true");
  strcpy (ArvFont, ini.GetStr ("common", "Font file"));

  int i = 0;
  word Base;
  bool NoBase = true;

  while ((Base = BasePortTable [i++]))
    if (Base == BasePort)
       NoBase = false;

  if (ArvidInsert)
   {
    if (NoBase)
     {
      PrintWorkStatusEnd ("Failed");
      printf ("Base port: %Xh unavaible!!!\n", BasePort);
      exit (0);
     }

    if (ArvidIRQ != 10 && ArvidIRQ != 11 && ArvidIRQ != 12 && ArvidIRQ != 15)
     {
      PrintWorkStatusEnd ("Failed");
      printf ("IRQ: %d unavaible!!!\n", ArvidIRQ);
      exit (0);
     }

    if (Phase > (7 << 5))
     {
      PrintWorkStatusEnd ("Failed");
      printf ("Error phase value!!! Avaible phase 0...7.\n");
      exit (0);
     }

    if (SubPhase > (3 << 11))
     {
      PrintWorkStatusEnd ("Failed");
      printf ("Error subphase value!!! Avaible subphase 0...3.\n");
      exit (0);
     }
   }

  PrintWorkStatusEnd ("Success");
  return true;
}
