#ifndef __COMMON_H

#define __COMMON_H
typedef unsigned long   dword;
typedef unsigned char   byte;
typedef unsigned long   poly;           /* CRC polynomial */
typedef struct {
    poly                hi;             /* degree <= 63 */
    poly                lo;
    }                   long_poly;
#if defined(WS_16)||defined(WS_32)
#if defined(WS_16)
typedef int twobytes;
typedef unsigned int utwobytes;
#endif
#if defined(WS_32)
typedef short twobytes;
typedef unsigned short utwobytes;
#endif
#else
#ifdef BCOS2
typedef short twobytes;
typedef unsigned short utwobytes;
#else
#if WATC
typedef short twobytes;
typedef unsigned short utwobytes;
#else
#if defined(AIX) || defined(aix)  /* jce */
typedef short twobytes;
typedef unsigned short utwobytes;
#else
typedef int twobytes;
typedef unsigned int utwobytes;
#endif
#endif
#endif
#endif
#endif

#if defined(aix) || defined(AIX)   /* jce */
#define HI_BYTE 0
#define LO_BYTE 3
#else
#define HI_BYTE 3
#define LO_BYTE 0

#define HDRMASK "YAKVAMPISHUCHEGOZHEBOLE."
#endif
