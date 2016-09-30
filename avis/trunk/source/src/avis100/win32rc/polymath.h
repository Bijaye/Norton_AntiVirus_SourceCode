#ifndef __POLYMATH_H
#define __POLYMATH_H

/************************************************************/
/* A polynomial of degree < 32 is represented as            */
/* a bit string of coefficients 32 bits long.               */
/* The high-order bit represents coefficient of x**31, and  */
/* the low-order bit represents coefficient of x**0 = 1.    */
/************************************************************/

#define BIT0    1L
#define bit(n)  (BIT0 << (n))
// ?? temp fix - copy will still overflow if larger password given
#define PWB_SIZE 10                      /* size of password buffer:
                                           - 1st 8 bytes of password
                                           - 1 byte for end-of-string */

#ifdef BIG_ENDIAN
   #define HI_BYTE 0
   #define LO_BYTE 3
#else
   #define HI_BYTE 3
   #define LO_BYTE 0
#endif

typedef unsigned char byte; 
typedef unsigned long poly;

typedef struct {
    poly                hi;             /* degree <= 63 */
    poly                lo;
    }                   long_poly;


typedef union {                         /* password buffer */
  byte db[PWB_SIZE];                    /* bytes of password */
  poly dd;                              /* polynomial (1st 4 bytes) */
}               PWB;

typedef union {                         /* password buffer */
  poly dd;
  byte db[4];
}               CRC;
 
/*******************************************************************/
poly *  crc_init(poly p);
poly    crc_calc(poly * crctab, poly crcval, byte *bufp, int nbytes);
int     irreducible(poly p);      /* works only on degree 31 */
poly    next_poly(poly p);        /* works only on degree 31 */
poly    new_poly(PWB *password);  /* works only on degree 31 */
long_poly poly_mul(poly, poly);
/*******************************************************************/

#endif
