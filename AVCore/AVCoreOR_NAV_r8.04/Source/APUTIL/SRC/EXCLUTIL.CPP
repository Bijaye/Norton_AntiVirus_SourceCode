// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEXCLU/VCS/EXCLUTIL.CPv   1.2   16 May 1997 09:46:02   RStanev  $
//
// Description:
//
//
//
// Contains:
//
// See Also:
//
//************************************************************************

#include "platform.h"
#include "dbcs.h"
#include "xapi.h"
#include "file.h"
#include "avexcl.h"

// --------------------------------------------------------------------------
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< LOOK >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// These hardcoded DOS type limits are for compatibility with old file
// structures. They belong in an include somewhere that defines the
// structure that require the compatibility.


#define MAX_DOS_PATH       144


#define MAX_SFN_BASE       8
#define MAX_SFN_EXTENSION  3

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<< END LOOK >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
//                          LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------
BOOL LOCAL IsValidSFNSection( LPTSTR lpszSection );
BOOL LOCAL IsOnlyDotsSection ( LPTSTR lpszSection );
LPTSTR LOCAL HasWildcards (              // Checks for wildcard chars
    LPCTSTR  lpString                    // [in] withing this string.
    );

STATUS LOCAL NavNameReturnName (        // Convert from SFN to LFN or vice versa
    LPTSTR   lpszSourceName,             // [in] convert from here
    LPTSTR   lpszTargetName,             // [out] and place result here
    UINT    uTargetSize,                // [in] max size of target
    BOOL    bFromShortToLong            // [in] flag indicating conv. order
    );

#define NavNameReturnShortName(lpS,lpT,s) (NavNameReturnName(lpS,lpT,s,FALSE))
#define NavNameReturnLongName(lpS,lpT,s)  (NavNameReturnName(lpS,lpT,s,TRUE))

// --------------------------------------------------------------------------
// Local functions pulled out of NAVXUTIL
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

UINT SYM_EXPORT WINAPI NavReturnIllegalChars ( // Get illegal characters
    WORD        wOsFlag,                // [in] use any SYM_ILLEGAL_xxx
    LPCTSTR     lpVolume,               // [in] pointer to volume, for LFN
    LPTSTR      lpIllegals,             // [out] illegal chars placed here
    UINT        uSizeIllegals           // [in] max size of lpIllegals
    )
{

    UINT uNumChars = NameReturnIllegalChars (
                                        wOsFlag,
                                        lpVolume,
                                        lpIllegals,
                                        uSizeIllegals
                                        );

    if ( lpIllegals && uSizeIllegals )
        {
                                        // -----------------------------------
                                        // Period is valid character for SFN
                                        // directories. This removes the
                                        // period if it's part of the buffer.
                                        // -----------------------------------

        for ( UINT uIndex = 0; uIndex < uNumChars; uIndex++ )
            {
            if ( lpIllegals[uIndex] == '.' ||
                 lpIllegals[uIndex] == EOS )
                {
                MEMMOVE ( &lpIllegals[uIndex], &lpIllegals[uIndex+1],
                          (uNumChars - uIndex) * sizeof(TCHAR) );

                uNumChars --;
                }
            }
        }

    return ( uNumChars );
}

