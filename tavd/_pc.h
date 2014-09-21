#ifndef __PC_H__
#define __PC_H__

#define ScreenPrimary 0xB8000

static inline int ScreenMode (void)
{ return (int)(*((char*)0x449)); }

static inline int ScreenRows (void)
{ return (int)(*((char*)0x484) + 1); }

static inline int ScreenCols (void)
{ return (int)(*((short*)0x44A)); }

static inline void ScreenSetCursor (int  _row, int  _col) {}
static inline void ScreenGetCursor (int *_row, int *_col) {}
static inline void ScreenClear (void) {}

static inline int gettext (int _left, int _top, int _right, int _bottom, void *_destin)
{
  int width = _right - _left;
  short *pScr = (short*)(ScreenPrimary + (spar.cols * _top + _left) * 2);
  short *pBuff = (short*)_destin;

  for (int i = 0; i < _bottom - _top; i++)
   {
    for (int j = 0; j < width; j++, pBuff++, pScr++)
      *pBuff = *pScr;

    pScr += spar.cols - width;
   }

  return 1;
}

static inline int puttext (int _left, int _top, int _right, int _bottom, void *_source)
{
  int width = _right - _left;
  short *pScr = (short*)(ScreenPrimary + (spar.cols * _top + _left) * 2);
  short *pBuff = (short*)_source;

  for (int i = 0; i < _bottom - _top; i++)
   {
    for (int j = 0; j < width; j++, pBuff++, pScr++)
      *pScr = *pBuff;

    pScr += spar.cols - width;
   }

  return 1;
}

#endif