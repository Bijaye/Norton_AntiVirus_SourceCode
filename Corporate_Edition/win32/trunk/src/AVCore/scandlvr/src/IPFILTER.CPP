// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996, 2005 Symantec, Peter Norton Product Group. All rights reserved.
//************************************************************************
//
// $Header:   S:/SOCKLIB/VCS/IPFILTER.CPv   1.0   25 Apr 1997 11:51:42   ksackin  $
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
// $Log:   S:/SOCKLIB/VCS/IPFILTER.CPv  $
// 
//    Rev 1.0   25 Apr 1997 11:51:42   ksackin
// Initial revision.
// 
//    Rev 1.5   24 Jan 1997 19:46:04   RAY
// Must free the strdup()ed string!
// 
//    Rev 1.4   24 Jan 1997 19:37:08   RAY
// Fixed strtok on passed in string.  Now we strdup it first.
// 
//    Rev 1.3   18 Nov 1996 13:56:10   RAY
// Small bug fix for "-" case in addIPRanges
// 
//    Rev 1.2   14 Nov 1996 13:21:18   RAY
// Fixes to addIPRanges()
// 
//    Rev 1.1   13 Nov 1996 19:06:18   RAY
// Added addIPRanges().
// 
//    Rev 1.0   04 Nov 1996 18:26:18   RAY
// Initial revision.
// 
//    Rev 1.1   01 Nov 1996 14:13:30   RAY
// Added comments
// 
//    Rev 1.0   31 Oct 1996 18:14:02   RAY
// Initial revision.
//
//************************************************************************

//#include "windows.h"
#include "stdafx.h"

#include "ipfilter.h"

//*************************************************************************
//
// BOOL createIPFilter()
//
// Parameters:
//  lphIPFilter         Ptr to newly created IPFilter HANDLE
//  dwDefaultCookie     Value to associate with any unmatched ranges.
//
// Description:
//  This function creates a new IPFILTER structure and returns a HANDLE
//  to it.
//
// Returns:
//  TRUE                On success
//  FALSE               On error  (only possible error is out of memory)
//
//*************************************************************************
BOOL createIPFilter
(
    LPHANDLE lphIPFilter,
    DWORD    dwDefaultCookie
)
{
    if( NULL == ( *lphIPFilter = (HANDLE) malloc( sIPFILTER ))) return( FALSE );

    ((LPIPFILTER) *lphIPFilter)->lprIPRanges = NULL;
    ((LPIPFILTER) *lphIPFilter)->dwDefaultCookie = dwDefaultCookie;
    return TRUE;
}


//*************************************************************************
//
// BOOL destroyIPFilter()
//
// Parameters:
//  hIPFilter           HANDLE to IPFilter to destroy.
//
// Description:
//  This function destroys an IPFILTER structure as well as any ranges
//  which have been added to it since it's creation.
//
// Returns:
//  TRUE                On success
//  FALSE               On error  (none defined yet)
//
//*************************************************************************
BOOL destroyIPFilter
(
    HANDLE  hIPFilter
)
{
    LPIPFILTER lprIPFilter = (LPIPFILTER) hIPFilter;
    LPIPRANGE lprIPRange = lprIPFilter->lprIPRanges;

    while( NULL != lprIPRange )
        {
        LPIPRANGE lprIPRangeParent = lprIPRange;
        lprIPRange = lprIPRange->lprNext;
        free( lprIPRangeParent );
        }

    free( hIPFilter );
    return TRUE;
}


//*************************************************************************
//
// BOOL addIPRange()
//
// Parameters:
//  abyAddressLo[]      Low end of the address range to add.
//  abyAddressHi[]      High end of the address range to add.
//  dwCookie            Value to associate with this range of addresses.
//  hIPFilter           IPFilter to add this range to.
//
// Description:
//  Adds a range of addresses to an IPFilter.  Does no special handling of
//  overlapping or duplicate ranges.  Prepends new ranges to the IPFilter's
//  list of ranges.  Addresses should be passed in as an array of bytes,
//  MSB first (e.g. 155.64.153.47 = { 155, 64, 153, 47 }; )
//
// Returns:
//  TRUE                On success
//  FALSE               On error  (only possible error is out of memory)
//
//*************************************************************************
BOOL addIPRange
(
    BYTE    abyAddressLo[],
    BYTE    abyAddressHi[],
    DWORD   dwCookie,
    HANDLE  hIPFilter
)
{
    LPIPFILTER  lprIPFilter = (LPIPFILTER) hIPFilter;
    LPIPRANGE   lprIPRange = lprIPFilter->lprIPRanges;
    LPIPRANGE   lprIPRangeParent = NULL;

    DWORD       dwAddressLo = abyAddressLo[0] << 24 |
                              abyAddressLo[1] << 16 |
                              abyAddressLo[2] << 8 |
                              abyAddressLo[3];

    DWORD       dwAddressHi = abyAddressHi[0] << 24 |
                              abyAddressHi[1] << 16 |
                              abyAddressHi[2] << 8 |
                              abyAddressHi[3];

    lprIPFilter->lprIPRanges = (LPIPRANGE) malloc( sIPRANGE );

    if( NULL == lprIPFilter->lprIPRanges )
        {
        lprIPFilter->lprIPRanges = lprIPRange;
        return FALSE;
        }

    lprIPFilter->lprIPRanges->dwAddressLo = dwAddressLo;
    lprIPFilter->lprIPRanges->dwAddressHi = dwAddressHi;
    lprIPFilter->lprIPRanges->dwCookie = dwCookie;
    lprIPFilter->lprIPRanges->lprNext = lprIPRange;

    return TRUE;
}


