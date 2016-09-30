// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2003, 2005, Symantec Corporation. All rights reserved.

#include "StdAfx.h"
#include <tchar.h>
#include "PrependText.h"
#include "ModifyMail.h"  // Bring in the functions to do code page text translations
#include "SavAssert.h"

typedef struct _TRANSLATE_ELEM
{
    const TCHAR *szMatch;         // ASCII string match against szIn 
    DWORD        dwNumMatchChars; // Number of characters in szMatch to compare against.
    const TCHAR *szReplace;       // ASCII string that replaces szMatch in szIn when a match occurs.
} TRANSLATE_ELEM;

//------------------------------------------------------------------------
// Returns the maximum szResult character length within the array.
//------------------------------------------------------------------------
inline DWORD MaxReplaceChars (const TRANSLATE_ELEM *array, DWORD dwNumElems)
{
    DWORD dwRtn = 0;

    for (DWORD xx = 0; xx < dwNumElems; ++xx)
    {
        DWORD dwChars = _tcslen (array[xx].szReplace);
        if (dwChars > dwRtn)
            dwRtn = dwChars;
    }
    return dwRtn;
}

//------------------------------------------------------------------------
// Determine if sz1 and sz2 are equal. This is optimized for comparing
// the first two bytes by hand and then switching to _tcscmp if the first
// two bytes match. This ensures an efficent search.
//
// Localization concerns: Note that this function only deals with ASCII
// string comparisions. It is okay to mix a ASCII and multi-byte string
// comparision because the multi-byte lead character will never match to
// an ASCII character. As such, this will always return false for a 
// multi-byte string.
//
// Parameters:
//   sz1        [in] string to compare
//   sz2        [in] string to compare
//   dwNumChars [in] number of characters to compare
//
// Returns:
//   true   - they match
//   false  - they do not match
//------------------------------------------------------------------------
inline bool IsMatch (const TCHAR *sz1, const TCHAR *sz2, DWORD dwNumChars)
{
    SAVASSERT (sz1);
    SAVASSERT (sz2);
    SAVASSERT (dwNumChars > 0);

    // If 1st character matches, see if the 2nd character matches.
    if (sz1[0] == sz2[0])
    {
        if (dwNumChars == 1)
            return true;

        // Often we have two byte sequences. So simply try our
        // match on the second character.
        if (sz1[1] == sz2[1])
        {
            if (dwNumChars == 2)
                return true;

            // Okay, well, the 1st two bytes match and there is more to
            // compare. Switch to using _tcsncmp to finish the comparison.
            return _tcsncmp (sz1 + 2, sz2 + 2, dwNumChars - 2) == 0;
        }
    }
    return false;
}

//------------------------------------------------------------------------
// Loop through szHtml looking for the end of the <body> tag.
//
// Returns:
//   NULL     - <body> tag not found
//   non-NULL - pointer just past the last > character in the <body> tag
//              within szHtml.
//------------------------------------------------------------------------
static const TCHAR *FindEndBodyTag (const TCHAR *szHtml)
{
    bool bInTag     = false;
    bool bBodyFound = false;

    // Look for <body>, but also allow whitespace between the <   body   >
    while (*szHtml)
    {
        if (*szHtml == '<')
            bInTag = true;

        else if (bInTag && _tcsnicmp (szHtml, "body", 4) == 0)
            bBodyFound = true;

        else if (*szHtml == '>' && bBodyFound)
            return szHtml + 1;

        else if (*szHtml == '>')
            bInTag = false;

        szHtml = _tcsinc (szHtml);
    }
    return NULL;
}

//------------------------------------------------------------------------
// Copy szIn into szOut while translating character sequences based on
// the encoding map.
//
// Parameters:
//   arrTrans     [in]  An array of TRANSLATE_ELEM structures. This provides
//                      the translation table used when parsing szIn.
//   dwNumElems   [in]  Number of elements in the arrTrans array.
//   szOut        [out] Place to copy the characters to. The caller must
//                      ensure that this buffer has enough space in it
//                      to handle the translation. Typically this means
//                      _tcslen (szIn) * max (<longest-possible-replace-text>).
//   szIn         [in]  Original string requiring copy and translation.
//
// Returns:
//   Pointer to '\0' character written out for szOut.
//------------------------------------------------------------------------
static TCHAR *CopyWhileTranslating (const TRANSLATE_ELEM *arrTrans,
                                    DWORD                 dwNumElems,
                                    TCHAR                *szOut,
                                    const TCHAR          *szIn)
{
    // Loop through all of the characters in szIn individually.
    while (szIn[0])
    {
        int nTransFound = -1;

        // Look for a matching translation string
        for (DWORD xx = 0; nTransFound == -1 && xx < dwNumElems; ++xx)
        {
            if (IsMatch (szIn, arrTrans[xx].szMatch, arrTrans[xx].dwNumMatchChars))
                nTransFound = xx;
        }

        // If there was no translation match, simply copy the potentially
        // multi-byte character from szIn to szOut.
        if (nTransFound == -1)
        {
            char *szNext = _tcsinc (szIn);
            while (szIn != szNext)
            {
                *szOut = *szIn;
                ++szIn;
                ++szOut;
            }
        }
        // Else, we found a translation for the current character(s), copy
        // the string from the translation table to szOut.
        else
        {
            const DWORD dwNumReplaceChars = _tcslen (arrTrans[nTransFound].szReplace);

            // Copy the replacement characters. We do not care about
            // '\0' terminating szOut yet.
            memcpy (szOut, arrTrans[nTransFound].szReplace, dwNumReplaceChars);
            szOut += dwNumReplaceChars;
            szIn  += arrTrans[nTransFound].dwNumMatchChars;
        }
    }
    *szOut = '\0';

    return szOut;
}

