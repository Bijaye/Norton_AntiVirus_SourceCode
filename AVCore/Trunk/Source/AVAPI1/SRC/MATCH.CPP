// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVAPI1/VCS/match.cpv   1.0   11 May 1997 19:55:16   DALLEE  $
//
// Description:
//      Code for matching a wildcard specification of a single path
//      section.
//
// Contains:
//      FileNameMatchSection()
//
// See Also:
//      fname.h
//************************************************************************
// $Log:   S:/AVAPI1/VCS/match.cpv  $
// 
//    Rev 1.0   11 May 1997 19:55:16   DALLEE
// Initial revision.
//************************************************************************

#include "platform.h"
#include "fname.h"

//************************************************************************
// Helper prototypes

static LPTSTR NonWildStrStr ( LPUINT     lpuStrLen,
                              LPCTSTR    lpszFile,
                              LPCTSTR    lpszWild );

static BOOL MatchSection ( LPCTSTR lpszWild, LPCTSTR lpszFile );


//************************************************************************
// FileNameMatchSection()
//
// This routine determines whether one path section (filename w/o directory)
// matches a wildcard specification according to LFN filename matching rules.
//
// Parameters:
//  LPCTSTR lpszWild        Wildcard file specification.
//  LPCTSTR lpszFile        Single path section.
//
// Returns:
//  TRUE                    if match.
//  FALSE                   otherwise.
//************************************************************************
// 04/29/1996 DALLEE, created - based on _WildcardMatchLong from SYMKRNL.
//************************************************************************

BOOL WINAPI FileNameMatchSection ( LPCTSTR lpszWild, LPCTSTR lpszFile )
{
    auto    LPTSTR  lpszLowerWild;
    auto    LPTSTR  lpszLowerFile;

    auto    LPTSTR  lpszWildExt;
    auto    UINT    uWildLength;

    auto    BOOL    bMatch;

    SYM_ASSERT( lpszWild );
    SYM_ASSERT( lpszFile );

    // Initialize locals.

    lpszLowerWild = NULL;
    lpszLowerFile = NULL;
    bMatch = FALSE;

    // Check for special case "*.*" to match all files.

    if ( 0 == STRCMP( lpszWild, _T("*.*") ) )
    {
        bMatch = TRUE;
    }
    else
    {
        // Convert lpszWild and lpszFile to lower case and see if
        // we have a match.

        lpszLowerWild = (LPTSTR)MemAllocPtr( GMEM_MOVEABLE, ( STRLEN( lpszWild ) + 1 ) * sizeof( TCHAR ) );
        lpszLowerFile = (LPTSTR)MemAllocPtr( GMEM_MOVEABLE, ( STRLEN( lpszFile ) + 1 ) * sizeof( TCHAR ) );

        if ( ( NULL == lpszLowerWild ) ||
             ( NULL == lpszLowerFile ) )
        {
            goto BailOut;
        }

        STRCPY( lpszLowerWild, lpszWild );
        STRCPY( lpszLowerFile, lpszFile );

        FileNameToLower( lpszLowerWild );
        FileNameToLower( lpszLowerFile );

        // Special handling for wildcard specifications having only
        // one '.'.  Special endings are ".*" and ".".

        lpszWildExt = STRCHR( lpszLowerWild, _T('.') );
        uWildLength = STRLEN( lpszLowerWild );

        if ( ( ( lpszLowerWild + uWildLength - 2 ) == lpszWildExt ) &&
             ( _T('*') == lpszWildExt[1] ) )
        {
            // Ending of ".*" can match any extension, or no extension
            // at all.

            if ( NULL == STRCHR( lpszLowerFile, _T('.') ) )
            {
                *lpszWildExt = _T('\0');
            }
        }
        else if ( ( lpszLowerWild + uWildLength - 1 ) == lpszWildExt )
        {
            // Ending of "." means match only files with no extension.

            if ( NULL != STRCHR( lpszLowerFile, _T('.') ) )
            {
                // Filename had extension -- no match.

                goto BailOut;
            }
            else
            {
                // Otherwise strip ending '.' and see if names match.

                *lpszWildExt = _T('\0');
            }
        }

        // See if sections match

        bMatch = MatchSection( lpszLowerWild, lpszLowerFile );
    }

BailOut:
    if ( NULL != lpszLowerWild )
        MemFreePtr( lpszLowerWild );

    if ( NULL != lpszLowerFile )
        MemFreePtr( lpszLowerFile );

    return ( bMatch );
} // FileNameMatchSection()


//************************************************************************
// MatchSection()
//
// This routine is called by FileNameMatchSection to perform the filename
// matching. FileNameMatchSection() is really a front end for this routine
// to massage the input and handle a few special cases.
//
// Parameters:
//      As for FileNameMatchSection
// Returns:
//      As for FileNameMatchSection
//************************************************************************
// 04/29/1997 DALLEE, created - based on _WildCardMatchLong from SYMKRNL.
//************************************************************************

