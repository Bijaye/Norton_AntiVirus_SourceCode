//************************************************************************
// Copyright 1992-1995 Symantec, Peter Norton Product Group
//************************************************************************
//                                                                
// $Header:   S:/NAVXUTIL/VCS/navutl.cpv   1.1   05 Jan 1998 17:56:06   MDUNN  $
//                                                                
// Description:                                                   
//      This file contains the utility routines required by the Norton
// AntiVirus for Windows and all of its Dynamic Link Libraries.
//                                                                
// Contains (as exports): 
//************************************************************************
// $Log:   S:/NAVXUTIL/VCS/navutl.cpv  $
// 
//    Rev 1.1   05 Jan 1998 17:56:06   MDUNN
// Added GetLastAccessDateNT() as part of the fix to preserve the last-accessed
// date when scanning w/ NAVW on NT.
// 
//    Rev 1.0   06 Feb 1997 21:05:04   RFULLER
// Initial revision
// 
//    Rev 1.6   10 Oct 1996 14:43:00   JBRENNA
// 1. DBCS enable NavAnsiToOem() and NavOemToAnsi().
// 2. Create NavAnsiToOemUpper(). This function is used instead of
//    NAnsiToOem(). The difference is that NavAnsiToOemUpper() upper cases
//    all non-DBCS characters. Where as NAnsiToOem() upper cases all
//    characters.
// 
//    Rev 1.5   09 Oct 1996 15:16:18   JBRENNA
// Significant changes to NavStrUpr() NavOemStrUdr() so that DBCS characters
// do not get upper cased. This can cause problems when trying to correctly
// identify a DBCS file. For DBCS, a file named "a.com" is different than
// a file named "A.com" where "a" and "A" are DBCS characters.
// 
//    Rev 1.4   23 Aug 1996 12:22:38   DALLEE
// Moved NAVUTIL.H outside SYM_WIN only section.
// DX was getting name mangled functions and failing to link.
// 
//    Rev 1.3   24 Jun 1996 19:54:30   RHAYES
// OEM<->Ansi functions ported from Luigi.
// 
//    Rev 1.2   26 May 1996 13:04:00   PVENKAT
// 
//    Rev 1.1   04 Apr 1996 17:23:14   YVANG
// Ported from Kirin.
// 
//    Rev 1.1   03 Apr 1996 15:25:32   YVANG
// DBCS enable.
// 
//    Rev 1.0   30 Jan 1996 15:56:16   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:51:16   BARRY
// Initial revision.
// 
//    Rev 1.3   09 Mar 1995 15:49:38   DALLEE
// Removed "windowsx.h" - we pick it up from platform.h with <> brackets.
// Since this comes right from the SDK, we don't want to pick it up
// as a dependency w/ MKDEPS.
//************************************************************************


#include "platform.h"                   // Platform independence
#include "navutil.h"                    // Prototypes
#include "dbcs.h"
#include "xapi.h"

#ifdef SYM_WIN
#include "ngui.h"                       // Norton Look/Feel


//************************************************************************
// Ye actual code
//************************************************************************

//************************************************************************
// NavLoadString()
//
// This routine calls the windows function LoadString() but looks for all 
// occurances of the character '|' and replaces it with EOS.  This builds
// lists strings and guarentees contiguous memory for the multistring
// buffer.
//
// I.E. If the string "Hello|There|How|Are|You|" is loaded from the
// string table, the value   Hello\0There\0How\0Are\0You\0\0  is returned
// to the caller.
//
// Parameters:
//      Same paramters as the Windows API LoadString()
//
// Returns: 
//      Same return value as from the Windows API LoadString()
//************************************************************************
// 12/28/92 MSL Function created.
//************************************************************************
int SYM_EXPORT WINAPI NavLoadString (       // Calls LoadString and parses '|'
    HINSTANCE   hInst,                  // Instance to load resource from
    UINT        idResource,             // ID of resource to load
    LPSTR       lpszBuffer,             // Buffer to copy loaded/parsed string
    int         cbBuffer)               // Size of buffer
{
#define EOSSTRINGDELIM  '|'
    LPSTR       lpszTemp;               // Pointer into lpszBuffer;
    int         i, iRet;                   // LoadString() return code

    if ((iRet = LoadString (hInst, idResource, lpszBuffer, cbBuffer)) != 0)
        {
        for (lpszTemp = lpszBuffer, i = 0;
#ifdef  TEST
             *lpszTemp != EOS;
#else
            i < iRet;
#endif
             i++)
             {
             if (*lpszTemp == EOSSTRINGDELIM)
                 {
                 *lpszTemp = EOS;
                 }
			 lpszTemp = AnsiNext(lpszTemp);
			 if ( *lpszTemp == EOS)
				lpszTemp++;
             }
        }

                                        // Return to caller.
    return (iRet);
} // NavLoadString()



