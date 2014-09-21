/*
 * Справочные функции по формату.
 */
#include "tape.h"

int prevformat(int format) {
	switch (format) {
	default:
	case	FORMAT_K4:
		return FORMAT_ERROR;
	case	FORMAT_K7:
		return FORMAT_K4;
	case	FORMAT_K7CRC:
		return FORMAT_K7;
	case	FORMAT_RS:
		return FORMAT_K7CRC;
	case	FORMAT_RSC:
		return FORMAT_RS;
	case	FORMAT_RSC_50:
		return FORMAT_RSC;
	}
};

unsigned int sectformat(unsigned short Format) {
	switch (Format) {
	case	FORMAT_K4:
	case	FORMAT_K7:
	case	FORMAT_K7CRC:
		return 4;
	case	FORMAT_RS:
	case	FORMAT_RSC:
		return 8;
	case	FORMAT_RSC_50:
		return 13;
	}
	return 0;
};
