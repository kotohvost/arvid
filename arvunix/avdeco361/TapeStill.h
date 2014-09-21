#include "avtreg.h"

// return error
//enum {
#define	TSOK		0	// operation ok
#define	TSPBAD		1	// bad parameter
#define	TSOEXIST	2	// Ident - tape already ident
#define	TSOERROR	3	// Ident - error open file or device
#define	TSIOCTLERROR	4	// Ident - ioctl request error
#define	TSMEMERROR	5	// Ident - memory allocated error
#define	TSREADERROR	6	// Ident - read error
#define	TSNOTAPE	7	// Ident - ident error
				// GetTape - tape not found
//} TSERROR;

/*
 * Tape Parametr Block
 */
struct TPB {
	unsigned short	format;
	unsigned short	length;
	unsigned int	id;
	unsigned char	name[8];
};

class TapeStill {
	char		*dev;		// device name
	int		fd;		// file description
	struct avt_info	avt_info;	// arvid info
	struct set_rw	avt_rw;		// arvid mode

	TPB		tpb;		// tape param block
//	char		*name;	// tape name
//	unsigned int	id;	// tape id
public:
	TapeStill(char *);
	~TapeStill();
int	Ident();	// ident tape
int	GetTape(TPB *);	// get TPB
};
