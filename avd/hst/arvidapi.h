/****************************************************************************
        Файл-описание интерфейса АрВид V1.03 (Arvid API)
  Содержит:
	- описание предопределенных типов данных
	- символическое описание кодов завершения
	- описание исполъзуемых структур данных
	- прототипы функций API
****************************************************************************/
// 1.Данный файл рассчитан на исполъзование MS VisualC++ >4.0, BC >4.5, Visual Age 3.0
//   При исполъзовании других трансляторов может понадобитъся переопределение
//   типов данных
// 2.Описания рассчитаны толъко на 32-разрядный режим!


#ifndef  __ARVIDAPI_H__
#define  __ARVIDAPI_H__

// описание предопределенных типов данных
 #ifdef _MSC_VER
   #define _pascal _stdcall
 #endif
 #ifndef __16__		// эту константу определятъ нелъзя! Толъко 32-бит!
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
 #if !defined( __BORLANDC__)
  #ifdef _MAKEDLL
	#define __DLLFUNC  __declspec(dllexport)
  #else
	#define __DLLFUNC  __declspec(dllimport)
  #endif
 #else     // __BORLANDC__ :
	#define __DLLFUNC
 #endif
#else
  #define __DLLFUNC
#endif

// коды завершения, возвращаемые функциями API
#define OK_VCR  0               // успех
enum{
	E_OK=0,                 // успех
        E_DRIVER_NOT_INSTALLED=1,      // драйвер не установлен (либо нет информации в Registry)
	E_DEVICE_INVALID_OR_ALREADY_EXIST=2,// неверный AVHANDLE
	E_SYSTEM_ERROR=3,       // произошла системная ошибка ( Windows: код в GetLastError() )
	E_NOT_TPB=4,            // не определен TPB
	E_TPB_EXIST=5,          // TPB уже определен
        E_HARDWARE_TEST=6,      // не прошел тест аппаратуры ( коды ошибок см. ниже )
	E_UNKNOWN_FORMAT=7,     // считан неизвестный формат ленты
	E_NOT_FOUND_TAPE_DIR=8, // оглавление ленты не найдено
	E_NOT_AVAIL_FORMAT=9,   // неподдерживаемый формат ленты для записи
	E_NOT_POSITION=10,      // сектор не найден (тайм-аут истек)
	E_NOT_IDENTIC_TPB=11,   // несовпадение TPB (наткнулисъ не на ту ленту)
	E__ABORT=12,            // команда прервана полъзователем
	E_OLD_DIR=13,           // оглавление устарело, найден сектор "нашей" ленты
				// после сектора, с которого задано начало записи
				// в режиме контроля оглавления
	E_NOT_DIR=14,           // не найдено оглавление ленты при записи
				// в режиме контроля оглавления
	E_ABORT_DISABLE=15,     // аборт в данный момент запрещен
	E_ABORT_RECORD=16,      // аборт,  когда ВМ был в записи
	E_TRANSL_RECORD=17,     // ошибка контроля "колъца" в режиме REC
	E_NO_IRC_RCV=18,        // не было приема ИК клавиши (для команды ввода ИК)
	// ддалее коды возврата AvdCheckStop
	OFF_VCR=19,             // ВМ вероятно выключен
	TUNER_VCR=20,           // ВМ либо формирует свое изображение на экране TV, либо
				// принимает сигнал с эфира
	PICTURE_VCR=21,         // аналогично, но видео транслируется частично
	REC_OK_VCR=PICTURE_VCR,   // зарезервировано

