// DecPlat.h : Decomposer platform-specific global includes
// Copyright 1999 by Symantec Corporation.  All rights reserved.

#if defined(UNIX)
#include "platform.h"
#include "tcsapi.h"

///////// ENDIAN Definitions /////////////////////////// 

#if !defined(WENDIAN) || !defined(DWENDIAN)

    #ifdef  BIG_ENDIAN

        #define WENDIAN(w)   ( (WORD)((WORD)(w) >> 8) | (WORD)((WORD)(w) << 8) )
        #define DWENDIAN(dw) ( (DWORD)((DWORD)(dw) >> 24) | \
                               (DWORD)((DWORD)dw << 24)  | \
                               (DWORD)(((DWORD)(dw) >> 8) & 0xFF00U) | \
                               (DWORD)(((DWORD)(dw) << 8) & 0xFF0000UL) )

    #else   // if LITTLE_ENDIAN (default)

        #define WENDIAN(w)   (w)
        #define DWENDIAN(dw) (dw)

    #endif

#endif // #if !defined(WENDIAN) || !defined(DWENDIAN)

#define min(a, b)  (((a) < (b)) ? (a) : (b))
#define max(a, b)  (((a) > (b)) ? (a) : (b))
#endif // UNIX

