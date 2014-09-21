#Makefile for Watcom 11.0 by Levtchenko V.

# Pentium, k5, 5x86
HOW_OPTIMIZE= -5r -obpmantrile -s
# PentiumPro, Pentium2, K6, 6x86
#HOW_OPTIMIZE=-6r -obpmantrile -s

PROJECT = tavd

O=obj
.SUFFIXES:
.SUFFIXES: .exe .$(O) .cpp .h
.EXTENSIONS:
.EXTENSIONS: .exe .$(O) .cpp .h

CXX = wpp386
LINK = wlink
CFLAGS= -zc -zp1 -zq $(HOW_OPTIMIZE) -i.

project: $(PROJECT).exe
gtest: gal_test.exe
itest: ini_test.exe
all:   $(PROJECT).exe gal_test.exe ini_test.exe

rar: .SYMBOLIC
	@echo ------------------------------
	@echo Backing up to archive tavd.rar
	@echo ------------------------------
	-@rar a -rr -m4 -md1024 -av- -u -cl -r -isnd -ztavd.diz -x*.map -x*.exe -x*.obj -x*.err -x*.o ..\$(PROJECT).rar ..\$(PROJECT)

clean: .SYMBOLIC
	-@if exist *.obj @del *.obj
	-@if exist *.err @del *.err
	-@if exist *.map @del *.map
	-@if exist *.exe @del *.exe

.BEFORE
	@set DOS4G=QUIET

.cpp.$(O):
	$(CXX) $^& $(CFLAGS)
.$(O).exe:
	@%create $^&.lnk
	@%append $^&.lnk name $@
	@%append $^&.lnk system dos4g
	@%append $^&.lnk option stack=256000, quiet
	@for %f in ($<) do @%append $^&.lnk file %f
	$(LINK) @$^&.lnk
	@del $^&.lnk

include $(PROJECT).dep

$(PROJECT).exe: $(PROJECT).$(O) galey.$(O) djvidsys.$(O) dpmiserv.$(O) \
		aftp.$(O) adat.$(O) crc32.$(O) inifile.$(O) csvector.$(O) \
		console.$(O) manchstr.$(O) wdpmi.$(O)
gal_test.exe: gal_test.$(O) galey.$(O) dpmiserv.$(O) wdpmi.$(O)
ini_test.exe: ini_test.$(O) inifile.$(O) csvector.$(O)
machtest.exe: machtest.$(O) manchstr.$(O) dpmiserv.$(O) wdpmi.$(O)
