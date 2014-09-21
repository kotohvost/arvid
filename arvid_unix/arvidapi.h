/****************************************************************************
	����-�������� ���������� ����� V0.01beta (Arvid API)
  ��������:
	- �������� ���������������� ����� ������
	- ������������� �������� ����� ����������
	- �������� ������������ �������� ������
	- ��������� ������� API
****************************************************************************/
// 1.������ ���� ��������� �� ������������� MS VisualC++
//   ��� ������������� ������ ������������ ����� ������������ ���������������
//   ����� ������
// 2.�������� ���������� ������ �� 32-��������� �����!


#ifndef  __ARVIDAPI_H__
#define  __ARVIDAPI_H__

// �������� ���������������� ����� ������
 #ifdef _MSC_VER
   #define _pascal _stdcall
 #endif
 #ifndef __16__		// ��� ��������� ���������� ������! ������ 32-���!
  #define _FAR_
  #define _HUGE_
  #define __32__        // default: 32  Applications!
 #else
  #define _FAR_ _far
  #define _HUGE_ _huge
 #endif
 typedef unsigned char  U8;
 typedef signed   char  I8;
 typedef unsigned short U16;
 typedef signed   short I16;
 typedef unsigned long  U32;
 typedef signed   long  I32;

#if !defined( _NODLL_) && !defined(__OS2__)
 #ifdef _MAKEDLL
  #define __DLLFUNC  __declspec(dllexport)
 #else
  #define __DLLFUNC  __declspec(dllimport)
 #endif
#else
  #define __DLLFUNC
#endif

// ���� ����������, ������������ ��������� API
#define OK_VCR  0               // �����
enum{
	E_OK=0,                 // �����
	E_DRIVER_NOT_INSTALLED=1,      // ������� �� ����������
	E_DEVICE_INVALID_OR_ALREADY_EXIST=2,// �������� AVHANDLE
	E_SYSTEM_ERROR=3,       // ��������� ��������� ������ ( Windows: ��� � GetLastError() )
	E_NOT_TPB=4,            // �� ��������� TPB
	E_TPB_EXIST=5,          // TPB ��� ���������
	E_NO_FUNCTION=6,        // ������� ��� ������� �� �������������� (�������� �����)
	E_UNKNOWN_FORMAT=7,     // ������ ����������� ������ �����
	E_NOT_FOUND_TAPE_DIR=8, // ���������� ����� �� �������
	E_NOT_AVAIL_FORMAT=9,   // ���������������� ������ ����� ��� ������
	E_NOT_POSITION=10,      // ������ �� ������ (����-��� �����)
	E_NOT_IDENTIC_TPB=11,   // ������������ TPB (���������� �� �� �� �����)
	E__ABORT=12,            // ������� �������� �������������
	E_OLD_DIR=13,           // ���������� ��������, ������ ������ "�����" �����
				// ����� �������, � �������� ������ ������ ������
				// � ������ �������� ����������
	E_NOT_DIR=14,           // �� ������� ���������� ����� ��� ������
				// � ������ �������� ����������
	E_ABORT_DISABLE=15,     // ����� � ������ ������ ��������
	E_ABORT_RECORD=16,      // �����,  ����� �� ��� � ������
	E_TRANSL_RECORD=17,     // ������ �������� "������" � ������ REC
	E_NO_IRC_RCV=18,        // �� ���� ������ �� ������� (��� ������� ����� ��)
	// ������ ���� �������� AvdCheckStop
	OFF_VCR=19,             // �� �������� ��������
	TUNER_VCR=20,           // �� ���� ��������� ���� ����������� �� ������ TV, ����
				// ��������� ������ � �����
	PICTURE_VCR=21,         // ����������, �� ����� ������������� ��������
	REC_OK_VCR=PICTURE_VCR,   // ���������������

