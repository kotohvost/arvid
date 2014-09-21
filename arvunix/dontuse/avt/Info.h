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

class my_info {
	char	*name;		// name
	u_int	attr;		// attributes
	u_short	date, time;	// date and time
	char	*desc;		// description
public:
	my_info	*next;		// next info
	my_info	*root;		// root info
	my_info(char *Name, u_int Attr, char *Desc=NULL);
	~my_info();
char	*Name() { return name; }
u_int	Attr() { return attr; }
//off_t	Size() { return size; }
u_short	Date() { return date; }
u_short Time() { return time; }
char	*Desc() { return desc; }
};

my_info::my_info(char *aName, u_int aAttr, char *aDesc) {
	name = new char[strlen(aName) + 1];
	strcpy(name, aName);
	attr = aAttr;
	date = time = 0;
	if (aDesc) {
		desc = new char[strlen(aDesc) + 1];
		strcpy(desc, aDesc);
	} else
		desc = NULL;
	next = root = NULL;
};

my_info::~my_info() {
	if (name)
		delete [] name;
	if (desc)
		delete [] desc;
};

#if 0
void my_info::print() const {
	fprintf(stdout, "%s", name);
	fprintf(stdout, "  %02d/%02d/%d",
		date & 0x1f, (date >> 5) & 0x0f, (date >> 9) + 1980);
	fprintf(stdout, " %02d:%02d:%02d",
		time >> 11, (time >> 5) & 0x3f, (time & 0x1f) << 1);
	fprintf(stdout, "\n");
};

class my_file : public my_info {
	u_int	sector;		// start sector on tape
public:
	my_file(char *name);
void	print();
};

my_file::my_file(char *n) : my_info(n) {
};

my_file::print() {
	my_info::print();
};

class my_dir : my_info {
	my_info	*tree;		// directory entry
public:
	my_dir(char *n);
};

my_dir::my_dir(char *n) : my_info(n) {
};
#endif

#if 0
/* info */
class	Info {
	Info		*next, *prev, *root;	//
	char		*name;
	u_int		attr;
//	u_int		sect;			//
	u_short		date, time;
//	union {
//		u_int		size;		//
//		Info		*dir;		//
//	};
//	u_int		desc;
	/* for dir */
//	u_short		level;			//
public:
	Info(char *name, u_int attr);		//
//	Info(tdr_file *info);			//
//	Info(tdr_file *info, u_short level=0);	//
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
//size_t	getsize() { return(size); };		//
//size_t	getsector() { return(sect); };		//
u_short	getdate() { return(date); };		//
u_short	gettime() { return(time); };		//
//u_int	getdesc() { return(desc); };		//
void	print(void);				//
};

Info::~Info() {
	delete name;
	if (isdir())
		delete dir;
};

Info::Info(char *nm, u_int at) {
int	s, l;

	next = prev = dir = root = NULL;
	s = strlen(nm);
	if (s == 0)
		return;
	name = new char[++s];
	bzero(name, s);
	strcpy(name, nm, s);

	/* other */
	sect = 0;
	size = 0;
//	desc = in->startdesc;
};

char *Info::getattr(char *at, int len) {
	if (len < 10)
		return(at);
	if (at == NULL)
		at = new char[len + 1];
	bzero(at, len + 1);

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

void Info::print() {
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
//	d = getdesc();
//	if (des != NULL && d) {
//		i = des[d - 1];
//		c = 1;
//		while (i--)
//			fprintf(stdout, "%c", des[d + c++]);
//	} else if (d)
//		fprintf(stdout, "%u", d);
	fprintf(stdout, "\n");
};
#endif
