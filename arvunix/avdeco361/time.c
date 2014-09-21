/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Serge Vakulenko not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Serge Vakulenko makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * SERGE VAKULENKO DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL SERGE VAKULENKO BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

# ifdef MYLOCALTIME
#    include "tm.h"
# else
#    include <time.h>
#    define TM tm
#    define TMlocaltime localtime
# endif

# define STOREINT(p,nn) {\
	register char *cp = p;\
	register n = nn;\
	*cp++ = n/10 + '0';\
	*cp = n%10 + '0';\
	}

static char pattern [] = "00-Mon-1900 00:00:00";

extern char *strncpy ();

char *timestr (long tim) {
	struct TM *t = TMlocaltime (&tim);
	register char *p = pattern;

	STOREINT (p, t->tm_mday);
	if (*p == '0')
		*p = ' ';


	strncpy (p+3, "JanFebMarAprMayJunJulAugSepOctNovDec"+t->tm_mon*3, 3);
	STOREINT (p+9, t->tm_year);
	if (t->tm_year < 100) {
		p[7] = '1';
		p[8] = '9';
	} else {
		p[7] = '2';
		p[8] = '0';
	}
	STOREINT (p+12, t->tm_hour);
	STOREINT (p+15, t->tm_min);
	STOREINT (p+18, t->tm_sec);
	return (p);
}
