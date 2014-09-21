/*
 *
 */
#define	POLINOM16	0x100B		/* */
#define	POLINOM32	0x4c11db7	/* */

unsigned short crc16 (unsigned char *data, int c) {
unsigned short		crc = 0;
register		byte;	/* обязательно так */

	while (c--) {
		byte = *data;
		crc += crc;
		asm ("jnc	1f");
		byte ^= POLINOM16;
		asm ("1:");
		crc ^= byte;
		data++;
		}
	return (crc);
}

/*
 * Подсчет контрольной суммы CRC32. Используется
 * для формата FORMAT_K7CRC и выше. Размер в СЛОВАХ ???
 */
unsigned int crc32 (unsigned char *data, unsigned int count) {
unsigned int	i;
unsigned int	crc = 0;

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
unsigned int crc32s (unsigned char *data, unsigned int count) {
unsigned int	crc;
	crc = crc32(data, count);
	crc += crc;
	asm("	jnc	crc32s1f");
		crc ^= POLINOM32;
	asm("crc32s1f:");
	return (crc);
}