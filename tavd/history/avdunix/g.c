/*
 *	ARVID10
 * Created:	13-Feb-96
 * Version:	3.46r4
 * Copyright 1991 Borland Intl.
 *
 * дешифратор
 * вход:
 * ds:bx - шифрованный буфер (ebx)
 * es:di - дешифрованный код (24 байт) (edi)
 *
 * Структура кадра. 23 блока по 88 слов
 * шифование из 12 в 23. Итого 12*88=1056 слов в кадре
 *
 * 6E66
 */

#include "g.h"
typedef	unsigned char	GF;

int DecoderG(GF *info, GF *code, unsigned int start, unsigned int count) {
	asm("
Kadr2	=0x00B0
Kadr3	=0x0160
Kadr4	=0x0210
Kadr5	=0x02C0
Kadr6	=0x0370
Kadr7	=0x0420
Kadr8	=0x04D0
Kadr9	=0x0580
Kadr10	=0x0630
Kadr11	=0x06E0
Kadr12	=0x0790
Kadr13	=0x0840
Kadr14	=0x08F0
Kadr15	=0x09A0
Kadr16	=0x0A50
Kadr17	=0x0B00
Kadr18	=0x0BB0
Kadr19	=0x0C60
Kadr20	=0x0D10
Kadr21	=0x0DC0
Kadr22	=0x0E70
Kadr23	=0x0F20

/* все в словах (word) */
	addl	%%eax, %%eax
/* номер блока в шифре */
	addl	%%eax, %%ebx

/* Дешефруем блок */
NextDec100:
/* сохраним счетчик */
	pushl	%%ecx
	xorl	%%edx, %%edx
/* Zero register */

	movw	(%%ebx), %%ax
	movw	%%ax, %%si
	xorw	Kadr4(%%ebx), %%ax
	movw	%%ax, data_4f8

	xorw	Kadr7(%%ebx), %%ax
	movw	%%ax, data_4fa

	xorw	Kadr12(%%ebx), %%ax
	xorw	Kadr2(%%ebx), %%ax
	xorw	Kadr10(%%ebx), %%ax
	xorw	Kadr3(%%ebx), %%ax
	movw	%%ax, 0x16(%%edi)
/* 12 слово */

	xorw	Kadr23(%%ebx), %%ax
	movw	%%ax, vWord12

	orw	%%ax, %%dx

	movw	Kadr2(%%ebx), %%ax
	xorw	%%si, %%ax
	movw	Kadr3(%%ebx), %%cx
	xorw	Kadr6(%%ebx), %%cx
	xorw	%%cx, %%ax
	xorw	Kadr9(%%ebx), %%ax
	xorw	Kadr11(%%ebx), %%ax
	movw	%%ax, 0x14(%%edi)
/* 11 слово */

	xorw	0x16(%%edi), %%ax
	movw	%%ax, data_4fc

	xorw	Kadr22(%%ebx), %%ax
	movw	%%ax, vWord11

	orw	%%ax, %%dx

	movw	Kadr10(%%ebx), %%ax
	xorw	Kadr8(%%ebx), %%ax
	xorw	%%si, %%ax
	movw	Kadr2(%%ebx), %%si
	xorw	Kadr5(%%ebx), %%si
	xorw	%%si, %%ax
	movw	%%ax, 0x12(%%edi)
/* 10 слово */

	xorw	0x14(%%edi), %%ax
	movw	%%ax, data_4fe

	xorw	Kadr21(%%ebx), %%ax
	movw	%%ax, vWord10

	orw	%%ax, %%dx

	movw	data_4fa, %%ax
	xorw	Kadr9(%%ebx), %%ax
	movw	%%ax, 0x10(%%edi)
/* 9 слово */

	xorw	0x12(%%edi), %%ax
	movw	%%ax, data_500

	xorw	Kadr20(%%ebx), %%ax
	movw	%%ax, vWord9

	orw	%%ax, %%dx

	xorw	Kadr8(%%ebx), %%cx
	movw	%%cx, 0x0e(%%edi)
/* 8 слово */

	movw	0x10(%%edi), %%ax
	xorw	%%cx, %%ax
	xorw	Kadr19(%%ebx), %%ax
	movw	%%ax, vWord8

	orw	%%ax, %%dx

	xorw	Kadr7(%%ebx), %%si
	movw	%%si, 0x0c(%%edi)
/* 7 слово */

	xorw	0x16(%%edi), %%cx
	movw	Kadr18(%%ebx), %%ax
	xorw	%%si, %%ax
	xorw	%%cx, %%ax
	movw	%%ax, vWord7

	orw	%%ax, %%dx

	movw	data_4f8, %%ax
	xorw	Kadr6(%%ebx), %%ax
	movw	%%ax, 0x0a(%%edi)
/* 6 слово */

	xorw	%%ax, %%si
	xorw	data_4fc, %%si
	xorw	Kadr17(%%ebx), %%si
	movw	%%si, vWord6

	orw	%%si, %%dx

	movw	Kadr5(%%ebx), %%si
	xorw	Kadr3(%%ebx), %%si
	movw	%%si, 0x08(%%edi)
/* 5 слово */

	xorw	%%si, %%ax
	xorw	data_4fc, %%ax
	xorw	0x12(%%edi), %%ax
	xorw	Kadr16(%%ebx), %%ax
	movw	%%ax, vWord5

	orw	%%ax, %%dx

	movw	Kadr2(%%ebx), %%ax
	movw	%%ax, 0x02(%%edi)
/* 2 слово */

	xorw	Kadr4(%%ebx), %%ax
	movw	%%ax, 0x06(%%edi)
/* 4 слово */

	xorw	%%si, %%ax
	xorw	0x10(%%edi), %%ax
	xorw	data_4fe, %%ax
	movw	Kadr15(%%ebx), %%si
	xorw	%%ax, %%si
	movw	%%si, vWord4

	orw	%%si, %%dx

	xorw	0x12(%%edi), %%ax
	xorw	data_4fa, %%ax
	xorw	0x0e(%%edi), %%ax
	xorw	Kadr2(%%ebx), %%ax
	xorw	Kadr13(%%ebx), %%ax
	movw	%%ax, vWord2

	orw	%%ax, %%dx

	movw	(%%ebx), %%ax
	movw	%%ax, (%%edi)
/* 1 слово */

	xorw	Kadr3(%%ebx), %%ax
	movw	%%ax, 0x04(%%edi)
/* 3 слово */

	xorw	%%cx, %%ax
	xorw	data_500, %%ax
	xorw	0x06(%%edi), %%ax
	xorw	Kadr14(%%ebx), %%ax
	movw	%%ax, vWord3

	orw	%%ax, %%dx
/* потом можно с'оптемезировать */
/*	movw	%%dx, vWord1 */
	jnz	Dec10024Exit

/* edi - дешифрованный код */
/* dx  - общие ошибки */
_restore:
	push	%%ebx
/* гуляющий бит */
	movl	$1, %%ebx

loc_719F:
	testw	%%bx, %%dx
/* нет ошибки */
	jz	loc_7219

	movw	%%bx, %%si
	negw	%%si
	subl	%%ecx, %%ecx
/* вичисляем слова содержищие ошибки */
	movw	vWord12, %%ax
	andw	%%bx, %%ax
	addw	%%si, %%ax
	rclw	$1, %%ecx
	movw	vWord11, %%ax
	andw	%%bx, %%ax
	addw	%%si, %%ax
	rclw	$1, %%ecx
	movw	vWord10, %%ax
	andw	%%bx, %%ax
	addw	%%si, %%ax
	rclw	$1, %%ecx
	movw	vWord9, %%ax
	andw	%%bx, %%ax
	addw	%%si, %%ax
	rclw	$1, %%cx
	movw	vWord8, %%ax
	andw	%%bx, %%ax
	addw	%%si, %%ax
	rclw	$1, %%cx
	movw	vWord7, %%ax
	andw	%%bx, %%ax
	addw	%%si, %%ax
	rclw	$1, %%cx
	movw	vWord6, %%ax
	andw	%%bx, %%ax
	addw	%%si, %%ax
	rclw	$1, %%cx
	movw	vWord5, %%ax
	andw	%%bx, %%ax
	addw	%%si, %%ax
	rclw	$1, %%cx
	movw	vWord4, %%ax
	andw	%%bx, %%ax
	addw	%%si, %%ax
	rclw	$1, %%cx
	movw	vWord3, %%ax
	andw	%%bx, %%ax
	addw	%%si, %%ax
	rclw	$1, %%cx
	movw	vWord2, %%ax
	andw	%%bx, %%ax
	addw	%%si, %%ax
	rclw	$1, %%cx

	addl	%%ecx, %%ecx
/* адрес таблицы кодов коррекции */
	leal	_XorCorrections, %%esi
	addl	%%ecx, %%esi
/* код определяющий местоположение ошибки */
	movzwl	(%%esi), %%eax

	orl	%%eax, %%eax
/* несчитается. */
	jz	loc_7219

	movl	$2, %%esi

	shrw	$1, %%ax
	jnc	loc_W1
	xorw	%%bx, (%%edi)

loc_W1:
	addl	%%esi, %%edi
	shrw	$1, %%ax
	jnc	loc_W2
	xorw	%%bx, (%%edi)

loc_W2:
	addl	%%esi, %%edi
	shrw	$1, %%ax
	jnc	loc_W3
	xorw	%%bx, (%%edi)

loc_W3:
	addl	%%esi, %%edi
	shrw	$1, %%ax
	jnc	loc_W4
	xorw	%%bx, (%%edi)

loc_W4:
	addl	%%esi, %%edi
	shrw	$1, %%ax
	jnc	loc_W5
	xorw	%%bx, (%%edi)

loc_W5:
	addl	%%esi, %%edi
	shrw	$1, %%ax
	jnc	loc_W6
	xorw	%%bx, (%%edi)

loc_W6:
	addl	%%esi, %%edi
	shrw	$1, %%ax
	jnc	loc_W7
	xorw	%%bx, (%%edi)

loc_W7:
	addl	%%esi, %%edi
	shrw	$1, %%ax
	jnc	loc_W8
	xorw	%%bx, (%%edi)

loc_W8:
	addl	%%esi, %%edi
	shrw	$1, %%ax
	jnc	loc_W9
	xorw	%%bx, (%%edi)

loc_W9:
	addl	%%esi, %%edi
	shrw	$1, %%ax
	jnc	loc_W10
	xorw	%%bx, (%%edi)

loc_W10:
	addl	%%esi, %%edi
	shrw	$1, %%ax
	jnc	loc_W11
	xorw	%%bx, (%%edi)

loc_W11:
	addl	%%esi, %%edi
	shrw	$1, %%ax
	jnc	loc_W12
	xorw	%%bx, (%%edi)

loc_W12:
	addl	%%esi, %%edi
	subl	$24, %%edi

loc_7219:
	shlw	$1, %%bx
	jnc	loc_719F

	xorl	%%eax, %%eax
	pop	%%ebx
	jmp	Dec10024Exit

	.data
/* статистика для востановления */
vWord1:		.word	0
vWord2:		.word	0
vWord3:		.word	0
vWord4:		.word	0
vWord5:		.word	0
vWord6:		.word	0
vWord7:		.word	0
vWord8:		.word	0
vWord9:		.word	0
vWord10:	.word	0
vWord11:	.word	0
vWord12:	.word	0

/* временные переменные */
data_4f8:	.word	0
data_4fa:	.word	0
data_4fc:	.word	0
data_4fe:	.word	0
data_500:	.word	0

	.text
Dec10024Exit:
/* востановим счетчик */
	popl	%%ecx
	addl	$24, %%edi
	addl	$2, %%ebx
	decl	%%ecx
	jnz	NextDec100
	"
	:
	:"ebx" (code), "D" (info), "eax" (start), "ecx" (count)
	:"eax", "ebx", "ecx", "edx", "esi", "edi", "ebp");
#if 0
	pushal
	leal	_inbuf, %ebx
	leal	_outbuf, %edi
	movzwl	_start, %edx
_nextblock:
	call	_xdescr24
	orw	%dx, %dx
	jz	NoErrors
	call	_restore

NoErrors:
	decw	_blk
	jnz	_nextblock

	popal
	ret
#endif
	return (0);
}
