#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <ios.h>
#if defined(TURBOC)
#include <bios.h>
#else
#include <time.h>
#endif
#define  POLYMATH

#if !defined(DEBUG)
#   define DEBUG               0
#else
#   include "dotracef.h"
#endif


#include "polymath.h"

/* #include "all.h" */

/*
 * 25Apr88 - Ported to IBM C/2 from TURBO-C.  Changes are enabled
 *           by defining compiler variable IBMC2.  CF Burton.
 * 27Apr88 - Modified build_crc() and crc_check() to calculate CRC
 *           left-justified - faster that way.  Also simplified
 *           algorithm for generating CRC lookup table in
 *           build_crc().  CF Burton.
 * 06May88 - Modified crc_check() to use (fast) assembler routine,
 *           crc_calc(), to generate residue.  Changed lookup
 *           table, crctab, from static so crc_calc() can access
 *           it.  Changes are enabled by defining compiler
 *           variable ASM.  CF Burton.
 * 18Jun88 - Modified for compact model.  CF Burton.
 * 23Oct88 - In new_poly(), added password generation of poly-
 *           nomial.  In crc_check(), input and output residue
 *           left-justified (to avoid extra shifts).  CF Burton.
 * 20Aug89 - Renamed build_crc() and crc_check() to crc_init() and
 *           crc_calc().  Modified crc_calc():  external routines must
 *           now call crc_init() to build the crc lookup table; also,
 *           crc_calc() and the assembler routine are now functionally
 *           identical (and have the same name), so when the assembler
 *           routine is used, crc_calc() is completed omitted, and the
 *           external routines call the assembler routine directly.
 *           Changed from compiler variables IBMC2 and ASM to TURBOC
 *           and NOASM (the inverse); IBMC2 and ASM are now the default.
 *           CF Burton.
 */


poly *   crc_init(poly p)
{
    /***************************************************************/
    /* Adapted from Charles Burton's 32-bit code in C-LANG PROCS.  */
    /* This code works for arbitrary degree <= 31.                 */
    /* By special-casing the degree calculation,                   */
    /* it will also work for degree 32                             */
    /* 27Apr88 - Simplified algorithm.  Left-justified values in   */
    /*           lookup table to make things easier on CRC-by-byte */
    /*           calculating routine (crc_check()).  CF Burton.    */
    /***************************************************************/

    poly            work;
    int             i;
    int             j;
    int             k;
    int             xor;
    int             degree;
    poly * crctab;

    /* allocate crc table */
	
    if ((crctab = new poly[256]) == NULL) {
       return(NULL);
    } /* endif */

    /* determine degree: */
    /* the position of the highest nonzero coefficient bit */
    for (degree = 31, work = bit(31);
         degree && !(work & p);
         --degree, work >>= 1) {
    } /* endfor */

#if DEBUG
    do_tracef(0,"Generating new CRC table for degree %d: %8lX\n", degree, p);
#endif

    p <<= 32-degree;             /* left justify the polynomial,
                                    and strip off the msb */

    /* Calculate the CRC-by-byte lookup table entries by doing CRC-by-bit */
    /* for all 256 input byte values.                                     */

    for (i=0; i<256; ++i) {      /* do for all 256 possible 1x8 vectors */
        j=i;                     /* copy 1x8 vector for work */
        work = 0L;               /* initialize entry in lookup table */
        for (k = 128; k > 0; k/=2) {
                                 /* do CRC from msb to lsb */
            xor = j/k ^ ((long)work < 0L);
                                 /* XOR input bit with residue msb */
            work <<= 1;          /* shift residue 1 bit left */
            if (xor) work ^= p;  /* if bits XORed, XOR poly into residue */
            j %= k;              /* remove bit from input byte */
        } /* endfor */
        crctab[i] = work;        /* save residue in lookup table */

//#if DEBUG
//        do_tracef(0," %08lX", work);
//        if ((i % 8) == 7)
//            do_trace(0,"");
//#endif

    } /* endfor */
    return(crctab);
}   /* build_crc */

