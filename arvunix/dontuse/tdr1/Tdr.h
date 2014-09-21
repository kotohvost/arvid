#include "Info.h"

class	List : public Info {
	Info	*rootdir, *cdir;		//
	char	*cpwd;				// cur dir name

void	prints(Info *dir, u_char *des);		//

public:
	List(tdr_file *, u_short lev=0);	//
	~List();				//
void	store(tdr_file *, u_short lev=0);	//
//	remove();				// cur
int	chdir(char *name);			//
char	*pwd();					// get cur dir name
void	print(u_char *des, int sub = 0);	//
};

List::List(tdr_file *in, u_short lev=0) : Info(in, lev) {
	rootdir = cdir = new Info(in, lev);
	cpwd = new char[1024];
	bzero(cpwd, 1024);
	strncpy(cpwd, rootdir->getname(), 1023);
};

List::~List() {
Info	*p, *np, *pp;
u_int	f, d;

	f = d = 0;
	cdir = rootdir;
	while(cdir) {
		p = cdir->getdir();
		/*
		 * Remove all files in current dir
		 */
		while(p) {
			pp = p->getprev();
			np = p->getnext();
			if (p->isdir()) {
				p = np;
				goto nextfile;
			}
			if (pp)
				pp->setnext(np);
			else
				cdir->setdir(np);
			if (np)
				np->setprev(pp);
			delete p;
			p = np;
			f++;
nextfile:;
		}
		/*
		 * Remove all empty dirs in current dir
		 */
		p = cdir->getdir();
		while(p) {
			if (p->getdir()) {
				cdir = p;
				goto nextdir;
			}
			pp = p->getprev();
			np = p->getnext();
			if (pp)
				pp->setnext(np);
			else
				cdir->setdir(np);
			if (np)
				np->setprev(pp);
			delete p;
			p = np;
			d++;
		}
		cdir = cdir->getroot();
nextdir:;
	}
	delete rootdir;
//	fprintf(stdout, "%d file's in %d dir's\n", f, d);
};

/*
 * Store Info in current dir (*cdir)
 */
void List::store(tdr_file *in, u_short lev) {
Info	*p, *pp;

	/* DON'T */
	if (cdir == NULL)
		cdir = rootdir;
	if (cdir == NULL) {
		fprintf(stderr, "panic: cdir = rootdir = NULL\n");
		return;
	}
	pp = cdir->getdir();
	p = new Info(in, lev);
	p->setroot(cdir);
	if (pp) {
		while(pp->getnext())
			pp = pp->getnext();
		pp->setnext(p);
		p->setprev(pp);
	} else {
		cdir->setdir(p);
	}
//	p->print(NULL);
};

int List::chdir(char *name) {
Info	*p;

//	fprintf(stdout, "ChDir to '%s'\n", name);
	if (cdir == NULL)
		cdir = rootdir;
	if (cdir == NULL) {
		fprintf(stderr, "panic: cdir = rootdir = NULL\n");
		return(0);
	}
	if (*name == '/') {
		cdir = rootdir;
		while(*name == '/')
			name++;
		if (!*name)
			return(1);
	}
	if (name[0] == '.' && name[1] == 0)
		return(1);
	if (name[0] == '.' && name[1] == '.' && name[2] == 0) {
		p = cdir->getroot();
		if (p)
			cdir = p;
//		fprintf(stdout, "pwd: '%s'\n", pwd());
		return(1);
	}
	if (cdir->isdir() == 0)
		return(0);
	p = cdir->getdir();
	while(p) {
		if (p->isdir() && !strncmp(name, p->getname(), 255)) {
			cdir = p;
//			fprintf(stdout, "pwd: '%s'\n", pwd());
			return(1);
		}
		p = p->getnext();
	}
	return(0);
};

void List::prints(Info *dir, u_char *des) {
Info	*p;

	p = cdir;
	cdir = dir;
	print(des, 1);
	cdir = p;
};

/*
 * Печать каталога от текущего.
 * sub = 1 - включая подкаталоги
 */
void List::print(u_char *des, int sub) {
Info	*p;

	if (cdir == NULL)
		cdir = rootdir;
	if (cdir == NULL) {
		fprintf(stderr, "panic: cdir = rootdir = NULL\n");
		return;
	}
//	cdir->print(des);
	fprintf(stdout, "\n%s\n", pwd());
	p = cdir->getdir();
	/*
	 * печать текущего каталога.
	 */
	while(p) {
		p->print(des);
		p = p->getnext();
	}
	/*
	 * печать в подкаталогах. Рекурсия %$#$%
	 */
	if (sub) {
		p = cdir->getdir();
		while(p) {
			if (p->isdir())
				prints(p, des);
			p = p->getnext();
		}
	}
};

char *List::pwd() {
Info	*p;
char	buf[1024];

	if (cdir == NULL)
		cdir = rootdir;
	if (cdir == NULL) {
		fprintf(stderr, "panic: cdir = rootdir = NULL\n");
		return(NULL);
	}
	p = cdir;
	strncpy(cpwd, p->getname(), 1023);
	p = cdir->getroot();
	while(p) {
		strncpy(buf, p->getname(), 1023);
		strcat(buf, "/");
		strncat(buf, cpwd, 1023);
		strncpy(cpwd, buf, 1023);
		p = p->getroot();
	}
	return(cpwd);
};
