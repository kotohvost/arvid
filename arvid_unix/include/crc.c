#include <stdio.h>
#include <machine/param.h>
#include "../include/rsgen.h"
#include "../include/avtreg.h"

u_short crc16 (avt_kadr100 *kadr, int start, int c)
{
u_short	crc = 0;
register	byte, i;	/* обязательно так */

	for (i = start; c; i++, c--)
	{
	byte = kadr->data[i];
	crc +=crc;
	asm ("jnc	1f");
	byte ^= 0x100B;
	asm ("1:");
	crc ^= byte;
	}
	return (crc);
}

/*
 * Подсчет контрольной суммы CRC32. Используется
 * для формата FORMAT_K7CRC и выше. Размер в СЛОВАХ ???
 */
u_int crc32 (u_char *data, u_int count)
{
#define	POLINOM32	0x4c11db7
u_int	i;
u_int	crc = 0;

/*	for (i=0; i < count; i++) {
		if ((crc <<= 1) > 0xffffffffL) crc ^= POLINOM32;
		}
*/
	asm ("
	xorl	%%edx, %%edx
	shrl	$1, %%ecx
	jnc	f713d
	incl	%%ecx
	lodsw
	shll	$16, %%eax
	jmp	f715f
f713d:
	addl	%%edx, %%edx
	jnc	f7145
	xorl	%%ebx, %%edx
f7145:
	lodsl
	xorl	%%eax, %%edx
	decl	%%ecx
	jz	f7165

	add	%%edx, %%edx
	jnc	f7159
	xorl	%%ebx, %%edx
f7159:
	lodsl
f715f:
	xorl	%%eax, %%edx
	decl	%%ecx
	jnz	f713d
f7165:
	"
	: "=d" (crc)
	: "ebx" (POLINOM32),
	"ecx" (count),
	"edx" (crc),
	"S" (data) );
	return (crc);
}

/*
 * Подсчет контрольной суммы CRC32. Используется
 * для формата FORMAT_K7CRC и выше. Размер в СЛОВАХ ???
 */
u_int crc32s (u_char *data, u_int count)
{
u_int	crc;
	crc = crc32(data, count);
	crc += crc;
	asm("	jnc	crc32s1f");
		crc ^= POLINOM32;
	asm("crc32s1f:");
	return (crc);
}