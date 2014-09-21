
#ifndef __TAPE_H__
#define __TAPE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Tape level API definition */

#define IDENT_WITH_REW 1 /* with rewind    */
#define IDENT_QUICK    0 /* without rewind */
/* Fill cst.
 * Flags == IDENT_WITH_REW
 * 1. Rewind on -(pvcr->svcr->IdentRew >> 2) seconds
 * 2. Set play VHS mode
 * 3. Read sectors untill find data
 * 4. Set stop VHS mode
 * 5. Return frame id or
 *    E_WRITE_TO_MAIN_QUEUE,
 *    E_READ_FROM_DRV_QUEUE,
 *    E_NOT_POSITION
 *
 * Flags == IDENT_QUICK
 * 1. Set play VHS mode
 * 2. Read sectors untill find data
 * 3. Set stop VHS mode
 * 4. Return frame id or
 *    E_WRITE_TO_MAIN_QUEUE,
 *    E_READ_FROM_DRV_QUEUE,
 *    E_NOT_POSITION
 */
extern int IdentifyTape (vcr *pvcr, int Flags);

/* On mounted tape do this
 * used PT, CST and modify it
 * 1. Move tape on target frame position
 * 2. Set play VHS mode 
 * 3. Read data frame untill find data
 * 4. Set end_mode VHS mode
 * 5. Return target frame id or
 *    E_INVALID_PARAM,
 *    E_BAD_PT,
 *    E_OUT_WRITE_AREA,
 *    E_WRITE_TO_MAIN_QUEUE,
 *    E_READ_FROM_DRV_QUEUE,
 *    E_NOT_POSITION,
 *    E_NOT_IDENTIC_TPB
 */
extern int PositionTape (vcr *pvcr, dword cur_frame, dword trg_frame, int end_mode);

/* Pause in microseconds(mks) */
extern void WritePause (vcr *pvcr, __useconds_t pause_mks);

/* RecMode == REC_NO_DIR_CHECK
 * 1. PositionTape afterframe
 * 2. Set "Record" VHS mode
 * 3. Write gap frames with afterframe + 1
 * 4. Return true on success or
 *    E_BAD_PT,
 *    PositionTape errors codes
 *
 * RecMode == REC_CURRENT_PLACE
 * 1. Set "Record" VHS mode
 * 2. Write gap frames with afterframe + 1
 * After every write frame update PT: cur_tpb->cur_time + 1/PAL_FPS
 * After play-record add cur_tpb->cur_time + timePlayRecord
 * 3. Return true on success or
 *    E_BAD_PT,
 *    PositionTape errors codes
 */
extern int  WriteStart (vcr *pvcr, int RecMode, dword afterframe);

/* Return last frame id */
extern int  WriteData (vcr *pvcr, void *buf, dword beg_frame, dword num);
extern void WriteEnd  (vcr *pvcr);

/* Return number reading frames or
 *    E_WRITE_TO_MAIN_QUEUE,
 *    E_READ_FROM_DRV_QUEUE,
 *    E_NOT_POSITION,
 *    E_NOT_IDENTIC_TPB,
 *    PositionTape errors codes
 */
extern int  ReadData (vcr *pvcr, void *buf, dword beg_frame, dword num, estat *err);
extern void ReadEnd  (vcr *pvcr);
extern void Rewind   (vcr *pvcr);
extern void Forward  (vcr *pvcr);

/* Part of original high level API definition arvidapi.h */
/* Implemented for old software portability        */

#define REC_CURRENT_PLACE 0 /* with current position do set tape position and set play-record-pause */
#define REC_NO_DIR_CHECK  2 /* after sector, do set tape position and set play-record-pause */

#define MASK_AUTOTUNING_A 0
#define MASK_AUTOTUNING_B 0x2000

/****************************************************************************
        ����-�������� ���������� ����� V1.03 (Arvid API)
  ��������:
	- ������������� �������� ����� ����������
****************************************************************************/

