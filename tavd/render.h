#ifndef __RENDER_H__
#define __RENDER_H__

static inline void mover (byte *dst, byte *src, dword num_dots)
{
  for (dword i = 0; i < num_dots; i++)
   {
    dst [i] = *src;
    src += 4;
   }
}

static inline void movecol (byte *dst, byte *src, dword hiest, dword screen_width)
{
  for (dword i = 0; i < hiest; i++)
   {
    *dst = src [i];
    dst += screen_width;
   }
}

#ifdef __GNUC__
// Move "count" bytes from src to (far *)dest in X-mode
static inline void XFmove (unsigned char *src, unsigned char *dest, int count)
{
  static unsigned int oldebp;
  asm ("	movl	%%ebp, %3
		movl	$16, %%ebx
		movl	%2, %%ebp
		movl	$64, %%edx
		shrl	$4, %%ebp
		jz	2f

1:		movb	8(%1), %%cl
		movb	0(%1), %%al
		movb	12(%1), %%ch
		movb	4(%1), %%ah
		shll	$16, %%ecx
		movw	%%ax, %%cx
		fs
		movl	%%ecx, (%0)

		movb	24(%1), %%cl
		movb	16(%1), %%al
		movb	28(%1), %%ch
		movb	20(%1), %%ah
		shll	$16, %%ecx
		movw	%%ax, %%cx
		fs
		movl	%%ecx, 4(%0)

		movb	40(%1), %%cl
		movb	32(%1), %%al
		movb	44(%1), %%ch
		movb	36(%1), %%ah
		shll	$16, %%ecx
		movw	%%ax, %%cx
		addl	%%edx, %1
		fs
		movl	%%ecx, 8(%0)

		movb	-8(%1), %%cl
		movb	-16(%1), %%al
		movb	-4(%1), %%ch
		movb	-12(%1), %%ah
		shll	$16, %%ecx
		movw	%%ax, %%cx
		addl	%%ebx, %0
		fs
		movl	%%ecx, -4(%0)

		decl	%%ebp
		jnz	1b

2:		movl	%3, %%ebp
		movl	%2, %%ebp
		andl	$15, %%ebp
		jz	4f

3:		movb	(%1), %%al
		addl	$4, %1
		fs
		movb	%%al, (%0)
		incl	%0
		decl	%%ebp
		jnz	3b

4:		movl	%3, %%ebp"
    :
    : "D" (dest), "S" (src), "m" (count), "m" (oldebp)
    : "%eax", "%ecx", "%ebx", "%edx", "%esi", "%edi");
}

// Move "count" bytes from src to dest in X-mode
static inline void Xmove (unsigned char *src, unsigned char *dest, int count)
{
  static unsigned int oldebp;
  asm ("	movl	%%ebp, %3
		movl	$16, %%ebx
		movl	%2, %%ebp
		movl	$64, %%edx
		shrl	$4, %%ebp
		jz	2f

1:		movb	8(%1), %%cl
		movb	0(%1), %%al
		movb	12(%1), %%ch
		movb	4(%1), %%ah
		shll	$16, %%ecx
		movw	%%ax, %%cx
		movl	%%ecx, (%0)

		movb	24(%1), %%cl
		movb	16(%1), %%al
		movb	28(%1), %%ch
		movb	20(%1), %%ah
		shll	$16, %%ecx
		movw	%%ax, %%cx
		movl	%%ecx, 4(%0)

		movb	40(%1), %%cl
		movb	32(%1), %%al
		movb	44(%1), %%ch
		movb	36(%1), %%ah
		shll	$16, %%ecx
		movw	%%ax, %%cx
		addl	%%edx, %1
		movl	%%ecx, 8(%0)

		movb	-8(%1), %%cl
		movb	-16(%1), %%al
		movb	-4(%1), %%ch
		movb	-12(%1), %%ah
		shll	$16, %%ecx
		movw	%%ax, %%cx
		addl	%%ebx, %0
		movl	%%ecx, -4(%0)

		decl	%%ebp
		jnz	1b

2:		movl	%3, %%ebp
		movl	%2, %%ebp
		andl	$15, %%ebp
		jz	4f

3:		movb	(%1), %%al
		addl	$4, %1
		movb	%%al, (%0)
		incl	%0
		decl	%%ebp
		jnz	3b

4:		movl	%3, %%ebp"
    :
    : "D" (dest), "S" (src), "m" (count), "m" (oldebp)
    : "%eax", "%ecx", "%ebx", "%edx", "%esi", "%edi");
}

