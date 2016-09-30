// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVAPI1/VCS/fname.cpv   1.3   11 Sep 1997 17:44:22   KSACKIN  $
//
// Description:
//      File name routines used by new directory iterator
//      and some exclusions filename matching code.
//
// Contains:
//      FileNameStrip()
//      FileNameAppendSeparator()
//      FileNameCat()
//      FileNameSkipVolume()
//      FileNameSkipDirectory()
//      FileNameSkipSection()
//      FileNameToLower()
//      FileNameGetLongPathName()
//      FileNameGetShortPathName()
//
// See Also:
//************************************************************************
// $Log:   S:/AVAPI1/VCS/fname.cpv  $
// 
//    Rev 1.3   11 Sep 1997 17:44:22   KSACKIN
// Ported changes from QAKG.
// 
//    Rev 1.2.1.2   02 Sep 1997 13:43:36   DALLEE
// Added FileNameGetShortPathName() which operates as GetShortPathName(),
// but accepts OEM input when file API's are in OEM mode.
// Basically a cut and paste of the GetLongPathName() implementation already
// here.
// 
//    Rev 1.2.1.1   24 Aug 1997 16:31:58   DALLEE
// Oops! Had to change a ++ to CharNext() in SkipSection().
// 
//    Rev 1.2.1.0   12 Aug 1997 22:40:22   DALVIDR
// Branch base for version QAKG
// 
//    Rev 1.2   27 Jun 1997 17:12:06   RStanev
// Changed FileNameToLower() for SYM_VXD to return the same result as
// FileNameToUpper().
//
//    Rev 1.1   11 May 1997 20:50:18   DALLEE
// Removed problematic GetLongName() from VXD and NTK compiles.
//
//    Rev 1.0   11 May 1997 19:54:50   DALLEE
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "fname.h"

//************************************************************************
// Filename manipulation functions

//************************************************************************
// FileNameStrip()
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

LPTSTR WINAPI FileNameStrip( LPTSTR lpszPath )
{
    SYM_ASSERT( lpszPath );

    *FileNameSkipDirectory( lpszPath ) = EOS;

    return ( lpszPath );
} // FileNameStrip()


//************************************************************************
// FileNameAppendSeparator()
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

LPTSTR WINAPI FileNameAppendSeparator( LPTSTR lpszPath )
{
    auto    LPTSTR  lpszEnd;
    auto    LPTSTR  lpszLastChar;

    SYM_ASSERT( lpszPath );

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
} // FileNameAppendSeparator()


//************************************************************************
// FileNameCat()
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

LPTSTR WINAPI FileNameCat( LPTSTR lpszPath, LPCTSTR lpszFileName )
{
    SYM_ASSERT( lpszPath );
    SYM_ASSERT( lpszFileName );

    FileNameAppendSeparator( lpszPath );

    if ( _T('\\') == *lpszFileName )
        lpszFileName++;

    STRCAT( lpszPath, lpszFileName );

    return ( lpszPath );
} // FileNameCat()


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

LPTSTR WINAPI FileNameSkipVolume( LPCTSTR lpszPath )
{
    auto    int     i;

    SYM_ASSERT( lpszPath );

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
} // FileNameSkipVolume()


//************************************************************************
// FileNameSkipDirectory()
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

LPTSTR WINAPI FileNameSkipDirectory( LPCTSTR lpszPath )
{
    auto    LPCTSTR  lpszFile;

    SYM_ASSERT( lpszPath );

    lpszPath = FileNameSkipVolume( lpszPath );

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
} // FileNameSkipDirectory()


//************************************************************************
// FileNameSkipSection()
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

LPTSTR WINAPI FileNameSkipSection( LPCTSTR lpszPath )
{
    auto    LPCTSTR lpszNextSection;

    SYM_ASSERT( lpszPath );

    lpszNextSection = FileNameSkipVolume( lpszPath );

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
            lpszNextSection = CharNext( lpszNextSection );
    }

    return ( (LPTSTR)lpszNextSection );
} // FileNameSkipSection()


//************************************************************************
// FileNameToLower()
//
// This routine translates a filename to lowercase. Double-byte characters
// are not changed as upper and lower case double-byte characters are
// not considered equivalent when comparing filenames.
//
// SYM_VXD: Because there are no APIs for converting to lower case for
//          this platform, FileNameToLower() works the same as
//          FileNameToUpper().
//
// Parameters:
//  LPTSTR  lpszFile        Filename to convert to lowercase.
//
// Returns:
//  lpszFile
//************************************************************************
// 04/18/1997 DALLEE, created.
//************************************************************************

LPTSTR WINAPI FileNameToLower ( LPTSTR lpszFile )
{
#if defined(SYM_VXD)

    return ( FileNameToUpper ( lpszFile ) );

#else

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
            *lpchCurrent = CharToLower( *lpchCurrent );
        }
    }
