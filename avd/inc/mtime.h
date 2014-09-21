
#ifndef __MTIME_H__
#define __MTIME_H__

#include <sys/timeb.h>

/* OS independent time function, return time in miliseconds */
static inline unsigned long get_time (void)
{
  struct timeb tp;
  ftime (&tp);
  return tp.time * 1000 + tp.millitm;
}

#endif
