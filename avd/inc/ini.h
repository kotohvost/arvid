
#ifndef __INI_H__
#define __INI_H__

typedef struct
{
  word insert; /* board inserting flag */
  word phase;  /* tapes reading phase */
  float velosity; /* VHS tape velosity in meters */
  word sub_phase; /* tapes reading subphase */
  byte auto_phase; /* type of phase automat */
  byte auto_power; /* VHS power auto on/off */
  char sp_vcr_filename [MAX_PATH]; /* path to .vcr filename for "single play" speed */
  char lp_vcr_filename [MAX_PATH]; /* path to .vcr filename for "long play" speed */
  char elp_vcr_filename[MAX_PATH]; /* path to .vcr filename for "extra long play" speed */
} avdinipar;

#define ASV		 "autosave value"
#define DEFAULT_INI_FILE "./avd.ini"

#ifdef __cplusplus
extern "C" {
#endif

/* Global variables */
/* trains finding of frame, frame not found timeout */
extern int retrain_count, timeout;
extern avdinipar avdpar [MAX_AVT];

/* .ini files methods */
extern int LoadIniFile (char *ini_file, avdinipar *pai, int boards);
extern int SaveIniFile (char *ini_file, avdinipar *pai, int boards);

#ifdef __cplusplus
}
#endif

#endif
