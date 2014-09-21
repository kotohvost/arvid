
#ifndef __IRC_H__
#define __IRC_H__

#ifdef __cplusplus
extern "C" {
#endif

extern int SendIRCCommand (int unit, byte *IRCCom, int size_buf);
extern int PlayIRCCommandFromFile (command *pcom, char *name);
extern int SaveIRCCommandToFile (command *pcom, char *name, int empty_frames);

#ifdef __cplusplus
}
#endif

#endif
