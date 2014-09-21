// Project TeleArVid
// Copyright 1998 Mike Jangolenko jangolen@infopro.spb.su,
// Slavik Levtchenko bit@eltech.ru

#include "compiler.h"
#include <bios.h>
#include <_dpmi.h>

#include "mytypes.h"
#include "adriver.h"
#include "galey.h"
#include "dpmiserv.h"
#include "manchstr.h"
#include "djvidsys.h"
#include "console.h"
#include "inline.h"
#include "mtime.h"

void BeginTAVDLockedCode () {}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~\_Global_variable_/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool ArvidInsert = true, ErrAudit = true, ControlCodeEnable = true;
bool ReceiveAct = true, StopByUser = false, AsyncFlag = true;
word ArvidModel = 1031, ArvidModelDetect = 1031, DelayToTransmit = 0;

word ValueRK = 0, Arv105X = 0, HDensity = 0, FrameMask = 0;
word Phase = 5 << 5, SubPhase = 0, AutoPhase = 0, BasePort = 0x390; //default my value;

// Hardware parametrs
dword HArvFrame = HFRAME_1031, WArvFrame = WFRAME_1031, NumArvFrame = 8;
dword SizeArvFrame = WArvFrame * HArvFrame, ArvidIRQ = 11;

// Logical parametrs
dword HeadStr = HEADER_STR;
dword HDataFrame = HArvFrame - HeadStr, WDataFrame = WArvFrame << 1;
dword WDataFrBit = WArvFrame << 4;
dword SizeDataFrame = WDataFrame * HDataFrame;
dword SizeOrigCluster = SizeDataFrame * VALUE_LEN;
dword SizeGaleyCluster = SizeDataFrame * CODE_LEN;

//~~~~~~~~~~~~~~~~~~~~~~~~~~\_Level_0_drivers_/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  byte NumBit [0x100];

static void RecalcNumBitTable (void)
{
  int Mask;

  for (int i = 0; i < 0x100; i++)
   {
    Mask = 1;
    NumBit [i] = 0;

    for (int j = 0; j < 8; j++)
     {
      NumBit [i] += (i & Mask) >> j;
      Mask <<= 1;
     }
   }
}

//~~~~~~~~~~~~~~~~~~~~~~~~\_Hardware_header_structure_/~~~~~~~~~~~~~~~~~~~~~~~~~
// Template for generation of headers of sending frames
 word NoFrameHeader [13]   = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
			      0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000};
 word ClusterHeader [13]   = {0x00F0, 0x00F0, 0x00F0, 0x00F0, 0x00F0, 0x00F0, 0x00F0,
			      0x00F0, 0x00F0, 0x00F0, 0x00F0, 0x00F0, 0x00F0};
 word FrameHeader [13]	   = {0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF,
			      0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF, 0x0FFF};
 word EmptyFrameHeader [13]= {0xF0F0, 0xF0F0, 0xF0F0, 0xF0F0, 0xF0F0, 0xF0F0, 0xF0F0,
			      0xF0F0, 0xF0F0, 0xF0F0, 0xF0F0, 0xF0F0, 0xF0F0};

// Template for decoding of headers of receiving frames
 byte bNoFrameHeader [13 * 2]	= {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				   0x00, 0x00};
 byte bClusterHeader [13 * 2]	= {0xF0, 0x00, 0xF0, 0x00, 0xF0, 0x00, 0xF0, 0x00,
				   0xF0, 0x00, 0xF0, 0x00, 0xF0, 0x00, 0xF0, 0x00,
				   0xF0, 0x00, 0xF0, 0x00, 0xF0, 0x00, 0xF0, 0x00,
				   0xF0, 0x00};
 byte bFrameHeader [13 * 2]	= {0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F,
				   0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F,
				   0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F,
				   0xFF, 0x0F};
 byte bEmptyFrameHeader [13 * 2]= {0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
				   0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
				   0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
				   0xF0, 0xF0};
//	   ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
//	   º Level Values !!!! º
//ÚÄÄÄÄÄÄÄÄÐÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÐÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
#define DIFF_LIMIT 4 // Max limit 4				 ³
#define LEVEL_VALUE (WArvFrame/2) // Max (WArvFrame/2)		 ³
//ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

int CheckOnTypeFrame (byte *String, byte *Example, int Type)
{
  dword Level = 0, DiffBit, Index;

  for (dword i = 0; i < WArvFrame; i++)
   {
    Index = i << 1;
    DiffBit  = NumBit [String [Index] ^ Example [Index]];
    DiffBit += NumBit [String [Index + 1] ^ Example [Index + 1]];
    Level   += DiffBit < DIFF_LIMIT ? 1:0;
   }

  if (Level < LEVEL_VALUE)
    return ID_NO_FRAME;
  else
    return Type;
}

//~~~~~~~~~~~~~~~~~~~~~~~~\_Hardware_header_routine_/~~~~~~~~~~~~~~~~~~~~~~~~~
// *String - pointer to string of frame header.
// Return: - iType of frame.
int GetTypeFrame (byte *String)
{
  dword Level = 0;

  // Summary bits per current string
  for (dword i = 0; i < WDataFrame; i++)
    Level += NumBit [String[i]];

  // Compare with level values
  if (Level > (WDataFrBit/4 - WDataFrBit/8)
   && Level < (WDataFrBit/4 + WDataFrBit/8))
     return CheckOnTypeFrame (String, bClusterHeader, ID_CLASTER);
  else if (Level > (WDataFrBit/2 + WDataFrBit/4 - WDataFrBit/8)
	&& Level < (WDataFrBit/2 + WDataFrBit/4 + WDataFrBit/8))
    return CheckOnTypeFrame (String, bFrameHeader, ID_FRAME);
  else if (Level > (WDataFrBit/2 - WDataFrBit/8)
	&& Level < (WDataFrBit/2 + WDataFrBit/8))
    return CheckOnTypeFrame (String, bEmptyFrameHeader, ID_EMPTY_FRAME);
  else
    return ID_NO_FRAME;
}

// Current subframe (block lenght 0x100 words) - main global value for this drivers
  dword SubFrame;
  byte SoftHdr [13 * 2 + 1];
  bool ResetSeqFlag = false;

#define HEADER_STEP 0x40

// Level 0 routine sending of hardware frame. Uses as syncronus as asyncronus
// routins. *InBuffer - buffer of logical frame, type is iType. :-)))
void SendFrame (word *InBuffer, int Type, frhdr *pFrHdr)
{
  // WArvFrame - 1 (?Š‘ˆ) words writen.
  dword CurWord = 0, NumWord, LastWord = WArvFrame - 1 - ResetSeqFlag;
  dword pSoftHdr = 0, SkipStr, DataIndex = 0, InsertIndex = HEADER_STEP;
  word *pHeader;
  ResetSeqFlag = false;

  // Make pointer to header of hardware frame
  switch (Type)
   {
    case     ID_CLASTER: pHeader = ClusterHeader;    break;
    case       ID_FRAME: pHeader = FrameHeader;      break;
    case ID_EMPTY_FRAME: pHeader = EmptyFrameHeader; break;
		default: pHeader = NoFrameHeader;    break;
   }

  outpw (RA, SubFrame++);
  SkipStr = HeadStr - 1; // -hardware header string

  if (pFrHdr)
    SkipStr -= 2 + 2; // -software header string = G + LowSpeed

  // Generate skip strings of hardware header of frame.
  while (SkipStr--)
   {
    CurWord += LastWord;

    while (LastWord--)
      outpw (RD_INC, 0);

    LastWord = WArvFrame;
   }

  // Generate string of hardware header of frame.
  CurWord += LastWord;

  while (LastWord--)
    outpw (RD_INC, pHeader [LastWord]);

  // Generate string of software header of frame.
  if (pFrHdr)
   {
    // Prepare 2 strings
    GaleyStringCoder (SoftHdr, (byte*)pFrHdr, sizeof (frhdr)/3);
    StringMixer (SoftHdr, WDataFrame);
   }

  // reload LastWord
  LastWord = SizeArvFrame - CurWord;

  while (LastWord)
   {
    if (LastWord > 0x100)
     {
      NumWord	= 0x100;
      LastWord -= 0x100 - CurWord;
     }
    else
     {
      NumWord  = LastWord;
      LastWord = 0;
     }

    // Warning: Header must be < 100h
    for (; CurWord < NumWord; CurWord++)
      //1 if empty frame
      //2 if not insert time
      //3 if end insert time
      if (!pFrHdr || DataIndex < InsertIndex || pSoftHdr >= WArvFrame << 1)
       {
	outpw (RD_INC, *InBuffer++);
	DataIndex++;
       }
      else
       {
	outpw (RD_INC, SpeedLowTabl [SoftHdr [pSoftHdr++]]);
	InsertIndex += HEADER_STEP;
       }

    outpw (RA, SubFrame++);
    CurWord = 0;
   }

  outpw (RK, ValueRK |= SET_INC_BUFFER); //increment frame count
  outpw (RK, ValueRK &= RESET_INC_BUFFER);
}

