/* Copyright 1992-1995 Symantec Corporation                             */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/prm_str.c_v   1.4   20 Oct 1997 18:09:46   MBROWN  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *      StringCompress                                                  *
 *      StringUnCompress                                                *
 *      lstrncmp                                                        *
 *      lstrncmpi                                                       *
 *      DOSIntlStringCompare                                            *
 *      StringAppendChar                                                *
 *      StringAppendNChars                                              *
 *      StringMaxLength                                                 *
 *      StringSkipWhite                                                 *
 *      StringSkipChar                                                  *
 *      StringSkipChars                                                 *
 *      StringSkipToWhite                                               *
 *      StringSkipIniDelim                                              *
 *      StringSkipToIniDelim                                            *
 *      STRCPYCHR                                                       *
 *      StringRemoveTrailingSpaces                                      *
 *      StringGetEnd                                                    *
 *      StringMaxCopy                                                   *
 *      ConvertStringToWord                                             *
 *      ConvertStringToLong                                             *
 *      _ConvertStringToLong                                            *
 *      ConvertLongToString                                             *
 *      ConvertWordToString                                             *
 *      _ConvertLongToString                                            *
 *      _ConvertWordToString                                            *
 *      CharIsAlphaNumeric                                              *
 *      CharIsHexNumeric                                                *
 *      CharIsNumeric                                                   *
 *      CharIsWhite                                                     *
 *      CharIsAlpha                                                     *
 *      CharIsLower                                                     *
 *      CharIsUpper                                                     *
 *      ConvertLongToAbbreviation                                       *
 *      _ConvertLongToAbbreviation                                      *
 *      ConvertSizeToFullString                                         *
 *      ConvertSizeToShortString                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/prm_str.c_v  $ *
// 
//    Rev 1.4   20 Oct 1997 18:09:46   MBROWN
// New conversion fn's that take KB to overcome 4GB DWORD limit.
// 
//    Rev 1.3   08 Feb 1996 14:23:02   RSTANEV
// Due to bad prototypes of CharIsAlpha(), CharIsLower() and CharIsUpper(),
// some of the parameters had to be explicitly typecasted to TCHAR.
//
//    Rev 1.2   07 Feb 1996 08:37:12   RSTANEV
// SYM_VXD now uses _strnicmp() instead of strnicmp()
//
//    Rev 1.1   30 Jan 1996 11:21:14   RSTANEV
// Unicode enabled _ConvertLongToString() and _ConvertWordToString().
//
//    Rev 1.0   26 Jan 1996 20:23:14   JREARDON
// Initial revision.
//
//    Rev 1.73   18 Jan 1996 11:29:48   RSTANEV
// Added Unicode support.
//
//    Rev 1.72   30 Apr 1995 00:47:32   MBARNES
// Fixed StringStripSpaces() and StringRemoveTrailingSpaces() so that trailing
// spaces are actually stripped from strings!
//
//    Rev 1.71   27 Apr 1995 15:31:38   Basil
// Modified the format of abbreviation routines (see function header comments)
//
//    Rev 1.70   17 Apr 1995 16:18:32   MARKK
// Fixed a problem with 1.44 meg floppies
//
//    Rev 1.69   23 Mar 1995 12:03:18   DALLEE
// Fixed OS2 compile error I introduced in heavy handed search and replace
// in my previous change.
// Okay, henceforth, I promise to update all platforms including OS2 for
// every line of source I touch. :-)
//
//    Rev 1.68   22 Mar 1995 15:54:12   DALLEE
// Moved floating point routines to new file PRM_STRD.C so DOS linkers can
// prune out FP emulation when these aren't called.
//
//    Rev 1.67   20 Mar 1995 14:25:20   RSTANEV
// One more change so that the doubles are not referenced.
//
//    Rev 1.66   20 Mar 1995 10:51:48   RSTANEV
// VxD platform does not need functions for double-types.
//
//    Rev 1.65   17 Mar 1995 17:13:54   Basil
// Added conversion routines that handle double (8 byte) numbers.
//
//    Rev 1.64   12 Jan 1995 15:38:12   MARKL
// VxD platform CharIsAlpha():  Also, since there is no real lower case
//     support, the character is converted to an uppercase before checking
//     to see if it is between 'A' and 'Z' inclusively.
//
//    Rev 1.63   12 Jan 1995 15:04:10   MARKL
// For VxD platform:  CharIsAlpha() didn't work since VxD's only know about
//    lower case.  Modified code to simply check to see if character passed
//    falls within the range of 'A' through 'Z' for this platform only.
//
//    Rev 1.62   10 Jan 1995 14:23:32   RSTANEV
// Enabled CharIsAlpha() and CharIsLower() for VxD platform.
//
//    Rev 1.61   03 Jan 1995 16:53:14   Basil
// fixed _ConvertLongToAbbreviation() to convert KB -> MB correctly, it used
// to assume 1 Meg == 1000K, which is incorrect (it should be 1024K).
//
//    Rev 1.60   15 Dec 1994 17:32:18   BRIANF
// Removed the check for the Swiss collating table.
// The table has been fixed in version 6.22 of MSDOS.
// DOS versions prior to 6.22 will have to live with
// an incorrect collating table.
//
//    Rev 1.59   09 Dec 1994 19:42:28   RSTANEV
// Excluded several functions from compiling for VxD platform.
//
//    Rev 1.58   09 Dec 1994 19:06:06   BRAD
// Cleaned up for VXD
//
//    Rev 1.57   09 Dec 1994 17:06:04   BRAD
// Added SYM_VXD support
//
//    Rev 1.56   08 Dec 1994 14:27:42   BRAD
// Added StringAppendNChars
//
//    Rev 1.55   29 Nov 1994 12:34:00   BRAD
// Added a few new string functions.  Needed to make DBCS life easier
//
//    Rev 1.54   16 Nov 1994 13:27:34   BRUCE
// Made StringGetLastChar() take an LPCSTR
//
//    Rev 1.53   16 Nov 1994 07:50:00   JMILLARD
//
// fix temporarily to build for NLM platform - will need to revisit to
// set up a permanent solution
//
//    Rev 1.52   11 Nov 1994 12:17:08   BRAD
// Further DBCS enabling
//
//    Rev 1.51   02 Nov 1994 16:43:10   BRAD
// Added DBCS support
//
//    Rev 1.50   14 Sep 1994 10:58:26   MARKK
// Fixed bug in converting 0 to short abreviation
//
//    Rev 1.49   07 Jun 1994 00:00:28   GVOGEL
// Changed '@Library:' to '@Include:' for comment blocks.
//
//    Rev 1.48   20 May 1994 02:04:08   GVOGEL
// Added more comment blocks for Geologist project.
//
//    Rev 1.47   19 May 1994 13:52:06   DAVID
// Fixed order of null pointer tests in StringMaxLength() for protected mode.
//
//    Rev 1.46   19 May 1994 11:51:06   DBUCHES
// Fixed definition for _ConvertWordToString().  Had PSTR, needed LPSTR.
//
//    Rev 1.45   19 May 1994 03:53:42   GVOGEL
// Added comment blocks for Geologist project.
//
//    Rev 1.44   05 May 1994 13:07:04   BRAD
// Don't include SYMINTL for DOS
//
//    Rev 1.43   05 May 1994 12:14:20   BRAD
// DX support
//
//    Rev 1.42   15 Mar 1994 12:34:34   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.41   25 Feb 1994 15:02:52   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.39   09 Feb 1994 17:59:16   PGRAVES
// Win32.
//
//    Rev 1.38   27 Jan 1994 19:13:44   TONY
// OS/2 conversion of routines needed for Anvil's NAINV
//
//    Rev 1.37   17 Nov 1993 18:01:32   BARRY
// Merge changes from QAK2 branch
//
//    Rev 1.36.1.1   17 Nov 1993 17:58:54   BARRY
// Add StringSkip[To]IniDelim() to fix install bugs
//
//    Rev 1.36.1.0   19 Oct 1993 09:29:40   SCOTTP
// Branch base for version QAK2
//
//    Rev 1.36   30 Sep 1993 06:12:02   DWHITE
// Added DBCS support by including DBCS.H
//
//    Rev 1.35   22 Jul 1993 18:33:24   JOHN
// Added DOSIntlStringCompare().
//
//    Rev 1.34   02 Jul 1993 08:51:50   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//    Rev 1.33   16 Jun 1993 14:29:32   BRUCE
// [FIX] Fixed StringAppendChar to return a value, also detabed
//
//    Rev 1.32   15 Jun 1993 13:34:56   BRAD
// Added StringStripSpaces() and StringAppendChar().
//
//    Rev 1.31   10 Jun 1993 19:18:38   BRAD
// Changes so some code works in DOS, not just Windows.
//
//    Rev 1.30   10 Jun 1993 19:01:14   BRAD
//
//    Rev 1.29   10 Jun 1993 10:07:52   ED
// Uses SYMKRNLI.H now instead of SYMPRIMI.H
//
//    Rev 1.28   10 Jun 1993 09:49:34   ED
// [Fix] Minor modification of the last fix.  If the number is less than
// 1K, we round it up to 1K instead of using the actual value in bytes.
//
//    Rev 1.27   10 Jun 1993 08:12:58   ED
// [Fix] Fixed up _ConvertLongToAbbreviation and the Windows versions of
// functions associated with it.  Added __WinConvertLongToAbbreviation
// to handle LoadStrings.
//
// [Fix] _ConvertLongToAbbreviation should have used the actual size in
// bytes if it was less than 1K, but instead it used the size / 1K, which
// was always zero.
//
//    Rev 1.26   03 May 1993 22:45:50   BRAD
// Friggen bug in 'C' DWORD multiplication routine, causes a
// problem in _ConvertStringToLong().  Doesn't preserve return
// value from multiplication.  Moved things around a little, so
// simplified code that compiler needs to generate.
//
//    Rev 1.25   26 Apr 1993 09:55:42   ED
// [Enh] Finally added StringRemoveCRLF, after writing it for the 100th
// time in my own code.
//
//    Rev 1.24   23 Mar 1993 21:26:16   BRAD
// Compiler is/was generating wrong code.  It was destroying
// BX after call to __aFlmul, but BX was index used for local
// variable.  May be C 8.0 problem.
//
//    Rev 1.23   17 Mar 1993 16:18:36   TORSTEN
// Fixed a redeclaration of an extern for the DOS library.
//
//    Rev 1.22   21 Feb 1993 18:31:10   BRAD
// No change.
//
//    Rev 1.21   21 Feb 1993 18:20:18   BRAD
// Screw it.  Don't use StringMaxCopy() as a replacment.
// Just use fstrncpy().
//
//    Rev 1.20   21 Feb 1993 18:01:00   BRAD
// Cleaned up some STRNCPY() routines.
//
//    Rev 1.19   21 Feb 1993 16:37:40   BRAD
// Changed StringMaxCopy() to return same value as strncpy.
//
//    Rev 1.18   12 Feb 1993 04:29:18   ENRIQUE
// Modify the instance handle that was being used.
//
//    Rev 1.17   04 Feb 1993 17:10:30   MARKL
// GerProfileString sizes did not take the '\0' in account.
//
//    Rev 1.16   04 Feb 1993 16:43:30   MARKL
// "[Intl]" should be "intl".  One need not put []s around INI sections.
//
//    Rev 1.15   11 Jan 1993 08:00:56   ED
// Added StringSeparate
//
//    Rev 1.14   08 Jan 1993 13:34:18   HENRI
// Fixed LoadString bug in ConvertLongToAbbreviation.
//
//    Rev 1.13   17 Dec 1992 11:38:44   CRAIG
// Fixed a typo.
//
//    Rev 1.12   17 Dec 1992 11:33:00   CRAIG
// Added _ConvertStringToLong() and rewrote ConvertStringToLong() and
// ConvertStringToWord() to use it.
//
//    Rev 1.11   08 Dec 1992 09:13:56   CRAIG
// Added ConvertSizeToFullString() and ConvertSizeToShortString().
//
//    Rev 1.10   07 Dec 1992 15:50:58   CRAIG
// Forgot to cast string args to wsprintf() as LPSTR's!
//
//    Rev 1.9   07 Dec 1992 15:42:30   CRAIG
// Made string parms of _ConvertLongToAbbreviation LPCSTR's.
//
//    Rev 1.8   07 Dec 1992 15:37:38   CRAIG
// Fixed a typo.
//
//    Rev 1.7   07 Dec 1992 14:38:22   CRAIG
// Added ConvertLongToAbbreviation().
//
//    Rev 1.6   15 Sep 1992 08:34:26   ED
// Windows cleanup
//
//    Rev 1.5   14 Sep 1992 15:55:44   MARKK
// Stopped infinite recursion in StringMaxCopy
//
//    Rev 1.4   10 Sep 1992 18:39:08   BRUCE
// Eliminated unnecesary includes
//
//    Rev 1.3   08 Sep 1992 12:31:22   MARKK
// No change.
//
//    Rev 1.2   03 Sep 1992 12:20:18   ED
// Reversed the SYM_WIN logic in some places
//
//    Rev 1.1   02 Sep 1992 11:25:50   ED
// Updated
//
//    Rev 1.0   26 Aug 1992 17:00:50   ED
// Initial revision.
 ************************************************************************/

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
#if !defined(SYM_NTK)
#include "stddos.h"                    // DOS international stuff
#endif
#include "dbcs.h"

