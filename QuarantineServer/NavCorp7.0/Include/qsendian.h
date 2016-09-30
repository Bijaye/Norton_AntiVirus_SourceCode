// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/AVENDIAN.H_v   1.3   18 Apr 1997 13:44:18   MKEATIN  $
//
// Description:
//      Endianization support macros.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/AVENDIAN.H_v  $
//************************************************************************

#ifndef _qsendian_h

    #define _qsendian_h


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

//
#endif  // _qsendian_h

