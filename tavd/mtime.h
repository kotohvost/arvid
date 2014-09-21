#ifndef __MTIME_H__
#define __MTIME_H__

#include <sys/timeb.h>

static inline time_t get_time ()
{
  timeb tp;
  ftime (&tp);
  return tp.time * 1000 + tp.millitm;
}

#endif