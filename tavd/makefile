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
LFLAGS= -s -o
CFLAGS= -c $(HOW_OPTIMIZE) -I. -o

.PHONY: zip rar clean dep irc it

it: it.exe
irc: irc_test.exe
project: $(PROJECT).exe adat.exe
gtest:	gal_test.exe
itest:	ini_test.exe
mtest:	machtest.exe
dat:	adat.exe
all:	$(PROJECT).exe gal_test.exe ini_test.exe machtest.exe adat.exe

rar:
	@echo ------------------------------
	@echo Backing up to archive tavd.rar
	@echo ------------------------------
	-@rar.exe a -rr -m4 -md1024 -av- -u -cl -r -isnd -ztavd.diz -x*.map -x*.exe -x*.obj -x*.err -x*.o ..\$(PROJECT).rar ..\$(PROJECT)

zip:
	@echo ------------------------------
	@echo Backing up to archive tavd.zip
	@echo ------------------------------
	cd ..
	-@zip.exe -ru9D $(PROJECT).zip $(PROJECT) -x *.map *.exe *.obj *.err *.o
	cd tavd

#-@zip.exe -r -4 -u -ztavd.diz -n*.map -n*.exe -n*.obj -n*.err -n*.o ..\$(PROJECT).zip ..\$(PROJECT)

clean:
	-@if exist *.$(O) del *.$(O)
	-@if exist *.exe del *.exe

ALL_MODULES = adat.cpp audio.cpp aftp.cpp csvector.cpp dpmiserv.cpp \
	      gal_test.cpp galey.cpp ini_test.cpp inifile.cpp crc32.cpp \
	      manchstr.cpp tavd.cpp djvidsys.cpp machtest.cpp adriver.cpp \
	      console.cpp

dep:
	makedep -o.$$(O) -c -I../MIKM300/INCLUDE -I. -I/djgpp/include -f$(PROJECT).dep $(ALL_MODULES)

#@includes -e -l -so $(ALL_MODULES) >tavd.dep
NEW=new/
IRC=irc/

%.$(O): $(IRC)$(NEW)%.cpp
	 gcc $(CFLAGS) $@ $<

%.$(O): $(IRC)%.cpp
	 gcc $(CFLAGS) $@ $<

%.$(O): %.cpp
	 gcc $(CFLAGS) $@ $<

%.exe: %.$(O)
	 gcc $(LFLAGS) $@ $^

-include $(PROJECT).dep

LIB=../mikm300/lib/
Lib_files=$(LIB)mikmod.a $(LIB)mmio.a $(LIB)extdos.a

MAINDRIVER= adriver.$(O) galey.$(O) djvidsys.$(O) dpmiserv.$(O)  \
	 manchstr.$(O) inifile.$(O) csvector.$(O) console.$(O)

$(PROJECT).exe: $(PROJECT).$(O) aftp.$(O) $(MAINDRIVER) crc32.$(O)
gal_test.exe: gal_test.$(O) galey.$(O) dpmiserv.$(O)
ini_test.exe: ini_test.$(O) inifile.$(O) csvector.$(O)
machtest.exe: machtest.$(O) manchstr.$(O) dpmiserv.$(O)
adat.exe: adat.$(O) audio.$(O) $(MAINDRIVER) $(Lib_files)
irc_test.exe: irc_test.$(O) irc.$(O) rle.$(O) vcr.$(O)
it.exe: it.$(O) ircnew.$(O) $(MAINDRIVER)