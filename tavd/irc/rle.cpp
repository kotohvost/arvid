#include <stdio.h>

#include "vcr.h"
#include "rle.h"

void rle_unpack (struct RLE *rle, int control)
{
  unsigned char key, *pkey;
  unsigned int count, type;

  rle->maxpause = 0;
  rle->nperiod = 0;

  if (control >= KEY_NR)
   {
    *rle->buf = 0;
    return;
   }

  key = rle->ik->key_ref [control];
  fprintf (stdout, "key = %d\n", key);

  if (key == 0xFF)
   {
    *rle->buf = 0;
    return;
   }

  pkey = (unsigned char *)&rle->ik->keys[key];
  fprintf (stdout, "pkey = %u\n", (unsigned int) pkey);
  count = 0;

  while (count < RLEBUFSIZE)
   {
    key = *pkey++;
    fprintf (stdout, "key = %02X, pkey = %u\n", key, (unsigned int) pkey);
    type = key & 0x0F;

    if (type == 0x0F)
      break;

    type = rle->ik->irc_header.type_pack[type];
    rle->buf[count++] = type;
    rle->nperiod += type;
    key >>= 4;

    if (key == 0x0F)
      break;

    type = rle->ik->irc_header.type_pause[key];
    rle->buf[count++] = type;

    if (rle->maxpause < type)
      rle->maxpause = type;
    rle->nperiod += type;
   }

  rle->buf[count] = 0;

  { unsigned int i;
  fprintf (stdout, "rle->nperiod  = %u/%08X\n", rle->nperiod, rle->nperiod);
  fprintf (stdout, "rle->maxpause = %u/%08X\n", rle->maxpause, rle->maxpause);
  fprintf (stdout, "rle->buf:\n");

  for (i = 0; i < count; i++)
   {
    fprintf (stdout, "%04X", rle->buf[i]);

    if (!((i+1)%16))
      fprintf (stdout, "\n");
    else
      fprintf (stdout, " ");
   }

  fprintf (stdout, "\n");
  }

  return;
}

unsigned int *rle_getbuf (struct RLE *rle)
{
  return rle->buf;
}

unsigned short rle_getmaxpause (struct RLE *rle)
{
  return (rle->maxpause);
}

// RLE::RLE (int control, IRC_PAR *ik)
struct RLE *rle_init (struct RLE *rle, int control, struct IRC_PAR *ik)
{
  fprintf (stdout, "control = %d\n", control);
  rle->f8 = 0;
  rle->f10 = 0;
  rle->ik = ik;

  if (control >= 0)
    rle_unpack (rle, control);

  return rle;
}
