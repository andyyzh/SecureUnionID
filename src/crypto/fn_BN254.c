//
// Created by bytedance on 2020/12/8.
//
/*
	Licensed to the Apache Software Foundation (ASF) under one
	or more contributor license agreements.  See the NOTICE file
	distributed with this work for additional information
	regarding copyright ownership.  The ASF licenses this file
	to you under the Apache License, Version 2.0 (the
	"License"); you may not use this file except in compliance
	with the License.  You may obtain a copy of the License at

	  http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing,
	software distributed under the License is distributed on an
	"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
	KIND, either express or implied.  See the License for the
	specific language governing permissions and limitations
	under the License.
*/

/* AMCL mod p functions */
/* Small Finite Field arithmetic */
/* SU=m, SU is Stack Usage (NOT_SPECIAL Modulus) */

#include "fn_BN254.h"

/* Fast Modular Reduction Methods */

/* r=d mod m */
/* d MUST be normalised */
/* Products must be less than pR in all cases !!! */
/* So when multiplying two numbers, their product *must* be less than MODBITS+BASEBITS*NLEN */
/* Results *may* be one bit bigger than MODBITS */

/* convert to Montgomery n-residue form */
void FN_BN254_nres(FN_BN254 *y, BIG_256_56 x) {
    DBIG_256_56 d;
    BIG_256_56 r;
    BIG_256_56_rcopy(r, R2modn_BN254);
    BIG_256_56_mul(d, x, r);
    FN_BN254_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void FN_BN254_redc(BIG_256_56 x, FN_BN254 *y) {
    DBIG_256_56 d;
    BIG_256_56_dzero(d);
    BIG_256_56_dscopy(d, y->g);
    FN_BN254_mod(x, d);
}


/* reduce a DBIG to a BIG using Montgomery's no trial division method */
/* d is expected to be dnormed before entry */
/* SU= 112 */
void FN_BN254_mod(BIG_256_56 a, DBIG_256_56 d) {
    BIG_256_56 mdls;
    BIG_256_56_rcopy(mdls, CURVE_Order_BN254);
    BIG_256_56_monty(a, mdls, NConst_BN254, d);
}

/* test x==0 ? */
/* SU= 48 */
int FN_BN254_iszilch(FN_BN254 *x) {
    BIG_256_56 m, t;
    BIG_256_56_rcopy(m, CURVE_Order_BN254);
    BIG_256_56_copy(t, x->g);
    BIG_256_56_mod(t, m);
    return BIG_256_56_iszilch(t);
}

void FN_BN254_copy(FN_BN254 *y, FN_BN254 *x) {
    BIG_256_56_copy(y->g, x->g);
    y->XES = x->XES;
}

void FN_BN254_rcopy(FN_BN254 *y, const BIG_256_56 c) {
    BIG_256_56 b;
    BIG_256_56_rcopy(b, c);
    FN_BN254_nres(y, b);
}

/* Swap a and b if d=1 */
void FN_BN254_cswap(FN_BN254 *a, FN_BN254 *b, int d) {
    sign32 t, c = d;
    BIG_256_56_cswap(a->g, b->g, d);

    c = ~(c - 1);
    t = c & ((a->XES) ^ (b->XES));
    a->XES ^= t;
    b->XES ^= t;

}

/* Move b to a if d=1 */
void FN_BN254_cmove(FN_BN254 *a, FN_BN254 *b, int d) {
    sign32 c = -d;

    BIG_256_56_cmove(a->g, b->g, d);
    a->XES ^= (a->XES ^ b->XES) & c;
}

void FN_BN254_zero(FN_BN254 *x) {
    BIG_256_56_zero(x->g);
    x->XES = 1;
}

int FN_BN254_equals(FN_BN254 *x, FN_BN254 *y) {
    FN_BN254 xg, yg;
    FN_BN254_copy(&xg, x);
    FN_BN254_copy(&yg, y);
    FN_BN254_reduce(&xg);
    FN_BN254_reduce(&yg);
    if (BIG_256_56_comp(xg.g, yg.g) == 0) return 1;
    return 0;
}

/* output FP */
/* SU= 48 */
void FN_BN254_output(FN_BN254 *r) {
    BIG_256_56 c;
    FN_BN254_redc(c, r);
    BIG_256_56_output(c);
}

void FN_BN254_rawoutput(FN_BN254 *r) {
    BIG_256_56_rawoutput(r->g);
}

/* r=a*b mod Modulus */
/* product must be less that p.R - and we need to know this in advance! */
/* SU= 88 */
void FN_BN254_mul(FN_BN254 *r, FN_BN254 *a, FN_BN254 *b) {
    DBIG_256_56 d;

    if ((sign64) a->XES * b->XES > (sign64) FEXCESS_BN254_N) {
        FN_BN254_reduce(a);  /* it is sufficient to fully reduce just one of them < p */
    }

    BIG_256_56_mul(d, a->g, b->g);
    FN_BN254_mod(r->g, d);

    r->XES = 2;
}


/* multiplication by an integer, r=a*c */
/* SU= 136 */
void FN_BN254_imul(FN_BN254 *r, FN_BN254 *a, int c) {
    int s = 0;

    if (c < 0) {
        c = -c;
        s = 1;
    }

    //Montgomery
    BIG_256_56 k;
    FN_BN254 f;
    if (a->XES * c <= FEXCESS_BN254_N) {
        BIG_256_56_pmul(r->g, a->g, c);
        r->XES = a->XES * c;    // careful here - XES jumps!
    } else {
        // don't want to do this - only a problem for Montgomery modulus and larger constants
        BIG_256_56_zero(k);
        BIG_256_56_inc(k, c);
        BIG_256_56_norm(k);
        FN_BN254_nres(&f, k);
        FN_BN254_mul(r, a, &f);
    }

    if (s) {
        FN_BN254_neg(r, r);
        FN_BN254_norm(r);
    }
}

/* Set r=a^2 mod m */
/* SU= 88 */
void FN_BN254_sqr(FN_BN254 *r, FN_BN254 *a) {
    DBIG_256_56 d;

    if ((sign64) a->XES * a->XES > (sign64) FEXCESS_BN254_N) {
        FN_BN254_reduce(a);
    }

    BIG_256_56_sqr(d, a->g);
    FN_BN254_mod(r->g, d);
    r->XES = 2;
}

/* SU= 16 */
/* Set r=a+b */
void FN_BN254_add(FN_BN254 *r, FN_BN254 *a, FN_BN254 *b) {
    BIG_256_56_add(r->g, a->g, b->g);
    r->XES = a->XES + b->XES;
    if (r->XES > FEXCESS_BN254_N) {
        FN_BN254_reduce(r);
    }
}

/* Set r=a-b mod m */
/* SU= 56 */
void FN_BN254_sub(FN_BN254 *r, FN_BN254 *a, FN_BN254 *b) {
    FN_BN254 n;
    FN_BN254_neg(&n, b);
    FN_BN254_add(r, a, &n);
}

// https://graphics.stanford.edu/~seander/bithacks.html
// constant time log to base 2 (or number of bits in)

static int logb2(unsign32 v) {
    int r;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    v = v - ((v >> 1) & 0x55555555);
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
    r = (((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
    return r;
}

// find appoximation to quotient of a/m
// Out by at most 2.
// Note that MAXXES is bounded to be 2-bits less than half a word
static int quo(BIG_256_56 n, BIG_256_56 m) {
    int sh;
    chunk num, den;
    int hb = CHUNK / 2;
    if (TBITS_BN254_N < hb) {
        sh = hb - TBITS_BN254_N;
        num = (n[NLEN_256_56 - 1] << sh) | (n[NLEN_256_56 - 2] >> (BASEBITS_256_56 - sh));
        den = (m[NLEN_256_56 - 1] << sh) | (m[NLEN_256_56 - 2] >> (BASEBITS_256_56 - sh));
    } else {
        num = n[NLEN_256_56 - 1];
        den = m[NLEN_256_56 - 1];
    }
    return (int) (num / (den + 1));
}

/* SU= 48 */
/* Fully reduce a mod Modulus */
void FN_BN254_reduce(FN_BN254 *a) {
    BIG_256_56 m, r;
    int sr, sb, q;
    chunk carry;

    BIG_256_56_rcopy(m, CURVE_Order_BN254);

    BIG_256_56_norm(a->g);

    if (a->XES > 16) {
        q = quo(a->g, m);
        carry = BIG_256_56_pmul(r, m, q);
        r[NLEN_256_56 - 1] += (carry << BASEBITS_256_56); // correction - put any carry out back in again
        BIG_256_56_sub(a->g, a->g, r);
        BIG_256_56_norm(a->g);
        sb = 2;
    } else sb = logb2(a->XES - 1);  // sb does not depend on the actual data

    BIG_256_56_fshl(m, sb);

    while (sb > 0) {
// constant time...
        sr = BIG_256_56_ssn(r, a->g, m);  // optimized combined shift, subtract and norm
        BIG_256_56_cmove(a->g, r, 1 - sr);
        sb--;
    }

    //BIG_256_56_mod(a->g,m);
    a->XES = 1;
}

void FN_BN254_norm(FN_BN254 *x) {
    BIG_256_56_norm(x->g);
}

/* Set r=-a mod Modulus */
/* SU= 64 */
void FN_BN254_neg(FN_BN254 *r, FN_BN254 *a) {
    int sb;
    BIG_256_56 m;

    BIG_256_56_rcopy(m, CURVE_Order_BN254);

    sb = logb2(a->XES - 1);
    BIG_256_56_fshl(m, sb);
    BIG_256_56_sub(r->g, m, a->g);
    r->XES = ((sign32) 1 << sb) + 1;

    if (r->XES > FEXCESS_BN254_N) {
        FN_BN254_reduce(r);
    }

}

/* Set r=a/2. */
/* SU= 56 */
void FN_BN254_div2(FN_BN254 *r, FN_BN254 *a) {
    BIG_256_56 m;
    BIG_256_56_rcopy(m, CURVE_Order_BN254);
    FN_BN254_copy(r, a);

    if (BIG_256_56_parity(a->g) == 0) {

        BIG_256_56_fshr(r->g, 1);
    } else {
        BIG_256_56_add(r->g, r->g, m);
        BIG_256_56_norm(r->g);
        BIG_256_56_fshr(r->g, 1);
    }
}

void FN_BN254_pow(FN_BN254 *r, FN_BN254 *a, BIG_256_56 b) {
    sign8 w[1 + (NLEN_256_56 * BASEBITS_256_56 + 3) / 4];
    FN_BN254 tb[16];
    BIG_256_56 t;
    int i, nb;

    FN_BN254_norm(a);
    BIG_256_56_norm(b);
    BIG_256_56_copy(t, b);
    nb = 1 + (BIG_256_56_nbits(t) + 3) / 4;
    /* convert exponent to 4-bit window */
    for (i = 0; i < nb; i++) {
        w[i] = BIG_256_56_lastbits(t, 4);
        BIG_256_56_dec(t, w[i]);
        BIG_256_56_norm(t);
        BIG_256_56_fshr(t, 4);
    }

    FN_BN254_one(&tb[0]);
    FN_BN254_copy(&tb[1], a);
    for (i = 2; i < 16; i++)
        FN_BN254_mul(&tb[i], &tb[i - 1], a);

    FN_BN254_copy(r, &tb[w[nb - 1]]);
    for (i = nb - 2; i >= 0; i--) {
        FN_BN254_sqr(r, r);
        FN_BN254_sqr(r, r);
        FN_BN254_sqr(r, r);
        FN_BN254_sqr(r, r);
        FN_BN254_mul(r, r, &tb[w[i]]);
    }
    FN_BN254_reduce(r);
}

/* set w=1/x */
void FN_BN254_inv(FN_BN254 *w, FN_BN254 *x) {

    BIG_256_56 m2;
    BIG_256_56_rcopy(m2, CURVE_Order_BN254);
    BIG_256_56_dec(m2, 2);
    BIG_256_56_norm(m2);
    FN_BN254_pow(w, x, m2);
}

/* SU=8 */
/* set n=1 */
void FN_BN254_one(FN_BN254 *n) {
    BIG_256_56 b;
    BIG_256_56_one(b);
    FN_BN254_nres(n, b);
}

/* is r a QR? */
int FN_BN254_qr(FN_BN254 *r) {
    int j;
    BIG_256_56 m;
    BIG_256_56 b;
    BIG_256_56_rcopy(m, CURVE_Order_BN254);
    FN_BN254_redc(b, r);
    j = BIG_256_56_jacobi(b, m);
    FN_BN254_nres(r, b);
    if (j == 1) return 1;
    return 0;

}

/* Set a=sqrt(b) mod Modulus */
/* SU= 160 */
void FN_BN254_sqrt(FN_BN254 *r, FN_BN254 *a) {
    FN_BN254 v, i;
    BIG_256_56 b;
    BIG_256_56 m;
    BIG_256_56_rcopy(m, CURVE_Order_BN254);
    BIG_256_56_mod(a->g, m);
    BIG_256_56_copy(b, m);


    FN_BN254_copy(&i, a); // i=x
    BIG_256_56_fshl(i.g, 1); // i=2x
    BIG_256_56_dec(b, 5);
    BIG_256_56_norm(b);
    BIG_256_56_fshr(b, 3); // (p-5)/8
    FN_BN254_pow(&v, &i, b); // v=(2x)^(p-5)/8
    FN_BN254_mul(&i, &i, &v); // i=(2x)^(p+3)/8
    FN_BN254_mul(&i, &i, &v); // i=(2x)^(p-1)/4
    BIG_256_56_dec(i.g, 1);  // i=(2x)^(p-1)/4 - 1
    FN_BN254_mul(r, a, &v);
    FN_BN254_mul(r, r, &i);
    FN_BN254_reduce(r);

}


