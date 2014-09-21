#define <dos.h>
#define <conio.h>

#define "mytypes.h"

int Xchg (unsigned int *var, unsigned int value);
#pragma aux Xchg parm [ecx] [eax] = "xchg eax,[ecx]" value [eax];

void DebugClick ()
{
  outp (0x43, 0xb6);
  outp (0x42, 8);
  outp (0x42, 8);
  outp (0x61, inp (0x61) | 3);
  for (dword i = 0; i < 0x1000000; i++) {}
  outp (0x61, inp (0x61) & 0xfc);
}

extern int mTaskSystem, mTaskVersion;
const mtNothing = 0;
const mtWindows = 1;
const mtOS2 = 2;

void DetectMultiTask ();
#pragma aux DetectMultiTask =			\
	       "mov	ax,0x1600"		\
	       "int	0x2F"			\
	       "mov	bx,0x0200"		\
	       "cmp	al,1"			\
	       "je	setWin" 		\
	       "cmp	al,0x0FF"		\
	       "je	setWin" 		\
	       "mov	ax,0x160A"		\
	       "xor	bx,bx"			\
	       "int	0x2F"			\
	       "test	bx,bx"			\
	       "jz	noWindows"		\
"setWin:	mov	mTaskSystem,1"		\
	       "movzx	ebx,bx" 		\
	       "mov	mTaskVersion,ebx"	\
	       "jmp	locEx"			\
"noWindows:	clc"				\
	       "mov	ax,0x3306"		\
	       "int	0x21"			\
	       "jc	DRDOS"			\
"noDRDOS:	cmp	al,0x0FF"		\
	       "je	locEx"			\
	       "cmp	bh,100" 		\
	       "jae	locEx"			\
	       "cmp	bl,10"			\
	       "jb	checkHLT"		\
	       "mov	al,bl"			\
	       "mov	ah,0"			\
	       "mov	bl,10"			\
	       "div	bl"			\
	       "mov	ah,bh"			\
	       "xchg	al,ah"			\
"setOS2:	mov	mTaskSystem,2"		\
	       "movzx	eax,ax" 		\
	       "mov	mTaskVersion,eax"	\
	       "jmp	locEx"			\
"checkHLT:	xor	dx,dx"			\
	       "mov	ax,1"			\
	       "hlt"				\
	       "db	0x35,0xCA,0xEB,1,0xC0"	\
	       "cmp	ax,1"			\
	       "ja	locEx"			\
	       "mov	ax,0x0201"		\
	       "jmp	setOS2" 		\
"DRDOS: 	cmp	ax,1"			\
	       "jne	noDRDOS"		\
"locEx:" modify [eax ebx ecx edx];

void GiveUpTimeSlice ();
#pragma aux GiveUpTimeSlice =			\
	       "mov	al,byte ptr mTaskSystem"\
	       "cmp	al,byte mtOS2"		\
	       "jne	noOS2"			\
	       "xor	dx,dx"			\
	       "mov	ax,1"			\
	       "hlt"				\
	       "db	0x35,0xCA"		\
	       "clc"				\
	       "jmp	locEx"			\
"noOS2: 	cmp	al,byte mtWindows"	\
	       "jne	locEx"			\
	       "mov	ax,0x1680"		\
	       "int	0x2F"			\
	       "clc"				\
	       "ret"				\
"locEx:" modify [eax ecx edx];

#endif // __WATDPMI_H__
