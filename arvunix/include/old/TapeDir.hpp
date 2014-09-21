/*
 * ������� �����
 * +-----------+-----------+---------+---------------------------------
 * | ��������� |   ����    | ������� | ��������
 * +-----------+-----------+---------+---------------------------------
 * |  void   4 | *next     | *next   | ��������� �� ������
 * |  void   4 | sect      | *nextd  | ��������� �� ������ ���������
 * |  void   4 | size      | *nextf  | ��������� �� ������ ������
 * |  u_int  4 | Id        | Id      | ����� ����� � ������ ����
 * |  u_int  4 | Desc      | Desc    | ����� �������� � ������ ��������
 * |  u_long 4 | Attr      | Attr    | Hyd/Sys/Rd/Arc
 * |  u_long 4 | Dtime     | Dtime   | ����� � ���� ��������
 * +-----------+-----------+---------+---------------------------------
 *
 * � ��������� *next ������ ��������� �� ������� ��� NULL
 * � ������ *next ������ ��������� �� ���� ��� NULL
 */
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"
/* �������� ������������ ������ */
#include "errno.h"

/*
 * ��������� ��������/����� ��� ����������/��������
 */
struct	DirFile_t {
	char	*Name;
	char	*Desc;
	u_int	Attr;
	u_long	Dtime;
	u_long	Sect;
	size_t	Size;
};

/*
 * ��������� ������ �����
 */
struct	TapeDir_t {
	TapeDir_t	*next;
	union	{
		TapeDir_t	*nextd;
		u_int		sect;
		};
	union	{
		TapeDir_t	*nextf;
		u_int		size;
		};
	u_long		Id;
	u_int		Desc;
	u_int		Attr;
	u_long		Dtime;
	TapeDir_t	*root;		// ��������� �� ������ ������
};

class	TapeDir {
 public:
	TapeDir_t	*root;	// ��������� �� ������
	TapeDir_t	*curd;	// ��������� �� ������� ����������
	TapeDir_t	*curf;	// ��������� �� ������� ����
			/* ����� */
	char	**name;		// ���������
	u_long	nameId;		// ����� ���������� +1
	u_long	namesize;	// ������ �������
			/* ��������� */
	char	**desc;		// ���������
	u_long	descId;		// ����� ���������� +1
	u_long	descsize;	// ������ �������
 public:
 int	InsD(DirFile_t *dir);	// �������� �������
 int	InsF(DirFile_t *file);	// �������� ���� � ������� �������
 int	Cd(char *str);		// ������� � ����������
 int	Dir();			// ������ ��������
	TapeDir();
	~TapeDir();
 private:
 TapeDir_t	*FindDir(char *dir);	// ����� ���������
 TapeDir_t	*FindFile(char *file);	// ����� ���������
 u_long	AddName(char *fname);	// �������� ��� � ������
 u_long	AddDesc(char *fdesc);	// �������� ��������
 void	FreeFile(TapeDir_t *f);	// �������� ������ ������
 void	FreeDir(TapeDir_t *d);	// �������� ������ ���������
};

// constructor
TapeDir::TapeDir() {
	name = desc = NULL;
	namesize = descsize = 0;
	nameId = descId = 0;
	curd = curf = NULL;

	/* ����������� root */
	root = (TapeDir_t *)malloc(sizeof(TapeDir_t));
	root->next = root->nextd = root->nextf = NULL;
	root->root = NULL;
	root->Attr = 0;
	root->Dtime = 0;
	root->Id = AddName("/");
	root->Desc = AddDesc("Default description");
};

// ���������� ������ �����
void TapeDir::FreeFile(TapeDir_t *ff) {
	if (ff->next)
		FreeFile(ff->next);
	free(ff);
};

// ���������� ������ ��������
// ���� �� ������ �� ������������ ����� ��������� FreeFile � ������ ���� :)
void TapeDir::FreeDir(TapeDir_t *fd) {
	// ����������� �������� �������
	if (fd->next)
		// ����������� �����
		FreeDir(fd->next);
	// ����������� �����������
	if (fd->nextd)
		FreeDir(fd->nextd);
	// ����������� ����� ������
	if (fd->nextf)
		FreeFile(fd->nextf);
	// ��������� ������
	free(fd);
};

// ������
TapeDir::~TapeDir() {
	if (root)
		FreeDir(root);
	if (name)
		free(name);
	if (desc)
		free(desc);
};

/*
 * ����� ��������� �������� � ������� ��������
 * curd = NULL - ���� � root
 * 0 - ����
 */
