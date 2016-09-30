/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/


#ifndef _LOCALIZE_H_DEFINED
   #define  _LOCALIZE_H_DEFINED

// MBCS SUPPORT
// #if defined(SYM_WIN32)
//     #if !defined(UNICODE)
//         #ifndef _MBCS
//             #define _MBCS
//         #endif
//     #endif
// #endif


#ifdef _UNICODE
    #if !defined(UNICODE)
        #define UNICODE
    #endif
#endif

#ifndef SYM_WIN32
   #define SYM_WIN32
#endif
#ifndef SYM_WIN
   #define SYM_WIN
#endif
#if defined(SYM_WIN32) && !defined(WIN32)
    #define WIN32            1
#endif
#ifndef       STRICT
    #define   STRICT
#endif



#if defined(SYM_MFC)
   #undef STRICT                     // MFC will define STRICT
   #include <afxwin.h>               // MFC libraries use different Win.h
#else
   #include <windows.h>              // Windows (all variants) 
   #include <windowsx.h>
#endif


#define SYM_EXPORT           __declspec(dllexport)
#define SYM_MAX_PATH         260        /* max. length of full pathname     */
#ifndef EOS
#   define EOS               _T('\0')   /* End Of String             */
#endif
#ifndef NOERR
#   define NOERR             0          /* No error.                 */
#endif
#ifndef PATH_CHAR
#   define PATH_CHAR         _T('\\')
#endif
#ifndef INVALID_HANDLE_VALUE
#   define INVALID_HANDLE_VALUE (HANDLE)-1
#endif
#ifndef BYTE
    typedef unsigned char BYTE;
#endif

// IN UTIL.C
//HGLOBAL  MemAlloc(DWORD dwSize);
//HGLOBAL  MemFree(HGLOBAL hHandle);



#include  <tchar.h>    // 7/14/99

#if defined(UNICODE) || defined(_UNICODE)  
    #include  <wchar.h>   
    #define STRCSPN(x,y)        wcscspn(x,y)
    #define STRDUP(s)           _wcsdup(s)
    #define STRNCAT(d,s,n)      wcsncat(d,s,n)
    #define STRCMP(d,s)         wcscmp(d,s)
    #define STRNCMP(d,s,n)      wcsncmp(d,s,n)
    #define STRNSET(s,c,n)      _wcsnset(s,c,n)
    #define STRSET(s,c)         _wcsset(s,c)
    #define STRSPN(x,y)         wcsspn(x,y)
    #define STRTOK(x,y)         wcstok(x,y)
    #define STRCPY(d,s)         wcscpy((LPTSTR)(d),(LPCTSTR)(s))
    #define STRNCPY(d,s,n)      wcsncpy((LPTSTR)(d),(LPCTSTR)(s),(wint_t)(n))
    #define ISSPACE(ch)         iswspace((wint_t)ch)
    #define STRCHR(s,ch)        wcschr((LPCTSTR)s,(wint_t)ch) 
    #define STRPBRK(d,s)        wcspbrk((LPTSTR)(d),(LPTSTR)(s))
    #define STRRCHR(s,ch) 	    wcsrchr((LPCTSTR)s,(wint_t)ch) 
    #define STRSTR(d,s) 	    wcsstr((LPCTSTR)(d),(LPCTSTR)(s))
    #define STRLEN(s)           wcslen(s)

    #define STRTOUL(n,e,b)      wcstoul(n,e,b)
    #define TOUPPER(ch)         towupper((wint_t)ch)
    #define TOLOWER(ch)         towlower((wint_t)ch)

#else
    //#include  <tchar.h>   
    // // MUST BE MBCS or TCHAR
    
    #define STRDUP(d) 	        _tcsdup((LPCTSTR)(d)) 
    #define STRNCAT(d,s,n)      _tcsncat((LPTSTR)(d),(LPCTSTR)(s),(int)(n))
    #define STRNICMP(d,s,n)     _tcsncicmp((LPCTSTR)(d),(LPCTSTR)(s),(UINT)(n))
    #define STRNCMP(d,s,n)      _tcsncmp((LPCTSTR)(d),(LPCTSTR)(s),(UINT)(n))
    #define STRNSET(s,c,n)      _tcsncset(s,c,n)
    #define STRSET(s,c)         _tcsset(s,c)
    #define STRSPN(x,y)         _tcsspn(x,y)
    #define STRCMP(x,y)         _tcscmp(x,y)
    #define STRTOK(d,s) 	    _tcstok((LPTSTR)(d),(LPTSTR)(s)) 
    #define ISSPACE(ch)         _istspace(ch)
//  #define ISDIGIT(ch)         _istxdigit(ch)
    #define STRCPY(d,s)         _tcscpy((LPTSTR)(d),(LPCTSTR)(s))
    #define STRNCPY(d,s,n)      _tcsncpy((LPTSTR)(d),(LPCTSTR)(s),(int)(n))
    #define STRCHR(s,ch)        _tcschr((LPCTSTR)s,(int)ch) 
    #define STRPBRK(d,s)        _tcspbrk((LPTSTR)(d),(LPTSTR)(s))
    #define STRRCHR(s,ch) 	    _tcsrchr((LPCTSTR)s,(int)ch) 
    #define STRSTR(d,s) 	    _tcsstr((LPCTSTR)(d),(LPCTSTR)(s))
    #define STRLEN(s)           _tcslen(s)

    #define STRTOUL(n,e,b)      _tcstoul(n,e,b)
    #define TOUPPER(ch)         _totupper((unsigned short)ch)
    #define TOLOWER(ch)         _totlower((unsigned short)ch)

#endif                              // if defined(UNICODE)

#define MEMSET(d,c,n)           memset(d,c,n)
#define MEMMOVE(d,s,n)          memmove(d,s,n)
#define MEMCPY(d,s,n)           memcpy(d,s,n)
#define MEMCHR(d,c,n)           memchr(d,c,n)
#define MEMCMP(x,y,n)           memcmp(x,y,n)
#define MEMICMP(x,y,n)          memicmp(x,y,n)

















#endif  // _LOCALIZE_H_DEFINED