//************************************************************************
// NavComboBoxLoad()
//
// This routine initializes a combo box by loading the appropriate values
// from the string table and setting the current selection.
//
// NOTE:  ComboBox entries are in the string table with the '|' character
// delimetering entries.  This is then converted to EOS delimeters.
//
// Parameters:
//      hWnd                    [in] Handle of combo box window
//      hInstRes                [in] Handle to resources
//      wStringID               [in] ID to string table entry containing 
//                              combobox entries
//      wIndexSelect            [in] Index to entry to select.
//
// RETURNS:
//      nothing
//************************************************************************
// 01/05/93 MSL Function Created
//************************************************************************
VOID SYM_EXPORT WINAPI NavComboBoxLoad (    // Initializes combo boxes
    HWND        hWnd,                   // [in] Handle combo box
    HINSTANCE   hInstRes,               // [in] Resource Instance handle
    WORD        wStringID,              // [in] Identifies combobox
    WORD        wIndexSelect)           // [in] Default selection
{
    BYTE        szzComboList[256];
    LPSTR       lpszzCurEntry;

    if (NavLoadString (hInstRes, 
                       wStringID, 
                       (LPSTR)szzComboList, 
                       sizeof (szzComboList)))
       {
                                        // Load up the combo list with
                                        // the valid entries.
       lpszzCurEntry = (LPSTR)szzComboList;
       do  {
           ComboBox_AddString (hWnd, lpszzCurEntry);
           while (*(lpszzCurEntry = AnsiNext (lpszzCurEntry)) != EOS)
               {
               ;
               }
           lpszzCurEntry = AnsiNext(lpszzCurEntry);
           } while (*lpszzCurEntry != EOS);

                                        // Set the current selection to the
                                        // one specified.
        ComboBox_SetCurSel (hWnd, wIndexSelect);
       }
} // NavComboBoxLoad()


