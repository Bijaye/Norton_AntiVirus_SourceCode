// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEXCLU/VCS/exclmtch.cpv   1.4   08 Aug 1997 14:59:48   MKEATIN  $
//
// Description:
//      NAV match function for exclusions.
//
// Contains:
//      NAVExcludeMatch()
//
// See Also:
//************************************************************************

#include "platform.h"
#include "avexcl.h"
#include "fname.h"


//************************************************************************
// Local helper prototypes

static LPTSTR GetVolume ( LPTSTR lpszVolume, LPCTSTR lpszPath );
static LPTSTR GetDirectory ( LPTSTR lpszDir, LPCTSTR lpszPath );
static LPTSTR GetFile ( LPTSTR lpszFile, LPCTSTR lpszPath );

static BOOL ExcludeDirectoryMatch ( LPCTSTR lpszExclude,
                                    LPCTSTR lpszLong,
                                    LPCTSTR lpszShort,
                                    BOOL    bSubDirs );

static BOOL ExcludeNameMatch ( LPCTSTR lpszExclude,
                               LPCTSTR lpszLong,
                               LPCTSTR lpszShort,
                               BOOL    bSubDirs );


//************************************************************************
// ExcludeNameMatch()
//
// This routine will compare a full filename and activity flags versus
// a single exclusion specification.
// If LFN or SFN is not available, either lpszLongPath or lpszShortPath
// may be NULL, but not both.
//
// Parameters:
//  LPEXCLUDEITEM lpItem    Exclusion specification.
//  LPCTSTR lpszLongPath    Full LFN path to check vs lpszExclude. May be NULL.
//  LPCTSTR lpszShortPath   Full SFN path to check vs lpszExclude. May be NULL.
//  WORD    wFlags          Type of activity to match vs. exclusion item.
//
// Returns:
//  TRUE    if file and flags match exclusion.
//  FALSE   otherwise.
//************************************************************************
// 05/07/1997 DALLEE, created.
//************************************************************************

BOOL SYM_EXPORT NavExcludeCheck (
    LPEXCLUDEITEM   lpItem,
    LPTSTR          lpszShortPath,
    LPTSTR          lpszLongPath,
    WORD            wFlags
    )
{
    SYM_ASSERT ( lpItem );
    SYM_ASSERT ( lpszShortPath || lpszLongPath );

    return ( ( ( lpItem->wBits & wFlags ) == wFlags )
                &&
             ExcludeNameMatch( lpItem->szText,
                               lpszLongPath,
                               lpszShortPath,
                               lpItem->bSubDirectory ) );
} // NavExcludeCheck()


//************************************************************************
// ExcludeNameMatch()
//
// This routine will compare a full filename versus a wildcard
// exclusions specification.
// If LFN or SFN is not available, either lpszLong or lpszShort may be NULL,
// but not both.
//
// Parameters:
//  LPCTSTR lpszExclude     Wildcard exclusion path specification.
//  LPCTSTR lpszLong        Full LFN path to check vs lpszExclude. May be NULL.
//  LPCTSTR lpszShort       Full SFN path to check vs lpszExclude. May be NULL.
//  BOOL    bSubDirs        Whether files in subdirectories of lpszExclude
//                          should also be excluded.
//
// Returns:
//  TRUE    if filename matches lpszExclude.
//  FALSE   otherwise.
//************************************************************************
// 05/07/1997 DALLEE, created.
//************************************************************************