#if defined(SYM_NLM)
#include <ctype.h>
#include <stream.h>
#endif

extern  HINSTANCE       hInst_SYMKRNL;

#if !defined(SYM_NLM) && !defined(SYM_NTK)
#ifdef SYM_WIN
LPSTR LOCAL PASCAL __WinConvertLongToAbbreviation (DWORD number, LPBYTE string, int nType);
#else
extern  BYTE    FAR MEGABYTE_ABBR[];
extern  BYTE    FAR KILOBYTE_ABBR[];
extern  BYTE    FAR GIGABYTE_ABBR[];
#endif
#endif

#define COMPRESSION_START       ((BYTE)255)

/*@API:**********************************************************************
@Declaration: VOID  StringCompress(LPSTR lpCompressed, LPSTR lpSource)

@Description:
This function compresses a string into a more memory-efficient format.
Repetitive sequences of three or more characters are converted to a 3-byte
sequence, with the first byte beign a meta-symbol for compressed text, the
second byte being the repetition count, and the third byte being the character
to repeat.

@Parameters:
$lpCompressed$ Pointer to a buffer that contains the compressed string.

$lpSource$ Pointer to the string to be converted.

@Returns:
There is no return value for this function.

@See: StringUnCompress
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
VOID SYM_EXPORT WINAPI StringCompress(LPSTR compressedStr, LPSTR sourceStr)
{
    register    BYTE    ch;
    auto        BYTE    lastChar;
    auto        BYTE    repetitionCount;


    repetitionCount = lastChar = 0;     /* No characters yet.           */
    while (TRUE)
        {
        ch = *sourceStr++;              /* Get the character            */

                                        /* See if we should compress    */
        if (ch == lastChar)
            {
                                        /* Save it and continue         */
            repetitionCount++;
            continue;
            }
                                        /* Character not the same as the*/
                                        /* previous character.          */
                                        /* See if we need to flush a    */
                                        /* meta-character               */
        if (repetitionCount)
            {
            if (repetitionCount == 2)
                {
                                        /* Not worth compressing        */
                                        /* Save it uncompressed.        */
                *compressedStr++ = lastChar;
                }
            else if (repetitionCount > 2)
                {
                                        /* Save compressed              */
                *compressedStr++ = COMPRESSION_START;
                *compressedStr++ = repetitionCount;
                }
            *compressedStr++ = lastChar; /* Save the character itself   */
            }
                                        /*------------------------------*/
                                        /* If end of string, break.     */
                                        /*------------------------------*/
        if (ch == EOS)
            break;

        lastChar = ch;                  /* Save this character          */
        repetitionCount = 1;            /* One character so far         */
        }

    *compressedStr = EOS;               /* Mark EOS of compressed string*/
}


/*@API:**********************************************************************
@Declaration: VOID  StringUncompress (LPSTR lpDest, LPSTR lpCompressed)

@Description:
This function uncompresses a string that was compressed with the StringCompress
function.

@Parameters:
$lpDest$ Pointer to a buffer that contains the uncompressed string.

$lpCompressed$ Pointer to a string in compressed format

@Returns:
There is no return value for this function.

@See: StringCompress
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
VOID SYM_EXPORT WINAPI StringUncompress (LPSTR destStr, LPSTR compressedStr)
{
    register    BYTE            ch;
    auto        BYTE            i;
    auto        BYTE            repetitionCount;


                                        /* Stuff characters into the    */
                                        /* 'line', until we hit a       */
                                        /* EOS.                         */
    while (*compressedStr != EOS)
        {
                                        /* Get character out of buffer  */
        ch = *compressedStr++;
                                        /* If meta-symbol, we need to   */
                                        /* decompress it.               */
        if (ch == COMPRESSION_START)
            {
            repetitionCount = *compressedStr++;
            ch = *compressedStr++;
            for (i = 0; i < repetitionCount; i++)
                *destStr++ = ch;
            }
        else
            {
                                        /* Store it into current line   */
            *destStr++ = ch;
            }
        }
    *destStr = EOS;                     /* Mark EOS                     */
}

typedef int (WINAPI *STRCOMPPROC)(LPCSTR, LPCSTR);

