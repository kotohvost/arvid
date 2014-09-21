#include "tdr.h"
/* атрибуты входа */
#define	TAPE_DIR	0x00040000
#define	TAPE_RWXU	0x00000700
#define	TAPE_RUSR	0x00000400
#define	TAPE_WUSR	0x00000200
#define	TAPE_XUSR	0x00000100
#define	TAPE_RWXG	0x00000070
#define	TAPE_RGRP	0x00000040
#define	TAPE_WGRP	0x00000020
#define	TAPE_XGRP	0x00000010
#define	TAPE_RWXO	0x00000007
#define	TAPE_ROTH	0x00000004
#define	TAPE_WOTH	0x00000002
#define	TAPE_XOTH	0x00000001

class	Info {
	Info		*next, *prev, *root;	//
	tdr_file	info;			//
	char		*name;
	u_int		attr;
	u_int		sect;
	u_int		size;
	u_int		des;
	char		*desc;
	/* for dir */
	u_short		level;			//
	Info		*dir;			//
public:
//	Info(char *name);			//
//	Info(tdr_file *info);			//
	Info(tdr_file *info, u_short level=0);	//
	~Info();
int	isdir() { return(attr & TAPE_DIR); };	//
Info	*getroot() { return(root); };		//
	setroot(Info *p) { root = p; };		//
Info	*getdir() { return(dir); };		//
	setdir(Info *p) { dir = p; };		//
Info	*getnext() { return(next); };		//
	setnext(Info *p) { next = p; };		//
Info	*getprev() { return(prev); };		//
	setprev(Info *p) { prev = p; };		//
char	*getname() { return(name); };		//
char	*getattr(char *attr, int len=10);	//
u_int	getattr() { return(attr); };		//
size_t	getsize() { return(size); };		//
size_t	getsector() { return(sect); };		//
u_short	getdate() { return(info.dd); };		//
u_short	gettime() { return(info.dt); };		//
u_int	getdesc() { return(des); };		//
//char	*getdesc() { return(desc); };		//
void	setdesc(char *d);			//
void	print(u_char *des);			//
};

Info::~Info() {
	delete name;
};

Info::Info(tdr_file *in, u_short lev) {
int	i, l;

	next = prev = dir = root = NULL;
	bcopy(in, &info, sizeof(info));
	name = new char[255];
	bzero(name, 255);
	level = lev;

	/*
	 * set name
	 */
	for(i = 0; i < 8; i++)
		name[i] = in->name[i];
	l = 8;
	while(l && (name[l] == ' ' || name[l] == 0))
		name[l--] = 0;
	name[++l] = '.';
	l++;
	for(i = 0; i < 3; i++)
		name[l++] = in->ext[i];
	i = 8 + 1 + 3;
	while(i && (name[i] == ' ' || name[i] == 0 || name[i] == '.'))
		name[i--] = 0;
	if (!i && name[i] == ' ')
		name[i] = '/';

	/*
	 * attributes
	 */
	attr = TAPE_RUSR | TAPE_RGRP | TAPE_ROTH;
	attr |= TAPE_WUSR | TAPE_WGRP;
	/* Dir */
	if (in->attr & 0x10)
		attr |= TAPE_DIR | TAPE_XUSR | TAPE_XGRP | TAPE_XOTH;
	/* System */
	if (in->attr & 0x04)
		attr &= ~(TAPE_ROTH | TAPE_RGRP | TAPE_WOTH | TAPE_WGRP);
	/* Hiden */
	if (in->attr & 0x02)
		attr &= ~(TAPE_ROTH | TAPE_WOTH);
	/* Read only */
	if (in->attr & 0x01)
		attr &= ~(TAPE_WUSR | TAPE_WGRP | TAPE_WOTH);

	/* other */
	sect = in->startsect;
	size = in->size;
	des = in->startdesc;
	desc = NULL;
};

char *Info::getattr(char *at, int len) {
	if (len < 10)
		return(at);
	if (at == NULL)
		at = new char[++len];
	bzero(at, len);

	if (attr & TAPE_DIR)	at[0] = 'd';
	else			at[0] = '-';
	if (attr & TAPE_RUSR)	at[1] = 'r';
	else			at[1] = '-';
	if (attr & TAPE_WUSR)	at[2] = 'w';
	else			at[2] = '-';
	if (attr & TAPE_XUSR)	at[3] = 'x';
	else			at[3] = '-';
	if (attr & TAPE_RGRP)	at[4] = 'r';
	else			at[4] = '-';
	if (attr & TAPE_WGRP)	at[5] = 'w';
	else			at[5] = '-';
	if (attr & TAPE_XGRP)	at[6] = 'x';
	else			at[6] = '-';
	if (attr & TAPE_ROTH)	at[7] = 'r';
	else			at[7] = '-';
	if (attr & TAPE_WOTH)	at[8] = 'w';
	else			at[8] = '-';
	if (attr & TAPE_XOTH)	at[9] = 'x';
	else			at[9] = '-';
	return(at);
};

void Info::print(u_char *des) {
int	i, c;
u_int	d;
char	*name;
u_short	dd;

	name = NULL;
	fprintf(stdout, "%s", getattr(name));
	fprintf(stdout, "\t%10u", getsize());
	dd = getdate();
	fprintf(stdout, "  %02d/%02d/%d",
		dd & 0x1f, (dd >> 5) & 0x0f, (dd >> 9) + 1980);
	dd = gettime();
	fprintf(stdout, " %02d:%02d:%02d",
		dd >> 11, (dd >> 5) & 0x3f, (dd & 0x1f) << 1);
	fprintf(stdout, "  %7u", getsector());
	fprintf(stdout, " ");
	fprintf(stdout, "%s", getname());
	if (isdir() && level)
		fprintf(stdout, "/");
	fprintf(stdout, "\t");
	d = getdesc();
	if (des != NULL && d) {
		i = des[d - 1];
		c = 1;
		while (i--)
			fprintf(stdout, "%c", des[d + c++]);
	} else if (d)
		fprintf(stdout, "%u", d);
	fprintf(stdout, "\n");
};

void Info::setdesc(char *d) {
u_int	len;

	if (desc != NULL)
		delete desc;
	if (d == NULL)
		desc = NULL;
	else {
		len = strlen(d);
		desc = new char[++len];
		strncpy(desc, d, len);
	}
};