static BOOL ExcludeNameMatch ( LPCTSTR lpszExclude,
                               LPCTSTR lpszLong,
                               LPCTSTR lpszShort,
                               BOOL    bSubDirs )
{
    auto    LPTSTR  lpszLowerExclude;
    auto    LPTSTR  lpszLowerLong;
    auto    LPTSTR  lpszLowerShort;

    auto    BOOL    bMatch;

    // Must have exclusion specification and at least one of
    // lpszLong and lpszShort.

    SYM_ASSERT( lpszExclude );
    SYM_ASSERT( lpszLong || lpszShort );

    // Initialize locals.

    lpszLowerExclude = \
    lpszLowerLong    = \
    lpszLowerShort   = NULL;

    bMatch = FALSE;

    // This is illegal and will be caught by the SYM_ASSERTion, but handle double
    // NULL's for filenames at runtime as well since I can't track everyone
    // calling this function.

    if ( ( NULL == lpszLong ) && ( NULL == lpszShort ) )
    {
        goto BailOut;
    }

    // Allocate scratch buffers for filenames.

    lpszLowerExclude = (LPTSTR) MemAllocPtr( GMEM_MOVEABLE, ( STRLEN( lpszExclude ) + 1 ) * sizeof( TCHAR ) );

    if ( lpszLong )
        lpszLowerLong = (LPTSTR) MemAllocPtr( GMEM_MOVEABLE, ( STRLEN( lpszLong ) + 1 ) * sizeof( TCHAR ) );

    if ( lpszShort )
        lpszLowerShort = (LPTSTR) MemAllocPtr( GMEM_MOVEABLE, ( STRLEN( lpszShort ) + 1 ) * sizeof( TCHAR ) );

    if ( ( NULL == lpszLowerExclude ) ||
         ( lpszLong  && ( NULL == lpszLowerLong ) ) ||
         ( lpszShort && ( NULL == lpszLowerShort ) ) )
    {
        // Memory allocation error. Just treating this as no match.

        goto BailOut;
    }

    //********************************************************************
    // Match volumes.

    GetVolume( lpszLowerExclude, lpszExclude );
    FileNameToLower( lpszLowerExclude );

    // Only need to check one volume -- SFN and LFN should be the same.

    if ( lpszLong )
    {
        // Use LFN, if available.

        GetVolume( lpszLowerLong, lpszLong );
        FileNameToLower( lpszLowerLong );
    }
    else
    {
        // Else -- SFN.

        GetVolume( lpszLowerShort, lpszShort );
        FileNameToLower( lpszLowerShort );
    }

    if ( lpszLong )
    {
        if ( ( *lpszLowerExclude ) &&
             ( 0 != STRCMP( lpszLowerExclude, lpszLowerLong ) ) )
        {
            // Volumes didn't match.

            goto BailOut;
        }
    }
    else
    {
        if ( ( *lpszLowerExclude ) &&
             ( 0 != STRCMP( lpszLowerExclude, lpszLowerShort ) ) )
        {
            // Volumes didn't match.

            goto BailOut;
        }
    }

    //********************************************************************
    // Match directories.

    if ( lpszLong )
    {
        GetDirectory( lpszLowerLong, lpszLong );
        FileNameToLower( lpszLowerLong );
    }

    if ( lpszShort )
    {
        GetDirectory( lpszLowerShort, lpszShort );
        FileNameToLower( lpszLowerShort );
    }

    // Some klugery here:
    // NAV's exclusion specifications are slightly ambiguous. The last
    // section can count as either part of the directory, or as
    // a filename specification.

    if ( NULL == STRPBRK( lpszExclude, _T("*?") ) )
    {
        // If the last section has no wildcards, first assume it is part
        // of the directory, with the implied filename specification of "*.*".

        STRCPY( lpszLowerExclude, FileNameSkipVolume( lpszExclude ) );
        FileNameToLower( lpszLowerExclude );

        if ( ( *lpszLowerExclude ) &&
             ( ExcludeDirectoryMatch( lpszLowerExclude,
                                      lpszLowerLong,
                                      lpszLowerShort,
                                      bSubDirs ) ) )
        {
            // We had a match on the directory. So with the implied
            // file specification of "*.*", the entire name is a match.

            bMatch = TRUE;
            goto BailOut;
        }
    }

    GetDirectory( lpszLowerExclude, lpszExclude );
    FileNameToLower( lpszLowerExclude );

    if ( ( *lpszLowerExclude ) &&
         ( FALSE == ExcludeDirectoryMatch( lpszLowerExclude,
                                           lpszLowerLong,
                                           lpszLowerShort,
                                           bSubDirs ) ) )
    {
        // Directories didn't match.

        goto BailOut;
    }

    //********************************************************************
    // Match filenames.

    GetFile( lpszLowerExclude, lpszExclude );

    if ( lpszLong )
        GetFile( lpszLowerLong, lpszLong );

    if ( lpszShort )
        GetFile( lpszLowerShort, lpszShort );

    // No need to convert case; FileNameMatchSection() handles mixed case.

    if ( ( EOS == *lpszLowerExclude ) ||
         ( lpszLong  && FileNameMatchSection( lpszLowerExclude, lpszLowerLong ) ) ||
         ( lpszShort && FileNameMatchSection( lpszLowerExclude, lpszLowerShort ) ) )
    {
        // Filenames match.

        bMatch = TRUE;
    }

BailOut:
    if ( lpszLowerExclude )
        MemFreePtr( lpszLowerExclude );

    if ( lpszLowerLong )
        MemFreePtr( lpszLowerLong );

    if ( lpszLowerShort )
        MemFreePtr( lpszLowerShort );

    return ( bMatch );
} // ExcludeNameMatch()


