/*
 * Используется в ListToRead.H
 */
#define	DEFAULT_VEREFY	5

class EntryFile {
	char	*Name;	/* full name */
	u_long	Sector;	/* start sector in tape */
	u_long 	Size;	/* in byte */
	u_char	*ver;	/* verefy count */
public:
	EntryFile	*next, *prev;

	EntryFile(char *, u_long, u_long, u_char cv = DEFAULT_VEREFY);
	~EntryFile();
	void Ls();
	int CheckSector(u_long);
	char *GetName() { return Name; };
	u_long GetSector() { return Sector; };
	u_long GetSize() { return Size; };
};

/* Constructor */
EntryFile::EntryFile(char *name, u_long sector, u_long size, u_char cv) {
	Name = (char *)malloc(strlen(name)+1);
	strcpy(Name, name);
	Sector = sector;
	Size = size;
	ver = (u_char *)malloc(size / 512 + 1);
	memset(ver, cv, size / 512 + 1);
	next = prev = NULL;
}
EntryFile::~EntryFile() {
	free(Name);
	free(ver);
}

void EntryFile::Ls() {
	printf("%s\t%d\t%d (%d)\n",
		Name, Sector, Size, Size / 512 + 1);
}

int EntryFile::CheckSector(u_long sect) {
//	if (sect >= Sector && sect <= Sector + Size / 512 + 1)
//		return(0);
	if (sect < Sector)
		return(-1);
	if (sect > Sector + Size / 512 + 1)
		return(1);
	return(0);
}
