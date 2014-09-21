#define MaxXMode 16

typedef struct
{
  byte RedMaskSize;	   // Size of direct color red mask
  byte RedFieldPosition;   // Bit posn of lsb of red mask
  byte GreenMaskSize;	   // Size of direct color green mask
  byte GreenFieldPosition; // Bit posn of lsb of green mask
  byte BlueMaskSize;	   // Size of direct color blue mask
  byte BlueFieldPosition;  // Bit posn of lsb of blue mask
} HiColor;

typedef struct {int mode,rows,cols;} scr_par;
extern scr_par spar;

extern short GetGraphMode  (dword *X, dword *Y, dword *Depth, HiColor *Color);
extern short SetGraphMode  (dword X, dword Y, dword Depth, byte *Memory);
extern short SetGraphSMode (dword X, dword Y, dword Depth, byte *Memory);
extern short SetGraphXMode (dword X, dword Y, byte *Memory);
extern bool  vgaDetect (void);
extern void  GetScreenParametrs (scr_par &spar);

extern void  RestoreVMode (void);
extern void  RestoreScreen (void);
extern byte *SaveTextScreen (void);
extern void  RestoreTextScreen (void);
extern void  PutChar (dword _ch, dword _attr, dword _x, dword _y);
extern void  GetChar (dword *_ch, dword *_attr, dword _x, dword _y);
extern void  PutString (char *_ch, dword _attr, dword _x, dword _y);
extern void  BeginSCRCTRLLockedCode ();
extern void  EndSCRCTRLLockedCode ();

extern void  WaitVrt (dword loop);
extern void  RecalcWidthAdressTable (dword X, dword Y, byte *Memory);
extern void  CopyShadowToScreen (byte *Memory);
extern void  CopyShadowHorRasterToCurVPage (byte *Memory);
extern void  CopyShadowHorRasterToCurVPage_NoFlip (byte *Memory);
extern void  SwitchVideoPage (void);
extern dword GetNearXMode (dword *X, dword *Y);

extern void  SetPalette (dword i, dword r, dword g, dword b);
extern void  SetDotHorRaster (dword XPos, dword YPos, dword Color);
extern void  MoveColumn2HorRaster (dword XBeg, dword YBeg, dword Hiest);
extern void  MoveString2HorRaster (dword XBeg, dword YBeg, dword Lenght);
extern short TestVMode (dword X, dword Y, byte *Memory);

extern int   VBE_Detect (char *str_ver);
extern short VESA_ScanMode (dword X, dword Y, dword Depth, HiColor *Color);
extern short VESA_InitGraphics (dword X, dword Y,int Depth);
extern void  VESA_EndGraphics ();
extern void  VESA_ScreenCopy (void *src);
extern void  AvailableModes ();
extern void  VESA_UpdateScreen (dword X1, dword Y1, dword X2, dword Y2, void *src);

//~~~~~~~~~~~~~~~~~~~~~~~ VGA register's ~~~~~~~~~~~~~~~~~~~~~~~~//
//^^^^^^^^^^^^^^^^^^^^^^^^ General Regs ^^^^^^^^^^^^^^^^^^^^^^^^^//
#define MISC_OUTPUT	  0x3c2
#define MISC_OUTPUT_READ  0x3cc
#define FCW		  0x3da
#define FCR		  0x3ca
#define INPUT_STATUS_0	  0x3c2
#define INPUT_STATUS_1	  0x3da //;reg of mode: VSYNC is activ

//""""""""""""""""""""""" Sequencer Regs """"""""""""""""""""""""//
#define SC_INDEX	  0x3c4
#define SC_DATA 	  0x3c5
#define CPWER		  2
#define MAP_MASK	  02

//'''''''''''''''''''''''''' CRTC Regs ``````````````````````````//
#define CRTC_INDEX	  0x3d4
#define CRTC_DATA	  0x3d5

//------------------------ Graphics Regs ------------------------//
#define GC_INDEX	  0x3ce
#define GC_DATA 	  0x3cf
#define READ_MAP	    04

//~~~~~~~~~~~~~~~~~~~~~~~ Attribute Regs ~~~~~~~~~~~~~~~~~~~~~~~~//
#define ACW_FF		  0x3c0
#define ACR_FF		  0x3c1

//######################### Color Regs ##########################//
#define PEL_MASK	  0x3c6
#define PEL_GET 	  0x3c7
#define PEL_SET 	  0x3c8
#define PEL_DATA	  0x3c9

#define START_ADDRESS_HIGH 0xC	 //bitmap start address high byte
#define START_ADDRESS_LOW  0xD	 //bitmap start address low byte
