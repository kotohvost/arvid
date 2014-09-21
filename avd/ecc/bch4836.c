
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "bch4836.h"

/* static int m = 6, n = 63, k = 36, t = 2, d = 5, length = 48; */

/* Primitive irreducible polynomial g(x) of degree 6 */
/*  p[0] = p[1] = p[6] = 1;
 *  p[2] = p[3] = p[4] = p[5] = 0; */

static int p[7] = { 1, 1, 0, 0, 0, 0, 1 };
static int Alpha_to[1 << MM], Index_of[1 << MM], g[RDNC + 1];

static int vwcount = 0;
static float MaxW = 0, MinW = (float)10E12;

static void vw (float CurPar)
{
  float absPar = (float)fabs (CurPar);

  if (CurPar < 0)
    printf ("CurPar: %f\n", CurPar);

  if (absPar > MaxW)
   {
    MaxW = absPar;
    printf ("Max_vw %08d: %f\n", vwcount++, CurPar);
    return;
   }

  if (absPar < MinW)
   {
    MinW = absPar;
    printf ("miN_vw %08d: %f\n", vwcount++, CurPar);
    return;
   }
}

/* Compute x % NN, where NN is 2**MM - 1,
 * without a slow divide */

static inline gf modnn (int x)
{
  vw (x); /* 188. 1.*/
  while (x >= NN)
   {
    x -= NN;
    x = (x >> MM) + (x & NN);
   }

  return x;
}

static gf modn  [0x100];
static gf amodn [0x100];

/* Compute x % NN, where NN is 2**MM - 1, without a slow divide */
static void gen_modnn (void)
{
  int i;

  for (i = 0; i < 0x100; i++)
    modn [i] = i % NN;
}

static void gen_amodnn (void)
{
  int i;

  for (i = 0; i < 0x100; i++)
    amodn [i] = Alpha_to[i % NN];
}

/* generate GF(2**MM) from the irreducible polynomial p(X) in p[0]..p[MM]
 * lookup tables:  index->polynomial form   Alpha_to[] contains j = alpha**i;
 * polynomial form -> index form  index_of[j = alpha**i] = i alpha=2 is the
 * primitive element of GF(2**MM) */

static void gen_gf ()
{
  register int i, mask = 1;
  Alpha_to[MM] = 0;

  for (i = 0; i < MM; i++)
   {
    Alpha_to[i] = mask;
    Index_of[Alpha_to[i]] = i;

    if (p[i] != 0)
      Alpha_to[MM] ^= mask;

    mask <<= 1;
   }

  Index_of[Alpha_to[MM]] = MM;
  mask >>= 1;

  for (i = MM + 1; i < NN; i++)
   {
    if (Alpha_to[i - 1] >= mask)
      Alpha_to[i] = Alpha_to[MM] ^ ((Alpha_to[i - 1] ^ mask) << 1);
    else
      Alpha_to[i] = Alpha_to[i - 1] << 1;

    Index_of[Alpha_to[i]] = i;
   }

  Index_of[0] = -1;
}

/* Compute generator polynomial of BCH code of LENGTH = 48, redundancy = 12
 * (OK, this is not very efficient, but we only do it once, right? :) */

