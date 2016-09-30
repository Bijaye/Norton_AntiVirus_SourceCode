// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWUTIL/VCS/Apwname.cpv   1.0   02 Jun 1997 02:48:16   DALLEE  $
//
// Description:
//
// Contains:
//      ApwNameStrip()
//      ApwNameAppendSeparator()
//      ApwNameCat()
//      ApwNameSkipVolume()
//      ApwNameSkipDirectory()
//      ApwNameSkipSection()
//      ApwNameToLower()
//      GetLongPathName()
//
// See Also:
//************************************************************************
// $Log:   S:/APWUTIL/VCS/Apwname.cpv  $
// 
//    Rev 1.0   02 Jun 1997 02:48:16   DALLEE
// Initial revision.
//************************************************************************

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>

#include "apwutil.h"

bool _InitResources();


//************************************************************************
// Filename manipulation functions

//************************************************************************
// ApwNameStrip()
//
// This routine removes the filename portion (last '\' delimited section)
// of a path.
//
// Parameters:
//  LPTSTR  lpszPath        Path with filename to remove.
//
// Returns:
//  Pointer to lpszPath
//************************************************************************
// 04/18/1997 DALLEE, created.
//************************************************************************

LPTSTR WINAPI ApwNameStrip( LPTSTR lpszPath )
{
	_InitResources();

	assert( lpszPath );

    *ApwNameSkipDirectory( lpszPath ) = EOS;

    return ( lpszPath );
} // ApwNameStrip()


//************************************************************************
// ApwNameAppendSeparator()
//
// This routine ensures a filename is terminated by a ':' or '\' by
// appending a '\' if necessary.
//
// Parameters:
//  LPTSTR  lpszPath
//
// Returns:
//  lpszPath
//************************************************************************
// 04/18/1997 DALLEE, created.
//************************************************************************

LPTSTR WINAPI ApwNameAppendSeparator( LPTSTR lpszPath )
{
	_InitResources();

    auto    LPTSTR  lpszEnd;
    auto    LPTSTR  lpszLastChar;

    assert( lpszPath );

    lpszEnd      = lpszPath;
    lpszLastChar = NULL;

    while ( *lpszEnd )
    {
        lpszLastChar = lpszEnd;
        lpszEnd = CharNext( lpszEnd );
    }

    if ( ( NULL != lpszLastChar ) &&
         ( _T('\\') != *lpszLastChar ) &&
         ( _T(':') != *lpszLastChar ) )
    {
        lpszEnd[0] = _T('\\');
        lpszEnd[1] = EOS;
    }

    return ( lpszPath );
} // ApwNameAppendSeparator()


//************************************************************************
// ApwNameCat()
//
// This routine appends a filename to a path.
//
// Parameters:
//  LPTSTR  lpszPath        Path to modify.
//  LPCTSTR lpszFileName    Filename to append to lpszPath.
//
// Returns:
//  lpszPath
//************************************************************************
// 04/18/1997 DALLEE, created.
//************************************************************************

LPTSTR WINAPI ApwNameCat( LPTSTR lpszPath, LPCTSTR lpszFileName )
{
	_InitResources();

    assert( lpszPath );
    assert( lpszFileName );

    ApwNameAppendSeparator( lpszPath );

    if ( _T('\\') == *lpszFileName )
        lpszFileName++;

    _tcscat( lpszPath, lpszFileName );

    return ( lpszPath );
} // ApwNameCat()


//************************************************************************
// FilenameSkipVolume()
//
// This routine skips past the volume portion of a filespec.
//
// Parameters:
//  LPCTSTR     lpszPath        Path with volume to skip.
//
// Returns:
//  A pointer to the first character after the volume identifier -- either
//      UNC server/volume, or logical drive specification. This will be
//      the address of the leading backslash if it is a full path spec.
//  If the path consists of only a volume identifier, a pointer to the
//      end of the path will be returned -- an empty string.
//  If a volume identifier is not found, lpszPath is returned.
//************************************************************************
// 04/18/1997 DALLEE, created.
//************************************************************************

LPTSTR WINAPI ApwNameSkipVolume( LPCTSTR lpszPath )
{
	_InitResources();

   auto    int     i;

    assert( lpszPath );

    // UNC volume
    if ( ( _T('\\') == lpszPath[0] ) &&
         ( _T('\\') == lpszPath[1] ) )
    {
        lpszPath += 2;

        for ( i = 0; *lpszPath; lpszPath = CharNext( lpszPath ) )
        {
            if ( _T('\\') == *lpszPath )
            {
                i++;

                if ( i >= 2 )
                    break;
            }
        }
    }
    // Drive letter volume
    else if ( _T(':') == *CharNext( lpszPath ) )
    {
        lpszPath = CharNext( lpszPath ) + 1;
    }
    // Else unknown, return start of path

    return ( (LPTSTR)lpszPath );
} // ApwNameSkipVolume()


