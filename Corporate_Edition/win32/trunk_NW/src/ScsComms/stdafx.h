// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2004, 2005, Symantec Corporation, All rights reserved.
// stdafx.h

#ifndef __SCSCOMMS_PRECOMPILED_HEADER__
#define __SCSCOMMS_PRECOMPILED_HEADER__


// We're targeting NT 4 and 98 with IE 4

#define _WIN32_WINNT   0x0400
#define _WIN32_IE      0x0400


#define VC_EXTRALEAN                // Exclude rarely-used stuff from Windows headers

// Prior to the inclusion of WinSock2, set the hard-coded array size for Select.
// The default is 64, but we want to handle 1000's of concurrent conncetions.

#define FD_SETSIZE      256

#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

#include <winsock2.h>
#include <ws2tcpip.h>
#include <wsipx.h>
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <tchar.h>

// Use the C-RunTime's debug heap in debug builds.

#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif


// array_sizeof to help with element count as opposed to byte count.

#define array_sizeof(x) (sizeof(x) / sizeof(x[0]))

// array_end to help with fixed array iteration

#define array_end(x)    ((x) + array_sizeof(x))

// MIN and MAX macros

#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))


// This is the master header file for RSA's SSL-C.

// #define SSLC_SMALL_CODE // *WORK* Enable this to reduce  SSL-C footprint.
#include "sslc.h"

// Global handle to the SSL-C library.

extern R_LIB_CTX*  g_SSLCContext;
extern int         g_SSLCContextLoadErr;


// This is the master header file for RSA's Cert-C

#define CERTC_SW_PLATFORM   CERTC_SW_WINDOWS
#define CERTC_HW_PLATFORM   CERTC_HW_X86

#include "certc.h"
#include "rsacsp.h" // Crypto-C provider for Cert-C

// Global handle to the RSA Cert-C library.

extern CERTC_CTX g_CertCContext;


// This is the master header file for RSA's Crypto-C

#include "bsafe.h"


#ifdef CONN_PROFILING

// Time routines for profiling

#include <sys/timeb.h>

inline void AddTimeDiff( struct _timeb& total_time, struct _timeb& start_time, struct _timeb& end_time )
{
    if( end_time.millitm < start_time.millitm )
    {
        total_time.time    += (end_time.time - 1) - start_time.time;
        total_time.millitm += (end_time.millitm + 1000) - start_time.millitm;
    }
    else
    {
        total_time.time    += end_time.time - start_time.time;
        total_time.millitm += end_time.millitm - start_time.millitm;
    }

    if( total_time.millitm >= (unsigned short)1000 )
    {
        total_time.time += 1;
        total_time.millitm -= (unsigned short)1000;
    }
}

#endif // CONN_PROFILING

// This is a global cached result from GetFineLinearTime.
// The monitor thread keeps it updated at a rate of 100 msec
// it's used in high traction code where there are repeated calls
// to lookup clock ticks, but where the granularity doesn't really
// need to be any less than 100 msec.  This saves lots and lots of
// OS calls.

#define CHUNKLINEARTIME_GRANULARITY  100

extern unsigned long g_ChunkLinearTime;

#endif // __SCSCOMMS_PRECOMPILED_HEADER__

