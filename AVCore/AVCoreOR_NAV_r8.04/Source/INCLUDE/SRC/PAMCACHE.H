// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/pamcache.h_v   1.2   15 May 1996 14:08:52   CNACHEN  $
//
// Description:
//
//  This file contains the structures for the cache
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/pamcache.h_v  $
// 
//    Rev 1.2   15 May 1996 14:08:52   CNACHEN
// Changed number of bytes to save @CS:IP from 8 to 128.
// 
//    Rev 1.1   04 May 1996 13:32:24   RSTANEV
// Changed CACHE_T so that it does not directly allocate space for mutex.
//
//    Rev 1.0   04 Mar 1996 16:22:02   CNACHEN
// Initial revision.
//
//    Rev 1.0   04 Mar 1996 16:02:10   CNACHEN
// Initial revision.
//
//************************************************************************

#ifndef _CACHE_H_

#define _CACHE_H_

#define CACHE_SAVE_CODE_LEN     128         /* changed from 8 */
#define CACHE_NUM_ENTRIES       32
#define CACHE_INVALID_ITER      0

#define CACHESTATUS_ENTRY_NOT_FOUND 0
#define CACHESTATUS_ENTRY_FOUND     1
#define CACHESTATUS_ERROR           2

typedef unsigned int CACHESTATUS;

typedef struct
{
    WORD    wCS;                            /* CS at given # of iterations */
    WORD    wIP;                            /* IP at given # of iterations */
    BYTE    byCode[CACHE_SAVE_CODE_LEN];    /* Opcode bytes from CS:IP onwards */
    DWORD   dwImageSize;                    /* image size */
    DWORD   dwEAX, dwEBX, dwECX, dwEDX,     /* register state */
            dwESI, dwEDI, dwESP, dwEBP;
    DWORD   dwNumWrites;                    /* Number of writes */
} CPU_STATE_T;

typedef CPU_STATE_T *LPCPU_STATE_T;

typedef struct
{
    DWORD           dwLRUIter;              /* 0 iteration means unused */
    CPU_STATE_T     stCPUState;             /* CPU state */

} CACHE_ENTRY_T;

typedef CACHE_ENTRY_T *LPCACHE_ENTRY_T;

typedef struct
{
#if !defined(SYM_NTK)
    LONG                lLocalMutex;
#endif
    LPLONG              lplMutex;
    CACHE_ENTRY_T       aCache[CACHE_NUM_ENTRIES];
    DWORD               dwLRUIter;
} CACHE_T;

typedef CACHE_T *LPCACHE;


#endif  /* _CACHE_H_ */

