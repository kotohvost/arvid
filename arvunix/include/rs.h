/*
 * defined names for Reed-Solomon
 * error structure
 */
#define	POLINOM	0x0000001d	/* полином для создания таблиц (поля)*/

#define	NRS	 28	/* длина информационной части кодовой группы */
#define	NR	  6	/* длина синдрома */
#define	NGR	149	/* число групп в перемежонном коде */
#define	NRS50	 44	/* длина информационной части кодовой группы */
#define	NR50	  6	/* длина синдрома */
#define	NGR50	152	/* число групп в перемежонном коде */

struct DCStError {
	unsigned int	one;	/* +00 */
	unsigned int	two;	/* +04 */
	unsigned int	tree;	/* +08 */
	unsigned int	fatal;	/* +12 */
};
