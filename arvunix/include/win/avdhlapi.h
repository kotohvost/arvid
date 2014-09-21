#ifndef __AVDHLAPI__

#define __AVDHLAPI__

#include"arvidapi.h"

#ifndef AVDRET
 #define AVDRET int;
#endif

#ifndef __HLAPIDLL
  #ifdef _MAKEHLAPIDLL
    #define __HLAPIDLL extern "C" __declspec(dllexport) AVDRET
  #else
    #define __HLAPIDLL extern "C" __declspec(dllimport) AVDRET
  #endif
#endif

#define TAPEHANDLE		HANDLE
#define DIRHANDLE		HANDLE
#define MAX_DESCRIPTION	4096

enum 
{
HL_E_INVALID_HANDLE=(1<<8),
HL_E_NOT_OPEN,
HL_E_BAD_AVT,
HL_E_ALREADY_EXIST,
HL_E_DIR_SAVE_ER,
HL_E_BAD_PT,
HL_E_BAD_OPER,
HL_E_CONVERTED_AVT,
HL_E_NOT_ALL_DIR_CLOSED,
HL_E_DIRECTORY_NOT_EXIST,
HL_E_EXIST,
HL_E_NO_FILE,
HL_E_DIR_NOT_EMPTY,
HL_E_NO_MORE_FILES,
HL_E_ARVID_ALREADY_INSTALL,
HL_E_ARVID_ALREADY_CLOSE,
HL_E_ARVID_NOT_INSTALL,
HL_E_REGISTRY,
HL_E_BACKUP_TO_TDR,
HL_E_TAPE_NOT_SAME,
HL_E_THREAD,
HL_E_UNKNOWN
};

enum ErrReason
{
ERR_FATAL,
ERR_NOT_OPEN,
ERR_FILE_READ_ERROR,
ERR_FILE_WRITE_ERROR,
ERR_DISK_FULL,
ERR_ARVID_READ_ERROR,
ERR_NOT_CLOSE,
ERR_COMPARE_ERROR
};

typedef struct
{
BOOL		read_only;
DWORD		format;
DWORD		length;
}TAPE_INFO;

typedef struct 
{
BOOL		fdir;	// TRUE - директория, FALSE - файл
FILETIME	time;
DWORD		size;
DWORD		startsector;
char		name[MAX_PATH];
char		description[MAX_DESCRIPTION];
}TAPE_FILE_DATA;

typedef struct
{
ErrReason	error_code;
char		disk_name[MAX_PATH];
char		tape_name[MAX_PATH];
char		description[MAX_DESCRIPTION];
}ARVID_FILE_DATA;

class StatError
{public:
DWORD	err[3];
DWORD	nsect;};

class Statistics
{public:
char				CurrentFile[MAX_PATH];
StatError			CurFileErrors,
					TotalErrors;
unsigned __int64	TotalSize,
					TotalObr,
					DopObr;
DWORD				CurFileSize,
					CurFileObr,
					CurFileDensity,
					CurFileRetries,
					TotalData,
					StartTime,
					UpdateTime,
					TotalDensity,
					TotalRetries,
					TotalFiles,
					GoodFiles,
					BadFiles,
					regime;};

typedef AVDRET (*InFileFunc)(ARVID_FILE_DATA* lpafd);
typedef void (*OutFileFunc)(const ARVID_FILE_DATA* lpafd);
typedef void (*OutDelFile)(const char *del_file);

__HLAPIDLL AvdOpenArvid(AVHANDLE *avh);
__HLAPIDLL AvdCloseArvid();

__HLAPIDLL AvdOpenTape(LPCSTR tapename,TAPEHANDLE *lpthandle);
__HLAPIDLL AvdCreateTape(LPCSTR tapename,DWORD length,DWORD format,TAPEHANDLE *lpthandle);
__HLAPIDLL AvdTestTape(TAPEHANDLE th);
__HLAPIDLL AvdGetTapeInfo(TAPEHANDLE th,TAPE_INFO *lpti);
__HLAPIDLL AvdCloseTape(TAPEHANDLE th);

__HLAPIDLL AvdIdentifyTape(int control);

__HLAPIDLL AvdSearchPath(TAPEHANDLE th,LPCSTR pathname,TAPE_FILE_DATA *fdata);
__HLAPIDLL AvdRenamePath(TAPEHANDLE th,LPCSTR pathname,LPCSTR newname);
__HLAPIDLL AvdDeletePath(TAPEHANDLE th,LPCSTR pathname,OutDelFile odf);

__HLAPIDLL AvdOpenDirectory(TAPEHANDLE th,LPCSTR dirname,DIRHANDLE *lpdhandle);
__HLAPIDLL AvdCloseDirectory(DIRHANDLE dh);
__HLAPIDLL AvdGetNextFile(DIRHANDLE dh,TAPE_FILE_DATA *fdata);

__HLAPIDLL AvdSetDescription(TAPEHANDLE th,LPCSTR pathname,char *pdescript);

__HLAPIDLL AvdBackup(TAPEHANDLE th,InFileFunc iff,OutFileFunc off,OutFileFunc eff,unsigned __int64 totsize,DWORD totfiles,HANDLE *handle);
__HLAPIDLL AvdRestore(TAPEHANDLE th,short mode,InFileFunc iff,OutFileFunc off,OutFileFunc eff,unsigned __int64 totsize,DWORD totfiles,DWORD nRetry,HANDLE *handle);
__HLAPIDLL AvdGetThreadResult();

#ifndef __ACTION__
  enum Action {D_WRITE,D_READ,D_COMPARE,D_VERIFY};
#endif
#ifndef __ACTION__
  #define __ACTION__
#endif

__HLAPIDLL AvdShow(TAPEHANDLE th,Action act,BOOL wait);

__HLAPIDLL AvdGetStatistics(Statistics *stat);

#endif
