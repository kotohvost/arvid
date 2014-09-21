#include "stdio.h"
#include "fcntl.h"
#include "Tape.h"
#include "mystring.h"

main(int argc, char *argv[]) {
Tape	*mt;
char	*name, *dir, *src, *dst;
FILE	*fout;
int	fdout;
u_int	nbyte;
mystring	my;

//	my = new mystring();
	my = "ddd";
	my = "BBBB";
//	delete my;
//	my = new mystring("AAAA");
//	delete my;
	mt = new Tape();
	name = "Jvc_d520.vcr";
	mt->LoadVCR(name);

	name = "XXX.tdr";
	mt->LoadTDR(name);

	name = "XXX.avt";
	mt->LoadAVT(name);

	mt->pwd(dir, nbyte);
	mt->cd(dir);
	mt->get(src, dst);
	mt->put(src, dst);
	mt->unget(dst);
	mt->unput(dst);

	name = "XXX.cmd";
	mt->LoadCMD(name);
	mt->SaveCMD(name);
	mt->run();
	mt->run(fout);
	mt->run(fdout);
	mt->Abort();

	delete mt;
};