static void gen_poly ()
{
  register int i, j, q, kaux;
  int test, aux, nocycles, root, noterms, rdncy;
  int cycle[RDNC + 1][7], size[RDNC + 1], min[RDNC + 1], zeros[RDNC + 1];

  /* Generate cycle sets modulo 63 */
  cycle[0][0] = 0; size[0] = 1;
  cycle[1][0] = 1; size[1] = 1;
  j = 1; /* cycle set index */

  do
   {
    /* Generate the j-th cycle set */
    i = 0;

    do
     {
      i++;
      cycle[j][i] = modn [cycle[j][i - 1] * 2];
      size[j]++;
      aux = modn [cycle[j][i] * 2];
     }
    while (aux != cycle[j][0]);

    /* Next cycle set representative */
    q = 0;

    do
     {
      q++;
      test = 0;

      for (i = 1; ((i <= j) && (!test)); i++)
	/* Examine previous cycle sets */
	for (kaux = 0; ((kaux < size[i]) && (!test)); kaux++)
	  if (q == cycle[i][kaux])
	    test = 1;
     }
    while ((test) && (q < (NN - 1)));

    if (!(test))
     {
      j++; /* next cycle set index */
      cycle[j][0] = q;
      size[j] = 1;
     }
   }
  while (q < (NN - 1));

  nocycles = j; /* number of cycle sets modulo NN */

  /* Search for roots 1, 2, ..., DD-1 in cycle sets */
  kaux = 0;
  rdncy = 0;

  for (i = 1; i <= nocycles; i++)
   {
    min[kaux] = 0;

    for (j = 0; j < size[i]; j++)
      for (root = 1; root < DD; root++)
	if (root == cycle[i][j])
	  min[kaux] = i;

    if (min[kaux])
     {
      rdncy += size[min[kaux]];
      kaux++;
     }
   }

  noterms = kaux;
  kaux = 1;

  for (i = 0; i < noterms; i++)
    for (j = 0; j < size[min[i]]; j++)
     {
      zeros[kaux] = cycle[min[i]][j];
      kaux++;
     }

#ifdef ORIGINAL_TEST
  printf ("This is a (%d, %d, %d) binary BCH code\n", LENGTH, KK, DD);
#endif

  /* Compute generator polynomial */
  g[0] = Alpha_to[zeros[1]];
  g[1] = 1; /* g(x) = (X + zeros[1]) initially */

  for (i = 2; i <= rdncy; i++)
   {
    g[i] = 1;

    for (j = i - 1; j > 0; j--)
      if (g[j] != 0)
        g[j] = g[j - 1] ^ amodn [Index_of[g[j]] + zeros[i]];
        // g[j] = g[j - 1] ^ Alpha_to[modnn (Index_of[g[j]] + zeros[i])];
      else
        g[j] = g[j - 1];

    // g[0] = Alpha_to[modnn (Index_of[g[0]] + zeros[i])];
    g[0] = amodn [Index_of[g[0]] + zeros[i]];
   }

#ifdef ORIGINAL_TEST
  printf ("g(x) = ");

  for (i = 0; i <= rdncy; i++)
   {
    printf ("%d", g[i]);

    if (i && ((i % 70) == 0))
      printf ("\n");
   }

  printf ("\n");
#endif
}

void init_bch (void)
{
  gen_gf (); /* generate the Galois Field GF(2**m) */
  gen_modnn ();
  gen_amodnn ();
  gen_poly ();	  /* Compute the generator polynomial of BCH code */
}

/* Calculate redundant bits bb[], codeword is c(X) = data(X)*X**(NN-KK)+ bb(X) */

void encode_bch (int data[], int bb[])
{
  register int i, j, feedback;

  for (i = 0; i < LENGTH - KK; i++)
    bb[i] = 0;

  // memset (bb, 0, sizeof (bb[0]) * (LENGTH - KK));

  for (i = KK - 1; i >= 0; i--)
   {
    feedback = data[i] ^ bb[LENGTH - KK - 1];

    if (feedback != 0)
     {
      for (j = LENGTH - KK - 1; j > 0; j--)
        if (g[j] != 0)
	  bb[j] = bb[j - 1] ^ feedback;
	else
	  bb[j] = bb[j - 1];

	bb[0] = g[0] && feedback;
     }
    else
     {
      for (j = LENGTH - KK - 1; j > 0; j--)
	bb[j] = bb[j - 1];

      bb[0] = 0;
     }
   }
}

/* We do not need the Berlekamp algorithm to decode.
 * We solve before hand two equations in two variables. */

