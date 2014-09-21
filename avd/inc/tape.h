
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
        Файл-описание интерфейса АрВид V1.03 (Arvid API)
  Содержит:
	- символическое описание кодов завершения
****************************************************************************/

enum {
	E_OK                 = 0,   /* успех */
        E_DRIVER_NOT_INSTALLED = -1,/* драйвер не установлен (либо нет информации в Registry) */
	E_DEVICE_INVALID_OR_ALREADY_EXIST = -2, /* неверный AVHANDLE */
	E_SYSTEM_ERROR       = -3,  /* произошла системная ошибка ( Windows: код в GetLastError() ) */
	E_NOT_TPB            = -4,  /* не определен TPB */
	E_TPB_EXIST          = -5,  /* TPB уже определен */
        E_HARDWARE_TEST      = -6,  /* не прошел тест аппаратуры ( коды ошибок см. ниже ) */
	E_UNKNOWN_FORMAT     = -7,  /* считан неизвестный формат ленты */
	E_NOT_FOUND_TAPE_DIR = -8,  /* оглавление ленты не найдено */
	E_NOT_AVAIL_FORMAT   = -9,  /* неподдерживаемый формат ленты для записи */
	E_NOT_POSITION       = -10, /* сектор не найден (тайм-аут истек) */
	E_NOT_IDENTIC_TPB    = -11, /* несовпадение TPB (наткнулисъ не на ту ленту) */
	E__ABORT             = -12, /* команда прервана полъзователем */
	E_OLD_DIR            = -13, /* оглавление устарело, найден сектор "нашей" ленты */
				    /* после сектора, с которого задано начало записи */
				    /* в режиме контроля оглавления   */
	E_NOT_DIR            = -14, /* не найдено оглавление ленты при записи */
				    /* в режиме контроля оглавления   */
	E_ABORT_DISABLE      = -15, /* аборт в данный момент запрещен */
	E_ABORT_RECORD       = -16, /* аборт,  когда ВМ был в записи  */
	E_TRANSL_RECORD      = -17, /* ошибка контроля "колъца" в режиме REC */
	E_NO_IRC_RCV         = -18, /* не было приема ИК клавиши (для команды ввода ИК) */
	
	/* AvdCheckStop's method errors codes */
	OFF_VCR              = -19, /* ВМ вероятно выключен */
	TUNER_VCR            = -20, /* ВМ либо формирует свое изображение на экране TV, либо */
			            /* принимает сигнал с эфира */
	PICTURE_VCR          = -21, /* аналогично, но видео транслируется частично */
	REC_OK_VCR           = PICTURE_VCR, /* зарезервировано */
	E_TAPE_FULL          = -22, /* раннее предупреждение о конце ленты */
			            /* при записи (выдается за 1 мин. до конца) */
			            /* выдается при записи */
	E_FIND_DIR           = -23, /* при поиске сектора со спец. аттрибутом */
			            /* обнаружено оглавление ленты */
	E_INVALID_PARAM      = -24, /* неверные параметры в команде */
        E_INVALID_SEQUENCE   = -25, /* неверная последователъностъ команд (чтения или записи) */
	E_REACH_END_TAPE     = -26, /* достигнута метка конца ленты (при чтении) */
	E_ERROR_OR_SKIP_DATA = -27, /* не все данные команды чтения считаны */
	E_BAD_VCR_TABLE      = -28, /* загружается неверная таблица ВМ */
	E_INCOMPATIBLE_LOW_LEVEL_DRIVER = -29, /* несовместимый драйвер драйвер нижнего уровня ( .VxD or .SYS ) */
	E_UNKNOWN_FORMAT_PT  = -30, /* неизвестный формат таблицы позиционирования */
	E_BAD_PT             = -31, /* неверная таблица позиционирования (ТП) */
	E_MISMATCH_FORMAT_PT = -32, /* несоответствие форматов ленты и ТП */
	E_NO_FUNCTION        = -33,
        E_INTERNAL_DRIVE_ERROR = E_NO_FUNCTION, /* internal drive error: функция или команда не поддерживается (неверный номер) */
        E_SECTOR_NOT_ACCESSIBLE = -34, /* сектор для поиска недоступен( истек счетчик откатов ) */
        E_FILE_NOT_ACCESIBLE = -35, /* загружаемый файл не доступен */
	
	/* Driver errors codes */
        E_HW_INT             = E_NOT_IDENTIC_TPB, /* сбои теста прерываний */
        E_HW_MEM_TEST        = E_HARDWARE_TEST, /* сбои теста памяти контроллера */
        E_HW_DMA_NOT_WORKING = TUNER_VCR,   /* DMA не выполняется */
        E_HW_DMA_INCORRECT   = PICTURE_VCR, /* неверный чипсет для DMA */
        E_SYSTEM_ALLOC_PORTS = E_TAPE_FULL, /* операционная система не выделяет порты */
        E_SYSTEM_ALLOC_IRQ   = E_FIND_DIR,  /* операционная система не выделяет irq */
        E_SYSTEM_ALLOC_DMA   = E_INVALID_PARAM, /* операционная система не выделяет канал DMA */
        E_SYSTEM_ALLOC_RESOURCES = E_INVALID_SEQUENCE, /* операционная система не выделяет ресурсы */
	
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
	E_UNKNOWN_ERROR      = -43       /* неизвестная ошибка, если >= E_UNKNOWN_ERROR */
};

#ifdef __cplusplus
}
#endif

#endif
