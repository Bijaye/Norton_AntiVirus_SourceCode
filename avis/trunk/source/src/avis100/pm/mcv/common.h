/* common.h -- common defines and structures */

#ifndef __COMMON_H
#define __COMMON_H

typedef unsigned long   dword;
typedef unsigned char   byte;
typedef short unsigned int word; 
typedef unsigned long   poly;           /* CRC polynomial */
typedef struct {
    poly                hi;             /* degree <= 63 */
    poly                lo;
    }                   long_poly;

#if (!defined(WS_16) && !defined(WS_32))
#error Word size not defined -- fix the makefile
#endif

#if defined(WS_16)
typedef int twobytes;
typedef unsigned int utwobytes;
#endif

#if defined(WS_32)
typedef short twobytes;
typedef unsigned short utwobytes;
#endif

typedef struct {
   long           length;
   unsigned short time;
   unsigned short date;
   }  file_attrib;

#ifdef aix
#define HI_BYTE 0
#define LO_BYTE 3
#else
#define HI_BYTE 3
#define LO_BYTE 0
#endif

/* this stuff came from basict.h and defines some of the stuff from before with different
   names */

/* unsigned types */
#ifndef INT16
#define INT16 	short int
#endif
#ifndef INT32
#define INT32 	long int
#endif

/* Boolean types */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* just in case this isn't defined by the compiler */
#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAXINT
#ifdef WS_16
#define MAXINT (0x7fff)
#else
#define MAXINT (0x7fffffff)
#endif
#endif

#endif

