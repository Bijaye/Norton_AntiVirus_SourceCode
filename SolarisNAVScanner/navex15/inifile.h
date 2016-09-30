//************************************************************************
//
// $Header:   S:/NAVEX/VCS/inifile.h_v   1.1   03 Nov 1997 18:09:06   CNACHEN  $
//
// Description:
//      Contains INI file function prototypes
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/inifile.h_v  $
// 
//    Rev 1.1   03 Nov 1997 18:09:06   CNACHEN
// Prototypes proper strcpy/strcat functions and now uses LPCALLBACKREV1 instead
// of REV2.
// 
//    Rev 1.0   07 Aug 1997 18:23:28   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _INIFILE_H_

#define _INIFILE_H_

int GetProfileString
(
    LPCALLBACKREV1      lpstCallBacks,
    HFILE               hFile,
    LPSTR               lpszSection,
    LPSTR               lpszKey,
    LPSTR               lpszDefault,
    LPSTR               lpszOut,
    int                 nOutBufferSize
);

int GetProfileInt
(
    LPCALLBACKREV1      lpstCallBacks,
    HFILE               hFile,
    LPSTR               lpszSection,
    LPSTR               lpszKey,
    int                 nDefault,
    LPINT               lpnOut
);


void mystrcpy
(
    LPSTR           lpszDest,
    LPSTR           lpszSource
);

void mystrcat
(
    LPSTR           lpszDest,
    LPSTR           lpszSource
);


#endif
