// -------------01-17-95 11:08am--------------
//           описание формата .VCR файла
// файл--описание констант, структур ИК команд
//--------------------------------------------

#ifndef __VCR_H__
#define _IRC_
#define  __VCR_H__
#define I16 signed short
#define U16 unsigned short

// функция для подсчета CRC .vcr файла
#if 0
#if !defined(__BORLANDC__)

inline U16 _stdcall TstSum3 ( U16 _FAR_ *p,unsigned nr )
{
  U16 sum=0;

  for ( unsigned i=0; i<nr; i++ )
   {
    U16 dt= p[i];

    if ( sum & 0x8000 )
      dt ^= 0x100b;

    sum= (sum << 1) ^ dt;
   }

  return sum;
}
#endif
#endif

#ifndef TRUE
 #define        TRUE  1
 #define        FALSE 0
#endif

#define FIRST_FORMAT  2         // формат .VCR файла
#define FIRM_LEN      20        // длина имени фирмы
#define VCR_LEN       16        // длина имени ВМ
#define MODULIR       0         // модулированная ИК-команда
#define NOT_MODULIR   1         // немодулированная ИК-команда
#define COD_LEN_OLD   100
#define COD_LEN       104       // мах длина упакованной ИК команды
#define KEY_LEN       10        // длина имени клавиши
#define CLIENT_NAME_LEN 30      // длина имени клиента
#define CLIENT_ADDR_LEN 70      // адрес клиента
#define KEY_NR          50      // диапазон номеров клавиш
#define FLAG_DELAY (KEY_NR)     // код клавиши задержка 1/3 сек (17 кадров)
                                // например 50 -1/3 сек 52-3/3 сек

#define DELAY_MAX   58          // 3 сек задержка мах
#define COP_ENDIF   59          // окончить выполнение IF
#define COP_IFOFF   60          // проверить ВМ выключен и если да, выполним IF
#define COP_IFTUNER 61          // проверить ВМ выдает картинку и если да, выполним IF
#define COP_IFON    62          // проверить ВМ включен и если да, выполним IF
#define MAX_KEYS    16          // кол-во клавиш на ВМ
#define FLAG_DOUBLE 0x40        // дублирование клавиш  64

#define S_END    0xff
#define S1_END   0xffff
#define T_TO_S   55/20
#define S_TO_T   20/55

// Примечание: отрицательный код клавиши означает выдачу репетитора
//        до конца действия данной клавиши

// заголовок ИК команды
#define MAX_TYPE  11
typedef struct
{
  U16 period;                   // период, мкс
  U16 type_pack[MAX_TYPE];      // кол-во периодов шубы
  U16 type_pause[MAX_TYPE];     // +1 - для записи в конце 0
} IRC_HEAD;

// упакованная клавиша
typedef unsigned char IRC_KEY[COD_LEN];

#define SEQ_LEN      6
#define LONG_SEQ_LEN 16
#define NR_STATE     10

   #define M_NORMAL    0
   #define M_STOP      M_NORMAL
   #define M_PLAY      1
   #define M_PLAY_FF   2
   #define M_PLAY_REW  3
   #define M_nPLAY_FF  4
   #define M_nPLAY_REW 5
   #define M_FF        6
   #define M_REW       7
   #define M_REC       8
   #define M_PAUSE     9

struct  VCR_STATE_OLD
{
  unsigned char begin[ SEQ_LEN ];                // перед включением
  unsigned char on   [ SEQ_LEN ];                // последовательность on
  unsigned char init [ SEQ_LEN ];                // инициализир. послед.
  unsigned char main [2][ NR_STATE ][ SEQ_LEN ]; // из STOP и PLAY
  unsigned char add  [ NR_STATE][2][ SEQ_LEN ];  // из других состояний
  unsigned char off  [ SEQ_LEN ];                // последовательность off
  unsigned char repeat[ 2*SEQ_LEN ];             // список клавиш с репетит.
  unsigned char rsrv [2][ SEQ_LEN ];
};

// структура таблицы переходов ВМ
struct VCR_STATE
{
  unsigned char begin[ LONG_SEQ_LEN ];           // перед включением
  unsigned char on   [ LONG_SEQ_LEN ];           // последовательность on
  unsigned char init [ LONG_SEQ_LEN ];           // инициализир. послед.
  unsigned char main [2][ NR_STATE ][ SEQ_LEN ]; // из STOP и PLAY
  unsigned char add  [ NR_STATE][2][ SEQ_LEN ];  // из других состояний
  unsigned char eject[ SEQ_LEN ];                // последовательность Eject
  unsigned char rsrv1[ LONG_SEQ_LEN-SEQ_LEN ];   // перед выключением
  unsigned char off  [ LONG_SEQ_LEN ];           // последовательность off
  unsigned char last [ LONG_SEQ_LEN ];           // после выключения
  unsigned char repeat[ 2*SEQ_LEN ];             // список клавиш с репетит.
  unsigned char rsrv [2][ LONG_SEQ_LEN ];
};


#define D_RIGHT_ 1
#define D_LEFT_  2

