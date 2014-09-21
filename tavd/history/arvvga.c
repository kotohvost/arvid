#include <conio.h>
#include <string.h>
#include <i86.h>
// program arvid
// Copyright 1998 Mike Jangolenko jangolen@infopro.spb.su

#define BasePort 0x390
#define Phase	 0x04
#include "1052.h"

#define VgaMem	 0xA000
#define WidtScreen  320
#define HiestScreen 200

typedef unsigned char byte;
typedef unsigned int  word;

int  i,j,n,x,y;
byte DimWord[16];
word Yoffset[HiestScreen];
byte *pScreen;

main(int   argc, char  *argv[])
{
  register DataWord;
  union REGS Reg;
  word	ValueRK;

  Reg.x.ax =0x13;
  int86(0x10,&Reg,&Reg);

  for(i=0;i<HiestScreen;i++) Yoffset[i] =i * WidtScreen;
  memset(DimWord,0,sizeof(DimWord));

  //запись экранов
  outpw(RK,ValueRK =SET_DATA|SET_AUTO_PHASE|LINK_VM2PC); //connect PC to VM, reset seq, set operate "data"

  //clear ArVid memory
  for(i=0;i<=0x7f;i++) //128 subcadr count
   {
    outpw(RA,i); //RA - set subcadr
    for(j=0;j<=0xff;j++) outpw(RD_INC,0x0000); //clear subcadr
   }

  //transmit
  for(i=0;i<=50*1;i++) while((inp(RS1)&LOW_COUNT_CADR_MASK)>6) outpw(RK,ValueRK|=SET_INC_BUFFER);

  //прием
  outpw(RK,ValueRK|=SET_RESET_SEQ|(Phase<<5));

  while(!kbhit())
   {
    pScreen =MK_FP(VgaMem,0);
    memset(pScreen,0,WidtScreen*HiestScreen);

    for(i=0;i<14;i++) //receive 5632 byte
     {
      outp(RA,i); //RA - set subcadr

//	ValueRK&=RESET_AUTO_PHASE;
//	outpw(RK,ValueRK|=SET_RECIVE);

      for(j=0;j<=0xff;j++)
       {
	if((n= (i<<8)+j)>1792)
	 {
	  n-=1792;
	  x =((n%9)<<4) +9*17;
	 }
	  else x =(n%9)<<4;

	y= n/9; //9 word, density 200 kb per second
	n= Yoffset[y] +x; //offset in screen

	DataWord =inpw(RD_INC);
	DimWord[0] =((DataWord&0x0001)<<4) -1;
	DimWord[1] =((DataWord&0x0002)<<3) -1;
	DimWord[2] =((DataWord&0x0004)<<2) -1;
	DimWord[3] =((DataWord&0x0008)<<1) -1;
	DimWord[4] =((DataWord&0x0010)<<0) -1;
	DimWord[5] =((DataWord&0x0020)>>1) -1;
	DimWord[6] =((DataWord&0x0040)>>2) -1;
	DimWord[7] =((DataWord&0x0080)>>3) -1;

	DimWord[8] =((DataWord&0x0100)>>4) -1;
	DimWord[9] =((DataWord&0x0200)>>5) -1;
	DimWord[10]=((DataWord&0x0400)>>6) -1;
	DimWord[11]=((DataWord&0x0800)>>7) -1;
	DimWord[12]=((DataWord&0x1000)>>8) -1;
	DimWord[13]=((DataWord&0x2000)>>9) -1;
	DimWord[14]=((DataWord&0x4000)>>10)-1;
	DimWord[15]=((DataWord&0x8000)>>11)-1;

	pScreen =MK_FP(VgaMem,n);
	memcpy(pScreen,DimWord,16);
	ValueRK&=RESET_AUTO_PHASE;
//	  outpw(RK,ValueRK|=SET_RECIVE|SET_RESET_SEQ);
	outpw(RK,ValueRK|=SET_RECIVE);
       }
     }
   }
  outpw(RK,ValueRK&=LINK_VM2TV);

  Reg.x.ax =3;
  int86(0x10,&Reg,&Reg);
}