/****************************************************************************
* Function   : ExcludeIsValid - Valdidates an exclusion SFN/LFN.
*
* Description: This routine is also called by macros ExcludeIsValidSFN()
*              and ExcludeIsValidLFN().
*
* Inputs     : LPCTSTR  lpItem      - Exclusion item to validate
*              BOOL    bIsLFN       - TRUE for LFN, FALSE = SFN only
*
* Returns    : STATUSEXCLUDE_OK     - Item IS valid.
*              STATUSEXCLUDE_ERR    - Item IS NOT valid.
*
****************************************************************************/
STATUSEXCLUDE SYM_EXPORT WINAPI ExcludeIsValid ( LPTSTR lpszItem, BOOL bIsLFN )
{
    int   iItemLen;
    BOOL  bHasVolume = FALSE;
    BOOL  bIsUNCName = FALSE;
    LPTSTR lpTargetChar;
    TCHAR  szPathName    [SYM_MAX_PATH + 1];
    TCHAR  szFileName    [SYM_MAX_PATH + 1];
    TCHAR  szIllegalChars[SYM_MAX_PATH + 1];

    SYM_ASSERT ( lpszItem );
    if ( ( lpszItem ) && ( *lpszItem ) )
        {
                                        //------------------------------------
                                        // First test overall min./max length
                                        // Change all '/' to '\'.
                                        // Next let's test invalid cases
                                        // common to both SFN and LFN's:
                                        //  - An embedded '**' anywhere in the
                                        //    path/name
                                        //  - An embedded '\\' anywhere past
                                        //    the FIRST char. (UNC == '\\...')
                                        //------------------------------------
        iItemLen = STRLEN( lpszItem );
        if ( ( !iItemLen ) ||
            ( iItemLen > ( bIsLFN ? SYM_MAX_PATH : (MAX_DOS_PATH - 1) ) ) )
            return( STATUSEXCLUDE_ERR );
#ifndef SYM_NTK
        NameConvertFwdSlashes( lpszItem );
#endif
        if ( ( STRSTR( lpszItem,     _T("**") ) ) ||
             ( STRSTR( &lpszItem[1], _T("\\\\") ) ) )
            return( STATUSEXCLUDE_ERR );
                                        //------------------------------------
                                        // Now let's break the name into it's
                                        // individual components, namely:
                                        //  - The Volume (X:), if any
                                        //  - Test for, note, and adjust for a
                                        //    UNC path.
                                        //  - The path excluding the volume,
                                        //    if any.
                                        //  - The file, if any.
                                        //  - Set a flag if name is UNC (starts
                                        //    w/'\\')
                                        //------------------------------------
        if ( ( CharIsAlpha( lpszItem[0] ) ) && ( lpszItem[1] == ':' ) )
            {
            bHasVolume = TRUE;
            lpszItem = AnsiNext( AnsiNext( lpszItem ) );

                                        //------------------------------------
                                        // Test for special case of
                                        // volume only (e.g., "C:")
                                        //------------------------------------
            if (*lpszItem == EOS )
                return (STATUSEXCLUDE_OK);
            }
        else
            {
            bHasVolume = FALSE;

            if ( bIsUNCName = (BOOL) ( ( lpszItem[0] == '\\' ) &&
                                    ( lpszItem[1] == '\\' ) ) )
                lpszItem = AnsiNext( lpszItem );
            }
        NameSeparatePath( lpszItem, szPathName, szFileName );

                                        //----------------------------------
                                        // If we can not get a name or
                                        // a path then this is not a valid
                                        // exclusion.
                                        //----------------------------------
        if ( !szPathName[0] && !szFileName[0])
            return( STATUSEXCLUDE_ERR );

                                        //------------------------------------
                                        // Grab the illegal char. set to be
                                        // used to test both the path and
                                        // file name(s).
                                        //------------------------------------
        NavReturnIllegalChars( bIsLFN ? SYM_ILLEGAL_WIN95 : SYM_ILLEGAL_DOS,
                               NULL, szIllegalChars,
                               sizeof(szIllegalChars) / sizeof(TCHAR) );

                                        //------------------------------------
                                        // If a file name is specified then
                                        // validate it first
                                        //------------------------------------
        if ( szFileName[0] )
            {
                                        //------------------------------------
                                        // Validate '*' usage. Although WIN95
                                        // allows regular expressions within
                                        // LFN's, we currently do not support
                                        // them in our matching routine.
                                        // Therefore, we force both SFN/LFN's
                                        // to adhere to the same following
                                        // rule(s):
                                        //  - A '*' must be followed by a
                                        //    period or a NULL.
                                        //------------------------------------
            if ( lpTargetChar = STRCHR( szFileName, '*' ) )
                {
                ++lpTargetChar;
                if ( ( *lpTargetChar != '.' ) && ( *lpTargetChar != '\0' ) )
                    return ( STATUSEXCLUDE_ERR );
                }

                                        //------------------------------------
                                        // If Name is entirely dots, then it's
                                        // invalid
                                        //------------------------------------
            if ( IsOnlyDotsSection(szFileName) )
                return ( STATUSEXCLUDE_ERR );

                                        //------------------------------------
                                        // Test for/reject illegal char's in
                                        // the file name.
                                        //------------------------------------
            if ( STRPBRK( szFileName, szIllegalChars ) )
                return( STATUSEXCLUDE_ERR );
                                        //------------------------------------
                                        // The remaining restrictions only
                                        // apply to SFN's:
                                        //  - They can have a max. of 8
                                        //    char.'s in base name
                                        //  - They can have a max. of 3
                                        //    char.'s in the extension
                                        // Both tests are handled w/n
                                        // IsValidSFNSection()
                                        //------------------------------------
            if ( ( !bIsLFN ) && ( !IsValidSFNSection( szFileName ) ) )
                return( STATUSEXCLUDE_ERR );
            }

                                        //------------------------------------
                                        // Now we test the path, if specified
                                        //------------------------------------
        if ( szPathName[0] )
            {
                                        //------------------------------------
                                        // The first char. MUST be a '\' or a
                                        // '/' - we don't support relative
                                        // paths (e.g., TEST\*.*)
                                        //------------------------------------
            if ( ( szPathName[0] != '\\' ) && ( szPathName[0] != '/' ) )
                return( STATUSEXCLUDE_ERR );
                                        //------------------------------------
                                        // Although WIN95 supports wildcards
                                        // within paths, we do not since
                                        // our match engine does not handle it.
                                        // Therefore, if ANY wildcards are found
                                        // embedded in the path, we kick it
                                        // out...
                                        //------------------------------------
            if ( HasWildcards( szPathName ) )
                return( STATUSEXCLUDE_ERR );
                                        //------------------------------------
                                        // Test for/reject illegal char.'s in
                                        // the path name.
                                        //------------------------------------
            if ( STRPBRK( szPathName, szIllegalChars ) )
                return( STATUSEXCLUDE_ERR );
                                        //------------------------------------
                                        // If the name started with '\\' then
                                        // perform the following special
                                        // UNC tests:
                                        // - Make sure that a volume name is
                                        //   specified. A min. spec. would be:
                                        //   \\Server\Volume. Unfortunately,
                                        //   NameSeparatePath() removes the final '\',
                                        //   so it is implied if there is file
                                        //   name. Note that we already
                                        //   skipped over the initial '\' in
                                        //   our bIsUNCName test above.
                                        //------------------------------------
            if ( ( bIsUNCName ) && ( ( !szPathName[1] ) || ( !szFileName[0] ) ) )
                return( STATUSEXCLUDE_ERR );
                                        //------------------------------------
                                        // For all cases, check for section
                                        // with only dots. If this is a SFN,
                                        // then check each section for 8.3
                                        // format.
                                        //------------------------------------
            for ( lpTargetChar = szPathName; lpTargetChar;
                  lpTargetChar = STRCHR( lpTargetChar, '\\' ) )
                {
                    if ( *++lpTargetChar )
                        {
                        if ( IsOnlyDotsSection ( lpTargetChar ) )
                            return (STATUSEXCLUDE_ERR);
                        else if ( !bIsLFN && !IsValidSFNSection( lpTargetChar ) )
                            return (STATUSEXCLUDE_ERR);
                        }
                }
            }  // if( szPathName[0]
        }   // if ( ( lpszItem ) &&...
                                        //------------------------------------
                                        // All tests have passed, give the
                                        // name our blessing...
                                        //------------------------------------
    return( STATUSEXCLUDE_OK );
}