//************************************************************************
// ApwNameSkipDirectory()
//
// This routine skips past the volume and directory portions of a path.
// It is assumed the last path section denotes a filename -- no lookup or
// validation of the path is performed.
//
// Parameters:
//  LPCTSTR     lpszPath        Path with directory to skip.
//
// Returns:
//  A pointer to the first character after the volume and directory portions
//      of a path. If the volume/directory is separated from the filename
//      by a '\', this routine will return a pointer to the '\'.
//  If neither a volume or directory is found, lpszPath is returned.
//************************************************************************
// 04/18/1997 DALLEE, created.
//************************************************************************

LPTSTR WINAPI ApwNameSkipDirectory( LPCTSTR lpszPath )
{
	_InitResources();

   auto    LPCTSTR  lpszFile;

    assert( lpszPath );

    lpszPath = ApwNameSkipVolume( lpszPath );

    if ( _T('\\') == *lpszPath )
        lpszPath++;

    for ( lpszFile = lpszPath;
          *lpszPath;
          lpszPath = CharNext( lpszPath ) )
    {
        if ( _T('\\') == *lpszPath )
            lpszFile = lpszPath;
    }

    return ( (LPTSTR)lpszFile );
} // ApwNameSkipDirectory()


//************************************************************************
// ApwNameSkipSection()
//
// This routine will skip to the next path section in a filename.
//
// Parameters:
//  LPCTSTR lpszPath        Path with section to skip.
//
// Returns:
//  A pointer to the next section in a path.
//  If the path has a volume, a pointer to the beginning of the directory
//      will be returned.
//  Otherwise, a pointer to the '\' delimiting the next path segment will
//      be returned.
//  If there is only one path element, a pointer to the end of the path
//      will be returned -- an empty string.
//************************************************************************
// 04/18/1997 DALLEE, created.
//************************************************************************

LPTSTR WINAPI ApwNameSkipSection( LPCTSTR lpszPath )
{
	_InitResources();

   auto    LPCTSTR lpszNextSection;

    assert( lpszPath );

    lpszNextSection = ApwNameSkipVolume( lpszPath );

    if ( lpszNextSection != lpszPath )
    {
        // Had volume, return first section after that.
    }
    else
    {
        // Skip past leading backslashes in this section.
        if ( _T('\\') == *lpszNextSection )
            lpszNextSection++;

        // Return start of next '\\' delimited section, or EOS.
        while ( *lpszNextSection &&
                ( _T('\\') != *lpszNextSection ) )
            lpszNextSection++;
    }

    return ( (LPTSTR)lpszNextSection );
} // ApwNameSkipSection()


//************************************************************************
// ApwNameToLower()
//
// This routine translates a filename to lowercase. Double-byte characters
// are not changed as upper and lower case double-byte characters are
// not considered equivalent when comparing filenames.
//
// Parameters:
//  LPTSTR  lpszFile        Filename to convert to lowercase.
//
// Returns:
//  lpszFile
//************************************************************************
// 04/18/1997 DALLEE, created.
//************************************************************************

LPTSTR WINAPI ApwNameToLower ( LPTSTR lpszFile )
{
	_InitResources();

#ifdef _MBCS
    // Filenames are case-sensitive for double-byte characters.
    // only convert non-DBCS characters.
    //&? How do these characters behave under UNICODE?

    auto    LPTSTR  lpchCurrent;

    for ( lpchCurrent = lpszFile;
          EOS != *lpchCurrent;
          lpchCurrent = CharNext( lpchCurrent ) )
    {
        if ( FALSE == IsDBCSLeadByte( *lpchCurrent ) )
        {
            *lpchCurrent = (TCHAR)CharLower( (LPTSTR)*lpchCurrent );
        }
    }
#else
    CharLower( lpszFile );
#endif

    return ( lpszFile );
} // ApwNameToLower()