LPCSTR  LOCAL PASCAL lstrnmove(LPSTR lpTo, LPCSTR lpFrom, UINT wLen);
int     LOCAL PASCAL lstrncompare(LPCSTR lpString1, LPCSTR lpString2,
                                UINT wLength, STRCOMPPROC lpfnComp);

#if !defined(SYM_NLM) && !defined(SYM_NTK)

/************************************************************************/
/*                                                                      */
/*  Description:                                                        */
/*      This function copies wLength characters from lpFrom to lpTo     */
/*  with a null appended at the end.                                    */
/*      It does not check if the string length of lpFrom is shorter     */
/*  than wLength.                                                       */
/*      It returns a pointer to the location (lpFrom + wLength).        */
/*      Subprocedure of lstrncompare().                                 */
/*                                                                      */
/************************************************************************/
/* 08/08/91 MW Function created.                                        */
/************************************************************************/
LPCSTR LOCAL PASCAL lstrnmove(LPSTR lpTo, LPCSTR lpFrom, UINT wLen)
{
    MEMMOVE(lpTo, lpFrom, wLen);
    lpTo[wLen] = EOS;
    return(lpFrom + wLen);
} /* lstrnmove */


/************************************************************************/
/*                                                                      */
/*  Description:                                                        */
/*      This function calls STRCMP((to do the comparison, dividing    */
/*  the string into several passes if necessary.                        */
/*      Engine for lstrncmp((and lstrncmpi().                          */
/*                                                                      */
/************************************************************************/
/* 08/08/91 MW Function created.                                        */
/************************************************************************/
#define MAX_LEN 150

int LOCAL PASCAL lstrncompare(LPCSTR   lpString1,
                              LPCSTR   lpString2,
                              UINT    wLength,
                              STRCOMPPROC lpfnComp)
{
    BYTE    byaBuffer1[MAX_LEN + 1];
    BYTE    byaBuffer2[MAX_LEN + 1];
    UINT    wPassLen;
    int     iReturn;

    /*
    **  The two strings are copied to the two buffers, respective, up to
    **    the size of the latter.
    **  The loop terminates when either the decided length is reached or
    **    when the two strings are found to be different.
    */
    while (TRUE)
        {
        wPassLen = min(wLength, MAX_LEN);
        wLength -= wPassLen;
        lpString1 = lstrnmove(byaBuffer1, lpString1, wPassLen);
        lpString2 = lstrnmove(byaBuffer2, lpString2, wPassLen);
        iReturn = (*lpfnComp)(byaBuffer1, byaBuffer2);

        if (wLength == 0  ||  iReturn != 0)
            return(iReturn);
        }

} /* lstrncompare */


/************************************************************************/
/*                                                                      */
/*  Description:                                                        */
/*      This function compares two strings with a given length.         */
/*      The comparison is made based on the current language selected   */
/*  and is case-sensitive.                                              */
/*                                                                      */
/* Don't document for Geologist. STRNCMP should be used instead.         */
/************************************************************************/
/* 08/08/91 MW Function created.                                        */
/************************************************************************/
int SYM_EXPORT WINAPI lstrncmp(LPCSTR lpString1, LPCSTR lpString2, UINT wLength)
{
#if defined(SYM_OS2) || defined(SYM_VXD)
    return strncmp(lpString1, lpString2, wLength);
#else
    return(lstrncompare(lpString1, lpString2, wLength, (STRCOMPPROC) lstrcmp));
#endif
} /* lstrncmp */


/************************************************************************/
/*                                                                      */
/*  Description:                                                        */
/*      This function compares two strings with a given length.         */
/*      The comparison is made based on the current language selected   */
/*  and is case-insensitive.                                            */
/*                                                                      */
/************************************************************************/
/* 08/08/91 MW Function created.                                        */
/************************************************************************/
int SYM_EXPORT WINAPI lstrncmpi(LPCSTR lpString1, LPCSTR lpString2, UINT wLength)
{
#if defined(SYM_VXD)
    return _strnicmp(lpString1, lpString2, wLength);
#elif defined(SYM_OS2)
    return strnicmp(lpString1, lpString2, wLength);
#else
    return(lstrncompare(lpString1, lpString2, wLength, (STRCOMPPROC) lstrcmpi));
#endif
} /* lstrncmpi */

#endif  // !NLM && !NTK

/*----------------------------------------------------------------------*
 * DOSIntlStringCompare()                                               *
 *                                                                      *
 * Provides the same functionality as the DOS platform lstrcmpi().      *
 * Uses the MSDOS collating table to do a CASE INSENSTIVE comparison;   *
 * does not use the Windows langauge DLL.  The intended purpose of      *
 * this function is for database key comparisons, where we have both    *
 * DOS and a Windows programs that need to access the same database.    *
 * Getting DOS and Windows to agree is more important than being right. *
 *----------------------------------------------------------------------*/

#ifdef SYM_PROTMODE
#if !defined(SYM_WIN32)
WORD gwCollatingTableSelector;

typedef struct
    {
    BYTE        byFunction;
    LPBYTE      lpTable;
    } INTLDATAREC, far *LPINTLDATAREC;

int SYM_EXPORT WINAPI DOSIntlStringCompare(LPCSTR lpString1, LPCSTR lpString2)
{
    static      BOOL            bFirstTime = TRUE;
    static      LPBYTE          lpTable = NULL;
    auto        union REGS      Regs;
    auto        struct SREGS    SRegs;
    auto        DWORD           dwSegSel;
    auto        LPINTLDATAREC   lpIntlData;
    auto        int             nReturn;

    // Get address of collating table on first call
    if (bFirstTime)
        {
        // Only need to get address once
        bFirstTime = FALSE;

        // Must be DOS 3.30 or greater to use collating table
        if (DOSGetVersion() < DOS_3_30)
            {
            // I'd prefer to just crash the system, as a subtle hint to the
            // user that they need to GET WITH IT and upgrade their DOS.
            // However I'll be nice and call the Windows function instead.
            return lstrcmpi(lpString1, lpString2);
            }

        // Get current country code to see if table is OK
        dwSegSel = GlobalDosAlloc(64);
        SRegs.es  = 0;
        SRegs.ds  = HIWORD(dwSegSel);
        Regs.x.dx = 0x0000;             // DS:DX = Junk buffer
        Regs.x.bx = 0x0000;             // BX = 0 for current country
        Regs.x.ax = 0x3800;
        IntWin(0x21, &Regs, &SRegs);
        GlobalDosFree(LOWORD(dwSegSel));

        // If country is Switzerland (41) we have to pretend the collating
        // table doesn't exist.  Microsoft has confirmed that the table is
        // just plain wrong in DOS 5 and 6.
        //if (Regs.x.bx == 41)
        //    return lstrcmpi(lpString1, lpString2);
        // In DOS 6.22, the Swiss collating table has been fixed.
        // -BCF 12/15/94

        // Get collating table pointer
        dwSegSel = GlobalDosAlloc(sizeof(INTLDATAREC));
        lpIntlData = MAKELP(LOWORD(dwSegSel), 0);
        lpIntlData->byFunction = 0x06;
        SRegs.es  = HIWORD(dwSegSel);
        Regs.x.di = 0x0000;             // ES:DI = Real mode INTLDATAREC
        Regs.x.ax = 0x6506;
        Regs.x.bx = 0xFFFF;
        Regs.x.dx = 0xFFFF;
        Regs.x.cx = sizeof(INTLDATAREC);
        IntWin(0x21, &Regs, &SRegs);
        if (!Regs.x.cflag && lpIntlData->lpTable != NULL)
            {
            SelectorAlloc(FP_SEG(lpIntlData->lpTable), 0xFFFF, &gwCollatingTableSelector);
            lpTable = MAKELP(gwCollatingTableSelector,
                             FP_OFF(lpIntlData->lpTable) + 2);
            }
        GlobalDosFree(LOWORD(dwSegSel));
        }

    // Must use Windows function if no collating table
    if (lpTable == NULL)
        return lstrcmpi(lpString1, lpString2);

    // Perform comparison using inline assembly for speed
    _asm
        {
        push    si
        push    di
        push    ds
        lds     bx,[lpTable]

NextByte:
        les     si,[lpString1]
        inc     word ptr [lpString1]
        mov     ah,es:[si]              ;Get byte from String1

        les     si,[lpString2]
        inc     word ptr [lpString2]
        mov     al,es:[si]              ;Get byte from String2

        or      ax,ax                   ;End of both strings?
        jz      Done

        xlat                            ;Convert String2 to sort value
        xchg    al,ah
        xlat                            ;Convert String1 to sort value

        cmp     al,ah                   ;Compare sort values
        je      NextByte

Done:
        sub     al,ah
        cbw
        pop     ds
        pop     di
        pop     si
        mov     [nReturn],ax
        }

    return(nReturn);
}
#endif                                  // #if !defined(SYM_WIN32)
#endif                                  // #if defined(SYM_PROTMODE)

