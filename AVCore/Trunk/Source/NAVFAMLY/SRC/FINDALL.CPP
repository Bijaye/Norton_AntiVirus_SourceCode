// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVFAMLY/VCS/findall.cpv   1.24   11 Dec 1996 15:34:58   MZAREMB  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVFAMLY/VCS/findall.cpv  $
// 
//    Rev 1.24   11 Dec 1996 15:34:58   MZAREMB
// DOS-specific change for debug mode.
// 
//    Rev 1.23   09 Sep 1996 15:53:56   JBRENNA
// Add NON_TRANS_OEM_CHARS_* logic. Previously, the characters were
// hard coded to "*?". Which for NT is fine, but Win95 needs "_". Now,
// the characters are determined at run-time based on the windows
// platform.
// 
//    Rev 1.22   11 Jun 1996 11:08:58   RHAYES
// Correctly use short name when long name contains wildcards
// which occurs when scanning files created using DBCS.
// 
//    Rev 1.21   03 Jun 1996 16:16:28   RHAYES
// Additional OEM,<->Ansi mod.'s from Luigi.
// 
//    Rev 1.20   21 May 1996 14:33:18   RHAYES
// Merged Luigi OEM<->Ansi code for more robust support of
// Non-Ansi directory and filenames.
// 
//    Rev 1.19   26 Feb 1996 12:13:26   JWORDEN
// More DBCS changes
// 
//    Rev 1.18   20 Feb 1996 15:08:16   MLELE
// Included mmsystem.h if SYM_DEBUG for OutputDebugStr prototype and
// replaced SetFileApisTo... calls by SET_FILE_APIS_TO_... macros in function
// CFindAll::WrapperForFileFindNext32.
// 
//    Rev 1.17   14 Feb 1996 16:11:08   JWORDEN
// Implement WrapperForFileFindNext32 ()
// 
//    Rev 1.16   12 Jul 1995 13:59:56   BARRY
// The test in StartIterate to assure we don't go into a path too deep has been
// changed from <= to < (SYM_MAX_PATH - 14). This allows the maximum possible
// valid names to pass through.
// 
//    Rev 1.15   07 Jul 1995 23:01:58   BARRY
// Watch for max path and don't go there
// 
//    Rev 1.14   07 Jul 1995 00:47:14   RHAYES
// Fix for traversing too deep (past 260 max.). Changes include removing
// CFindAll::FixupNewPath() (incorporated NameAppendFile() in IsValidItem()).
// Added new logic to IsValidItem() to assure that no items pass through that
// can pass our/Win95's 260 char. path max.
// 
//    Rev 1.13   04 May 1995 16:06:00   MFALLEN
// NameValidatePathEx() assert problem.
// 
//    Rev 1.12   03 May 1995 18:03:46   MFALLEN
// Typo
// 
//    Rev 1.11   03 May 1995 17:55:38   MFALLEN
// Solved peer to peer scanning problem.
// 
//    Rev 1.10   30 Apr 1995 19:18:14   DALLEE
// &? #ifdef SYM_WIN32 for NameValidatePathEx() assertions.
// For non-W32 platforms, this automatically assumes non-LFN rules.
// No time to fix this in Quake before our ET (too scared of the Quack Police).
// 
//    Rev 1.9   20 Apr 1995 17:57:00   MFALLEN
// User NameValidateEx() that can handle wildcard characters. This was required
// for UNC with wildcard name validation.
// 
//    Rev 1.8   17 Apr 1995 14:57:50   MFALLEN
// SYM_MAX_PATH + 1
// 
//    Rev 1.7   10 Apr 1995 15:53:42   MFALLEN
// Typo
// 
//    Rev 1.6   10 Apr 1995 15:17:06   MFALLEN
// Check the STRCMP way if item is the current directory or the previous dir.
// 
//    Rev 1.5   29 Mar 1995 17:43:22   MFALLEN
// Replaced NameGetIllegalChars() with NameReturnIllegalChars()
// 
//    Rev 1.4   01 Nov 1994 13:32:50   GDZIECI
// Made FileFindFirst-Next iteration work for Win16 platform.
// 
// 
//    Rev 1.3   01 Nov 1994 13:24:18   GDZIECI
// 
//    Rev 1.2   20 Oct 1994 08:57:10   KEITH
// Remove error "more than one memory attribute" for Win16 builds.
// 
//    Rev 1.1   07 Oct 1994 18:10:42   MFALLEN
// 
//    Rev 1.0   07 Oct 1994 17:55:40   MFALLEN
// Initial revision.
// 
//    Rev 1.7   13 Sep 1994 15:42:00   MFALLEN
// DALLEE checking in MFALLEN's clean-up changes.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "findall.hpp"