//*************************************************************************
//
// BOOL addIPRanges()
//
// Parameters:
//  lpstrIPRanges       String containing ranges to add in the form:
//                      "-1.2.3.4, a.b.c.d, 5.5.5.5-6.6.6.6, 7.7.7.7-,"
//  dwCookie            Cookie to associate with these ranges.
//  hIPFilter           IPFilter to add these ranges to.
//
// Description:
//  Parses the lpstrIPRanges for IP hosts, addresses, and ranges and adds
//  each to hIPFilter.
//
// Returns:
//  BOOL
//
//*************************************************************************
BOOL addIPRanges
(
    LPSTR   lpstrIPRanges,
    DWORD   dwCookie,
    HANDLE  hIPFilter
)
{
    SOCKET_ADDRESS_T    rTempAddress;
    LPSTR               lpstrCopy;
    LPSTR               lpstrDummy;
    LPSTR               lpstrOffset;
    LPSTR               lpstrHyphen;
    DWORD               dwLoAddress;
    DWORD               dwHiAddress;
    DWORD               dwDummy;

    lpstrDummy = lpstrCopy = strdup( lpstrIPRanges );

    while( NULL != ( lpstrOffset = strtok( lpstrDummy, "," )))
        {
        lpstrDummy = NULL;
        lpstrHyphen = strchr( lpstrOffset, '-' );
        while( isspace( *lpstrOffset )) ++lpstrOffset;

        if( *lpstrOffset == '-' )
            {
                                        // handles the "-a.a.a.a" case
            dwLoAddress = 0;
            }
        else
            {
            if( NULL != lpstrHyphen ) *lpstrHyphen = 0;
            if( SOCKET_STATUS_OK != SocketAddressFill( &rTempAddress,
                                                       lpstrOffset,
                                                       0,
                                                       &dwDummy ))
                {
                free( lpstrCopy );
                return FALSE;
                }

            dwLoAddress = rTempAddress.stSockAddr.sin_addr.S_un.S_addr;
            if( NULL != lpstrHyphen ) *lpstrHyphen = '-';
            }


        if( NULL == lpstrHyphen )
            {
                                         // handles the "a.a.a.a" case
            dwHiAddress = dwLoAddress;
            }
        else if( 0 == lpstrHyphen[1] )
            {
                                        // handles the "a.a.a.a-" case
            dwHiAddress = 0xFFFFFFFF;
            }
        else
            {
            ++lpstrHyphen;
            while( isspace( *lpstrHyphen )) ++lpstrHyphen;

            if( SOCKET_STATUS_OK != SocketAddressFill( &rTempAddress,
                                                       lpstrHyphen,
                                                       0,
                                                       &dwDummy ))
                {
                free( lpstrCopy );
                return FALSE;
                }

            dwHiAddress = rTempAddress.stSockAddr.sin_addr.S_un.S_addr;
            }

        if( !addIPRange( (LPBYTE) &dwLoAddress,
                         (LPBYTE) &dwHiAddress,
                         dwCookie,
                         hIPFilter ))
            {
            free( lpstrCopy );
            return FALSE;
            }
        }

    free( lpstrCopy );
    return TRUE;
}


//*************************************************************************
//
// BOOL getIPAddressCookie()
//
// Parameters:
//  abyAddress[]        Address to lookup.
//  hIPFilter           IPFilter to lookup this address in.
//
// Description:
//  Looks for the first matching range and returns dwCookie.
//
// Returns:
//  DWORD               dwCookie associated with first matching range.
//
//*************************************************************************
DWORD getIPAddressCookie
(
    BYTE    abyAddress[],
    HANDLE  hIPFilter
)
{
    LPIPFILTER lprIPFilter = (LPIPFILTER) hIPFilter;
    LPIPRANGE lprIPRange = lprIPFilter->lprIPRanges;

    DWORD   dwAddress = abyAddress[0] << 24 |
                        abyAddress[1] << 16 |
                        abyAddress[2] << 8 |
                        abyAddress[3];

    while( NULL != lprIPRange )
        {
        if( dwAddress >= lprIPRange->dwAddressLo &&
            dwAddress <= lprIPRange->dwAddressHi )
            {
            return lprIPRange->dwCookie;
            }

        lprIPRange = lprIPRange->lprNext;
        }

    return lprIPFilter->dwDefaultCookie;
}


