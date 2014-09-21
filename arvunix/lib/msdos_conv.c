#include "tape.h"

#define	DD_2SECONDS_MASK	0x1F
#define DD_2SECONDS_SHIFT	0
#define	DD_MINUTES_MASK		0x7E0
#define	DD_MINUTES_SHIFT	5
#define	DD_HOURS_MASK		0xF800
#define	DD_HOURS_SHIFT		11
#define	DD_DAY_MASK		0x1F
#define	DD_DAY_SHIFT		0
#define	DD_MONTH_MASK		0x1E0
#define	DD_MONTH_SHIFT		5
#define	DD_YEAR_MASK		0xFE00
#define	DD_YEAR_SHIFT		9
#define	SECONDSTO1980	(((8 * 365) + (2 * 366)) * (24 * 60 * 60))

static unsigned short regyear[] = {
	31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
static unsigned short leapyear[] = {
	31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

long dos2unixtime(unsigned short dd, unsigned short dt) {
long		myear=0;
long		days=0;
unsigned char	*tt;
unsigned short	*months;

	/* convert dostime to unixtime */
	myear = (dd & DD_YEAR_MASK) >> DD_YEAR_SHIFT;
	days = myear * 365;
	days += myear/4 + 1;
	if ((myear & 0x3) == 0)
		days--;
	months = myear & 0x3 ? regyear : leapyear;
	myear = (dd & DD_MONTH_MASK) >> DD_MONTH_SHIFT;
	if (myear < 1 || myear > 12)
		myear = 1;
	if (myear > 1)
		days += months[myear - 2];
	days += (dd & DD_DAY_MASK) - 1; /* >> DD_DAY_SHIFT */
	days = (days * 24 * 60 * 60) + SECONDSTO1980;
	days += ((dt & DD_2SECONDS_MASK) << 1)
		+ ((dt & DD_MINUTES_MASK) >> DD_MINUTES_SHIFT) * 60
		+ ((dt & DD_HOURS_MASK) >> DD_HOURS_SHIFT) * 3600;

	return(days);
};

/* атрибуты от DOS */
#define	FDOS_READ	0x01
#define	FDOS_HYD	0x02
#define	FDOS_SYS	0x04
#define	FDOS_VOL	0x08
#define	FDOS_DIR	0x10
#define	FDOS_ARC	0x20

u_int dos2tapeattr(u_char at) {
u_int	attr;

//	attr = (TAPE_DIR | TAPE_RWXU | TAPE_RWXG | TAPE_ROTH | TAPE_XOTH);
	attr = 0;
	if (at & FDOS_DIR) {
		attr |= TAPE_DIR | TAPE_XUSR;
		if ((at & FDOS_HYD) || (at & FDOS_SYS))
			attr |= TAPE_XGRP;
		else
			attr |= TAPE_XGRP | TAPE_XOTH;
		};

	attr |= TAPE_RUSR;
	if ((at & FDOS_HYD) || (at & FDOS_SYS))
		attr |= TAPE_RGRP;
	else
		attr |= TAPE_RGRP | TAPE_ROTH;

	if (!(at & FDOS_READ))
		attr |= TAPE_WUSR;

/*
#define	FDOS_VOL	0x08
#define	FDOS_ARC	0x20
*/
	return(attr);
};