#ifdef SYM_DOS
#include "stdio.h"
#define  OutputDebugStr printf
#endif

#define SIZE_FINDDATA   sizeof(FINDDATA)


// --------------------------------------------------------------------------
//                      LOCAL FUNCTION PROTOTYPES
// --------------------------------------------------------------------------

BOOL LOCAL IsUNCPath (                 // Checks path to be valid UNC path
    LPSTR   lpPath                     // [in] path to check
    );

// --------------------------------------------------------------------------

#if !defined(SYM_WIN32) && !defined(SYM_VXD)

// --------------------------------------------------------------------------
//                      ONLY NON WIN32 SPECIFIC STUFF
// --------------------------------------------------------------------------

#define ERROR_NO_MORE_FILES 1
WORD LOCAL GetLastError ( VOID );

#endif // #ifndef SYM_WIN32

// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: CFindAll::CFindAll()
//
// Author:      Martin Fallenstedt,                 Date: July 24, 1994
//
// Description: Constructor for CFindAll class. Initializes variables.
//
// Input:       Nothing
//
// Output:      Nothing
//
// --------------------------------------------------------------------------

SYM_EXPORT CFindAll::CFindAll()
{
    FreeAllVars ( FALSE );
}

// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: CFindAll::~CFindAll()
//
// Author:      Martin Fallenstedt,                 Date: July 24, 1994
//
// Description: Destructor for CFindAll class. Clears variables.
//
// Input:       Nothing
//
// Output:      Nothing
//
// --------------------------------------------------------------------------

SYM_EXPORT CFindAll::~CFindAll()
{
    FreeAllVars ( TRUE );
}

// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: CFindAll::FreeAllVars()
//
// Author:      Martin Fallenstedt,                 Date: July 24, 1994
//
// Description: Initializes and / or frees up variables.
//
// Input:       bFreeHandle; TRUE if functions should free up all the
//              find handles, FALSE otherwise.
//
// Output:      Nothing
//
// --------------------------------------------------------------------------

VOID WINAPI CFindAll::FreeAllVars (     // Free all the variables
    BOOL bFreeHandle )                  // [in] if handles should be freed
{
    m_uCurrLevel = 0;
    MEMSET ( &m_FindData, EOS, sizeof(m_FindData) );
    MEMSET ( &m_StartInfo, EOS, sizeof(m_StartInfo) );

    for ( UINT i = 0; i < MAX_LEVEL; i++ )
        {
        if ( m_hHandles[ i ] && bFreeHandle == TRUE )
            {
            FileFindClose ( m_hHandles[ i ] );
            }

        m_hHandles[ i ] = NULL;
#ifdef  SYM_WIN16
        if ( m_pFindData [ i ] && bFreeHandle == TRUE )
            MemFreePtr ( m_pFindData [ i ] );

        m_pFindData [ i ] = NULL;
#endif
        }
}

// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: CFindAll::StartIterate()
//
// Author:      Martin Fallenstedt,                 Date: July 24, 1994
//
// Description: Enumerates files and directories. For every item found the
//              application specified callback function is called.
//
// Input:       pointer to FINDALLINFO structure that specifies what drive
//              and directory to enumerate and other relevant information.
//              For details please refer to the FINDALL.HPP include file.
//
// Output:
//              ERR_ENUM_OK,             No problem.
//              ERR_ENUM_INITIALIZATION, lpStartInfo is not valid.
//              ERR_ENUM_INTERNAL,       Internal error. FIX IT.
//              ERR_ENUM_NO_MEMORY,      Not enough memory (Win16 only)
//
// --------------------------------------------------------------------------
// 1/11/94 GAD - Fix for Win16:
//
//      In Win16 FindFirst-Next handles are useless - it's the find data
// structure that identifies the search. Therefore we have to maintain an 
// array of ptrs to structures for each level (just like handles for Win32).
// --------------------------------------------------------------------------

