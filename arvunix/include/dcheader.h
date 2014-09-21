#include "tape.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DcStill {
	void	*code;		/* ����� ������ ���� */
	int	csize;		/* ������ ������ */
	int	coffs;		/* �������� ���� � ������� ������ */
	void	*data;		/* ����� ������ */
	int	dsize;		/* ������ ������ */
	void	*cdata;		/* ��������� �� ������������ ������ */
	int	csect;		/* ����� ������������� ������� � ����� */
	u_int	dstat;		/* ����� ������� */
	UHEADER	header;		/* ��������� ����� */
		/* ����� � ����� */
	unsigned int	fHead;		/* ������ ������ */
	unsigned int	fTail;		/* ����� ������ */
	unsigned int	fDir;		/* ���������� */
	unsigned int	fSpec;		/* */
};

/* ������������� ��������� �����. 0 - OK */
int DcHeader(void *code, UHEADER *header, int *coffs, int format);

/* ����� �����. */
int flagFromHeader(struct DcStill *Dc);

/* ������������� �����. 0 - OK */
int DcData(void *code, void *data, UHEADER *header, int coffs, unsigned int *st);

/* ����� ������� ������� � �����. */
int getSector(UHEADER *header);

/* ���������� �������� � �����. */
int getAvailSector(UHEADER *header);

/* ��������� ������� � ����.	*
 * -1	�������			*
 *  0	���� �����		*
 * +1	�������			*/
int isSector(UHEADER *header, unsigned int sector);

/* ������ ������ �� ��������������� �����.   *
 * ���������� ���������� ����������� ������. */
int readData(struct DcStill *Dc, void *buf, unsigned int sect, unsigned int size);

#ifdef __cplusplus
}
#endif
