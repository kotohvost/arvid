#include "rs.h"

typedef	unsigned char	GF;

extern struct DCStError DCErrStat;

/* �/� ������������� ����� ���������� ��� 100 �/� */
extern int DecoderG(GF *data, GF *code, unsigned int start, unsigned int count);

/* �/� ������������� ����� ���������� ��� 200 � 325 �/� */
/* �/� ���������� 0 - ��� ������ */
extern int DecoderRS(GF *data, GF *code, unsigned int start, unsigned int count, unsigned int group);

/* �/� ����������� ����� ���������� ��� 200 � 325 �/� */
extern int CoderRS(GF *data, GF *code, unsigned int start, unsigned int count, unsigned int group);

/* ������� ����������� ����� CRC32. ������ � ������ ??? */
extern unsigned short crc16(unsigned char *data, int c);
extern unsigned int crc32(unsigned char *data, unsigned int count);
extern unsigned int crc32s(unsigned char *data, unsigned int count);