//************************************************************************
// NavStrUpr()
//
// This routine performs an upper case conversion on the supplied string.
// If the string cannot be converted reliably back and forth between
// upper and lower case we will progress one character at a time and
// convert whatever we can. We can only perform this function on ANSI strings.
//
// Note, DBCS characters are not upper cased.
//
// Parameters:
//      lpszString     String to be converted (ANSI)
//
// Returns: 
//      Pointer to the converted string
//************************************************************************
// 05/30/96 Jim Worden - Function created
// 10/09/96 Jim Brennan - Changed behavior so that DBCS characters do not
//          get upper cased.
//************************************************************************
LPTSTR SYM_EXPORT WINAPI NavStrUpr
(
    LPSTR       lpszString             // [in] String to be converted (ANSI)
)
{
    SYM_ASSERT (lpszString);

    auto   BOOL    bProcessed      = FALSE;
    auto   char    szUpperString[SYM_MAX_PATH * 2];

                                        // -----------------------------------
                                        // JBRENNA - Only attempt a easy 
                                        // STRUPR if we are not running on a 
                                        // DBCS machine.
                                        // -----------------------------------
    if (!DBCSInstalled ())
    {
                                        // -----------------------------------
                                        // First try converting the easy way
                                        // -----------------------------------
        STRCPY (szUpperString, lpszString);
        STRUPR (szUpperString);
                                        // -----------------------------------
                                        // If the string already happens to be
                                        // in uppercase we can quit early.
                                        // -----------------------------------
        if (!STRCMP (szUpperString, lpszString))
            bProcessed = TRUE;
    }

                                        //------------------------------------
                                        // If we couldn't take the short cut
                                        // above, process the string manually.
                                        //------------------------------------
    if (!bProcessed)
    {
        auto    LPSTR   lpszCurStr;
        auto    LPSTR   lpszCurUpperStr;

        lpszCurStr      = lpszString;
        lpszCurUpperStr = szUpperString;

                                        // Loop thru the entire input string.
        for (; *lpszCurStr; lpszCurStr = AnsiNext (lpszCurStr))
        {
                                        // -----------------------------------
                                        // If DBCS is installed and this is a 
                                        // DBCS lead byte, simply copy the two
                                        // byte character to szUpperString
                                        // without upper casing it.
                                        // -----------------------------------
            if (DBCSInstalled () && DBCSIsLeadByte (*lpszCurStr))
            {
                *lpszCurUpperStr = lpszCurStr[0];
                ++lpszCurUpperStr;
            
                *lpszCurUpperStr = lpszCurStr[1];
                ++lpszCurUpperStr;
            }
            else
            {
                auto    char    szOneChar[4];
                auto    char    szOneCharUpper[4];
                auto    short   nOneCharLen;

                                        // -----------------------------------
                                        // Isolate one character
                                        // -----------------------------------
                szOneChar[0] = *lpszCurStr;
                szOneChar[1] = EOS;
                nOneCharLen  = STRLEN (szOneChar);

                                        // -----------------------------------
                                        // Convert it to uppercase then back to
                                        // lowercase. If it matches the original
                                        // character it's okay to be converted.
                                        // If not, it's because the original is
                                        // either already in uppercase or it
                                        // won't reliably convert. Either way
                                        // we have to use it as is.
                                        // -----------------------------------
                STRCPY (szOneCharUpper, szOneChar);
                STRUPR (szOneCharUpper);
                STRLWR (szOneCharUpper);
                if (!MEMCMP (szOneCharUpper, lpszCurStr, nOneCharLen))
                    STRUPR (szOneChar);

                STRCPY (lpszCurUpperStr, szOneChar);
                lpszCurUpperStr += nOneCharLen;
            }
        }
                                        // NULL terminate the szUpperString.
        *lpszCurUpperStr = EOS;
                                        // -----------------------------------
                                        // Put the converted string back into
                                        // the original.
                                        // -----------------------------------
        STRCPY (lpszString, szUpperString);
    }

    return (lpszString);

} // NavStrUpr()


