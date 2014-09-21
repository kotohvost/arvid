# TeleArVid, 2004y.

# Comment/uncomment the following line to disable/enable debugging
DEBUG = y

# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSIMPLE_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

ECC = ecc/

CFLAGS += $(DEBFLAGS)
CFLAGS += -Wall -I./inc
CXXFLAGS = $(CFLAGS)

LFLAGS = -s
TARGET = avd

.SUFFIXES: .cpp .c .o
.PHONY: all clean zip tgz bz2

ALL_TARGETS = $(TARGET)_view $(TARGET)_test tape_main field_gen \
	   $(ECC)bch1b_test $(ECC)goley_test ini_test $(TARGET)fs_mount \
	   $(ECC)rsb_test $(ECC)rs2b_test $(ECC)rs3b_test \
	   mod/$(TARGET)/$(TARGET).o mod/$(TARGET)fs/$(TARGET)fs.o

ECC_SRCS = $(ECC)ecc.c $(ECC)goley.c $(ECC)rs.c $(ECC)rsb.c \
	   $(ECC)rs2.c $(ECC)rs2b.c $(ECC)rs3.c $(ECC)rs3b.c \
	   $(ECC)bch4836.c $(ECC)bchb.c $(ECC)crc32.c $(ECC)chc.c

C_SRCS   = driver_thread.c tape.c tape_main.c dprint.c \
	   vcr.c irc.c pt.c $(TARGET)_test.c field_gen.c \
	   $(ECC)rsb_test.c $(ECC)rs2b_test.c $(ECC)rs3b_test.c \
	   $(ECC)goley_test.c $(ECC_SRCS)

CXX_SRCS = ini.cpp ini_test.cpp inifile.cpp csvector.cpp

C_OBJS   = $(C_SRCS:.c=.o)
ECC_OBJS = $(ECC_SRCS:.c=.o)
CXX_OBJS = $(CXX_SRCS:.cpp=.o)

all: .depend $(ALL_TARGETS)

clean:
	rm -f ./$(ECC)/*.o *.o core .depend $(ALL_TARGETS)
	cd mod/$(TARGET) && make clean
	cd mod/$(TARGET)fs && make clean

src: zip tgz bz2

zip: 
	cd .. && zip -ru9D $(TARGET).zip $(TARGET) -x@$(TARGET)/exclude.lst

tgz:
	cd .. && tar cf - $(TARGET)/ | gzip -9 > $(TARGET).tar.gz

bz2:
	cd .. && tar cf - $(TARGET)/ | bzip2 -9 > $(TARGET).tar.bz2

depend .depend dep:
	$(CC) $(CFLAGS) -MM *.c *.cpp $(ECC)*.c > $@

$(TARGET)_test: $(TARGET)_test.o
	$(CC) -o $@ $<

$(TARGET)_view: $(TARGET)_view.c
	$(CC) $(CFLAGS) -g $(TARGET)_view.c -o $(TARGET)_view `gtk-config --cflags` `gtk-config --libs`

$(TARGET)fs_mount: $(TARGET)fs_mount.o
	$(CC) -o $@ $<

field_gen: field_gen.o
	$(CC) -o $@ $<

tape_main: tape_main.o tape.o driver_thread.o ini.o inifile.o \
	   csvector.o dprint.o pt.o irc.o vcr.o $(ECC_OBJS)
	$(CXX) -o $@ $^

$(ECC)goley_test: $(ECC)goley_test.o $(ECC_OBJS) dprint.o
	$(CC) -o $@ $^

$(ECC)rsb_test: $(ECC)rsb_test.o $(ECC_OBJS) dprint.o
	$(CC) -o $@ $^

$(ECC)rs2b_test: $(ECC)rs2b_test.o $(ECC_OBJS) dprint.o
	$(CC) -o $@ $^

$(ECC)rs3b_test: $(ECC)rs3b_test.o $(ECC_OBJS) dprint.o
	$(CC) -o $@ $^

$(ECC)bch1b_test: $(ECC)bch1b_test.o $(ECC_OBJS) dprint.o
	$(CC) -o $@ $^

ini_test: ini_test.o inifile.o csvector.o
	$(CXX) -o $@ $^

mod/$(TARGET)/$(TARGET).o:
	cd mod/$(TARGET) && make

mod/$(TARGET)fs/$(TARGET)fs.o:
	cd mod/$(TARGET)fs && make

ifeq (.depend,$(wildcard .depend))
include .depend
endif