//************************************************************************
// GetLongPathName()
//
// This function obtains the LFN form of the specified path.
// lpszShort and lpszLong may point to the same buffer.
// This function may be called with cchBuffer == 0 and lpszLong == NULL
// to determine the size of the buffer required for lpszLong.
//
// Parameters:
//  LPCTSTR lpszShort       [in] Input path string.
//  LPTSTR  lpszLong        [out] buffer to receive LFN.
//  DWORD   cchBuffer       [in] size of buffer lpszLong.
//
// Returns:
//  On success:
//      The number of characters copied to lpszLong excluding EOS.
//  On failure:
//      If the buffer is too small, the return value is the size of the
//      buffer required in characters including the EOS.
//      The return value is 0 for all other errors.
//************************************************************************
// 04/18/1997 DALLEE, created.
//************************************************************************
/*
DWORD WINAPI GetLongPathName( LPCTSTR   lpszShort,
                              LPTSTR    lpszLong,
                              DWORD     cchBuffer )
{
    auto    LPTSTR  lpszOut;
    auto    LPTSTR  lpszScratch;

    auto    LPCTSTR lpszNextSection;
    auto    LPCTSTR lpszThisSection;

    auto    UINT    uInputSize;
    auto    UINT    uScratchSize;
    auto    BOOL    bOverrun;
    auto    DWORD   dwChars;

    auto    WIN32_FIND_DATA rFind;
    auto    HANDLE          hFind;

    // Should always have lpszShort. Must have lpszLong, if cchBuffer != 0.

    assert( lpszShort );
    assert( cchBuffer ? NULL != lpszLong : TRUE );

    // Initialize locals.

    lpszOut     = \
    lpszScratch = NULL;
    dwChars     = 0;
    bOverrun    = FALSE;

    // Allocate buffers for final output and scratch for FindFirst lookups.

    uInputSize   = ( _tcslen( lpszShort ) + 1 ) * sizeof( TCHAR );
    uScratchSize = max ( uInputSize, cchBuffer );

    lpszOut     = (LPTSTR)GlobalAllocPtr( GMEM_MOVEABLE, uScratchSize );
    lpszScratch = (LPTSTR)GlobalAllocPtr( GMEM_MOVEABLE, uScratchSize );

    if ( ( NULL == lpszOut ) ||
         ( NULL == lpszScratch ) )
        goto BailOut;

    // Copy volume name into output and scratch buffers.
    // Include trailing '\\' if present.

    lpszNextSection = ApwNameSkipVolume( lpszShort );
    if ( _T('\\') == *lpszNextSection )
        lpszNextSection++;

    dwChars = lpszNextSection - lpszShort;

    _tcsncpy( lpszOut, lpszShort, dwChars );
    lpszOut[ dwChars ] = EOS;

    dwChars++; // Count includes EOS

    _tcscpy( lpszScratch, lpszOut );

    // While sections...

    while ( lpszThisSection = lpszNextSection,
            *lpszThisSection )
    {
        // Copy section to scratch buffer.

        lpszNextSection = ApwNameSkipSection( lpszThisSection );

        if ( _T('\\') == *lpszThisSection )
            lpszThisSection++;

        ApwNameAppendSeparator( lpszScratch );
        _tcsncat( lpszScratch,
                  lpszThisSection,
                  lpszNextSection - lpszThisSection );

        // Find long name of section.

        hFind = FindFirstFile( lpszScratch, &rFind );

        if ( INVALID_HANDLE_VALUE != hFind )
        {
            // Calculate length and add to output buffer.

            if ( ( FALSE == bOverrun ) &&
                 ( dwChars < uScratchSize ) )
            {
                ApwNameAppendSeparator( lpszOut );
                dwChars = 1 + _tcslen( lpszOut );
            }
            else
            {
                dwChars++;
            }

            dwChars += _tcslen( rFind.cFileName );

            bOverrun = dwChars > cchBuffer;

            if ( FALSE == bOverrun )
            {
                _tcscat( lpszOut, rFind.cFileName );
            }

            FindClose( hFind );
        }
        else
        {
            dwChars = 0;
            goto BailOut;
        }
    }

    // Copy to output buffer, or set error for insufficient buffer.

    if ( FALSE == bOverrun )
    {
        _tcscpy( lpszLong, lpszOut );
    }
    else
    {
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
    }

BailOut:
    if ( NULL != lpszOut )
        GlobalFreePtr( lpszOut );

    if ( NULL != lpszScratch )
        GlobalFreePtr( lpszScratch );

    // On success, return string length not including EOS.
    // On overrun, we should return required buffer size including EOS.

    if ( ( dwChars > 0 ) && ( FALSE == bOverrun ) )
        dwChars--;

    return ( dwChars );
} // GetLongPathName()
*/