// таблица задержек, порогов, скоростей (зависящих от ВМ)
struct VCR_VAR
{
  U16 std_delay; // типовая задержка между клавишами
                 // параметры для расчета времени позиционирования
                 // расстояние до цели d=abs(Stag-Scur)
                 // d=V*Tрасч+Serr
                 // при setup_ировании: Serr=d-V*Tрасч
                 // при работе:         Tрасч=(d-Serr)/V
  U16 w_rad_ff;             // скорости движения об.*160/тик=0.374*160
  U16 w_rad_rew;            // скорости движения об.*160/тик=0.374*160
  U16 v_play_ff;            // кадр/тик
  U16 v_play_rew;
  U16 v_Nplay_ff;
  U16 v_Nplay_rew;
  I16 d_ff;                 // Serr для перемотки: смещение в секундах
  I16 d_rew;                // от расчета */
  I16 d_play_ff;            // Serr для PLAY:
                            //  смещение в tick от расчета по v
  I16 d_play_rew;
  I16 d_Nplay_ff;
  I16 d_Nplay_rew;
  U16 play_bord_ff;         // границы перехода tick
  U16 play_bord_rew;        // границы перехода tick
  U16 q_play_bord_ff;
  U16 q_play_bord_rew;
  U16 q_Nplay_bord_ff;
  U16 q_Nplay_bord_rew;
  U16 on_translate;         // границы трансляции  on/off
  U16 q_play_translate;     // границы трансляции
  U16 tv_ext_translate;     // границы трансляции  внешний вход/tv
  U16 lost;                 // потери при переходе в запись(тик)
  I16 disp;                 // смещение (тик)при переходе в запись
  U16 d_begin;              // задержка перед включением
  U16 d_on;                 // задержки при включении и выкл.
  U16 d_off;
  U16 d_init;               // задержка после инициализир. послед.
  unsigned char d_tbl[ NR_STATE ][ NR_STATE ]; // таблица задержек
  U16 zona;                 // гарантированный зазор в кадрах
  U16 rec_disp_type;        // откат ленты перед записью:
                            // 0 - не зависит от положения
                            // 1 - зависит (датчик на приемнике)
                            // 2 - зависит (датчик на передатчике)
  U16 empty;                // кол-во пустых кадров перед записью данных
  U16 k_line_ff;            // коеффициэнт ускорения перемотки вперед
                            // реально время ускорения в кадрах
  U16 k_line_rew;           // коеффициэнт ускорения перемотки назад
  U16 distance_ident;       // дистанция для отмотки назад перед идентификацией, сек
  U16 reserve[24];
};


#define REPEAT_KEY 0x8000   // репетитор--клавиша

// таблица ИК команд (зависят от типа и фирмы ВМ)
struct IRC_PAR
{
  U16 repeat;             // признак наличия репетотора
  U16 nr_repeat;          // кол-во повторов репетитора
  U16 nr_key;             // кол-во используемых клавиш
  unsigned char key_ref[ KEY_NR ]; // cсылки: номер клав.-позиция в списке
  IRC_HEAD irc_header;    // заголовок ИК команды
  I16 operStep;           // количество выполненных шагов установки (здесь есть резерв)
  I16 lengthTape;         // длина тестовой ленты (здесь есть резерв)
  U16 after_hold_play;
  U16 reserve[ 36 ];
  U16 irc_type;           // тип ИК-клавиш: модулированный/немодулированный
  IRC_KEY keys[ MAX_KEYS ]; // ИК key
};

struct VCR_TBL
{ // охватывается CRC
  U16 format;             // номер форматa .vcr файла
  U16 long_play;          // наличие лонг-PLAY резерв
  U16 lp_mode;            // режим работы SP/LP резерв
  U16 n_play;             // наличие очень быстрого PLAY
  struct VCR_STATE state; // таблицы переходов ИК команд
  struct IRC_PAR   ik;    // ИК команд параметры
  U16 check_sum;          // КС файла
  U16 len_add_section;    // длина дополнительной секции (байт)
               // после которой начинается текстовая информация
               // сейчас д.б. == 0
  struct VCR_VAR time[ 2 ]; // задержки ВМ SP
};

struct KEY_NAMES
{
  char key_names [MAX_KEYS-1][KEY_LEN];
};

struct VCR_INFO
{
  unsigned char firm_name[FIRM_LEN];          // имя фирмы
  unsigned char vcr_name [ VCR_LEN];          // имя магнитофона
  unsigned char client_name[CLIENT_NAME_LEN]; // имя клиента-изготовителя
  unsigned char client_addr[CLIENT_ADDR_LEN]; // адрес клиента-изготовителя
  unsigned char reserv[80];
//  unsigned char   comment[2];  // комментарий клиета (длина неограничена)
};

// файл .VCR состоит из следующих частей:
//   VCR_TBL охваченная подсчетом CRC, далее секция длиной len_add_section=0,
//       затем KEY_NAMES и VCR_INFO
struct VCR_ALL
{
  struct VCR_TBL    tbl;
  struct KEY_NAMES  keynames;
  struct VCR_INFO   info;
};

struct VCR
{
  long long      vcr_size; //
  struct VCR_ALL *vcr;     // один vcr на все
  struct RLE     *rle;     // один на всех
  struct IRC     *irc;     // один за всех
};

struct VCR *open_vcr (char *fname);
void close_vcr (struct VCR *vcr);

#endif  // __VCR_H__
