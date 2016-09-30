// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SYMKRNL/VCS/PRM_STRD.C_v   1.0   26 Jan 1996 20:21:20   JREARDON  $
//
// Description:
//
// Contains:
//      ConvertStringToDouble()
//      _ConvertStringToDouble()
//      ConvertDoubleToString()
//      _ConvertDoubleToString()
//      ConvertDoubleToAbbreviation()
//      _ConvertDoubleToAbbreviation()
//      ConvertDoubleSizeToFullString()
//      ConvertDoubleSizeToShortString()
//
// See Also:
//************************************************************************
// $Log:   S:/SYMKRNL/VCS/PRM_STRD.C_v  $
// 
//    Rev 1.0   26 Jan 1996 20:21:20   JREARDON
// Initial revision.
// 
//    Rev 1.3   27 Apr 1995 16:22:06   Basil
// Fixed DOS platform warning
// 
//    Rev 1.2   27 Apr 1995 15:38:40   Basil
// Modified the format of abbreviation routines (see function header comments)
// 
//    Rev 1.1   23 Mar 1995 12:08:50   DALLEE
// Fixed OS2 compile error I introduced in heavy handed search and replace
// in my previous change.
// Okay, henceforth, I promise to update all platforms including OS2 for
// every line of source I touch. :-)
// 
//    Rev 1.0   22 Mar 1995 15:57:22   DALLEE
// Initial revision.
// Moved floating point routines here from PRM_STR.C so DOS linkers
// can prune out FP code.
//************************************************************************

/*@Library: SYMKRNL */

#include <dos.h>
#ifdef SYM_OS2
#   define INCL_DOSNLS   /* National Language Support values */
#endif
#include "platform.h"
#include "xapi.h"
#if defined(SYM_WIN)
#include "symintl.h"
#include "symkrnli.h"                  // For string IDs
#endif
#include "stddos.h"                    // DOS international stuff
#include "dbcs.h"

#if defined(SYM_NLM)
#include <ctype.h>
#include <stream.h>
#endif

#ifndef SYM_NLM
 #ifdef SYM_WIN
  LPSTR LOCAL PASCAL __WinConvertDoubleToAbbreviation (double ddNumber, LPBYTE string, int nType);

  extern HINSTANCE       hInst_SYMKRNL;

  extern BYTE    szPrmStrINTL[];
  extern BYTE    szPrmStrTHOUSANDLABEL[];
  extern BYTE    szPrmStrCOMMA[];
 #else
  extern BYTE    FAR MEGABYTE_ABBR[];
  extern BYTE    FAR KILOBYTE_ABBR[];
  extern BYTE    FAR GIGABYTE_ABBR[];
 #endif
#endif

extern BYTE    szPrmStrThousand[2];
extern BOOL    bPrmStrThousandRead;


#if !defined(SYM_NLM) && !defined(SYM_VXD)