/*******************************************************************/


poly    crc_calc(poly *crctab, poly crcval, byte *bufp, int nbytes)
{
    /***************************************************************/
    /* 27Apr88 - Calculate CRC value left-justified to allow       */
    /*           faster algorithm (looking forward to assembler    */
    /*           version of this function).  CF Burton.            */
    /* 06May88 - Modified to use (fast) assembler routine          */
    /*           crc_calc() to calculate residue.  CF Burton.      */
    /* 24Oct88 - Input and output residue (crcval) left-justified. */
    /*           CF Burton.                                        */
    /***************************************************************/

    union {                     /* working residue . . */
       poly dd;                 /* . . as polynomial */
       byte db[4];              /* . . as byte array */
    } crc;

    crc.dd = crcval;            /* initialize working residue */
    for (; nbytes; --nbytes) {

        crc.dd = (crc.dd << 8) ^ crctab[crc.db[HI_BYTE] ^ *bufp++];

    } /* endfor */              /* calculate new residue */
    return (crc.dd);            /* return residue to caller */

}   /* crc_calc */



/*******************************************************************/

/*******************************************************************/
/* The following routines are suitable for degree-31 polynomials   */
/* only.  31 is prime, as is 2**31 - 1; this simplifies the task   */
/* of determining irreducibility of a polynomial.                  */
/* The following theorems from algebra are relevant:               */
/*                                                                 */
/* x**(2**n) + x is the product of all irreducible polynomials     */
/* of degree k where k divides n.                                  */
/*                                                                 */
/* For n = 31, x**(2**31) + x is the product of all irreducible    */
/* polynomials of degree 1 or 31.  Therefore, if p(x) of degree 31 */
/* divides x**(2**31) + x, it's either irreducible or divisible    */
/* by one of the two irreducible degree 1 polynomials: x or x+1.   */
/*                                                                 */
/* Take care of x by factoring it out of x**(2**31) + x, and       */
/* testing p(x) against x**(2**31 - 1) + 1.  Alternatively, x      */
/* divides p(x) only if p(x) has no constant term.                 */
/*                                                                 */
/* If x+1 is a factor of p(x), then p(1) = 0, which means p(x)     */
/* has an even number of nonzero coefficients.                     */
/*                                                                 */
/* Hence, IF p(x) of degree 31 has a constant term, and an odd     */
/* number of nonzero coefficients, and it divides x**(2**31-1)+1,  */
/* THEN it's irreducible.                                          */
/*******************************************************************/

/*******************************************************************/

long_poly    poly_mul(poly p, poly q)
/****************************************************/
/* 32-bit factors multiplied to give 64-bit result  */
/* This is just your basic shift-and-add, except    */
/* here it's shift-and-XOR                          */
/****************************************************/
{
#if defined(TURBOC)
    long_poly   r = {0L, 0L};
    long_poly   s = {0L, 0L};
#else
    long_poly   r,s;

    r.hi = r.lo = s.hi = 0L;
#endif

    s.lo = p;

    for (; q; q >>= 1 ) {
        if (q & 1L) {
            r.lo ^= s.lo;
            r.hi ^= s.hi;
        }
        s.hi = (s.hi << 1) | ((s.lo & bit(31)) != 0);
        s.lo = (s.lo << 1);
    }

    return r;
}

int     oddparity(poly p)
/* An irreducible polynomial has an odd number of non-0 coefficients */
{
    int     bits_on = 0;
    poly    mask;

    for (mask = bit(31); mask; mask >>= 1) {
        if (mask & p) {
            ++bits_on;
        }
    } /* endfor */
    return (bits_on & 1);
}

/*******************************************************************/