// Move bytes from src to (far *)dest until src >= last
static inline void VESAFmove (unsigned char *src, unsigned char *dest,
  unsigned char *last)
{
  asm ("	movl	$32, %%ecx
		movl	%2, %%edx
		subl	%0, %%edx
		shrl	$5, %%edx
		jz	2f
1:		movl	(%0), %%eax
		fs
		movl	%%eax, (%1)
		movl	4(%0), %%eax
		fs
		movl	%%eax, 4(%1)
		movl	8(%0), %%eax
		fs
		movl	%%eax, 8(%1)
		movl	12(%0), %%eax
		fs
		movl	%%eax, 12(%1)
		movl	16(%0), %%eax
		fs
		movl	%%eax, 16(%1)
		movl	20(%0), %%eax
		fs
		movl	%%eax, 20(%1)
		movl	24(%0), %%eax
		fs
		movl	%%eax, 24(%1)
		movl	28(%0), %%eax
		fs
		movl	%%eax, 28(%1)
		addl	%%ecx, %0
		addl	%%ecx, %1
		decl	%%edx
		jnz	1b
2:		movl	%2, %%edx
		subl	%0, %%edx
		jz	4f
3:		movb	(%0), %%al
		incl	%0
		fs
		movb	%%al, (%1)
		incl	%1
		decl	%%edx
		jnz	3b
4:" :
    : "S" (src), "D" (dest), "m" (last)
    : "%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi");
}

// Move bytes from src to dest until src >= last
static inline void VESAmove (unsigned char *src, unsigned char *dest,
  unsigned char *last)
{
  asm ("	movl	$32, %%ecx
		movl	%2, %%edx
		subl	%0, %%edx
		shrl	$5, %%edx
		jz	2f
1:		movl	(%0), %%eax
		movl	%%eax, (%1)
		movl	4(%0), %%eax
		movl	%%eax, 4(%1)
		movl	8(%0), %%eax
		movl	%%eax, 8(%1)
		movl	12(%0), %%eax
		movl	%%eax, 12(%1)
		movl	16(%0), %%eax
		movl	%%eax, 16(%1)
		movl	20(%0), %%eax
		movl	%%eax, 20(%1)
		movl	24(%0), %%eax
		movl	%%eax, 24(%1)
		movl	28(%0), %%eax
		movl	%%eax, 28(%1)
		addl	%%ecx, %0
		addl	%%ecx, %1
		decl	%%edx
		jnz	1b
2:		movl	%2, %%edx
		subl	%0, %%edx
		jz	4f
3:		movb	(%0), %%al
		incl	%0
		movb	%%al, (%1)
		incl	%1
		decl	%%edx
		jnz	3b
4:" :
    : "S" (src), "D" (dest), "m" (last)
    : "%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi");
}

#else //else if __WATCOMC__
// Move "count" bytes from src to (far *)dest in X-mode
void XFmove (unsigned char *src, unsigned char *dest, int count);
#pragma aux XFmove parm [esi][edi][ecx] = \
"push ebp"\
"mov ebp,ecx"\
"push ebp"\
"mov ebx,16"\
"mov edx,64"\
"shr ebp,4"\
"jz m2"\
"m1: mov cl,[esi+8]"\
"mov al,[esi]"\
"mov ch,[esi+12]"\
"mov ah,[esi+4] "\
"shl ecx,16"\
"mov cx,ax"\
"mov fs:[edi], ecx"\
"mov cl,[esi+24]"\
"mov al,[esi+16]"\
"mov ch,[esi+28]"\
"mov ah,[esi+20]"\
"shl ecx,16"\
"mov cx,ax"\
"mov fs:[edi+4], ecx"\
"mov cl,[esi+40]"\
"mov al,[esi+32]"\
"mov ch,[esi+44]"\
"mov ah,[esi+36]"\
"shl ecx,16"\
"mov cx,ax"\
"add esi,edx"\
"mov fs:[edi+8],ecx"\
"mov cl,[esi-8]"\
"mov al,[esi-16]"\
"mov ch,[esi-4]"\
"mov ah,[esi-12]"\
"shl ecx,16"\
"mov cx,ax"\
"add edi,ebx"\
"mov fs:[edi-4],ecx"\
"dec ebp"\
"jnz m1"\
"m2: pop ebp"\
"and ebp,0fh"\
"jz m4"\
"m3: "\
"mov al,[esi]"\
"add esi,4"\
"mov fs:[edi],al"\
"inc edi"\
"dec ebp"\
"jnz m3"\
"m4:"\
"pop ebp"\
modify [eax ecx ebx edx esi edi];