ENUMSTATUS SYM_EXPORT WINAPI CFindAll::StartIterate ( // Start iterate function
    LPFINDALLINFO lpStartInfo )         // [in] initialization info
{
    auto    ENUMSTATUS  Status = ERR_ENUM_OK;
    auto    UINT        uDummyAttr;
#ifdef SYM_WIN16
    BOOL    bFound;
#endif
                                        // -----------------------------------
                                        // Validate information and initialize
                                        // this iteration object
                                        // -----------------------------------
    if ( ValidateStartInfo ( lpStartInfo ) == TRUE &&
         InitializeObject  ( lpStartInfo ) == TRUE )
        {
        BOOL bCallFindFirst = TRUE;

                                        // -----------------------------------
                                        // Stay in this loop while there are
                                        // files to be processed and the app
                                        // wants to continue.
                                        // -----------------------------------
        for ( BOOL bContinue = TRUE; bContinue == TRUE; )
            {
                                        // -----------------------------------
                                        // Stay in this loop until we find a
                                        // valid item. The only non valid item
                                        // at present time the current dir '.'
                                        // -----------------------------------
            do
                {
                if ( bCallFindFirst == TRUE )
                    {
                                        // -----------------------------------
                                        // If first time in this directory
                                        // build up the full path to the
                                        // directory and file name to search
                                        // -----------------------------------

                    char szSearchFor [ SYM_MAX_PATH + 1 ];
                    STRCPY ( szSearchFor, m_szFullPath );
                    NameAppendFile ( szSearchFor, m_szFileName );

                    bCallFindFirst = FALSE;

#ifdef SYM_WIN16
                    // Find data ptr should be initilaized to 0
                    SYM_ASSERT ( m_pFindData [ m_uCurrLevel ] == NULL );

                    m_pFindData [ m_uCurrLevel ] = (FINDDATA *)MemAllocPtr (
                                                        GHND,
                                                        SIZE_FINDDATA );
                    if (m_pFindData [ m_uCurrLevel ])
                        {
                        m_hHandles [ m_uCurrLevel ] = FileFindFirst (
                                                szSearchFor,
                                                m_pFindData [m_uCurrLevel] );
                        MEMCPY ( &m_FindData,
                            m_pFindData [m_uCurrLevel],
                            SIZE_FINDDATA);
                        }
                    else
                        {
                        Status = ERR_ENUM_NO_MEMORY;
                        bContinue = FALSE;
                        break;
                        }
#else
                    m_hHandles [ m_uCurrLevel ] = FileFindFirst (
                                                        szSearchFor,
                                                        &m_FindData );
#endif
                    if ( m_hHandles [ m_uCurrLevel ] == INVALID_HANDLE_VALUE )
                        {
                                        // -----------------------------------
                                        // Something nasty happened or the
                                        // directory didn't contain any files.
                                        // Neither the '.' or the '..' that is
                                        // the case on a network drive. Since
                                        // there aren't any specific statuses
                                        // that indicate this, assume that
                                        // things are OK and switch back to
                                        // previous level.
                                        // -----------------------------------

                        bContinue = IndicateBackOneDir ();

                                        // -----------------------------------
                                        // The only time this function returns
                                        // FALSE if we have reached the direc-
                                        // tory level that started from, or if
                                        // the app indicated to halt.
                                        // -----------------------------------
                        if ( bContinue == FALSE )
                            break;
                        }
                    }
                else
                    {
                                        // -----------------------------------
                                        // We've been here before.
                                        // -----------------------------------
#ifdef SYM_WIN16
                    while ((bFound = FileFindNext ( m_hHandles [ m_uCurrLevel ],
                                    m_pFindData  [ m_uCurrLevel ] )) == FALSE )
#else
                    while ( WrapperForFileFindNext32 ( m_hHandles [ m_uCurrLevel ],
                                           &m_FindData ) == FALSE )
#endif
                        {
                        if ( GetLastError() == ERROR_NO_MORE_FILES )
                            {
                                        // -----------------------------------
                                        // If we just ran out of files see
                                        // if we should continue.
                                        // -----------------------------------

                            bContinue = IndicateBackOneDir ();

                                        // -----------------------------------
                                        // The only time this function returns
                                        // FALSE if we have reached the direc-
                                        // tory level that started from, or if
                                        // the app indicated to halt.
                                        // -----------------------------------
                            if ( bContinue == FALSE )
                                break;
                            }
                        else
                            {
                                        // -----------------------------------
                                        // Something serious happened.
                                        // -----------------------------------
                            bContinue = FALSE;
                            break;
                            }
                        }
#ifdef SYM_WIN16
                    if (bFound)                 // if file found - copy data
                        MEMCPY ( &m_FindData,
                            m_pFindData [ m_uCurrLevel ],
                            SIZE_FINDDATA);
#endif
                    }
                }
            while ( IsValidItem() == FALSE && bContinue == TRUE );

                                        // -----------------------------------
                                        // If no problem, lets rock.
                                        // -----------------------------------
            if ( bContinue == TRUE )
                {
                                        // Don't show something they didn't
                                        // ask for.
                if ( !IsDirectory() || EnterDirectories() )
                    bContinue = ShowItemFound();

                if ( bContinue == TRUE &&
                     IsDirectory() == TRUE &&
                     EnterDirectories() == TRUE )
                    {
                                        // -----------------------------------
                                        // Make sure that we're not getting
                                        // too deep down into the directory
                                        // tree. Better NEVER occur!
                                        // -----------------------------------
                    if ( ++m_uCurrLevel < MAX_LEVEL )
                        {
                                        // If combining paths won't allow room for
                                        // any more chars, don't bother.
                                        // -1 is for implied backslash separator
                                        // (C:\CURDIR + NEWDIR = C:\CURDIR\NEWDIR)
                                        // -12 is for an 8.3 filename.
                                        // (Can't travel deeper than 246+EOS)
                        if ( (m_iLenFullPath + STRLEN(m_FindData.szFileName)
                              > SYM_MAX_PATH - 14) ||
                             (m_iLenFullPath + STRLEN(m_FindData.szAlternateFileName)
                              > SYM_MAX_PATH - 14) )
                            {
                                        // We're not going into this directory
                            --m_uCurrLevel;
                            }
                        else
                            {
                            UINT    nWinType           = SystemGetWindowsType ();
                            LPCTSTR szNonTransOemChars;

                                            // Determine the correct non-translating
                                            // OEM characters based on the windows platform.
                            if (SYM_SYSTEM_WIN_NT == nWinType)
                                szNonTransOemChars = NON_TRANS_OEM_CHARS_NT;
                            else if (SYM_SYSTEM_WIN_95 == nWinType)
                                szNonTransOemChars = NON_TRANS_OEM_CHARS_95;
                            else
                                szNonTransOemChars = "";

                            bCallFindFirst = TRUE;
                            NameAppendFile ( m_szFullPath, m_FindData.szFileName );

                                        // Long name may be bogus...
                                        // (ANSI/OEM non-translatable)
                                        // or contains a WildCard; this happens
                                        // when a DBCS directory name is 
                                        // created in Japanese NT and scanned in
                                        // a different version (shows up as ??)
                                        // Go with short name in this case.
                            if ( (EOS != m_FindData.szAlternateFileName[0]) &&
                                 (NULL != STRPBRK(m_FindData.szFileName, szNonTransOemChars ) ) &&
                                 (NOERR != FileGetAttr(m_FindData.szFileName, &uDummyAttr)) )
                                {
                                NameStripFile ( m_szFullPath );
                                NameAppendFile ( m_szFullPath, m_FindData.szAlternateFileName );
                                }

                            m_iLenFullPath = STRLEN (m_szFullPath);
                            }
                        }
                    else
                        {
                        --m_uCurrLevel;
                        bContinue = FALSE;
                        }
                    }
                }
            }
        }
    else
        Status = ERR_ENUM_INITIALIZATION;

    return ( Status );
}

// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1996
// --------------------------------------------------------------------------
//
// Function Name: CFindAll::WrapperForFileFindNext32 ()
//
// Author:      Jim Worden,                 Date: Feb 5, 1996
//
// Description: Wrapper for the find next function for Win32 to force
//              find next to occur in OEM mode
//
// Input:       Nothing
//
// Output:      Return from FileFindNext ()
//
// --------------------------------------------------------------------------

BOOL WINAPI CFindAll::WrapperForFileFindNext32
(
    HFIND hFind,
    LPFINDDATA lpFindData
)
{
    BOOL bReturn;

    bReturn = FileFindNext (hFind, lpFindData);

    return (bReturn);
}

// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: CFindAll::IsDirectory()
//
// Author:      Martin Fallenstedt,                 Date: July 24, 1994
//
// Description: Figures out if the current item is a directory or a file.
//
// Input:       Nothing
//
// Output:      TRUE if the item is a directory.
//              FALSE the item is not a directory.
//
// --------------------------------------------------------------------------

BOOL WINAPI CFindAll::IsDirectory(
    VOID )
{
    return ( ( m_FindData.dwFileAttributes & FA_DIR ) ? TRUE : FALSE );
}

// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: CFindAll::EnterDirectories()
//
// Author:      Martin Fallenstedt,                 Date: July 24, 1994
//
// Description: Figures out if we should traverse sub-directories.
//
// Input:       Nothing
//
// Output:      TRUE sub-directories should be traversed.
//              FALSE sub-directories should not be traversed.
//
// --------------------------------------------------------------------------