static BOOL MatchSection ( LPCTSTR lpszWild, LPCTSTR lpszFile )
{
    auto    UINT    uStrLen;            // Length of string matched by
                                        // NonWildStrStr().

    while ( *lpszWild )
    {
        if ( _T('*') == *lpszWild )
        {
            // '*' which will match 0 or more characters.

            // The scheme here is to find the next non-wild characters
            // in lpszWild, skip to the next matching characters in lpszFile
            // and call this function recursively to match the rest.

            // Skip past consecutive wildcards to find next non-wild chars.

            for (;;)
            {
                if ( _T('?') == *lpszWild )
                {
                    // '?' must match exactly one, so advance both lpszWild
                    // and lpszFile.

                    if ( _T('\0') == *lpszFile )
                    {
                        return ( FALSE );
                    }
                    else
                    {
                        lpszWild++;
                        lpszFile = CharNext( lpszFile );
                    }
                }
                else if ( _T('*') == *lpszWild )
                {
                    // '*' matches zero or more, so advance only lpszWild.

                    lpszWild++;
                }
                else
                {
                    // End of wildcard characters so break.

                    break;
                }
            }

            if ( _T('\0') == *lpszWild )
            {
                // If we had wildcards to the end of lpszWild, then
                // names matched.

                return ( TRUE );
            }

            for (;;)
            {
                // Get the start and length of the string in lpszFile
                // that matches the non-wild leading characters of lpszWild.

                lpszFile = NonWildStrStr( &uStrLen, lpszFile, lpszWild );

                // Non-wild string not found in lpszFile, no match.

                if ( NULL == lpszFile )
                    return ( FALSE );

                // Skip past this matching section and call this function
                // recursively to match rest of lpszWild.

                if ( MatchSection( lpszWild + uStrLen, lpszFile + uStrLen ) )
                {
                    return ( TRUE );
                }
                else
                {
                    // If no match yet, advance lpszFile one char and find the
                    // next instance of the non-wild leading chars of lpszWild.

                    lpszFile = CharNext( lpszFile );
                }
            }
        }
        else
        {
            if ( _T('?') == *lpszWild )
            {
                // '?' which must match exactly one character in lpszFile.

                if ( _T('\0') == *lpszFile )
                    return ( FALSE );
            }
            else
            {
                // Not a wildcard, so regular characters must match.

#ifdef _MBCS
                if ( IsDBCSLeadByte( *lpszWild ) )
                {
                    // Both halves of DBCS char must match.
                    // Note DB comparison SHOULD be case-sensitive.

                    if ( ( *lpszWild   != *lpszFile ) ||
                         ( lpszWild[1] != lpszFile[1] ) )
                        return ( FALSE );
                }
                else
#endif
                {
                    if ( *lpszWild != *lpszFile )
                        return ( FALSE );
                }
            }

            // Characters matched, advance one character each.

            lpszWild = CharNext( lpszWild );
            lpszFile = CharNext( lpszFile );
        }
    }

    // We've reached the end of the wildcard specification, so this
    // should be the end of the filename as well for a match.

    return( _T('\0') == *lpszFile ? TRUE : FALSE );
} // MatchSection()


//************************************************************************
// NonWildStrStr()
//
// This routine searches lpszFile for the leading non-wild string of
// characters found in lpszWild.
// This function is similar to STRSTR() except the search string is limited
// and the length is returned through lpuStrLen.
//
// Parameters:
//      LPUINT  lpuStrLen               [out] Length of match string.
//      LPCTSTR lpszFile                Buffer to search.
//      LPCTSTR lpszWild                Buffer with target string.  This
//                                        should begin with at least one
//                                        valid non-wild search character.
//
// Returns:
//      If found, a pointer to the first character of the search string
//      in lpszFile is returned and *lpuStrLen is set to the length of
//      the search string.
//      Otherwise, the function returns NULL.
//************************************************************************
// 05/01/1997 DALLEE, created.
//************************************************************************

static LPTSTR NonWildStrStr ( LPUINT     lpuStrLen,
                              LPCTSTR    lpszFile,
                              LPCTSTR    lpszWild )
{
    auto    LPCTSTR lpszEnd;            // last location to search in lpszFile

    auto    UINT    uSearchLen;         // length of non-wild search string
    auto    UINT    uBufferLen;         // length of buffer to search

    auto    LPCTSTR s1;                 // char to compare in buffer
    auto    LPCTSTR s2;                 // char to compare in search string
    auto    UINT    u;                  // number of chars left to compare

    // Find length of non-wild string to match.
    // Input should be filtered such that lpszWild always begins with
    // at least one non-wild character.

    *lpuStrLen = \
    uSearchLen = STRCSPN( lpszWild, _T("*?") );

    SYM_ASSERT( 0 != uSearchLen );

    // Verify buffer to search is large enough to contain string.
    // (simplifies end conditional in for() statement below)

    uBufferLen = STRLEN( lpszFile );

    if ( uBufferLen < uSearchLen )
    {
        return ( NULL );
    }

    // Find search string in buffer.
    // lpszEnd is last position to search for string.

    for ( lpszEnd = lpszFile + uBufferLen - uSearchLen;
          lpszFile <= lpszEnd;
          lpszFile = CharNext( lpszFile ) )
    {
        s1 = lpszFile;
        s2 = lpszWild;
        u  = uSearchLen;

        // See if this buffer location matches search string.

        while ( *s1 == *s2 )
        {
            if ( 0 == --u )
            {
                // Found string.

                return ( (LPTSTR) lpszFile );
            }

            s1++;
            s2++;
        }
    }

    return ( NULL );
} // NonWildStrStr()