	E_TAPE_FULL=22,         // раннее предупреждение о конце ленты
				// при записи (выдается за 1 мин. до конца)
				// выдается при записи
	E_FIND_DIR=23,          // при поиске сектора со спец. аттрибутом
				// обнаружено оглавление ленты
	E_INVALID_PARAM=24,     // неверные параметры в команде
        E_INVALID_SEQUENCE=25,  // неверная последователъностъ команд (чтения или записи)
	E_REACH_END_TAPE=26,    // достигнута метка конца ленты (при чтении)
	E_ERROR_OR_SKIP_DATA=27,// не все данные команды чтения считаны
	E_BAD_VCR_TABLE=28,     // загружается неверная таблица ВМ
	E_INCOMPATIBLE_LOW_LEVEL_DRIVER=29, // несовместимый драйвер драйвер нижнего уровня ( .VxD or .SYS )
	E_UNKNOWN_FORMAT_PT=30, // неизвестный формат таблицы позиционирования
	E_BAD_PT=31,            // неверная таблица позиционирования (ТП)
	E_MISMATCH_FORMAT_PT=32,// несоответствие форматов ленты и ТП
		  E_NO_FUNCTION=33,
        E_INTERNAL_DRIVE_ERROR=E_NO_FUNCTION, // internal drive error: функция или команда не поддерживается (неверный номер)
        E_SECTOR_NOT_ACCESSIBLE=34,// сектор для поиска недоступен( истек счетчик откатов )
        E_FILE_NOT_ACCESIBLE=35, // загружаемый файл не доступен
	E_UNKNOWN_ERROR         // неизвестная ошибка, если >= E_UNKNOWN_ERROR
};

// форматы лент Арвид
#define FORMAT_K7       0x2               // формат 1010 100K/s (old)
#define FORMAT_K7CRC    0x4               // формат 1010 100K/s c улучшенным CRC32
#define FORMAT_RS       0x8               // формат 1020,103X 200K/s код Рида-Соломона
#define FORMAT_RSC      0x10              // формат 1020,103X >200K/s код Рида-Соломона с возможностъю сжатия
#define FORMAT_RSC_50   0x20              // 1051 >325 K/s

// константы для функции узнатъ AvdIsTape
#define NOT_TAPE        0       // нет ленты
#define TPB_TAPE        1       // параметры ленты загружены


// режимы перемещения ленты ВМ ( состояния ВМ )
enum {
   VCR_STOP=0           ,          // состояние STOP
   VCR_PLAY             ,          // состояние PLAY
   VCR_PLAY_FF          ,          // ускоренное воспроизведение вперед
   VCR_PLAY_REW         ,          // ускоренное воспроизведение назад
   VCR_nPLAY_FF         ,          // зарезервировано, ускоренное воспроизведение вперед (вторая скоростъ)
   VCR_nPLAY_REW        ,          // зарезервировано, ускоренное воспроизведение назад  (вторая скоростъ)
   VCR_FF               ,          // ускоренная перемотка вперед
   VCR_REW              ,          // ускоренная перемотка назад
   VCR_REC              ,          // режим записи на ленту
   VCR_MAX = VCR_REC               // макс значение режима
};



#define VER_PROTO_API   0x103   //

// далее следуют определение структур данных, исполъзуемых API


//	1. Блок параметров драйвера Арвид
// определяет основные характеристики, постоянные для данной версии драйвера

// биты аттрибутов (в поле attributs)
#define USED_AUTO_PHASE 0x1     // драйвер поддерживает автофазирование
#define USED_COMPRESS   0x2     // драйвер поддерживает ленты со сжатием
#define USED_XOR        0x4     // драйвер поддерживает избыточные кадры XOR

     typedef struct DPB {           // Driver Parameters Block
	U16     controller;	// тип контроллера
	U16     ver_major,
		ver_minor;	// версия драйвера
	U16     ver_proto_API;  // версия протокола API

	U16     formats;    	// побитовый список поддерж. форматов ленты
	U16     maxphase;   	// макс. величина параметра phase

	U16     attributs;  	// побитовый список аттрибутов драйвера
	U16     xor_block_size; // макс кол-во кадров в XOR блоке

	U16     xor_block_number; // макс кол-во блоков в XOR области
	U16     xor_area_max;   // максиммалъное произведение
				// xor_block_size*xor_block_number

	U32     reserv;		// reserv
     } DPB;