//************************************************************************
// ExcludeDirectoryMatch()
//
// This routine compares the directory portions of the filename versus
// the directory portion of the exclusion path.
//
// All input filenames are assumed to be already converted to the same case.
// If LFN or SFN is not available, either lpszLong or lpszShort may be NULL,
// but not both.
//
// Parameters:
//  LPCTSTR lpszExclude     Exclusion directory specification.
//  LPCTSTR lpszLong        LFN directory to check vs lpszExclude. May be NULL.
//  LPCTSTR lpszShort       SFN directory to check vs lpszExclude. May be NULL.
//  BOOL    bSubDirs        Whether subdirectories of lpszExclude match.
//
// Returns:
//  TRUE    if directory matches lpszExclude.
//  FALSE   otherwise.
//************************************************************************
// 05/07/1997 DALLEE, created.
//************************************************************************

static BOOL ExcludeDirectoryMatch ( LPCTSTR lpszExclude,
                                    LPCTSTR lpszLong,
                                    LPCTSTR lpszShort,
                                    BOOL    bSubDirs )
{
    auto    LPCTSTR     lpszNextExclude;
    auto    LPCTSTR     lpszNextLong;
    auto    LPCTSTR     lpszNextShort;

    auto    LPCTSTR     lpszSingleDir;

    auto    int         nExcludeLength;

    // Must have exclusion specification and at least one of
    // lpszLong and lpszShort.

    SYM_ASSERT( lpszExclude );
    SYM_ASSERT( lpszLong || lpszShort );

    // Check whether all directories specified in lpszExclude
    // are matched by either lpszLong or lpszShort
    //
    // We have three possible plans:
    // The Cheap Way - if lpszExclude is the root dir, then we match so far.
    // The Hard Way  - try match with both LFN and SFN directory sections.
    // The Easy Way  - optimize for a single directory input.

    if ( ( _T('\\') == lpszExclude[0] ) &&
         ( EOS == lpszExclude[1] ) )
    {
        // Cheap Way - Scheme:
        // Advance filename directories past initial '\' and call this
        // a match so far.
        // Handling the special case for root directories here makes
        // the logic for the next two ways a little simpler.

        if ( lpszLong && _T('\\') == *lpszLong )
            lpszLong++;

        if ( lpszShort && _T('\\') == *lpszShort )
            lpszShort++;
    }
    else if ( lpszLong && lpszShort )
    {
        // Hard Way - Scheme:
        // While we have sections of the exclusion directory, check
        // each section versus the both the corresponding section of the
        // LFN and SFN directories.

        while ( *lpszExclude )
        {
            // Save start of next section for each directory and get
            // length of current exclude directory section.

            lpszNextExclude = FileNameSkipSection( lpszExclude );
            lpszNextLong    = FileNameSkipSection( lpszLong );
            lpszNextShort   = FileNameSkipSection( lpszShort );

            nExcludeLength  = lpszNextExclude - lpszExclude;

            // Check versus this section of long directory and short directory.

            if ( ( nExcludeLength == ( lpszNextLong - lpszLong ) ) &&
                 ( 0 == STRNCMP( lpszExclude, lpszLong, nExcludeLength ) ) )
            {
                // Long names matched.
            }
            else if ( ( nExcludeLength == ( lpszNextShort - lpszShort ) ) &&
                      ( 0 == STRNCMP( lpszExclude, lpszShort, nExcludeLength ) ) )
            {
                // Short names matched.
            }
            else
            {
                // Directories don't match.

                return ( FALSE );
            }

            // Advance to next section.

            lpszExclude = lpszNextExclude;
            lpszLong    = lpszNextLong;
            lpszShort   = lpszNextShort;
        }
    }
    else
    {
        // Easy Way - Scheme:
        // Do strncmp between lpszExclude and single input.
        // Also ensure that next character in single input is
        // either EOS or '\' so we don't match "\abc" with "\abcd".

        lpszSingleDir = lpszLong ? lpszLong : lpszShort;

        nExcludeLength = STRLEN( lpszExclude );

        if ( ( 0 != STRNCMP( lpszExclude, lpszSingleDir, nExcludeLength ) ||
             ( lpszSingleDir[ nExcludeLength ] != EOS &&
               lpszSingleDir[ nExcludeLength ] != _T('\\') ) ) )
        {
            // Directory didn't match.

            return ( FALSE );
        }
    }

    // At this point, all exclusion directory segments have been matched
    // by either LFN or SFN segments.

    if ( !bSubDirs )
    {
        // If subdirs are not included, we only have a match if this is the
        // end of the filename directories as well as the exclude directory.

        return ( ( lpszLong  && ( EOS == *lpszLong ) ) ||
                 ( lpszShort && ( EOS == *lpszShort ) ) );
    }
    else
    {
        // If subdirectories are included, then we have a match.

        return ( TRUE );
    }
} // ExcludeDirectoryMatch()


