// Copyright 1996 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/navname.cpv   1.1   15 Aug 1997 11:00:22   mdunn  $
//
// Description:
//      Routines to process File Names. This is the NAV equivalent for
//      CORE\SYMKRNL\NAME.C.
//
//****************************************************************************
// $Log:   S:/NAVXUTIL/VCS/navname.cpv  $
// 
//    Rev 1.1   15 Aug 1997 11:00:22   mdunn
// Added NavNameIsReservedName()
// 
//    Rev 1.0   06 Feb 1997 21:05:38   RFULLER
// Initial revision
// 
//    Rev 1.2   05 Dec 1996 12:55:34   JBRENNA
// NavNameFormat(): Add support for processing NewLine characters in the format
// string.
// 
//    Rev 1.1   02 Dec 1996 15:09:52   JBRENNA
// Fix NavNameFormat() so that the first "%s" string is replaced with the
// formated filename. Previously, the %s had to be a separate word in the
// string. Now %s can be embedding in words.
// 
//    Rev 1.0   08 Nov 1996 17:37:12   JBRENNA
// Initial revision.
// 
//****************************************************************************

#include "platform.h"
#include "dbcs.h"
#include "navutil.h"

//****************************************************************************
// Local Prototypes **********************************************************
//****************************************************************************

static LPCSTR CopyCharsThatFit (HDC hDC, LPCSTR lpInput, LPSTR lpOutput,
                                long lWidth);

//****************************************************************************
// Static Functions **********************************************************
//****************************************************************************

//****************************************************************************
// Static Variables **********************************************************
//****************************************************************************

// List of filenames which are "reserved" and are not scanned for viruses
// by NAVW.  These strings are not translated, and must contain SB
// characters only.  A '?' in a string here is a wildcard that matches a
// number when the string is compared to a filename.
// All letters in these strings MUST be in uppercase.

LPCTSTR g_aszReservedNames[] =
{
    _T("CON"),
    _T("NUL"),
    _T("AUX"),
    _T("PRN"),
    _T("LPT?"),
    _T("COM?"),
    _T("NETQ"),
    NULL                                // sentinel - end of table
};

//****************************************************************************
// CopyCharsThatFit()
//
// Description:
//   Copies just enough characters from lpInput consume the space specified
//   by dwWidth.
//
// Parameters:
//   hDC       [in]   Device Context used for GetTextExtentPoint().
//   lpInput   [in]   String to partially copy.
//   lpOutput  [out]  Where to copy lpInput to.
//   lWidth    [in]   Width to be consumed by some lpInput characters.
//
//****************************************************************************
// 10/30/1996 Jim Brennan - created.
//****************************************************************************
static LPCSTR CopyCharsThatFit (HDC hDC, LPCSTR lpInput, LPSTR lpOutput,
                                long lWidth)
{
    auto   char    szChar[3];
    auto   WORD    wCharLen;
    auto   BOOL    bSuccess;
    auto   SIZE    sExtents;

    while (lWidth >= 0 && *lpInput)
    {
        wCharLen           = 0;
        szChar[wCharLen++] = lpInput[0];

        if (DBCSIsLeadByte (szChar[0]))
            szChar[wCharLen++] = lpInput[1];

        szChar[wCharLen] = EOS;

        bSuccess = GetTextExtentPoint (hDC, szChar, wCharLen, &sExtents);
        if (!bSuccess)
            MEMCPY (&sExtents, 0, sizeof (sExtents));
        else
            sExtents.cx += GetTextCharacterExtra (hDC);

        lWidth -= sExtents.cx;

        if (lWidth >= 0)
        {
            STRCPY (lpOutput, szChar);
            lpInput  += wCharLen;
            lpOutput += wCharLen;
        }
    }

    return lpInput;
}

//****************************************************************************
// External Functions ********************************************************
//****************************************************************************