//************************************************************************
// NavOemStrUpr()
//
// This routine performs an upper case conversion on the supplied OEM string.
// Since "uppercase" really is only defined in terms of the Ansi character
// set we must perform this as an Ansi operation. The catch is that some
// characters that are perfectly valid file name characters will not map
// to Ansi. That's a problem. We must convert everything that is mappable
// to Ansi and uppercase it. Everything must be preserved as is.
//
// Note the intentional use of the Windows functions.
//
// Note, DBCS characters are not uppercased.
//
// Parameters:
//      lpszString     String to be converted (OEM)
//
// Returns: 
//      Pointer to the converted string
//************************************************************************
// 05/31/96 Jim Worden - Function created
// 10/08/96 Jim Brennan - Change function so that DBCS characters are not
//          upper cased. ... This is one ugly routine.
//************************************************************************
LPTSTR SYM_EXPORT WINAPI NavOemStrUpr
(
    LPSTR       lpszString             // [in] String to be converted (OEM)
)
{
    SYM_ASSERT (lpszString);

    auto   char   szAnsiString[SYM_MAX_PATH * 2];
    auto   char   szMaskString[SYM_MAX_PATH * 2];

                                        // -----------------------------------
                                        // If the string will convert safely
                                        // to Ansi we can use the standard
                                        // functions to simply uppercase the
                                        // Ansi and then convert it back to Oem
                                        // -----------------------------------
                                        // JBRENNA - Only take the short cut if
                                        // DBCS is not installed.
                                        // -----------------------------------
    if (!NavTestOemToAnsi (lpszString) && !DBCSInstalled())
    {
        NavOemToAnsi (lpszString, lpszString);
        AnsiUpper (lpszString);
        NavAnsiToOem (lpszString, lpszString);
    }
    else
    {
        auto    BOOL   bMaskHasData = FALSE;
        auto    WORD   wChar;

                                        // -----------------------------------
                                        // Here's where it begins to get
                                        // interesting. Since uppercase is only
                                        // valid for the Ansi codepage we must
                                        // do it in Ansi mode. Any character
                                        // that can't be converted must be left
                                        // as is. To do that we need to build
                                        // a mask of non-convertible characters
                                        // and convert everything else
                                        // -----------------------------------

        MEMSET (szAnsiString, 0, sizeof (szAnsiString));
        MEMSET (szMaskString, 0, sizeof (szMaskString));
        for (wChar = 0; lpszString[wChar] != EOS; wChar++)
        {
                                        // -----------------------------------
                                        // If DBCS is installed and this is a 
                                        // DBCS lead byte, simply copy the two
                                        // byte character to szAnsiString
                                        // without upper casing it.
                                        // -----------------------------------
            if (DBCSInstalled () && DBCSIsLeadByte (lpszString[wChar]))
            {
                szAnsiString[wChar] = lpszString[wChar];
                ++wChar;
                szAnsiString[wChar] = lpszString[wChar];
            }\
            else
            {
                char szOneChar[4];
                                        // -----------------------------------
                                        // Isolate one character
                                        // -----------------------------------
                szOneChar[0] = lpszString[wChar];
                szOneChar[1] = EOS;

                                        // -----------------------------------
                                        // Convert the char to uppercase Ansi.
                                        // If it won't convert save it in the
                                        // mask for later.
                                        // -----------------------------------
                                        // JBRENNA - How does this convert to
                                        // uppercase ansi? Shoot, is szOneChar
                                        // Oem or Ansi? It is supposed to be OEM,
                                        // but then why does JimW call AnsiToOem
                                        // instead of OemToAnsi()? Too scary to
                                        // touch.
                                        // -----------------------------------
                if (!NavTestOemToAnsi (szOneChar))
                    AnsiToOem (szOneChar, szOneChar);
                else
                {
                    szMaskString[wChar] = szOneChar[0];
                    bMaskHasData = TRUE;
                }
             
                szAnsiString[wChar] = szOneChar[0];
            }
        }

                                        // -----------------------------------
                                        // Now we have a string where every
                                        // char that will convert to Ansi is
                                        // in Ansi upper case and a mask where
                                        // everything else is saved. To
                                        // convert this mess back to Oem we
                                        // first convert the entire Ansi string
                                        // to Oem. The non-mapping chars will
                                        // get screwed up here but we'll fix
                                        // that by moving them from the mask
                                        // back into their original positions
                                        // -----------------------------------
        OemToAnsi (szAnsiString, lpszString);

                                        // Only need to process the mask if it  
                                        // has data.
        if (bMaskHasData)
            for (wChar = 0; szAnsiString[wChar] != 0; wChar ++)
            {
                if (szMaskString[wChar] != 0)
                    lpszString[wChar] = szMaskString[wChar];
            }
    }

    return (lpszString);

} // NavOemStrUpr()

#endif // #ifdef SYM_WIN

#ifdef SYM_WIN


