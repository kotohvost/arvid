/*
 * !!! - возможные ошибки
 */

#include "stdio.h"
#include "string.h"
#include "Pt.h"

/*
 * Работа с форматом TDR.
 */
/* + */
PT_TDR::~PT_TDR() {
	if (pt)
		delete pt;
};

/* + */
PT_TDR::PT_TDR(void *Ptr, unsigned int Sect) {
	pt = new PTABLE;
	if (Ptr)
		bcopy(Ptr, pt, sizeof(PTABLE));
	MaxSect(Sect);
};

/* + */
void PT_TDR::clear(void) {
	if (!pt)
		pt = new PTABLE;
	EndTime(0);
};

/* + */
unsigned int PT_TDR::PTLength(unsigned int *Ptr) {
	if (Ptr)
		*Ptr = sizeof(PTABLE);
	return (sizeof(PTABLE));
};

/* + */
void *PT_TDR::PTAddress(void *Ptr) {
	if (Ptr && pt)
		bcopy(pt, Ptr, sizeof(PTABLE));
	return pt;
};

/*
 * !!! > Low > 31
 * !!! > High > 141
 */
/* + */
unsigned int PT_TDR::getHashValue(unsigned char Low, unsigned char High) {
unsigned int	rc;

	if (!pt)
		return 0;
	if (Low == 0)
		rc = 0;
	else
		rc = pt->l[High * 31 + Low - 1];
	rc += pt->h[High];
	return rc;
};

/* + */
unsigned int PT_TDR::getHash(unsigned char Low, unsigned char High) {

	if (!pt)
		return 0;
	if (Low == 0)
		if (pt->h[High] == 0)
			return 0;
		else
			return getHashValue(Low, High);
	else
		if (pt->l[High * 31 + Low - 1] == 0)
			return 0;
		else
			return getHashValue(Low, High);
};

/* + */
unsigned int PT_TDR::getHashTime(unsigned char Low, unsigned char High) {
	return (High<<5) + Low;
};

/* + */
void PT_TDR::setsize(unsigned char Low, unsigned char High) {
	if (pt)
		pt->size = getHashTime(Low, High);
};

/* + */
void PT_TDR::setSize(unsigned char Low, unsigned char High) {
unsigned char	*plow;
unsigned short	*phigh;
unsigned int	size;

	if (!pt)
		return;
	setsize(Low, High);
	if (Low != 0) {
		plow = pt->l + High * 31;
		for (size = Low - 1; size < 31; size++)
			plow[size] = 0;
		High++;
	}
	plow = pt->l + High * 31;
	size = (141 - High) * 31;
	memset(plow, 0, size);
	phigh = pt->h + High;
	size = (141 - High) * 2;
	memset(phigh, 0, size);
};

/* + */
void PT_TDR::EndTime(unsigned short Time) {
	setSize((Time >> 3) & 0x1f, Time >> 8);
};

/* + */
unsigned short PT_TDR::nextHash(unsigned char Low, unsigned char High) {
	if (++Low > 31) {
		Low = 0;
		High++;
	}
	return (High << 8) + Low;
};

/* + */
unsigned short PT_TDR::prevHash(unsigned char Low, unsigned char High) {
	if (Low)
		Low--;
	else if (High != 0) {
			High--; Low = 31;
	}
	return (High << 8) + Low;
};

void PT_TDR::insertHash(unsigned short Sect, unsigned char Low, unsigned char High) {
};

/* + */
unsigned short PT_TDR::EndTime(void) {
	if (!pt)
		return 0;
	if (pt->size == 0)
		return 0;
	return pt->size << 8;
};

/* - */
void PT_TDR::Insert(long Sect, unsigned short Time) {
	if (!pt)
		return;
	Sect >>= density;
	Time >>= 3;
	if (pt->size < Time + 1)
		pt->size = Time + 1;
	insertHash(Sect, Time & 0x1f, Time >> 5);
};

/* + */
int PT_TDR::isEmpty(void) {
	if (!pt)
		return 0;
	if (pt->size == 0)
		return 1;
	return 0;
};

/* - */
unsigned short PT_TDR::TimeOfSect(long Sect) {
unsigned short	time;
short		hashsect;
unsigned char	high, low;

	hashsect = Sect >> density;
	if (!pt || !hashsect)
		return 0;
	high = (pt->size - 1) >> 5;
	while(high && pt->h[high] > hashsect)
		high--;
	low = 0;
	while(getHashValue(low, high) < hashsect && low < 31)
		low++;
	time = getHashTime(low, high) << 3;
	time += hashsect - getHashValue(low, high);
	if (time > ((pt->size - 1) << 3))
		time = 0;
	return time;
};

