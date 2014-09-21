#ifndef __ADRIVER_H__
#define __ADRIVER_H__

#define HFRAME_1031 284
// 300? (EZ)
#define HFRAME_105X 296
#define WFRAME_1031 9
#define WFRAME_105X 13

#define HFRAME_IRC  312
#define WFRAME_IRC  12
//Minimal for cluster 3, maximal for low density 0x100 / 9 = 28,
//maximal for high density 0x100 / 13 = 19
#define HEADER_STR  8
#define HDATAFRAME_1031 (HFRAME_1031 - HEADER_STR)
#define HDATAFRAME_105X (HFRAME_105X - HEADER_STR)
#define WDATAFRAME_1031 (WFRAME_1031 * 2)
#define WDATAFRAME_105X (WFRAME_105X * 2)
#define WIRCFRAME	(WFRAME_IRC * 2)

#define SIZE_DATA_FRAME_1031 (HDATAFRAME_1031 * WDATAFRAME_1031)
#define SIZE_DATA_FRAME_105X (HDATAFRAME_105X * WDATAFRAME_105X)
#define SIZE_IRC_FRAME	     ((HFRAME_IRC * WIRCFRAME))
#define SSIZE_IRC_FRAME      (SIZE_IRC_FRAME + 12)

#define END_SEND_COM	     -1
#define END_REC_COM	     -2
#define END_DEC_COM	     -3
#define FREE_SEMAPHOR	     -4

#define ER_NO_FREE_SEMAPHOR  -1
#define ER_NO_END_DEC_COM    -2
#define ER_NO_FREE_MEM	     -3
#define ER_NO_AS_COM	     -4

#define ID_NO_FRAME    0
#define ID_CLASTER     1
#define ID_FRAME       2
#define ID_EMPTY_FRAME 3
#define ID_IRC	       4

//16bit register.Access: base port + index
//base port setup by jumpers on ArVid card
#define RD_INC	 (BasePort + 0)
#define RD	 (BasePort + 2)
#define RK	 (BasePort + 4)
#define RA	 (BasePort + 6)
#define RS1	 (BasePort + 4)
#define RS2	 (BasePort + 6)

//Command flag
#define SET_IR		    0xFFFE //0 = режим ИК-последовательностей
#define SET_DATA	    0x0001 //1 = режим данных ВМ
#define SET_TRANSMIT	    0xFFFD //0 = пеpедача
#define SET_RECIVE	    0x0002 //1 = пpием
#define RESET_AUTO_PHASE    0xFFFB //0 = выключен автоматический выбоp фазы пpиема данных
#define SET_AUTO_PHASE	    0x0004 //1 = включен
#define RESET_INC_BUFFER    0xFFF7 //0 = нет инкpемента номеpа буфеpа кадpа
#define SET_INC_BUFFER	    0x0008 //1 = инкpемент
#define RESET_RESET_SEQ     0xFFEF //0 = нет сбpоса очеpеди в начальное состояние
#define SET_RESET_SEQ	    0x0010 //1 = сбpос очеpеди
#define PHASE_MASK	    0x00E0 //5,6,7 : код фазы пpиема данных
#define SET_EMU_1031	    0xFEFF //0 = режим эмуляции Арвид-1031
#define SET_1052	    0x0100 //1 = работа в режиме Арвид-1052
#define SET_200K	    0xFDFF //0 = плотность записи 200КБ/с
#define SET_325K	    0x0200 //1 = плотность записи 325КБ/с
#define LINK_VM2TV	    0xFBFF //0 = подключение ВМ к TV
#define LINK_VM2PC	    0x0400 //1 = подключение ВМ к РС
#define SUBPHASE_MASK	    0x1800 //11,12 : код подфазы пpиема данных

//Status flag
#define LOW_COUNT_CADR_MASK  0x07 //0,1,2 : младшие 3 разряда кода счетчика очеpеди буфеpов кадpов
#define CHECK_LOOP_MASK      0x08 //бит 3 : 0 = ошибка кольца данных пpи пеpедаче, 1 = нет ошибки
#define ID_1052_MASK	     0x10 //бит 4 : функция ИЛИ битов 2 и 9 RK ( пpизнак контроллера 1052)
#define HIGH_COUNT_CADR_MASK 0x00E0 //5,6,7 : старшие 3 разряда кода счетчика очеpеди буфеpов кадpов

typedef struct ErrorStatistics
{
  dword OrdinaryErr;
  dword DupletErr;
  dword TripletErr;
  word	BadFrames;
  word	MapBadFrames;
} estat;

// Stek of asyncronus command
#define MAX_AS_COM 300

typedef struct
{
  dword CRC;
  byte	FrameNumber;
  dword ClusterNumber;
} frhdr;

// Element of command table. Consist Semaphor object
typedef struct
{
  word	*Buffer;
  frhdr *pFrHdr;
  vint	Semaphore;
  int	Type;
  dword Size;
  estat ES;
} ComElem;

extern bool InitArvid (void);
extern bool FreeArvid (void);
extern void InitAsynCommand (void);
extern void FreeAsynCommand (void);
extern bool InitSyncroCommand (void);
extern void FreeSyncroCommand (void);

extern int SendACluster (dword NumCluster, byte *OutBuffer, dword ClusterID);
extern int SendIRCCommand (byte *OutBuffer, dword Size);
extern int SendIRCSCommand (byte *OutBuffer, dword Size);
extern int ReceiveIRCSCommand (int hCom, dword NumFrames);
extern int ReceiveACluster (int NumCluster);
extern dword SendSCluster (dword NumCluster, byte *OutBuffer, dword ClusterID);
extern bool SendAFrames (word *OutBuffer, vint *pSemaphore, int Type, frhdr *pFrHdr);
extern bool SendAIFrames (word *OutBuffer, vint *pSemaphore);
extern bool ReceiveAFrames (word *InBuffer, vint *pSemaphore, int Type, frhdr *pFrHdr);
extern void DecodeANDFreeSemaphor (void);
extern bool CheckOnFreeComBuffer (void);
extern void PrintRFStatisics (void);
extern void FreeSemaphor (int i);
extern void BeginTAVDLockedCode (void);
extern void EndTAVDLockedCode (void);

extern bool ArvidInsert, AsyncFlag, ReceiveAct;
extern word ArvidModel, BasePort;
extern dword HArvFrame, WArvFrame, HDataFrame, WDataFrame, ArvidIRQ;
extern dword SizeDataFrame, SizeOrigCluster;
extern bool IComFlag, RComFlag, SComFlag, StartFlag, StopByUser, ErrAudit;
extern ComElem *ComTable;
extern word ValueRK, BasePort, HDensity, Phase, SubPhase, Arv105X, AutoPhase;
extern word DelayToTransmit;
extern char ArvFont [];

#endif