#include "stdio.h"
#include "stdlib.h"
#include "fcntl.h"
#include "unistd.h"
#include "string.h"
#include "sys/stat.h"
#include "avt.h"
#include "Info.h"

int tree_view(int fdin, u_int rootdir);
void print_head(AVT_HEAD head);
void print_iphystape(AVT_IPHYSTAPE *iphystape);
int print_elem(int fdin, AVT_ELEM elem);
int print_str(int fdin, u_int offs);
int get_str(int fdin, char *name, u_int offs, u_int noff);

main(int argc, char *argv[]) {
int		fdin;		// file desk
struct stat	fdstat;		// stat file
AVT_HEAD	head;		// head of AVT file
AVT_IPHYSTAPE	*iphystape;	// list iphystape

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <file.avt>\n", argv[0]);
		return(0);
	}

	// пробуем открыть
	fdin = open(argv[1], O_RDONLY, 0);
	if (fdin < 0) {
		fprintf(stderr, "Can't open file '%s'\n", argv[1]);
		return(0);
	}

	// статистика по файлу для проверок
	if (fstat(fdin, &fdstat) < 0) {
		fprintf(stderr, "Can't stat file '%s'\n", argv[1]);
		close(fdin);
		return(0);
	}

	// загрузим заголовок
	if (read(fdin, &head, sizeof(AVT_HEAD)) != 40) {
		fprintf(stderr, "Error read HEAD from file '%s'.\n", argv[1]);
		close(fdin);
		return(0);
	}
	print_head(head);
	// проверим версию
	if (head.avtformat != 1) {
		fprintf(stderr, "Format '%d' not supported.\n", head.avtformat);
		close(fdin);
		return(0);
	}

	iphystape = new AVT_IPHYSTAPE;
	if ((lseek(fdin, head.iphystape, SEEK_SET) != head.iphystape) ||
		(read(fdin, iphystape, sizeof(AVT_IPHYSTAPE)) != 40)) {
		fprintf(stderr, "Error read from file '%s'!\n", argv[1]);
		delete iphystape;
		close(fdin);
		return(0);
	}
	print_iphystape(iphystape);

	if (tree_view(fdin, head.rootdir)) {
		fprintf(stderr, "Error read from file '%s'!\n", argv[1]);
		delete iphystape;
		close(fdin);
	}

	delete iphystape;
	close(fdin);
};

int tree_view(int fdin, u_int rootdir) {
AVT_ELEM	elem;		// one element

	if ((lseek(fdin, rootdir, SEEK_SET) != rootdir) ||
		(read(fdin, &elem, sizeof(AVT_ELEM)) != 40)) {
		return(1);
	}
	print_elem(fdin, elem);
	if (elem.bits.fdir && elem.tree)
		tree_view(fdin, elem.tree);
	if (elem.left)
		tree_view(fdin, elem.left);
	if (elem.right)
		tree_view(fdin, elem.right);

	return(0);
};

void print_head(AVT_HEAD head) {
	fprintf(stdout, "\t*** Head ***\n");
	fprintf(stdout, "Signature:\t%c%c%c%c\n",
		head.sig[0], head.sig[1], head.sig[2], head.sig[3]);
	fprintf(stdout, "Format file:\t%u\n", head.avtformat);
	fprintf(stdout, "Check summ:\t0x%08X\n", head.checksum);
	fprintf(stdout, "IEmpty:\t\t0x%08X\n", head.IEmpty);
	fprintf(stdout, "hFree:\t\t0x%08X\n", head.hFree);
	fprintf(stdout, "rootdir:\t0x%08X\n", head.rootdir);
	fprintf(stdout, "newsect:\t%u\n", head.newsect);
	fprintf(stdout, "lrnewsect:\t%u\n", head.lrnewsect);
	fprintf(stdout, "iphystape:\t0x%08X\n", head.iphystape);
	fprintf(stdout, "reserv:\t\t0x%08X\n", head.reserv);
	fprintf(stdout, "\n");
};

void print_iphystape(AVT_IPHYSTAPE *iphystape) {
	fprintf(stdout, "\t*** iphystape ***\n");
	fprintf(stdout, "Next:\t\t0x%08X\n", iphystape->next);
	fprintf(stdout, "Format:\t\t0x%02X\n", iphystape->tpb.format);
	fprintf(stdout, "Length:\t\t%um/%uh\n",
		iphystape->tpb.length, iphystape->tpb.length/60);
	if (iphystape->tpb.format & 0x0f) {
		fprintf(stdout, "Name:\n");
	} else
		fprintf(stdout, "Ident:\t\t0x%08X\n", iphystape->tpb.tapeID);
	fprintf(stdout, "startsect:\t%u\n", iphystape->startsect);
	fprintf(stdout, "numsect:\t%u\n", iphystape->numsect);
	fprintf(stdout, "PToffset:\t0x%08X\n", iphystape->PToffset);
	fprintf(stdout, "PTsize:\t\t%u\n", iphystape->PTsize);
	fprintf(stdout, "\n");
};

