/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

# define LTZNMAX 10

struct TM {     /* see ctime(S) */
	int	tm_sec;		/* time of day, seconds */
	int	tm_min;		/* time of day, minutes */
	int	tm_hour;	/* time of day, hours (24 hour clock) */
	int	tm_mday;	/* day of month (1-31) */
	int	tm_mon;		/* month of year (0-11) */
	int	tm_year;	/* year - 1900 */
	int	tm_wday;	/* day of week (Sunday = 0) */
	int	tm_yday;	/* day of year (0-365) */
	int	tm_isdst;	/* non-0 if summer time in effect */
	long	tm_tzadj;	/* seconds from GMT (east < 0) */
	char    tm_name [LTZNMAX];      /* name of timezone */
};

extern struct TM *TMgmtime (), *TMlocaltime ();
extern void TMtzset ();
extern long TMtimezone;
extern int TMdaylight;
extern char *TMtzname[];