// Move "count" bytes from src to dest in X-mode
void Xmove (unsigned char *src, unsigned char *dest, int count);
#pragma aux Xmove parm [esi][edi][ecx] = \
"push ebp"\
"mov ebp,ecx"\
"push ebp"\
"mov ebx,16"\
"mov edx,64"\
"shr ebp,4"\
"jz m2"\
"m1: mov cl,[esi+8]"\
"mov al,[esi]"\
"mov ch,[esi+12]"\
"mov ah,[esi+4] "\
"shl ecx,16"\
"mov cx,ax"\
"mov [edi], ecx"\
"mov cl,[esi+24]"\
"mov al,[esi+16]"\
"mov ch,[esi+28]"\
"mov ah,[esi+20]"\
"shl ecx,16"\
"mov cx,ax"\
"mov [edi+4], ecx"\
"mov cl,[esi+40]"\
"mov al,[esi+32]"\
"mov ch,[esi+44]"\
"mov ah,[esi+36]"\
"shl ecx,16"\
"mov cx,ax"\
"add esi,edx"\
"mov [edi+8],ecx"\
"mov cl,[esi-8]"\
"mov al,[esi-16]"\
"mov ch,[esi-4]"\
"mov ah,[esi-12]"\
"shl ecx,16"\
"mov cx,ax"\
"add edi,ebx"\
"mov [edi-4],ecx"\
"dec ebp"\
"jnz m1"\
"m2: pop ebp"\
"and ebp,0fh"\
"jz m4"\
"m3: "\
"mov al,[esi]"\
"add esi,4"\
"mov [edi],al"\
"inc edi"\
"dec ebp"\
"jnz m3"\
"m4:"\
"pop ebp"\
modify [eax ecx ebx edx esi edi];

// Move bytes from src to (far *)dest until src >= last
void VESAFmove (unsigned char *src, unsigned char *dest, unsigned char *last);
#pragma aux VESAFmove parm [esi][edi][edx] = \
"push edx"\
"mov ecx,32"\
"sub edx,esi"\
"shr edx,5"\
"jz m2"\
"m1: mov eax,[esi]"\
"mov fs:[edi],eax"\
"mov eax,[esi+4]"\
"mov fs:[edi+4],eax"\
"mov eax,[esi+8]"\
"mov fs:[edi+8],eax"\
"mov eax,[esi+12]"\
"mov fs:[edi+12],eax"\
"mov eax,[esi+16]"\
"mov fs:[edi+16],eax"\
"mov eax,[esi+20]"\
"mov fs:[edi+20],eax"\
"mov eax,[esi+24]"\
"mov fs:[edi+24],eax"\
"mov eax,[esi+28]"\
"mov fs:[edi+28],eax"\
"add esi,ecx"\
"add edi,ecx"\
"dec edx"\
"jnz m1"\
"m2: pop edx"\
"sub edx,esi"\
"jz m4"\
"m3: mov al,[esi]"\
"inc esi"\
"mov fs:[edi],al"\
"inc edi"\
"dec edx"\
"jnz m3"\
"m4:"\
modify [eax ecx esi edi];

// Move bytes from src to dest until src >= last
void VESAmove (unsigned char *src, unsigned char *dest, unsigned char *last);
#pragma aux VESAmove parm [esi][edi][edx] = \
"push edx"\
"mov ecx,32"\
"sub edx,esi"\
"shr edx,5"\
"jz m2"\
"m1: mov eax,[esi]"\
"mov [edi],eax"\
"mov eax,[esi+4]"\
"mov [edi+4],eax"\
"mov eax,[esi+8]"\
"mov [edi+8],eax"\
"mov eax,[esi+12]"\
"mov [edi+12],eax"\
"mov eax,[esi+16]"\
"mov [edi+16],eax"\
"mov eax,[esi+20]"\
"mov [edi+20],eax"\
"mov eax,[esi+24]"\
"mov [edi+24],eax"\
"mov eax,[esi+28]"\
"mov [edi+28],eax"\
"add esi,ecx"\
"add edi,ecx"\
"dec edx"\
"jnz m1"\
"m2: pop edx"\
"sub edx,esi"\
"jz m4"\
"m3: mov al,[esi]"\
"inc esi"\
"mov [edi],al"\
"inc edi"\
"dec edx"\
"jnz m3"\
"m4:"\
modify [eax ecx esi edi];

#endif
#endif