BOOL WINAPI CFindAll::EnterDirectories (
    VOID )
{
    return ( m_StartInfo.bSubDirs );
}


// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: CFindAll::ValidateStartInfo()
//
// Author:      Martin Fallenstedt,                 Date: July 24, 1994
//
// Description: Validates the information passed by the application.
//
// Input:       The information passed by the application, pointer to
//              FINDALLINFO structure.
//
// Output:      TRUE the info looks valid, FALSE it's trash.
//
// --------------------------------------------------------------------------

BOOL WINAPI CFindAll::ValidateStartInfo (
    LPFINDALLINFO lpStartInfo )         // [in] initialization info
{
    auto    BOOL    bReturn = TRUE;

    SYM_ASSERT ( lpStartInfo->lpfn != NULL );

    if ( ( NameValidatePathEx ( lpStartInfo->szStartPath, TRUE ) == FALSE   &&
           IsUNCPath        ( lpStartInfo->szStartPath ) == FALSE ) ||
         lpStartInfo->lpfn == NULL  )
        {
        bReturn = FALSE;
        }

    return ( bReturn );
}

// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: CFindAll::InitializeObject()
//
// Author:      Martin Fallenstedt,                 Date: July 24, 1994
//
// Description: This function initializes this object with the information
//              provided by the application.
//
// Input:       The information passed by the application, pointer to
//              FINDALLINFO structure.
//
// Output:      TRUE object initialized, FALSE error
//
// --------------------------------------------------------------------------

BOOL WINAPI CFindAll::InitializeObject (
    LPFINDALLINFO lpStartInfo )    // [in] initialization info
{
    auto    BOOL    bReturn = TRUE;

    FreeAllVars ( TRUE );
    MEMCPY ( &m_StartInfo, lpStartInfo, sizeof(m_StartInfo) );

    if ( NameIsDirectory ( lpStartInfo->szStartPath ) )
        {
        STRCPY ( m_szFullPath, lpStartInfo->szStartPath );
        *m_szFileName = EOS;
        }
    else
        {
        NameReturnPath ( lpStartInfo->szStartPath, m_szFullPath );
        NameReturnFile ( lpStartInfo->szStartPath, m_szFileName );
        }

    m_iLenFullPath = STRLEN (m_szFullPath);
                                        // ---------------------------------
                                        // If the app didn't specify a file
                                        // name then use a default.
                                        // ---------------------------------
    if ( *m_szFileName == EOS )
        STRCPY ( m_szFileName, _T("*.*") );

    return ( bReturn );
}

// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: CFindAll::IndicateBackOneDir()
//
// Author:      Martin Fallenstedt,                 Date: July 24, 1994
//
// Description: This function informs the app that we're about to go back
//              one directory level.
//
// Input:       Nothing
//
// Output:      TRUE if we should continue with enumeration, FALSE to stop.
//
// --------------------------------------------------------------------------

BOOL WINAPI CFindAll::IndicateBackOneDir (
    VOID )
{
    auto    FINDALL    FindItem = {0};

                                        // ---------------------------------
                                        // Make sure the the FindData struct
                                        // is cleared so the IsValidItem()
                                        // function returns FALSE;
                                        // ---------------------------------

    MEMSET ( &m_FindData, EOS, sizeof(m_FindData) );

                                        // ---------------------------------
                                        // Prepare the information for the
                                        // callback function to notify.
                                        // ---------------------------------
    FindItem.dwStructSize     = sizeof(FindItem);
    FindItem.dwCookie         = m_StartInfo.dwCookie;
    FindItem.dwFileAttributes = FA_DIR;

    STRCPY ( FindItem.szShortFileName, _T("..") );
    STRCPY ( FindItem.szLongFileName,  _T("..") );

    SYM_ASSERT ( m_StartInfo.lpfn );

    BOOL bReturn = (*m_StartInfo.lpfn)(&FindItem);

    FileFindClose ( m_hHandles [ m_uCurrLevel ] );

    NameStripFile ( m_szFullPath );
    m_iLenFullPath = STRLEN (m_szFullPath);
    m_hHandles [ m_uCurrLevel ] = NULL;
#ifdef  SYM_WIN16
    SYM_ASSERT ( m_pFindData[ m_uCurrLevel ] );    // should not be 0 here

    if ( m_pFindData[ m_uCurrLevel ] )
        MemFreePtr ( m_pFindData[ m_uCurrLevel ] );

    m_pFindData[ m_uCurrLevel ] = NULL;
#endif

    if ( m_uCurrLevel )
        m_uCurrLevel--;
    else
        bReturn = FALSE;

    return ( bReturn );
}

// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: CFindAll::IsValidItem()
//
// Author:      Martin Fallenstedt,                 Date: July 24, 1994
//
// Description: Checks if the currently found item is valid so we can pass it
//              to the app. At present time the current directory and the
//              previous directory indicators are considered as not valid.
//              Note that we're using the ".." to indicate to the app that
//              we're going up one level so don't screw it up!
//       Is
// Input:       Nothing
//
// Output:      TRUE if item is valid, FALSE item is not valid.
//
// --------------------------------------------------------------------------

BOOL WINAPI CFindAll::IsValidItem (
    VOID )
{
    SYM_ASSERT ( *m_szFullPath );
    SYM_ASSERT ( *m_szFileName );

                                        // File name not valid when path
                                        // exceeds max. This can happen with
                                        // UNC long paths.
    if ( !m_FindData.szFileName[0] )
        return( FALSE );

#ifdef SLOWER_BUT_MORE_STRAIGHTFORWARD
                                        // Skip current/parent sub-dir.'s
    if ( ( !STRCMP ( m_FindData.szFileName, _T("..") ) ) ||
         ( !STRCMP ( m_FindData.szFileName, _T(".") ) ) )
        return( FALSE );
#else
                                        // Since this function is called
                                        // for every file, avoid two 
                                        // calls to STRCMP to be faster...
                                        // This should be OK for DBCS
    if ( m_FindData.szFileName[0] == _T('.') )
        {
        if ( (m_FindData.szFileName[1] == _T('.') && m_FindData.szFileName[2] == EOS)
             || m_FindData.szFileName[1] == EOS)
            {
            return (FALSE);
            }
        }
#endif

                                        // Reject spec. if it larger than our/
                                        // Win95's max. file spec. size
                                        // - 1 is for the NULL terminator
    if ( m_iLenFullPath + STRLEN ( m_FindData.szFileName ) >= SYM_MAX_PATH - 1 )
        {
#ifdef SYM_DEBUG
        OutputDebugStr( _T("\nCFindall:IsValidItem - Rejected spec:\n") );
        OutputDebugStr( m_szFullPath );
#endif
        return( FALSE );
        }
                                        // File spec. meets current criteria,
                                        // let it pass...
    return ( TRUE );
}


// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: CFindAll::ShowItemFound()
//
// Author:      Martin Fallenstedt,                 Date: July 24, 1994
//
// Description: Shows the item that was found to the application.
//
// Input:       Nothing
//
// Output:      TRUE if we should continue with enumeration, FALSE to stop.
//
// --------------------------------------------------------------------------

BOOL WINAPI CFindAll::ShowItemFound (
    VOID )
{
    auto    FINDALL    FindItem = {0};

    FindItem.dwStructSize     = sizeof(FindItem);
    FindItem.dwCookie         = m_StartInfo.dwCookie;
    FindItem.dwFileAttributes = m_FindData.dwFileAttributes;
    FindItem.ftCreationTime   = m_FindData.ftCreationTime;
    FindItem.ftLastAccessTime = m_FindData.ftLastAccessTime;
    FindItem.ftLastWriteTime  = m_FindData.ftLastWriteTime;
    FindItem.dwFileSize       = m_FindData.dwFileSizeLow;

    STRCPY ( FindItem.szFullPath,      m_szFullPath );
    STRCPY ( FindItem.szShortFileName, m_FindData.szAlternateFileName );
    STRCPY ( FindItem.szLongFileName,  m_FindData.szFileName );

    SYM_ASSERT ( m_StartInfo.lpfn );

    return ( (*m_StartInfo.lpfn)( &FindItem ) );
}