/* + */
void PT_TDR::MaxSect(unsigned int Sect) {
	sector = Sect;
	if (Sect == 8)
		density = 9;
	else
		density = 8;
};

void PT_TDR::print() {
unsigned char	ih, il;
unsigned int	sect;

	if (!pt)
		return;

//	EndTime(12500);

	printf("%u sec.\n", (pt->size - 1) << 3);

	for (ih = 0; ih < 141; ih++) {
		for (il = 0; il < 32; il++) {
			sect = pt->h[ih] << density;
			if (il == 0) {
				printf("%03d-%04X %02d-%02X %09u %d (%u/%u)-%u\n",
					ih, pt->h[ih],
					il, pt->l[ih*31],
					sect, ih*31,
					getHashValue(il, ih),
					getHash(il, ih),
					getHashTime(il, ih)<<3);
			} else {
				sect += pt->l[ih * 31 + il-1] << density;
				printf("%03d-%04X %02d-%02X %09u %d (%u/%u)-%u\n",
					ih, pt->h[ih],
					il, pt->l[ih*31+il-1],
					sect, ih*31 + il - 1,
					getHashValue(il, ih),
					getHash(il, ih),
					getHashTime(il, ih)<<3);
			}
		}
	}
};

/*
 * ==========================================================================
 * Работа с форматом AVT.
 * ==========================================================================
 */
/* - */
PT_AVT::~PT_AVT() {
	if (fpt)
		delete fpt;
	if (pt)
		delete pt;
};

/* - */
PT_AVT::PT_AVT(void *Ptr, unsigned int Sect) {
	fpt = new PTABLE;
	pt = new TABLE;
	if (Ptr)
		bcopy(Ptr, fpt, sizeof(PTABLE));
	sector = Sect;
	unpack();
};

/* - */
void PT_AVT::clear(void) {
	if (!fpt)
		fpt = new PTABLE;
	if (!pt)
		pt = new TABLE;
	EndTime(0);
};

/* - */
unsigned int PT_AVT::PTLength(unsigned int *Ptr) {
	if (Ptr)
		*Ptr = sizeof(PTABLE);
	return (sizeof(PTABLE));
};

/* - */
void *PT_AVT::PTAddress(void *Ptr) {
	if (Ptr) {
		if (pt)
			pack();
		if (fpt)
			bcopy(fpt, Ptr, sizeof(PTABLE));
	}
	if (pt)
		pack();
	if (fpt)
		return fpt;

	return NULL;
};

/* - */
void PT_AVT::EndTime(unsigned short Time) {
};

/* - */
unsigned short PT_AVT::EndTime(void) {
	if (!pt)
		return 0;
	if (pt->size == 0)
		return 0;
	return pt->size << 8;
};

/* - */
void PT_AVT::Insert(long Sect, unsigned short Time) {
	if (!pt)
		return;
	Sect >>= sector;
	Time >>= 3;
	if (pt->size < Time + 1)
		pt->size = Time + 1;
};

/* - */
int PT_AVT::isEmpty(void) {
	if (!pt)
		return 0;
	if (pt->size == 0)
		return 1;
	return 0;
};

/* - */
unsigned short PT_AVT::TimeOfSect(long Sect) {
unsigned short	time;

	time = (Sect << 1) / (sector << 8);
	return time;
};

/* - */
void PT_AVT::MaxSect(unsigned int Sect) {
	sector = Sect;
};

void PT_AVT::pack(void) {
};

void PT_AVT::unpack(void) {
	printf("PTABLE = %u\nTABLE = %X\n",
		sizeof(PTABLE), sizeof(TABLE));
};

/* - */
void PT_AVT::print() {
unsigned char	ih, il;
unsigned int	sect;

	if (!fpt)
		return;

//	EndTime(12500);

	printf("%u sec.\n", (fpt->size - 1) << 2);

	for (ih = 0; ih < 141; ih++) {
		sect = fpt->h[ih];
		for (il = 0; il < 64; il++) {
			if (il == 0) {
				printf("%03d-%08lX %02d-%02X %010u %d %u\n",
					ih, fpt->h[ih],
					il, fpt->l[ih*63],
					sect, ih*63,
					((ih << 6) + il)<< 2);
			} else {
				if (fpt->l[ih*63+il-1] != 0xff)
					sect += fpt->l[ih*63+il-1] << 7;
				printf("%03d-%08lX %02d-%02X %010u %d %u\n",
					ih, fpt->h[ih],
					il, fpt->l[ih*63+il-1],
					sect, ih*63 + il-1,
					((ih << 6) + il)<< 2);
			}
		}
	}
};
