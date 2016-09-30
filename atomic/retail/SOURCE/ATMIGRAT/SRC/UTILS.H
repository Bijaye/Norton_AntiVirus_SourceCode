/////////////////////////////////////////////////////////////////////
// File: Utils.h
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/ATMIGRAT/VCS/utils.h_v   1.0   26 May 1998 14:36:02   SEDWARD  $
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGRAT/VCS/utils.h_v  $
// 
//    Rev 1.0   26 May 1998 14:36:02   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////




#ifndef MIGRATION_UTILS_HEADER
#define MIGRATION_UTILS_HEADER

// some debug defines
// #define _LOGFILE_TRACE_     // enable this to generate a logfile


INT         _mbsbytes(IN LPCTSTR  str);
VOID CDECL  LTrace(LPCSTR blpszFmt, ...);
BOOL WINAPI FileExists( LPTSTR lpFile );
BOOL        UnicodeToAnsi(LPCOLESTR pszW, LPSTR ppszA,UINT cbSize);

#endif  // MIGRATION_UTILS_HEADER