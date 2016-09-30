//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/wbutil.h_v   1.1   05 Aug 1997 16:04:04   DCHI  $
//
// Description:
//      Header file for utility functions.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/wbutil.h_v  $
// 
//    Rev 1.1   05 Aug 1997 16:04:04   DCHI
// Added prototype for PascalStrHasNonNumChars().
// 
//    Rev 1.0   30 Jun 1997 16:17:36   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WBUTIL_H_

#define _WBUTIL_H_

int PascalStrCmp
(
    LPBYTE      lpbyStr0,
    LPBYTE      lpbyStr1
);

int PascalStrICmp
(
    LPBYTE      lpbyStr0,
    LPBYTE      lpbyStr1
);

BOOL PascalStrIEq
(
    LPBYTE      lpbyStr0,
    LPBYTE      lpbyStr1
);

BOOL PascalIsGlobalMacroName
(
    LPBYTE      lpbyName
);

BOOL PascalEqErrStr
(
    LPBYTE      lpbyStr
);

BOOL PascalStrHasNonNumChars
(
    LPBYTE      lpbyStr
);

long PascalStrToLong
(
    LPBYTE      lpbyStr
);

void LongToPascalStr
(
    long        lValue,
    LPBYTE      lpbyStr
);

void PascalStrToLower
(
    LPBYTE      lpbySrc,
    LPBYTE      lpbyDst
);

void PascalStrToUpper
(
    LPBYTE      lpbySrc,
    LPBYTE      lpbyDst
);

void PascalStrLeft
(
    LPBYTE      lpbySrc,
    long        lCount,
    LPBYTE      lpbyDst
);

void PascalStrRight
(
    LPBYTE      lpbySrc,
    long        lCount,
    LPBYTE      lpbyDst
);

void PascalStrMid
(
    LPBYTE      lpbySrc,
    long        lStart,
    long        lCount,
    LPBYTE      lpbyDst
);

void PascalStrLTrim
(
    LPBYTE      lpbySrc,
    LPBYTE      lpbyDst
);

void PascalStrRTrim
(
    LPBYTE      lpbySrc,
    LPBYTE      lpbyDst
);

long PascalStrInStr
(
    LPBYTE      lpbySrc,
    long        lIndex,
    LPBYTE      lpbySearch
);

#endif // #ifndef _WBUTIL_H_