/*@API:**********************************************************************
@Declaration: UINT  StringMaxLength (LPTSTR * lpArray)

@Description:
This function determines the length of the longest string in an array of
strings.

@Parameters:
$lpArray$ Pointer to an array of strings.

@Returns:
The return value is the length of the longest string.

@Include: xapi.h

@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
UINT SYM_EXPORT WINAPI StringMaxLength(LPTSTR *s)
{
    auto        UINT    maxLength;
    auto        UINT    sl;

                                        /* Search string until NULL     */
                                        /* pointer or first character   */
                                        /* is NULL                      */
    maxLength = 0;
    while (*s != NULL && **s)
        {
        if ((sl = STRLEN(*s)) > maxLength)
            maxLength = sl;

        s++;                            /* Next string                  */
        }

    return (maxLength);
}


/*@API:**********************************************************************
@Declaration: LPTSTR  StringSkipWhite (LPTSTR lpString)

@Description:
This function returns a pointer to the first non-white space character in a
string.  White space is considered to be any combination of spaces and tabs.

@Parameters:
$lpString$ Pointer to the string.

@Returns:
The return value is a pointer to first non-white space character, or the end of
the string if no non-white space characters are found.

@See: StringSkipToWhite StringRemoveTrailingSpaces
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringSkipWhite(LPTSTR lpString)
{
    LPTSTR ptr = lpString;       /* Starting character           */

                                        // DBCS-enabled.
    while (*ptr == ' ' || *ptr == '\t')
        ptr++;                          /* Skip over white space        */

    return(ptr);                        /* Return1st non-white char     */
}

/*@API:**********************************************************************
@Declaration: LPTSTR  StringSkipChar (LPTSTR lpString, TCHAR cChar)

@Description:
This function returns a pointer to the first character not equal to 'cChar'.
NOTE: 'cChar' can only be SBCS.  Don't see a need for DBCS at this time.

@Parameters:
$lpString$ Pointer to the string.
$cChar$ Character to skip over.

@Returns:
The return value is a pointer to first character not equal to 'cChar', or the
end of the string, if 'cChar' is not found.

@See: StringSkipWhite StringSkipToWhite StringRemoveTrailingSpaces
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringSkipChar(LPTSTR lpString, TCHAR cChar)
{
    LPTSTR ptr = lpString;       /* Starting character           */

                                        // DBCS-enabled.
    while (*ptr == cChar)
        ptr++;

    return(ptr);
}


/*@API:**********************************************************************
@Declaration: LPTSTR  StringSkipChars (LPTSTR lpszString, LPTSTR lpszChars)

@Description:
This function returns a pointer to the first character not equal to any of
the characters in 'lpszChars'.  This is the same as StringSkipChar(), but
it can take a string of characters.
NOTE: 'lpszChars' can only be SBCS.  Don't see a need for DBCS at this time.

@Parameters:
$lpszString$ Pointer to the string.
$lpszChars$ Pointer to list of characters that should be skipped over.

@Returns:
The return value is a pointer to first character not in the string 'lpszChars'.

@See: StringSkipChar StringSkipWhite StringSkipToWhite StringRemoveTrailingSpaces
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringSkipChars(LPTSTR lpString, LPTSTR lpszCharsStart)
{
    LPTSTR  ptr = lpString;       /* Starting character           */
    LPTSTR  lpszChars;
    BOOL    bFound;

                                        // DBCS-enabled.
    while (*ptr)
        {
        bFound = FALSE;
        lpszChars = lpszCharsStart;
        while (*lpszChars)
            {
            if (*ptr == *lpszChars)
                {
                bFound = TRUE;
                break;
                }
            lpszChars++;                // These must be SBCS, so okay.
            }
        if (!bFound)
            break;

        ptr++;                          // Can '++', since chars that it matches
        }                               // must be SBCS.

    return(ptr);
}

/*@API:**********************************************************************
@Declaration: VOID  STRCPYCHR (LPTSTR FAR *lplpszDest, LPTSTR FAR *lplpszSrc, TCHAR cChar)

@Description:
This function copies 1 string to another, until a specific character is
found.  The character found is NOT added.  NOTE: Currently, 'cChar' is
SBCS.  If you need it DBCS, we can change it.

@Parameters:
$lplpszDest$ Pointer to the destination string pointer.
$lplpszSrc$ Pointer to the source string pointer.
$cChar$ Character within 'lplpszSrc' that the strings will stop copying on.

@Returns:
No return value.  Since this rouine returns its values in its parameters,
doesn't make sense to return a value.

@See: STRCHR STRCPY
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
VOID SYM_EXPORT WINAPI STRCPYCHR(LPTSTR FAR *lplpszDest, LPTSTR FAR *lplpszSrc, TCHAR cChar)
{
    LPTSTR   lpszSrc     = *lplpszSrc;
    LPTSTR   lpszDest    = *lplpszDest;

    while (*lpszSrc && *lpszSrc != cChar)
        {
        if (DBCSIsLeadByte(*lpszSrc))
            *lpszDest++ = *lpszSrc++;

        *lpszDest++ = *lpszSrc++;
        }
    *lpszDest = EOS;

    *lplpszSrc  = lpszSrc;
    *lplpszDest = lpszDest;
}



/*@API:**********************************************************************
@Declaration: LPTSTR  StringSkipToWhite(LPTSTR lpString)

@Description:
This function returns a pointer to the first white space character in a string.
White space is considered to be any combination of spaces and tabs.

@Parameters:
$lpString$ Pointer to the string.

@Returns:
The return value is a pointer to first white space character, or the end of the
string if no white space characters are found.

@See: StringSkipWhite StringRemoveTrailingSpaces
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringSkipToWhite(LPTSTR lpString)
{
    LPTSTR ptr = lpString;               /* Starting character           */

    while (*ptr && *ptr != ' ' && *ptr != '\t')
        ptr = AnsiNext(ptr);            /* Find first white space       */
    return(ptr);                        /* Return this pointer          */
}

/*@API:******************************************************************
@Declaration: LPTSTR StringSkipIniDelim(LPTSTR lpString)

@Description:
This function skips over characters used as delimiters on a LOAD= or RUN= line.
Use in place of the StringSkipWhite function when scanning a line with multiple
items on it.

@Parameters:
$lpString$ Points to the string containing the INI delimiters.

@Returns:
The return value is a pointer to the first non-whitespace or non-comma
character in the string.

@See: StringSkipWhite
@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringSkipIniDelim(LPTSTR lpString)
{
    LPTSTR ptr = lpString;       /* Starting character           */

                                        // DBCS enabled.
    while (*ptr == ' ' || *ptr == '\t' || *ptr == ',')
        ptr++;                          /* Skip over white space & comma */

    return(ptr);                        /* Return 1st non-white/comma */
}

/*@API:******************************************************************
@Declaration: LPTSTR StringSkipToIniDelim (LPTSTR lpString)

@Description:
This function skips to the next delimiter on a LOAD= or RUN= line. Use in
place of StringSkipToWhite() when scanning a line with multiple items on it.

@Parameters:
$lpString$ Points to the string containing the INI delimiters.

@Returns:
The return value is a pointer to the first whitespace or comma character in
the string.

@See: StringSkipToWhite
@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringSkipToIniDelim (LPTSTR lpString)
{
    LPTSTR ptr = lpString;       /* Starting character           */

    while (*ptr && *ptr != ' ' && *ptr != '\t' && *ptr != ',')
        ptr = AnsiNext(ptr);            /* Find first white space or comma */
    return(ptr);                        /* Return this pointer          */
}


/*@API:**********************************************************************
@Declaration: LPTSTR  StringRemoveTrailingSpaces (LPTSTR lpString)

@Description:
This function removes trailing whitespace characters from a string.  Whitespace
is considered to be any combination of spaces and tabs.

@Parameters:
$lpString$ Pointer to the string.

@Returns:
The return value is a pointer to the string after stripping the whitespaces.

@See: StringSkipToWhite StringRemoveCRLF
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringRemoveTrailingSpaces(LPTSTR lpString)
{
    LPTSTR      ptr = lpString;
    LPTSTR      lpszFound;


                                        // Move in forward direction
    lpszFound = NULL;
    while (*ptr)
        {
        if (*ptr == ' ' || *ptr == '\t')
            {
            if (lpszFound == NULL)
                {
                lpszFound = ptr;
                }
            }
        else
            lpszFound = NULL;

        ptr = AnsiNext(ptr);
        }
                                        // Truncate at first trailing
                                        // white space.
    if (lpszFound)
        *lpszFound = EOS;

    return(ptr);
}


/*@API:**********************************************************************
@Declaration: LPTSTR  StringGetEnd (LPTSTR lpString)

@Description:
This function returns the location of the null terminator for a string.

@Parameters:
$lpString$ Pointer to the string.

@Returns:
The return value is a pointer to the null terminator for the string.

@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringGetEnd(LPTSTR lpszString)
{
    while (*lpszString)
        lpszString++;
    return(lpszString);
} /* StringGetEnd */