//************************************************************************
// IsUNCPath()
//
// This routine checks if the provided path appears to be a valid UNC path.
//
// Parameters:
//      lpPath  [in] path to verify
//
// Returns:
//      TRUE if the path appears to be valid, FALSE otherwise
//
//************************************************************************
// 8/25/94  MARTIN, function created
//************************************************************************

BOOL LOCAL IsUNCPath (                  // Checks path to be valid UNC path
    LPSTR   lpPath                      // [in] path to check
    )
{
    SYM_ASSERT ( lpPath );

    auto    BOOL    bValid = TRUE;
    auto    char    szPath [ SYM_MAX_PATH + 1 ];

    szPath[0] = EOS;

                                        // ----------------------------------
                                        // This is to preserve the case.
                                        // ----------------------------------
    STRCPY ( szPath, lpPath );
    STRUPR ( szPath );

                                        // ----------------------------------
                                        // Stay in loop while there are stuff
                                        // to be verified and the path still
                                        // looks valid.
                                        // ----------------------------------
    UINT uVerifyStep = 1;

    while ( bValid == TRUE && uVerifyStep )
        {
        switch ( uVerifyStep )
            {
            case 1:
                                        // ----------------------------------
                                        // Path must be larger than 2 chars
                                        // ----------------------------------
                if ( STRLEN ( szPath ) > 2 )
                    uVerifyStep++;
                else
                    bValid = FALSE;
                break;

            case 2:
                                        // ----------------------------------
                                        // Check for invalid characters.
                                        // ----------------------------------
                {
                char    szIllegalChars [ SYM_MAX_PATH ];

                szIllegalChars[0] = EOS;

                                        // ----------------------------------
                                        // SYM_ILLEGAL_USEVOLUME,
                                        // ----------------------------------
                                        // This was causing problems when
                                        // attached to other computers through
                                        // peer to peer comm. and the area
                                        // attached contained spaces an other
                                        // funky chars.
                                        // ----------------------------------

                WORD wNumChars = NameReturnIllegalChars (
                                        SYM_ILLEGAL_WIN95,
                                        lpPath,
                                        szIllegalChars,
                                        sizeof(szIllegalChars)
                                        );

                for ( WORD wIndex = 0; wIndex < wNumChars; wIndex++ )
                    {
                                        // ----------------------------------
                                        // Skip over NULL character.
                                        // ----------------------------------

                    if ( szIllegalChars[wIndex] == _T('.') ||
                         szIllegalChars[wIndex] == EOS )
                        continue;

                    if ( STRCHR ( szPath, szIllegalChars[wIndex] ) )
                        {
                        bValid = FALSE;
                        break;
                        }
                    }

                if ( bValid == TRUE )
                    uVerifyStep++;
                }
                break;

            case 3:
                {
                if ( szPath[0] == _T('\\') && szPath[1] == _T('\\') )
                    uVerifyStep++;
                else
                    bValid = FALSE;
                }
                break;

            case 4:
                {
                LPSTR lpWild = STRCHR ( szPath, _T('*') );

                if ( lpWild )
                    {
                    while ( *lpWild && bValid == TRUE )
                        {
                        if ( *lpWild == _T('\\') )
                            bValid = FALSE;

                        lpWild = AnsiNext ( lpWild );
                        }
                    }

                if ( bValid == TRUE )
                    {
                    lpWild = STRCHR ( szPath, _T('?') );

                    if ( lpWild )
                        {
                        while ( *lpWild && bValid == TRUE )
                            {
                            if ( *lpWild == _T('\\') )
                                bValid = FALSE;

                            lpWild = AnsiNext ( lpWild );
                            }
                        }
                    }

                uVerifyStep = 0;
                }
                break;

            default:
                SYM_ASSERT ( 0 );
                break;
            }
        }

    return ( bValid );
}

#if !defined(SYM_WIN32) && !defined(SYM_VXD)
// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: GetLastError()
//
// Author:      Martin Fallenstedt,                 Date: July 24, 1994
//
// Description: This function doesn't exist in DOS or Windows so emulate it.
//
// Input:       Nothing
//
// Output:      ERROR_NO_MORE_FILES to indicate that there aren't any more
//              files in the directory.
//
// --------------------------------------------------------------------------

WORD LOCAL GetLastError ( VOID )
{
    return ( ERROR_NO_MORE_FILES );
}
#endif // #ifndef SYM_WIN32