/****************************************************************************
* Function   : IsValidSFNSection - Determines if a section of a SFN is valid.
*
* Description: A section is defined as any level within a path or the actual
*              file name. The tests assure that there nor more than one
*              period and that the max. base name is 8 and ext. is 3.
*
* Inputs     : LPTSTR lpszSection - Pointer to the start of an SFN section
*                                  terminated by either NULL, '\' or '/'
*                                  (This parameter is treated as read only)
*
* Returns    : TRUE  - Section is valid
*              FALSE - Section is invalid
*
****************************************************************************/
BOOL LOCAL IsValidSFNSection( LPTSTR lpszSection )
{
    LPTSTR lpSlash, lpDot;
    int   iBaseNameLen;
    BOOL  bIsValid = TRUE;


                                        //------------------------------------
                                        // Look for/replace '\' with a NULL;
                                        // will undo before the end.
                                        // Look for one and only one period.
                                        // If valid period found then validate
                                        // the max. extension length
                                        //------------------------------------
    if ( lpSlash = STRCHR( lpszSection, '\\' ) )
        *lpSlash = '\0';
    if ( lpDot = STRCHR( lpszSection, '.' ) )
        {
        iBaseNameLen = FP_OFF(lpDot) - FP_OFF(lpszSection);
        if ( ( STRCHR( ++lpDot, '.' ) ) ||
             ( STRLEN( lpDot ) > MAX_SFN_EXTENSION ) )
            bIsValid = FALSE;
        }
        else
            iBaseNameLen = STRLEN( lpszSection );
                                        // Validate base name (!= 0 && <= MAX)
    if ( ( !iBaseNameLen ) || ( iBaseNameLen > MAX_SFN_BASE ) )
        bIsValid = FALSE;
    if ( lpSlash )
        *lpSlash = '\\';
    return( bIsValid );
}



