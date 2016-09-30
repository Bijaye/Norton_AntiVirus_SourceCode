// Copyright 1994 Symantec Corporation
//************************************************************************
//
// $Header:   S:/SYMKRNL/VCS/WILDCARD.C_v   1.5   09 Sep 1997 18:06:48   asanjab  $
//
// Description:
//      Contains all routines that work with wildcards.
//
// Contains:
//      NameUnWildcard (8.3 only)
//      NameWildtoName (8.3 only)
//      NameWildcardMatch (8.3 and LFN)
//
//************************************************************************
// $Log:   S:/SYMKRNL/VCS/WILDCARD.C_v  $
// 
//    Rev 1.5   09 Sep 1997 18:06:48   asanjab
// Merged changes from USHI
// 
//    Rev 1.4   07 Jul 1997 12:02:06   DBUCHES
// Removed previous revision.
// 
//    Rev 1.3   06 Jul 1997 17:47:48   ASANJAB
// Fixed Probelm wint FA5x characters
// 
//    Rev 1.2   20 Dec 1996 12:07:06   MLOPEZ
// Rolling in fix for bogus orphaned LFN back to trunk
// 
//    Rev 1.1   06 May 1996 14:24:32   RSTANEV
// Merged Ron's SYM_NTK and Unicode changes.
//
//    Rev 1.0   26 Jan 1996 20:21:28   JREARDON
// Initial revision.
//
//    Rev 1.8   14 Sep 1995 16:51:24   MARKK
// Fixed problem with DBCS wildcard matching
//
//    Rev 1.7   05 Jan 1995 20:37:00   DAVID
// Made some changes to _NonWildStrStr() to support UnErase.
//
//    Rev 1.6   03 Jan 1995 17:45:54   MARKL
// Fixed some routines for VxD platform.
//
//    Rev 1.5   09 Nov 1994 15:51:22   BRAD
// DBCS-enabled
//
//    Rev 1.4   02 Nov 1994 22:20:08   BRUCE
// Modified matching to allow '?' in the actual filename for Unerase support
//
//    Rev 1.3   02 Nov 1994 20:29:22   BRUCE
// Changed NameWildcardMatch() to take LPCSTR parms
//
//    Rev 1.2   26 Sep 1994 19:05:20   DALLEE
// _WildcardMatchLong: Fixed handling of '?' following '*'.
//    Removed SYM_MAX_PATH size buffer from stack.
//
//    Rev 1.1   26 Sep 1994 12:38:10   BRAD
// Added *.* support
//
//    Rev 1.0   26 Jul 1994 11:29:58   BRAD
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "dbcs.h"

//-------------------------------------------------------------------------
// LOCAL PROTOTYPES
//-------------------------------------------------------------------------
BOOL LOCAL PASCAL _WildcardMatchLong(LPTSTR lpszWildcard, LPTSTR lpszFile);
BOOL LOCAL PASCAL _WildcardMatchShort(LPTSTR lpszWildcard, LPTSTR lpszFile);
VOID LOCAL PASCAL _CompressWildcardChars(LPTSTR lpszWildcard);
VOID LOCAL PASCAL _RemoveStarDotStar(LPTSTR lpszWildcard);
LPTSTR LOCAL PASCAL _NonWildStrStr(LPUINT    lpuStrLen,
                                  LPTSTR     lpszFile,
                                  LPTSTR     lpszWildcard);

