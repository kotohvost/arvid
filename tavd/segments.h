/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#ifndef __dj_include_sys_segments_h_
#define __dj_include_sys_segments_h_

#ifdef __cplusplus
extern "C" {
#endif

int _my_cs (void);
#pragma aux _my_cs = "mov ax,cs" "movzx eax,ax" value [eax];

int _my_ds (void);
#pragma aux _my_ds = "mov ax,ds" "movzx eax,ax" value [eax];

int _my_ss (void);
#pragma aux _my_ss = "mov ax,ss" "movzx eax,ax" value [eax];

#ifdef __cplusplus
}
#endif

#endif /* !__dj_include_sys_segment_h_ */
