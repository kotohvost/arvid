#include "stdio.h"
#define EOPEN           1       // ������ �������� �����
#define EREAD           2       // ������ ������ �����
#define ETABLENONE      3       // ������� �����������
#define ENUMKEY         4       // ����� ������� �� �����

inline void errinf (int err, char *ErrMes)
{
  printf("ErrNum: %d, Mess: %s", err, ErrMes);
};