/*@API:**********************************************************************
@Declaration: LPSTR  ConvertDoubleToString (double ddNumber, LPSTR lpString)

@Description:
This function converts a DOUBLE value into an ASCII string.  The string includes
thousands separator characters, which are based on the thousands separator
specified in WIN.INI (Windows) or the current country setting (DOS).

@Parameters:
$ddNumber$ The number to be converted.

$lpString$ Pointer to a buffer that is to receive the converted string.

@Returns:
The return value is a pointer to the converted string.

@See: ConvertStringToDouble ConvertStringToWord
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPSTR SYM_EXPORT WINAPI ConvertDoubleToString (double ddNumber, LPSTR lpString)
{
#ifdef SYM_OS2
    COUNTRYCODE     Country;
    COUNTRYINFO     CtryBuffer;
    ULONG           Length;

    if (!bPrmStrThousandRead)
        {
        Country.country = 0;                //  Read current
        Country.codepage = 0;               //  same
        DosQueryCtryInfo(sizeof(CtryBuffer), &Country, &CtryBuffer, &Length);
        szPrmStrThousand[0] = (BYTE) CtryBuffer.szThousandsSeparator;
        bPrmStrThousandRead = TRUE;
        }
#else   //  #ifdef SYM_OS2 else
    if (!bPrmStrThousandRead)
        {
#ifdef  SYM_WIN
                                        // get the WIN.INI setting
        GetProfileString (szPrmStrINTL, szPrmStrTHOUSANDLABEL, szPrmStrCOMMA,
                                szPrmStrThousand, sizeof(szPrmStrThousand));
#elif defined(SYM_DOS)
        szPrmStrThousand[0] = international.thousandsChar;
#else
        szPrmStrThousand[0] = ',';
#endif
        bPrmStrThousandRead = TRUE;
        }
#endif // #ifndef SYM_OS2
    return (_ConvertDoubleToString (ddNumber, lpString, 10, szPrmStrThousand[0]));
}

#endif  // if !defined(SYM_NLM) && !defined(SYM_VXD)


#if !defined(SYM_NLM) && !defined(SYM_VXD)

/*@API:**********************************************************************
@Declaration: LPSTR  _ConvertDoubleToString (double ddNumber, LPSTR lpString, UINT
wRadix, BYTE byThousandsChar)

@Description:
This function converts a DOUBLE value into an ASCII string, based on the
specified radix.

@Parameters:
$ddNumber$ The number to be converted.

$lpString$ Pointer to a buffer that is to receive the converted string.

$wRadix$ The radix to use when converting the string.

$byThousandsChar$ The separator to use for designating thousands.  This value
should be zero to eliminate the separator.

@Returns:
The return value is a pointer to the converted string.

@See: ConvertDoubleToString ConvertStringToDouble ConvertStringToWord
ConvertStringToLong
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/

LPSTR SYM_EXPORT WINAPI _ConvertDoubleToString (
    double ddNumber,
    LPSTR lpDest,
    UINT  wRadix,
    BYTE byThousandsChar)
{
    auto        LPSTR           lpString = lpDest;
    auto        double            x;
    auto        int             wCount;
    auto        LPSTR           lpTemp;
                                        /* Initialization               */
    wCount = 0;
    lpTemp = lpString;
                                        /* Convert the number from int  *
                                         *  to ascii                    */
    do  {
		double	__cdecl fmod(double, double);
                                        /* Get the remainder            */
        x = fmod(ddNumber,(double) wRadix);
                                        /* Add a comma?                 */
        if (byThousandsChar && (wCount++ == 3))
            {
                                        /* Yes                          */
            *lpString++ = byThousandsChar;
            wCount = 1;
            }

                                        /* Convert to letter            */
        *lpString++ = (char)((x < 10) ? x + '0' : 'A' + (x - 10));

                                        /* Whittle down the number  */
        ddNumber /= (double)(wRadix);

    } while (ddNumber >= 1);

                                        /* Null terminate the string    */
    *lpString = EOS;
                                        /* And reverse it               */
    STRREV (lpTemp);
                                        /* Return to caller             */
    return (lpTemp);

} /* _ConvertDoubleToString    */

#endif      // if !defined(SYM_NLM) && !defined(SYM_VXD)

#if !defined(SYM_NLM) && !defined(SYM_VXD)