//***************************************************************************
// IsOnlyDotsSection()
//
// Description:
//      Evaluates a string (up to a backslash or EOS) and returns
//      TRUE if it only contains dots. This function is DBCS enabled.
//
//***************************************************************************
// 07/03/1995 BARRY Function Created.
//***************************************************************************

BOOL LOCAL IsOnlyDotsSection ( LPTSTR lpszSection )
{
    while ( *lpszSection && *lpszSection != '\\' )
        {
        if ( *lpszSection != '.' )
            return (FALSE);

        lpszSection = AnsiNext ( lpszSection );
        }

    return (TRUE);
}


//******************************************************************************
//  ExcludeSFN2LFN()
//
// Description:
//      This function finds a short filenames long filename equivalent.
//
// Parameters:
//      lpShortName full path to the short filename to find
//      lpLongName pointer to buffer where the long filename equivalent is placed.
//      uSizeLongName max number of chars that lpLongName can hold
//
// Returns:
//      TRUE if the item was successfully processed. FALSE if the item was
//      determined that it's an invalid short filename. Note that UNC paths are
//      considered as not valid.
//
//******************************************************************************
// Compatibility: Win16, Win32, DOS, VXD
//******************************************************************************
// 06/28/96 DJESCH Modified to call CharPrev (and call it correctly) and fix
//          string boundary checks
// 07/01/96 DJESCH Changed back to AnsiPrev -- oops
//******************************************************************************