//-------------------------------------------------------------------------
// NameWildcardMatch()
//
// Tests to see if the filename 'lpszFile' matches the filespec (possibly
// containing wildcards) 'lpszWildcard'.  If it matches, then it will
// return TRUE, else it will return FALSE.
//
// Note: This routine works for both LONG and SHORT filenames.
//
// Note: The exceptions listed are those defined in the "DOS/WIN32
//      Installable File System Specification (7/8/94; page 7)", which
//      is the internal file system usage for wildcard matching.  We use
//      the definition explained in this manual as is.
//
// Note:  VxD only cares about WILDCARDMATCH_LONG and WILDCARDMATCH_SHORT.
//
//
// uMatchType can be:
//      WILDCARDMATCH_QUERY     Query the filespec for LONG or SHORT support
//      WILDCARDMATCH_LONG      Filter using LFN method.
//      WILDCARDMATCH_SHORT     Filter using DOS method.
//      WILDCARDMATCH_COMMAND   If LFN, use COMMAND.COM pattern matching (&? NOT YET SUPPORTED)
//      WILDCARDMATCH_FINDFIRST If LFN, use FINDFIRST pattern matching (default)
//-------------------------------------------------------------------------
BOOL SYM_EXPORT WINAPI NameWildcardMatch(
   LPCTSTR lpszWildcard,
   LPCTSTR lpszFile,
   UINT uMatchType)
{
    BOOL        bResult;
    TCHAR       szFile[SYM_MAX_PATH];
    TCHAR       szWildcard[SYM_MAX_PATH];


                                        // Get the filename portion of the
                                        // file.
    NameReturnFile(lpszFile, szFile);
    STRCPY(szWildcard, lpszWildcard);
                                        // Uppercase both strings
    STRUPR(szWildcard);
    STRUPR(szFile);
                                        // Remove 'junk' from wildcard.
    _CompressWildcardChars(szWildcard);

                                        // There are a few 'special cases'
                                        // that need to be handled.
                                        // EXCEPTION 1: "*.*" matches all
                                        // files, and not just files with a
                                        // period in them.
    if (STRCMP(szWildcard, _T("*.*")) == 0)
        return(TRUE);                   // Everything matches this case

                                        // EXCEPTION 2: "*." matches all
                                        // files which don't have an extension,
                                        // and not just all files that end in a
                                        // period.
    if (STRCMP(szWildcard, _T("*.")) == 0)
        {
        LPTSTR   lpszDot;

        lpszDot = STRSTR(szFile, _T("."));
        if (lpszDot == NULL || !*(lpszDot + 1))
            return(TRUE);
        }

#if !defined(SYM_VXD) && !defined(SYM_NTK)
    if (uMatchType == WILDCARDMATCH_QUERY)
        {
        DWORD   dwComponentLength;
        BOOL    bOK;
        TCHAR   szFullPath[SYM_MAX_PATH];
        TCHAR   szFile[SYM_MAX_PATH];

        STRCPY(szFile, lpszFile);

        NameToFull(szFullPath, szFile);
        bOK = VolumeInformationGet(szFullPath, NULL, 0, NULL, &dwComponentLength,
                             NULL, NULL, 0);
        uMatchType = (bOK && dwComponentLength > 12) ? WILDCARDMATCH_LONG :
                                                       WILDCARDMATCH_SHORT;
        }
#endif

    if (uMatchType == WILDCARDMATCH_SHORT)
        {
        bResult = _WildcardMatchShort(szWildcard, szFile);
        }
    else
        {
        _RemoveStarDotStar(szWildcard);
        bResult = _WildcardMatchLong(szWildcard, szFile);
        }
    return(bResult);
}


//-------------------------------------------------------------------------
// _CompressWildcardChars()
//      1) Removes any extra '*' characters (If 2 '*' are next to each other).
//      2) Converts *.* into just a '*'
//-------------------------------------------------------------------------
VOID LOCAL PASCAL _CompressWildcardChars(LPTSTR lpszWildcard)
{
    while (*lpszWildcard)
        {
        if (*lpszWildcard == '*')
            {
            if (*(lpszWildcard + 1) == '*')
                MEMMOVE(lpszWildcard+1, lpszWildcard+2,
                        (STRLEN(lpszWildcard+2)+1) * sizeof(TCHAR) );
            }

        lpszWildcard = AnsiNext(lpszWildcard);
        }
}


//-------------------------------------------------------------------------
// Converts "*.*"s into "*", since thats what LFN pattern matching does.
//-------------------------------------------------------------------------
VOID LOCAL PASCAL _RemoveStarDotStar(LPTSTR lpszWildcard)
{
    while (TRUE)
        {
        lpszWildcard = STRSTR(lpszWildcard, _T("*.*"));
        if (!lpszWildcard)
            break;
                                        // Move string up, removing ".*"
                                        // portion.
        MEMMOVE(lpszWildcard+1, lpszWildcard+3,
                (STRLEN(lpszWildcard+3)+1) * sizeof(TCHAR) );
        }
}


