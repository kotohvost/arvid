
#ifndef __DRIVER_THREAD_H__
#define __DRIVER_THREAD_H__

#ifdef __cplusplus
extern "C" {
#endif

/* msg command */
#define BASE_COMMAND 777 /* magic number */

/* commands for control driver thread */
#define EOP_COMMAND                 (BASE_COMMAND + 0)
#define AVT_LOOP_TEST_COMMAND       (BASE_COMMAND + 1)
#define AVT_SET_PHASE_COMMAND       (BASE_COMMAND + 2)
#define AVT_SET_SUBPHASE_COMMAND    (BASE_COMMAND + 3)
#define AVT_SET_AUTOPHASE_COMMAND   (BASE_COMMAND + 4)
#define AVT_RESET_AUTOPHASE_COMMAND (BASE_COMMAND + 5)
#define AVT_DATA_FRAME_NOT_FOUND_COMMAND (BASE_COMMAND + 6)

/* hdr.fr_type - frame types in frame header */
#define ID_EMPTY_FRAME 0 /* virtual type, not used in real headers */
#define ID_RAW_FRAME   1 /* virtual type, not used in real headers */
#define ID_IRC_FRAME   2 /* virtual type, not used in real headers */
#define ID_G1_FRAME    3
#define ID_RS1_FRAME   4
#define ID_RS2_FRAME   5
#define ID_RS3_FRAME   6
#define ID_RS4_FRAME   7 /* not implemented yet, reserved */
#define ID_BCH1_FRAME  8
#define ID_G1_CLUSTER  9 /* not implemented yet, reserved */
#define ID_RS1_CLUSTER 10 /* not implemented yet, reserved */
#define ID_RS2_CLUSTER 11 /* not implemented yet, reserved */
#define ID_RS3_CLUSTER 12 /* not implemented yet, reserved */
#define ID_RS4_CLUSTER 13 /* not implemented yet, reserved */
#define ID_BCH1_CLUSTER 14 /* not implemented yet, reserved */

#define ID_ECC_DATA    0xF /* virtual type for multicasting receive or mask for selecting frames type */
#define ID_FRAME_MASK  ID_ECC_DATA

/* irc send command frame inserting stuff */
#define BEG_PAUSE SSIZE_IRC_FRAME
#define END_PAUSE 0

/* used for frames exchange in queues */
#define MAX_FRAME_SIZE 8000 /* length commands data */

typedef struct
{
  long mtype;
  long com;  /* fmode O_RDONLY || O_WRONLY */
  long unit; /* Arvid board number */
  unsigned long tray; /* read tray count */
  char mode; /* high speed (325 kBps), low speed (200 kBps), irc; in short ('h','l','i') */
  estat err; /* recoveri errors statistic structure */
  fr_hdr hdr; /* frame header data */
} command;

extern int FrameIDToECCType (int fr_type);
extern int ECCTypeToFrameID (int flags);
extern int LoopBackTest (command *pcom, dword Flags);

/* phase: 0,1,2,3,4,5,6,7 */
extern int SetPhase (int unit, int phase);

/* subphase: 0,1,2,3,4 */
extern int SetSubPhase (int unit, int subphase);

/* phase: 0,1 */
extern int SetAutoPhase (int unit, int autophase);

extern int SetReadMode (int unit, int phase, int subphase, int autophase);

/* handles of queues */
extern int msgmain, msgdrv;

#ifdef __cplusplus
}
#endif

#endif
