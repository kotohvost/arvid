#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#define STR_LIMIT 60
#define WIDT_SCREEN  360 //default my value
#define HIEST_SCREEN 400 //default my value

extern void InitBuffer (word Mask, word *Buffer, int Frames);
extern bool LoadIniFile (void);
extern void ViewBuffer (word *Buffer, int Frames);
extern void PrintWorkStatusBegin (char *String);
extern void PrintWorkStatusEnd (const char *str, ...);
extern void ErrorExit (dword NumError);
extern void WaitLoop (int Seconds);
extern void WaitPushKey (void);
extern bool TVSet (void);

extern int X, Y;
extern bool TVSetFlag, ViewBufferFlag;
typedef struct {int xmin, ymin, width, height;} csRect;

/// graphics driver
class Graphics
{
  /// Palette has been changed?
  bool PaletteChanged;

public:
  virtual ~Graphics ();

  virtual bool Initialize ();
  virtual bool Open ();
  virtual void Close ();
  virtual void Print (csRect *area = NULL);

  virtual bool BeginDraw ();
  virtual void FinishDraw () {};
  virtual void Clear (int color);
  virtual int  GetPage ();
  virtual bool DoubleBuffer ();
  virtual bool DoubleBuffer (bool Enable);
};

extern Graphics GD;
#endif