/*
 * Основной класс работы с данными на ленте и видеомагнитофоном.
 */
#include "Irc.h"	// управление видеомагнитофоном

class Tape {
	VCR	*irc;			// управление VM
public:
	Tape(void);
int	LoadVCR(char *name);
int	LoadTDR(char *name);
int	LoadAVT(char *name);
int	LoadCMD(char *name);
int	SaveCMD(char *name);
int	pwd(char *dir, u_int nbyte);
int	cd(char *dir);
int	get(char *src, char *dst);
int	put(char *src, char *dst);
int	unget(char *dst);
int	unput(char *dst);

int	run(void);
int	run(FILE *fdout);
int	run(int fdout);

int	Abort(void);

	~Tape();
};
