// -------------01-17-95 11:08am--------------
//           �������� ������� .VCR �����
// ����--�������� ��������, �������� �� ������
//--------------------------------------------

#ifndef __VCR_H__
#define _IRC_
#define  __VCR_H__
#define I16 signed short
#define U16 unsigned short

// ������� ��� �������� CRC .vcr �����
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

#define FIRST_FORMAT  2         // ������ .VCR �����
#define FIRM_LEN      20        // ����� ����� �����
#define VCR_LEN       16        // ����� ����� ��
#define MODULIR       0         // �������������� ��-�������
#define NOT_MODULIR   1         // ���������������� ��-�������
#define COD_LEN_OLD   100
#define COD_LEN       104       // ��� ����� ����������� �� �������
#define KEY_LEN       10        // ����� ����� �������
#define CLIENT_NAME_LEN 30      // ����� ����� �������
#define CLIENT_ADDR_LEN 70      // ����� �������
#define KEY_NR          50      // �������� ������� ������
#define FLAG_DELAY (KEY_NR)     // ��� ������� �������� 1/3 ��� (17 ������)
                                // �������� 50 -1/3 ��� 52-3/3 ���

#define DELAY_MAX   58          // 3 ��� �������� ���
#define COP_ENDIF   59          // �������� ���������� IF
#define COP_IFOFF   60          // ��������� �� �������� � ���� ��, �������� IF
#define COP_IFTUNER 61          // ��������� �� ������ �������� � ���� ��, �������� IF
#define COP_IFON    62          // ��������� �� ������� � ���� ��, �������� IF
#define MAX_KEYS    16          // ���-�� ������ �� ��
#define FLAG_DOUBLE 0x40        // ������������ ������  64

#define S_END    0xff
#define S1_END   0xffff
#define T_TO_S   55/20
#define S_TO_T   20/55

// ����������: ������������� ��� ������� �������� ������ ����������
//        �� ����� �������� ������ �������

// ��������� �� �������
#define MAX_TYPE  11
typedef struct
{
  U16 period;                   // ������, ���
  U16 type_pack[MAX_TYPE];      // ���-�� �������� ����
  U16 type_pause[MAX_TYPE];     // +1 - ��� ������ � ����� 0
} IRC_HEAD;

// ����������� �������
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
  unsigned char begin[ SEQ_LEN ];                // ����� ����������
  unsigned char on   [ SEQ_LEN ];                // ������������������ on
  unsigned char init [ SEQ_LEN ];                // �����������. ������.
  unsigned char main [2][ NR_STATE ][ SEQ_LEN ]; // �� STOP � PLAY
  unsigned char add  [ NR_STATE][2][ SEQ_LEN ];  // �� ������ ���������
  unsigned char off  [ SEQ_LEN ];                // ������������������ off
  unsigned char repeat[ 2*SEQ_LEN ];             // ������ ������ � �������.
  unsigned char rsrv [2][ SEQ_LEN ];
};

// ��������� ������� ��������� ��
struct VCR_STATE
{
  unsigned char begin[ LONG_SEQ_LEN ];           // ����� ����������
  unsigned char on   [ LONG_SEQ_LEN ];           // ������������������ on
  unsigned char init [ LONG_SEQ_LEN ];           // �����������. ������.
  unsigned char main [2][ NR_STATE ][ SEQ_LEN ]; // �� STOP � PLAY
  unsigned char add  [ NR_STATE][2][ SEQ_LEN ];  // �� ������ ���������
  unsigned char eject[ SEQ_LEN ];                // ������������������ Eject
  unsigned char rsrv1[ LONG_SEQ_LEN-SEQ_LEN ];   // ����� �����������
  unsigned char off  [ LONG_SEQ_LEN ];           // ������������������ off
  unsigned char last [ LONG_SEQ_LEN ];           // ����� ����������
  unsigned char repeat[ 2*SEQ_LEN ];             // ������ ������ � �������.
  unsigned char rsrv [2][ LONG_SEQ_LEN ];
};


#define D_RIGHT_ 1
#define D_LEFT_  2

