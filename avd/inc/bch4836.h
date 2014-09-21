
#ifndef __BCH4836_H__
#define __BCH4836_H__

/*
 * File:    bch4836.h
 * Author:  Robert Morelos-Zaragoza
 *
 * %%%%%%%%%%% Encoder/Decoder for a (48, 36, 5) binary BCH code %%%%%%%%%%%%%
 *
 *	This code is used in control channels for cellular TDMA in the U.S.A.
 *
 *	In this specific case, there is no need to use the Berlekamp-Massey
 *	algorithm, since the error locator polynomial is of at most degree 2.
 *	Instead, we simply solve by hand two simultaneous equations to give
 * 	the coefficients of the error locator polynomial in the case of two 
 *	errors. In the case of one error, the location is given by the first
 *	syndrome.
 *
 *	This program derivates from the original bch2.c, which was written
 *	to simulate the encoding/decoding of primitive binary BCH codes.
 *	Part of this program is adapted from a Reed-Solomon encoder/decoder
 *	program,  'rs.c', to the binary case. 
 *
 *	rs.c by Simon Rockliff, University of Adelaide, 21/9/89 
 *	bch2.c by Robert Morelos-Zaragoza, University of Hawaii, 5/19/92 
 *
 * COPYRIGHT NOTICE: This computer program is free for non-commercial purposes.
 * You may implement this program for any non-commercial application. You may 
 * also implement this program for commercial purposes, provided that you
 * obtain my written permission. Any modification of this program is covered
 * by this copyright.
 *
 * %%%% Copyright 1994 (c) Robert Morelos-Zaragoza. All rights reserved. %%%%%
 *
 * MM = order of the field GF(2**6) = 6
 * NN = 2**6 - 1 - 15 = 48 = length 
 * TT = 2 = error correcting capability 
 * DD = 2 * TT + 1 = 5 = designed minimum distance 
 * KK = NN - deg(g(x)) = 36 = dimension 
 * p[] = coefficients of primitive polynomial used to generate GF(2**6)
 * g[] = coefficients of generator polynomial, g(x)
 * Alpha_to [] = log table of GF(2**6)
 * Index_of [] = antilog table of GF(2**6)
 * data[] = coefficients of data polynomial, i(x)
 * bb[] = coefficients of redundancy polynomial ( x**(12) i(x) ) modulo g(x)
 * numerr = number of errors 
 * errpos[] = error positions 
 * recd[] = coefficients of received polynomial 
 * decerror = number of decoding errors (in MESSAGE positions) */

/* sl: restore some times 3-4 errors in offset group range from 0 to 11, 
 * and any only 2 errors in offset range from 12 to LENGTH */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define random  rand
#define srandom srand
#endif

#define MM     6
#define NN     ((1 << MM) - 1)
#define KK     51 /* 36 */
#define TT     2
#define DD     5
#define LENGTH NN /* (NN - 15) */
#define RDNC   (LENGTH - KK)

typedef unsigned char gf;

void init_bch (void);
void free_bch (void);

void encode_bch (int data[], int bb[]);
int  decode_bch (int recd[]);

#ifdef __cplusplus
}
#endif

#endif