/*@API:******************************************************************
@Declaration: LPTSTR StringMaxCopy (LPTSTR lpDest, LPCTSTR lpSource,
UINT uMaxSize)

@Description:
This function copies one string to another, copying no more than the specified
number of characters.

@Parameters:
$lpDest$ Pointer to a string buffer to receive the string.

$lpSource$ Pointer to the string to copy.

$uMaxSize$ The maximum number of characters to copy. If the source string has
fewer than uMaxSize characters in it then this parameter has no affect.

@Returns:
The return value is a pointer to lpDest.

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringMaxCopy (LPTSTR lpDest, LPCTSTR lpSource, UINT uMaxSize)
{
    UINT        wLen = min ((UINT)STRLEN (lpSource), uMaxSize - 1);

    STRNCPY(lpDest, lpSource, wLen);
    *(lpDest + wLen) = EOS;

    return (lpDest);
}


#if !defined(SYM_NTK) // don't define some conversion routines

BYTE    szPrmStrThousand[2];
BOOL    bPrmStrThousandRead = FALSE;

#ifdef SYM_WIN

BYTE    szPrmStrINTL[] = "intl";
BYTE    szPrmStrTHOUSANDLABEL[] = "sThousand";
BYTE    szPrmStrCOMMA[] = ",";

#endif


/*@API:**********************************************************************
@Declaration: LPSTR  ConvertWordToString (UINT wNumber, LPSTR lpString)

@Description:
This function converts a UINT value into an ASCII string.  The string includes
thousands separator characters, which are based on the thousands separator
specified in WIN.INI (Windows) or the current country setting (DOS).

@Parameters:
$wNumber$ The number to be converted.

$lpString$ Pointer to a buffer that is to receive the converted string.

@Returns:
The return value is a pointer to the converted string.

@See: _ConvertWordToString ConvertStringToWord

@Include: xapi.h

@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPSTR SYM_EXPORT WINAPI ConvertWordToString (UINT wNumber, LPSTR lpString)
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
#endif  //  #ifdef SYM_OS2 else

    return (_ConvertWordToString (wNumber, lpString, 10, szPrmStrThousand[0]));
}



/*@API:**********************************************************************
@Declaration: LPSTR  ConvertLongToString (DWORD wNumber, LPSTR lpString)

@Description:
This function converts a DWORD value into an ASCII string.  The string includes
thousands separator characters, which are based on the thousands separator
specified in WIN.INI (Windows) or the current country setting (DOS).

@Parameters:
$wNumber$ The number to be converted.

$lpString$ Pointer to a buffer that is to receive the converted string.

@Returns:
The return value is a pointer to the converted string.

@See: ConvertStringToLong ConvertStringToWord
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPSTR SYM_EXPORT WINAPI ConvertLongToString (DWORD dwNumber, LPSTR lpString)
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
    return (_ConvertLongToString (dwNumber, lpString, 10, szPrmStrThousand[0]));
}
#endif // #if !defined(SYM_NTK)

/*@API:**********************************************************************
@Declaration: LPTSTR _ConvertWordToString (UINT wNumber, LPTSTR lpString, UINT
wRadix, TCHAR byThousandsChar)

@Description:
This function converts a UINT value into an ASCII string.

@Parameters:
$wNumber$ The number to be converted.

$lpString$ Pointer to a buffer that is to receive the converted string.

$wRadix$ The radix to use when converting the string.

$byThousandsChar$ The separator to use for designating thousands.  This value
should be zero to eliminate the separator.

@Returns:
The return value is a pointer to the converted string.

@See: ConvertStringToWord _ConvertLongToString ConvertStringToLong
@Include: xapi.h
@Compatibility: DOS, Win16, Win32, VxD, NTK
*****************************************************************************/
LPTSTR SYM_EXPORT WINAPI _ConvertWordToString (
    UINT wNumber,
    LPTSTR lpString,
    UINT wRadix,
    TCHAR byThousandsChar)
{

    auto        UINT            x;
    auto        int             nCount;
    auto        LPTSTR          lpTemp;


                                        /* Initialization               */
    nCount = 0;
    lpTemp = lpString;
                                        /* Convert the number from int  *
                                         *  to ascii                    */
    do  {
                                        /* Get the remainder            */
        x = (wNumber % wRadix);
                                        /* Add a comma?                 */
        if (byThousandsChar && (nCount++ == 3))
            {
                                        /* Yes                          */
            *lpString++ = byThousandsChar;
            nCount = 1;
            }

                                        /* Convert to letter            */
        *lpString++ = (TCHAR)((x <= 9) ? x + '0' : 'A' + (x - 10));

                                        /* Whittle down the number  */
        wNumber /= wRadix;

    } while (wNumber > 0);

                                        /* Null terminate the string    */
    *lpString = EOS;
                                        /* And reverse it               */
    STRREV (lpTemp);
                                        /* Return to caller             */
    return (lpTemp);

}


/*@API:**********************************************************************
@Declaration: LPTSTR  _ConvertLongToString (DWORD wNumber, LPTSTR lpString, UINT
wRadix, TCHAR byThousandsChar)

@Description:
This function converts a DWORD value into an ASCII string, based on the
specified radix.

@Parameters:
$wNumber$ The number to be converted.

$lpString$ Pointer to a buffer that is to receive the converted string.

$wRadix$ The radix to use when converting the string.

$byThousandsChar$ The separator to use for designating thousands.  This value
should be zero to eliminate the separator.

@Returns:
The return value is a pointer to the converted string.

@See: ConvertLongToString ConvertStringToLong ConvertStringToWord
@Include: xapi.h
@Compatibility: DOS, Win16, Win32, VXD, NTK
*****************************************************************************/

LPTSTR SYM_EXPORT WINAPI _ConvertLongToString (
    DWORD dwNumber,
    LPTSTR lpDest,
    UINT  wRadix,
    TCHAR byThousandsChar)
{
    auto        LPTSTR          lpString = lpDest;
    auto        UINT            x;
    auto        int             wCount;
    auto        LPTSTR          lpTemp;
                                        /* Initialization               */
    wCount = 0;
    lpTemp = lpString;
                                        /* Convert the number from int  *
                                         *  to ascii                    */
    do  {
                                        /* Get the remainder            */
        x = (UINT) (dwNumber % (DWORD) wRadix);
                                        /* Add a comma?                 */
        if (byThousandsChar && (wCount++ == 3))
            {
                                        /* Yes                          */
            *lpString++ = byThousandsChar;
            wCount = 1;
            }

                                        /* Convert to letter            */
        *lpString++ = (TCHAR)((x <= 9) ? x + '0' : 'A' + (x - 10));

                                        /* Whittle down the number  */
        dwNumber /= (DWORD)(wRadix);

    } while (dwNumber > 0);

                                        /* Null terminate the string    */
    *lpString = EOS;
                                        /* And reverse it               */
    STRREV (lpTemp);
                                        /* Return to caller             */
    return (lpTemp);

} /* ConvertLongToString    */