// ¥¦¨¬ § ¯¨á¨ ¤ ­­ëå ¢ ª àâã. ¨è¥¬ ¯® 100h á«®¢. (256w/512b)
// ¥¦¨¬ ­¥ ãáâ ­ ¢«¨¢ ¥¬, áç¨â ¥âáï çâ® ®­ ãáâ ­®¢«¥­.
// ¥à¥¤ § ¯¨áìî ãáâ ­ ¢«¨¢ ¥¬ ­®¬¥à ª ¤à  ¨§ t->RADDRES.
// Œ ªá¨¬ «ì­ë© à §¬¥à ¨­ä®à¬ æ¨®­­®£® ª ¤à  1000h ‘‹Ž‚
void SendIRCFrame (word *InBuffer)
{
  dword Size = 0x100;// - 1 - ResetSeqFlag;
  dword Len = SSIZE_IRC_FRAME/2;
  //InBuffer++;
  //InBuffer += ResetSeqFlag;
  ResetSeqFlag = false;

  while (Len)
   {
    outpw (RA, SubFrame++);	    // ãáâ ­®¢¨¬ ª ¤à

    if (Len < Size)
      Size = Len;		    // «¨è­¥¥ ­¥ ¯¨è¥¬

    // § ¯¨è¥¬ ªãá®ç¥ª
//#ifdef __GNUC__
//    outportsw (RD_INC, InBuffer, Size);
//    InBuffer += Size; 	      // ã¢¥«¨ç¨¬  ¤à¥á ¢ ¡ãä¥à¥
//#else
    for (dword i = 0; i < Size; i++)
      outpw (RD_INC, *InBuffer++);    // ã¢¥«¨ç¨¬  ¤à¥á ¢ ¡ãä¥à¥
//#endif

    Len -= Size;		    // ã¬¥­ìè¨¬ ¤«¨­ã ¯¨á ­¨ï
    // Size = 0x100;
   } // for

// ®á«¥ çâ¥­¨ï/§ ¯¨á¨ ¤ ­­ëå ­ ¤® ã¢¥«¨ç¨âì ­®¬¥à ¡ãä¥à  ª ¤à 
// ¯ãâ¥¬ ¢ë¤ ç¨ "1" ¢ ¡¨â 3 RK
  outpw (RK, ValueRK | SET_INC_BUFFER); //increment frame count
}

void ReceiveIRCFrame (word *OutBuffer)
{
  dword Size = 0x100;
  dword Len = SIZE_IRC_FRAME/2;

  while (Len)
   {
    outpw (RA, SubFrame++);
    inpw (RD); //Arvid have gluck - point 2 of hints - that work

    if (Len < Size)
      Size = Len;		    // «¨è­¥¥ ­¥ ¯¨è¥¬

    for (dword i = 0; i < Size; i++)
      *OutBuffer++ = inpw (RD_INC); // ã¢¥«¨ç¨¬  ¤à¥á ¢ ¡ãä¥à¥

    Len -= Size;		    // ã¬¥­ìè¨¬ ¤«¨­ã ¯¨á ­¨ï
   }

// ®á«¥ çâ¥­¨ï/§ ¯¨á¨ ¤ ­­ëå ­ ¤® ã¢¥«¨ç¨âì ­®¬¥à ¡ãä¥à  ª ¤à 
// ¯ãâ¥¬ ¢ë¤ ç¨ "1" ¢ ¡¨â 3 RK
  outpw (RK, ValueRK | SET_INC_BUFFER); //increment frame count
}

static inline word GetArvidFrameCount (void)
{
  word ArvStatus = inpw (RS1);
  return ArvStatus = ((ArvStatus & LOW_COUNT_CADR_MASK) | ((ArvStatus & HIGH_COUNT_CADR_MASK) >> 2)) & FrameMask;

/*
  register word first, second;

  do
   {
    first = inpw (RS1);

    // 1110 0111
    if (ArvidModelDetect >= 1051)
      first = ((first & LOW_COUNT_CADR_MASK) | ((first & HIGH_COUNT_CADR_MASK) >> 2)) & FrameMask;
    else
      first &= LOW_COUNT_CADR_MASK;

    second = inpw (RS1);

    if (ArvidModelDetect >= 1051)
      second = ((second & LOW_COUNT_CADR_MASK) | ((second & HIGH_COUNT_CADR_MASK) >> 2)) & FrameMask;
    else
      second &= LOW_COUNT_CADR_MASK;
   }
  while (first != second);

  return first;
*/
}

  word HookBuf [13 + 1];
  frhdr tmpFrHdr;

  vint LeftVariable, GlobalReceiveFrameCount = 0, GlobalRCFC = 0;
  vint DublicateClustersFrames = 0, RightReceivedClusterFrames = 0;