	E_TAPE_FULL=22,         // ������ �������������� � ����� �����
				// ��� ������ (�������� �� 1 ���. �� �����)
				// �������� ��� ������
	E_FIND_DIR=23,          // ��� ������ ������� �� ����. ����������
				// ���������� ���������� �����
	E_INVALID_PARAM=24,     // �������� ��������� � �������
        E_INVALID_SEQUENCE=25,  // �������� ������������������ ������ (������ ��� ������)
	E_REACH_END_TAPE=26,    // ���������� ����� ����� ����� (��� ������)
	E_ERROR_OR_SKIP_DATA=27,// �� ��� ������ ������� ������ �������
	E_BAD_VCR_TABLE=28,     // ����������� �������� ������� ��
	E_INCOMPATIBLE_LOW_LEVEL_DRIVER=29, // ������������� ������� ������� ������� ������ ( .VxD or .SYS )
	E_UNKNOWN_FORMAT_PT=30, // ����������� ������ ������� ����������������
	E_BAD_PT=31,            // �������� ������� ���������������� (��)
	E_MISMATCH_FORMAT_PT=32,// �������������� �������� ����� � ��
	E_INTERNAL_DRIVE_ERROR=33, // internal drive error
        E_SECTOR_NOT_ACCESSIBLE=34,// ������ ��� ������ ����������( ����� ������� ������� )
	E_UNKNOWN_ERROR         // ����������� ������, ���� >= E_UNKNOWN_ERROR
};

// ������� ���� �����
#define FORMAT_K4       0x1               // ������ (reserved)
#define FORMAT_K7       0x2               // ������ 1010 100K/s (old)
#define FORMAT_K7CRC    0x4               // ������ 1010 100K/s c ���������� CRC32
#define FORMAT_RS       0x8               // ������ 1020,103X 200K/s ��� ����-��������
#define FORMAT_RSC      0x10              // ������ 1020,103X >200K/s ��� ����-�������� � ������������ ������

// ��������� ��� ������� ������ AvdIsTape
#define NOT_TAPE        0       // ��� �����
#define TPB_TAPE        1       // ��������� ����� ���������


// ������ ����������� ����� �� ( ��������� �� )
enum {
   VCR_STOP=0           ,          // ��������� STOP
   VCR_PLAY             ,          // ��������� PLAY
   VCR_PLAY_FF          ,          // ���������� ��������������� ������
   VCR_PLAY_REW         ,          // ���������� ��������������� �����
   VCR_nPLAY_FF         ,          // ���������������, ���������� ��������������� ������ (������ ��������)
   VCR_nPLAY_REW        ,          // ���������������, ���������� ��������������� �����  (������ ��������)
   VCR_FF               ,          // ���������� ��������� ������
   VCR_REW              ,          // ���������� ��������� �����
   VCR_REC              ,          // ����� ������ �� �����
   VCR_MAX= VCR_REC                // ���� �������� ������
};



#define VER_PROTO_API   0x100   //

// ����� ������� ����������� �������� ������, ������������ API


//	1. ���� ���������� �������� �����
// ���������� �������� ��������������, ���������� ��� ������ ������ ��������

// ���� ���������� (� ���� attributs)
#define USED_AUTO_PHASE 0x1     // ������� ������������ ���������������
#define USED_COMPRESS   0x2     // ������� ������������ ����� �� �������
#define USED_XOR        0x4     // ������� ������������ ���������� ����� XOR

     struct DPB {           // Driver Parameters Block
	U16     controller;	// ��� �����������
	U16     ver_major,
		ver_minor;	// ������ ��������
	U16     ver_proto_API;  // ������ ��������� API

	U16     formats;    	// ��������� ������ �������. �������� �����
	U16     maxphase;   	// ����. �������� ��������� phase

	U16     attributs;  	// ��������� ������ ���������� ��������
	U16     xor_block_size; // ���� ���-�� ������ � XOR �����

	U16     xor_block_number; // ���� ���-�� ������ � XOR �������
	U16     xor_area_max;   // ������������� ������������
				// xor_block_size*xor_block_number

	U32     reserv;		// reserv
     };


//	2. ���� ������� �������� �����
// ���������� ��������������, ������� ����� ����������
// ��� ���������� Device

     struct CDPB {           // Current Device Parameters Block
	U16     time_out;    // ����� ����-����, ������
	// �.�. �����, �� ��������� �������� �������� ������������, ���� �� �����
	// �� ������� ������� ���������� (�� ��������� 13 ������)
	U16     rate;        // �������� �������� [100-200] �����/c��
			     // ������ ������ �� ������ FORMAT_RS
	U8      phase;       // ������� ���� ������
	U8      check_rec;   // ���� �������� ������ � ���. Record
	U8      lp;          // ���� ������ � ������ LongPlay ( �.�. 0 ) reserved
	U8      auto_phase;  // ����: ������� ������ � ������ �������������� ����

	U8      compress_enable; // ����: ��� ������ ������ � ������� FORMAT_RSC
				 // ������ ���������
	U8      xor_enable;  //  ��������� ����������� XOR ����� ��� ������
	U8      xor_block_size;   // ���-�� ������ � XOR ����� ��� ������
	U8      xor_block_number; // ���-�� ������ � XOR ������� ��� ������
	U32     reserv;     // reserv
     };


