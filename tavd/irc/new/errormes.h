#include "stdio.h"
#define EOPEN           1       // Ошибка открытия файла
#define EREAD           2       // Ошибка чтения файла
#define ETABLENONE      3       // Таблица отсутствует
#define ENUMKEY         4       // Номер клавиши не верен

inline void errinf (int err, char *ErrMes)
{
  printf("ErrNum: %d, Mess: %s", err, ErrMes);
};
