/* Tape Parametr Block */
struct	CTPB {
	char	*name;		// ��� �����
	u_int	id;		// ���������� ����� �����
	int	format;		// ������� ������ �����, ������� �� ��
	int	length;		// ����� �����
	int	time;		// ������� ����� �� �����
	u_int	sector;		// ������� ������ �� �����
};
