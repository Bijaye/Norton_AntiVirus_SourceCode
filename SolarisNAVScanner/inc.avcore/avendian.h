// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/AVENDIAN.H_v   1.3   18 Apr 1997 13:44:18   MKEATIN  $
//
// Description:
//  AVAPI 2.0 endianization support macros.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/AVENDIAN.H_v  $
// 
//    Rev 1.3   18 Apr 1997 13:44:18   MKEATIN
// Latest header from NEWVIR/INCLUDE
// 
//    Rev 1.5   20 Nov 1996 15:23:40   AOONWAL
// No change.
// 
//    Rev 1.4   22 Oct 1996 12:05:46   AOONWAL
// No change.
// 
//    Rev 1.3   13 Aug 1996 11:42:48   DCHI
// Put parens around parameter usage in AVDEREF_WORD.
// 
//    Rev 1.2   12 Aug 1996 17:43:14   DCHI
// Modifications for compilation on UNIX.
// 
//    Rev 1.1   09 Aug 1996 18:26:58   CNACHEN
// Fixed WENDIAN and DWENDIAN...
// 
//    Rev 1.0   04 Jun 1996 12:57:08   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _AVENDIAN_H

    #define _AVENDIAN_H


	#ifdef  BIG_ENDIAN

        #define WENDIAN(w)   ( (WORD)((WORD)(w) >> 8) | (WORD)((WORD)(w) << 8) )
        #define DWENDIAN(dw) ( (DWORD)((DWORD)(dw) >> 24) | \
                               (DWORD)((DWORD)dw << 24)  | \
                               (DWORD)(((DWORD)(dw) >> 8) & 0xFF00U) | \
                               (DWORD)(((DWORD)(dw) << 8) & 0xFF0000UL) )

        #define AVDEREF_WORD(lpvBuf) ((WORD) \
            (((LPBYTE)(lpvBuf))[0] | (((LPBYTE)(lpvBuf))[1] << 8)))

    #else   // if LITTLE_ENDIAN (default)

        #define WENDIAN(w)   (w)
        #define DWENDIAN(dw) (dw)

        #define AVDEREF_WORD(lpvBuf) WENDIAN(*(LPWORD)(lpvBuf))

    #endif


    #define SignExtendByte(byData) ((WORD)(signed short)(signed char)(byData))

#endif  // _AVENDIAN_H