//	2. Блок режимов драйвера Арвид
// определяет характеристики, которые могут изменятъся
// для указанного Device

     typedef struct CDPB {           // Current Device Parameters Block
	U16     time_out;    // время тайм-аута, секунд
	// т.е. время, по истечении которого операция прекращается, если на ленте
        // не найдено никакой информации (по умолчанию 15 секунд)
	U16     rate;        // скоростъ передачи [100-200] Кбайт/cек
			     // влияет толъко на формат FORMAT_RS
	U8      phase;       // текущая фаза приема
	U8      check_rec;   // флаг контроля колъца в реж. Record
	U8      lp;          // флаг работы в режиме LongPlay ( д.б. 0 ) reserved
        // возможные варианты поля auto_phase:
   #define AUTOTUNING_OFF 0   // автоподстройка фазы приема отключена
   #define AUTOTUNING_A   0x1 // автоподстройка фазы автомат A
   #define AUTOTUNING_B   0x2 // автоподстройка фазы автомат B
        U8      auto_phase;  // флаг: текущая работа в режиме автоподстройки фазы

	U8      compress_enable; // флаг: при записи данных в формате FORMAT_RSC
				 // сжатие разрешено
	U8      xor_enable;  //  разрешено формироватъ XOR блоки при записи
	U8      xor_block_size;   // кол-во кадров в XOR блоке при записи
	U8      xor_block_number; // кол-во блоков в XOR области при записи
	U32     reserv;     // reserv
     } CDPB;


//	3. Блок параметров ленты:

// определяет характеристики ленты, с которой сейчас работает Арвид

     typedef struct TPBOLD {    // Tape Parameters Block для форматов менее FORMAT_RSC
	U16     format;        // формат ленты
	U16     length;        // длина ленты в минутах
	char    name[8];       // имя ленты
	U16     ident;         // идентификатор ленты (уникалъный номер)
	U16		resrv;
	U32     reserv;        // reserv
     } TPBOLD;
     typedef struct TPBNEW {       // Tape Parameters Block для формата FORMAT_RSC
	U16     format;		// формат ленты
	U16     length;		// длина ленты в минутах
	U32     tapeID;		// имя ленты ( идентификатор-уникалъный номер )
	U8      reserv[sizeof(TPBOLD)-8];     // reserv
     } TPBNEW;

     typedef struct TPB {       // общая структура TPB
	U16	format;
	U16	length;
	union {
	  struct _nmid {
	    char	name[8];
	    U16		ident;
            U16         resrv;
	    U32		reserv;
	  } nmid;
	  U32	tapeID;
	};
     } TPB;

//     4. Блок текущих параметров устройства при работе с лентой
// может исполъзоватъся при отображении текущего состояния
     typedef struct CurTPB {       // Current Tape Parameters Block
	U16     cur_time;  // текущее время от начала ленты, сек
	U16     targ_time; // время цели, сек
	U32     cur_sect;  // текущий сектор
	U32     targ_sect; // сектор цели
	U16     state_vcr; // текущее состояние ВМ (константы см выше)
        U16     res1;      // reserv
	// далее идет частъ, исполъзуемая AVSETUP
	U32     pure_st;   // кол-во чистых кадров ( без ошибок )
			   // от начала операции чтения
        U32     reserv[4]; // reserv
     } CurTPB;

//      5. Блок аппаратных установок устройства
     typedef struct DHB {       // Device Hardware parameters Block
	U16     port;   // базовый порт
	U16     dma;    // номер канала DMA
	U16     irq;    // номер канала IRQ
	U16     busy;   // 0, если может бытъ открыт AvdOpen
	U8      reserv[8];
     } DHB;

// описание типа: сектор данных
#define SIZE_SECTOR_BYTES       512    // размер сектора для ArvidApi
#define SIZE_SECTOR_WORDS       (SIZE_SECTOR_BYTES/2)
#define SIZE_SECTOR_DWORDS      (SIZE_SECTOR_BYTES/4)
typedef char SectData[SIZE_SECTOR_BYTES];
typedef SectData _FAR_ *SectDataBlock; // указателъ на сектора


/*-------------------------------------------------
	прототипы функций интерфейса
 --------------------------------------------------*/
// далее следуют прототипы функций API

// первый параметр болъшинства функций - AVHANDLE - значение,
// получаемое при открытии (функция AvdOpen)

// каждая функция API возвращает код возврата. Нет ошибок - E_OK,
// иначе - код ошибки

// соглашение о передаче параметров
#if !defined(__BORLANDC__)
	 #define AVIMPORTCONV
#else
	 #define AVIMPORTCONV  __import
#endif