//	3. ���� ���������� �����:

// ���������� �������������� �����, � ������� ������ �������� �����

     struct TPBOLD {    // Tape Parameters Block ��� �������� ����� FORMAT_RSC
	U16     format;        // ������ �����
	U16     length;        // ����� ����� � �������
	char    name[8];       // ��� �����
	U16     ident;         // ������������� ����� (���������� �����)
	U16		resrv;
	U32     reserv;        // reserv
     };
     struct TPBNEW {       // Tape Parameters Block ��� ������� FORMAT_RSC
	U16     format;		// ������ �����
	U16     length;		// ����� ����� � �������
	U32     tapeID;		// ��� ����� ( �������������-���������� ����� )
	U8      reserv[sizeof(TPBOLD)-8];     // reserv
     };

     struct TPB {	// ����� ��������� TPB
	U16	format;
	U16	length;
	union {
	  struct {
	    char	name[8];
	    U16		ident;
		U16		resrv;
	    U32		reserv;
	  };
	  U32	tapeID;
	};
     };

//     4. ���� ������� ���������� ���������� ��� ������ � ������
// ����� �������������� ��� ����������� �������� ���������
     struct CurTPB {       // Current Tape Parameters Block
	U16     cur_time;  // ������� ����� �� ������ �����, ���
	U16     targ_time; // ����� ����, ���
	U32     cur_sect;  // ������� ������
	U32     targ_sect; // ������ ����
	U16     state_vcr; // ������� ��������� �� (��������� �� ����)
	// ����� ���� �����, ������������ AVSETUP
	U32     pure_st;   // ���-�� ������ ������ ( ��� ������ )
			   // �� ������ �������� ������
        U32     reserv[4];     // reserv
     };

//      5. ���� ���������� ��������� ����������
     struct DHB {       // Device Hardware parameters Block
	U16     port;   // ������� ����
	U16     dma;    // ����� ������ DMA
	U16     irq;    // ����� ������ IRQ
	U16     busy;   // 0, ���� ����� ���� ������ AvdOpen
	U8      reserv[8];
     };

// �������� ����: ������ ������
#define SIZE_SECTOR_BYTES       512    // ������ ������� ��� ArvidApi
#define SIZE_SECTOR_WORDS       (SIZE_SECTOR_BYTES/2)
#define SIZE_SECTOR_DWORDS      (SIZE_SECTOR_BYTES/4)
typedef char SectData[SIZE_SECTOR_BYTES];
typedef SectData _FAR_ *SectDataBlock; // ��������� �� �������


/*-------------------------------------------------
	��������� ������� ����������
 --------------------------------------------------*/
// ����� ������� ��������� ������� API

// ������ �������� ����������� ������� - AVHANDLE - ��������,
// ���������� ��� �������� (������� AvdOpen)

// ������ ������� API ���������� ��� ��������. ��� ������ - E_OK,
// ����� - ��� ������

// ���������� � �������� ����������
#ifndef __OS2__
    #define AVDCONV  _FAR_ _stdcall
#else
 #ifdef __BORLANDC__
    #define AVDCONV  pascal
 #else
    #define AVDCONV  _Pascal
 #endif
#endif
    #ifndef  INVALID_HANDLE_VALUE
     #define AVHANDLE        unsigned
     #define INVALID_HANDLE_VALUE        ((AVHANDLE)(-1))
    #else
     #define AVHANDLE        unsigned
    #endif
     #define AVDRET          int

     #define AVDPROC         extern "C" __DLLFUNC  AVDRET       AVDCONV


