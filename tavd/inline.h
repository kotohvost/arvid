#ifndef __INLINE_H__
#define __INLINE_H__

static inline void imemset (void *pnt, byte Mask, dword NumByte)
{
  dword NumDword = NumByte >> 2;
  dword LastByte = NumByte & 3;

  if (NumDword)
   {
    dword dMask = Mask;
    dMask <<= 8;
    dMask |= Mask;
    dMask <<= 8;
    dMask |= Mask;
    dMask <<= 8;
    dMask |= Mask;

    for (dword i = 0; i < NumDword; i++)
      ((dword*)pnt)[i] = dMask;

    pnt = (void*)((dword)pnt + (NumDword << 2));
   }

  for (dword i = 0; i < LastByte; i++)
    ((byte*)pnt) [i] = Mask;
}

static inline void imemcpy (void *dst, void *src, dword NumByte)
{
  dword NumDword = NumByte >> 2;
  dword LastByte = NumByte & 3;

  if (NumDword)
   {
    for (dword i = 0; i < NumDword; i++)
      ((dword*)dst) [i] = ((dword*)src) [i];

    dword AddrIncr = NumDword << 2;
    src = (void*)((dword)src + AddrIncr);
    dst = (void*)((dword)dst + AddrIncr);
   }

  for (dword i = 0; i < LastByte; i++)
    ((byte*)dst) [i] = ((byte*)src) [i];
}

static inline void imemcpyb (void *dst, void *src, dword NumByte)
{
  for (dword i = 0; i < NumByte; i++)
    ((byte*)dst) [i] = ((byte*)src) [i];
}

#endif