BOOL SYM_EXPORT WINAPI ExcludeSFN2LFN (
    LPTSTR       lpShortName,
    LPTSTR       lpLongName,
    UINT        uSizeLongName
    )
{
    SYM_ASSERT ( lpShortName && *lpShortName );
    SYM_ASSERT ( lpLongName );
    SYM_ASSERT ( uSizeLongName );

    auto    BOOL    bCouldConvert = FALSE;

                                        // ----------------------------------
                                        // This check covers most bonehead
                                        // problems.
                                        // ----------------------------------

    if ( lpShortName && lpLongName && uSizeLongName && *lpShortName )
        {
                                        // ----------------------------------
                                        // Find out if there are any wild guys
                                        // and if there are find where file
                                        // item starts.
                                        // ----------------------------------

        LPTSTR lpWildCard = HasWildcards ( lpShortName );

        if ( lpWildCard )
            {
            while ( lpWildCard > lpShortName )
                {
                lpWildCard = AnsiPrev ( lpShortName, lpWildCard);

                if ( *lpWildCard == '\\' )
                    break;
                }

            if ( lpWildCard <= lpShortName )
                lpWildCard = NULL;
            }
                                        // ----------------------------------
                                        // Check for illegal characters.
                                        // ----------------------------------

        TCHAR    szIllegalChars [ SYM_MAX_PATH + 1 ] ;
        BOOL    bValid = TRUE;

        szIllegalChars[0]=EOS;

        NavReturnIllegalChars ( SYM_ILLEGAL_DOS,
                                NULL,
                                szIllegalChars,
                                sizeof(szIllegalChars) / sizeof(TCHAR)
                                );

        LPTSTR lpBad = STRPBRK ( lpShortName, szIllegalChars );


        if ( lpBad )
            {
                                    // ----------------------------------
                                    // If there are wildcard chars and
                                    // the bad guy is somewhere after where
                                    // the wild guys start then add wildcards
                                    // to the list and check again. (cheap!)
                                    // ----------------------------------

            if ( lpWildCard && lpBad >= lpWildCard )
                {
                STRCAT ( szIllegalChars, _T("*?") );

                if ( STRPBRK ( lpBad, szIllegalChars ) )
                    bValid = FALSE;     // Still no good...it's bad
                }
                                    // ----------------------------------
                                    // No wild guys so this entry is not
                                    // valid
                                    // ----------------------------------
            else
                {
                bValid = FALSE;
                }
            }

        if ( bValid == TRUE )
            {
                                    // ----------------------------------
                                    // If second char is ':' then make sure
                                    // the first character is a valid drive
                                    // letter and the third character must
                                    // be a backslash '\'. (leave "C:" alone)
                                    //              OR
                                    // check for UNC path.
                                    // ----------------------------------

            if ( ( CharIsAlpha (lpShortName[0]) && lpShortName[1] == ':' &&
                   lpShortName[2] == '\\' )
                                        ||
                 ( lpShortName[0] == '\\' && lpShortName[1] == '\\' ) )
                {
                TCHAR szShortName [ SYM_MAX_PATH + 1 ] ;
                WORD  wNumBytes, wNumChars;

                                        // Remember, already at start of segment
                if ( lpWildCard )
                    {
                    wNumBytes = lpWildCard - lpShortName;
                    wNumChars = wNumBytes / sizeof(TCHAR);
                    MEMCPY ( szShortName, lpShortName, (int) wNumChars );
                    szShortName[wNumChars] = EOS;
                    }
                else
                    {
                    STRCPY ( szShortName, lpShortName );
                    }

                if ( NavNameReturnLongName ( szShortName, lpLongName,
                                             uSizeLongName ) == NOERR )
                    {

                    if ( lpWildCard )
                        {
                        NameAppendFile ( lpLongName, lpWildCard );
                        }

                    bCouldConvert = TRUE;
                    }
                }
            }
        }

    return ( bCouldConvert );
}

