/*
 * Таблица позиционирования.
 */
#ifndef POSTABLE
#define POSTABLE

/*
 * class PosTable - чисто виртуальнай класс
 */

class PosTable {
public:
virtual unsigned int	version(void) = 0;
virtual void		clear(void) = 0;
virtual void		EndTime(unsigned short) = 0;
virtual void		Insert(long, unsigned short) = 0;
virtual unsigned short	EndTime(void) = 0;
virtual unsigned short	TimeOfSect(long) = 0;
virtual int		isEmpty(void) = 0;
virtual void		*PTAddress(void * = 0) = 0;
virtual unsigned int	PTLength(unsigned int * = 0) = 0;
virtual void		MaxSect(unsigned int) = 0;

virtual void		print(void) = 0;
};

/*
 * Работа с таблицей TDR.
 */
class PT_TDR : public PosTable {
	struct	PTABLE {
		unsigned short	size;
		unsigned short	h[141];
		unsigned char	l[141*31];
		unsigned char	resv;
	}		*pt;
	unsigned int	sector;		/* секторов в кадре */
	unsigned int	density;	/* плотность таблицы	*/
					/* 8 - 100K/s		*/
					/* 9 - 200K/s		*/

unsigned int	getHashValue(unsigned char, unsigned char);
unsigned int	getHash(unsigned char, unsigned char);
unsigned int	getHashTime(unsigned char, unsigned char);
void		setsize(unsigned char, unsigned char);
void		setSize(unsigned char, unsigned char);
unsigned short	nextHash(unsigned char, unsigned char);
unsigned short	prevHash(unsigned char, unsigned char);
void		insertHash(unsigned short, unsigned char, unsigned char);

public:
	PT_TDR(void *, unsigned int);
	~PT_TDR();

unsigned int	version(void) { return 0; };
void		clear(void);
void		EndTime(unsigned short);
void		Insert(long, unsigned short);
unsigned short	EndTime(void);
unsigned short	TimeOfSect(long);
int		isEmpty(void);
void		*PTAddress(void *);
unsigned int	PTLength(unsigned int *);
void		MaxSect(unsigned int);

void		print(void);
};

/*
 * Работа с таблицей AVT.
 */
class PT_AVT : public PosTable {
	struct	PTABLE {
		unsigned int	size;
		unsigned int	h[141];
		unsigned char	l[141*63];
		unsigned char	resv;
	}		*fpt;
	struct	TABLE {
		unsigned int	size;
		struct	STR {
			unsigned int	h;
			unsigned short	l[63];
		}	str[141];
	}		*pt;
	unsigned int	sector;		/* секторов в кадре	*/

void		pack(void);
void		unpack(void);

public:
	PT_AVT(void *, unsigned int);
	~PT_AVT();

unsigned int	version(void) { return 1; };
void		clear(void);
void		EndTime(unsigned short);
void		Insert(long, unsigned short);
unsigned short	EndTime(void);
unsigned short	TimeOfSect(long);
int		isEmpty(void);
void		*PTAddress(void *);
unsigned int	PTLength(unsigned int *);
void		MaxSect(unsigned int);

void		print(void);
};

#endif
