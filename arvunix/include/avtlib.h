#include "rs.h"

typedef	unsigned char	GF;

extern struct DCStError DCErrStat;

/* П/п декодирования блока информации для 100 К/с */
extern int DecoderG(GF *data, GF *code, unsigned int start, unsigned int count);

/* П/п декодирования блока информации для 200 и 325 К/с */
/* п/п возвращает 0 - нет ошибок */
extern int DecoderRS(GF *data, GF *code, unsigned int start, unsigned int count, unsigned int group);

/* П/п кодирования блока информации для 200 и 325 К/с */
extern int CoderRS(GF *data, GF *code, unsigned int start, unsigned int count, unsigned int group);

/* Подсчет контрольной суммы CRC32. Размер в СЛОВАХ ??? */
extern unsigned short crc16(unsigned char *data, int c);
extern unsigned int crc32(unsigned char *data, unsigned int count);
extern unsigned int crc32s(unsigned char *data, unsigned int count);
