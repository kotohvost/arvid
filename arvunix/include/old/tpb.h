/* Tape Parametr Block */
struct	CTPB {
	char	*name;		// имя ленты
	u_int	id;		// уникальный номер ленты
	int	format;		// текущий формат ленты, зависит от ТП
	int	length;		// длина ленты
	int	time;		// текущее время на ленте
	u_int	sector;		// текущий сектор на ленте
};