DWORD PrependPlainText (TCHAR **pszOut, const TCHAR *szFront, const TCHAR *szBack)
{
    const int nNumFrontChars = _tcslen (szFront);
    const int nNumBackChars  = _tcslen (szBack);
    try
    {
        *pszOut = new TCHAR[nNumFrontChars + nNumBackChars + 1 /* for '\0' */];
    }
    catch (...)
    {
        *pszOut = NULL;
    }
    if (*pszOut == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;

    memcpy (*pszOut,                  szFront, nNumFrontChars);
    memcpy ((*pszOut)+nNumFrontChars, szBack,  nNumBackChars);
    
    (*pszOut)[nNumFrontChars + nNumBackChars] = '\0';
    
    return NO_ERROR;
}

DWORD InsertHtmlText (TCHAR **pszOut, const TCHAR *szMsg, const TCHAR *szHtml, WORD wCodePage)
{
    static const TRANSLATE_ELEM arrTranslateTable[] =
    { //  szMatch     dwNumMatchChars   szReplace
        { "\r\n",     2,                "<br>" },
        { "\r",       1,                "<br>" },
        { "\n",       1,                "<br>" },
        { "<",        1,                "&lt;" },
        { ">",        1,                "&gt;" }
    };
    const DWORD dwNumTranslateElems = sizeof (arrTranslateTable) / sizeof (arrTranslateTable[0]);
    const int   nNumRawMsgChars     = _tcslen (szMsg);
    const int   nNumHtmlChars       = _tcslen (szHtml);
    TCHAR * szHtmlReady = NULL;
    TCHAR * szCodePageReady = NULL;
    DWORD dwResult = NO_ERROR;

    *pszOut = NULL;

    // STEP 1: Prepare the text for inclusion in HTML.
    // This needs to be done with text in a code page that can be understood by the 
    // CharNext function (CP_ACP).

    try
    {
        // Allocate for our worse case scenario: szMsg has every character
        // translated to the longest szReplace string from our TRANSLATE_ELEM
        // table. 
        szHtmlReady = new TCHAR[(nNumRawMsgChars * MaxReplaceChars (arrTranslateTable,
                                                             dwNumTranslateElems)) +
                            + 1 /* for '\0' */];
    }
    catch (...)
    {
        dwResult = ERROR_NOT_ENOUGH_MEMORY;
        goto Cleanup;
    }

    // Do any character translations to make the text "HTML Ready"
    CopyWhileTranslating (arrTranslateTable, dwNumTranslateElems, szHtmlReady, szMsg);

    // STEP 2: Translate it to the encoding used in the HTML text.

    // Work case encoding: iso-2022-jp has 3 byte prefix and postfix escape sequences.
    // 3n + 6 + 1 would work for iso-2022-jp, but others might be worse, so:
    int cchCodePageReady = 4 * _tcslen( szHtmlReady ) + 8 + 1;
	try
	{
        // Allocate for the "code page translated" text
		szCodePageReady = new char [ cchCodePageReady ];
	}
    catch (...)
    {
        dwResult = ERROR_NOT_ENOUGH_MEMORY;
        goto Cleanup;
    }

    if ( !ConvertWindowsString( CP_ACP,
                               wCodePage,
                               szHtmlReady,
                               szCodePageReady, cchCodePageReady ) )
    {
        // Could not translate to the provided code page.
        dwResult = ERROR_INVALID_PARAMETER;
        goto Cleanup;
    }

    // We no longer need the HTML Ready intermediate text, so clean it up.
    delete [] szHtmlReady;
    szHtmlReady = NULL;

    // STEP 3:  Merge it with the exist HTML text.

    const int   nNumMsgChars = _tcslen (szCodePageReady);

    try
    {
        // Allocate for the complete text
        *pszOut = new TCHAR[nNumMsgChars + nNumHtmlChars + 1 /* for '\0' */];
    }
    catch (...)
    {
        dwResult = ERROR_NOT_ENOUGH_MEMORY;
        goto Cleanup;
    }

    TCHAR       *szOut        = *pszOut;
    const TCHAR *szEndBodyTag = FindEndBodyTag (szHtml);

    // There should always be a <BODY> tag in the HTML. Tell the caller
    // when there is not one.
    if (szEndBodyTag == NULL)
    {
        dwResult = ERROR_INVALID_DATA;
        goto Cleanup;
    }

    // Copy all of the text from szHtml to just past the <body> tag.
    memcpy (szOut, szHtml, szEndBodyTag - szHtml);
    szOut += szEndBodyTag - szHtml;

    memcpy (szOut, szCodePageReady, nNumMsgChars );
    szOut += nNumMsgChars;
    _tcscpy (szOut, szEndBodyTag);
    
Cleanup:
    if (dwResult != NO_ERROR)
    {
        delete [] *pszOut;
        *pszOut = NULL;
    }

    delete [] szHtmlReady;
    delete [] szCodePageReady;

    return dwResult;
}