/*@API:**********************************************************************
@Declaration: LPSTR   ConvertStringToDouble(double FAR * lpddValue, LPSTR lpBuffer)

@Description:
This function converts an ASCII string value into a double value.  Preceding
non-numeric characters are skipped.

@Parameters:
$lpddValue$ Pointer to the variable that is to receive the converted value.

$lpBuffer$ Pointer to the string to be converted.

@Returns:
The return value is a pointer to the first character after the last numeric
character.

@See: _ConvertDoubleToString _ConvertStringToDouble
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPSTR SYM_EXPORT WINAPI ConvertStringToDouble (double FAR * lpddValue, LPSTR lpBuffer)
{
    return _ConvertStringToDouble( lpddValue, lpBuffer, 10, FALSE );
}

#endif  // if !defined(SYM_NLM) && !defined(SYM_VXD)

#if !defined(SYM_NLM) && !defined(SYM_VXD)

/*@API:**********************************************************************
@Declaration: LPSTR _ConvertStringToDouble(double FAR * lpddValue, LPSTR lpBuffer,
UINT uiRadix, BYTE bIgnoreSeparators)

@Description:
This function converts an ASCII string value into a double value.  Preceding
non-numeric characters are skipped.

@Parameters:
$lpddValue$ Pointer to the variable that is to receive the converted value.

$lpBuffer$ Pointer to the string to be converted.

$uiRadix$ Specifies what base to use in the conversion.

$bIgnoreSeparators$ Specifies that separators, such as a comma (,), be ignored.

@Returns:
The return value is a pointer to the first character after the last numeric
character.

@See: _ConvertDoubleToString ConvertStringToDouble
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPSTR SYM_EXPORT WINAPI _ConvertStringToDouble( double FAR *lpddValue, LPSTR lpszString,
                                          UINT wRadix, BYTE bIgnoreSeparators )
{
#ifdef SYM_WIN
    char    szPrmStrThousandsString[INTL_MAXLEN_sTHOUSAND];
#endif
#ifdef SYM_OS2
    COUNTRYCODE     Country;
    COUNTRYINFO     CtryBuffer;
    ULONG           Length;
#endif
    double   ddValue = 0UL;
    BYTE    byChar, byLimit, byThousands;


    if (bIgnoreSeparators)
        {
#ifdef SYM_OS2
        Country.country = 0;                //  Read current
        Country.codepage = 0;               //  same
        DosQueryCtryInfo(sizeof(CtryBuffer), &Country, &CtryBuffer, &Length);
        szPrmStrThousand[0] = (BYTE) CtryBuffer.szThousandsSeparator;
        bPrmStrThousandRead = TRUE;
#else   //  #ifdef SYM_OS2 else

#ifdef SYM_WIN
        NIntlGetString( szPrmStrThousandsString, INTL_ENUM_THOUSAND );
        byThousands = (BYTE) szPrmStrThousandsString[0];
#elif defined(SYM_DOS)
        byThousands = (BYTE) international.decimalChar;
#else
        byThousands = '.';
#endif
#endif
        if (!byThousands)               // just in case...
            bIgnoreSeparators = FALSE;
        }

    byLimit = (BYTE) (wRadix <= 10? '0' + wRadix - 1: 'A' + wRadix - 11);

    for (;;)
        {
        byChar = (BYTE) CharToUpper( *lpszString );

        if (!byChar ||
            (byChar >= '0' && byChar <= byLimit && (byChar <= '9' || byChar >= 'A')))
            break;

        lpszString++;
        }

    while ((byChar = (BYTE) CharToUpper( *lpszString )) != '\0')
        {
        if (!bIgnoreSeparators || byChar != byThousands)
            {
            if (byChar < '0' || byChar > byLimit || (byChar > '9' && byChar < 'A'))
                break;

            ddValue = (ddValue * wRadix) +
                      (DWORD) (byChar > '9'? byChar - '7': byChar - '0');
            }

        lpszString++;
        }

    *lpddValue = ddValue;
    return lpszString;
}

#endif  // if !defined(SYM_NLM) && !defined(SYM_VXD)

#ifdef SYM_WIN

enum { CLTA_ABBR, CLTA_FULL, CLTA_SHORT };

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************/

LPSTR LOCAL PASCAL __WinConvertDoubleToAbbreviation (double ddNumber, LPBYTE string, int nType)
{
    char        szMB [25], szKB [25], szGB[25];
    int         nStrK, nStrM, nStrG;

    switch (nType)
        {
        case CLTA_ABBR:
            nStrK = ABRVSIZ_K;
            nStrM = ABRVSIZ_M;
            nStrG = ABRVSIZ_G;
            break;

        case CLTA_FULL:
            nStrK = KILOBYTE_STR;
            nStrM = MEGABYTE_STR;
            nStrG = GIGABYTE_STR;
            break;

        case CLTA_SHORT:
            nStrK = SHORT_KILOBYTE_STR;
            nStrM = SHORT_MEGABYTE_STR;
            nStrG = SHORT_GIGABYTE_STR;
            break;
        }

    LoadString( hInst_SYMKRNL, nStrK, szKB, sizeof(szKB));
    LoadString( hInst_SYMKRNL, nStrM, szMB, sizeof(szMB));
    LoadString( hInst_SYMKRNL, nStrG, szGB, sizeof(szGB));

    return _ConvertDoubleToAbbreviation (ddNumber, string, szKB, szMB, szGB);
}
#endif

