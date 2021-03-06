
#ifndef __ARVID_H__
#define __ARVID_H__

#define HFRAME_1031 284 /* number strings in native 200 kBps speed */
/* 300? (EZ) */
#define HFRAME_105X 296 /* number strings in native 325 kBps speed */
#define WFRAME_1031 9   /* number words in string in native 200 kBps speed */
#define WFRAME_105X 13  /* number words in string in native 325 kBps speed */

#define HFRAME_IRC  312 /* number strings in irc mode */
#define WFRAME_IRC  12  /* number words in strings in irc mode */

/* 16bit registers.Access: base port + index */
/* base port setup by jumpers on ArVid card */
#define RD_INC	    (punit->BasePort + 0) /* I/O data register with address increment */
#define RD	    (punit->BasePort + 2) /* I/O data register without address increment */
#define RK	    (punit->BasePort + 4) /* I command register */
#define RA	    (punit->BasePort + 6) /* I address register */
#define RS1	    (punit->BasePort + 4) /* first status register */
#define RS2	    (punit->BasePort + 6) /* second status register */
#define PORT_RANGE  8   /* number bytes port */

/* Commands flags */
#define SET_IR		    0xFFFE /* 0 = AND mask for irc mode set */
#define SET_DATA	    0x0001 /* 1 = OR mask for data mode set */
#define SET_TRANSMIT	    0xFFFD /* 0 = AND mask for transmit mode set */
#define SET_RECIVE	    0x0002 /* 1 = OR mask for receive mode set */
#define RESET_AUTO_PHASE    0xFFFB /* 0 = AND mask for auto phase reset */
#define SET_AUTO_PHASE	    0x0004 /* 1 = OR mask for auto phase set */
#define RESET_INC_BUFFER    0xFFF7 /* 0 = AND mask for frame count reset */
#define SET_INC_BUFFER	    0x0008 /* 1 = OR mask for frame count set */
#define RESET_RESET_SEQ     0xFFEF /* 0 = AND mask for seqience autoincrement reset */
#define SET_RESET_SEQ	    0x0010 /* 1 = OR mask for seqience autoincrement set*/
#define PHASE_MASK	    0x00E0 /* 5,6,7: AND mask for phase select */
#define SET_EMU_1031	    0xFEFF /* 0 = AND mask for set arvid 1031 emulation */
#define SET_1052	    0x0100 /* 1 = OR mask for set arvid 1052 mode */
#define SET_200K	    0xFDFF /* 0 = AND mask for set low data speed */
#define SET_325K	    0x0200 /* 1 = OR mask for set high data speed */
#define LINK_VM2TV	    0xFBFF /* 0 = AND mask for VHS to TV connect */
#define LINK_VM2PC	    0x0400 /* 1 = OR mask for VHS to PC connect */
#define SUBPHASE_MASK	    0x1800 /* 11,12: AND mask for subphase select */

/* Status flags */
#define LOW_COUNT_FRAME_MASK  0x07 /* 0,1,2: AND mask for frame count low part select */
#define CHECK_LOOP_MASK       0x08 /* 3: AND mask for data loop check */
#define ID_1052_MASK	      0x10 /* 4: AND mask for arvid 1052 detect */
#define HIGH_COUNT_FRAME_MASK 0x00E0 /* 5,6,7: AND mask for frame count high part select */

#define MOD_NAME            "avd"  /* device node name in folder /dev */
#define MAX_AVT             4      /* ArVid boards maximal number */
#define MAX_AVT_SUBFRAMES   0x800  /* maximal avaible number memory subframes, used for memory testing */
#define	AVT_MAJOR           127    /* major number for link device name with his methods */

/* DT - driver thread, service module, may be locked by module */
/* MT - main thread, main program loop */

#pragma pack(push, 1)
/* must be packed */
typedef struct
{
  dword crc_id; /* DT fill this */
  word  size;   /* 0-11 bits: high level api (HLA) data size in words, 12-15 bits - frame type */
  byte  cl_num; /* DT fill this */
  dword id;     /* incremental needs for streams, but time_stamp is better of this */
  dword volume; /* volume id */
} fr_hdr;
#pragma pack(pop)

/* data frame header definition */
/* first GAP_STR fill of 0xAAAA */
#define GAP_STR  2 /* strings for AGC turning */

/* Goley redundancing is ~ 2 and low speed convectional adds more 2 */
#define HEADER_SIZE (sizeof (fr_hdr) * 2 * 2)

#define HDATAFRAME_1031 (HFRAME_1031 - GAP_STR)
#define HDATAFRAME_105X (HFRAME_105X - GAP_STR)
#define WDATAFRAME_1031 (WFRAME_1031 * 2)
#define WDATAFRAME_105X (WFRAME_105X * 2)

/* used by ECC */
#define SIZE_DATA_FRAME_1031 ((HDATAFRAME_1031 * WDATAFRAME_1031) - HEADER_SIZE)
#define SIZE_DATA_FRAME_105X ((HDATAFRAME_105X * WDATAFRAME_105X) - HEADER_SIZE)

/* irc receive and send sizes */
#define WIRCFRAME	(WFRAME_IRC * 2)
#define SIZE_IRC_FRAME	(HFRAME_IRC * WIRCFRAME)
#define SSIZE_IRC_FRAME (HFRAME_IRC * WIRCFRAME + 12)

#define PAL_FPS          50 /* PAL/SECAM frame per seconds */

#endif
