// ������ � �����
#include "iostream.h"

class	ReadSector {
 public:
	u_char	maxsector;	/* ����. ���. �������� � ������� ������ */
 private:
	int	mode;		/* ����� (SIZE100/SIZE200/SIZE325) */
	int	rawbflag;	/* ���� ������������� ������ ReadRaw() */
	u_char	*rawb;		/* ��������� �� RAW ����� */
//	u_char	startcode;	/* ��������� �� ������ ������ � ����� */
//	u_char	cursector;	/* ������� ������ � ����� */
 public:
	/* ������ ����������� */
		ReadSector();
	/* ����������� � ������� ������ */
		ReadSector(int speed);
	/* ���������� */
		~ReadSector() {
		cout << "ReadSector destructor\n";
		/* ������ ���� ���� */
		if (rawb)
			delete rawb;
		};
	/* �� ������ RAW (�����) ������			*/
	/* ���������� ���������� ����������� ����	*/
	/* � ����������� ���� ��������� � private	*/
	int	ReadKadr(int device) {
		int	tmp = 0;
		/* ������ � ����� */
		if (rawb) {
		/* ������ ���� */
			while(!(tmp = read(device, rawb, mode)));
		/* ���� �������� */
			rawbflag = 1;
			}
		return (tmp);
		};
	/* �� ������ ������ ���� � ����� */
	/* � ����������� ���� ��������� � private */
//	u_int	FindCode();
	/* �� ������������� ������� sector � ������� ����� */
	/* � ����������� ���� ��������� � private */
//	int	DecSector(int sector);
	/*
	 * �� ������ ���������� �������
	 * ��� ������������� ���������� ReadRaw()
	 */
//	int	ReadData(int device);
};

/* ������ ����������� */
ReadSector::ReadSector()
{
	cout << "ReadSector constructor\n";
};

/* ����������� � ������� */
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
