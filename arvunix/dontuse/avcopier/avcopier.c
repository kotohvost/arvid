#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/avtreg.h>
/* #include <machine/ioctl_fd.h> */

avt_fifo	fifo;

main(int argc, char *argv)
{

int	i;
int	arvid;
char	*avt_device = "/dev/avt0";

  arvid = open ( "/dev/avt0", O_WRONLY, 0);
/*
    printf("Open status: %ud\n", arvid);

    ioctl( arvid, VGET_INFO, &ptr);
    printf ("Read info\n");
    printf ("Type: %d\n", ptr.controller);
    printf ("Port: %x\n", ptr.port);
    printf ("Irq:  %d\n", ffs(ptr.irq)-1);
    printf ("MaxPhase: %d\n", ptr.maxphase);
    printf ("MaxRate: %d Kbyte/s\n", ptr.maxrate);
    printf ("Version: %d.%d\n\n", ptr.ver_major, ptr.ver_minor);

    printf ("data info:\n");
*/
nnn:
	ioctl( arvid, VAVT_TEST, &fifo);
/*	printf ("num: %u\n", data.buf); */
/*
	for (i=0; i < 32; i=i+2) printf (" %x%x", data.info[i+1], data.info[i]);
	printf ("\n");
	for (i=32; i < 32*2; i=i+2) printf (" %x%x", data.info[i+1], data.info[i]);
	printf ("\n");
*/
	for (i=0; i < fifo.count; i++)
	{
	int count;
	/* выводим данные */
	 for (count=0; count < SizeData; count++)
	  printf ("%c", fifo.buf[i].data[count]);
#if 0
	/* выводим TPB */
	 for (count=0; count < sizeof (TPBInfo *); count++)
	  printf ("%c", fifo.buf[i].TPB[count]);
#endif
	}
/*
	printf ("count = %d\n", data.buf);
	printf ("\n"); */
goto nnn;
/*
    ioctl( arvid, VGET_RAW, &raw);
    for (i=0; i < 25; i++) printf (" %x", raw.data[i]);
    printf ("\n");
    for (i=25; i < 25*2; i++) printf (" %x", raw.data[i]);
    printf ("\n");
    for (i=25*2; i < 25*3; i++) printf (" %x", raw.data[i]);
    printf ("\n");
*/
    close (arvid);
}
