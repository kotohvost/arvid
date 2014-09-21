#Makefile for DJGPP by Levtchenko V.
#####################################################################
#-O4...-O6 bugs
HOW_OPTIMIZE=-s -Wall -O3 -fno-exceptions -fno-rtti -mpentium
#####################################################################

PROJECT = tavd

O=o
.SUFFIXES:
.SUFFIXES: .exe .$(O) .cpp .h

CXX=gcc
LINK=gcc
LFLAGS=-s -o
CFLAGS= -c $(HOW_OPTIMIZE) -I. -o

project: $(PROJECT).exe
gtest:	gal_test.exe
itest:	ini_test.exe
mtest:	machtest.exe
all:	$(PROJECT).exe gal_test.exe ini_test.exe machtest.exe

.PHONY: rar clean dep
rar:
	@echo ------------------------------
	@echo Backing up to archive tavd.rar
	@echo ------------------------------
	-@rar a -rr -m4 -md1024 -av- -u -cl -r -isnd -ztavd.diz -x*.map -x*.exe -x*.obj -x*.err -x*.o ..\$(PROJECT).rar ..\$(PROJECT)

clean:
	-@if exist *.$(O) del *.$(O)
	-@if exist *.exe del *.exe

ALL_MODULES = aftp.cpp console.cpp crc32.cpp csvector.cpp dpmiserv.cpp \
	      gal_test.cpp galey.cpp ini_test.cpp inifile.cpp machtest.cpp \
	      manchstr.cpp tavd.cpp djvidsys.cpp

dep:
	makedep -o.$$(O) -c -I. -I/djgpp/include -f$(PROJECT).dep $(ALL_MODULES)

#@includes -e -l -so $(ALL_MODULES) >tavd.dep
%.$(O): %.cpp
	 gcc $(CFLAGS) $@ $<
%.exe: %.$(O)
	 gcc $(LFLAGS) $@ $^

-include $(PROJECT).dep

$(PROJECT).exe: $(PROJECT).$(O) galey.$(O) djvidsys.$(O) dpmiserv.$(O) \
	 aftp.$(O) crc32.$(O) inifile.$(O) csvector.$(O) console.$(O) \
	 manchstr.$(O)
gal_test.exe: gal_test.$(O) galey.$(O) dpmiserv.$(O)
ini_test.exe: ini_test.$(O) inifile.$(O) csvector.$(O)
machtest.exe: machtest.$(O) manchstr.$(O) dpmiserv.$(O)