poly    poly_div(long_poly p, poly q)
/* divide 32-bit q into 64-bit p, returning 32-bit remainder */
{
   /********************************************************************/
   /* if q(x) = x**31 + q30*x**30 + ... + q1*x + q0*1, then            */
   /* x**31 = q30*x**30 + ... + q0*1              mod q(x), and indeed */
   /* x**k  = q30*x**(k-1) + ... + q0*x**(k-31)   mod q(x)             */
   /* So, starting at high-order bit of long_poly p(x), repeatedly     */
   /* cancel leading powers of x by exclusive OR with appropriate      */
   /* multiple q(x)*x**k (the qotient would get the corresponding      */
   /* x**k bit set).  Finally, after pp.hi bits are cleared out        */
   /* (along with qq.hi), one last operation clears out bit 31 of      */
   /* pp.lo.  This is just the everyday CRC residue calculation.       */
   /********************************************************************/

    poly        mask;
#if defined(TURBOC)
    long_poly   pp = p;
    long_poly   qq = {0L, 0L};
#else
    long_poly   pp,qq;

    pp.hi = p.hi;
    pp.lo = p.lo;
    qq.lo = 0L;
#endif

    qq.hi = q;
    for (mask = bit(31); mask != 0L; mask >>= 1) {
        if (mask & pp.hi) {
            pp.hi ^= qq.hi;
            pp.lo ^= qq.lo;
        }
        qq.lo >>= 1;
        if (qq.hi & BIT0) {
            qq.lo |= bit(31);
        }
        qq.hi >>= 1;
    } /* endfor */

    if (pp.lo & bit(31)) {
        pp.lo ^= qq.lo;
    }

    return pp.lo;
}

/*******************************************************************/

int     irreducible(poly p)
{
    /**********************************************************/
    /* Determine if p(x) is irreducible. p(x) of degree 31 is */
    /* irreducible if x**(2**31) = x mod p(x).                */
    /* I.e., x**(2**31) + x is a multiple of p(x).
    /* Let y[k](x) = x**(2**k), and compute                   */
    /* y[k+1](x) mod p(x)  = (y[k](x))**2 mod p(x).           */
    /* We can start with k = 4, and run up to 31.             */
    /**********************************************************/

    int         i;
    poly        y;

    /* first dispose of the special cases */

    if (!(p & BIT0))                /* divisible by x ? */
        return 0;
    if (!oddparity(p))              /* divisible by x+1 ? */
        return 0;

    y = bit(16);            /* y(x) = x**16 = x**(2**4) */

    for (i = 5; i < 32; ++i) {  /* compute 2**i power of x, mod p*/
        y = poly_div( poly_mul(y, y), p);
    } /* endfor */
    /* we now have x**(2**31) mod p(x) */
    /* is this residue = x ? */

    return (y == bit(1));
}   /* irreducible */

/*******************************************************************/

poly    next_poly(poly p)
/* find the next irreducible polynomial 'after' p */
{
#   define pnext(q) ((q+2L) | bit(31) | BIT0)

    /* pnext just increments its argument, but insures that it's odd  */
    /* by setting bit 0, and insures it's degree 31 by setting bit(31) */

    for (p = pnext(p); !irreducible(p); p = pnext(p)) {
             /* loop control does it all */;
         }
    return p;
}   /* next_poly */

/*******************************************************************/

poly    new_poly(PWB *password)
/* generate a 'completely new' irreducible polynomial */
/* by messing with the 1st 8 bytes of the user-specified password */
{
    int i;                      /* working index */

    for (i=0; i<4; ++i)
      password->db[i+4] ^= password->db[i];
                                /* XOR 1st 4 bytes with last 4 to get
                                   initial value for polynomial */
    for(i=0; i<4; ++i)
#ifdef BIG_ENDIAN
      password->db[i]=password->db[7-i];
#else
      password->db[i]=password->db[i+4];
#endif

    return next_poly(password->dd);
                                /* adjust initial value and return */
}   /* new_poly */