//-------------------------------------------------------------------------
// _WildcardMatchShort()
//
// Tests to see if the filename 'lpszFile' matches the filespec (possibly
// containing wildcards) 'lpszWildcard'.  If it matches, then it will
// return TRUE, else it will return FALSE.
//
// Note: This routine is called for SHORT filenames (8.3).
//
// 11/2/94  BEM - Modified to allow ? character in lpszFile for UnErase
//                to match filenames without a first character.
//-------------------------------------------------------------------------
BOOL LOCAL PASCAL _WildcardMatchShort(LPTSTR lpszWildcard, LPTSTR lpszFile)
{
    UINT        i;
    TCHAR       szWildcard[SYM_MAX_PATH];
    TCHAR       szFile[SYM_MAX_PATH];


    NameUnWildcard(lpszWildcard, szWildcard);
    NameUnWildcard(lpszFile,     szFile);

    for (i = 0; i < 11; i++)
        {
// 12-20-96
// This is a necessary hack to prevent a bug in which occurred if the
// 5th character was DBCS which causes the basis name to be 8 bytes
// and the short name to be 7 bytes thus causing a fail when comparing
// the eighth byte.
        if ( ( i == 5 ) && (DBCSIsLeadByte( szFile[i] )) &&
             (szWildcard[i] == '?') && (szWildcard[i+1] == '?'))
             szWildcard[7] = '?';

                                        //&? BEM - This is where I've added
                                        //&? special casing for erased files
                                        //&? that have a ? where DOS lost
                                        //&? the first character
        if (szWildcard[i] == '?' || (szFile[i] == '?' && i == 0))
            continue;
        				
										//Fix for the FA5x Characters.						
                                        //In WIN32 platform:
                                        //If we find a DBCS character
                                        //compare it using comparestring with the
                                        //Ignorekanatype flag so it will not flag
                                        //Hiragana and Katakana characters that look
                                        //the same.
                                        //Shouldn't worry about this for DOS for now
                                        //Since we don't validate the LFNs in DOS.  
#ifdef SYM_WIN32
		if ( DBCSIsLeadByte(szFile[i]) )
                {
                       if (2 != CompareString(LOCALE_SYSTEM_DEFAULT, 
                                        NORM_IGNORECASE | NORM_IGNOREKANATYPE,
                                        &szFile[i],
                                        2,
                                        &szWildcard[i],
                                        2))
                                
                            return (FALSE);
                        //endif
                        i++;   //Skip to next char
                }
		else
                {
                        if (szWildcard[i] != szFile[i])
                                return(FALSE);
                }  
#else
        if (szWildcard[i] != szFile[i])
            return(FALSE);

        if (DBCSIsLeadByte(szFile[i]))
            {
            i++;
            if (szWildcard[i] != szFile[i])
                return(FALSE);
            }
#endif
        }//End For Loop for i
    return(TRUE);
}


//-------------------------------------------------------------------------
// _WildcardMatchLong()
//
// Tests to see if the filename 'lpszFile' matches the filespec (possibly
// containing wildcards) 'lpszWildcard'.  If it matches, then it will
// return TRUE, else it will return FALSE.
//
// Note: This routine is called for LONG filenames.  It is RECURSIVE.
//       Long File Names support Regular Expressions.  We need to be
//       able to match the following example:
//              lpszFile = "abcabcde"
//              lpszWildcard = "*abc?e"
//      Make sure we don't just match the first occurance of 'abc', since
//      that would be wrong.
//
//      The regular expression matching is as follows:
//              *       - 0 or more characters
//              ?       - match exactly 1 character
//
//      These are defined in the "DOS/WIN32 Installable File System
//      Specification (7/8/94; page 7)", which is the internal file system
//      usage for wildcard matching.  We use the definition explained in
//      this manual as is.
//
// AGAIN NOTE: RECURSIVE ROUTINE--DON'T DECLARE LOCAL VARIABLES.  IT WILL
// RECURSE TO A MAXIMUM DEPTH EQUAL TO THE NUMBER OF '*' CHARACTERS IN
// THE WILDCARD STRING.
//-------------------------------------------------------------------------
// 09/26/94 DALLEE, removed SYM_MAX_PATH size buffer from stack.
//            Call _NonWildStrStr() instead.
//          Advance lpszFile after finding "*?" -- "?" still must match
//            exactly one character.
// 11/2/94  BEM - Modified to allow ? character in lpszFile for UnErase
//                to match filenames without a first character.
//-------------------------------------------------------------------------