//************************************************************************
// GetVolume()
//
// This routine retrieves the volume portion of a path.
// If the path does not contain a volume identifier, an empty string
// will be returned.
//
// Parameters:
//  LPTSTR  lpszVolume      [out] Buffer to receive volume.
//  LPCTSTR lpszPath        [in] Path with volume to retrieve.
//
// Returns:
//  lpszVolume
//************************************************************************
// 05/08/1997 DALLEE, created.
//************************************************************************

static LPTSTR GetVolume ( LPTSTR lpszVolume, LPCTSTR lpszPath )
{
    auto    int     nVolumeLength;

    SYM_ASSERT( lpszVolume );
    SYM_ASSERT( lpszPath );

    nVolumeLength = FileNameSkipVolume( lpszPath ) - lpszPath;

    STRNCPY( lpszVolume, lpszPath, nVolumeLength );
    lpszVolume[ nVolumeLength ] = EOS;

    return ( lpszVolume );
} // GetVolume()


//************************************************************************
// GetDirectory()
//
// This routine retrieves the directory portion of a path.
// If the path does not contain a directory, an empty string
// will be returned.
//
// Parameters:
//  LPTSTR  lpszDirectory   [out] Buffer to receive directory.
//  LPCTSTR lpszPath        [in] Path with directory to retrieve.
//
// Returns:
//  lpszDirectory
//************************************************************************
// 05/08/1997 DALLEE, created.
//************************************************************************

static LPTSTR GetDirectory ( LPTSTR lpszDirectory, LPCTSTR lpszPath )
{
    auto    LPCTSTR lpszDirectoryStart;
    auto    int     nDirectoryLength;

    SYM_ASSERT( lpszDirectory );
    SYM_ASSERT( lpszPath );

    lpszDirectoryStart = FileNameSkipVolume( lpszPath );
    nDirectoryLength   = FileNameSkipDirectory( lpszPath ) - lpszDirectoryStart;

    STRNCPY( lpszDirectory, lpszDirectoryStart, nDirectoryLength );
    lpszDirectory[ nDirectoryLength ] = EOS;

    return ( lpszDirectory );
} // GetDirectory()


//************************************************************************
// GetFile()
//
// This routine retrieves the filename portion of a path.
// If the path does not contain a filename portion, an empty string
// will be returned.
//
// Parameters:
//  LPTSTR  lpszFile        [out] Buffer to receive filename.
//  LPCTSTR lpszPath        [in] Path with filename to retrieve.
//
// Returns:
//  lpszFile
//************************************************************************
// 05/08/1997 DALLEE, created.
//************************************************************************

static LPTSTR GetFile ( LPTSTR lpszFile, LPCTSTR lpszPath )
{
    auto    LPCTSTR lpszFileStart;

    SYM_ASSERT( lpszFile );
    SYM_ASSERT( lpszPath );

    lpszFileStart = FileNameSkipDirectory( lpszPath );

    // FileNameSkipDirectory() returns pointer to last '\', if any.
    // We may need to skip past this.

    if ( _T('\\') == *lpszFileStart )
    {
        lpszFileStart++;
    }

    STRCPY( lpszFile, lpszFileStart );

    return ( lpszFile );
} // GetFile()

