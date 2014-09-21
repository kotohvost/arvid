
#ifndef __DPRINT_H__
#define __DPRINT_H__

#ifdef __cplusplus
extern "C" {
#endif

extern void Progress (void);
extern void print_data (int Verb, byte *str, int max_len);

#ifdef unix
#define dprintf dprintfl
#endif

/* verbosing printf */
extern void dprintf (int Verb, const char *str, ...);

/* view traditional hexadecimal dump */
extern void dump (int Verb, byte *buf, int len);

extern bool DFlag; /* true - debug output avaible */
extern int  VerbFlag, AnimPhase; /* 0 - minimal verbose, 5 - maximal verbose */
extern char AnimTable []; /* progress indicators template */

#ifdef __cplusplus
}
#endif

#endif