TapeDir_t *TapeDir::FindDir(char *fname) {
TapeDir_t	*pdir;

	if (curd == NULL)
		curd = root;

	if (strncmp(fname, ".\0", 2) == 0)
		return(curd);
	if (strncmp(fname, "..\0", 3) == 0)
		if (curd->root == NULL)
			return(root);
		else
			return(curd->root);

	pdir = curd->nextd;
	while (pdir) {
//	printf("Find: %lu %s\n", pdir, name[pdir->Id]);
		if (strcmp(name[pdir->Id], fname) == 0)
			return(pdir);
		pdir = pdir->next;
		};
	return(NULL);
};

/*
 * ����� ��������� ����� � ������� ��������
 * curd = NULL - ���� � root
 * 0 - ����
 */
TapeDir_t *TapeDir::FindFile(char *fname) {
TapeDir_t	*pfile;

	if (curd == NULL)
		curd = root;
//		pfile = root->nextf;
//	else
	pfile = curd->nextf;

	while (pfile) {
		if (strcmp(name[pfile->Id], fname) == 0)
			return(pfile);
		pfile = pfile->next;
		};
	return(NULL);
};

/*
 * ���������� ����� � ������ ID
 */
u_long TapeDir::AddName(char *fname) {
int	len;
u_long	sizem;

//	for(len = 0; len < nameId; len++)
//		printf("\nAddName: %s", name[len]);

	len = strlen(fname) + 1;
	sizem = (nameId + 1) * sizeof(char *);
	if (name == NULL) {
	// ���������� ������
		name = (char **)malloc(sizem);
	//	name[nameId] = (char *)malloc(len);
		}
	else	{
	// ������� ������
		name = (char **)realloc(name, sizem);
	//	name[0] = (char *)realloc(name[0], namesize + len);
	//	name[nameId] = name[0] + namesize;
		};
	name[nameId] = (char *)malloc(len);
	// ����� ������
	namesize += len;
	strncpy(name[nameId], fname, len);

//	for(len = 0; len <= nameId; len++)
//		printf("\nAddName: %s", name[len]);
//	printf("\n");

	return(nameId++);
};

/*
 * ���������� �������� � ������ Id
 */
u_long TapeDir::AddDesc(char *fdesc) {
int	len;
u_long	sizem;

	if (fdesc == NULL)
		return(0);
	if (*fdesc == '\0')
		return(0);

//	for(len = 0; len < descId; len++)
//		printf("\nAddDesc: %s", desc[len]);

	len = strlen(fdesc) + 1;
	sizem = (descId + 1) * sizeof(char *);
	if (desc == NULL) {
	// ���������� ������
		desc = (char **)malloc(sizem);
	//	desc[descId] = (char *)malloc(len);
		}
	else	{
	// ������� ������
		desc = (char **)realloc(desc, sizem);
	//	desc[0] = (char *)realloc(desc[0], descsize + len);
	//	desc[descId] = desc[0] + descsize;
		};
	desc[descId] = (char *)malloc(len);
	// ����� ������
	descsize += len;
	strncpy(desc[descId], fdesc, len);

//	for(len = 0; len <= descId; len++)
//		printf("\nAddDesc: %s", desc[len]);
//	printf("\n");

	return(descId++);
};

/*
 * ���������� �������� � ������� �������
 * curd - ������� ������� ��� '/'
 * 0 - OK
 */
int TapeDir::InsD(DirFile_t *dir) {
TapeDir_t	*pdir, *pcur;

	if (dir == NULL)
		return(EFAULT);
	if (dir->Name == NULL)
		return(EINVAL);
	if (*dir->Name == '\0')
		return(ENOTDIR);
	// ������ ���������
	if (FindDir(dir->Name)) {
		printf("Directory '%s' exist\n", dir->Name);
		return(EEXIST);
		};
	if (FindFile(dir->Name)) {
		printf("Directory '%s' is file\n", dir->Name);
		return(EEXIST);
		};

	// pdir - ��������� �� ����������� �������
	pdir = (TapeDir_t *)malloc(sizeof(TapeDir_t));
	pdir->next = pdir->nextd = pdir->nextf = NULL;
	pdir->Attr = dir->Attr;
	pdir->Dtime = dir->Dtime;
	pdir->Id = AddName(dir->Name);
 printf("Id: %lu", pdir->Id);
 printf("\tName: %s\t", name[pdir->Id]);
	pdir->Desc = AddDesc(dir->Desc);
 printf("Id: %lu", pdir->Desc);
 printf("\tDesc: %s\n", desc[pdir->Desc]);

	// ������ ��������� � ������� �������
	if (curd == NULL) {
		curd = root;
	//	while (pcur->next)
	//		pcur = pcur->next;
	//	pcur->next = pdir;
	//	pdir->root = pcur->root;
		}
//	else 
	if (curd->nextd == NULL) {
		curd->nextd = pdir;
		pdir->root = curd;
		}
	else	{
		pcur = curd->nextd;
		while (pcur->next)
			pcur = pcur->next;
		pcur->next = pdir;
		pdir->root = pcur->root;
		};
	return (0);
};

