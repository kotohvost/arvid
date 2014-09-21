/*
 */
#include "EntryFile.H"

class ListToRead {
	EntryFile	*beg, *end, *cur;
public:
	ListToRead();
	ListToRead(char *, u_long, u_long, u_char cv = 5);
	~ListToRead();
	/* Add entry to list, check sector region */
	int AddList(char *, u_long, u_long, u_char cv = 5);
	/* Set cur = beg; if beg != NULL return(0) */
	int SetBeginFile();
	/* Set cur = cur->next; if cur != NULL return(0) */
	int SetNextReadFile();

	/* Checking sector in region list */
	int CheckSectorList(u_long);
	/* Checking sector in current region file */
	int CheckSectorCFile(u_long);

	/* Get Name current file */
	char *GetCName();
	/* Get StartSector current file */
	u_long GetCSectorS();

	/* print all list */
	void Ls();
};

/* Constructor */
ListToRead::ListToRead() {
	beg = end = cur = NULL;
}
ListToRead::ListToRead(char *name, u_long sect, u_long size, u_char cv) {
	beg = end = cur = new EntryFile(name, sect, size, cv);
}

ListToRead::~ListToRead() {
	if (beg) {
		for(cur = beg; cur != NULL; cur = beg) {
			beg = cur->next;
			delete(cur);
		}
	}
}

int ListToRead::AddList(char *name, u_long sect, u_long size, u_char cv) {
EntryFile	*tc;
int		cs;

	if (beg == NULL)
		beg = end = cur = new EntryFile(name, sect, size, cv);
	else {
	/* сортируем по секторам */
		int	up, down;

		if (cur == NULL)
			cur = beg;
		up = down = 0;
		while (!up || !down) {
			cs = cur->CheckSector(sect);
	//	printf("%s\tsect:%u\t%s\tcs=%d, ",
	//		name, sect, cur->GetName(), cs);
			if (cs == 1) {
			/* спускаемся вниз */
				if (cur->next == NULL || up)
					up = 1;
				else
					cur = cur->next;
				down = 1;
			} else if (cs == -1) {
			/* поднимаемся вверх */
				if (cur->prev == NULL || down)
					down = 1;
				else
					cur = cur->prev;
				up = 1;
			} else
			/* приехали */
				up = down = 1;
	//	printf("up=%d, down=%d\n", up, down);
		} /* while */

		if (cs == 0) {
		/* попадание в сектора - ошибка */
			printf("Error: Sector '%u' found region in '%s'.\n",
				sect, cur->GetName());
		} else {
		/* вставляем */
			tc = new EntryFile(name, sect, size, cv);
			if (cs == 1) {
			/* вставляем после cur */
				tc->prev = cur;
				tc->next = cur->next;
				if (cur->next == NULL)
				/* попали в конец */
					end = tc;
				else
					cur->next->prev = tc;
				cur->next = tc;
			} else {
			/* вставляем перед cur */
				tc->next = cur;
				tc->prev = cur->prev;
				if (cur->prev == NULL)
					beg = tc;
				else
					cur->prev->next = tc;
				cur->prev = tc;
			}
			cur = tc;
		} /* cs == 0 */
	} /* beg == NULL */
}

int ListToRead::SetBeginFile() {
int	rc;

	rc = 0;
	if (beg == NULL)
		rc = 1;
	else
		cur = beg;
	return(rc);
}

int ListToRead::SetNextReadFile() {
int	rc;

	rc = 0;
	if (cur == NULL)
		rc = 1;
	else if (cur->next == NULL)
		rc = 1;
	else
		cur = cur->next;
	return(rc);
}

void ListToRead::Ls() {
EntryFile	*cl;

	for(cl = beg; cl != NULL; cl = cl->next)
		cl->Ls();
}

/*
 *
 */
int ListToRead::CheckSectorList(u_long sect) {
	if (beg != NULL) {
		if (beg->CheckSector(sect) == -1)
			return(-1);
		if (end->CheckSector(sect) == 1)
			return(1);
	}
	return(0);
}

/*
 *
 */
int ListToRead::CheckSectorCFile(u_long sect) {
	if (cur != NULL)
		return(cur->CheckSector(sect));
	return(1);
}

/*
 *
 */
char *ListToRead::GetCName() {
	if (cur != NULL)
		return(cur->GetName());
	return(NULL);
}

u_long ListToRead::GetCSectorS() {
	if (cur != NULL)
		return(cur->GetSector());
	return(0);
}