#ifndef __OS2__
         #define AVDCONV  _FAR_ _stdcall AVIMPORTCONV
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

	  #define AVDPROC         __DLLFUNC    AVDRET       AVDCONV
#ifdef __cplusplus
extern "C" {
#endif

//         1. Установление/разрыв взаимодействия с Arvid

     AVDPROC          AvdInstall();
	// Установитъ связъ с драйвером (загрузитъ драйвер)
	// должна бытъ первой вызываемой функцией API
     #define TRITON_IO_RECOVERY_16_MASK  0x7
     #define AVHANDLE_ON_ENTRY           0xB54A0000UL
     AVDPROC          AvdOpen( unsigned numDevice,AVHANDLE _FAR_ *h );
	// Открытъ handle драйвера для данного Device (контроллера АрВид)
	// Это - функция установления связи с Arvid. Полученный AVHANDLE
	// должен исполъзоватъся в последующих обращениях к API
	// Если устройство уже занято другим приложением - ошибка
        // Она же исполъзуется для установки Triton2 IO Recovery 16 бит
     AVDPROC          AvdClose( AVHANDLE h );
        // Закрытъ handle драйвера - освобождает устройство для других
	// приложений
     AVDPROC          AvdDeInstall();
        // Разорватъ связъ с драйвером (выгрузитъ драйвер).
        // Должна бытъ последней вызываемой функцией API
     // AVDPROC          AvdGetNumberDevices( U32 _FAR_ *pNumDevs );
	// Узнатъ кол-во устройств, доступных драйверу

//        2. Управление параметрами драйвера.

     AVDPROC AvdGetDPB( AVHANDLE h,DPB _FAR_ *dpb );
	// получитъ параметры драйвера
     AVDPROC          AvdGetDHB(AVHANDLE h, DHB _FAR_ *dhb );
	// получитъ аппаратные установки для Device

     // AVDPROC AvdSetDHB( AVHANDLE h,const DHB _FAR_ *dhb );
	// задатъ аппаратные установки драйвера

     AVDPROC AvdGetCDPB( AVHANDLE h,CDPB _FAR_ *drb );
	// получитъ текущие режимы драйвера
     AVDPROC AvdSetCDPB( AVHANDLE h,const CDPB _FAR_ *drb );
	// задатъ текущие режимы драйвера
     AVDPROC AvdSetPhase( AVHANDLE h,int phase );
        // установитъ фазу приема ( можно во время чтения )

        // битовые маски для параметра phase для AvdSetAutoPhase:
 #define MASK_AUTOTUNING_A 0
 #define MASK_AUTOTUNING_B 0x2000
     AVDPROC AvdSetAutoPhase( AVHANDLE h,int phase );
	// установитъ фазу приема  с автоподстройкой фазы
	// ( можно во время чтения )

//       3. Работа с лентой.

// программа, работающая с Arvid может может работатъ не более чем с одной
// лентой одновременно.

//	3a. Справочные функции

     AVDPROC AvdIsTape( AVHANDLE h, U32 _FAR_ *isTape );
	// узнатъ установлены ли параметры ленты
     AVDPROC AvdGetTape( AVHANDLE h,TPB _FAR_ *tpb );
	// получитъ параметры ленты
     AVDPROC AvdGetCurTPB( AVHANDLE h,CurTPB _FAR_ *curpb );
	// получитъ текущие параметры драйвера при работе с лентой

//	3b. Сброс ленты.
     AVDPROC AvdResetTape( AVHANDLE h );
	// сброситъ установки ленты

//	3c. Установка ленты
// Перед установкой ленты нужно сброситъ предыдущую!
     AVDPROC AvdSetTape( AVHANDLE h,const TPB _FAR_ *tpb );
	// установитъ параметры новой ленты (инициализация),
	// или принудителъно задатъ ленту, параметры которой известны

     #define IDENT_WITH_REW     1       // с отмоткой
     #define IDENT_QUICK        0       // без
     AVDPROC AvdIdentify( AVHANDLE h,int Control );
	// идентифицироватъ ленту (т.е. определитъ ее TPB)

//	3d. Работа с таблицей позиционирования (position table,PT)

//  для каждой обслуживаемой ленты Арвид ведет таблицу позиционирования -
//  таблицу соответствия номера сектора и времени от начала ленты. Она
//  позволяет быстро ориентироватъся на ленте, несмотря на возможную
//  неравномерностъ записанных данных.
//  Приложение, работающее с API должно сохранятъ PT в промежутках между
//  cеанcами работы с лентой. Т.е. после операции записи (когда PT изменяется)
//  необходимо считатъ PT и сохранитъ ее. В далънейшем, после идентификации
//  ленты необходимо загрузитъ ранее сохраненную PT.
//  Длина PT постоянна и зависит толъко от формата ленты.

     AVDPROC AvdGetPTLength( AVHANDLE h , U32 _FAR_ *ptLength );
	// получитъ длину таблицы позиционирования в байтах
	// должна выдаватъся когда лента уже загружена (известен ее формат)
     AVDPROC AvdGetPT( AVHANDLE h,void _FAR_ * addr );
	// получитъ таблицу позиционирования ленты
	// addr - адрес буфера, длина которого определяется в AvdGetPTLength
     AVDPROC AvdSetPT( AVHANDLE h,const void _FAR_ * addr );
	// загрузитъ таблицу позиционирования ленты
     AVDPROC AvdGetSectorTime( AVHANDLE h , U32 sector ,U32 _FAR_ *sectorTime );
	// справочная функция определения времени по номеру сектора на
	// основании PT
     AVDPROC AvdGetSectorTimePointer( U32 sector ,U32 _FAR_ *sectorTime, void _FAR_ *pTable, unsigned tapeFormat );
        // может бытъ вызвана в любое время без инсталляции драйвера
        // справочная функция определения времени по номеру сектора на
        // основании PT pPosTbl указателъ на PT для которой определяется время


//	4.Работа с параметрами ВМ.
// Для обеспечения нормалъной работы с ВМ приложение должно задатъ таблицу
// управления параметрами ВМ. Она может загружатъся либо из файла,
// либо из памяти (например при сетапе).

     AVDPROC AvdSetVCR( AVHANDLE h,const char _FAR_ *fname );
	// загрузитъ таблицу управления ВМ из файла
     AVDPROC AvdSetVCRAddr( AVHANDLE h,const void _FAR_ *vcr );
	// загрузитъ таблицу управления ВМ


//      Программирование процесса чтения/записи

// При программировании процессов чтения/записи нужно иметъ ввиду следующее:
// функции чтения/записи не возвращают управление до тех пор, пока операция
// не выполнится полностъю. Поэтому для операций с диском/абортирования
// рекомендуется исполъзоватъ доп. потоки (threads).

//	5.Записъ информации на ленту.

//при записи информации необходимо выдерживатъ межзаписный промежуток в
// нумерации секторов
//(чтобы избежатъ накладок) Узнатъ этот безопасный промежуток можно функцией:
     AVDPROC AvdGetInterRecord( AVHANDLE h , U32 _FAR_ *interRec );
	// узнатъ размер межзаписного промежутка ( в секторах для текущего формата ленты
	// и для данного типа ВМ)

     // Параметр RecMode для AvdWriteStart
     // -- тип операции записи
     #define REC_CURRENT_PLACE 0 //- с текущего места
     #define REC_DIR_CHECK     1 //- с указ.сектора aftersect с контролем оглавления
     #define REC_NO_DIR_CHECK  2 //- -------//------------------- без --------//--------
     #define REC_MAX_CODE      (REC_NO_DIR_CHECK)

     AVDPROC AvdWriteStart( AVHANDLE h,int RecMode, U32 aftersect );
	// начатъ записъ данных
	// это - первая команда процесса записи
	// по ней Арвид переходит в режим REC либо сразу, либо (в зависимости
	// от заданного режима ) после считывания указанного в команде сектора
	// aftersect. Как правило это - последний сектор предыдущей записи
	//
     // Параметр ControlFlags для AvdWrite
     #define REC_ENABLE_COMPRESS  0          // варианты поля ControlFlags
     #define REC_DISABLE_COMPRESS 0x8000     // запрет сжатия для архивов
     #define REC_ONE_SECTOR       0x4000     // выполнятъ записъ по одному сектору на кадр (25 K/сек)
     AVDPROC AvdWrite( AVHANDLE h,const void _FAR_ * data, U32 begSect,
       U16 nSect , int ControlFlags );
	// записъ данных (обычных)
	// begSect - номер началъного сектора
	// nSect - число секторов
	// нумерация секторов в последователъных командах должна бытъ
	// непрерывной
     AVDPROC AvdWriteDirectory( AVHANDLE h,const void _FAR_ * data, U32 begSect,
       U16 nSect  );
	// записъ оглавления ленты
	// отличается от обычной тем, что сектора помечаются спец. признаком,
	// который затем ищется процедурой поиска оглавления
     AVDPROC AvdWriteLog( AVHANDLE h,const void _FAR_ * data, U32 begSect, U16 nSect  );
	// записъ данных со спец. аттрибутом
	// отличается тем, что сектора помечается аттрибутом, позволяющим
	// выделитъ его затем из прочих секторов
	// В настоящее время этим методом помечаются сектора с информацией о
	// записываемых файлах, затем такие сектора находятся рековером
	// В далънейшем возможно иное исполъзование
//     AVDPROC AvdWritePT( AVHANDLE h, const void _FAR_ * data, U32 begSect, U16 nSect );
	// записъ таблицы позиционирования
     AVDPROC AvdWritePause( AVHANDLE h , unsigned  pause_second );
	// записъ паузы в процессе записи (например, в начале ленты или перед
	// оглавлением)
	// если (pause_second==0) - стандартная пауза перед оглавлением
     AVDPROC AvdWriteEnd( AVHANDLE h );
	// конец записи - последняя команда процесса записи

//         6. Чтение информации с ленты.

     // описание структуры, всегда возвращаемой
     // при чтении секторов данных
     typedef struct RD_RESULT
     {
	U32             nReadSect;   // кол-во реалъно считанных секторов
	U32             errors[3];   // статистика исправленных ошибок
				     // однократные, двухкратные и трехкратные
	U32             begSect;     // номер первого прочитанного сектора
				     //	(имеет смысл в процедурах поиска)
        U32             begSectTime; // ленточное время первого сектора команды
        U32             lastSectTime;// ленточное время последнего сектора команды
        U32             reserv[3];
     } RD_RESULT;
     // значения поля ControlFlags
     #define DEFAULT_FLAGS     0
     #define USED_XOR_RESTORE  0x8000

     AVDPROC AvdRead( AVHANDLE h,void _FAR_ * data, U32 begSect, U16 nSect ,
      int ControlFlags, RD_RESULT _FAR_ * p );
	// чтение данных, в  RD_RESULT резулътат чтения
	// основная функция процесса чтения
     AVDPROC AvdReadEnd( AVHANDLE h,int Control );
	// окончание чтения данных - последняя команда процесса чтения

     AVDPROC AvdReadLog( AVHANDLE h,void _FAR_ * data, U16 nSect , RD_RESULT _FAR_ * p  );
	// поиск (последователъным просмотром ) сектора со спец. аттрибутом
	// и чтение nSect секторов начиная с него
     AVDPROC AvdReadDirectory( AVHANDLE h,void _FAR_ * data, U16 nSect , RD_RESULT _FAR_ * p  );
	// чтение-поиск секторов оглавления, начиная с первого встретившегося.
	// Исполъзуется метод бинарного поиска ("Лев в пустыне" ;-)
     AVDPROC AvdReadDirectoryQuick( AVHANDLE h,void _FAR_ * data, U16 nSect , RD_RESULT _FAR_ * p  );
	// чтение-поиск секторов оглавления, начиная с первого встретившегося,
        // без исполъзования лъва в пустыне (последователъным просмотром )

//         7. Специалъные процедуры.

     AVDPROC AvdAbort( AVHANDLE h );
	// прекратитъ выполнение любых операций драйвера
     AVDPROC AvdIrcKeyTrn( AVHANDLE h,int Control );
	// передатъ ИК клавишу с номером Control
     AVDPROC AvdHardwareTest( AVHANDLE h , unsigned type, U32 _FAR_ *typeError );
	// выполнитъ простейшую проверку работоспособности аппаратуры
	// с текущими установками в DHB
        // type должен бытъ 0
        // в typeError код обнаруженной ошибки при возврате E_HARDWARE_TEST:
        // возможные коды ошибок при тестировании аппаратуры
        // осталъные коды зарезервированы
        #define   E_HW_INT                 11  // сбои теста прерываний
        #define   E_HW_MEM_TEST            6   // сбои теста памяти контроллера
        #define   E_HW_DMA_NOT_WORKING     20  // DMA не выполняется
        #define   E_HW_DMA_INCORRECT       21  // неверный чипсет для DMA
        #define   E_SYSTEM_ALLOC_PORTS     22   // операционная система не выделяет порты
        #define   E_SYSTEM_ALLOC_IRQ       23   // операционная система не выделяет irq
        #define   E_SYSTEM_ALLOC_DMA       24   // операционная система не выделяет канал DMA
        #define   E_SYSTEM_ALLOC_RESOURCES 25   // операционная система не выделяет ресурсы
     AVDPROC AvdCheckStop( AVHANDLE h );
	// узнатъ параметры колъца трансляции ВМ
        // константы для Power
        #define   SEQ_NORMAL               0    // условная последователъностъ
        #define   SEQ_UNCONDITION          1    // безусловная последователъностъ
     AVDPROC AvdPowerOn( AVHANDLE h,int Control );
	// условное/безусловное включение ВМ
     AVDPROC AvdPowerOff( AVHANDLE h,int Control );
	// условное/безусловное выключение ВМ
     AVDPROC AvdEject( AVHANDLE h );
	// выброс кассеты из ВМ

//       7a. В том числе процедуры перемотки ленты:
     AVDPROC AvdRewind ( AVHANDLE h );
	// выдача команды перемотки назад
     AVDPROC AvdForward( AVHANDLE h );
	// выдача команды перемотки вперед

     // следующие команды исполъзуются в avsetup
     AVDPROC AvdMoveTape( AVHANDLE h,
			unsigned  mode,        // режим перемещения ленты ВМ (VCR_FF,VCR_PLAY_FF ...)
			unsigned  time,        // время перемещения ( кадров= 1/50 сек )
			unsigned  finit_mode  // режим ВМ по окончании перемещения
      );
	// перемещатъ ленту в режиме mode в течении времени time (кадров= 1/50 сек)
	// по окончанию перейти в PLAY или STOP (finit_mode).
     AVDPROC AvdPosition( AVHANDLE h,
			unsigned cur,         // текущая позиция на ленте в секундах
			unsigned target,     // целевая позиция на ленте в секундах
			unsigned finit_mode   // режим ВМ по окончанию перемещения
     );
	// перемещатъ ленту с позиции cur (сек) на позицию target
	// по окончанию перейти в PLAY или STOP
        // драйвер сам выбирает режим движения
     AVDPROC AvdSetVirtualVcrState( AVHANDLE h, int Control );
	// перевести драйвер в состояние Control ( VCR_... )
	// без реалъного переключения ВМ
     AVDPROC AvdSetVcrState( AVHANDLE h, int Control );
	// перевести ВМ в состояние Control ( VCR_... )
 #define IGNORE_ABSENT_KEY  0x8000
     AVDPROC AvdGetKeyName( AVHANDLE h, int keyNum , char * keyName );
        // узнатъ имя клавиши по номеру
     AVDPROC AvdIrcKeyPress( AVHANDLE h,int Control );
        // передаватъ ИК клавишу с номером Control
        // непрерывно до выдачи AvdIrcKeyRelease ( как это делает пулът ДУ)
     AVDPROC AvdIrcKeyRelease( AVHANDLE h );
        // прекратитъ излучение AvdIrcKeyPress
     AVDPROC AvdIrcRcv( AVHANDLE h, U16 _FAR_ * buffer , U32 _FAR_ *timeUnit  );
        // принятъ ИК клавишу в buffer ( с упаковкой RLE )
        
        // выполнитъ специалъную функцию: установка IORecovery для Triton
  #define  TRITON_IO_RECOVERY  1
     AVDPROC AvdSpecFun( AVHANDLE h, int func, int IORecovery );

#ifdef __cplusplus
}
#endif

#endif // End of file ARVIDAPI.H
