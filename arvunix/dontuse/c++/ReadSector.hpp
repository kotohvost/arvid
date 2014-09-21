class	ReadSector {
 public:
	u_char	maxsector;	/* ����. ���. �������� � ������� ������ */
 private:
	int	rawbflag;	/* ���� ������������� ������ ReadRaw() */
	u_char	*rawb;		/* ��������� �� RAW ����� */
	u_char	startcode;	/* ��������� �� ������ ������ � ����� */
	u_char	cursector;	/* ������� ������ � ����� */
 public:
	/* ����������� */
		ReadSector() {
		printf("ReadSector constructor\n");
		maxsector = 8;	/* 200 K/s */
		rawb=new u_char[SIZE200];
		rawbflag = 0;
		cursector = 0;
		};
	/* ���������� */
		~ReadSector() {
		printf("ReadSector destructor\n");
		delete rawb;
		};
	/* �� ������ RAW ������ */
	/* � ����������� ���� ��������� � private */
	int	ReadRaw(int device) {
		int	tmp;
		while(!(tmp = read(device, rawb, SIZE200)));
		/* ���� �������� */
		rawbflag = 1;
		return (tmp);
		};
	/* �� ������ ������ ���� � ����� */
	/* � ����������� ���� ��������� � private */
	u_int	FindCode();
	/* �� ������������� ������� sector � ������� ����� */
	/* � ����������� ���� ��������� � private */
	int	DecSector(int sector);
	/*
	 * �� ������ ���������� �������
	 * ��� ������������� ���������� ReadRaw()
	 */
	int	ReadData(int device);
};
