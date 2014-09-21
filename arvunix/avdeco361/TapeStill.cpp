/* system header include */
#include "stdio.h"
#include "fcntl.h"
#include "unistd.h"
#include "string.h"
#include "sys/ioctl.h"

/* my header include */
//#include "avtreg.h"
#include "format.h"
#include "dcheader.h"
#include "TapeStill.h"

TapeStill::TapeStill(char *avddev) {
	fd = -1;
//	name = NULL;
//	id = 0;
	bzero(&tpb, sizeof(tpb));
	dev = new char[strlen(avddev) + 1];
	strcpy(dev, avddev);
};

TapeStill::~TapeStill() {
	if (fd >= 0)
		close(fd);

/*	if (name)
		delete name;
*/
	if (dev)
		delete dev;
};

int TapeStill::Ident() {
int		rc, done, cformat, offscode, i, phase;
unsigned int	rate;
void		*code;
UHEADER		head;

	rc = TSOK;

	/* checking */
/*	if (name && *name || id)
		return TSOEXIST;
*/
	if (tpb.format && tpb.length && tpb.id)
		return TSOEXIST;

	/* reopen */
	if (fd < 0) {
		fd = open(dev, O_RDWR, 0);
		if (fd < 0)
			return TSOERROR;
	}

	/* get info */
	if (ioctl(fd, VGET_INFO, &avt_info) < 0)
		return TSIOCTLERROR;

	/* set phase */
	phase = 1;
	if (ioctl(fd, VSET_PHASE, &phase) < 0)
		return TSIOCTLERROR;

	/* set max format */
	if (avt_info.arvid >= Av1051) {
		rate = 325;
		cformat = FORMAT_RSC_50;
	} else if (avt_info.arvid >= Av1020) {
		rate = 200;
		cformat = FORMAT_RSC;
	} else {
		rate = 100;
		cformat = FORMAT_K7CRC;
	}

	/* set prepare */
	avt_rw.rate = rate;
	avt_rw.mode = IORAW;
	if (ioctl(fd, VSET_READ, &avt_rw) < 0)
		return TSIOCTLERROR;

	code = new char[SIZE325];	
	if (code == NULL) {
		return TSMEMERROR;
	}

	/* ident */
	done = 1;
	i = 40;
	while (done && i-- > 0) {
		int	cf;

		bzero(code, SIZE325);
		while (!(done = read(fd, code, SIZE325)));
		if (done < 0) {
			delete code;
			return TSREADERROR;
		}

		/* get Header and find format */
		cf = cformat;
		while (cf != FORMAT_ERROR && done) {
			done = DcHeader(code, &head, &offscode, cf);
			if (done)
				cf = prevformat(cf);
		}
	}

	if (done)
		return TSNOTAPE;

	delete code;

	/* fill TPB */
	tpb.format = head.g.format;
	bcopy(head.g.name, tpb.name, 8);
	tpb.id = head.g.ident;
	tpb.length = head.g.length;
	return rc;
};

int TapeStill::GetTape(TPB *Tpb) {
int	rc;

	rc = TSOK;
	if (!Tpb)
		rc = TSPBAD;
	else if (tpb.format && tpb.length && tpb.id)
		bcopy(&tpb, Tpb, sizeof(tpb));
	else
		rc = TSNOTAPE;
	return rc;
};
