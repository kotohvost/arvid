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
/*
 * This routine converts time as follows.
 * The epoch is 0000 Jan 1 1970 GMT.
 * The argument time is in seconds since then.
 *
 * The localtime(t) entry returns a pointer to an array containing
 *  seconds (0-59)
 *  minutes (0-59)
 *  hours (0-23)
 *  day of month (1-31)
 *  month (0-11)
 *  year-1970
 *  weekday (0-6, Sun is 0)
 *  day of the year
 *  daylight savings flag
 *
 * The routine corrects for daylight saving
 * time and will work in any time zone provided
 * "timezone" is adjusted to the difference between
 * Greenwich and local standard time (measured in seconds).
 * In places like Michigan "daylight" must
 * be initialized to 0 to prevent the conversion
 * to daylight time.
 * There is a table which accounts for the peculiarities
 * undergone by daylight time in 1974-1975.
 *
 * The routine does not work
 * in Saudi Arabia which runs on Solar time.
 *
 * asctime(tvec)
 * where tvec is produced by localtime
 * returns a ptr to a character string
 * that has the ascii time in the form
 *      Thu Jan 01 00:00:00 1970\n\0
 *      0123456789012345678901234 5
 *	0	  1	    2
 *
 * ctime(t) just calls localtime, then asctime.
 *
 * tzset() looks for an environment variable named
 * TZ. It should be in the form "ESTn" or "ESTnEDT",
 * where "n" represents a string of digits with an optional
 * negative sign (for locations east of Greenwich, England).
 * If the variable is present, it will set the external
 * variables "timezone", "daylight", and "tzname"
 * appropriately. It is called by localtime, and
 * may also be called explicitly by the user.
 */

# include "tm.h"
# include "env.h"

# define dysize(A) (((A)%4)? 365: 366)

long TMtimezone = 5*60*60;
int TMdaylight = 1;

static char tz1name [4] = "EST";
static char tz2name [4] = "EDT";
char *TMtzname [2] = { tz1name, tz2name, };

struct TM *TMgmtime (), *TMlocaltime ();
void TMtzset ();

static tzinit;
static dmsize [12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*
 * The following table is used for 1974 and 1975 and
 * gives the day number of the first day after the Sunday of the
 * change.
 */

static struct {
	int daylb;
	int dayle;
} daytab [] = {
	5,	333,	/* 1974: Jan 6 - last Sun. in Nov */
	58,	303,	/* 1975: Last Sun. in Feb - last Sun in Oct */
};

static sunday ();

struct TM *TMlocaltime (tim)
long *tim;
{
	register dayno, daylbegin, daylend;
	register struct TM *ct;
	long copyt;

	if (! tzinit)
		TMtzset ();
	copyt = *tim - TMtimezone;
	ct = TMgmtime (&copyt);
	dayno = ct->tm_yday;
	daylbegin = 119;	/* last Sun in Apr */
	daylend = 303;		/* Last Sun in Oct */
	if (ct->tm_year == 74 || ct->tm_year == 75) {
		daylbegin = daytab[ct->tm_year-74].daylb;
		daylend = daytab[ct->tm_year-74].dayle;
	}
	daylbegin = sunday (ct, daylbegin);
	daylend = sunday (ct, daylend);
	if (TMdaylight &&
	    (dayno>daylbegin || (dayno==daylbegin && ct->tm_hour>=2)) &&
	    (dayno<daylend || (dayno==daylend && ct->tm_hour<1))) {
		copyt += 1*60*60;
		ct = TMgmtime (&copyt);
		ct->tm_isdst++;
	}
	return (ct);
}

/*
 * The argument is a 0-origin day number.
 * The value is the day number of the first
 * Sunday on or after the day.
 */

static sunday (t, d)
register struct TM *t;
register d;
{
	if (d >= 58)
		d += dysize (t->tm_year) - 365;
	return (d - (d - t->tm_yday + t->tm_wday + 700) % 7);
}

struct TM *TMgmtime (tim)
long *tim;
{
	register d0, d1;
	long hms, day;
	static struct TM xtime;

	/*
	 * break initial number into days
	 */
	hms = *tim % 86400L;
	day = *tim / 86400L;
	if (hms < 0) {
		hms += 86400L;
		day -= 1;
	}

	/*
	 * generate hours:minutes:seconds
	 */
	xtime.tm_sec = hms % 60;
	d1 = hms / 60;
	xtime.tm_min = d1 % 60;
	d1 /= 60;
	xtime.tm_hour = d1;

	/*
	 * day is the day number.
	 * generate day of the week.
	 * The addend is 4 mod 7 (1/1/1970 was Thursday)
	 */
	xtime.tm_wday = (day + 7340036L) % 7;

	/*
	 * year number
	 */
	if (day >= 0)
		for (d1=70; day >= dysize (d1); ++d1)
			day -= dysize (d1);
	else
		for (d1=70; day<0; --d1)
			day += dysize (d1-1);
	xtime.tm_year = d1;
	xtime.tm_yday = d0 = day;

	/*
	 * generate month
	 */
	if (dysize (d1) == 366)
		dmsize[1] = 29;
	for (d1=0; d0 >= dmsize[d1]; d1++)
		d0 -= dmsize[d1];
	dmsize[1] = 28;
	xtime.tm_mday = d0+1;
	xtime.tm_mon = d1;
	xtime.tm_isdst = 0;
	return (&xtime);
}

void TMtzset ()
{
	register char *p, *q;
	register n;
	int sign;

	tzinit = 1;
	if ((p = EnvGet ("TZ")) && *p) {
		n = 3;
		q = TMtzname[0];
		do {
			*q++ = *p ? *p++ : ' ';
		} while(--n);
		if (sign = *p == '-')
			p++;
		n = 0;
		while (*p >= '0' && *p <= '9')
			n = (n * 10) + *p++ - '0';
		if (sign)
			n = -n;
		TMtimezone = ((long)(n * 60)) * 60;
		if (TMdaylight = *p != '\0') {
			q = TMtzname[1];
			n = 3;
			do {
				*q++ = *p ? *p++ : ' ';
			} while (--n);
		}
	}
}
# endif /* MYLOCALTIME */