/*@API:*************************************************************************
@Declaration:

BOOL SYM_EXPORT WINAPI ExcludeLFN2SFN (
    LPCTSTR      lpLongName,
    LPTSTR       lpShortName,
    UINT        uSizeShortName
    );

@Description:
This function finds a long filenames short filename equivalent.

@Parameters:
$lpLongName$ full path to the long filename to find
$lpShortName$ pointer to buffer where the short filename equivalent is placed.
$uSizeShortName$ max number of chars that lpShortName can hold

@Returns:
TRUE if the item was successfully processed. FALSE if the item was determined
that it's an invalid long filename. Note that UNC paths are considered as
not valid.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

BOOL SYM_EXPORT WINAPI ExcludeLFN2SFN (
    LPTSTR       lpLongName,
    LPTSTR       lpShortName,
    UINT        uSizeShortName
    )
{
    BOOL bCouldConvert = FALSE;
    TCHAR szIllegalChars [SYM_MAX_PATH + 1];
    TCHAR szPathName     [SYM_MAX_PATH + 1];

    SYM_ASSERT ( lpShortName );
    SYM_ASSERT ( lpLongName );
    SYM_ASSERT ( uSizeShortName );

    if ( *lpLongName )
        {
                                        //------------------------------------
                                        // Kick out name if it contains
                                        // any illegal chars
                                        //------------------------------------
        NavReturnIllegalChars( SYM_ILLEGAL_WIN95, NULL, szIllegalChars,
                                sizeof(szIllegalChars) / sizeof(TCHAR) );
        if ( STRPBRK( lpLongName, szIllegalChars ) )
            return( FALSE );

                                        //------------------------------------
                                        // We will only convert valid, fully
                                        // qualified long names. Therefore,
                                        // the file name must be [Drive]:\...
                                        // or \\... (UNC), otherwise, the name
                                        // is relative and cannot be aliased.
                                        //------------------------------------
        if ( ( ( NameHasDriveSpec( lpLongName ) )  &&
                ( lpLongName[2] == '\\' ) ) ||
                ( NameIsUNC( lpLongName ) ) )
            {
                                        //------------------------------------
                                        // Find the path portion of the LFN.
                                        // Kick out the name if the path
                                        // contains wildcard(s) since we
                                        // currently don't support them
                                        // embedded in paths.
                                        //------------------------------------
            NameReturnPath( lpLongName, szPathName );
            if ( HasWildcards ( szPathName ) )
                return( FALSE );
            if ( NavNameReturnShortName ( lpLongName, lpShortName,
                                            uSizeShortName ) == NOERR )
                {
                                        //------------------------------------
                                        // Force short name to uppercase and
                                        // make sure it is still <= our DOS
                                        // max. SFN length.
                                        //------------------------------------
                if ( STRLEN( STRUPR( lpShortName ) ) < MAX_DOS_PATH )
                    bCouldConvert = TRUE;
                else
                    *lpShortName = '\0';
                }
            }
        }
    return ( bCouldConvert );
}



/*@API:*************************************************************************
@Declaration:

BOOL SYM_EXPORT WINAPI ExcludeCreateCopy (// Create a copy of existing items.
    LPEXCLUDE       lpOriginal )        // [in] original list
    LPEXCLUDE       lpCopy )            // [out] copy of original

@Description:
This function creates a memory copy of an existing exclude object. This is
used in nav options to allow the user to muck around with the exclusion and
still be able to save the user from itself when he presses the cancel button.

@Parameters:
 $lpOriginal$ original exclude object
 $lpCopy$     copy is placed here.

@Returns:
TRUE if the item was successfully copied,
FALSE dudu happened.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

BOOL SYM_EXPORT WINAPI ExcludeCreateCopy (// Create a copy of existing items.
    LPEXCLUDE       lpOriginal,         // [in] original list
    LPEXCLUDE       lpCopy )            // [out] copy of original
{
    SYM_ASSERT ( lpOriginal );
    SYM_ASSERT ( lpCopy );

    MEMSET ( lpCopy, EOS, sizeof(EXCLUDE) );

    if ( lpCopy->lpPrivate =
         (LPEXCLUDEPRIVATE) MemAllocPtr ( GHND, sizeof(EXCLUDEPRIVATE) )
       )
        {
                                        // -----------------------------------
                                        // NOTE: We allocate 1 extra entry.
                                        // This insures we work correctly
                                        // when there are no exclude items.
                                        // -----------------------------------

        DWORD dwBytes = sizeof(EXCLUDEITEM) * ( lpOriginal->wEntries + 1 );

        if ( lpCopy->hExcludeMem = MemAlloc ( GHND, dwBytes ) )
            {
            lpCopy->wEntries = lpOriginal->wEntries;

            if ( lpCopy->lpPrivate->lpExcludeMem =
                 (LPEXCLUDEITEM) MemLock ( lpCopy->hExcludeMem )
               )
                {
                MEMCPY ( lpCopy->lpPrivate->lpExcludeMem,
                         lpOriginal->lpPrivate->lpExcludeMem,
                         (UINT)dwBytes );

                if ( lpCopy->lpPrivate->hLExclude =
                     ExcludeLCreateCopy ( lpOriginal->lpPrivate->hLExclude )
                   )
                    {
                    return ( TRUE );
                    }

                MemRelease ( lpCopy->hExcludeMem );
                }

            MemFree ( lpCopy->hExcludeMem );
            }

        MemFreePtr ( lpCopy->lpPrivate );

        MEMSET ( lpCopy, EOS, sizeof(EXCLUDE) );
        }

    return ( FALSE );
}


/*@API:*************************************************************************
@Declaration:

BOOL SYM_EXPORT WINAPI ExcludeKillCopy (// Zaps a temporary exclude handle
    LPEXCLUDE       lpDoomed )          // [in] kill this

@Description:
This function zaps the copy created with ExcludeCreateCopy().

@Parameters:
 $lpCopy$     copy to kill

@Returns:
Always TRUE - no dudus.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

BOOL SYM_EXPORT WINAPI ExcludeKillCopy (// Zaps a temporary exclude handle
    LPEXCLUDE       lpDoomed )          // [in] kill this
{
    SYM_ASSERT ( lpDoomed );

    if ( lpDoomed )
        {
        if ( lpDoomed->hExcludeMem )
            {
            MemRelease ( lpDoomed->hExcludeMem );
            MemFree    ( lpDoomed->hExcludeMem );
            }

        if ( lpDoomed->lpPrivate->hLExclude )
            {
            ExcludeLKillCopy ( lpDoomed->lpPrivate->hLExclude );
            }

        if ( lpDoomed->lpPrivate )
            {
            MemFreePtr ( lpDoomed->lpPrivate );
            }

        MEMSET ( lpDoomed, EOS, sizeof(EXCLUDE) );
        }

    return ( TRUE );
}

/*@API:*************************************************************************
@Declaration:

BOOL SYM_EXPORT WINAPI ExcludeObjectsIdentical (
    LPEXCLUDE       lpThis,             // [in] compare this
    LPEXCLUDE       lpThat              // [in] with this.
    );

@Description:
This function compares two exclude objects to see if their contents is
identical.

@Parameters:
$lpThis$ compare this object
$lpThat$ with this one.

@Returns:
TRUE if the objects are identical, FALSE otherwise.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

BOOL SYM_EXPORT WINAPI ExcludeObjectsIdentical (
    LPEXCLUDE       lpThis,             // [in] compare this
    LPEXCLUDE       lpThat              // [in] with this.
    )
{
    SYM_ASSERT ( lpThis );
    SYM_ASSERT ( lpThat );

    auto    BOOL    bIdentical = FALSE;

    if ( lpThis && lpThat && lpThis->wEntries == lpThat->wEntries )
        {
        if ( !MEMCMP (
                    lpThis->lpPrivate->lpExcludeMem,
                    lpThat->lpPrivate->lpExcludeMem,
                    lpThis->wEntries * sizeof(EXCLUDEITEM) ) )
            {
            if ( ExcludeLObjectsIdentical (
                                    lpThis->lpPrivate->hLExclude,
                                    lpThat->lpPrivate->hLExclude ) ==
                                    STATUSEXCLUDE_OK )
                {
                bIdentical = TRUE;
                }
            }
        }

    return ( bIdentical );
}


// --------------------------------------------------------------------------
//                          LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------


/*@API:*************************************************************************
@Declaration:

LPTSTR LOCAL HasWildcards (              // Checks for wildcard chars
    LPTSTR   lpString );                 // [in] withing this string.

@Description:
This function searches the given string for any wildcard characters.

@Parameters:
$lpString$ string to search for wildcard characters.

@Returns:
pointer to the first wildcard character. NULL if there's no wildcard character.

@Compatibility: Win16, Win32, DOS, VXD
*******************************************************************************/

