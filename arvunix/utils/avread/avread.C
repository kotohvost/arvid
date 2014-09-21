/*
 * чтение файлов с арвида согласно списку
 */
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "string.h"
#include "signal.h"
#include "sys/ioctl.h"
#include "../../include/avtreg.h"
#include "../../include/Irc.hpp"
#include "../../include/ListToRead.H"
#include "../../include/dcheader.H"

char	*CfgDev = "/dev/ravt0";
char	*CfgVcr = "/usr/local/lib/arvid/video.vcr";
int	CfgTimeOut = 13;

ListToRead	*RL;	/* ReadList - глобальный список читаемых файлов */
int		arvid;	/* открытый девайс и его установки */
char		cPhase;
struct set_rw	SetRW;
VCR		Vcr;	/* VCR - класс управления видеомагнитофоном */
char		code[8192];	/* данные из карты */
char		data[8192];	/* декодированные данные */
struct	DcStill	Still;		/* структура для чтения */
int		timeout;	/* время ожидания сектора */

int LoadRL(char *name) {
FILE	*fdin;
char	buf[1024], tstr[1024];
u_long	sect, size;

	fdin = fopen(name, "r");
	if (fdin == NULL) {
		printf("Error open file '%s'!!!\n", name);
		return(1);
	}
	if (RL == NULL)
		RL = new ListToRead();

	while(fgets(tstr, sizeof(tstr), fdin) != NULL) {
		if (sscanf(tstr, "%s %u %u", &buf, &sect, &size) == 3)
			RL->AddList(buf, sect, size);
	} /* while */
	fclose(fdin);
	return(0);
};

void AlarmIdent(int sig) {
	timeout = 0;
};

char GetPhase() {
char	p;

	ioctl(arvid, VGET_PHASE, &p);
	return(p);
};

int SetPhase(char p) {
	return(ioctl(arvid, VSET_PHASE, &p));
};

int Ident(void) {
int	fHead;
int	ph;

	fHead = 1;
	ph = 0;
	timeout = CfgTimeOut;
	signal(SIGALRM, AlarmIdent);
	alarm(timeout);
	while(timeout != 0 && fHead != 0) {
		if (ph++ > 10) {
			SetPhase(GetPhase());
			ph = 0;
		}
		/* читаем кадр */
		bzero(Still.code, Still.csize);
		while(read(arvid, Still.code, Still.csize) == 0);
		/* декодируем загаловок */
		bzero(Still.data, Still.dsize);
		/* FORMAT_RSC_50 */
		fHead = DcHeader(Still.code, &Still.header, &Still.coffs, FORMAT_RSC_50);
		if (fHead) {
		/* FORMAT_RSC || FORMAT_RS */
			fHead = DcHeader(Still.code, &Still.header, &Still.coffs, FORMAT_RS);
		}
		if (fHead) {
		/* FORMAT_G || FORMAT_K7 || FORMAT_K7CRC */
		}
	} /* while */
	alarm(0);
	return(fHead);
};

int main(int argc, char *argv[]) {
int		rc, sector;

	if (argc != 2) {
		printf("Usage: %s <file>\n", argv[0]);
		return(0);
	}

	Still.code = code;
	Still.csize = sizeof(code);
	Still.data = data;
	Still.dsize = sizeof(data);
	/*
	 * Загрузим список
	 */
	if (LoadRL(argv[1])) {
		printf("Error: LoadRL(%s)\n", argv[1]);
		return(1);
	}
	/*
	 * Проверим на пустой список
	 */
	if (RL->SetBeginFile()) {
		printf("Error: List empty\n");
		return(2);
	}
	/*
	 * Загрузим фаил управления видеомагнитофоном
	 */
	if (Vcr.LoadVCR(CfgVcr)) {
		printf("Error load '%s'\n", CfgVcr);
		return(3);
	}
	/*
	 * Откроем девайс
	 */
	arvid = open(CfgDev, O_RDWR, 0);
	if (arvid < 0) {
		printf("Error open '%s'\n", CfgDev);
		return(4);
	}
	/*
	 * Индентифицируем ленту
	 */
	/* Play */
	SetRW.rate = 200;
	SetRW.mode = IOIRC;
	ioctl(arvid, VSET_WRITE, &SetRW);
	Vcr.IrcKeyTrn(arvid, 1);

	/* Read */
	SetRW.rate = 200;
	SetRW.mode = IORAW;
	ioctl(arvid, VSET_READ, &SetRW);

	/* Ident */
	rc = Ident();

	/* ? */
	/* Stop */
	SetRW.rate = 200;
	SetRW.mode = IOIRC;
	ioctl(arvid, VSET_WRITE, &SetRW);
	Vcr.IrcKeyTrn(arvid, 0);

	if (rc) {
		printf("Error Indent tape\n");
		close(arvid);
		return(5);
	}

//	RL->Ls();
	sector = getSector(&Still.header);
	rc = RL->CheckSectorList(sector);
	if (rc == 0) {
		rc = RL->SetBeginFile();
		while (rc == 0) {
			rc = RL->CheckSectorCFile(sector);
			if (rc == 0) {
				printf("%u", RL->GetCSectorS());
/*				printf("\t%u", RL->GetCSize());
*/				printf("\t%s\n", RL->GetCName());
				rc = 1;
			} else
				rc = RL->SetNextReadFile();
		}
	} else
		printf("RangList = %d (%u)\n", rc, sector);
	printf("Format: %02X\n", Still.header.g.format);
	if (Still.header.g.format <= 0x08) {
		int	i;
		printf("Name: ");
		for(i=0; i<8; i++)
			printf("%c", Still.header.g.name[i]);
		printf("\n");
	}

	close(arvid);
	return(rc);
}