int print_elem(int fdin, AVT_ELEM elem) {
int	i, rc;
u_short	dd, dt;
char	*p, *name, *ext, *desc;
my_info		*info;
//my_file	*f;
//my_dir	*d;

	rc = 0;
	name = new char[1024];
	bzero(name, 1024);
	ext = new char[1024];
	bzero(ext, 1024);
	desc = new char[1024];
	bzero(desc, 1024);
//	fprintf(stdout, "\t*** element ***\n");
//	fprintf(stdout, "Left:\t\t0x%08X\n", elem.left);
//	fprintf(stdout, "Right:\t\t0x%08X\n", elem.right);
	if (elem.bits.fdir)
		fprintf(stdout, "\t0x%08X", elem.tree);
	else
		fprintf(stdout, "\t%10u", elem.size);
	dd = elem.datime >> 16;
	dt = elem.datime;
	fprintf(stdout, "  %02d/%02d/%d",
		dd & 0x1f,
		(dd >> 5) & 0x0f,
		(dd >> 9) + 1980);
	fprintf(stdout, " %02d:%02d:%02d",
		dt >> 11,
		(dt >> 5) & 0x3f,
		(dt & 0x1f) << 1);
	fprintf(stdout, "  %10u", elem.startsect);
//	fprintf(stdout, "nlogsect:\t%u\n", elem.bits.nlogsect);
//	fprintf(stdout, "bal:\t\t%u\n", elem.bits.bal);
//	fprintf(stdout, "format:\t\t%u\n", elem.bits.format);
//	fprintf(stdout, "fdir:\t\t%u\n", elem.bits.fdir);
	fprintf(stdout, " ");
	switch(elem.bits.format) {
	case	0:
		p = elem.data1c;
		for (i = 0; i < 16 && *p; i++, p++)
			name[i] = *p;
		break;
	case	1:
		p = elem.data1c;
		for (i = 0; i < 12 && *p; i++, p++)
			name[i] = *p;
		if (elem.data4)
			rc = get_str(fdin, ext, elem.data4, 0);
		break;
	case	2:
		p = elem.data1c;
		for (i = 0; i < 12 && *p; i++, p++)
			name[i] = *p;
		if (elem.data4)
			rc = get_str(fdin, desc, elem.data4, 0);
		break;
	case	3:
		if (elem.data1)
			rc = get_str(fdin, name, elem.data1, 0);
		if (elem.data3 && rc == 0)
			rc = get_str(fdin, ext, elem.data3, 0);
		if (elem.data4 && rc == 0)
			rc = get_str(fdin, desc, elem.data4, 0);
		break;
	}
#if 0
	if (*name)
		fprintf(stdout, "%s", name);
	if (elem.bits.fdir)
		fprintf(stdout, "/");
	if (*ext)
		fprintf(stdout, " ext:\t\t%s", ext);
	if (*desc)
		fprintf(stdout, " description:\t%s", desc);
	fprintf(stdout, "\n");
#else
	info = new my_info(name, 0, desc);
	fprintf(stdout, "%s", info->Name());
	if (elem.bits.fdir)
		fprintf(stdout, "/");
//	fprintf(stdout, " ext:\t\t%s", ext);
	if (*info->Desc())
		fprintf(stdout, " description:\t%s", info->Desc());
	fprintf(stdout, "\n");
	delete info;
#endif
	if (name)
		delete name;
	if (ext)
		delete ext;
	if (desc)
		delete desc;
	return(rc);
};

int print_str(int fdin, u_int offs) {
AVT_STR	str;
int		i, rc;

	rc = 0;
	if ((lseek(fdin, offs, SEEK_SET) != offs) ||
		(read(fdin, &str, sizeof(AVT_STR)) != 40)) {
		return(1);
	}
	for (i = 0; i < 36 && str.data[i]; i++)
		fprintf(stdout, "%c", str.data[i]);
	if (str.next)
		rc = print_str(fdin, str.next);
	return(rc);
};

int get_str(int fdin, char *name, u_int offs, u_int noff) {
AVT_STR	str;
int	i, rc;

	rc = 0;
	if ((lseek(fdin, offs, SEEK_SET) != offs) ||
		(read(fdin, &str, sizeof(AVT_STR)) != 40)) {
		return(1);
	}
	for (i = 0; i < 36 && str.data[i] && noff < 1024; i++)
		name[noff++] = str.data[i];
	if (str.next && noff < 1024)
		rc = get_str(fdin, name, str.next, noff);
	else if (noff > 1024)
	/* nothingh */
		noff = 1024;
	return(rc);
};