LPTSTR LOCAL HasWildcards (              // Checks for wildcard chars
    LPCTSTR  lpString )                  // [in] within this string.
{
    SYM_ASSERT ( lpString );

    return STRPBRK(lpString, _T("*?") );
}

/*@API:*************************************************************************
@Declaration:

STATUS LOCAL NavNameReturnName (          // Convert from SFN to LFN or vice versa
    LPTSTR   lpszSourceName,             // [in] convert from here
    LPTSTR   lpszTargetName,             // [out] and place result here
    BOOL    bFromShortToLong            // [in] flag indicating conv. order
    )

@Description:
This function is cover for the Quake NameReturnLongName() and
NameReturnShortName() functions. A cover is required to filter out filespecs
that are valid for exclusions but the Quake functions dislike.

@Parameters:
$lpszSourceName$    pointer to source name to convert
$lpszTargetName$    pointer to output buffer.
$bFromShortToLong$  flag to convert from SFN to LFN or vice versa.

@Returns:
Nothing

@Compatibility: Win32
*******************************************************************************/

STATUS LOCAL NavNameReturnName (        // Convert from SFN to LFN or vice versa
    LPTSTR   lpszSourceName,             // [in] convert from here
    LPTSTR   lpszTargetName,             // [out] and place result here
    UINT    uTargetSize,                // [in] max size of target
    BOOL    bFromShortToLong            // [in] flag indicating conv. order
    )
{
    auto    TCHAR    szSource      [ SYM_MAX_PATH + 1 ] ;
    auto    TCHAR    szWildPortion [ SYM_MAX_PATH + 1 ] ;
    auto    int     iStrLen     = 0;    // Remembers length of various strs
    auto    BOOL    bEndSlash   =   FALSE;
    auto    STATUS  Status      =   NOERR;


    szSource[0] = szWildPortion[0] = EOS;

    SYM_ASSERT ( lpszSourceName && *lpszSourceName );
    SYM_ASSERT ( lpszTargetName );

    STRCPY ( szSource, lpszSourceName );

                                        // If there are wildcards,
                                        // separate. Assume already
                                        // validated for wildcards in path.

    NameReturnFile(szSource, szWildPortion);

    if ( HasWildcards(szWildPortion) )
    {
                                        //------------------------------------
                                        // If the name contains wildcards AND
                                        // we are converting from long to
                                        // short, then the name must be a
                                        // valid DOS 8.3 SFN in order to
                                        // convert it, else fail.
                                        //------------------------------------
        if ( ( !bFromShortToLong ) &&
             ( ExcludeIsValidSFN( szWildPortion ) != STATUSEXCLUDE_OK ) )
            return( ERR );
        else
            NameStripFile(szSource);
    }
    else
        szWildPortion[0] = EOS;         // No wild portion


                                        // If ending in a backslash, remove it
                                        // (note that wildcard removal only
                                        // saved filename portion)
    iStrLen = STRLEN ( szSource );

                                        // Kick out if path portion too long
    if ( iStrLen >= ( SYM_MAX_PATH - 14 ) )
        return ( ERR );

    if ( iStrLen && szSource [ iStrLen - 1 ] == '\\' )
        {
        szSource[ --iStrLen ] = EOS;
        bEndSlash = TRUE;
        }

                                        // Handle drive by itself (e.g., "C:")
                                        // Note that UNC won't be an issue
    if ( iStrLen == 2 && szSource[1] == ':' )
        {
                                        // Make drive name upper case always
        *szSource = CharToUpper(*szSource);
        STRCPY ( lpszTargetName, szSource );
        }
    else
        {
        if ( bFromShortToLong )
            Status = NameReturnLongName ( szSource, lpszTargetName, FALSE );
        else
            Status = NameReturnShortName ( szSource, lpszTargetName );
        }

                                        // Recombine wildcards
    if ( Status == NOERR )
        {
        iStrLen = STRLEN ( lpszTargetName );

        if ( bEndSlash )
            {
            lpszTargetName[iStrLen++] = '\\';

            if ( iStrLen >= uTargetSize )
                Status = ERR;
            else
                lpszTargetName[iStrLen] = EOS;
            }

                                        // This whole backslash handling
                                        // is a little sloppy since if
                                        // we stripped the file, there
                                        // won't be an ending backslash
                                        // but at least it handles the
                                        // "C:\" and "C:\DIR\" cases which,
                                        // I believe, NameReturn...() doesn't
        if ( *szWildPortion )
            {
            if ( iStrLen + STRLEN(szWildPortion) + 1 >= uTargetSize )
                Status = ERR ;
            else
                NameAppendFile ( lpszTargetName, szWildPortion );
            }
        }

    if ( Status != NOERR )
        lpszTargetName[0] = EOS;

    return ( Status );
}