//****************************************************************************
// NavNameShortenToWindow()
//
// Description:
//   Use the NameShortenFileName() to shorten the filename. The difference is
//   that this function calculates the maximum possible number of characters
//   given the actually filename and the current font. Unforunately, this
//   algorithm is expensive. Without rewriting NameShortenFileName(), there is
//   no easy way to accurately add the "..." given the filename.
//
// Parameters:
//   hWnd           [in]   Control that determines the font and width info.
//   lpszName       [in]   Original long version of the filename.
//   lpszShortName  [out]  Shorten version of the filename.
//   nLines         [in]   Number of lines that the control is allowing for
//                         the filename.
//   nUsedPixels    [in]   Number of pixels for the line that are already
//                         consumed by prefix text. Use AlignWndGetTextSize
//                         to calculate text size. Usually this is 0.
//
//****************************************************************************
// 05/09/1995 KEITH MUND Function Created (NameShortenFileNameForControl).
// 10/29/1996 Jim Brennan - Rename func to ShortenFileNameToWindow;
//            dramatically rewrite the calling of NameShortenFileName() to be
//            more accurate; move the adding CRLF code to FormatFileName().
// 11/08/1996 Jim Brennan - Move to NAVXUTIL project and rename to
//            NavNameShortenToWindow(); Add nUsedPixels parameter.
//****************************************************************************
void SYM_EXPORT NavNameShortenToWindow (HWND hWnd, LPCSTR lpszName,
                                        LPSTR lpszShortName, int nLines,
                                        int nUsedPixels)
{
    SYM_ASSERT (hWnd);
    SYM_ASSERT (::IsWindow (hWnd));
    SYM_ASSERT (lpszName);
    SYM_ASSERT (lpszShortName);
    SYM_ASSERT (nLines      >  0);
    SYM_ASSERT (nUsedPixels >= 0);
    SYM_VERIFY_STRING (lpszName);

    const  HANDLE   hFont    = GetWindowFont (hWnd);
    const  HDC      hDC      = GetDC (hWnd);
    const  WORD     wNameLen = STRLEN (lpszName);

    auto   HANDLE   hOldFont = NULL;
    auto   long     lWindowWidth;
    auto   WORD     wMaxWidth;
    auto   WORD     wCurCharLen;
    auto   RECT     rect;
    auto   SIZE     sExtents;
    auto   BOOL     bSuccess;
    auto   BOOL     bFits;
    auto   BOOL     bRuffFit;

                                // Make sure the right font is selected.
    if (hFont)
        hOldFont = SelectObject (hDC, hFont);

    GetWindowRect (hWnd, &rect);

    lWindowWidth = (rect.right - rect.left);
    wMaxWidth    = (lWindowWidth * nLines) - nUsedPixels;

                                    // Determine space consumed by lpszName.
    bSuccess = GetTextExtentPoint (hDC, lpszName, wNameLen, &sExtents);

                                    // If func call failed or the text length
                                    // for all of lpszName is <= the allowed
                                    // maximum, no shortening necessary.
    if (!bSuccess || sExtents.cx <= wMaxWidth)
    {
        STRCPY (lpszShortName, lpszName);
        goto TheEnd;
    }

    // This is a brute force alogithm because it only steps down a character
    // with each loop. A more efficient alogithm would be to divide the string
    // by 2 with each loop.
    bRuffFit = FALSE;
    bFits    = FALSE;
    for (wCurCharLen = wNameLen; wCurCharLen > 7 && !bFits; --wCurCharLen)
    {
        NameShortenFileName (lpszName, lpszShortName, wCurCharLen);

        if (!bRuffFit)
        {
                                        // Determine space consumed.
            bSuccess = GetTextExtentPoint (hDC, lpszShortName,
                                           STRLEN (lpszShortName), &sExtents);
        
            bRuffFit = (sExtents.cx < wMaxWidth);
        }

                                        // If filename ruffly fits, ensure that
                                        // there are no dangling characters by
                                        // exactly calculating where a CRLF
                                        // will be.
        if (bRuffFit)
        {
            auto   LPCSTR lpRemainingChars             = lpszShortName;
            auto   char   lpDummyBuffer[SYM_MAX_PATH+1];
            auto   int    nCurLine;

            for (nCurLine = 0; nCurLine < nLines && *lpRemainingChars; ++nCurLine)
            {
                lpRemainingChars = CopyCharsThatFit (hDC, lpRemainingChars,
                                                     lpDummyBuffer,
                                                     lWindowWidth);
            }
            bFits = (*lpRemainingChars == EOS);
        }
    }

                                    // If impossible to make it fit, return
                                    // lpszName.
    if (!bFits)
        STRCPY (lpszShortName, lpszName);

TheEnd:
    if (hFont)
        SelectObject (hDC, hOldFont);
    ReleaseDC (hWnd, hDC);
}

