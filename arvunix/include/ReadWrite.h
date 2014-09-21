#include "sys/types.h"

typedef struct {
	char	from[1024];	/* ������ ���� �� ����� (From) */
	char	to[1024];	/* ������ ���� �� ����� (To) */
	u_long	sector;		/* ��������� ������ */
	size_t	size;		/* ������ ����� */
	time_t	time;		/* ����� ����� */
	u_long	attr;
	uid_t	uid;		/* user */
	gid_t	gid;		/* group */
//	FILE	*file;		/* ���������� ��������� ����� */
	u_long	state;		/* ����� */
} ReadWrite;

/*
 * ������� �����
 * ReadWrite->State
 */
#define	OK		0x0000	/* ������ ��� */
#define	ER_READ		0x0001	/* ������ ��� ������ */
#define	ER_WRITE	0x0002	/* ������ ��� ������ */
#define	ER_OPEN		0x0004	/* ������ ��� �������� */
#define IS_WORK		0x8000	/* ��� ������ */

/*
 * ReadFile - ������ � ��������.
 */
class ReadFiles {
	unsigned int	numList;	/* ���������� �������� */
	unsigned int	alloc;		/* ���������� �������� */
	ReadWrite	*list;		/* ������ �������� �� ������ */
public:
	ReadFiles();
	~ReadFiles();
int	AddReadFile(ReadWrite *rw);
//int	DelReadFile();
};
