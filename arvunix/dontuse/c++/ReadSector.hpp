class	ReadSector {
 public:
	u_char	maxsector;	/* макс. кол. секторов в текущем режиме */
 private:
	int	rawbflag;	/* флаг необходимости вызова ReadRaw() */
	u_char	*rawb;		/* указатель на RAW буфер */
	u_char	startcode;	/* указатель на начало данных в кадре */
	u_char	cursector;	/* текущий сектор в кадре */
 public:
	/* конструктор */
		ReadSector() {
		printf("ReadSector constructor\n");
		maxsector = 8;	/* 200 K/s */
		rawb=new u_char[SIZE200];
		rawbflag = 0;
		cursector = 0;
		};
	/* деструктор */
		~ReadSector() {
		printf("ReadSector destructor\n");
		delete rawb;
		};
	/* ПП чтения RAW данных */
	/* в последующем надо перевести в private */
	int	ReadRaw(int device) {
		int	tmp;
		while(!(tmp = read(device, rawb, SIZE200)));
		/* кадр прочитан */
		rawbflag = 1;
		return (tmp);
		};
	/* ПП поиска начала кода в кадре */
	/* в последующем надо перевести в private */
	u_int	FindCode();
	/* ПП декодирования сектора sector в текущем кадре */
	/* в последующем надо перевести в private */
	int	DecSector(int sector);
	/*
	 * ПП чтения следующего сектора
	 * при необходимости вызывается ReadRaw()
	 */
	int	ReadData(int device);
};
