#ifndef __dj_include_sys_movedata_h_
#define __dj_include_sys_movedata_h_

#ifdef __cplusplus
extern "C" {
#endif

/* These lengths are in bytes, optimized for speed */
void dosmemget (unsigned long _offset, int _length, void *_buffer);
void dosmemput (const void *_buffer, int _length, unsigned long _offset);

#ifdef __cplusplus
}
#endif

#endif /* !__dj_include_sys_movedata_h_ */
