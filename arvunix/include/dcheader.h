#include "tape.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DcStill {
	void	*code;		/* адрес буфера кода */
	int	csize;		/* размер буфера */
	int	coffs;		/* смещение кода в текущем буфере */
	void	*data;		/* адрес данных */
	int	dsize;		/* размер данных */
	void	*cdata;		/* указатель на компреснутый сектор */
	int	csect;		/* номер компреснутого сектора в кадре */
	u_int	dstat;		/* битые сектора */
	UHEADER	header;		/* заголовок кадра */
		/* флаги в кадре */
	unsigned int	fHead;		/* начало записи */
	unsigned int	fTail;		/* конец записи */
	unsigned int	fDir;		/* оглавление */
	unsigned int	fSpec;		/* */
};

/* Декодирование заголовка кадра. 0 - OK */
int DcHeader(void *code, UHEADER *header, int *coffs, int format);

/* Флаги кадра. */
int flagFromHeader(struct DcStill *Dc);

/* Декодирование кадра. 0 - OK */
int DcData(void *code, void *data, UHEADER *header, int coffs, unsigned int *st);

/* Номер первого сектора в кадре. */
int getSector(UHEADER *header);

/* Количество секторов в кадре. */
int getAvailSector(UHEADER *header);

/* Попадание сектора в кадр.	*
 * -1	перелет			*
 *  0	есть такой		*
 * +1	недолет			*/
int isSector(UHEADER *header, unsigned int sector);

/* Чтение данных из декодированного кадра.   *
 * возвращает количество прочитанных данных. */
int readData(struct DcStill *Dc, void *buf, unsigned int sect, unsigned int size);

#ifdef __cplusplus
}
#endif