BOOL LOCAL PASCAL _WildcardMatchLong(LPTSTR lpszWildcard, LPTSTR lpszFile)
{
    auto    UINT    uStrLen;            // Length of string to matched by
                                        // _NonWildStrStr().

    while (*lpszWildcard)
        {
        if (*lpszWildcard == '*')       // 0 or more characters wildcard
            {
                                        // Recursively call this function,
                                        // starting at the char after the
                                        // wildcard(s).
                                        // That way, we can properly handle
                                        // regular expressions.
            while (*lpszWildcard)
                {
                                        // '?' means match one, so we must
                                        // skip one character in lpszFile.
                if ('?' == *lpszWildcard)
                    {
                    if (EOS == *lpszFile)
                        {
                        return (FALSE);
                        }

                    lpszFile = AnsiNext(lpszFile);
                    lpszWildcard = AnsiNext(lpszWildcard);
                    }
                                        // '*' means match zero or more, so
                                        // just advance lpszExclude.
                else if ('*' == *lpszWildcard)
                    {
                    lpszWildcard = AnsiNext(lpszWildcard);
                    }
                else                    // Break if end of wildcard characters.
                    {
                    break;
                    }
                }
            if (!*lpszWildcard)
                return(TRUE);

            while (TRUE)
                {
                                        // Get start and length of string in
                                        // lpszFile that matches the non-wild
                                        // leading characters of lpszWildcard.

                lpszFile = _NonWildStrStr(&uStrLen, lpszFile, lpszWildcard);

                if (!lpszFile)          // Not found.  NO MATCH
                    return(FALSE);

                                        // Skip over this string.
                                        // Recursively call this function,
                                        // matching the rest of lpszWildcard.
                if (_WildcardMatchLong(lpszWildcard+uStrLen, lpszFile+uStrLen))
                    {
                    return(TRUE);
                    }
                else
                    {
                                        // Search 1 character later in the string
                    lpszFile = AnsiNext(lpszFile);
                    }
                }
            }
                                        // Single character wildcard
                                        //&? BEM - This is where I've added
                                        //&? special casing for erased files
                                        //&? that have a ? where DOS lost
                                        //&? the first character
        else if (*lpszWildcard == '?' || *lpszFile == '?')
            {
            if (*lpszFile == EOS)       // '?' never matches EOS
                return(FALSE);
            goto MoveToNextChar;
            }
        else if (*lpszWildcard != *lpszFile)
            {
            return(FALSE);              // Doesn't match.
            }
        else if (DBCSIsLeadByte(*lpszWildcard) &&
                 (lpszWildcard[1] != lpszFile[1]))
            {
            return (FALSE);
            }
        else
            {                           // Matches.  Move to next character
MoveToNextChar:
            lpszWildcard = AnsiNext(lpszWildcard);
            lpszFile = AnsiNext(lpszFile);
            }
        }
                                        // All characters of the filename
                                        // must also be exhausted (since all
                                        // characters in the wildcard have
                                        // been).
    return((*lpszFile == EOS) ? TRUE : FALSE);
}


//************************************************************************
// _NonWildStrStr()
//
// This routine searches lpszFile for the leading non-wild string of
// characters found in lpszWildcard.
// This function is similar to STRSTR() except the search string is limited
// and the length is returned through lpuStrLen.
//
// Parameters:
//      LPUINT  lpuStrLen               [out] Length of match string.
//      LPCTSTR  lpszFile                Buffer to search.
//      LPCTSTR  lpszWildcard            Buffer with target string.  This
//                                        should begin with at least one
//                                        valid search character.
//
// Returns:
//      If found, a pointer to the first character of the search string
//      in lpszFile is returned and *lpuStrLen is set to the length of
//      the search string.
//      Otherwise, the function returns NULL.
//************************************************************************
// 09/26/94 DALLEE, created.
// 01/05/96 DRH - Modified to allow ? character in lpszFile for UnErase
//                to match filenames without a first character.
//************************************************************************

