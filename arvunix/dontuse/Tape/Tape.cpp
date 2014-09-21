/*
 * Основной класс работы с данными на ленте и видеомагнитофоном.
 */
#include "stdio.h"
#include "Tape.h"

Tape::Tape(void) {
	irc = new VCR();
//	fprintf(stderr, "Tape::Tape(void) unimplimented\n");
};

int Tape::LoadVCR(char *name) {
//	fprintf(stderr, "Tape::LoadVCR(char *name) unimplimented\n");
//	return(-1);
	return(irc->LoadVCR(name));
};

int Tape::LoadTDR(char *name) {
	fprintf(stderr, "Tape::LoadTDR(char *name) unimplimented\n");
	return(-1);
};

int Tape::LoadAVT(char *name) {
	fprintf(stderr, "Tape::LoadAVT(char *name) unimplimented\n");
	return(-1);
};

int Tape::LoadCMD(char *name) {
	fprintf(stderr, "Tape::LoadCMD(char *name) unimplimented\n");
	return(-1);
};

int Tape::SaveCMD(char *name) {
	fprintf(stderr, "Tape::SaveCMD(char *name) unimplimented\n");
	return(-1);
};

int Tape::pwd(char *dir, u_int nbyte) {
	fprintf(stderr, "Tape::pwd(char *dir, u_int nbyte) unimplimented\n");
	return(-1);
};

int Tape::cd(char *dir) {
	fprintf(stderr, "Tape::cd(char *dir) unimplimented\n");
	return(-1);
};

int Tape::get(char *src, char *dst) {
	fprintf(stderr, "Tape::get(char *src, char *dst) unimplimented\n");
	return(-1);
};

int Tape::put(char *src, char *dst) {
	fprintf(stderr, "Tape::put(char *src, char *dst) unimplimented\n");
	return(-1);
};

int Tape::unget(char *dst) {
	fprintf(stderr, "Tape::unget(char *dst) unimplimented\n");
	return(-1);
};

int Tape::unput(char *dst) {
	fprintf(stderr, "Tape::unput(char *dst) unimplimented\n");
	return(-1);
};

int Tape::run(void) {
	fprintf(stderr, "Tape::run(void) unimplimented\n");
	return(-1);
};

int Tape::run(FILE *fdout) {
	fprintf(stderr, "Tape::run(FILE *fdout) unimplimented\n");
	return(-1);
};

int Tape::run(int fdout) {
	fprintf(stderr, "Tape::run(int fdout) unimplimented\n");
	return(-1);
};

int Tape::Abort(void) {
	fprintf(stderr, "Tape::Abort(void) unimplimented\n");
	return(-1);
};

Tape::~Tape() {
	delete irc;
//	fprintf(stderr, "Tape::~Tape() unimplimented\n");
};