//         1. ������������/������ �������������� � Arvid

     AVDPROC          AvdInstall();
	// ���������� ����� � ��������� (��������� �������)
	// ������ ���� ������ ���������� �������� API
     AVDPROC          AvdOpen( unsigned numDevice,AVHANDLE _FAR_ *h );
	// ������� handle �������� ��� ������� Device (����������� �����)
	// ��� - ������� ������������ ����� � Arvid. ���������� AVHANDLE
	// ������ �������������� � ����������� ���������� � API
	// ���� ���������� ��� ������ ������ ����������� - ������
     AVDPROC          AvdClose( AVHANDLE h );
        // ������� handle �������� - ����������� ���������� ��� ������
	// ����������
     AVDPROC          AvdDeInstall();
        // ��������� ����� � ��������� (��������� �������).
        // ������ ���� ��������� ���������� �������� API
     AVDPROC          AvdGetNumberDevices( U32 _FAR_ *pNumDevs );
	// ������ ���-�� ���������, ��������� ��������

//        2. ���������� ����������� ��������.

     AVDPROC AvdGetDPB( AVHANDLE h,DPB _FAR_ *dpb );
	// �������� ��������� ��������
     AVDPROC          AvdGetDHB(AVHANDLE h, DHB _FAR_ *dhb );
	// �������� ���������� ��������� ��� Device

     // AVDPROC AvdSetDHB( AVHANDLE h,const DHB _FAR_ *dhb );
	// ������ ���������� ��������� ��������

     AVDPROC AvdGetCDPB( AVHANDLE h,CDPB _FAR_ *drb );
	// �������� ������� ������ ��������
     AVDPROC AvdSetCDPB( AVHANDLE h,const CDPB _FAR_ *drb );
	// ������ ������� ������ ��������
     AVDPROC AvdSetPhase( AVHANDLE h,int phase );
	// ���������� ���� ������ ( ����� �� ����� ������ )
     AVDPROC AvdSetAutoPhase( AVHANDLE h,int phase );
	// ���������� ���� ������  � ��������������� ����
	// ( ����� �� ����� ������ )

//       3. ������ � ������.

// ���������, ���������� � Arvid ����� ����� �������� �� ����� ��� � �����
// ������ ������������.

//	3a. ���������� �������

     AVDPROC AvdIsTape( AVHANDLE h, U32 _FAR_ *isTape );
	// ������ ����������� �� ��������� �����
     AVDPROC AvdGetTape( AVHANDLE h,TPB _FAR_ *tpb );
	// �������� ��������� �����
     AVDPROC AvdGetCurTPB( AVHANDLE h,CurTPB _FAR_ *curpb );
	// �������� ������� ��������� �������� ��� ������ � ������

//	3b. ����� �����.
     AVDPROC AvdResetTape( AVHANDLE h );
	// �������� ��������� �����

//	3c. ��������� �����
// ����� ���������� ����� ����� �������� ����������!
     AVDPROC AvdSetTape( AVHANDLE h,const TPB _FAR_ *tpb );
	// ���������� ��������� ����� ����� (�������������),
	// ��� ������������� ������ �����, ��������� ������� ��������

     #define IDENT_WITH_REW     1       // � ��������
     #define IDENT_QUICK        0       // ���
     AVDPROC AvdIdentify( AVHANDLE h,int Control );
	// ���������������� ����� (�.�. ���������� �� TPB)

//	3d. ������ � �������� ���������������� (position table,PT)

//  ��� ������ ������������� ����� ����� ����� ������� ���������������� -
//  ������� ������������ ������ ������� � ������� �� ������ �����. ���
//  ��������� ������ ��������������� �� �����, �������� �� ���������
//  ��������������� ���������� ������.
//  ����������, ���������� � API ������ ��������� PT � ����������� �����
//  c���c��� ������ � ������. �.�. ����� �������� ������ (����� PT ����������)
//  ���������� ������� PT � ��������� ��. � ����������, ����� �������������
//  ����� ���������� ��������� ����� ����������� PT.
//  ����� PT ��������� � ������� ������ �� ������� �����.

     AVDPROC AvdGetPTLength( AVHANDLE h , U32 _FAR_ *ptLength );
	// �������� ����� ������� ���������������� � ������
	// ������ ���������� ����� ����� ��� ��������� (�������� �� ������)
     AVDPROC AvdGetPT( AVHANDLE h,void _FAR_ * addr );
	// �������� ������� ���������������� �����
	// addr - ����� ������, ����� �������� ������������ � AvdGetPTLength
     AVDPROC AvdSetPT( AVHANDLE h,const void _FAR_ * addr );
	// ��������� ������� ���������������� �����
     AVDPROC AvdGetSectorTime( AVHANDLE h , U32 sector ,U32 _FAR_ *sectorTime, void _FAR_ * pPosTbl );
	// ���������� ������� ����������� ������� �� ������ ������� ��
	// ��������� PT
	// pPosTbl ��������� �� PT (���� 0, �� �� ����������� � �������)