enum {
	E_OK                 = 0,   /* ����� */
        E_DRIVER_NOT_INSTALLED = -1,/* ������� �� ���������� (���� ��� ���������� � Registry) */
	E_DEVICE_INVALID_OR_ALREADY_EXIST = -2, /* �������� AVHANDLE */
	E_SYSTEM_ERROR       = -3,  /* ��������� ��������� ������ ( Windows: ��� � GetLastError() ) */
	E_NOT_TPB            = -4,  /* �� ��������� TPB */
	E_TPB_EXIST          = -5,  /* TPB ��� ��������� */
        E_HARDWARE_TEST      = -6,  /* �� ������ ���� ���������� ( ���� ������ ��. ���� ) */
	E_UNKNOWN_FORMAT     = -7,  /* ������ ����������� ������ ����� */
	E_NOT_FOUND_TAPE_DIR = -8,  /* ���������� ����� �� ������� */
	E_NOT_AVAIL_FORMAT   = -9,  /* ���������������� ������ ����� ��� ������ */
	E_NOT_POSITION       = -10, /* ������ �� ������ (����-��� �����) */
	E_NOT_IDENTIC_TPB    = -11, /* ������������ TPB (���������� �� �� �� �����) */
	E__ABORT             = -12, /* ������� �������� ������������� */
	E_OLD_DIR            = -13, /* ���������� ��������, ������ ������ "�����" ����� */
				    /* ����� �������, � �������� ������ ������ ������ */
				    /* � ������ �������� ����������   */
	E_NOT_DIR            = -14, /* �� ������� ���������� ����� ��� ������ */
				    /* � ������ �������� ����������   */
	E_ABORT_DISABLE      = -15, /* ����� � ������ ������ �������� */
	E_ABORT_RECORD       = -16, /* �����,  ����� �� ��� � ������  */
	E_TRANSL_RECORD      = -17, /* ������ �������� "������" � ������ REC */
	E_NO_IRC_RCV         = -18, /* �� ���� ������ �� ������� (��� ������� ����� ��) */
	
	/* AvdCheckStop's method errors codes */
	OFF_VCR              = -19, /* �� �������� �������� */
	TUNER_VCR            = -20, /* �� ���� ��������� ���� ����������� �� ������ TV, ���� */
			            /* ��������� ������ � ����� */
	PICTURE_VCR          = -21, /* ����������, �� ����� ������������� �������� */
	REC_OK_VCR           = PICTURE_VCR, /* ��������������� */
	E_TAPE_FULL          = -22, /* ������ �������������� � ����� ����� */
			            /* ��� ������ (�������� �� 1 ���. �� �����) */
			            /* �������� ��� ������ */
	E_FIND_DIR           = -23, /* ��� ������ ������� �� ����. ���������� */
			            /* ���������� ���������� ����� */
	E_INVALID_PARAM      = -24, /* �������� ��������� � ������� */
        E_INVALID_SEQUENCE   = -25, /* �������� ������������������ ������ (������ ��� ������) */
	E_REACH_END_TAPE     = -26, /* ���������� ����� ����� ����� (��� ������) */
	E_ERROR_OR_SKIP_DATA = -27, /* �� ��� ������ ������� ������ ������� */
	E_BAD_VCR_TABLE      = -28, /* ����������� �������� ������� �� */
	E_INCOMPATIBLE_LOW_LEVEL_DRIVER = -29, /* ������������� ������� ������� ������� ������ ( .VxD or .SYS ) */
	E_UNKNOWN_FORMAT_PT  = -30, /* ����������� ������ ������� ���������������� */
	E_BAD_PT             = -31, /* �������� ������� ���������������� (��) */
	E_MISMATCH_FORMAT_PT = -32, /* �������������� �������� ����� � �� */
	E_NO_FUNCTION        = -33,
        E_INTERNAL_DRIVE_ERROR = E_NO_FUNCTION, /* internal drive error: ������� ��� ������� �� �������������� (�������� �����) */
        E_SECTOR_NOT_ACCESSIBLE = -34, /* ������ ��� ������ ����������( ����� ������� ������� ) */
        E_FILE_NOT_ACCESIBLE = -35, /* ����������� ���� �� �������� */
	
	/* Driver errors codes */
        E_HW_INT             = E_NOT_IDENTIC_TPB, /* ���� ����� ���������� */
        E_HW_MEM_TEST        = E_HARDWARE_TEST, /* ���� ����� ������ ����������� */
        E_HW_DMA_NOT_WORKING = TUNER_VCR,   /* DMA �� ����������� */
        E_HW_DMA_INCORRECT   = PICTURE_VCR, /* �������� ������ ��� DMA */
        E_SYSTEM_ALLOC_PORTS = E_TAPE_FULL, /* ������������ ������� �� �������� ����� */
        E_SYSTEM_ALLOC_IRQ   = E_FIND_DIR,  /* ������������ ������� �� �������� irq */
        E_SYSTEM_ALLOC_DMA   = E_INVALID_PARAM, /* ������������ ������� �� �������� ����� DMA */
        E_SYSTEM_ALLOC_RESOURCES = E_INVALID_SEQUENCE, /* ������������ ������� �� �������� ������� */
	
	/* UNIX(tm) software errors codes */
	/* main queue from main thread to driver thread */
	E_WRITE_TO_MAIN_QUEUE = -36,
	E_READ_FROM_MAIN_QUEUE = -37,
	/* driver queue from driver thread to main thread */	
	E_WRITE_TO_DRV_QUEUE = -38,
	E_READ_FROM_DRV_QUEUE = -39,
	
	E_OUT_WRITE_AREA     = -40,
	E_FILE_NOT_FOUND     = -41,
	E_OUT_OF_MEMORY      = -42,
	E_UNKNOWN_ERROR      = -43       /* ����������� ������, ���� >= E_UNKNOWN_ERROR */
};

#ifdef __cplusplus
}
#endif

#endif