#ifndef SYM_OS2

#if !defined(SYM_VXD)
/*****************************************************************************
 * Do not document as part of Quake API, ConvertDoubleToAbbreviation and
 * ConvertDoubleSizeToFullString are the API "covers" to this function.
 ****************************************************************************/
LPSTR SYM_EXPORT WINAPI _ConvertDoubleToAbbreviation(double ddNumber, LPBYTE string,
                                               LPCSTR kiloStr,  LPCSTR megaStr, LPCSTR gigaStr)
{
#ifndef ONEG
#define ONEK ((double)(1024))
#define ONEM ((double)(ONEK*1024))
#define ONEG ((double)(ONEM*1024))
#endif

#ifdef SYM_NLM
    (void) ddNumber;
    (void) string;
    (void) kiloStr;
    (void) megaStr;
    (void) gigaStr;
#else 	// SYM_NLM
#ifdef SYM_WIN
    char szDecimalString[INTL_MAXLEN_sDECIMAL];
#endif	// SYM_WIN

    UINT gig;


    if (ddNumber < ONEG)
        {
		DWORD dwTemp = (DWORD)ddNumber;
		return (_ConvertLongToAbbreviation(dwTemp, string, kiloStr, megaStr));
        }
    else
        {
        gig = (UINT)( ((ddNumber * 100L) + (ONEG/2)) / ONEG);
#ifdef SYM_WIN
        NIntlGetString( szDecimalString, INTL_ENUM_DECIMAL );
        wsprintf(string, "%d%s%02d%s", gig / 100,
                 (LPSTR) szDecimalString, gig % 100, gigaStr);
#elif defined(SYM_DOS)
        StringPrint(string, "%d1%c2%02d3%s4", gig / 100,
                    international.decimalChar, gig % 100, gigaStr);
#endif // SYM_WIN
        }
#endif // else SYM_NLM
    return (string);
}
#endif // #if !defined(SYM_VXD)