//	4.������ � ����������� ��.
// ��� ����������� ���������� ������ � �� ���������� ������ ������ �������
// ���������� ����������� ��. ��� ����� ����������� ���� �� �����,
// ���� �� ������ (�������� ��� ������).

     AVDPROC AvdSetVCR( AVHANDLE h,const char _FAR_ *fname );
	// ��������� ������� ���������� �� �� �����
     AVDPROC AvdSetVCRAddr( AVHANDLE h,const void _FAR_ *vcr );
	// ��������� ������� ���������� ��


//      ���������������� �������� ������/������

// ��� ���������������� ��������� ������/������ ����� ����� ����� ���������:
// ������� ������/������ �� ���������� ���������� �� ��� ���, ���� ��������
// �� ���������� ���������. ������� ��� �������� � ������/�������������
// ������������� ������������ ���. ������ (threads).

//	5.������ ���������� �� �����.

//��� ������ ���������� ���������� ����������� ����������� ���������� �
// ��������� ��������
//(����� �������� ��������) ������ ���� ���������� ���������� ����� ��������:
     AVDPROC AvdGetInterRecord( AVHANDLE h , U32 _FAR_ *interRec );
	// ������ ������ ������������ ���������� ( � �������� ��� �������� ������� �����
	// � ��� ������� ���� ��)

     // �������� RecMode ��� AvdWriteStart
     // -- ��� �������� ������
     #define REC_CURRENT_PLACE 0 //- � �������� �����
     #define REC_DIR_CHECK     1 //- � ����.������� aftersect � ��������� ����������
     #define REC_NO_DIR_CHECK  2 //- -------//------------------- ��� --------//--------
     #define REC_MAX_CODE      (REC_NO_DIR_CHECK)

     AVDPROC AvdWriteStart( AVHANDLE h,int RecMode, U32 aftersect );
	// ������ ������ ������
	// ��� - ������ ������� �������� ������
	// �� ��� ����� ��������� � ����� REC ���� �����, ���� (� �����������
	// �� ��������� ������ ) ����� ���������� ���������� � ������� �������
	// aftersect. ��� ������� ��� - ��������� ������ ���������� ������
	//
     // �������� ControlFlags ��� AvdWrite
     #define REC_ENABLE_COMPRESS  0          // �������� ���� ControlFlags
     #define REC_DISABLE_COMPRESS 0x8000     // ������ ������ ��� �������
     AVDPROC AvdWrite( AVHANDLE h,const void _FAR_ * data, U32 begSect,
       U16 nSect , int ControlFlags );
	// ������ ������ (�������)
	// begSect - ����� ���������� �������
	// nSect - ����� ��������
	// ��������� �������� � ���������������� �������� ������ ����
	// �����������
     AVDPROC AvdWriteDirectory( AVHANDLE h,const void _FAR_ * data, U32 begSect,
       U16 nSect  );
	// ������ ���������� �����
	// ���������� �� ������� ���, ��� ������� ���������� ����. ���������,
	// ������� ����� ������ ���������� ������ ����������
     AVDPROC AvdWriteLog( AVHANDLE h,const void _FAR_ * data, U32 begSect, U16 nSect  );
	// ������ ������ �� ����. ����������
	// ���������� ���, ��� ������� ���������� ����������, �����������
	// �������� ��� ����� �� ������ ��������
	// � ��������� ����� ���� ������� ���������� ������� � ����������� �
	// ������������ ������, ����� ����� ������� ��������� ���������
	// � ���������� �������� ���� �������������
//     AVDPROC AvdWritePT( AVHANDLE h, const void _FAR_ * data, U32 begSect, U16 nSect );
	// ������ ������� ����������������
     AVDPROC AvdWritePause( AVHANDLE h , unsigned  pause_second );
	// ������ ����� � �������� ������ (��������, � ������ ����� ��� �����
	// �����������)
	// ���� (pause_second==0) - ����������� ����� ����� �����������
     AVDPROC AvdWriteEnd( AVHANDLE h );
	// ����� ������ - ��������� ������� �������� ������

//         6. ������ ���������� � �����.

     // �������� ���������, ������ ������������
     // ��� ������ �������� ������
     struct     RD_RESULT
     {
	U32             nReadSect;   // ���-�� ������� ��������� ��������
	U32             errors[3];   // ���������� ������������ ������
				     // �����������, ����������� � �����������
	U32             begSect;     // ����� ������� ������������ �������
				     //	(����� ����� � ���������� ������)
     };
     // �������� ���� ControlFlags
     #define DEFAULT_FLAGS     0
     #define USED_XOR_RESTORE  0x8000

     AVDPROC AvdRead( AVHANDLE h,void _FAR_ * data, U32 begSect, U16 nSect ,
      int ControlFlags, RD_RESULT _FAR_ * p );
	// ������ ������, �  RD_RESULT ��������� ������
	// �������� ������� �������� ������
     AVDPROC AvdReadEnd( AVHANDLE h,int Control );
	// ��������� ������ ������ - ��������� ������� �������� ������

     AVDPROC AvdReadLog( AVHANDLE h,void _FAR_ * data, U16 nSect , RD_RESULT _FAR_ * p  );
	// ����� (���������������� ���������� ) ������� �� ����. ����������
	// � ������ nSect �������� ������� � ����
     AVDPROC AvdReadDirectory( AVHANDLE h,void _FAR_ * data, U16 nSect , RD_RESULT _FAR_ * p  );
	// ������-����� �������� ����������, ������� � ������� ��������������.
	// ������������ ����� ��������� ������ ("��� � �������" ;-)
     AVDPROC AvdReadDirectoryQuick( AVHANDLE h,void _FAR_ * data, U16 nSect , RD_RESULT _FAR_ * p  );
	// ������-����� �������� ����������, ������� � ������� ��������������,
        // ��� ������������� ���� � ������� (���������������� ���������� )

//         7. ����������� ���������.

     AVDPROC AvdAbort( AVHANDLE h );
	// ���������� ���������� ����� �������� ��������
     AVDPROC AvdIrcKeyTrn( AVHANDLE h,int Control );
	// �������� �� ������� � ������� Control
     AVDPROC AvdHardwareTest( AVHANDLE h  );
	// ��������� ���������� �������� ����������������� ����������
	// � �������� ����������� � DHB
     AVDPROC AvdCheckStop( AVHANDLE h );
	// ������ ��������� ������ ���������� ��
     AVDPROC AvdPowerOn( AVHANDLE h,int Control );
	// ��������/����������� ��������� ��
     AVDPROC AvdPowerOff( AVHANDLE h,int Control );
	// ��������/����������� ���������� ��
     AVDPROC AvdEject( AVHANDLE h );
	// ������ ������� �� ��

//       7a. � ��� ����� ��������� ��������� �����:
     AVDPROC AvdRewind ( AVHANDLE h );
	// ������ ������� ��������� �����
     AVDPROC AvdForward( AVHANDLE h );
	// ������ ������� ��������� ������

     // ��������� ������� ������������ � avsetup
     AVDPROC AvdMoveTape( AVHANDLE h,
			unsigned  mode,        // ����� ����������� ����� �� (VCR_FF,VCR_PLAY_FF ...)
			unsigned  time,        // ����� ����������� ( ������= 1/50 ��� )
			unsigned  finit_mode  // ����� �� �� ��������� �����������
      );
	// ���������� ����� � ������ mode � ������� ������� time (������= 1/50 ���)
	// �� ��������� ������� � PLAY ��� STOP (finit_mode).
     AVDPROC AvdPosition( AVHANDLE h,
			unsigned cur,         // ������� ������� �� ����� � ��������
			unsigned target,     // ������� ������� �� ����� � ��������
			unsigned finit_mode   // ����� �� �� ��������� �����������
     );
	// ���������� ����� � ������� cur (���) �� ������� target
	// �� ��������� ������� � PLAY ��� STOP
        // ������� ��� �������� ����� ��������
     AVDPROC AvdSetVirtualVcrState( AVHANDLE h, int Control );
	// ��������� ������� � ��������� Control ( VCR_... )
	// ��� ��������� ������������ ��
     AVDPROC AvdSetVcrState( AVHANDLE h, int Control );
	// ��������� �� � ��������� Control ( VCR_... )

#endif // End of file ARVIDAPI.H
