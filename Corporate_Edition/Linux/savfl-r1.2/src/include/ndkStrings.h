// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK String Handling functions

#ifndef _ndkStrings_h_
#define _ndkStrings_h_

#include <locale.h>
#include <langinfo.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdio.h>

#include "ndkDefinitions.h"

#ifdef __cplusplus
extern "C" {
#endif


// ================== Internationalization ==============================================

// The use of the TCHAR, _T, and _TEOF macros is the Windows way of handling various
// characters sets. At compile, a decision has to be made which kind of characters to
// use, wide, multi- or single-byte. Wide characters on Linux are defined in ISO-10646
// (UCS). Multi-byte characters are encoded using UTF-8, that keeps the ASCII character
// set uniquely, and represents all other UCS characters with 1 to 6 bytes that all are
// larger than the largest ASCII byte, 0x7F. With this, the needed change for Unix
// functions to support multi-byte characters has been minimal.
//
//	All functions in this interface are thread safe, i.e their internal state is unique to
//	each function invocation and can thus not be corrupted by multiple simultaneous
//	invocations from multiple threads.

#if defined _UNICODE
	#define TextMapping(wideCharacter,multiByte,singleByte) wideCharacter
#elif defined _MBCS
	#define TextMapping(wideCharacter,multiByte,singleByte) multiByte
#else
	#define TextMapping(wideCharacter,multiByte,singleByte) singleByte
#endif

  // Copied from winStrings.h
#define _ttoi TextMapping(unitoi,atoi,atoi)
#define OutputDebugString(s) printf("%s",s)

#ifndef _TEXT
	#define _TEXT(s) TextMapping(L ## s,s,s)
#endif
#ifndef _T
	#define _T(s) _TEXT(s)
#endif
#ifndef __T
	#define __T(s) _TEXT(s)
#endif

#ifndef _TEOF
	#define _TEOF TextMapping(WEOF,EOF,EOF)
#endif

// --------------------- Language -------------------------------------------------------

int GetCurrentOSLanguageID();
int ReturnLanguageName(int languageID,BYTE* languageName);

// --------------------- Locale ---------------------------------------------------------

//			 LCONV* NWLlocaleconv(LCONV* pLconv) ->
// struct lconv*	  localeconv()

#define LCONV lconv
struct LCONV* NWLlocaleconv(struct LCONV* pLconv);

// pnstr NWLsetlocale(nint iCategory,const nstr* sLocale) ->
// char*		setlocale( int iCategory,const char* sLocale)

// Use the Linux command "locale -a" to see your locale repertoir. You have to use the
// "utf8" ones if you need to handle multi-byte characters.

#define NWLsetlocale setlocale

// --------------------- Characters -----------------------------------------------------

// size_t NWLmbslen(const nuint8*	tString) ->
// size_t	 wcslen(const wchar_t* wcString)
// size_t	_mbslen(const char*	  mbString)
// size_t	 strlen(const char*	  sbString)

#define NWLmbslen TextMapping(wcslen,_mbslen,strlen)
size_t _mbslen(const char* mbString);

// pnstr NWNextChar(const nstr*		tCurrent) ->
// wchar_t* _wcsinc(const wchar_t* wcCurrent)
// char*		_mbsinc(const char*	  mbCurrent)
// char*		_strinc(const char*	  sbCurrent)

#define NWNextChar TextMapping(_wcsinc,_mbsinc,_strinc)
wchar_t* _wcsinc(const wchar_t* wcCurrent);
char*		_mbsinc(const char*	  mbCurrent);
char*		_strinc(const char*	  sbCurrent);

// pnstr NWPrevChar(const nstr*		tStart,pnstr			  tCurrent) ->
// wchar_t* _wcsdec(const wchar_t* wcStart,const wchar_t* wcCurrent)
// char*		_mbsdec(const char*	  mbStart,const char*	 mbCurrent)
// char*		_strdec(const char*	  sbStart,const char*	 sbCurrent)

#define NWPrevChar TextMapping(_wcsdec,_mbsdec,_strdec)
wchar_t* _wcsdec(const wchar_t* wcStart,const wchar_t* wcCurrent);
char*		_mbsdec(const char*	  mbStart,const char*	 mbCurrent);
char*		_strdec(const char*	  sbStart,const char*	 sbCurrent);

#define _tcslen TextMapping(wcslen,strlen,strlen)

#define _tcsclen TextMapping(wcslen,NWLmbslen,strlen)

// --------------------- Search ---------------------------------------------------------

// Note that `NWLstrchr' and `NWLstrrchr' only allow searches for ASCII characters in a
// multi-byte string.

// pnstr NWLstrchr(const nstr*	  tString,nint		 tCharacter) ->
// wchar_t* wcschr(const wchar_t* wcString,wchar_t wcCharacter)
// char*		strchr(const char*	 mbString,int		sbCharacter)
// char*		strchr(const char*	 sbString,int		sbCharacter)

#define NWLstrchr TextMapping(wcschr,strchr,strchr)

// pnstr NWLstrrchr(const nstr*	   tString,nint	  tCharacter) ->
// wchar_t* wcsrchr(const wchar_t* wcString,wchar_t wcCharacter)
// char*		strrchr(const char*	  mbString,int		 sbCharacter)
// char*		strrchr(const char*	  sbString,int		 sbCharacter)

#define NWLstrrchr TextMapping(wcsrchr,strrchr,strrchr)

// pnstr NWLstrstr(const nstr*	  tString,const nstr*	  tSearch) ->
// wchar_t* wcsstr(const wchar_t* wcString,const wchar_t* wcSearch)
// char*	   strstr(const char*	 mbString,const char*	 mbSearch)
// char*		strstr(const char*	 sbString,const char*	 sbSearch)

#define NWLstrstr TextMapping(wcsstr,strstr,strstr)

#define _tcschr TextMapping(wcschr,NWLstrchr,strchr)

#define _tcsrchr TextMapping(wcsrchr,NWLstrrchr,strrchr)

#define _tcsstr TextMapping(wcsstr,NWLstrstr,strstr)

// These versions allow for search for wide character in a multibyte string.

char* mbschr( const char* mbString,wchar_t wcCharacter);
char* mbsrchr(const char* mbString,wchar_t wcCharacter);

// --------------------- Compare --------------------------------------------------------

#define _tcscmp TextMapping(wcscmp,_mbscmp,strcmp)
int _mbscmp(const char* s1,const char* s2);

#define _tcsicmp TextMapping(wcscasecmp,_mbsicmp,strcasecmp)
int _mbsicmp(const char* s1,const char* s2);

#define _tcsncmp TextMapping(wcsncmp,_mbsnbcmp,strncmp)
int _mbsnbcmp(const char* s1,const char* s2,size_t n);

#define _tcsnicmp TextMapping(wcsncasecmp,_mbsnbicmp,strncasecmp)
int _mbsnbicmp(const char* s1,const char* s2,size_t n);

// --------------------- Copy -----------------------------------------------------------

#define _tcscpy TextMapping(wcscpy,strcpy,strcpy)

#define _tcsncpy TextMapping(wcsncpy,strncpy,strncpy)

#define _tcscat TextMapping(wcscat,strcat,strcat)

// --------------------- Integer Conversion ---------------------------------------------

#define _itot TextMapping(itow,itoa,itoa) // see ndkIntegers, itow not yet implemented

// --------------------- Case Conversion ------------------------------------------------

// pnstr	 NWLstrupr(pnstr		tString) ->
// wchar_t* _wcsupr(wchar_t* wcString)
// char*		_mbsupr(char*	  mbString)
// char*		_sbsupr(char*	  sbString)

#define NWLstrupr TextMapping(_wcsupr,_mbsupr,_sbsupr)
wchar_t* _wcsupr(wchar_t* wcString);
char*		_mbsupr(char*	  mbString);
char*		_sbsupr(char*	  sbString);

// pnstr	 NWLstrlwr(pnstr		tString) ->
// wchar_t* _wcslwr(wchar_t* wcString)
// char*		_mbslwr(char*	  mbString)
// char*		_sbslwr(char*	  sbString)

#define NWLstrlwr TextMapping(_wcslwr,_mbslwr,_sbslwr)
wchar_t* _wcslwr(wchar_t* wcString);
char*		_mbslwr(char*	  mbString);
char*		_sbslwr(char*	  sbString);

#define _tcslwr NWLstrlwr

// --------------------- Character Set Conversion ---------------------------------------

char*    wcs2mbs(const wchar_t* wcString);
wchar_t* mbs2wcs(const char*    mbString);

// --------------------- Print ----------------------------------------------------------

// nint NWprintf(const nstr*		tFormat,...) ->
//	 int	wprintf(const wchar_t* wcFormat,...)
//	 int	 printf(const char*	  mbFormat,...)
//	 int	 printf(const char*	  sbFormat,...)

#define NWprintf TextMapping(wprintf,dprintf,dprintf)

// nint NWsprintf(pnstr	    tBuffer,const nstr*	    tFormat,...) ->
//	 int _swprintf(wchar_t* wcBuffer,const wchar_t* wcFormat,...)
//	 int	 sprintf(char*		mbBuffer,const char*		mbFormat,...)
//	 int	 sprintf(char*		sbBuffer,const char*		sbFormat,...)

#define NWsprintf TextMapping(_swprintf,sprintf,sprintf)
// TODO correct this the proper way. Warnings off had hidden that multiple #defines didn't work
// #define _swprintf(wcBuffer,wcFormat)                swprintf(wcBuffer,wcslen(wcBuffer),wcFormat)
// #define _swprintf(wcBuffer,wcFormat,a1)             swprintf(wcBuffer,wcslen(wcBuffer),wcFormat,a1)
// #define _swprintf(wcBuffer,wcFormat,a1,a2)          swprintf(wcBuffer,wcslen(wcBuffer),wcFormat,a1,a2)
// #define _swprintf(wcBuffer,wcFormat,a1,a2,a3)       swprintf(wcBuffer,wcslen(wcBuffer),wcFormat,a1,a2,a3)
// #define _swprintf(wcBuffer,wcFormat,a1,a2,a3,a4)    swprintf(wcBuffer,wcslen(wcBuffer),wcFormat,a1,a2,a3,a4)
#define _swprintf(wcBuffer,wcFormat,a1,a2,a3,a4,a5) swprintf(wcBuffer,wcslen(wcBuffer),wcFormat,a1,a2,a3,a4,a5)

// nint NWvsprintf(pnstr     tBuffer,const nstr*     tFormat,va_list arglist) ->
//	 int _vswprintf(wchar_t* wcBuffer,const wchar_t* wcFormat,va_list arglist)
//	 int	 vsprintf(char*	 mbBuffer,const char*	 mbFormat,va_list arglist)
//	 int	 vsprintf(char*	 sbBuffer,const char*	 sbFormat,va_list arglist)

#define NWvsprintf TextMapping(_vswprintf,vsprintf,vsprintf)
#define _vswprintf(wcBuffer,wcFormat,arglist) vswprintf(wcBuffer,wcslen(wcBuffer),wcFormat,arglist)

// ================== Single Byte Strings ===============================================

// --------------------- Comparison -----------------------------------------------------

// int strcmpi(	const char* sString1,const char* sString2) ->
// int strcasecmp(const char* sString1,const char* sString2)

#define strcmpi strcasecmp

// int stricmp(	const char* sString1,const char* sString2) ->
// int strcasecmp(const char* sString1,const char* sString2)

#define stricmp strcasecmp

// int strnicmp(	 const char* sString1,const char* sString2,size_t n) ->
// int strncasecmp(const char* sString1,const char* sString2,size_t n)

#define strnicmp strncasecmp

// --------------------- Case Conversion ------------------------------------------------

// char*	 strupr(char* sString) ->
// char* _sbsupr(char* sString)

#define  strupr _sbsupr
#define _strupr _sbsupr

// char*	 strlwr(char* sString) ->
// char* _sbslwr(char* sString)

#define  strlwr _sbslwr
#define _strlwr _sbslwr

#ifdef __cplusplus
}
#endif
	
#endif // _ndkStrings_h_