int decode_bch (int recd[])
{
  register int i, j, offset, q;
  int elp[3], s[5], s3, aux;
  int count = 0, syn_error = 0;
  int loc[3], reg[3]; /* err[3], */

  /* first form the syndromes */
#ifdef ORIGINAL_TEST
  printf ("s[] = (");
#endif

  for (i = 1; i <= 4; i++)
   {
    s[i] = offset = 0;

    for (j = 0; j < LENGTH; j++)
     {
      if (recd[j])
        s[i] ^= amodn [offset];
	// s[i] ^= amodn [i * j];
        // s[i] ^= Alpha_to[modnn (i * j)];

      offset += i;
     }

    if (s[i] != 0)
      syn_error = 1; /* set flag if non-zero syndrome */

    /* NOTE: If only error detection is needed,
     * then exit the program here... */

    /* convert syndrome from polynomial form to index form */
    s[i] = Index_of[s[i]];

#ifdef ORIGINAL_TEST
    printf ("%3d ", s[i]);
#endif
   }

#ifdef ORIGINAL_TEST
  printf (")\n");
#endif

  if (syn_error)
   {
    /* If there are errors, try to correct them */

    if (s[1] != -1)
     {
      s3 = modn [s[1] + s[1] + s[1]];

      /* Was it a single error ? */
      if (s[3] == s3) 
       {
#ifdef ORIGINAL_TEST
	printf ("One error at %d\n", s[1]);
#endif
	recd[s[1]] ^= 1; /* Yes: Correct it */
       }
      else
       {
       	/* Assume two errors occurred and solve
       	 * for the coefficients of sigma(x), the
	 * error locator polynomail */

        if (s[3] != -1)
          aux = Alpha_to[s3] ^ Alpha_to[s[3]];
        else
          aux = Alpha_to[s3];

	elp[0] = 0;
	elp[1] = modn [s[2] - Index_of[aux] + NN];
	elp[2] = modn [s[1] - Index_of[aux] + NN];

#ifdef ORIGINAL_TEST
	printf ("sigma(x) = ");

	for (i = 0; i <= 2; i++)
	  printf ("%3d ", elp[i]);

	printf ("\n");
	printf ("Roots: ");
#endif

	/* find roots of the error location polynomial */
/*	for (i = 1; i <= 2; i++)
	  reg[i] = elp[i]; */

	reg[1] = elp[1];
	reg[2] = elp[2];
	count = 0;

	for (i = 1; i <= NN; i++)
	 {
	  /* Chien search */
	  q = 1;

/*	  for (j = 1; j <= 2; j++)
	    if (reg[j] != -1)
	     {
	      reg[j] = modn [reg[j] + j];
	      q ^= Alpha_to[reg[j]];
	     } */

	  if (reg[1] != -1)
	   {
	    reg[1] = modn [reg[1] + 1];
	    q ^= Alpha_to[reg[1]];
	   }

	  if (reg[2] != -1)
	   {
	    reg[2] = modn [reg[2] + 2];
	    q ^= Alpha_to[reg[2]];
	   }

	  if (!q)
	   { /* store error location number indices */
	    loc[count] = modn [i];
	    count++;
#ifdef ORIGINAL_TEST
	    printf ("%3d ", modn [i]);
#endif
	   }
	 }

#ifdef ORIGINAL_TEST
	printf ("\n");
#endif
	if (count == 2)
	 {
	  /* no. roots = degree of elp hence 2 errors */
/*	  for (i = 0; i < 2; i++)
	    recd[loc[i]] ^= 1; */

          recd[loc[0]] ^= 1;
          recd[loc[1]] ^= 1;
	 }
	else /* Cannot solve: Error detection */
	 {
#ifdef ORIGINAL_TEST
	  printf ("incomplete decoding\n");
#endif
          return -1;
         }
       }
     }
    else
    if (s[2] != -1) /* Error detection */
     {
#ifdef ORIGINAL_TEST
      printf ("incomplete decoding\n");
#endif
      return -1;
     }
   } /* if syn error */

  return count;
}

void free_bch (void) {}

#ifdef ORIGINAL_TEST

#include <math.h>

/* static int recd[LENGTH], data[KK], bb[RDNC + 1]; 
 * static int numerr, errpos[1 << MM], decerror = 0; */

int main (int argc, char **argv)
{
  int data[KK], bb[RDNC + 1], recd[LENGTH], errpos[1 << MM];
  int i, numerr, decerror = 0;

  init_bch ();
  srandom (1);

  /* Randomly generate DATA */
  for (i = 0; i < KK; i++)
    data[i] = (random () & 67108864) >> 26;

  /* ENCODE */
  encode_bch (data, bb); /* encode data */
 
  for (i = 0; i < LENGTH - KK; i++)
    recd[i] = bb[i]; /* first (LENGTH - KK) bits are redundancy */

  for (i = 0; i < KK; i++)
    recd[i + LENGTH - KK] = data[i]; /* last KK bits are data */

  printf ("c(x) = ");

  for (i = 0; i < LENGTH; i++)
   {
    printf ("%1d", recd[i]);

    if (i && ((i % 70) == 0))
      printf ("\n");
   }

  printf ("\n");

  /* ERRORS */
  printf ("Enter the number of errors and their positions: ");

  scanf ("%d", &numerr);

  for (i = 0; i < numerr; i++)
   {
    scanf ("%d", &errpos[i]);
    recd [errpos[i]] ^= 1;
   }

  printf("r(x) = ");

  for (i = 0; i < LENGTH; i++)
    printf ("%1d", recd[i]);

  printf ("\n");

  /* DECODE */
  decerror = decode_bch (recd);

  /* print out original and decoded data */
  printf ("Results:\n");
  printf ("original data = ");

  for (i = 0; i < KK; i++)
    printf ("%1d", data[i]);

  printf ("\nrecovered data = ");
  for (i = LENGTH - KK; i < LENGTH; i++)
    printf("%1d", recd[i]);

  printf("\n");

  if (decerror == -1)
    printf ("BCH decoder return failure decoding code!!!\n");
  else
  if (decerror > 1)
    printf ("BCH decoder detect %d decoding errors!!!\n", decerror);
  else
    printf ("BCH decoder return successfull decoding code.\n");

  /* decoding errors: we compare only the data portion */
  for (decerror = 0, i = LENGTH - KK; i < LENGTH; i++)
    if (data[i - LENGTH + KK] != recd[i])
      decerror++;

  if (decerror)
    printf ("%d message decoding errors!!!\n", decerror);
  else
    printf ("Succesful decoding\n");
}

#endif