#if !defined(SYM_NTK) // don't define some conversion routines
/*@API:**********************************************************************
@Declaration: LPSTR   ConvertStringToWord(UINT FAR * lpwValue, LPSTR lpBuffer)

@Description:
This function converts an ASCII string value into a UINT value.  Preceding non-
numeric characters are skipped.

@Parameters:
$lpwValue$ Pointer to the variable that is to receive the converted value.

$lpBuffer$ Pointer to the string to be converted.

@Returns:
The return value is a pointer to the first character after the last numeric
character.

@See: _ConvertWordToString _ConvertLongToString
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPSTR SYM_EXPORT WINAPI ConvertStringToWord (UINT FAR * lpwValue, LPSTR lpBuffer)
{
    DWORD       dwValue;
    LPSTR       lpStr;

    lpStr = _ConvertStringToLong( &dwValue, lpBuffer, 10, FALSE );
    *lpwValue = (UINT) dwValue;
    return (lpStr);
}


/*@API:**********************************************************************
@Declaration: LPSTR   ConvertStringToLong(DWORD FAR * lpwValue, LPSTR lpBuffer)

@Description:
This function converts an ASCII string value into a DWORD value.  Preceding
non-numeric characters are skipped.

@Parameters:
$lpwValue$ Pointer to the variable that is to receive the converted value.

$lpBuffer$ Pointer to the string to be converted.

@Returns:
The return value is a pointer to the first character after the last numeric
character.

@See: _ConvertLongToString _ConvertStringToLong
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPSTR SYM_EXPORT WINAPI ConvertStringToLong (DWORD FAR * lpdwValue, LPSTR lpBuffer)
{
    return _ConvertStringToLong( lpdwValue, lpBuffer, 10, FALSE );
}


/*@API:**********************************************************************
@Declaration: LPSTR _ConvertStringToLong(DWORD FAR * lpwValue, LPSTR lpBuffer,
UINT uiRadix, BYTE bIgnoreSeparators)

@Description:
This function converts an ASCII string value into a DWORD value.  Preceding
non-numeric characters are skipped.

@Parameters:
$lpwValue$ Pointer to the variable that is to receive the converted value.

$lpBuffer$ Pointer to the string to be converted.

$uiRadix$ Specifies what base to use in the conversion.

$bIgnoreSeparators$ Specifies that separators, such as a comma (,), be ignored.

@Returns:
The return value is a pointer to the first character after the last numeric
character.

@See: _ConvertLongToString ConvertStringToLong
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
LPSTR SYM_EXPORT WINAPI _ConvertStringToLong( DWORD FAR *lpdwValue, LPSTR lpszString,
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
    DWORD   dwValue = 0UL;
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

            dwValue = (dwValue * wRadix) +
                      (DWORD) (byChar > '9'? byChar - '7': byChar - '0');
            }

        lpszString++;
        }

    *lpdwValue = dwValue;
    return lpszString;
}
#endif // #if defined(SYM_NTK)


/*@API:******************************************************************
@Declaration: BOOL CharIsHexNumeric (UINT ch)

@Description:
This function determines if a character is within the hexadecimal number set
of characters (0-9, A-F, and a-f).

@Parameters:
$ch$ The character to check.

@Returns:
The return value is TRUE if the character is a hexadecimal character; FALSE
otherwise.

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
BOOL SYM_EXPORT WINAPI CharIsHexNumeric (UINT ch)
{
    return ((BOOL)(((ch >= '0') && (ch <= '9')) ||
                         ((ch >= 'A') && (ch <= 'F')) ||
                         ((ch >= 'a') && (ch <= 'f'))));
}


/*@API:*******************************************************************
@Declaration: BOOL CharIsNumeric (UINT ch)

@Description:
This function determines if a character is a numeric digit (0-9).

@Parameters:
$ch$ The character to check.

@Returns:
The return value is TRUE if the character is a numeric digit; FALSE
otherwise.

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
BOOL SYM_EXPORT WINAPI CharIsNumeric (UINT ch)
{
    return ((BOOL)((ch >= '0') && (ch <= '9')));
}


/*@API:******************************************************************
@Declaration: BOOL CharIsWhite (UINT ch)

@Description:
This function determines if a character is whitespace (' ' or '\t').

@Parameters:
$ch$ The character to check.

@Returns:
The return value is TRUE if the character is a whitespace character; FALSE
otherwise.

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
BOOL SYM_EXPORT WINAPI CharIsWhite (UINT ch)
{
    return ((BOOL)(ch == ' ' || ch == '\t'));
}


#ifndef SYM_WIN                           // the following are macros in Windows

/*@API:******************************************************************
@Declaration: BOOL CharIsAlpha (UINT ch)

@Description:
This function checks to see if a character is alphabetic.

@Parameters:
$ch$ The character to check.

@Returns:
The return value is TRUE if the character is alphabetic; FALSE otherwise.

@Comments:
This function doesn't use the standard 'C' routine isalpha because the
IBM extended foreign alphabetic characters must be supported.

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, NLM and VxD
************************************************************************/
BOOL WINAPI CharIsAlpha (UINT ch)
{
        // All characters have up/low versions.  Use this as the
        // criteria for defining an alpha character.

#if !defined(SYM_VXD)
    TCHAR lowCh = CharToLower((TCHAR)ch);
    TCHAR upCh  = CharToUpper((TCHAR)ch);
    return((BOOL)(lowCh != upCh));
#else
                                        // For VxD's, there is only upper
                                        // case, so simply test for
                                        // 'A' -> 'Z' for now.
    UINT        chUpper = CharToUpper (ch);
    return ((BOOL)(('A' <= chUpper) && (chUpper <= 'Z')));
#endif
}

/*@API:******************************************************************
@Declaration: BOOL CharIsLower (UINT ch)

@Description:
This function checks to see if a character is lowercase.

@Parameters:
$ch$ The character to check.

@Returns:
The return value is TRUE if the character is lowercase; FALSE otherwise.

@Comments:
This function doesn't use the standard 'C' routine isalpha because the
IBM extended foreign alphabetic characters must be supported.

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, NLM and VxD
************************************************************************/
BOOL WINAPI CharIsLower (UINT ch)
{
    TCHAR ch2 = CharToLower ((TCHAR)ch);

    return ((BOOL)(ch == ch2));
}

/*@API:******************************************************************
@Declaration: BOOL CharIsUpper(UINT ch)

@Description:
This function checks to see if a character is uppercase.

@Parameters:
$ch$ The character to check.

@Returns:
The return value is TRUE if the character is uppercase; FALSE otherwise.

@Comments:
This function doesn't use the standard 'C' routine isalpha because the
IBM extended foreign alphabetic characters must be supported.

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
BOOL WINAPI CharIsUpper (UINT ch)
{
    TCHAR ch2 = CharToUpper ((TCHAR)ch);

    return ((BOOL)(ch == ch2));
}

/*@API:******************************************************************
@Declaration: BOOL CharIsAlphaNumeric(UINT ch)

@Description:
This function checks to see if a character is alphabetic or numeric.

@Parameters:
$ch$ The character to check.

@Returns:
The return value is TRUE if the character is alphanumeric; FALSE otherwise.

@Comments:
This function doesn't use the standard 'C' routine isalpha because the
IBM extended foreign alphabetic characters must be supported.

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
#if !defined(SYM_VXD) && !defined(SYM_NTK)
BOOL WINAPI CharIsAlphaNumeric (UINT ch)
{
    return ((BOOL) (CharIsAlpha(ch) || CharIsNumeric(ch)));
}
#endif

#endif

/////////////////////////////////////////////////////////////////////////////

#ifdef SYM_WIN

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Load the appropriate abbreviated strings from the resources.    *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 06/10/1993 ED Function Created.                                      *
 ************************************************************************/

enum { CLTA_ABBR, CLTA_FULL, CLTA_SHORT };

LPSTR LOCAL PASCAL __WinConvertLongToAbbreviation (DWORD number, LPBYTE string, int nType)
{
    char        szMB [25], szKB [25];
    int         nStrK, nStrM;

    switch (nType)
        {
        case CLTA_ABBR:
            nStrK = ABRVSIZ_K;
            nStrM = ABRVSIZ_M;
            break;

        case CLTA_FULL:
            nStrK = KILOBYTE_STR;
            nStrM = MEGABYTE_STR;
            break;

        case CLTA_SHORT:
            nStrK = SHORT_KILOBYTE_STR;
            nStrM = SHORT_MEGABYTE_STR;
            break;
        }

    LoadString( hInst_SYMKRNL, nStrK, szKB, sizeof(szKB));
    LoadString( hInst_SYMKRNL, nStrM, szMB, sizeof(szMB));

    return _ConvertLongToAbbreviation (number, string, szKB, szMB);
}
#endif

#if !defined(SYM_OS2) && !defined(SYM_NTK)

/*****************************************************************************
 * Do not document as part of Quake API, ConvertLongToAbbreviation and
 * ConvertSizeToFullString are the API "covers" to this function.
 ****************************************************************************/
LPSTR SYM_EXPORT WINAPI _ConvertLongToAbbreviation(DWORD number, LPBYTE string,
                                               LPCSTR kiloStr,  LPCSTR megaStr)
{
#ifdef SYM_NLM
    (void) number;
    (void) string;
    (void) kiloStr;
    (void) megaStr;
#else

#ifndef ONEK
#define ONEK ((DWORD)(1024))
#define ONEM ((DWORD)(ONEK*1024))
#endif

#ifdef SYM_WIN
    char szDecimalString[INTL_MAXLEN_sDECIMAL];
#endif

    UINT meg;
	BOOL bKilo=FALSE;

    if (number == 0)				// if 0, show 0K
        {
        STRCPY(string,"0");
        STRCAT(string, kiloStr);
		return (string);
        }

	if (number >= ONEM)
		{
    	number /= ONEK;
		bKilo=TRUE;					// Indicate we're working with K's
		}

    if (number < 10*ONEK)         	// < 10K or 10M show 2 decimal places
        {
        meg = (UINT)(((number * 100L) + ONEK/2) / ONEK);

		if ( meg == 0 && number !=0) // 0 -> .049K Convert to 0.01K
			meg = 10;

#ifdef SYM_WIN
        NIntlGetString( szDecimalString, INTL_ENUM_DECIMAL );
        wsprintf(string, "%d%s%02d%s",
        		 meg / 100,
                 (LPSTR) szDecimalString,
                 meg % 100,
                 (bKilo ? megaStr : kiloStr));
#elif defined(SYM_DOS)
        StringPrint(string, "%d1%c2%02d3%s4",
        			meg / 100,
                    international.decimalChar,
                    meg % 100,
                    (bKilo ? megaStr : kiloStr));
#endif
        }
    else if (number < 100*ONEK)         	// >10K or 10M && < 100K or 100M show 1 decimal place
        {
        meg = (UINT)(((number * 10L) + ONEK/20) / ONEK);

#ifdef SYM_WIN
        NIntlGetString( szDecimalString, INTL_ENUM_DECIMAL );
        wsprintf(string, "%d%s%1d%s",
        		 meg / 10,
                 (LPSTR) szDecimalString,
                 meg % 10,
                 (bKilo ? megaStr : kiloStr));
#elif defined(SYM_DOS)
        StringPrint(string, "%d1%c2%1d3%s4",
        			meg / 10,
                    international.decimalChar,
                    meg % 10,
                    (bKilo ? megaStr : kiloStr));
#endif
        }
    else                                // >100K or 100Meg, just show whole number
        {
        meg = (UINT) ((number + ONEK/2) / ONEK);
        _ConvertWordToString(meg, string, 10, 0);
        STRCAT(string, (bKilo ? megaStr : kiloStr));
        }
#endif
    return (string);
}