/*
 * ���������� ����� � ������� �������
 * 0 - OK
 */
int TapeDir::InsF(DirFile_t *file) {
TapeDir_t	*pfile, *pcur;

	if (file == NULL)
		return(EFAULT);
	if (file->Name == NULL)
		return(EINVAL);
	if (*file->Name == '\0')
		return(ENOENT);
	// ������ ���������
	if (FindDir(file->Name) != NULL) {
		printf("File '%s' is directory\n", file->Name);
		return(EEXIST);
		};
	if (FindFile(file->Name) != NULL) {
		printf("File '%s' exist\n", file->Name);
		return(EEXIST);
		};

	// pfile - ��������� �� ����������� ����
	pfile = (TapeDir_t *)malloc(sizeof(TapeDir_t));
	pfile->next = NULL;
	pfile->sect = file->Sect;
	pfile->size = file->Size;
	pfile->Attr = file->Attr;
	pfile->Dtime = file->Dtime;
	pfile->Id = AddName(file->Name);
 printf("Id: %lu", pfile->Id);
 printf("\tName: %s\t", name[pfile->Id]);
	pfile->Desc = AddDesc(file->Desc);
 printf("Id: %lu", pfile->Desc);
 printf("\tDesc: %s\n", desc[pfile->Desc]);

	// ������ ��������� � ������� �������
	if (curd == NULL) {
		curd = root;
	//	if (root->nextf == NULL) {
	//		root->nextf = pfile;
	//		pfile->root = root;
	//		}
	//	else	{
	//		pcur = root->nextf;
	//		while (pcur->next)
	//			pcur = pcur->next;
	//		pcur->next = pfile;
	//		pfile->root = pcur->root;
	//		};
		}
//	else
	if (curd->nextf == NULL) {
		curd->nextf = pfile;
		pfile->root = curd;
		}
	else	{
		pcur = curd->nextf;
		while (pcur->next)
			pcur = pcur->next;
		pcur->next = pfile;
		pfile->root = pcur->root;
		};
	return (0);
};

/*
 * ����� ������� ����������
 * 0 - OK
 */
int TapeDir::Cd(char *str) {
TapeDir_t	*pdir;
char	*pstr, *nstr, *fstr;
int	len;

	// ��������
	pdir = curd;
	// ��������� ������ �������
	pstr = str;
	while (isblank(*pstr)) pstr++;
	if ((*pstr == '\0') || (*pstr == '\n'))
		return(0);
	if (*pstr == '/') {
		curd = root;
		pstr++;
		};
	fstr = (char *)malloc(1024);
	// � ����� ��������� � �������
	while ((*pstr != '\0') && (*pstr != '\n')) {
		if (nstr=strchr(pstr, '/')) {
			len = nstr - pstr;
			nstr++;
			}
		else	{
			len = strlen(pstr);
			nstr = pstr + len;
			};
		if (len >= 1024) {
			free(fstr);
			return(1);
			};
		strncpy(fstr, pstr, len);
		fstr[len] = '\0';
		if ((curd=FindDir(fstr)) == NULL) {
			printf("Directory '%s' not exist\n", fstr);
			curd = pdir;
			free(fstr);
			return(ENOENT);
			};
	//	printf("Cur dir: %s\n", name[curd->Id]);
		pstr = nstr;
		};
	free(fstr);
	return(0);
};

int TapeDir::Dir() {
TapeDir_t	*pdir, *pfile;
int		ndir, nfile;

	if (curd == NULL)
		curd = root;
	printf("%s\n", name[curd->Id]);
	nfile = ndir = 0;
	for (pdir=curd->nextd; pdir; pdir=pdir->next) {
		printf("dr--r--r-- %s\n", name[pdir->Id]);
		ndir++;
		};
	for (pfile=curd->nextf; pfile; pfile=pfile->next) {
		printf("-r--r--r-- %s\n", name[pfile->Id]);
		nfile++;
		};
	printf("All: %d, Dir: %d, File: %d\n",
		ndir+nfile, ndir, nfile);
};
