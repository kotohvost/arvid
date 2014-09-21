/*
 * defined names for Reed-Solomon
 * error structure
 */
#define	POLINOM	0x0000001d	/* ������� ��� �������� ������ (����)*/

#define	NRS	 28	/* ����� �������������� ����� ������� ������ */
#define	NR	  6	/* ����� �������� */
#define	NGR	149	/* ����� ����� � ������������ ���� */
#define	NRS50	 44	/* ����� �������������� ����� ������� ������ */
#define	NR50	  6	/* ����� �������� */
#define	NGR50	152	/* ����� ����� � ������������ ���� */

struct DCStError {
	unsigned int	one;	/* +00 */
	unsigned int	two;	/* +04 */
	unsigned int	tree;	/* +08 */
	unsigned int	fatal;	/* +12 */
};