//************************************************************************
// NavTestOemToAnsi ()
//
// This routine performs test conversion of the string passed to determine
// if it contains characters that cannot be translated from Oem to Ansi
// and back reliably.
//
//
// Parameters:
//      lpszString     String to be tested (OEM)
//
// Returns: 
//      FALSE - Can be converted
//      TRUE  - Contains unmappable characters
//
//************************************************************************
// 05/31/96 Jim Worden - Function created
//************************************************************************
BOOL SYM_EXPORT WINAPI NavTestOemToAnsi
(
    LPSTR       lpszString             // [in] String to be tested (OEM)
)
{
    BOOL bReturn = FALSE;

    LPSTR lpszOemString,
          lpszAnsiString;

                                        // Allocate enough memory to make the
                                        // conversion. Allow for DBCS size
                                        // increase
    lpszOemString = (LPSTR)MemAllocPtr (GHND, (STRLEN (lpszString) * 2) + 1);
    lpszAnsiString = (LPSTR)MemAllocPtr (GHND, (STRLEN (lpszString) * 2) + 1);

    if (lpszAnsiString && lpszOemString)
    {
        STRCPY (lpszOemString, lpszString);

                                        // Switch to Ansi and back
        OemToAnsi (lpszOemString, lpszAnsiString);
        AnsiToOem (lpszAnsiString, lpszAnsiString);

                                        // Is it still the same string?
        if (STRCMP (lpszOemString, lpszAnsiString))
            bReturn = TRUE;
    }

    if (lpszOemString)
        MemFreePtr (lpszOemString);

    if (lpszAnsiString)
        MemFreePtr (lpszAnsiString);

    return (bReturn);
}

#else


//************************************************************************
// NavTestOemToAnsi ()
//
// This routine does nothing
// But it does it very well.
//
// Parameters:
//      lpszString
//
// Returns: 
//      FALSE
//
//************************************************************************
// 06/05/96 Jim Worden - Function created
//************************************************************************
BOOL SYM_EXPORT WINAPI NavTestOemToAnsi
(
    LPSTR       lpszString
)
{
    return (FALSE);
}

#endif

#ifdef SYM_WIN

//************************************************************************
// NavTestAnsiToOem ()
//
// This routine performs test conversion of the string passed to determine
// if it contains characters that cannot be translated from Ansi to Oem
// and back reliably.
//
//
// Parameters:
//      lpszString     ANSI string to be tested
//
// Returns: 
//      FALSE - Can be converted
//      TRUE  - Contains unmappable characters
//
//************************************************************************
// 05/31/96 Jim Worden - Function created
//************************************************************************
BOOL SYM_EXPORT WINAPI NavTestAnsiToOem
(
    LPSTR       lpszString             // [in] ANSI string to be tested
)
{
    BOOL bReturn = FALSE;

    LPSTR lpszOemString,
          lpszAnsiString;

                                        // Allocate enough memory to make the
                                        // increase   Allow for DBCS size
                                        // conversion.
    lpszOemString = (LPSTR)MemAllocPtr (GHND, (STRLEN (lpszString) * 2) + 1);
    lpszAnsiString = (LPSTR)MemAllocPtr (GHND, (STRLEN (lpszString) * 2) + 1);

    if (lpszAnsiString && lpszOemString)
    {
        STRCPY (lpszAnsiString, lpszString);

                                        // Switch to Oem and back
        AnsiToOem (lpszAnsiString, lpszOemString);
        OemToAnsi (lpszOemString, lpszOemString);

                                        // Is it still the same string?
        if (STRCMP (lpszOemString, lpszAnsiString))
            bReturn = TRUE;
    }

    if (lpszOemString)
        MemFreePtr (lpszOemString);

    if (lpszAnsiString)
        MemFreePtr (lpszAnsiString);

    return (bReturn);
}

#else

//************************************************************************
// NavTestAnsiToOem ()
//
// This routine does nothing
//
// Parameters:
//      lpszString
//
// Returns: 
//      FALSE
//
//************************************************************************
// 06/05/96 Jim Worden - Function created
//************************************************************************
BOOL SYM_EXPORT WINAPI NavTestAnsiToOem
(
    LPSTR       lpszString             // [in] ANSI string to be tested
)
{
    return (FALSE);
}
#endif



#ifdef SYM_WIN