// ������� ��������, �������, ��������� (��������� �� ��)
struct VCR_VAR
{
  U16 std_delay; // ������� �������� ����� ���������
                 // ��������� ��� ������� ������� ����������������
                 // ���������� �� ���� d=abs(Stag-Scur)
                 // d=V*T����+Serr
                 // ��� setup_��������: Serr=d-V*T����
                 // ��� ������:         T����=(d-Serr)/V
  U16 w_rad_ff;             // �������� �������� ��.*160/���=0.374*160
  U16 w_rad_rew;            // �������� �������� ��.*160/���=0.374*160
  U16 v_play_ff;            // ����/���
  U16 v_play_rew;
  U16 v_Nplay_ff;
  U16 v_Nplay_rew;
  I16 d_ff;                 // Serr ��� ���������: �������� � ��������
  I16 d_rew;                // �� ������� */
  I16 d_play_ff;            // Serr ��� PLAY:
                            //  �������� � tick �� ������� �� v
  I16 d_play_rew;
  I16 d_Nplay_ff;
  I16 d_Nplay_rew;
  U16 play_bord_ff;         // ������� �������� tick
  U16 play_bord_rew;        // ������� �������� tick
  U16 q_play_bord_ff;
  U16 q_play_bord_rew;
  U16 q_Nplay_bord_ff;
  U16 q_Nplay_bord_rew;
  U16 on_translate;         // ������� ����������  on/off
  U16 q_play_translate;     // ������� ����������
  U16 tv_ext_translate;     // ������� ����������  ������� ����/tv
  U16 lost;                 // ������ ��� �������� � ������(���)
  I16 disp;                 // �������� (���)��� �������� � ������
  U16 d_begin;              // �������� ����� ����������
  U16 d_on;                 // �������� ��� ��������� � ����.
  U16 d_off;
  U16 d_init;               // �������� ����� �����������. ������.
  unsigned char d_tbl[ NR_STATE ][ NR_STATE ]; // ������� ��������
  U16 zona;                 // ��������������� ����� � ������
  U16 rec_disp_type;        // ����� ����� ����� �������:
                            // 0 - �� ������� �� ���������
                            // 1 - ������� (������ �� ���������)
                            // 2 - ������� (������ �� �����������)
  U16 empty;                // ���-�� ������ ������ ����� ������� ������
  U16 k_line_ff;            // ����������� ��������� ��������� ������
                            // ������� ����� ��������� � ������
  U16 k_line_rew;           // ����������� ��������� ��������� �����
  U16 distance_ident;       // ��������� ��� ������� ����� ����� ��������������, ���
  U16 reserve[24];
};


#define REPEAT_KEY 0x8000   // ���������--�������

// ������� �� ������ (������� �� ���� � ����� ��)
struct IRC_PAR
{
  U16 repeat;             // ������� ������� ����������
  U16 nr_repeat;          // ���-�� �������� ����������
  U16 nr_key;             // ���-�� ������������ ������
  unsigned char key_ref[ KEY_NR ]; // c�����: ����� ����.-������� � ������
  IRC_HEAD irc_header;    // ��������� �� �������
  I16 operStep;           // ���������� ����������� ����� ��������� (����� ���� ������)
  I16 lengthTape;         // ����� �������� ����� (����� ���� ������)
  U16 after_hold_play;
  U16 reserve[ 36 ];
  U16 irc_type;           // ��� ��-������: ��������������/����������������
  IRC_KEY keys[ MAX_KEYS ]; // �� key
};

struct VCR_TBL
{ // ������������ CRC
  U16 format;             // ����� ������a .vcr �����
  U16 long_play;          // ������� ����-PLAY ������
  U16 lp_mode;            // ����� ������ SP/LP ������
  U16 n_play;             // ������� ����� �������� PLAY
  struct VCR_STATE state; // ������� ��������� �� ������
  struct IRC_PAR   ik;    // �� ������ ���������
  U16 check_sum;          // �� �����
  U16 len_add_section;    // ����� �������������� ������ (����)
               // ����� ������� ���������� ��������� ����������
               // ������ �.�. == 0
  struct VCR_VAR time[ 2 ]; // �������� �� SP
};

struct KEY_NAMES
{
  char key_names [MAX_KEYS-1][KEY_LEN];
};

struct VCR_INFO
{
  unsigned char firm_name[FIRM_LEN];          // ��� �����
  unsigned char vcr_name [ VCR_LEN];          // ��� �����������
  unsigned char client_name[CLIENT_NAME_LEN]; // ��� �������-������������
  unsigned char client_addr[CLIENT_ADDR_LEN]; // ����� �������-������������
  unsigned char reserv[80];
//  unsigned char   comment[2];  // ����������� ������ (����� ������������)
};

// ���� .VCR ������� �� ��������� ������:
//   VCR_TBL ���������� ��������� CRC, ����� ������ ������ len_add_section=0,
//       ����� KEY_NAMES � VCR_INFO
struct VCR_ALL
{
  struct VCR_TBL    tbl;
  struct KEY_NAMES  keynames;
  struct VCR_INFO   info;
};

struct VCR
{
  long long      vcr_size; //
  struct VCR_ALL *vcr;     // ���� vcr �� ���
  struct RLE     *rle;     // ���� �� ����
  struct IRC     *irc;     // ���� �� ����
};

struct VCR *open_vcr (char *fname);
void close_vcr (struct VCR *vcr);

#endif  // __VCR_H__
