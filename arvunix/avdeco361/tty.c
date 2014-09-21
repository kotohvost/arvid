/*
 * Copyright 1990-1994 by Serge Vakulenko.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Serge Vakulenko not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Serge Vakulenko makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * SERGE VAKULENKO DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL SERGE VAKULENKO BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Serge Vakulenko, vak@kiae.su
 */

/*
 *      TtySet ()
 *
 *              - set terminal CBREAK mode.
 *
 *      TtyReset ()
 *
 *              - restore terminal mode.
 *
 *      TtyFlushInput ()
 *              - flush input queue.
 */

#ifndef __MSDOS__

# ifdef TERMIOS
#    include <sys/termios.h>
#    define TERMIO
#    define termio termios
#    undef TCGETA
#    undef TCSETA
#    undef TCSETAW
#    ifdef sun
#       define TCGETA TCGETS
#       define TCSETA TCSETS
#       define TCSETAW TCSETSW
#    else
#       include <sys/ioctl.h>
#       define TCGETA TIOCGETA
#       define TCSETA TIOCSETA
#       define TCSETAW TIOCSETAW
#    endif
#    ifndef OLCUC
#      define OLCUC 0
#    endif
#    ifndef IUCLC
#       define IUCLC 0
#    endif
#    ifndef OCRNL
#       define OCRNL 0
#    endif
#    ifndef XCASE
#       define XCASE 0
#    endif
# else	/* TERMIOS */
# ifdef TERMIO
#    include <termio.h>
# else
#    include <sgtty.h>
# endif
# endif	/* TERMIOS */

# ifdef VENIX
#    undef TIOCSETC                     /* such a marasmatic system */
# endif

# define CHANNEL 2                      /* output file descriptor */

# ifdef TERMIO
static struct termio oldtio, newtio;
# else
static struct sgttyb tty;               /* режимы работы терминала */
static ttyflags;
# ifdef TIOCSETC
static struct tchars oldtchars, newtchars;
# endif
# endif	/* TERMIO */

# ifdef NEEDLITOUT
static oldlocal, newlocal;
# endif

# ifdef TIOCSLTC
static struct ltchars oldchars, newchars;
# endif

# ifdef TAB3
#    define OXTABS TAB3
# endif

int TtyUpperCase;

# define NOCHAR 0

/* scr */
void TtySet () {
# ifdef TERMIO
	if (ioctl (CHANNEL, TCGETA, (char *) &oldtio) < 0)
		return;
	if (oldtio.c_oflag & OLCUC)
		TtyUpperCase = 1;       /* uppercase on output */
	newtio = oldtio;
	newtio.c_iflag &= ~(INLCR | ICRNL | IGNCR | ISTRIP | IUCLC);
	newtio.c_oflag &= ~(OLCUC | OCRNL | OXTABS);
	newtio.c_lflag &= ~(ECHO | ICANON | XCASE);
	newtio.c_cc [VMIN] = 1;         /* break input after each character */
	newtio.c_cc [VTIME] = 1;        /* timeout is 100 msecs */
	newtio.c_cc [VINTR] = NOCHAR;
	newtio.c_cc [VQUIT] = NOCHAR;
# ifdef VSWTCH
	newtio.c_cc [VSWTCH] = NOCHAR;
# endif
# ifdef VDSUSP
	newtio.c_cc [VDSUSP] = NOCHAR;
# endif
# ifdef VLNEXT
	newtio.c_cc [VLNEXT] = NOCHAR;
# endif
# ifdef VDISCARD
	newtio.c_cc [VDISCARD] = NOCHAR;
# endif
	ioctl (CHANNEL, TCSETAW, (char *) &newtio);
# else
	if (gtty (CHANNEL, &tty) < 0)
		return;
	if (tty.sg_flags & LCASE)
		TtyUpperCase = 1;       /* uppercase on output */
	ttyflags = tty.sg_flags;
	tty.sg_flags &= ~(XTABS | ECHO | CRMOD | LCASE);
#    ifdef CBREAK
	tty.sg_flags |= CBREAK;
#    endif
#    ifdef NEEDLITOUT
	tty.sg_flags |= LITOUT;
#    endif
	stty (CHANNEL, &tty);
#    ifdef TIOCSETC
	ioctl (CHANNEL, TIOCGETC, (char *) &oldtchars);
	newtchars = oldtchars;
	newtchars.t_intrc = NOCHAR;
	newtchars.t_quitc = NOCHAR;
	newtchars.t_eofc = NOCHAR;
	newtchars.t_brkc = NOCHAR;
	ioctl (CHANNEL, TIOCSETC, (char *) &newtchars);
#    endif
# endif /* TERMIO */
# ifdef NEEDLITOUT
	ioctl (CHANNEL, TIOCLGET, (char *) &oldlocal);
	newlocal = oldlocal | LLITOUT;
	ioctl (CHANNEL, TIOCLSET, (char *) &newlocal);
# endif
# ifdef TIOCSLTC
	ioctl (CHANNEL, TIOCGLTC, (char *) &oldchars);
	newchars = oldchars;
	newchars.t_lnextc = NOCHAR;
	newchars.t_rprntc = NOCHAR;
	newchars.t_dsuspc = NOCHAR;
	newchars.t_flushc = NOCHAR;
	ioctl (CHANNEL, TIOCSLTC, (char *) &newchars);
# endif
}

/* scr */
void TtyReset () {
# ifdef TERMIO
	ioctl (CHANNEL, TCSETA, (char *) &oldtio);
# else
	tty.sg_flags = ttyflags;
	stty (CHANNEL, &tty);
#    ifdef TIOCSETC
	ioctl (CHANNEL, TIOCSETC, (char *) &oldtchars);
#    endif
# endif
# ifdef NEEDLITOUT
	ioctl (CHANNEL, TIOCLSET, (char *) &oldlocal);
# endif
# ifdef TIOCSLTC
	ioctl (CHANNEL, TIOCSLTC, (char *) &oldchars);
# endif
}

/* key */
void TtyFlushInput () {
# ifdef TCFLSH
	ioctl (CHANNEL, TCFLSH, 0);
# else
#    ifdef TIOCFLUSH
	int p = 1;

	ioctl (CHANNEL, TIOCFLUSH, (char *) &p);
#    endif
# endif
}

# endif /* MSDOS */