#else
    STRLWR( lpszFile );
#endif

    return ( lpszFile );

#endif
} // FileNameToLower()


//************************************************************************
// FileNameToUpper()
//
// This routine translates a filename to uppercase. Double-byte characters
// are not changed as upper and lower case double-byte characters are
// not considered equivalent when comparing filenames.
//
// Parameters:
//  LPTSTR  lpszFile        Filename to convert to uppercase.
//
// Returns:
//  lpszFile
//************************************************************************
// 04/18/1997 DALLEE, created.
//************************************************************************

LPTSTR WINAPI FileNameToUpper ( LPTSTR lpszFile )
{
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
            *lpchCurrent = CharToUpper( *lpchCurrent );
        }
    }
#else
    STRUPR( lpszFile );
#endif

    return ( lpszFile );
} // FileNameToUpper()


#if !defined(SYM_VXD) && !defined(SYM_NTK)


//************************************************************************
// FileNameGetLongPathName()
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

DWORD WINAPI FileNameGetLongPathName( LPCTSTR   lpszShort,
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

    auto    FINDDATA    rFind;
    auto    HFIND       hFind;

    // Should always have lpszShort. Must have lpszLong, if cchBuffer != 0.

    SYM_ASSERT( lpszShort );
    SYM_ASSERT( cchBuffer ? NULL != lpszLong : TRUE );

    // Initialize locals.

    lpszOut     = \
    lpszScratch = NULL;
    dwChars     = 0;
    bOverrun    = FALSE;

    // Allocate buffers for final output and scratch for FindFirst lookups.

    uInputSize   = ( STRLEN( lpszShort ) + 1 ) * sizeof( TCHAR );
    uScratchSize = max ( uInputSize, (UINT)cchBuffer );

    lpszOut     = (LPTSTR)MemAllocPtr( GMEM_MOVEABLE, uScratchSize );
    lpszScratch = (LPTSTR)MemAllocPtr( GMEM_MOVEABLE, uScratchSize );

    if ( ( NULL == lpszOut ) ||
         ( NULL == lpszScratch ) )
        goto BailOut;

    // Copy volume name into output and scratch buffers.
    // Include trailing '\\' if present.

    lpszNextSection = FileNameSkipVolume( lpszShort );
    if ( _T('\\') == *lpszNextSection )
        lpszNextSection++;

    dwChars = lpszNextSection - lpszShort;

    STRNCPY( lpszOut, lpszShort, (UINT)dwChars );
    lpszOut[ dwChars ] = EOS;

    dwChars++; // Count includes EOS

    STRCPY( lpszScratch, lpszOut );

    // While sections...

    while ( lpszThisSection = lpszNextSection,
            *lpszThisSection )
    {
        // Copy section to scratch buffer.

        lpszNextSection = FileNameSkipSection( lpszThisSection );

        if ( _T('\\') == *lpszThisSection )
            lpszThisSection++;

        FileNameAppendSeparator( lpszScratch );
        STRNCAT( lpszScratch,
                 lpszThisSection,
                 lpszNextSection - lpszThisSection );

        // Find long name of section.

        hFind = FileFindFirst( lpszScratch, &rFind );

        if ( INVALID_HANDLE_VALUE != hFind )
        {
            // Calculate length and add to output buffer.

            if ( ( FALSE == bOverrun ) &&
                 ( dwChars < uScratchSize ) )
            {
                FileNameAppendSeparator( lpszOut );
                dwChars = 1 + STRLEN( lpszOut );
            }
            else
            {
                dwChars++;
            }

            dwChars += STRLEN( rFind.szFileName );

            bOverrun = dwChars > cchBuffer;

            if ( FALSE == bOverrun )
            {
                STRCAT( lpszOut, rFind.szFileName );
            }

            FileFindClose( hFind );
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
        STRCPY( lpszLong, lpszOut );
    }
    else
    {
#ifdef SYM_WIN32
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
#endif
    }

BailOut:
    if ( NULL != lpszOut )
        MemFreePtr( lpszOut );

    if ( NULL != lpszScratch )
        MemFreePtr( lpszScratch );

    // On success, return string length not including EOS.
    // On overrun, we should return required buffer size including EOS.

    if ( ( dwChars > 0 ) && ( FALSE == bOverrun ) )
        dwChars--;

    return ( dwChars );
} // FileNameGetLongPathName()


//************************************************************************
// FileNameGetShortPathName()
//
// Replacement for Win32 API GetShortPathName() which will function if file
// API's are in OEM mode. GetShortPathName() always expects ANSI input
// and returns ANSI output.
// Input and output for FileNameGetShortPathName() will be OEM or ANSI
// depending on the setting of the file API's.
//
// This function obtains the SFN form of the specified path.
// lpszLong and lpszShort may point to the same buffer.
// This function may be called with cchBuffer == 0 and lpszShort == NULL
// to determine the size of the buffer required for lpszShort.
//
// Parameters:
//  LPCTSTR lpszLong        [in] Input path string.
//  LPTSTR  lpszShort       [out] buffer to receive SFN.
//  DWORD   cchBuffer       [in] size of buffer lpszShort.
//
// Returns:
//  On success:
//      The number of characters copied to lpszShort excluding EOS.
//  On failure:
//      If the buffer is too small, the return value is the size of the
//      buffer required in characters including the EOS.
//      The return value is 0 for all other errors.
//************************************************************************
// 09/02/1997 DALLEE, created.
//************************************************************************

DWORD WINAPI FileNameGetShortPathName( LPCTSTR   lpszLong,
                                       LPTSTR    lpszShort,
                                       DWORD     cchBuffer )
{
    auto    LPTSTR  lpszOut;
    auto    LPTSTR  lpszScratch;

    auto    LPCTSTR lpszNextSection;
    auto    LPCTSTR lpszThisSection;

    auto    LPCTSTR lpszShortNameSection;

    auto    UINT    uInputSize;
    auto    UINT    uScratchSize;
    auto    BOOL    bOverrun;
    auto    DWORD   dwChars;

    auto    FINDDATA    rFind;
    auto    HFIND       hFind;

    // Should always have lpszLong. Must have lpszShort, if cchBuffer != 0.

    SYM_ASSERT( lpszLong );
    SYM_ASSERT( cchBuffer ? NULL != lpszShort : TRUE );

    // Initialize locals.

    lpszOut     = \
    lpszScratch = NULL;
    dwChars     = 0;
    bOverrun    = FALSE;

    // Allocate buffers for final output and scratch for FindFirst lookups.

    uInputSize   = ( STRLEN( lpszLong ) + 1 ) * sizeof( TCHAR );
    uScratchSize = max ( uInputSize, (UINT)cchBuffer );

    lpszOut     = (LPTSTR)MemAllocPtr( GMEM_MOVEABLE, uScratchSize );
    lpszScratch = (LPTSTR)MemAllocPtr( GMEM_MOVEABLE, uScratchSize );

    if ( ( NULL == lpszOut ) ||
         ( NULL == lpszScratch ) )
        goto BailOut;

    // Copy volume name into output and scratch buffers.
    // Include trailing '\\' if present.

    lpszNextSection = FileNameSkipVolume( lpszLong );
    if ( _T('\\') == *lpszNextSection )
        lpszNextSection++;

    dwChars = lpszNextSection - lpszLong;

    STRNCPY( lpszOut, lpszLong, (UINT)dwChars );
    lpszOut[ dwChars ] = EOS;

    dwChars++; // Count includes EOS

    STRCPY( lpszScratch, lpszOut );

    // While sections...

    while ( lpszThisSection = lpszNextSection,
            *lpszThisSection )
    {
        // Copy section to scratch buffer.

        lpszNextSection = FileNameSkipSection( lpszThisSection );

        if ( _T('\\') == *lpszThisSection )
            lpszThisSection++;

        FileNameAppendSeparator( lpszScratch );
        STRNCAT( lpszScratch,
                 lpszThisSection,
                 lpszNextSection - lpszThisSection );

        // Find short name of section.

        hFind = FileFindFirst( lpszScratch, &rFind );

        if ( INVALID_HANDLE_VALUE != hFind )
        {
            // Calculate length and add to output buffer.

            if ( ( FALSE == bOverrun ) &&
                 ( dwChars < uScratchSize ) )
            {
                FileNameAppendSeparator( lpszOut );
                dwChars = 1 + STRLEN( lpszOut );
            }
            else
            {
                dwChars++;
            }

            // Get SFN - szAlternateFileName if present, else szFileName.

            lpszShortNameSection = rFind.szAlternateFileName[0] ?
                                        rFind.szAlternateFileName :
                                        rFind.szFileName;

            dwChars += STRLEN( lpszShortNameSection );

            bOverrun = dwChars > cchBuffer;

            if ( FALSE == bOverrun )
            {
                STRCAT( lpszOut, lpszShortNameSection );
            }

            FileFindClose( hFind );
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
        STRCPY( lpszShort, lpszOut );
    }
    else
    {
#ifdef SYM_WIN32
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
#endif
    }

BailOut:
    if ( NULL != lpszOut )
        MemFreePtr( lpszOut );

    if ( NULL != lpszScratch )
        MemFreePtr( lpszScratch );

    // On success, return string length not including EOS.
    // On overrun, we should return required buffer size including EOS.

    if ( ( dwChars > 0 ) && ( FALSE == bOverrun ) )
        dwChars--;

    return ( dwChars );
} // FileNameGetShortPathName()

#endif // !SYM_VXD && !SYM_NTK
