/*##################################################################*/
/*                                                                  */
/* Program name:  polymath.h                                        */
/*                                                                  */
/* Package name:  autoverv.exe                                      */
/*                                                                  */
/* Description:                                                     */
/*                                                                  */
/*                                                                  */
/* Statement:     Licensed Materials - Property of IBM              */
/*                (c) Copyright IBM Corp. 1999                      */
/*                                                                  */
/* Author:        Andy Raybould                                     */
/*                                                                  */
/*                U.S. Government Users Restricted Rights - use,    */
/*                duplication or disclosure restricted by GSA ADP   */
/*                Schedule Contract with IBM Corp.                  */
/*                                                                  */
/*                                                                  */
/*##################################################################*/

#ifndef __POLYMATH_H
#define __POLYMATH_H

#if defined(_cplusplus) || defined(__cplusplus)
   extern "C" {
#endif

typedef unsigned long   dword;
typedef unsigned char   byte;
typedef unsigned long   polyn;

/************************************************************/
/* A polynomial of degree < 32 is represented as            */
/* a bit string of coefficients 32 bits long.               */
/* The high-order bit represents coefficient of x**31, and  */
/* the low-order bit represents coefficient of x**0 = 1.    */
/************************************************************/

#define BIT0    1L
#define bit(n)  (BIT0 << (n))
#define PWB_SIZE 9                      /* size of password buffer:
                                           - 1st 8 bytes of password
                                           - 1 byte for end-of-string */

typedef union {                         /* password buffer */
           byte db[PWB_SIZE];           /* bytes of password */
           polyn dd;                     /* polynomial (1st 4 bytes) */
        }               PWB;

/*******************************************************************/
polyn *  crc_init(polyn p);
polyn    crc_calc(polyn * crctab, polyn crcval, byte *bufp, int nbytes);
int      irreducible(polyn p);      /* works only on degree 31 */
polyn    next_poly(polyn p);        /* works only on degree 31 */
polyn    new_poly(PWB *password);  /* works only on degree 31 */
/*******************************************************************/

#if defined(_cplusplus) || defined(__cplusplus)
   }   // extern "C" {
#endif

#endif