//****************************************************************************
// NavNameFormat()
//
// Description:
//   Walk through lpFormatStr and add CRLF when the control is going to wrap
//   the text. This routine does not CRLF in the middle of words contained in
//   lpFormatStr. When a %s appears in lpFormatStr, this routine replaces the
//   %s with lpFileName. Note that when putting lpFileName into lpOutputStr,
//   CRLFs are added in the middle of lpFileName. This causes the file name
//   to wrap in the middle of the string.
//
//   Note, this function only add CRLFs to the text that occurs before the %s
//   string. All text after the %s is copied unchanged.
//
//   This function is necessary because windows clips strings that are longer
//   than one line in the control.
//
// Parameters:
//   hWnd           [in]   Control that determines the font and width info.
//   lpszFormat     [in]   Format string with the "%s" for lpszFileName.
//   lpszFileName   [in]   File name to replace the first "%s" in lpszFormat.
//   lpszOutput     [out]  Completely formated string, with CRLFs added.
//
//****************************************************************************
// 10/30/1996 Jim Brennan - created.
// 11/08/1996 Jim Brennan - Move to NAVXUTIL project and rename to
//            NavNameFormat().
// 12/05/1996 Jim Brennan - Add support for "\n" characters causing returns
//            before the first "%s". Previously, a "\n" would cause
//            misformating of the string.
//****************************************************************************
void SYM_EXPORT NavNameFormat (HWND   hWnd,         LPCSTR lpszFormat,
                               LPCSTR lpszFileName, LPSTR  lpszOutput)
{
    SYM_ASSERT (hWnd);
    SYM_ASSERT (::IsWindow (hWnd));
    SYM_ASSERT (lpszFormat);
    SYM_ASSERT (lpszFileName);
    SYM_ASSERT (lpszOutput);
    SYM_VERIFY_STRING (lpszFormat);
    SYM_VERIFY_STRING (lpszFileName);

    const HANDLE   hFont    = GetWindowFont (hWnd);
    const HDC      hDC      = GetDC (hWnd);
    auto  HANDLE   hOldFont = NULL;

    auto   LPCSTR  lpWordDelimitors   = " \t\r\n";
    auto   LPCSTR  lpCurFormat        = lpszFormat;
    auto   LPCSTR  lpCurFileName      = lpszFileName;
    auto   LPSTR   lpCurOutput        = lpszOutput;
    auto   BOOL    bInFileName        = FALSE;
    auto   BOOL    bFileNameProcessed = FALSE;
    auto   BOOL    bNewLine           = FALSE;
    auto   long    lWindowWidth;
    auto   long    lRemainingWidth;
    auto   long    lSpaceWidth;
    auto   RECT    rect;
    auto   BOOL    bSuccess;
    auto   SIZE    sExtents;

                                // Make sure the right font is selected.
    if (hFont)
        hOldFont = SelectObject (hDC, hFont);

    GetWindowRect (hWnd, &rect);
    lWindowWidth = rect.right - rect.left;

                                // Get the width of a space character.
    bSuccess = GetTextExtentPoint (hDC, " ", 1, &sExtents);
    if (!bSuccess)
        lSpaceWidth = 0;
    else
        lSpaceWidth = sExtents.cx + GetTextCharacterExtra (hDC);

    lRemainingWidth = lWindowWidth;
    while (bInFileName || (*lpCurFormat && !bFileNameProcessed))
    {
        auto   WORD  wWordLen              = 0;
        auto   char  szWord[SYM_MAX_PATH+1];
        auto   DWORD dwOrigRemainingWidth;

        //********************************************************************
        // Setup szWord and wWordLen.

                                        // If we are processing the file name
                                        // and there are still characters left
                                        // over from the previous line,
                                        // continue to process the remaining
                                        // filename characters.
        if (bInFileName && *lpCurFileName)
        {
            STRCPY (szWord, lpCurFileName);
            wWordLen = STRLEN (szWord);
        }

        else
        {
            auto   LPSTR lpWordEnd;

                                        // Reset bInFileName.
            if (bFileNameProcessed && bInFileName)
                bInFileName = FALSE;

                                        // Copy word delimitors.
            while (!DBCSIsLeadByte (lpCurFormat[0]) &&
                   NULL != STRCHR (lpWordDelimitors, lpCurFormat[0]))
            {
                if (lpCurFormat[0] == ' ')
                    lRemainingWidth -= lSpaceWidth;

                                        // If one of the word delimintors will
                                        // cause a newline, remember that so
                                        // that we can reset our lRemainingWidth
                                        // variable.
                if (lpCurFormat[0] == '\r' || lpCurFormat[0] == '\n')
                    bNewLine = TRUE;

                *lpCurOutput++ = *lpCurFormat++;
            }

                                        // If string begins w/ "%s", use only
                                        // the "%s" as the word. Covers the case
                                        // when %s does not have white space
                                        // around it
            if (!DBCSIsLeadByte(lpCurFormat[0]) && lpCurFormat[0] == '%' &&
                !DBCSIsLeadByte(lpCurFormat[1]) && lpCurFormat[1] == 's')
            {
                STRCPY (szWord, "%s");
                wWordLen = 2;
            }
            else
            {
                                        // Put a word into szWord and set
                                        // wWordLen.
                lpWordEnd = STRPBRK (lpCurFormat, lpWordDelimitors);
                if (lpWordEnd)
                {
                    wWordLen = lpWordEnd - lpCurFormat;
                    STRNCPY (szWord, lpCurFormat, wWordLen);
                    szWord[wWordLen] = EOS;
                }
                else
                {
                    STRCPY (szWord, lpCurFormat);
                    wWordLen = STRLEN (szWord);
                }

                                        // If there is a %s in the middle of
                                        // the word, do not include %s in the word.
                lpWordEnd = STRSTR (szWord, "%s");
                if (lpWordEnd != NULL)
                {
                    *lpWordEnd = EOS;
                    wWordLen   = STRLEN (szWord);
                }
            }
            lpCurFormat += wWordLen;

                                        // process %s if haven't already.
            if (!bFileNameProcessed && 0 == STRCMP (szWord, "%s"))
            {
                STRCPY (szWord, lpCurFileName);
                wWordLen           = STRLEN (szWord);
                bInFileName        = TRUE;
                bFileNameProcessed = TRUE;
            }
        }

                                        // If we are beginning a new line with
                                        // this word, reset our width counter.
        if (bNewLine)
            lRemainingWidth = lWindowWidth;

        //********************************************************************
        // Process szWord adding CFLF when necessary.

                                        // Determine the space consumed by szWord.
        bSuccess = GetTextExtentPoint (hDC, szWord, STRLEN (szWord), &sExtents);
        if (!bSuccess)
            MEMCPY (&sExtents, 0, sizeof (sExtents));
        else
            sExtents.cx += GetTextCharacterExtra (hDC);

        dwOrigRemainingWidth  = lRemainingWidth;
        lRemainingWidth      -= sExtents.cx;

                                        // If the word fits on this line, ...
        if (lRemainingWidth >= 0)
        {
            STRCPY (lpCurOutput, szWord);
            lpCurOutput += wWordLen;

            if (bInFileName)
                bInFileName = FALSE;
        }
        else                            // Else,word does not fit on this line
        {
                                        // Flag that we are beginning a new line.        
            bNewLine = TRUE;

                                        // If we are processing the %s, output
                                        // as many of szWord's characters as
                                        // will fit on the remaining line.
            if (bInFileName)
            {
                lpCurFileName = CopyCharsThatFit (hDC, szWord, lpCurOutput,
                                                  dwOrigRemainingWidth);
                lpCurOutput += STRLEN (lpCurOutput);
                *lpCurOutput++ = '\r';
                *lpCurOutput++ = '\n';

                if (*lpCurFileName == EOS)
                    bInFileName = FALSE;
            }
            else
            {
                // Because we are not processing lpCurFileName, output a CRLF
                // before the word. But only add the CRLF if this is not the
                // first word of the string.

                if (lpszOutput != lpCurOutput)
                {
                    *lpCurOutput++ = '\r';
                    *lpCurOutput++ = '\n';
                }

                STRCPY (lpCurOutput, szWord);
                lpCurOutput += wWordLen;
            }
        }
    }

                                    // Copy the remainder of the format string
    while (*lpCurFormat)
        *lpCurOutput++ = *lpCurFormat++;
    *lpCurOutput = EOS;

    if (hFont)
        SelectObject (hDC, hOldFont);
    ReleaseDC (hWnd, hDC);
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    NavNameIsReservedName()
//
// Description:
//  Returns a BOOL indicating whether the base name portion of the passed-
//  in filename is a DOS reserved device name (eg, "aux").
//
// Input:
//  szPath: The filename to check.  This can be just the filename or the
//          fully-qualified path - the behavior of this fn is the same.
//
// Returns:
//  TRUE if the filename is a reserved name, FALSE if not.
//
// See also:
//  Static array g_aszReservedNames[] at the top of this file.
//
// Notes:
//  * This fn does not need to support DBCS-enabled string comparing,
//    since the device names are all SBCS strings.
//
//  * Code based on IsFileNameReserved() from the AP.
//
//////////////////////////////////////////////////////////////////////////
// 7/14/97 Mike Dunn -- function created 
//////////////////////////////////////////////////////////////////////////

BOOL SYM_EXPORT NavNameIsReservedName ( LPCTSTR szPath )
{
TCHAR   szFilename [ SYM_MAX_PATH ];    // holds just the file name part
UINT    uCurrString;                    // index into the name table
LPCTSTR szCurrReservedName;             // the name currently being compared against
LPCTSTR szCurrChar;                     // the character in the passed-in
                                        // filename currently being compared
BOOL    bRetVal = FALSE;

                                        // Validate input parameters
    SYM_ASSERT ( szPath != NULL );
    SYM_ASSERT ( '0' < '9' );

                                        // Grab the filename portion from
                                        // the passed-in path.
    if ( 0 != NameReturnFile ( szPath, szFilename ) )
        {
                                        // Error getting the filename.
                                        // Just bail out and let it be
                                        // scanned.
        return FALSE;
        }

                                        // We only care about the base name,
                                        // so get rid of the extension.
    NameStripExtension ( szFilename );

                                        // Begin testing for a reserved name

    uCurrString = 0;

    while ( !bRetVal  &&
            NULL != ( szCurrReservedName = g_aszReservedNames [uCurrString++] ))
        {
        szCurrChar = szFilename;

                                        // Compare chars, ignoring case,
                                        // until we hit the end of one/both
                                        // strings or find mismatching chars.

        while ( *szCurrChar != '\0'  &&  *szCurrReservedName != '\0' )
            {
                                        // First handle a wildcard.  '?'
                                        // must match up with a number.
            if ( *szCurrReservedName == '?' )
                {
                if ( !_istdigit ( *szCurrChar ) )
                    {
                    break;              // mismatch!
                    }
                }
                                        // otherwise just compare chars normally.
            else if ( *szCurrReservedName != _totupper ( *szCurrChar ) )
                {
                break;                  // mismatch!
                }

                                        // on to the next character...
                                        // Note that this doesn't have to
                                        // be DBCS'ed since reserved names
                                        // are single-byte strings.
            szCurrChar++;
            szCurrReservedName++;
            }   // end while

                                        // If we're at the end of both
                                        // strings, the names matched.
        if ( *szCurrReservedName == '\0'  &&  *szCurrChar == '\0' )
            {
            bRetVal = TRUE;
            }
        }   // end while

    return bRetVal;
}

