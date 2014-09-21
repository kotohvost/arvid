
#ifndef __dj_include_sys_farptr_h_
#define __dj_include_sys_farptr_h_

#ifdef __cplusplus
extern "C" {
#endif

void _farsetsel (unsigned short selector);
#pragma aux _farsetsel parm [ax] = "mov fs,ax";

unsigned short _fargetsel (void);
#pragma aux _fargetsel = "mov ax,fs" value [ax];

void _farpokew (unsigned short selector,
		unsigned long offset,
		unsigned short value);
#pragma aux _farpokew parm [cx][edi][ax] = "mov [edi],ax";

unsigned short _farpeekw (unsigned short selector,
			  unsigned long offset);
#pragma aux _farpeekw parm [cx][esi] = "mov ax,[esi]" value [ax];

#ifdef __cplusplus
}
#endif

#endif /* !__dj_include_sys_farptr_h_ */
