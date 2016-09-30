// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996 - 2003, 2005 Symantec Corporation. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/IPFILTER.H_v   1.0   25 Apr 1997 12:12:36   JBRENNA  $
//
// Description:
//      Associate cookies with IP Address Ranges
//
// Contains:
//      createIPFilter()
//      destroyIPFilter()
//      addIPRange()
//      getIPAddressCookie()
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/IPFILTER.H_v  $
// 
//    Rev 1.0   25 Apr 1997 12:12:36   JBRENNA
// Initial revision.
// 
//    Rev 1.2   20 Nov 1996 11:37:00   AOONWAL
// No change.
// 
//    Rev 1.1   13 Nov 1996 19:06:42   RAY
// Added addIPRanges().
// 
//    Rev 1.0   04 Nov 1996 18:26:00   RAY
// Initial revision.
// 
//    Rev 1.2   01 Nov 1996 12:37:08   DCHI
// Added IP_FILTER_INSIDE and IPFILTER_OUTSIDE defines.
// 
//    Rev 1.1   01 Nov 1996 11:37:36   CNACHEN
// Added cookie definitions.
// 
//    Rev 1.0   31 Oct 1996 18:14:04   RAY
// Initial revision.
//
//************************************************************************

#include "ossocket.h"

#if !defined( IPFILTER_H )
#define IPFILTER_H


BOOL createIPFilter
(
    LPHANDLE lphIPFilter,
    DWORD    dwDefaultCookie
);


BOOL destroyIPFilter
(
    HANDLE  hIPFilter
);


BOOL addIPRange
(
    BYTE    abyAddressLo[],
    BYTE    abyAddressHi[],
    DWORD   dwCookie,
    HANDLE  hIPFilter
);

BOOL addIPRanges
(
    LPSTR   lpstrIPRanges,
    DWORD   dwCookie,
    HANDLE  hIPFilter
);


DWORD getIPAddressCookie
(
    BYTE    abyAddress[],
    HANDLE  hIPFilter
);


typedef struct tagIPRANGE
{
    DWORD   dwAddressLo;
    DWORD   dwAddressHi;
    DWORD   dwCookie;
    struct tagIPRANGE FAR *lprNext;
} IPRANGE, FAR * LPIPRANGE;

#define sIPRANGE    (sizeof(IPRANGE))

typedef struct tagIPFILTER
{
    LPIPRANGE   lprIPRanges;
    DWORD       dwDefaultCookie;
} IPFILTER, FAR * LPIPFILTER;

#define sIPFILTER   (sizeof(IPFILTER))

// generic IPFilter Cookies

#define IP_FILTER_UNKNOWN               0
#define IP_FILTER_INCOMING              1
#define IP_FILTER_OUTGOING              2
#define IP_FILTER_OUTSIDE               IP_FILTER_INCOMING
#define IP_FILTER_INSIDE                IP_FILTER_OUTGOING

#endif                                  // !defined( IPFILTER_H )