//************************************************************************
// NavOemToAnsi ()
//
// This routine converts Oem strings to Ansi. Since not all characters
// can be reliably converted it first performs a verification test.
// If it can be reliably translated via the standard fuctions, it is.
// If not, it is translated one character at a time. Non-translatable
// characters are not converted
//
// Parameters:
//      lpszOemString     [in]  String to be converted (OEM)
//      lpszAnsiString    [out] Converted string (ANSI)
//
// Returns: 
//      Pointer to the converted string
//
//************************************************************************
// 06/03/96 Jim Worden - Function created
//************************************************************************
LPSTR SYM_EXPORT WINAPI NavOemToAnsi
(
    LPSTR lpszOemString,                // [in]  String to be converted (OEM)
    LPSTR lpszAnsiString                // [out] Converted string (ANSI)
)
{
    char szOemChar[3],
         szOemChar2[3],
         szAnsiChar[3];

    LPSTR lpszAnsi;
                                        // See if we can do this the easy way
                                        // Note that we're not using the quake
                                        // functions here on purpose to avoid
                                        // the automatic uppercase

    if (!NavTestOemToAnsi (lpszOemString))
        OemToAnsi (lpszOemString, lpszAnsiString);
    else
    {
                                        // Allocate enough memory to make the
                                        // conversion. Allow for DBCS size
                                        // increase
        lpszAnsi = (LPSTR)MemAllocPtr (GHND, (STRLEN (lpszOemString) * 2) + 1);

        while (*lpszOemString)
        {
            MEMSET (szOemChar, 0, sizeof (szOemChar));
            szOemChar[0] = *lpszOemString;

                                        // If this is a DBCS char, put it on
                                        // szOemChar.
            if (DBCSInstalled () && DBCSIsLeadByte (*lpszOemString))
                szOemChar[1] = lpszOemString[1];

                                        // Switch to Ansi and back
            OemToAnsi (szOemChar, szAnsiChar);
            AnsiToOem (szAnsiChar, szOemChar2);

                                        // Is it still the same string?
            if (!STRCMP (szOemChar, szOemChar2))
                STRCAT (lpszAnsi, szAnsiChar);
            else
                STRCAT (lpszAnsi, szOemChar);

                                        // Note that this is not an Ansi
                                        // string therefore AnsiNext is not
                                        // appropriate here. Instead, we do
                                        // DBCS handling based on STRLEN.
            lpszOemString += STRLEN (szOemChar);
        }

        STRCPY (lpszAnsiString, lpszAnsi);
        MemFreePtr (lpszAnsi);
    }

    return (lpszAnsiString);
}

#else

//************************************************************************
// NavOemToAnsi ()
//
// This routine does no processing other than to copy the source
// string to the destination
//
// Parameters:
//      lpszOemString     [in]  String to be converted (OEM)
//      lpszAnsiString    [out] Converted string (ANSI)
//
// Returns: 
//      Pointer to the converted string
//
//************************************************************************
// 06/03/96 Jim Worden - Function created
//************************************************************************
LPSTR SYM_EXPORT WINAPI NavOemToAnsi
(
    LPSTR lpszOemString,                // [in]  String to be converted (OEM)
    LPSTR lpszAnsiString                // [out] Converted string (ANSI)
)
{
    if (lpszOemString != lpszAnsiString)
        STRCPY (lpszAnsiString, lpszOemString);

    return (lpszAnsiString);
}


#endif



#ifdef SYM_WIN