LPTSTR LOCAL PASCAL _NonWildStrStr(LPUINT   lpuStrLen,
                                  LPTSTR    lpszFile,
                                  LPTSTR    lpszWildcard)
{
    auto    TCHAR   szSearch [SYM_MAX_PATH];
    auto    UINT    uLen;
    auto    LPTSTR  s, t;
    auto    BOOL    bMatch;

                                        // Find length of non-wild string
                                        // to match.
    uLen = 0;
    while ( (EOS != *lpszWildcard) &&
            ('*' != *lpszWildcard) &&
            ('?' != *lpszWildcard) )
        {
        szSearch[uLen] = *lpszWildcard;
        if (DBCSIsLeadByte(*lpszWildcard))
            {
            szSearch[++uLen] = *(++lpszWildcard);
            }
        lpszWildcard++;
        uLen++;
        }
    szSearch[uLen] = EOS;

    *lpuStrLen = uLen;

                                        // lpszWildcard is filtered so we
                                        // should have at least one valid
                                        // character to match.

    if (STRCHR(lpszFile, '?') == NULL)
	return (STRSTR(lpszFile, szSearch));
    else
        {
        while (*lpszFile != EOS)
	    {
	    bMatch = TRUE;
	    for (s = lpszFile, t = szSearch; *s != EOS && *t != EOS;
					     s = AnsiNext(s), t = AnsiNext(t))
		{
                if (*s != '?')
                    {
                    if (*s != *t)
                        {
                        bMatch = FALSE;
                        break;
                        }
                    else if (DBCSIsLeadByte(*s) && (s[1] != t[1]))
                        {
                        bMatch = FALSE;
                        break;
                        }
                    }
		}

	    if (bMatch && *t == EOS)
		return lpszFile;
	    lpszFile = AnsiNext(lpszFile);
	    }
        return NULL;
	}

} // _NonWildStrStr()


/*----------------------------------------------------------------------*
 *              Copyright (C) 1986-1991 by Peter Norton Computing       *
 *                                                                      *
 *      NameUnWildcard                                                 *
 *                                                                      *
 *      This procedure expands a filename, possibly containing the      *
 *      wildcards * and ?,into a 11-character sequence conforming to    *
 *      the format used by directory entries.                           *
 *                                                                      *
 *      NOTE: YOU SHOULD NEVER CALL THIS FUNCTION DIRECTLY, SINCE IT    *
 *      DOESN'T HANDLE LFNs. IN THE NEAR FUTURE, WE NEED TO REMOVE THE  *
 *      SYM_EXPORT REFERENCE AND MAKE IT LOCAL.                         *
 *                                                                      *
 *      That is, the new name will be in uppercase, the root and        *
 *      extension names will be padded with blanks as necessary,        *
 *      and asterisks will be expanded into a sequence of question      *
 *      marks.  The new string is NOT null terminated.                  *
 *                                                                      *
 *      The original name is typically something entered by the user.   *
 *                                                                      *
 *      Examples:       "*.*"           -->     ???????????             *
 *                      "qu.c"          -->     QU      C               *
 *                      "file.*"        -->     FILE    ???             *
 *                      "a?bc*.b*"      -->     A?BC????B??             *
 *                      "*"             -->     ????????      (fix?)    *
 *                      ".*"            -->             ???   (fix?)    *
 *                      "."             -->                   (fix?)    *
 *                      "*."            -->     ????????                *
 *                                                                      *
 *                      MODIFICATION   HISTORY                          *
 *                                                                      *
 * 04/21/86     Brad:   Created                                         *
 * 06/06/89     David:  Fixed bug which caused incorrect expansion of   *
 *                      asterisks in the extension which were not       *
 *                      immediately after the dot.  Removed erroneous   *
 *                      (and inconsistent) assignment of terminating    *
 *                      null.                                           *
 * 05/14/91     Enrique:Ported to windows.                              *
 *----------------------------------------------------------------------*/

VOID SYM_EXPORT WINAPI NameUnWildcard (LPCTSTR lpOrig, LPTSTR lpNew)
{
     register   int             i;      /* index into original string   */
     register   int             j;      /* index into new string        */
     auto       int             k;      /* counts extension chars       */
     auto       TCHAR           c;      /* char read from original str  */
     auto       TCHAR           szName [SYM_MAX_PATH];

     STRCPY (szName, lpOrig);           // don't alter the original
     STRUPR(szName);

     /* first parse filename */
     for (i = j = 0; i < 8; i++)
         {
         c = szName [i];
         if (!c)
             break;

         /* check for single character wildcard */
         if (c == '?')
             lpNew [j++] = c;

         /* check for global wild-card character */
         else if (c == '*')
             {
             for (j = i; j < 8; j++)
                 lpNew [j] = '?';
             break;
             }

         /* check for extension terminator */
         else if (c == '.')
             break;

         /* character is just single character of file name */
         else
             {
             lpNew [j++] = c;
             if (DBCSIsLeadByte(c))
                lpNew[j++] = szName[++i];
             }
         }

     /* clean up the destination (fill out the filename portion) */
     while (j < 8)
         lpNew [j++] = ' ';

     /* now clean up the source for extraneous stuff */
     while (szName[i] && szName [i] != '.')
         {
         if (DBCSIsLeadByte(szName[i]))
            i++;
         i++;
         }

     /* if the end, then give the extension blanks */
     if (!szName [i])
         {
         for (j = 8; j < 11; j++)
             lpNew [j] = ' ';
         return;
         }

     /* skip over the dot in the szName string */
     i++;

     /* now do the extension */
     for (k = 0; k < 3; k++,i++)
         {
         c = szName [i];
         if (!c)
             break;

         /* check for global wild-card character */
         if (c == '*')
             {
             while (k++ < 3)
                lpNew [j++] = '?';
             return;
             }

         /* character is either a single wildcard char ('?')
          * or an ordinary file name character */
         else
             {
             lpNew [j++] = c;
             if (DBCSIsLeadByte(c))
                lpNew[j++] = szName[++i];
             }
         }

     /* clean up destination string */
     while (j < 11)
         lpNew[j++] = ' ';
}

