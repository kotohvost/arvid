#ifndef _IRC_H_

#include <stdio.h>
#include "vcr.h"

struct IRC { // struct BUF
            unsigned int *hdb, *api;
            struct IRC_PAR *ik;
            unsigned int *parent, f10, f14, f18;
            unsigned short *buffer;
            unsigned int sizekadrw,
                         maxsizekadrw,
                         f28,
                         f105xtype,
                         mode,
                         f325;
            // end BUF
            unsigned int f38, f3c, done,
                         fpause,
                         *kodbuf,
                         *pkodbuf,
                         curkod,
                         period,
                         sizebitbuf,
                         startflag,
                         curbit;
            unsigned short *pcurbuf;
            unsigned int f68,
                         fstartpause,
                         f70, f74, f78, f7c, f80, f84;
};

void unpackpause (int bit, int period, int kod, unsigned short *buf);
void unpacknmod (int bit, int period, int kod, unsigned short *buf);
void unpackmod (int bit, int period, int kod, unsigned short *buf);

// ------------ settings -----------
void irc_setkodbuf1 (struct IRC *irc, unsigned int *kodbuf);
void irc_setsizeperbuf (struct IRC *irc);
void irc_initwr (struct IRC *irc, unsigned int *kodbuf, U16 period, U16 irc_type);

// ----- settings for gettings -----
void irc_initbitbuf (struct IRC *irc);
void irc_correctbitbuf (struct IRC *irc, unsigned int sizebit);

// ----------- gettings ------------
unsigned int irc_getperiod (struct IRC *irc);
unsigned int irc_unpack (struct IRC *irc);

#if 0
unsigned int irc_write_start (struct IRC *irc);
unsigned int irc_isfreekadr (struct IRC *irc);
unsigned int irc_writehl (struct IRC *irc);

// ---------------------------------
void irc_init_start (struct IRC *irc);
void irc_wrinit_start (struct IRC *irc);
void irc_write_init (struct IRC *irc);
#endif

#endif  // _IRC_H_