// Level 0 routine receive of hardware frame. Uses as syncronus as asyncronus
// routins. Receive in data frames buffer OutBuffer.
bool ReceiveFrame (word *OutBuffer, int Type, frhdr *pFrHdr)
{
  dword CurWord = 0, NumWord, LastWord, CurStr = 0;
  dword pSoftHdr = 0, DataIndex = 0, InsertIndex = HEADER_STEP;
  bool IdFrame = false;

  outpw (RA, SubFrame++);
  inpw (RD); //Arvid have gluck - point 2 of hints - that work
  GlobalReceiveFrameCount++;

  // Scan header strings of frame on frame.
  // Only last string may be descriptor of type header.
  while (CurStr < HeadStr)
   {
    LastWord = WArvFrame;

    while (LastWord--)
      HookBuf [LastWord] = inpw (RD_INC);

    CurWord += WArvFrame;

    // Two first strings - No0, No1 - have error rating 50%
    // CurStr > 1 &&
    if (GetTypeFrame ((byte*)HookBuf) == Type)
     {
      IdFrame = true;
      GlobalRCFC++;
      break;
     } // end if type = CLUSTER_ID
    CurStr++;
   }

  LastWord = SizeArvFrame - CurWord;

  while (LastWord)
   {
    if (LastWord > 0x100)
     {
      NumWord	= 0x100;
      LastWord -= 0x100 - CurWord;
     }
    else
     {
      NumWord  = LastWord;
      LastWord = 0;
     }

    // Warning: Header must be < 100h
    for (; CurWord < NumWord; CurWord++)
     if (!pFrHdr || DataIndex < InsertIndex || pSoftHdr >= WArvFrame << 1)
      {
       *OutBuffer++ = inpw (RD_INC);
       DataIndex++;
      }
     else
      {
       ((byte*)HookBuf) [pSoftHdr++] = SpeedHighTabl [inpw (RD_INC) & 0xAAAA] & 0xFF;
       InsertIndex += HEADER_STEP;
      }

    outpw (RA, SubFrame++);
    inpw (RD); //Arvid have gluck - point 2 of hints - that work
    CurWord = 0;
   }

  // Decode software frame header
  if (IdFrame && pFrHdr)
   {
    //brk ();
    StringDemixer ((byte*)HookBuf, WDataFrame);
    GaleyStringDecoder ((byte*)&tmpFrHdr, (byte*)HookBuf, sizeof (frhdr)/3);

    // Skip duplicate frame anti inserting/skipping error
    if (pFrHdr->FrameNumber
     && pFrHdr->FrameNumber == tmpFrHdr.FrameNumber
     && pFrHdr->ClusterNumber == tmpFrHdr.ClusterNumber)
      {
       DublicateClustersFrames++;
       IdFrame = false;
      }
     else
      {
       imemcpy (pFrHdr, &tmpFrHdr, sizeof (frhdr));
       RightReceivedClusterFrames++;
      }
   }

  outpw (RK, ValueRK |= SET_INC_BUFFER); //increment frame count
  outpw (RK, ValueRK &= RESET_INC_BUFFER);
  return IdFrame; // if type equial return true
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~\_Main_structure_/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Element of send table. Consist pointer to Semaphor object
typedef struct SendFrames
{
  word	*Buffer;
  frhdr *pFrHdr;
  vint	*pSemaphore;
  int	Type;
} SF;

// Element of receive table. Consist pointer to Semaphor object
typedef struct ReceiveFrames
{
  word	*Buffer;
  frhdr *pFrHdr;
  vint	*pSemaphore;
  int	Type;
} RF;

// Element of irc send table.
typedef struct IrcSendFrames
{
  word	*Buffer;
  vint	*pSemaphore;
} ISF;

  SF* TableSF	    = NULL; // Send frame table
  RF* TableRF	    = NULL; // Receive frame table
  ISF* TableIF	    = NULL; // Irc send frame table
  byte *DecBuffer   = NULL;
  ComElem *ComTable = NULL;

  int bTableSF, bTableRF, bTableIF; // pointers on begin stek FIFO
  int eTableSF, eTableRF, eTableIF; // pointers on end stek FIFO
  bool RComFlag, SComFlag, IComFlag;
  char ItoA_Table [] = "0123456789ABCDEF";

char *myitoa (dword Number, char *NumBuff)
{
  dword NumQuant = sizeof (dword) * 2, Mask = 0xF0000000;
  bool	BegNumber = false;

  for (dword i = 0; i < NumQuant; i++)
   {
    NumBuff [i] = (Number & Mask) >> ((7 - i) << 2);
    Mask >>=4;

    if (!BegNumber && !NumBuff [i])
      NumBuff [i] = ' ';
    else
     {
      NumBuff [i] = ItoA_Table [NumBuff [i]];
      BegNumber = true;
     }
   }

  NumBuff [NumQuant] = 'h';
  NumBuff [NumQuant + 1] = 0;
  return NumBuff;
}

char  NumBuff [20];
dword IRQCount = 0, LuncherFlag = 1, ClusterDetectCount = 0, InHandlerFlag = 0;
dword LastClustersFrames = 0, CastClusterCount = 0, NumFreeSemaphor = 0, ImportCount = 0;
bool  StartFlag = true, rc;

static byte pVRAM = 0;
static inline void PrintIRQIndicator ()
{
  PutChar (0xFF & IRQCount++, 7, pVRAM, 0);

  if (pVRAM < 80)
    pVRAM++;
  else
    pVRAM = 0;
}

#define BASE_DEB_STR 2
#define BASE_DEB_COL 70
#define DEBUG_COLOR  0x1A

int icrc32 = 0, ircframes = 0;

void IntHandler ()
{
  PrintIRQIndicator ();
  PutString (myitoa (IRQCount, NumBuff), DEBUG_COLOR, BASE_DEB_COL, BASE_DEB_STR);
  //Monitoring of cluster decoding
  //PutString (myitoa (NumFreeSemaphor, NumBuff), DEBUG_COLOR, BASE_DEB_COL, BASE_DEB_STR + 1);

  // IRC Command
  if (bTableIF != eTableIF && !SComFlag)
   {
    // Begin new irc send command
    if (!IComFlag)
     {
      // End command be receive -> set ArVid on transmit & subframe count;
      if (ValueRK & SET_RECIVE || LuncherFlag)
       {
	SubFrame = LuncherFlag = 0;

	// Connect PC to VM, reset seq, set operate "data", set transmit
	//outpw (RK, ValueRK = (((ValueRK | LINK_VM2PC | SET_RESET_SEQ | Arv105X ) & SET_200K) & SET_TRANSMIT) & SET_IR);
	outpw (RK, ValueRK = LINK_VM2PC);
	outpw (RK, ValueRK |= SET_RESET_SEQ);
	//outpw (RK, ValueRK &= RESET_RESET_SEQ);
	ValueRK &= RESET_RESET_SEQ;

	ResetSeqFlag = true; // This flag used in send irc frame
       }

      IComFlag = true;
      StartFlag = false; // Reset StartFlag for begin waiting finish of operations
     }

    ImportCount = 0;
    // If is cache have a memory for sending frame
    // while ((GetArvidFrameCount () < NumArvFrame - 1)
    while ((GetArvidFrameCount () < 8 - 1)
	    && *TableIF [bTableIF].pSemaphore
	    && ImportCount <= 4)
     {
      SendIRCFrame (TableIF [bTableIF].Buffer);

      for (int i = 0; i < SSIZE_IRC_FRAME; i++)
	icrc32 += *((byte*)((int)TableIF [bTableIF].Buffer + i));

      // Set on limit of frame
      SubFrame += (4096 >> 8) - ((SSIZE_IRC_FRAME/2 + 0xFF) >> 8);

      //if (SubFrame >= (4096 >> 8) * NumArvFrame)
      if (SubFrame >= (4096 >> 8) * 8)
	SubFrame = 0;

      ircframes++;
      PutString (myitoa (icrc32, NumBuff), DEBUG_COLOR, BASE_DEB_COL, BASE_DEB_STR + 1);
      PutString (myitoa (ircframes, NumBuff), DEBUG_COLOR, BASE_DEB_COL, BASE_DEB_STR + 2);
      
      TableIF [bTableIF].Buffer = (word*)((int)TableIF [bTableIF].Buffer + SSIZE_IRC_FRAME);
      (*TableIF [bTableIF].pSemaphore)--;
      ImportCount++;
     }

    // Check last sector count during command. If not zero, then send frame
    if (!(*TableIF [bTableIF].pSemaphore))
     {
      // End this send operation, enable deallocate semafor.
      // May free memory buffer, if need.
      // Inform client servis about ending operations
      *TableIF [bTableIF].pSemaphore = END_SEND_COM;

      // Move on next command in stack
      if (bTableIF != MAX_AS_COM)
	bTableIF++;
      else
	bTableIF = 0;

      icrc32 = 0;
     }

    return; //end IRC
   }

  // Do receive
  if (bTableRF != eTableRF && !SComFlag)
   {
    // Begin new receive command
    if (!RComFlag)
     {
      // Waiting free ArVid cache & going in to receive against last sendining frames
      // End command be send -> reset ArVid & subframe count; if receive save cache frame
      if ((!(ValueRK & SET_RECIVE) || LuncherFlag) && !GetArvidFrameCount ())
       {
	SubFrame = LuncherFlag = 0;
	// Connect PC to VM, reset seq, set operate "data", set receive
	outpw (RK, ValueRK = ValueRK | SET_DATA | LINK_VM2PC | SET_RESET_SEQ
	  | SET_RECIVE | AutoPhase | Phase | SubPhase | HDensity | Arv105X);
	outpw (RK, ValueRK &= RESET_RESET_SEQ);
       }
      else
	return; // Cashe have a no transmit frames

      RComFlag = true;
      StartFlag = false; // Reset StartFlag for begin waiting finish of operations
     }

    // If is receiving frame in cache and have free buffer
    while (GetArvidFrameCount ()
      && *TableRF [bTableRF].pSemaphore)
     {
      rc = ReceiveFrame (TableRF [bTableRF].Buffer,
			 TableRF [bTableRF].Type,
			 TableRF [bTableRF].pFrHdr);

      // Set on limit of frame
      SubFrame += (4096 >> 8) - ((SizeArvFrame + 0xFF) >> 8) - 1;

      if (SubFrame >= (4096 >> 8) * NumArvFrame)
	SubFrame = 0;

      if ((rc && !TableRF [bTableRF].pFrHdr)
       || (rc && TableRF [bTableRF].pFrHdr
       && ((!TableRF [bTableRF].pFrHdr->FrameNumber && *TableRF [bTableRF].pSemaphore == CODE_LEN)
       ||   (TableRF [bTableRF].pFrHdr->FrameNumber && *TableRF [bTableRF].pSemaphore < CODE_LEN)) ))
       {
	if (!TableRF [bTableRF].pFrHdr->FrameNumber)
	 {
	  PutString (myitoa (TableRF [bTableRF].pFrHdr->ClusterNumber, NumBuff),
		     DEBUG_COLOR, BASE_DEB_COL, BASE_DEB_STR + 2 + (ClusterDetectCount << 1));
	  PutString (myitoa (ClusterDetectCount, NumBuff), DEBUG_COLOR, BASE_DEB_COL,
		     BASE_DEB_STR + 3 + ((ClusterDetectCount - 1) << 1));
	 }

	TableRF [bTableRF].Buffer = (word*)((int)TableRF [bTableRF].Buffer + SizeDataFrame);
	(*TableRF [bTableRF].pSemaphore)--;
       }
      else
      if (rc
       &&  TableRF [bTableRF].pFrHdr
       && !TableRF [bTableRF].pFrHdr->FrameNumber
       && *TableRF [bTableRF].pSemaphore < CODE_LEN)
	{ // If no construct cluster go to next cluster
	 //LeftVariable = TableRF [bTableRF].pFrHdr->FrameNumber;
	 CastClusterCount++;
	 LastClustersFrames += *TableRF [bTableRF].pSemaphore;
	 // Inform client servis about ending operations
	 word *tmppBuff = TableRF [bTableRF].Buffer;
	 *TableRF [bTableRF].pSemaphore = END_REC_COM;

	 // Preserve received first frame of cluster
	 // Move on next command in stack
	 if (bTableRF == MAX_AS_COM)
	   bTableRF = 0;
	 else
	   bTableRF++;

	 // Check on empty stack
	 if (bTableRF == eTableRF)
	   RComFlag = false;
	 else
	  {
	   if (*TableRF [bTableRF].pSemaphore == CODE_LEN && TableRF [bTableRF].Buffer)
	    {
	     imemcpy (TableRF [bTableRF].Buffer, tmppBuff, SizeDataFrame);
	     TableRF [bTableRF].Buffer = (word*)((int)TableRF [bTableRF].Buffer + SizeDataFrame);
	     (*TableRF [bTableRF].pSemaphore)--;
	    }
	  }

	} // end if destruct cluster
     } // end while

    // Check last sector count during command.
    // If not zero, then receive frame
    if (!(*TableRF [bTableRF].pSemaphore))
     {
      // End this receive operation [bTableRF], may free semaphor.
      // May free memory buffer, if need.
      // Inform client servis about ending operations
      *TableRF [bTableRF].pSemaphore = END_REC_COM;
      // Move on next command in stack
      if (bTableRF == MAX_AS_COM)
	bTableRF = 0;
      else
	bTableRF++;

      // Check on empty stack
      if (bTableRF == eTableRF)
	RComFlag = false;
     }
   }

  // Send ¢ë§®¢¥âáï â®«ìª® â®£¤ , ª®£¤  ®ç¨áâ¨âìáï ¢áï ®ç¥à¥¤ì ¯à¨¥¬ 
  // Do send
  if (bTableSF != eTableSF && !RComFlag)
   {
    // Begin new send command
    if (!SComFlag)
     {
      // End command be receive -> set ArVid on transmit & subframe count;
      if (ValueRK & SET_RECIVE || LuncherFlag)
       {
	SubFrame = LuncherFlag = 0;

	// Connect PC to VM, reset seq, set operate "data", set transmit
	outpw (RK, ValueRK = (ValueRK | SET_DATA | LINK_VM2PC |
	SET_RESET_SEQ | Arv105X | HDensity) & SET_TRANSMIT);
	outpw (RK, ValueRK &= RESET_RESET_SEQ);

	ResetSeqFlag = true;
       }

      SComFlag = true;
      StartFlag = false; // Reset StartFlag for begin waiting finish of operations
     }

    // If is cache have a memory for sending frame
    while ((GetArvidFrameCount () < NumArvFrame - 1)
	    && *TableSF [bTableSF].pSemaphore)
     {
      SendFrame (TableSF [bTableSF].Buffer,
		 TableSF [bTableSF].Type,
		 TableSF [bTableSF].pFrHdr);

      if (TableSF [bTableSF].pFrHdr)
	TableSF [bTableSF].pFrHdr->FrameNumber++;

      // Set on limit of frame
      SubFrame += (4096 >> 8) - ((SizeArvFrame + 0xFF) >> 8) - 1;

      if (SubFrame >= (4096 >> 8) * NumArvFrame)
	SubFrame = 0;

      TableSF [bTableSF].Buffer = (word*)((int)TableSF [bTableSF].Buffer + SizeDataFrame);
      (*TableSF [bTableSF].pSemaphore)--;
     }

    // Check last sector count during command. If not zero, then send frame
    if (!(*TableSF [bTableSF].pSemaphore))
     {
      // End this send operation, enable deallocate semafor.
      // May free memory buffer, if need.
      // Inform client servis about ending operations
      *TableSF [bTableSF].pSemaphore = END_SEND_COM;

      // Move on next command in stack
      if (bTableSF != MAX_AS_COM)
	bTableSF++;
      else
	bTableSF = 0;
     }
    return;
   } // end if sequential not empty

  // Check on empty stack
  if (bTableSF == eTableSF
      && SComFlag
      && !GetArvidFrameCount ())
   {
    SComFlag = false;
    return;
   }

  // Check on empty stack
  if (bTableIF == eTableIF
      && IComFlag
      && !GetArvidFrameCount ())
   {
    IComFlag = false;
    return;
   }

  //Waiting free ArVid cache & going in to receive against dublicate transmit frame
  if (!(ValueRK & SET_RECIVE) && !GetArvidFrameCount ())
   {
    LuncherFlag = 1;
    // Connect PC to VM, reset seq, set operate "data", set receive
    outpw (RK, ValueRK = ValueRK | SET_DATA | LINK_VM2PC | SET_RESET_SEQ
      | SET_RECIVE | AutoPhase | Phase | SubPhase | HDensity | Arv105X);
   }
}

//~~~~~~~~~~~~~~~~~~~~~~~\_INTERRUPT_ON_FRONT_FRAME_/~~~~~~~~~~~~~~~~~~~~~~~~~
static __dpmi_paddr OldIntHandler = {NULL, NULL}, MyIntHandler = {NULL, NULL};

void interrupt IntRoutine ()
{
  if (!InHandlerFlag)
   {
    InHandlerFlag++;
    IntHandler ();
    InHandlerFlag--;
   }
  else
    brk ();

  outp (0x20, 0x20);
  outp (0xA0, 0x20);
}

void EndTAVDLockedCode () {}

word *SynBuffer = NULL;
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍSyncronusÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
// Level 2 syncronus driver init
bool InitSyncroCommand (void)
{
  InitGaleyTable ();
  InitManchTable ();
  SynBuffer = (word*) new byte [SizeGaleyCluster];

  if (!SynBuffer)
    return FALSE; // No free memory
  else
    return TRUE;
}

void FreeSyncroCommand (void)
{
  if (SynBuffer)
    delete SynBuffer;

  FreeManchTable ();
  FreeGaleyTable ();
}

void InitTransmit ()
{
  // wait for flush ArVid cache...
  if (!(ValueRK & SET_RECIVE))
    while (GetArvidFrameCount ()) {};

  SubFrame = 0;
  //connect PC to VM, reset seq, set operate "data"
  outpw (RK, ValueRK = (ValueRK | SET_DATA | LINK_VM2PC |
  SET_RESET_SEQ | Arv105X | HDensity) & SET_TRANSMIT);
  outpw (RK, ValueRK &= RESET_RESET_SEQ);
  ResetSeqFlag = true;
}

void InitReceive ()
{
  // wait for flush ArVid cache...
  if (!(ValueRK & SET_RECIVE))
    while (GetArvidFrameCount ()) {};

  // Connect PC to VM, reset seq, set operate "data", set receive
  outpw (RK, ValueRK = ValueRK | SET_DATA | LINK_VM2PC | SET_RESET_SEQ
    | SET_RECIVE | AutoPhase | Phase | SubPhase | HDensity | Arv105X);
}

// Receive syncrously Num frames in data frames buffer OutBuffer,
// type iType
static void ReceiveSFrames (int Num, word *OutBuffer, int Type, frhdr *pFrHdr)
{
  if (!OutBuffer)
    return;

  int  CountFrame = 1;
  char Numbuff[20];

  SubFrame = 0;
  //connect PC to VM, reset seq, set operate "data"
  outpw (RK, ValueRK = ValueRK | SET_DATA | LINK_VM2PC | SET_RESET_SEQ
    | SET_RECIVE | AutoPhase | Phase | SubPhase | HDensity | Arv105X);
  outpw (RK, ValueRK &= RESET_RESET_SEQ);

  printf ("Receive frames:\n");

  while (Num)
   {
    while (!GetArvidFrameCount ()) {};

    bool rc = ReceiveFrame (OutBuffer, Type, pFrHdr);

    SubFrame += (4096 >> 8) - ((SizeArvFrame + 0xFF) >> 8) - 1; //set on limit of frame

    if (SubFrame >= (4096 >> 8) * NumArvFrame)
      SubFrame = 0;

    if ((rc && !pFrHdr)
       || (rc && pFrHdr
       && ((!pFrHdr->FrameNumber && Num == CODE_LEN)
       ||   (pFrHdr->FrameNumber && Num < CODE_LEN)) ))
	OutBuffer = (word*)((int)OutBuffer + SizeDataFrame);
      else // If no construct cluster go to next cluster
      if (rc && pFrHdr
       && !pFrHdr->FrameNumber
       && Num < CODE_LEN)
	{
	 brk ();
	 Num = 0;
	}

    printf ("%s\r", itoa (CountFrame++, Numbuff, 10));
    fflush (stdout);
    Num--;
   }
  printf ("\n");
}

// Send block of frames syncrously. InBuffer pointer to data frames buffer.
static void SendSFrames (int Num, word *InBuffer, int Type, frhdr *pFrHdr)
{
  int  CountFrame = 1;
  printf ("Send frames: ");

  while (Num--)
   {
    // wait for flush ArVid cache...
    while (GetArvidFrameCount () == NumArvFrame - 1) {};

    SendFrame (InBuffer, Type, pFrHdr);
    InBuffer = (word*)((int)InBuffer + SizeDataFrame);

    if (pFrHdr)
      pFrHdr->FrameNumber++;

    SubFrame += (4096 >> 8) - ((SizeArvFrame + 0xFF) >> 8) - 1; //set on limit of frame

    if (SubFrame == (4096 >> 8) * NumArvFrame)
      SubFrame = 0;

    printf ("%d ", CountFrame++);
    fflush (stdout);
   }

  printf ("\n");
  printf ("SubFrame = %d\n", SubFrame);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~\_Level_2_drivers_/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Level 2 syncronus send driver - with error correction
dword SendSCluster (dword NumCluster, byte *OutBuffer, dword ClusterID)
{
  InitTransmit ();

  dword i;
  for (i = 0; i < NumCluster; i++)
   {
    frhdr FrHdr;
    FrHdr.ClusterNumber = ClusterID + NumCluster;
    FrHdr.FrameNumber = 0;

    CodeGaleyCluster ((byte*)SynBuffer, OutBuffer);
    SendSFrames (CODE_LEN, SynBuffer, ID_CLASTER, &FrHdr);
   }

  InitReceive ();
  return i;
}

 estat ErrStat;

// Level 2 syncronus receive driver - with error correction
static dword ReceiveSCluster (dword NumCluster, byte *InBuffer)
{
  dword i;
  for (i = 0 ;i < NumCluster; i++)
   {
    frhdr FrHdr;
    imemset (&FrHdr, (byte)(-1), sizeof (frhdr));

    ReceiveSFrames (CODE_LEN, SynBuffer, ID_CLASTER, &FrHdr);
    DecodeGaleyCluster (InBuffer, (byte*)SynBuffer);
   }

  return i;
}

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍAsyncronusÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
//~~~~~~~~~~~~~~~~~~~~~~~~~~~\_Servis_routines_/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void FreeSemaphor (int i)
{
  // Clear free buffer
  if (ComTable [i].Buffer)
   {
    delete ComTable [i].Buffer;
    LockMemory (ComTable [i].Buffer, ComTable [i].Size, UNLOCK_MEMORY);
   }

  ComTable [i].Buffer = NULL;

  if (ComTable [i].pFrHdr)
   {
    delete ComTable [i].pFrHdr;
    LockMemory (ComTable [i].pFrHdr, sizeof (frhdr), UNLOCK_MEMORY);
   }

  ComTable [i].pFrHdr = NULL;
  ComTable [i].Semaphore = FREE_SEMAPHOR;
}

void DecodeANDFreeSemaphor (void)
{
  for (int i = 0; i < MAX_AS_COM; i++)
   {
    if (ComTable [i].Type == ID_IRC &&
	ComTable [i].Semaphore == END_SEND_COM)
     {
      FreeSemaphor (i);
      continue;
     }

    if (ComTable [i].Type != ID_CLASTER)
      continue;

    switch (ComTable [i].Semaphore)
     { // Clear free buffer
      case END_SEND_COM:
       {
	FreeSemaphor (i);
	break;
       }
      case END_REC_COM:
       {
	if (ControlCodeEnable)
	 {
	  if (!ErrAudit)
	    DecodeGaleyCluster (DecBuffer, (byte*)ComTable [i].Buffer); // Decode to buffer
	  else // Decode to buffer
	    DecodeGaleyClusterWithErrStat (DecBuffer, (byte*)ComTable [i].Buffer, &ComTable [i].ES);

	  imemcpy (ComTable [i].Buffer, DecBuffer, SizeOrigCluster); // Update receive buffer
	 }

	ComTable [i].Semaphore = END_DEC_COM;
	NumFreeSemaphor++;
	break;
       }
     }
   }
}

int FindEndDecodeCommand (void)
{
  DecodeANDFreeSemaphor ();

  for (int i = 0; i < MAX_AS_COM; i++)
   {
    if (ComTable [i].Type != ID_CLASTER)
      continue;

    if (ComTable [i].Semaphore == END_DEC_COM)
      return i;
   }

  return ER_NO_END_DEC_COM;
}

int FindFreeSemaphor (void)
{
  for (int i = 0; i < MAX_AS_COM; i++)
   {
    switch (ComTable [i].Semaphore)
     {
      case END_REC_COM:
       {
	if (ComTable [i].Type != ID_CLASTER)
	 {
	  FreeSemaphor (i);
	  return i;
	 }
	break;
       }
      case END_SEND_COM:
       {
	FreeSemaphor (i);
	return i;
       }
      case FREE_SEMAPHOR:
	return i;
     }
   }

  return ER_NO_FREE_SEMAPHOR; //Not found free Semaphore
}

bool CheckOnFreeComBuffer (void)
{
  DecodeANDFreeSemaphor ();
  byte KeyCode = inp (0x60);

  if (KeyCode == 0xE0 || KeyCode == 0xE1)
    KeyCode = inp (0x60);

  KeyCode &= 0x7F;
  if (KeyCode == 1 || KeyCode == 0x39)
   {
    StopByUser = true;
    _bios_keybrd (_KEYBRD_READ);
   }

  for (int i = 0; i < MAX_AS_COM; i++)
    if (ComTable [i].Semaphore != FREE_SEMAPHOR)
      return false;

  return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~\_Level_1_drivers_/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Level 1 driver asyncronus send frames,
// untill call command *Semaphore = NumFrames;
bool SendAFrames (word *OutBuffer, vint *pSemaphore, int Type, frhdr *pFrHdr)
{
  int TMPeTableSF = eTableSF;

// Sequential control of stack FIFO
  if (TMPeTableSF == MAX_AS_COM)
    TMPeTableSF = 0;
  else
    TMPeTableSF++;

// Check overflow stack SendFrames command
  if (TMPeTableSF > bTableSF)
   {
    // Overflow stack, end go to head
    if (TMPeTableSF - bTableSF == MAX_AS_COM - 1)
     {
      brk ();
      return false;
     }
   }
  else
   {
    // Overflow stack, head go to end
    if (bTableSF - TMPeTableSF == 1)
     {
      brk ();
      return false;
     }
   }

// Make command in seq of sending frames
  TableSF [eTableSF].Type	= Type;
  TableSF [eTableSF].Buffer	= OutBuffer;
  TableSF [eTableSF].pFrHdr	= pFrHdr;
  TableSF [eTableSF].pSemaphore = pSemaphore;
  eTableSF = TMPeTableSF;

  return true;
}

bool SendAIFrames (word *OutBuffer, vint *pSemaphore)
{
  int TMPeTableIF = eTableIF;

  // Sequential control of stack FIFO
  if (TMPeTableIF == MAX_AS_COM)
    TMPeTableIF = 0;
  else
    TMPeTableIF++;

  // Check overflow stack SendFrames command
  if (TMPeTableIF > bTableIF)
   {
    // Overflow stack, end go to head
    if (TMPeTableIF - bTableIF == MAX_AS_COM - 1)
     {
      brk ();
      return false;
     }
   }
  else
   {
    // Overflow stack, head go to end
    if (bTableIF - TMPeTableIF == 1)
     {
      brk ();
      return false;
     }
   }

  // Make command in seq of sending frames
  TableIF [eTableIF].Buffer	= OutBuffer;
  TableIF [eTableIF].pSemaphore = pSemaphore;
  eTableIF = TMPeTableIF;

  return true;
}

// Level 1 driver asyncronus receive frames,
// untill call command *Semaphore = NumFrames;
bool ReceiveAFrames (word *InBuffer, vint *pSemaphore, int Type, frhdr *pFrHdr)
{
  int TMPeTableRF = eTableRF;

  if (TMPeTableRF == MAX_AS_COM)
    TMPeTableRF = 0;
  else
    TMPeTableRF++;

// Check overflow stack Receive Frames command
  if (TMPeTableRF > bTableRF)
   {
    // Overflow stek, end go to head
    if (TMPeTableRF - bTableRF == MAX_AS_COM - 1)
      return false;
   }
  else
   {
    // Overflow stek, head go to end
    if (bTableRF - TMPeTableRF == 1)
      return false;
   }

// Make command in seq of receiving frames
  TableRF [eTableRF].Type	= Type;
  TableRF [eTableRF].Buffer	= InBuffer;
  TableRF [eTableRF].pFrHdr	= pFrHdr;
  TableRF [eTableRF].pSemaphore = pSemaphore;
  eTableRF = TMPeTableRF;

  return true;
}

// Level 2 asyncronus send driver - with error correction
int SendACluster (dword NumCluster, byte *OutBuffer, dword ClusterID)
{
  dword CountCluster = 0;

  while (NumCluster--)
   {
    DecodeANDFreeSemaphor ();

    int NumSemaphor;
    if ((NumSemaphor = FindFreeSemaphor ()) == ER_NO_FREE_SEMAPHOR)
      return ER_NO_FREE_SEMAPHOR;

    ComTable [NumSemaphor].Buffer = (word*) new byte [SizeGaleyCluster];
    ComTable [NumSemaphor].pFrHdr = new frhdr;

    if (!ComTable [NumSemaphor].Buffer
     || !ComTable [NumSemaphor].pFrHdr)
      return CountCluster; // No free memory, please wait..., or free... ;-)))

    LockMemory (ComTable [NumSemaphor].Buffer, SizeGaleyCluster, LOCK_MEMORY);
    LockMemory (ComTable [NumSemaphor].pFrHdr, sizeof (frhdr), LOCK_MEMORY);

    if (ControlCodeEnable)
     {
      CodeGaleyCluster ((byte*)ComTable [NumSemaphor].Buffer, OutBuffer);
//	imemcpy (Buffer, ComTable [NumSemaphor].Buffer, SizeOrigCluster);  // debug
     }
    else
     {
      imemset (ComTable [NumSemaphor].Buffer, 0, SizeGaleyCluster);
      imemcpy (ComTable [NumSemaphor].Buffer, OutBuffer, SizeOrigCluster);
     }

    ComTable [NumSemaphor].pFrHdr->ClusterNumber = ClusterID + CountCluster;
    ComTable [NumSemaphor].pFrHdr->FrameNumber = 0;
    ComTable [NumSemaphor].Type = ID_CLASTER;
    ComTable [NumSemaphor].Size = SizeGaleyCluster;

    // Send command
    ComTable [NumSemaphor].Semaphore = CODE_LEN;

    if (SendAFrames (ComTable [NumSemaphor].Buffer,
		    &ComTable [NumSemaphor].Semaphore,
		     ComTable [NumSemaphor].Type,
		     ComTable [NumSemaphor].pFrHdr))
      CountCluster++;
    else
      return CountCluster;

    OutBuffer += SizeOrigCluster;
   }

  return CountCluster;
}

char num [4] = "0";
int pnum = 0;

// Level 2 asyncronus send driver
int SendIRCCommand (byte *OutBuffer, dword Size)
{
  DecodeANDFreeSemaphor ();

  int NumSemaphor;
  if ((NumSemaphor = FindFreeSemaphor ()) == ER_NO_FREE_SEMAPHOR)
    return ER_NO_FREE_SEMAPHOR;

  dword moreframe = 0;
  if (Size % SSIZE_IRC_FRAME)
    moreframe++;

  dword FullSize = (Size / SSIZE_IRC_FRAME + moreframe) * SSIZE_IRC_FRAME;
  ComTable [NumSemaphor].Buffer = (word*) new byte [FullSize];
  //printf ("Size: %d, FullSize: %d, Semaphore: %d\n", Size, FullSize, FullSize/SSIZE_IRC_FRAME);

  if (!ComTable [NumSemaphor].Buffer)
    return -1; // No free memory, please wait..., or free... ;-)))

  LockMemory (ComTable [NumSemaphor].Buffer, FullSize, LOCK_MEMORY);
  imemset (ComTable [NumSemaphor].Buffer, 0, FullSize);
  imemcpy (ComTable [NumSemaphor].Buffer, OutBuffer, Size);

/*
  int hIRC = -1;
  char filename [60] = "IRCDmp";
  strcat (filename, num);
  strcat (filename, ".bin");
  unlink (filename);
  hIRC = open (filename, O_CREAT | O_BINARY | O_RDWR, FILE_ACCESS);
  num [pnum]++;

  if (num [pnum] == '9')
   {
    num [pnum] = '1';
    pnum++;
    num [pnum] = '0';
    num [pnum + 1] = 0;
   }

  if (hIRC != -1)
   {
    write (hIRC, ComTable [NumSemaphor].Buffer, Size);
    close (hIRC);
   }
*/

  ComTable [NumSemaphor].Type = ID_IRC;
  ComTable [NumSemaphor].Size = FullSize;

  // Send command
  ComTable [NumSemaphor].Semaphore = FullSize/SSIZE_IRC_FRAME;
  printf ("Semaphore %d, FullSize %d, SSIZE_IRC_FRAME %d\n", ComTable [NumSemaphor].Semaphore, FullSize, SSIZE_IRC_FRAME);

  if (!SendAIFrames (ComTable [NumSemaphor].Buffer,
		    &ComTable [NumSemaphor].Semaphore))
    return -1;
  else
    return Size;
}

int SendIRCSCommand (byte *OutBuffer, dword Size)
{
  word* FrameBuff = new word [SSIZE_IRC_FRAME/2];

  SubFrame = 0;
  // Connect PC to VM, reset seq, set operate "data", set transmit
  //outpw (RK, ValueRK = (((ValueRK | LINK_VM2PC | SET_RESET_SEQ | Arv105X ) & SET_200K) & SET_TRANSMIT) & SET_IR);
  outpw (RK, ValueRK = LINK_VM2PC);
  outpw (RK, ValueRK |= SET_RESET_SEQ);
  //outpw (RK, ValueRK &= RESET_RESET_SEQ);
  ValueRK &= RESET_RESET_SEQ;

  ResetSeqFlag = true; // This flag used in send irc frame

  printf ("Send frames:");
  while (Size)
   {
    imemset (FrameBuff, 0, SSIZE_IRC_FRAME);

    bool WaitTrigger = true;
    while (GetArvidFrameCount () >= 8 - 1)
     {
      if (WaitTrigger)
       {
	//printf ("%d", GetArvidFrameCount ());
	WaitTrigger = false;
       }
     }

    dword Len = SSIZE_IRC_FRAME;
    if (Size < SSIZE_IRC_FRAME)
     {
      Len = Size;
      Size = 0;
     }
    else
      Size -= SSIZE_IRC_FRAME;

    imemcpy (FrameBuff, OutBuffer, Len);
    OutBuffer += SSIZE_IRC_FRAME;
    SendIRCFrame (FrameBuff);
    printf (".");

    // Set on limit of frame
    SubFrame += (4096 >> 8) - ((SSIZE_IRC_FRAME/2 + 0xFF) >> 8);

    //if (SubFrame >= (4096 >> 8) * NumArvFrame)
    if (SubFrame >= (4096 >> 8) * 8)
      SubFrame = 0;
    printf ("%X", SubFrame);
   }
  printf ("\n");
  delete FrameBuff;

  printf ("Wait flush arvid buffer:\n");

  int count = 0;
  while (GetArvidFrameCount ())
   {
     int tcount = count;
     while (tcount--) printf (".");
     tcount = 0x3f - count;
     while (tcount--) printf (" ");
     printf ("\r");

     count++;
     count &= 0x3f;
   }
  printf ("\nDone.\n");
  return Size;
}

int ReceiveIRCSCommand (int hCom, dword NumFrames)
{
  word* FrameBuff = new word [SIZE_IRC_FRAME/2];

  SubFrame = 0;
  // Connect PC to VM, reset seq, set operate "data", set transmit
  //outpw (RK, ValueRK = (((ValueRK | LINK_VM2PC | SET_RESET_SEQ | Arv105X ) & SET_200K) & SET_TRANSMIT) & SET_IR);
  outpw (RK, ValueRK = LINK_VM2PC | SET_RECIVE);
  outpw (RK, ValueRK |= SET_RESET_SEQ);
  //outpw (RK, ValueRK &= RESET_RESET_SEQ);
  ValueRK &= RESET_RESET_SEQ;
  ResetSeqFlag = true; // This flag used in send irc frame
  bool FoundIRData = false;

  printf ("Set IRC receive mode - wait data.\n");
  while (!FoundIRData)
   {
    imemset (FrameBuff, 0, SIZE_IRC_FRAME);
    while (GetArvidFrameCount () == 0) {}
    ReceiveIRCFrame (FrameBuff);

    // Set on limit of frame
    SubFrame += (4096 >> 8) - ((SIZE_IRC_FRAME/2 + 0xFF) >> 8);

    //if (SubFrame >= (4096 >> 8) * NumArvFrame)
    if (SubFrame >= (4096 >> 8) * 8)
      SubFrame = 0;

    int crc32 = 0;
    for (dword i = 0; i < SIZE_IRC_FRAME; i++)
      crc32 += *((byte*)((int)FrameBuff + i));

    if (crc32 > 0)
     {
      FoundIRData = true;
      write (hCom, FrameBuff, SIZE_IRC_FRAME);
     }
   }
  printf ("Found IRC Data - receive %d frames", NumFrames);

  while (NumFrames--)
   {
    imemset (FrameBuff, 0, SIZE_IRC_FRAME);
    while (GetArvidFrameCount () == 0) {}
    ReceiveIRCFrame (FrameBuff);

    // Set on limit of frame
    SubFrame += (4096 >> 8) - ((SIZE_IRC_FRAME/2 + 0xFF) >> 8);

    //if (SubFrame >= (4096 >> 8) * NumArvFrame)
    if (SubFrame >= (4096 >> 8) * 8)
      SubFrame = 0;

    write (hCom, FrameBuff, SIZE_IRC_FRAME);
    printf (".");
    printf ("%X", SubFrame);
   }
  printf ("\nDone.\n");

  delete FrameBuff;
  return true;
}

// Level 2 asyncronus receive driver - with error correction
int ReceiveACluster (int NumCluster)
{
  int CountCluster = 0;

  while (NumCluster--)
   {
    DecodeANDFreeSemaphor ();

    int NumSemaphor;
    if ((NumSemaphor = FindFreeSemaphor ()) == ER_NO_FREE_SEMAPHOR)
      return ER_NO_FREE_SEMAPHOR;

    ComTable [NumSemaphor].Buffer = (word*) new byte [SizeGaleyCluster];
    ComTable [NumSemaphor].pFrHdr = new frhdr;

    if (!ComTable [NumSemaphor].Buffer
     || !ComTable [NumSemaphor].pFrHdr)
      return ER_NO_FREE_MEM; // No free memory, please wait..., or free... ;-)))

    LockMemory (ComTable [NumSemaphor].Buffer, SizeGaleyCluster, LOCK_MEMORY);
    LockMemory (ComTable [NumSemaphor].pFrHdr, sizeof (frhdr), LOCK_MEMORY);
    imemset (ComTable [NumSemaphor].pFrHdr, (byte)(-1), sizeof (frhdr));
    // Receive command
    ComTable [NumSemaphor].Type = ID_CLASTER;
    ComTable [NumSemaphor].Size = SizeGaleyCluster;
    ComTable [NumSemaphor].Semaphore = CODE_LEN;

    if (ReceiveAFrames (ComTable [NumSemaphor].Buffer,
		       &ComTable [NumSemaphor].Semaphore,
			ComTable [NumSemaphor].Type,
			ComTable [NumSemaphor].pFrHdr))
      CountCluster++;
    else
      return CountCluster;
   }

  return CountCluster;
}

void InitAsynCommand (void)
{
  InitGaleyTable ();
  InitManchTable ();

  if (!ArvidInsert)
    return;

  TableSF = new SF [MAX_AS_COM];
  TableRF = new RF [MAX_AS_COM];
  TableIF = new ISF [MAX_AS_COM];

  if (!TableSF || !TableRF || !TableIF)
    ErrorExit (1);

  LockMemory (TableSF, sizeof (SF) * MAX_AS_COM, LOCK_MEMORY);
  LockMemory (TableRF, sizeof (RF) * MAX_AS_COM, LOCK_MEMORY);
  LockMemory (TableIF, sizeof (ISF) * MAX_AS_COM, LOCK_MEMORY);
  imemset (TableSF, 0, sizeof (SF) * MAX_AS_COM);
  imemset (TableRF, 0, sizeof (RF) * MAX_AS_COM);
  imemset (TableIF, 0, sizeof (ISF) * MAX_AS_COM);

  // Reset sequentience
  bTableSF = bTableRF = bTableIF = 0;
  eTableSF = eTableRF = eTableIF = 0;
  RComFlag = SComFlag = IComFlag = false;

  // Init command table
  if (!(ComTable = new ComElem [MAX_AS_COM]))
    ErrorExit (1);

  LockMemory (ComTable, sizeof (ComElem) * MAX_AS_COM, LOCK_MEMORY);

  for (int i = 0; i < MAX_AS_COM; i++)
   {
    ComTable [i].Buffer    = NULL;
    ComTable [i].pFrHdr    = NULL;
    ComTable [i].Type	   = ID_NO_FRAME;
    ComTable [i].Size	   = 0;
    ComTable [i].Semaphore = FREE_SEMAPHOR;
   }

  // Init level III driver
  // Temporary buffer for decode receive of Galey cluster
  if (!(DecBuffer = new byte [SizeOrigCluster]))
    ErrorExit (1);

  SetupInterrupt (ArvidIRQ, &MyIntHandler, &OldIntHandler, (dword)IntRoutine);
}

void FreeAsynCommand (void)
{
  if (ArvidInsert)
   {
    RestoreInterupt (ArvidIRQ, &MyIntHandler, &OldIntHandler);
    delete DecBuffer;
    delete TableRF;
    delete TableSF;
    LockMemory (TableRF, sizeof (RF) * MAX_AS_COM, UNLOCK_MEMORY);
    LockMemory (TableSF, sizeof (SF) * MAX_AS_COM, UNLOCK_MEMORY);
   }

  FreeManchTable ();
  FreeGaleyTable ();
}

static void SetArvMemory (word WordMask)
{
  // connect PC to VM, reset seq, set operate "data"
  outpw (RK, ValueRK = (ValueRK | SET_DATA | LINK_VM2PC |
  SET_RESET_SEQ | SET_1052) & SET_TRANSMIT);
  outpw (RK, ValueRK &= RESET_RESET_SEQ);

  for (dword i = 0; i <= 0x800; i++) // subcadr count
   {
    outpw (RA, i); //set subcadr

    for (dword j = 0; j <= 0xff; j++)
      outpw (RD_INC, WordMask);
   }
}

static dword TestArvMemory ()
{
  // connect PC to VM, reset seq, set operate "data"
  outpw (RK, ValueRK = (ValueRK | SET_DATA | LINK_VM2PC |
  SET_RESET_SEQ | SET_1052) & SET_TRANSMIT);
  outpw (RK, ValueRK &= RESET_RESET_SEQ);

  dword i;
  for (i = 0; i <= 0x800; i++) // subcadr count
   {
    outpw (RA, i); // set subcadr

    for (dword j = 0; j <= 0xff; j++)
      outpw (RD_INC, 0xCCCC);
   }

  dword MemCount = 0;

  for (i = 0; i <= 0x800; i++) // subcadr count
   {
    outpw (RA, i); // set subcadr
    inpw (RD); //Arvid have gluck - point 2 of hints - that work

    for (dword j = 0; j <= 0xff; j++, MemCount++)
     {
      if (inpw (RD) != 0xCCCC)
	return MemCount << 1;

      outpw (RD_INC, 0);
     }
   }

  return MemCount << 1;
}

void SetSpeed ()
{
  switch (ArvidModel)
   {
    case 1031:
     {
      Arv105X = 0;
      NumArvFrame = 8;

      HArvFrame = HFRAME_1031;
      WArvFrame = WFRAME_1031;

      if (ArvidModelDetect != 1031)
       {
	PrintWorkStatusBegin ("Arvid 1031 emulation");
	PrintWorkStatusEnd ("Set");
       }

      break;
     }
    default:
     {
      Arv105X = SET_1052;
      PrintWorkStatusBegin ("Density");

      if (HDensity)
       {
	HArvFrame = HFRAME_105X;
	WArvFrame = WFRAME_105X;
	PrintWorkStatusEnd ("High");
       }
      else
       {
	HArvFrame = HFRAME_1031;
	WArvFrame = WFRAME_1031;
	PrintWorkStatusEnd ("Low");
       }

      break;
     }
   }

  SizeArvFrame = WArvFrame * HArvFrame;

  // Logical parametrs
  HeadStr = HEADER_STR;
  HDataFrame = HArvFrame - HeadStr;
  WDataFrame = WArvFrame << 1;
  WDataFrBit = WArvFrame << 4;
  SizeDataFrame = WDataFrame * HDataFrame;
  SizeOrigCluster = SizeDataFrame * VALUE_LEN;
  SizeGaleyCluster = SizeDataFrame * CODE_LEN;

  PrintWorkStatusBegin ("Size Galey cluster");
  PrintWorkStatusEnd ("%7d", SizeGaleyCluster);
  PrintWorkStatusBegin ("Size data cluster");
  PrintWorkStatusEnd ("%7d", SizeOrigCluster);
}

bool InitArvid (void)
{
  dword MemorySize = 0x80000;

  if (ArvidInsert)
   {
    PrintWorkStatusBegin ("Checked arvid memory");
    MemorySize = TestArvMemory ();
    PrintWorkStatusEnd ("%7d", MemorySize);

    NumArvFrame = MemorySize / 0x2000;
    PrintWorkStatusBegin ("Buffers frames avaible");
    PrintWorkStatusEnd ("%d", NumArvFrame);

    PrintWorkStatusBegin ("Arvid model detected");
    if (NumArvFrame > 32 && NumArvFrame <= 64)
      ArvidModelDetect = 1052;
    else if (NumArvFrame > 8 && NumArvFrame <= 32)
      ArvidModelDetect = 1051;
    else if (NumArvFrame && NumArvFrame <= 8)
      ArvidModelDetect = 1031;
    else ArvidModelDetect = 0;

    if (ArvidModelDetect)
      PrintWorkStatusEnd ("%d", ArvidModelDetect);
    else
      PrintWorkStatusEnd ("Not found");

    RecalcNumBitTable ();
   }

  SetSpeed ();

  if (ArvidInsert)
   {
    FrameMask = 0;
    word mask = 1;

    for (int i = 0; i < 8; i++, mask <<= 1)
     {
      if (NumArvFrame & mask)
	break;
      else
	FrameMask |= mask;
     }
   }

  return true;
}

bool FreeArvid (void)
{
  if (ArvidInsert) //Disconnect
    outpw (RK, ValueRK &= LINK_VM2TV);

  return 0;
}

void PrintRFStatisics ()
{
  printf ("-Stat: NumIRQ = %d, ReceiveFC = %d, ReceiveCFC = %d, RightRCFC = %d.\n",
	  IRQCount, GlobalReceiveFrameCount, GlobalRCFC, RightReceivedClusterFrames);
  printf ("-ErrStat: DublicatCF = %d, LastCF = %d, CastCCount = %d.\n",
	  DublicateClustersFrames, LastClustersFrames, CastClusterCount);
}
