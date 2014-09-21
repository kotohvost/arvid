#include <stdio.h>
#include <sys/ioctl.h>	/* ioctl */
#include <fcntl.h>	/* open */
#include <unistd.h>	/* close */
#include <machine/avtreg.h>

avt_kadr	data;

main()
{

int	i, c;
int	arvid;
u_short	crc16;
int	crc32;

  arvid = open ( "/dev/ravt0", O_WRONLY, 0);

for (c=0; c<300; c++)
  {
  ioctl( arvid, VGET_KADR, &data);
  printf ("\nTape format  : %x\n", data.TPB.TapeFormat);
  printf ("Tape name    : ");
   for (i=0; i < 8; i++) printf ("%c", data.TPB.TapeName[i]); printf("\n");
  printf ("Tape ID      : %u\n", data.TPB.TapeID);
  printf ("Tape time    : %u\n", data.TPB.TapeTime);
  printf ("Block flag   : %x\t", data.TPB.TapeFlag);
  printf ("Sector/Block : %d\n", data.TPB.TapeNsector);
  printf ("Block crc  %x/", data.TPB.TapeCRC);
   crc16 = 0;
   {
   register	byte;	/* обязательно так */
   for (i=1032 ; i < (1032 + 11); i++)
    {
    byte = data.data[i];
    crc16 = crc16 * 2;
    asm("jnc	1f");
    byte ^= 0x100b;
    asm("1:");
    crc16 ^= byte;
    }
   };
  printf ("%x\n", crc16);
/* */
  printf ("Block0 crc %x/", data.CRC.block0);
{
 uint  a, c=0;
 for (a=0; a < (512/2); a+=2)
  {
   c+=c;
   asm("jnc	2f");
   c^=0x04c11db7;
   asm("2:");
   c^=(data.data[a] + (data.data[a+1] << 16));
/* printf("%c%c%c%c", data.data[a], (data.data[a]>>8), data.data[a+1], (data.data[a+1]>>8)); */
  }
 printf ("%x\n", c);
}
  printf ("Block1 crc %x\n", data.CRC.block1);
  printf ("Block2 crc %x\n", data.CRC.block2);
  printf ("Block3 crc %x\n", data.CRC.block3);
  printf ("Block crc  %x/", data.CRC.crc);
{
uint	a, c=0;
 for (a=0; a < 20/2; a+=2)
  {
   c+=c;
   asm("jnc	3f");
   c^=0x04c11db7;
   asm("3:");
   c^=(data.data[a+1044] + (data.data[a+1+1044] << 16));
  }
printf ("%x\n", c);
}
/* */
 for (i=0; i < 1056; i++) printf ("%c%c", data.data[i], (data.data[i] >> 8));

/* printf ("%d\n", ioctl( arvid, VGET_KADR, &data)); */
 }
  close (arvid);
}
