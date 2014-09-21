INCLUDEDIR = /usr/include

CFLAGS = -O2 -D__KERNEL__ -DMODULE -I$(INCLUDEDIR) # -Wall

OBJ = avt.o
HEADER = avt.h
SOURCE = avt.c
OSVERSION = `uname -r`
MOD = avt

DEVAVT = avt
MAJOR = 66

all: $(OBJ)

$(OBJ): $(HEADER) $(SOURCE)

clean:
	rm -f *.o *.ver *~ 

distclean: clean
	rm -f /lib/modules/$(OSVERSION)/misc/$(OBJ)

install: all
	cp $(HEADER) /usr/local/include/
	cp $(OBJ) /lib/modules/$(OSVERSION)/misc/
	depmod -a

devs:
	rm -f /dev/$(DEVAVT)
	mknod /dev/$(DEVAVT) c $(MAJOR) 0
	chmod a+w /dev/$(DEVAVT)

start: install
	insmod $(MOD)

stop:
	rmmod $(MOD)