//************************************************************************
// NavAnsiToOem ()
//
// This routine converts Ansi strings to Oem. Since not all characters
// can be reliably converted it first performs a verification test.
// If it can be reliably translated via the standard fuctions, it is.
// If not, it is translated one character at a time. Non-translatable
// characters are not converted
//
// Parameters:
//      lpszAnsiString,                 // [in]  String to be converted (ANSI)
//      lpszOemString                   // [out] Converted string (OEM)
//
// Returns: 
//      Pointer to the converted string
//
//************************************************************************
// 05/31/96 Jim Worden - Function created
// 10/09/96 Jim Brennan - DBCS enabled.
//************************************************************************
LPSTR SYM_EXPORT WINAPI NavAnsiToOem
(
    LPSTR lpszAnsiString,               // [in]  String to be converted (ANSI)
    LPSTR lpszOemString                 // [out] Converted string (OEM)
)
{
    char szAnsiChar[3],
         szAnsiChar2[3],
         szOemChar[3];

    LPSTR lpszOem;

                                        // See if we can do this the easy way
                                        // Note that we're not using the quake
                                        // functions here on purpose to avoid
                                        // the automatic uppercase

    if (!NavTestAnsiToOem (lpszAnsiString))
        AnsiToOem (lpszAnsiString, lpszOemString);
    else
    {
                                        // Allocate enough memory to make the
                                        // conversion. Allow for DBCS size
                                        // increase
        lpszOem = (LPSTR)MemAllocPtr (GHND, (STRLEN (lpszAnsiString) * 2) + 1);

        while (*lpszAnsiString)
        {
            MEMSET (szAnsiChar, 0, sizeof (szAnsiChar));
            szAnsiChar[0] = *lpszAnsiString;

                                        // If this is a DBCS char, put it on
                                        // szAnsiChar.
            if (DBCSInstalled () && DBCSIsLeadByte (*lpszAnsiString))
                szAnsiChar[1] = lpszAnsiString[1];

                                        // Switch to Oem and back
            AnsiToOem (szAnsiChar, szOemChar);
            OemToAnsi (szOemChar, szAnsiChar2);

                                        // Is it still the same string?
            if (!STRCMP (szAnsiChar, szAnsiChar2))
                STRCAT (lpszOem, szOemChar);
            else
                STRCAT (lpszOem, szAnsiChar);

            lpszAnsiString = AnsiNext (lpszAnsiString);
        }

        STRCPY (lpszOemString, lpszOem);
        MemFreePtr (lpszOem);
    }

    return (lpszOemString);
}

#else

//************************************************************************
// NavAnsiToOem ()
//
// This routine does no processing other than to copy the source
// string to the destination
//
// Parameters:
//      lpszAnsiString,                 // [in]  String to be converted (ANSI)
//      lpszOemString                   // [out] Converted string (OEM)
//
// Returns: 
//      Pointer to the converted string
//
//************************************************************************
// 05/31/96 Jim Worden - Function created
//************************************************************************
LPSTR SYM_EXPORT WINAPI NavAnsiToOem
(
    LPSTR lpszAnsiString,               // [in]  String to be converted (ANSI)
    LPSTR lpszOemString                 // [out] Converted string (OEM)
)
{
    if (lpszAnsiString != lpszOemString)
        STRCPY (lpszOemString, lpszAnsiString);

    return (lpszOemString);
}

#endif


#ifdef SYM_WIN

//************************************************************************
// NavAnsiToOemUpper()
//
// This routine translates the specified Ansi string to OEM. Unlike 
// NavAnsiToOem(), characters that do not translate to OEM are not preserved
// in the string. Also, like NAnsiToOem(), this routine upper cases the
// characters. However, DBCS characters are not upper cased.
//
// This function is meant as a replacement for NAnsiToOem() calls. The
// difference is that NAnsiToOem() upper cases all characters. This does
// not upper case DBCS characters.
//
// Remember that lpszAnsiStr may be the same pointer as lpszOemStr.
//
// Parameters:
//      lpszAnsiStr    [in]  Ansi string to convert to OEM.
//      lpszOemStr     [out] The OEM version of lpszAnsiStr.
//
//************************************************************************
// 10/09/96 Jim Brennan - Function created.
//************************************************************************
void SYM_EXPORT WINAPI NavAnsiToOemUpper (LPCSTR lpszAnsiStr,
                                          LPSTR  lpszOemStr)
{
    SYM_ASSERT (lpszAnsiStr);
    SYM_ASSERT (lpszOemStr);


                                        // Go the easy way if there is no way
                                        // for DBCS characters to be in the string.
    if (!DBCSInstalled ())
        NAnsiToOem ((LPSTR)lpszAnsiStr, lpszOemStr);
    else
    {
        auto   LPSTR   lpszCurOemStr = lpszOemStr;
        auto   char    szAnsiChar[3];
        auto   char    szOemChar[3];
        auto   WORD    wChars;

        for (; *lpszAnsiStr; lpszAnsiStr = AnsiNext (lpszAnsiStr))
        {
            MEMSET (szAnsiChar, 0, sizeof (szAnsiChar));
            szAnsiChar[0] = lpszAnsiStr[0];

                                        // If this is a DBCS char, put it on
                                        // szAnsiChar. Otherwise, this is not
                                        // a DBCS char, so upper case the 
                                        // non-DBCS char.
            if (DBCSInstalled () && DBCSIsLeadByte (*lpszAnsiStr))
            {
                szAnsiChar[1] = lpszAnsiStr[1];
                wChars = 2;
            }
            else 
            {
                AnsiUpper (szAnsiChar);
                wChars = 1;
            }

            AnsiToOem (szAnsiChar, szOemChar);

                                        // Copy 1st char to lpszOemStr.
            *lpszCurOemStr = szOemChar[0];
            ++lpszCurOemStr;

                                        // If there is a 2nd char, copy it too.
            if (wChars == 2)
            {
                *lpszCurOemStr = szOemChar[1];
                ++lpszCurOemStr;
            }
        }

        *lpszCurOemStr = EOS;           // NULL terminate the string.
    }

} // NavAnsiToOemUpper()