#if !defined(SYM_VXD)
/*@API:*******************************************************************
@Declaration: LPSTR ConvertDoubleToAbbreviation(double ddNumber, LPSTR string)

@Description:
This function converts double numbers assumed to be values in bytes, as in
100,000,000 bytes, to an abbreviated string with the largest size specifier,
as in "3.5G".

Converted number precision is as follows:
0                   0K
1 -> 9.99K          x.yyK            <two decimal places> 
10K -> 99.9K        xx.yK            <one decimal place> 
100K-> 1023K        xxxxK            <no fraction>

1M -> 9.99M         x.yyM            <two decimal places> 
10M -> 99.9M        xx.yM            <one decimal place> 
100M-> 1023.99M     xxxxM            <no fraction>

1G -> 9.99G         x.yyG            <two decimal places> 
10G -> 99.9G        xx.yG            <one decimal place> 
100G-> ...          xxxxG            <no fraction>

@Parameters:
$ddNumber$ Specifies the number to convert.

$string$ Points to a string buffer large enough to hold the result.

@Returns:
The return value is a pointer to the string.

@See: ConvertDoubleSizeToFullString ConvertDoubleSizeToShortString
@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPSTR SYM_EXPORT WINAPI ConvertDoubleToAbbreviation(double ddNumber, LPSTR string)
{
#ifdef SYM_NLM
    (void) ddNumber;
    (void) string;

    return string;
#else
#ifdef SYM_WIN
    return __WinConvertDoubleToAbbreviation (ddNumber, string, CLTA_ABBR);
#else
    return (_ConvertDoubleToAbbreviation(ddNumber, string,
                                       KILOBYTE_ABBR, MEGABYTE_ABBR, GIGABYTE_ABBR));
#endif
#endif
}
#endif // #if !defined(SYM_VXD)

#if !defined(SYM_VXD)
/*@API:******************************************************************
@Declaration: LPSTR ConvertDoubleSizeToFullString(double ddNumber, LPSTR string)

@Description:
This function converts double numbers assumed to be values in bytes, as in
100,000,000 bytes, to a string with the largest size specifier, as in
"1.5 Gigabytes".

Converted number precision is as follows:
0                   0K
1 -> 9.99K          x.yyK            <two decimal places> 
10K -> 99.9K        xx.yK            <one decimal place> 
100K-> 1023K        xxxxK            <no fraction>

1M -> 9.99M         x.yyM            <two decimal places> 
10M -> 99.9M        xx.yM            <one decimal place> 
100M-> 1023.99M     xxxxM            <no fraction>

1G -> 9.99G         x.yyG            <two decimal places> 
10G -> 99.9G        xx.yG            <one decimal place> 
100G-> ...          xxxxG            <no fraction>

@Parameters:
$ddNumber$ Specifies the number to convert.

$string$ Points to a string buffer large enough to hold the result.

@Returns:
The return value is a pointer to the string.

@See: ConvertSizeToAbbreviation ConvertSizeToShortString
@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPSTR SYM_EXPORT WINAPI ConvertDoubleSizeToFullString(double ddNumber, LPSTR string)
{
#ifdef SYM_NLM
    (void) ddNumber;
    (void) string;

    return string;
#else
#ifdef SYM_WIN
    return __WinConvertDoubleToAbbreviation (ddNumber, string, CLTA_FULL);
#else
    extern      char    FAR KILOBYTE_STR[];
    extern      char    FAR MEGABYTE_STR[];
    extern      char    FAR GIGABYTE_STR[];

    return (_ConvertDoubleToAbbreviation(ddNumber, string,
                                          KILOBYTE_STR, MEGABYTE_STR, GIGABYTE_STR));
#endif
#endif
}
#endif // #if !defined(SYM_VXD)

#if !defined(SYM_VXD)
/*@API:******************************************************************
@Declaration: LPSTR ConvertDoubleSizeToShortString(double ddNumber, LPSTR string)

@Description:
This function converts double numbers assumed to be values in bytes, as in
100,000,000 bytes, to an abbreviated string with the largest size specifier,
as in "2.3 GB".

Converted number precision is as follows:
0                   0K
1 -> 9.99K          x.yyK            <two decimal places> 
10K -> 99.9K        xx.yK            <one decimal place> 
100K-> 1023K        xxxxK            <no fraction>

1M -> 9.99M         x.yyM            <two decimal places> 
10M -> 99.9M        xx.yM            <one decimal place> 
100M-> 1023.99M     xxxxM            <no fraction>

1G -> 9.99G         x.yyG            <two decimal places> 
10G -> 99.9G        xx.yG            <one decimal place> 
100G-> ...          xxxxG            <no fraction>

@Parameters:
$ddNumber$ Specifies the number to convert.

$string$ Points to a string buffer large enough to hold the result.

@Returns:
The return value is a pointer to the string.

@See: ConvertSizeToAbbreviation ConvertSizeToFullString
@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPSTR SYM_EXPORT WINAPI ConvertDoubleSizeToShortString(double ddNumber, LPSTR string)
{
#ifdef SYM_NLM
    (void) ddNumber;
    (void) string;

    return string;

#else
#ifdef SYM_WIN
    return __WinConvertDoubleToAbbreviation (ddNumber, string, CLTA_SHORT);
#else
    extern      char    FAR SHORT_KILOBYTE_STR[];
    extern      char    FAR SHORT_MEGABYTE_STR[];
    extern      char    FAR SHORT_GIGABYTE_STR[];

    return (_ConvertDoubleToAbbreviation(ddNumber, string,
                                SHORT_KILOBYTE_STR, SHORT_MEGABYTE_STR, SHORT_GIGABYTE_STR));
#endif
#endif
}
#endif // #if !defined(SYM_VXD)
#endif // #ifndef SYM_OS2