/*@API:*******************************************************************
@Declaration: LPSTR ConvertLongToAbbreviation(DWORD number, LPSTR string)

@Description:
This function converts long numbers assumed to be values in bytes, as in
100,000,000 bytes, to an abbreviated string with the largest size specifier,
as in "100M".

Converted number precision is as follows:
0                   0K
1 -> 9.99K          x.yyK            <two decimal places>
10K -> 99.9K        xx.yK            <one decimal place>
100K-> 1023K        xxxxK            <no fraction>

1M -> 9.99M         x.yyM            <two decimal places>
10M -> 99.9M        xx.yM            <one decimal place>
100M-> ...          xxxxM            <no fraction>

@Parameters:
$number$ Specifies the number to convert.

$string$ Points to a string buffer large enough to hold the result.

@Returns:
The return value is a pointer to the string.

@See: ConvertSizeToFullString ConvertSizeToShortString
@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPSTR SYM_EXPORT WINAPI ConvertLongToAbbreviation(DWORD number, LPSTR string)
{
#ifdef SYM_NLM
    (void) number;
    (void) string;

    return string;
#else
#ifdef SYM_WIN
    return __WinConvertLongToAbbreviation (number, string, CLTA_ABBR);
#else
    return (_ConvertLongToAbbreviation(number, string,
                                       KILOBYTE_ABBR, MEGABYTE_ABBR));
#endif
#endif
}

/*@API:*******************************************************************
@Declaration: LPSTR ConvertLongKBsToAbbreviation(DWORD number, LPSTR string)

@Description:
This function converts long numbers assumed to be values in kilobytes, as in
100,000,000 KB, to an abbreviated string with the largest size specifier,
as in "100G". This fn is needed due to the 4GB limit of DWORD's.

Converted number precision is as follows:
0                   0K
1 -> 9.99M          x.yyK            <two decimal places>
10M -> 99.9M        xx.yK            <one decimal place>
100M-> 1023M        xxxxK            <no fraction>

1G -> 9.99G         x.yyM            <two decimal places>
10G -> 99.9G        xx.yM            <one decimal place>
100G-> ...          xxxxM            <no fraction>

@Parameters:
$number$ Specifies the number to convert.

$string$ Points to a string buffer large enough to hold the result.

@Returns:
The return value is a pointer to the string.

@See: ConvertSizeToFullString ConvertSizeToShortString
@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPSTR SYM_EXPORT WINAPI ConvertLongKBsToAbbreviation(DWORD number, LPSTR string)
{
#ifdef SYM_NLM
    (void) number;
    (void) string;

    return string;
#else
#ifdef SYM_WIN
    char        szMB [25], szGB [25];

    LoadString( hInst_SYMKRNL, ABRVSIZ_M, szMB, sizeof(szMB));
    LoadString( hInst_SYMKRNL, ABRVSIZ_G, szGB, sizeof(szGB));

    return (_ConvertLongKBsToAbbreviation(number, string, szMB, szGB));
#else
    return (_ConvertLongKBsToAbbreviation(number, string,
                                       MEGABYTE_ABBR, GIGABYTE_ABBR));
#endif  // SYM_WIN
#endif  // SYM_NLM
}


/*****************************************************************************
 * Do not document as part of Quake API, ConvertLongKBsToAbbreviation and
 * ConvertSizeToFullString are the API "covers" to this function.
 ****************************************************************************/
LPSTR SYM_EXPORT WINAPI _ConvertLongKBsToAbbreviation(DWORD number, LPBYTE string,
                                               LPCSTR megaStr,  LPCSTR gigaStr)
{
#ifdef SYM_NLM
    (void) number;
    (void) string;
    (void) megaStr;
    (void) gigaStr;
#else

#ifndef ONEK
#define ONEK ((DWORD)(1024))
#define ONEM ((DWORD)(ONEK*1024))
#endif

#ifdef SYM_WIN
    char szDecimalString[INTL_MAXLEN_sDECIMAL];
#endif

    UINT gig;
	BOOL bMega=FALSE;
    

    if (number == 0)				// if 0, show 0K
        {
        STRCPY(string,"0");
        STRCAT(string, megaStr);
		return (string);
        }

	if (number >= ONEM)             // This really indicates we're working with M's
		{
    	number /= ONEK;
		bMega=TRUE;					// Indicate we're working with M's
		}

    if (number < 10*ONEK)         	// < 10M or 10G show 2 decimal places
        {
        gig = (UINT)(((number * 100L) + ONEK/2) / ONEK);

		if ( gig == 0 && number !=0) // 0 -> .049K Convert to 0.01K
			gig = 10;

#ifdef SYM_WIN
        NIntlGetString( szDecimalString, INTL_ENUM_DECIMAL );
        wsprintf(string, "%d%s%02d%s",
        		 gig / 100,
                 (LPSTR) szDecimalString,
                 gig % 100,
                 (bMega ? gigaStr : megaStr));
#elif defined(SYM_DOS)
        StringPrint(string, "%d1%c2%02d3%s4",
        			gig / 100,
                    international.decimalChar,
                    gig % 100,
                    (bMega ? gigaStr : megaStr));
#endif
        }
    else if (number < 100*ONEK)         	// >10M or 10G && < 100M or 100G show 1 decimal place
        {
        gig = (UINT)(((number * 10L) + ONEK/20) / ONEK);

#ifdef SYM_WIN
        NIntlGetString( szDecimalString, INTL_ENUM_DECIMAL );
        wsprintf(string, "%d%s%1d%s",
        		 gig / 10,
                 (LPSTR) szDecimalString,
                 gig % 10,
                 (bMega ? gigaStr : megaStr));
#elif defined(SYM_DOS)
        StringPrint(string, "%d1%c2%1d3%s4",
        			gig / 10,
                    international.decimalChar,
                    gig % 10,
                    (bMega ? gigaStr : megaStr));
#endif
        }
    else                                // >100M or 100G, just show whole number
        {
        gig = (UINT) ((number + ONEK/2) / ONEK);
        _ConvertWordToString(gig, string, 10, 0);
        STRCAT(string, (bMega ? gigaStr : megaStr));
        }
#endif
    return (string);
}

