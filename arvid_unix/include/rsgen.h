/* смещения таблиц в структуре XX */
#define	XX_two_pnt	_XX+0
#define	XX_two_err	_XX+528
#define	XX_gfr		_XX+1212
#define	XX_gff		_XX+1468
#define	XX_cd1		_XX+1724
#define	XX_cd2		_XX+2236
#define	XX_cd3		_XX+2748
#define	XX_cds1		_XX+3260
#define	XX_cds2		_XX+3772
#define	XX_cds3		_XX+4284
#define	XX_sd11		_XX+4796
#define	XX_sd21		_XX+5308
#define	XX_sd31		_XX+5820
#define	XX_sd12		_XX+6332
#define	XX_sd22		_XX+6844
#define	XX_sd32		_XX+7356
#define	XX_sd13		_XX+7868
#define	XX_sd23		_XX+8380
#define	XX_sd33		_XX+8892
#define	XX_sd14		_XX+9404
#define	XX_sd24		_XX+9916
#define	XX_sd34		_XX+10428
#define	XX_sd15		_XX+10940
#define	XX_sd25		_XX+11452
#define	XX_sd35		_XX+11964
#define	XX_pow2		_XX+12476
#define	XX_pow3		_XX+12732
#define _DecodRSC	_XX+12988
#define	_DecodRSG	_XX+12992
#define	_DecodRSN	_XX+12996
#define Decod_r1	_XX+13000
#define Decod_r2	_XX+13002
#define Decod_r3	_XX+13004
#define Decod_d1	_XX+13006
#define Decod_d2	_XX+13007
#define Decod_d3	_XX+13008
#define Decod_m1	_XX+13010
#define Decod_m2	_XX+13011
#define	Decod_e1	_XX+13012
#define	Decod_e2	_XX+13016
#define	Decod_e3	_XX+13020
#define	Decod_e4	_XX+13024
#define	Decod_xloc1	_XX+13028
#define	Decod_xloc2	_XX+13029
#define	Decod_xloc3	_XX+13030
#define	Decod_g1	_XX+13032
#define	Decod_g2	_XX+13033
#define	Decod_g3	_XX+13034
#define	Decod_b1	_XX+13036
#define	Decod_b2	_XX+13037
#define	Decod_b3	_XX+13038
#define	Decod_det1	_XX+13040
#define	Decod_det2	_XX+13041
#define	Decod_det3	_XX+13042
#define	Decod_y1	_XX+13044
#define	Decod_y2	_XX+13045
#define	Decod_y3	_XX+13046

#define	POLINOM	0x0000001d	/* полином для создания таблиц (поля)*/

#define	NRS	 28	/* длина информационной части кодовой группы */
#define	NR	  6	/* длина синдрома */
#define	NGR	149	/* число групп в перемежонном коде */

typedef	unsigned char	GF;
/* Элемент таблицы для разложения двучлена на множетели */
typedef struct {
	int	two;	/* число элементов для проверки */
	GF	*err;	/* указатель на массив элементов для данной
			   суммы показателей примитивных элементов ошибок */
} TR;

struct _SY {
	GF	begin[0];
	/* указатели для разложения двучлена */
	TR	two_pnt[(NRS+NR)*2-2];
	/* таблица для разложения двучлена */
	GF	two_err[((NRS+NR)/2+1)*((NRS+NR)/2+2)*2];

	GF	gfr[256];	/* goley fast rewing */
	GF	gff[256];	/* goley fast forward */
	/* таблицы кодирования */
	GF	cd1[512];	/* старший байт */
	GF	cd2[512];
	GF	cd3[512];
	GF	cds1[512];	/* младший байт */
	GF	cds2[512];
	GF	cds3[512];
	/* таблицы вычисления синдромов */
	/* в степени 1 */
	GF	sd11[512];
	GF	sd21[512];
	GF	sd31[512];
	/* в степени 2 */
	GF	sd12[512];
	GF	sd22[512];
	GF	sd32[512];
	/* в степени 3 */
	GF	sd13[512];
	GF	sd23[512];
	GF	sd33[512];
	/* в степени 4 */
	GF	sd14[512];
	GF	sd24[512];
	GF	sd34[512];
	/* в степени 5 */
	GF	sd15[512];
	GF	sd25[512];
	GF	sd35[512];

	GF	pow2[256];	/* таблица вторых степеней эл. поля */
	GF	pow3[256];	/* таблица третьих степеней эл. поля */
unsigned int	DecodRSC;	/* счетчик циклов для 200/325 Kb/s */
unsigned int	DecodRSG;	/* счетчик групп для [де]кодирования */
unsigned int	DecodRSN;	/* количество групп в кадре */
unsigned short	Decod_rX[3];	/* временные переменные */
	GF	Decod_dX[3];	/* временные переменные для 3-х кр. ошибки */
unsigned int	Decod_eX[4];	/* счетчики ошибок */
	GF	Decod_xX[3];	/* временные переменные для 2-х кр. ошибки */
	GF	Decod_gX[4];	/* вр. для 3-х кр. ошибки */
	GF	Decod_bX[4];	/* вр. для 3-х кр. ошибки */
	GF	Decod_detX[4];	/* вр. для 3-х кр. ошибки */
	GF	Decod_yX[4];	/* вр. для 3-х кр. ошибки */
};
