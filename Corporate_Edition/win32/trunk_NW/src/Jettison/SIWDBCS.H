// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C"
    {
#endif

#ifndef WIN32
#ifndef LPTSTR
#define LPTSTR char FAR *
#endif
#ifndef TCHAR
#define TCHAR char
#endif
#endif


LPTSTR dbcsAppendBackslash(LPTSTR path);
LPTSTR dbcsFindChar(LPTSTR path, TCHAR c);
LPTSTR dbcsFindFinalBackslash(LPTSTR path);
LPTSTR dbcsFindFinalDot(LPTSTR path);
LPTSTR dbcsJustFileName(LPTSTR dest, LPTSTR path);
LPTSTR dbcsJustExtension(LPTSTR dest, LPCTSTR path);
LPTSTR dbcsStripFileName(LPTSTR dest, LPTSTR path);
BOOL dbcsCharEqual(LPTSTR char1, LPTSTR char2);
LPTSTR dbcsLastChar(LPTSTR path);
LPTSTR dbcsEndOfString(LPTSTR path);
WORD dbcsStrLen(LPTSTR path);
LPTSTR dbcsFileCat(LPTSTR path, LPCTSTR file);
BOOL dbcsPathSpecified(LPTSTR path);
LPTSTR dbcsStrStr(LPTSTR str1, LPTSTR str2);
#ifdef __cplusplus
    }
#endif