/*@API:******************************************************************
@Declaration: LPSTR ConvertSizeToFullString(DWORD number, LPSTR string)

@Description:
This function converts long numbers assumed to be values in bytes, as in
100,000,000 bytes, to a string with the largest size specifier, as in
"100 Megabytes".

Converted number precision is as follows:
0                   0K
1 -> 9.99K          x.yyK            <two decimal places>
10K -> 99.9K        xx.yK            <one decimal place>
100K-> 1023K        xxxxK            <no fraction>

1M -> 9.99M         x.yyM            <two decimal places>
10M -> 99.9M        xx.yM            <one decimal place>
100M-> ...          xxxxM            <no fraction>

@Parameters:
$number$ Specifies the number to convert.

$string$ Points to a string buffer large enough to hold the result.

@Returns:
The return value is a pointer to the string.

@See: ConvertSizeToAbbreviation ConvertSizeToShortString
@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPSTR SYM_EXPORT WINAPI ConvertSizeToFullString(DWORD number, LPSTR string)
{
#ifdef SYM_NLM
    (void) number;
    (void) string;

    return string;
#else
#ifdef SYM_WIN
    return __WinConvertLongToAbbreviation (number, string, CLTA_FULL);
#else
    extern      char    FAR KILOBYTE_STR[];
    extern      char    FAR MEGABYTE_STR[];

    return (_ConvertLongToAbbreviation(number, string,
                                          KILOBYTE_STR, MEGABYTE_STR));
#endif
#endif
}


/*@API:******************************************************************
@Declaration: LPSTR ConvertSizeToShortString(DWORD number, LPSTR string)

@Description:
This function converts long numbers assumed to be values in bytes, as in
100,000,000 bytes, to an abbreviated string with the largest size specifier,
as in "100 MB".

Converted number precision is as follows:
0                   0K
1 -> 9.99K          x.yyK            <two decimal places>
10K -> 99.9K        xx.yK            <one decimal place>
100K-> 1023K        xxxxK            <no fraction>

1M -> 9.99M         x.yyM            <two decimal places>
10M -> 99.9M        xx.yM            <one decimal place>
100M-> ...          xxxxM            <no fraction>

@Parameters:
$number$ Specifies the number to convert.

$string$ Points to a string buffer large enough to hold the result.

@Returns:
The return value is a pointer to the string.

@See: ConvertSizeToAbbreviation ConvertSizeToFullString
@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPSTR SYM_EXPORT WINAPI ConvertSizeToShortString(DWORD number, LPSTR string)
{
#ifdef SYM_NLM
    (void) number;
    (void) string;

    return string;

#else
#ifdef SYM_WIN
    return __WinConvertLongToAbbreviation (number, string, CLTA_SHORT);
#else
    extern      char    FAR SHORT_KILOBYTE_STR[];
    extern      char    FAR SHORT_MEGABYTE_STR[];

    return (_ConvertLongToAbbreviation(number, string,
                                SHORT_KILOBYTE_STR, SHORT_MEGABYTE_STR));
#endif
#endif
}
#endif // #if !defined(SYM_OS2) && !defined(SYM_NTK)


/*@API:******************************************************************
@Declaration: LPTSTR StringSeparate (LPTSTR lpString)

@Description:
This function replaces separator characters in a string with null-characters.
The last character of the string is assumed to be the separator character.

@Parameters:
$lpString$ Points to the string with separators within it.

@Returns:
The return value is a pointer to the converted string.

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringSeparate (LPTSTR lpString)
{
    TCHAR       ch;
    int         i;

                                        // get the delimiter at the end
    ch = lpString [STRLEN (lpString) - 1];

                                        // find and replace all delimiters
    for (i = 0; lpString [i] != '\0'; i++)
        {
        if (lpString [i] == ch)
            lpString [i] = '\0';
        else if (DBCSIsLeadByte(lpString[i]))
            i++;
        }

    return (lpString);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Removes CR/LF from string.                                      *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 01/11/1993 ED Function Created.                                      *
 ************************************************************************/


/*@API:******************************************************************
@Declaration: LPTSTR StringRemoveCRLF (LPTSTR lpszString)

@Description:
This function removes trailing carriage return (CR) and line feed (LF)
characters from a string.

@Parameters:
$lpString$ Pointer to the string to remove CR/LF characters from.

@Returns:
The return value is the input string minus any CR/LF characters.

@Comments:
This function does not remove all CR/LF characters from a string. For example,
the string "I am \r\n splitting this line up.\r\n" becomes "I am \r\n
splitting this line up."

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringRemoveCRLF (LPTSTR lpszString)
{
    LPTSTR      lpszStart = lpszString;
    LPTSTR      lpszFound;

                                        // Move in forward direction
    lpszFound = NULL;
    while (*lpszString)
        {
        if ((*lpszString == '\r' || *lpszString == '\n') && (lpszFound == NULL))
            lpszFound = lpszString;
        else
            lpszFound = NULL;

        lpszString = AnsiNext(lpszString);
        }
                                        // Truncate at first trailing
                                        // white space.
    if (lpszFound)
        *lpszFound = EOS;

    return(lpszStart);
}


/*----------------------------------------------------------------------*
 * This procedure removes the leading and trailing spaces form a        *
 * string.                                                              *
 *----------------------------------------------------------------------*/

/*@API:******************************************************************
@Declaration: LPTSTR StringStripSpaces(LPTSTR lpString)

@Description:
This function removes the leading and trailing whitespace from a string.

@Parameters:
$lpString$ Points to the string to remove spaces from.

@Returns:
The return value is a pointer to the original string with stripped whitespace.

@See: StringRemoveTrailingSpaces
@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringStripSpaces(LPTSTR lpszString)
{
    LPTSTR      lpszStart = lpszString;
    LPTSTR      lpszFound;


    while (*lpszString == ' ' || *lpszString == '\t')
        lpszString++;                   /* Skip over leading spaces     */
    STRCPY(lpszStart, lpszString);      /* Remove these spaces          */

                                        // Move in forward direction
    lpszFound = NULL;
    lpszString = lpszStart;
    while (*lpszString)
        {
        if (*lpszString == ' ' || *lpszString == '\t')
            {
            if (lpszFound == NULL)
                {
                lpszFound = lpszString;
                }
            }
        else
            lpszFound = NULL;

        lpszString = AnsiNext(lpszString);
        }
                                        // Truncate at first trailing
                                        // white space.
    if (lpszFound)
        *lpszFound = EOS;

    return(lpszStart);
}


/*@API:******************************************************************
@Declaration: LPTSTR StringAppendChar(LPTSTR lpString, WCHAR wChar)

@Description:
This function appends a single character to a string.  DBCS-enabled.

@Parameters:
$lpString$ Points to the string to append the character to. Be sure that the
string has space for the character.

$wChar$ Specifies the character to append.  Note that it's a WCHAR.  If you
pass it a single-byte char, it will get converted to a WCHAR.

@Returns:
The return value is a pointer to the string.

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringAppendChar(LPTSTR lpString, WCHAR wChar)
{
    auto        WORD    wLen;


    wLen = STRLEN(lpString);
#if defined(UNICODE)
    *(lpString + wLen) = wChar;
    *(lpString + wLen + 1) = EOS;
#else
    if (HIBYTE(wChar))
        {
                                        // DBCS.
        *((LPWSTR) (lpString + wLen)) = wChar;
        *(lpString + wLen + sizeof(wChar)) = EOS;
        }
    else
        {
        *(lpString + wLen) = (char) wChar;
        *(lpString + wLen + 1) = EOS;
        }
#endif
    return(lpString);
}

/*@API:******************************************************************
@Declaration: LPTSTR StringAppendNChars(LPTSTR lpString, WCHAR wChar, int nCount)

@Description:
This function appends n characters to a string.  DBCS-enabled.

@Parameters:
$lpString$ Points to the string to append the characters to. Be sure that the
string has space for the characters.

$wChar$ Specifies the character to append.  Note that it's a WCHAR.  If you
pass it a single-byte char, it will get converted to a WCHAR.

$nCount$ Number of times to append the character, 'wChar'.

@Returns:
The return value is a pointer to the string.

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringAppendNChars(LPTSTR lpString, WCHAR wChar, int nCount)
{
    auto        int         i;
    auto        LPTSTR      lpszStart = lpString;


    lpString += STRLEN(lpString);
#if defined(UNICODE)
    for (i = 0; i < nCount; i++)
        *lpString++ = wChar;
#else
    if (HIBYTE(wChar))
        {
                                        // DBCS.
        for (i = 0; i < nCount; i++, lpString += sizeof(WCHAR))
            *((LPWSTR) lpString) = wChar;
        }
    else
        {
        for (i = 0; i < nCount; i++)
            *lpString++ = (char) wChar;
        }
#endif
    *lpString = EOS;

    return(lpszStart);
}


/*@API:******************************************************************
@Declaration: LPTSTR StringStripChar(LPTSTR lpszString)

@Description:
This function removes the last character from a string.

@Parameters:
$lpszString$ Points to the string to remove the last character from.

@Returns:
The return value is a pointer to the string.

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
LPTSTR SYM_EXPORT WINAPI StringStripChar(LPTSTR lpszString)
{
    LPTSTR       lpszStart, lpszLast;


    lpszStart = lpszLast = lpszString;
    while (*lpszString)
        {
        lpszLast = lpszString;
        lpszString = AnsiNext(lpszString);
        }
    *lpszLast = EOS;

    return(lpszStart);
}


/*@API:******************************************************************
@Declaration: WCHAR StringGetLastChar(LPCTSTR lpszString)

@Description:
This function returns the last character in a string.  Note that its
a WCHAR.

@Parameters:
$lpszString$ Points to the string to get the last character from.

@Returns:
The return value is the last character in the string.  Note that it's
a WCHAR.

@Include: xapi.h
@Compatibility: Win16, Win32, DOS, and NLM
************************************************************************/
WCHAR SYM_EXPORT WINAPI StringGetLastChar(LPCTSTR lpszString)
{
    LPCTSTR     lpszLast = lpszString;
    WCHAR       wChar;


    while (*lpszString)
        {
        lpszLast = lpszString;
        lpszString = AnsiNext(lpszString);
        }
    if (DBCSIsLeadByte(*lpszLast))
        wChar = *((LPWSTR) lpszLast);
    else
        wChar = *lpszLast;
    return(wChar);
}
