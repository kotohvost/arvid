#include <X11/Xlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "../include/avtreg.h"

/*
 * static const char *dispname=":0";
 * extern Display *XOpenDisplay(const char*);
 */

unsigned char	bbuf[8192];
unsigned short	wbuf[8192];
unsigned short	trans[256];

#define SIZEX  (9*16*2)
#define SIZEY  (284+9)
#define FORMAT XYBitmap
#define DEPTH  1
#define PAD    8

void preparetrans(void) {
int	i;
u_short	w;

  for(i=0; i<256; i++) {
    w = 0xffff;
    if(i & 0x01) w ^= (3 << 0x00);
    if(i & 0x02) w ^= (3 << 0x02);
    if(i & 0x04) w ^= (3 << 0x04);
    if(i & 0x08) w ^= (3 << 0x06);
    if(i & 0x10) w ^= (3 << 0x08);
    if(i & 0x20) w ^= (3 << 0x0A);
    if(i & 0x40) w ^= (3 << 0x0C);
    if(i & 0x80) w ^= (3 << 0x0E);
    trans[i] = w;
  }
}

void bufupdate(void) {
}

int main(int ac,char *av[]) {
Display		*disp;
Window		win;
XEvent		evt;
XGCValues	gcval;
GC		gc;
XImage		*ximg;
long		emask;
int		i, vfd, count, stop=0, exstat;
char		*arvdev=ARVDEV;
struct set_rw	SetRW;
int		counter=0;
int		AvPhase, fPhase;
static char	ProgName[1024];
static char	AvContr[32];
struct avt_info	Arvid;

	if(ac > 2) {
		printf("\nusage: xarvideo [/dev/arvid]\n");
		return 1;
	}

	if (ac == 2) arvdev=av[1];

	vfd = open(arvdev, O_RDONLY);
	if(vfd < 0) {
		printf("\nERROR: can't open file %s\n", arvdev);
		return 2;
	}

/*
 * Open the display and defaults screen specified by the enviroment
 * variable DISPLAY.
 */
	if ((disp = XOpenDisplay(0)) == 0) {
		printf("cannot create a window on DISPLAY%s\n", getenv("DISPLAY"));
		return 3;
	}

	AvPhase = 0;
	fPhase = 0;
	if (ac == 1) {
		ioctl(vfd, VGET_INFO, &Arvid);
		snprintf(AvContr, 32, "Arvid-%d", Arvid.arvid);
		SetRW.rate = 200;
		SetRW.mode = IORAW;
		ioctl(vfd, VSET_READ, &SetRW);
		ioctl(vfd, VGET_PHASE, &AvPhase);
		sleep(1);
		ioctl(vfd, VSET_PHASE, &AvPhase);
	}

	preparetrans();

  win  = XCreateSimpleWindow(disp,
			     DefaultRootWindow(disp),
			     100,100,
			     SIZEX,SIZEY,
			     1,
			     BlackPixel(disp,DefaultScreen(disp)),
			     WhitePixel(disp,DefaultScreen(disp)));

/*  emask = ResizeRedirectMask | ExposureMask | ButtonPressMask | ButtonReleaseMask |
          PointerMotionMask | KeyPressMask | KeyReleaseMask | FocusChangeMask;
*/
	emask = ButtonPressMask | ButtonReleaseMask |
		KeyPressMask | KeyReleaseMask;
	XSelectInput(disp, win, emask);

	XMapWindow(disp, win);
	snprintf(ProgName, 1023, "%s, Phase: %d\0", AvContr, AvPhase);
	XStoreName(disp, win, ProgName);

  /* set necessary GC values */
  gcval.foreground = BlackPixel(disp, DefaultScreen(disp));
  gcval.background = WhitePixel(disp, DefaultScreen(disp));
  gc = XCreateGC(disp, win, GCForeground | GCBackground, &gcval);

  ximg = XCreateImage(disp,
		      CopyFromParent,
		      DEPTH,
		      FORMAT,
		      0,
		      (char *) wbuf,
		      SIZEX,
		      SIZEY,
		      PAD,
		      SIZEX/8);
  XInitImage(ximg);

	exstat = 0;
	while(!exstat) {
#if 1
		if (XCheckWindowEvent(disp, win, emask, &evt) == True ) {
			switch(evt.type) {
			case FocusIn:
			case FocusOut:
			case MotionNotify:
				break;
			case ButtonPress:
	printf("Button = %04x\n", evt.xbutton.button);
	printf("State  = %04x\n", evt.xbutton.state);
#if 0
	if(evt.xbutton.button == 1) stop = (stop)?0:1;
	if(evt.xbutton.button == 2) return 0;
	if(evt.xbutton.button == 3) {
	  lseek(vfd, 0, SEEK_SET);
	  stop=0;
	}
#endif
				break;
			case ButtonRelease:
				break;
			case KeyPress:
	printf("Key   = %04x\n", evt.xkey.keycode);
	printf("State = %04x\n", evt.xkey.state);
				if (evt.xkey.keycode == 0x18)
					exstat = 1;
				if (evt.xkey.keycode == 0x52) {
					if (AvPhase)
						AvPhase--;
					fPhase = 1;
				}
				if (evt.xkey.keycode == 0x56) {
					AvPhase++;
					fPhase = 1;
				}
				break;
			case KeyRelease:
				break;
			case ResizeRequest:
				XResizeWindow(disp, win, SIZEX, SIZEY);
				break;
			case Expose:
				printf("Expose\n");
				XPutImage(disp, win, gc, ximg, 0, 0, 0, 0, SIZEX, SIZEY);
				break;
			} /* switch */
		} /* XCheck */
#endif
		if (fPhase && ac == 1) {
			ioctl(vfd, VSET_PHASE, &AvPhase);
			ioctl(vfd, VGET_PHASE, &AvPhase);
			snprintf(ProgName, 1023, "%s, Phase: %d\0", AvContr, AvPhase);
			XStoreName(disp, win, ProgName);
			fPhase = 0;
		}

	/*    if(stop) continue; */
		count = read(vfd, bbuf, 8192);

		if (count > 0) {
			for(i=0; i<8192; i++) wbuf[i] = trans[bbuf[i]];
		/*	printf("Counter: %d, %d\n", counter, count); */
			XPutImage(disp, win, gc, ximg, 0, 0, 0, 0, SIZEX, SIZEY);
			XFlush(disp);
			usleep(100);
		} else if (count < 0)
		/* error */
			exstat = 1;
	} /* while */
	XCloseDisplay(disp);
}