#endif // SYM_WIN


#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)

// This stuff is needed for run-time linking to functionality
// only available on WinNT systems.

typedef BOOL (WINAPI *LPFNGETFILEATTRIBUTESEX)( LPCTSTR, GET_FILEEX_INFO_LEVELS, LPVOID );
#define GETFILEATTRIBUTESEXW    _T("GetFileAttributesExW")
#define GETFILEATTRIBUTESEXA    _T("GetFileAttributesExA")

///////////////////////////////////////////////////////////////////////////
//
// Function:    GetLastAccessDateNT()
//
// Description:
//  Reads a file's last accessed date w/o changing that date.  This function
//  exists because for NT we have to use a different API, and it must be
//  done before the file is opened, cuz opening a file changes the last
//  accessed date.  (This is a totally different procedure than on 95.)
//
// Input:
//  szFileName: [in] Fully-qualified path to the file.
//  lpFileTime: [out] Receives the last accessed date if the function is
//                    successful.
//
// Returns:
//  TRUE if successful, FALSE if not.
//
///////////////////////////////////////////////////////////////////////////
// 1/2/98 Mike Dunn -- swiped from avapi.cpp
///////////////////////////////////////////////////////////////////////////

BOOL SYM_EXPORT GetLastAccessDateNT ( LPCTSTR szFileName,
                                      FILETIME *lpFileTime )
{
       WIN32_FILE_ATTRIBUTE_DATA attribData;
static LPFNGETFILEATTRIBUTESEX   pGetFileAttributes = NULL;

    SYM_ASSERT ( SYM_SYSTEM_WIN_NT == SystemGetWindowsType() );


    // Get pointer to GetFileAttributesEx function
    if( pGetFileAttributes == NULL )
        {
        HMODULE hKernel32 = GetModuleHandle( _T("KERNEL32"));

        if( hKernel32 == NULL )
            return FALSE;

#ifdef UNICODE
        pGetFileAttributes = (LPFNGETFILEATTRIBUTESEX)
                             GetProcAddress( hKernel32,
                                             GETFILEATTRIBUTESEXW );
#else
        pGetFileAttributes = (LPFNGETFILEATTRIBUTESEX)
                             GetProcAddress( hKernel32,
                                             GETFILEATTRIBUTESEXA );

#endif
        if( pGetFileAttributes == NULL )
            return FALSE;
        }


    // Get extended file attributes
    if( pGetFileAttributes( szFileName,
                            GetFileExInfoStandard,
                            &attribData ) )
        {
        // Save off last file access.
        *lpFileTime = attribData.ftLastAccessTime;
        return TRUE;
        }

    return FALSE;
}

#endif  // defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)

