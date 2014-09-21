#ifndef _RLE_H_

#include <stdio.h>
#include "vcr.h"

#define RLEBUFSIZE      240
struct RLE { unsigned int f0,
                          nperiod,
                          f8,
                          maxpause,
                          f10,
                          buf [RLEBUFSIZE];
                          struct IRC_PAR  *ik;
};

void rle_unpack (struct RLE *rle, int control);
unsigned int *rle_getbuf (struct RLE *rle);
unsigned short rle_getmaxpause (struct RLE *rle);
struct RLE *rle_init (struct RLE *rle, int control, struct IRC_PAR *ik);

#endif  // _RLE_H_
