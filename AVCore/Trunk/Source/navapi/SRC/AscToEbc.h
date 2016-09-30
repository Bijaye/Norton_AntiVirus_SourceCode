// Copyright 2000 Symantec, Core Technology Group
//************************************************************************
//
// Description:	AscToEbc.h
//
// Contains:    Prototypes for ASCII-EBCDIC conversion routines
//
// See Also:
//************************************************************************
//    Rev 1.0   27 July 2000 RGOUGH
// Initial revision.
//************************************************************************

#if !defined ( _ASCTOEBC_H )
#   define _ASCTOEBC_H

// Function prototypes...

int StrAscToEbc(LPCSTR lpzAscii, LPSTR lpszEbcdic);
int StrEbcToAsc(LPCSTR lpzEbcdic, LPSTR lpszAscii);

#endif                                  // defined _ASCTOEC_H
