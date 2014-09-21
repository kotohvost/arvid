#include "sys/types.h"

typedef struct {
	char	from[1024];	/* полный путь на ленте (From) */
	char	to[1024];	/* полный путь на диске (To) */
	u_long	sector;		/* стартовый сектор */
	size_t	size;		/* размер файла */
	time_t	time;		/* время файла */
	u_long	attr;
	uid_t	uid;		/* user */
	gid_t	gid;		/* group */
//	FILE	*file;		/* дескриптор открытого файла */
	u_long	state;		/* флаги */
} ReadWrite;

/*
 * Статусы файла
 * ReadWrite->State
 */
#define	OK		0x0000	/* ошибок нет */
#define	ER_READ		0x0001	/* ошибка при чтении */
#define	ER_WRITE	0x0002	/* ошибка при записи */
#define	ER_OPEN		0x0004	/* ошибка при открытии */
#define IS_WORK		0x8000	/* для работы */

/*
 * ReadFile - работа с запросом.
 */
class ReadFiles {
	unsigned int	numList;	/* количество запросов */
	unsigned int	alloc;		/* выделенных запросов */
	ReadWrite	*list;		/* массив запросов на чтение */
public:
	ReadFiles();
	~ReadFiles();
int	AddReadFile(ReadWrite *rw);
//int	DelReadFile();
};