#if !defined(SYM_VXD) && !defined(SYM_NTK)

/*----------------------------------------------------------------------*
 * This procedure expands a wildcard name in d using the file name in   *
 * s.  d can contain a full path with a wildcard name at the end, but   *
 * s should be a simple file name without any path.                     *
 *                                                                      *
 * Returns:     ERROR   If the wildcard name is not well-formed.  For   *
 *                      example, "*fr.dat" is not legat.                *
 *                        '*' must be at the end of the string, or just *
 *                      before the '.'                                  *
 *                                                                      *
 * NOTE: This function has NOT been DBCS-enabled.  Don't think anyone   *
 * uses it.  Plus, very messy.  Should use routines above.              *
 *                                                                      *
 * Parameters:                                                          *
 *      char *full_name:                                                *
 *      char      *name:                                                *
 *----------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI NameWildtoName(LPTSTR lpFullName, LPCTSTR lpName)
{
    auto        BYTE            szWildName[SYM_MAX_PATH+1];
    auto        BYTE            szNewName[SYM_MAX_PATH+1];
    auto        BYTE            szExpand[SYM_MAX_PATH+1];
    auto        LPTSTR           lpPattern;
    auto        LPTSTR           lpSrc;
    auto        LPTSTR           lpDst;
    auto        int             i;
    auto        int             j;
    auto        int             nNumChars;

    NameReturnFile(lpFullName,szWildName);

    if (!STRPBRK(szWildName,_T("*?")))
        {                               /* There we no wild cards       */
        STRCPY(szNewName, szWildName);
        }
    else
        {
        NameUnWildcard (szWildName,szExpand);
        STRNCPY(szWildName, szExpand, 11);
        szWildName[11] = EOS;
        lpDst = szNewName;
        lpSrc = (LPTSTR) lpName;
        lpPattern = szWildName;
        nNumChars = 8;                  /* Copy the name                */
        for (i = 0; i < 2; i++)
            {
            for (j = 0; j < nNumChars; j++)
                {
                *lpDst = ' ';           /* In case we don't copy        */
                if (*lpPattern == '?')
                    {                   /* Found wild card              */
                    if (*lpSrc != '.' && *lpSrc != '\0')
                        *lpDst = *lpSrc++;      /* Fill from name               */
                    }
                else                    /* Not wild card                */
                    {
                    *lpDst = *lpPattern;/* Fill from lpPatternern               */
                    if (*lpSrc != '.' && *lpSrc != '\0')
                        lpSrc++;        /* Skip one source letter       */
                    }
                lpPattern++;            /* Move to next position        */
                if (*lpDst != ' ')
                    lpDst++;            /* Remove imbedded spaces       */
                }
            if (i == 0)
                {                       /* Just finished name           */
                *lpDst++ = '.';         /* Put '.' before extension     */
                while (*lpSrc != '.' && *lpSrc != '\0')
                    lpSrc++;            /* Find '.' before extension    */
                if (*lpSrc == '.')
                    lpSrc++;            /* Skip over '.'                */
                }
            nNumChars = 3;              /* Now copy extension           */
            }
        *lpDst = '\0';                  /* Write the end-of-string mrk  */
        if (lpDst-- > (LPTSTR)szNewName && *lpDst == '.')
            *lpDst = '\0';              /* Remove '.' if no extension   */
        }

    NameStripFile(lpFullName);          /* Remove the old file name     */
    NameAppendFile(lpFullName, szNewName);      /* Add new file name            */
    STRLWR(lpFullName);                 /* Convert to lower case chars  */
    return (0);                         /* Name was Ok.                 */
}

#endif  // if !defined(SYM_VXD)
