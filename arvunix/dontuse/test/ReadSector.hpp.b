// Чтение с ленты
#include "iostream.h"

class	ReadSector {
 public:
	u_char	maxsector;	/* макс. кол. секторов в текущем режиме */
 private:
	int	mode;		/* режим (SIZE100/SIZE200/SIZE325) */
	int	rawbflag;	/* флаг необходимости вызова ReadRaw() */
	u_char	*rawb;		/* указатель на RAW буфер */
//	u_char	startcode;	/* указатель на начало данных в кадре */
//	u_char	cursector;	/* текущий сектор в кадре */
 public:
	/* просто конструктор */
		ReadSector();
	/* конструктор с режимом работы */
		ReadSector(int speed);
	/* деструктор */
		~ReadSector() {
		cout << "ReadSector destructor\n";
		/* удалим если есть */
		if (rawb)
			delete rawb;
		};
	/* ПП чтения RAW (кадра) данных			*/
	/* Возвращает количество прочитанных байт	*/
	/* в последующем надо перевести в private	*/
	int	ReadKadr(int device) {
		int	tmp = 0;
		/* читаем в буфер */
		if (rawb) {
		/* читаем кадр */
			while(!(tmp = read(device, rawb, mode)));
		/* кадр прочитан */
			rawbflag = 1;
			}
		return (tmp);
		};
	/* ПП поиска начала кода в кадре */
	/* в последующем надо перевести в private */
//	u_int	FindCode();
	/* ПП декодирования сектора sector в текущем кадре */
	/* в последующем надо перевести в private */
//	int	DecSector(int sector);
	/*
	 * ПП чтения следующего сектора
	 * при необходимости вызывается ReadRaw()
	 */
//	int	ReadData(int device);
};

/* просто конструктор */
ReadSector::ReadSector()
{
	cout << "ReadSector constructor\n";
};

/* конструктор с режимом */
ReadSector::ReadSector(int speed)
{
	cout << "ReadSector constructor for mode: " << speed;
	switch(speed) {
		case SIZE100:
			maxsector = 4;	/* 100 K/s */
			break;
		case SIZE200:
			maxsector = 8;	/* 200 K/s */
			break;
		case SIZE325:
		default:
			/* for default */
			speed = SIZE325;
			maxsector = 13;	/* 325 K/s */
			break;
		} /* switch (speed) */
	mode = speed;
	rawbflag = 0;
	rawb = new u_char[speed];
	cout << "to " << mode << "\n";
};
