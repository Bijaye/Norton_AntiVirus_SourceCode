// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVAPI1/VCS/enumfile.cpv   1.2   11 Sep 1997 17:33:56   KSACKIN  $
//
// ENUMFILE.CPP
//
// CEnumFiles definition.
//
// Class for enumerating all files in a folder and its subfolders.
// See also helper class CEnumSingleDir.
//
//************************************************************************
// $Log:   S:/AVAPI1/VCS/enumfile.cpv  $
// 
//    Rev 1.2   11 Sep 1997 17:33:56   KSACKIN
// Ported changes from QAKG.
//
//    Rev 1.0.1.3   02 Sep 1997 13:45:24   DALLEE
// Call FileNameGetShortPathName() instead of GetShortPathName().
// GetShortPathName() always expects ANSI input and returns ANSI data.
// FileNameGetShortPathName() works w/ OEM data when file API's are in OEM mode.
//
//    Rev 1.0.1.2   29 Aug 1997 19:07:22   DALLEE
// Two bogus work arounds:
// 1. Use LFN instead of SFN when entering subdirectories. Using the MS W95
//    NetWare client w/ API's in OEM mode, FindFirstFile() fails if given
//    a SFN directory.
// 2. Since we're now using the LFN for entering subdirectories, we need to
//    do the check for bogus name (ANSI LFN that didn't convert correctly
//    to OEM) and go back to using the SFN in that case. This is for the mess
//    where the only way to guarantee being able to access all files on a W95
//    drive is to run the file API's in OEM mode.
//
//    Rev 1.1   17 Aug 1997 16:07:58   DALLEE
// On initialization, was doing a SFN conversion after a lossy LFN conversion.
// Now converts from original good data for each.
//
//    Rev 1.0   13 May 1997 18:01:50   DALLEE
// Initial revision.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "enumfile.h"
#include "fname.h"


//************************************************************************
// Constants

static const    TCHAR   szAllFiles    [] = _T("*");

static const    TCHAR   s_szOemReplacementCharsNT  [] = _T("?*_");
static const    TCHAR   s_szOemReplacementCharwW95 [] = _T("_");


//************************************************************************
// CEnumFiles
//************************************************************************

//************************************************************************
// CEnumFiles::CEnumFiles()
//
// Constructor which initializes all locals to default empty values.
// Init() function required to set path for class to operate on.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

CEnumFiles::CEnumFiles()
{
    Clear();
} // CEnumFiles()


//************************************************************************
// CEnumFiles::~CEnumFiles()
//
// Destructor free all mem and other such cleanup.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

CEnumFiles::~CEnumFiles()
{
    CloseAll();
    Clear();
} // ~CEnumFiles()


//************************************************************************
// CEnumFiles::Clear()
//
// Set all internal variables to default empty values.
// Note this function ONLY sets empty values. CloseAll() is used to
// free all items before calling this function.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

int CEnumFiles::Clear()
{
    m_pDirNode      = NULL;

    m_uMaxPath      = 0;

    m_pszFileSpec   = \
    m_pszFileMask   = \
    m_pszLongName   = \
    m_pszShortName  = \
    m_pszLongDir    = \
    m_pszShortDir   = NULL;

    memset( (LPFINDDATA)&m_rFind, 0, sizeof( m_rFind ) );

    return ( NO_ERROR );
} // Clear()


//************************************************************************
// CEnumFiles::CloseAll()
//
// Free all handles and allocated memory.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

int CEnumFiles::CloseAll()
{
    // Free file specification.

    // m_pszFileMask points at the file portion of m_pszFileSpec.
    // No need to free memory, just set to NULL.

    m_pszFileMask = NULL;

    if ( NULL != m_pszFileSpec )
    {
        MemFreePtr( m_pszFileSpec );
        m_pszFileSpec = NULL;
    }

    // Free file, full path, and directory buffers.

    if ( NULL != m_pszLongName )
        MemFreePtr( m_pszLongName );

    if ( NULL != m_pszShortName )
        MemFreePtr( m_pszShortName );

    if ( NULL != m_pszLongDir )
        MemFreePtr( m_pszLongDir );

    if ( NULL != m_pszShortDir )
        MemFreePtr( m_pszShortDir );

    m_pszLongName   = \
    m_pszShortName  = \
    m_pszLongDir    = \
    m_pszShortDir   = NULL;

    // Free directory enumeration helper class.

    if ( NULL != m_pDirNode )
        {
        delete m_pDirNode;
        m_pDirNode = NULL;
        }

    return ( NO_ERROR );
} // CloseAll()


//************************************************************************
// CEnumFiles::Init()
//
// Sets the file specification to enumerate.
// Input should be of the form
//      "C:\Program Files\*.*" to enumerate all files in a directory.
//
// Parameters:
//      LPCTSTR lpszFileSpec            File specification to enumerate.
//      UINT    uMaxPath                Max path length to track/return
//                                        including EOS.
//
// Returns:
//      NO_ERROR                        On success.
//      ERROR_NOT_ENOUGH_MEMORY         If out of memory.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

int CEnumFiles::Init( LPCTSTR lpszFileSpec, UINT uMaxPath )
{
    auto    LPTSTR      lpszAllFilesSpec;
    auto    LPTSTR      lpszDummyParam;
    auto    DWORD       dwChars;
    auto    int         nResult;

    SYM_ASSERT( uMaxPath > 0 );
    SYM_ASSERT( STRLEN( lpszFileSpec ) < uMaxPath );

    // Initialize locals

    lpszAllFilesSpec = NULL;
    nResult = -1;

    // Reset object

    CloseAll();
    Clear();

    // Allocate path buffers

    m_pszFileSpec   = (LPTSTR)MemAllocPtr( GMEM_MOVEABLE, ( STRLEN( lpszFileSpec ) + 1 ) * sizeof( TCHAR ) );

    m_pszLongName   = (LPTSTR)MemAllocPtr( GMEM_MOVEABLE, uMaxPath * sizeof( TCHAR ) );
    m_pszShortName  = (LPTSTR)MemAllocPtr( GMEM_MOVEABLE, uMaxPath * sizeof( TCHAR ) );

    m_pszLongDir    = (LPTSTR)MemAllocPtr( GMEM_MOVEABLE, uMaxPath * sizeof( TCHAR ) );
    m_pszShortDir   = (LPTSTR)MemAllocPtr( GMEM_MOVEABLE, uMaxPath * sizeof( TCHAR ) );

    if ( ( NULL == m_pszFileSpec )  ||
         ( NULL == m_pszLongName )  ||
         ( NULL == m_pszShortName ) ||
         ( NULL == m_pszLongDir )   ||
         ( NULL == m_pszShortDir ) )
    {
        goto BailOut;
    }

    *(LPTSTR)m_pszFileSpec       = \
    *(LPTSTR)m_pszLongName       = \
    *(LPTSTR)m_pszShortName      = \
    *(LPTSTR)m_pszLongDir        = \
    *(LPTSTR)m_pszShortDir       = _T('\0');

    // Save maximum path length

    m_uMaxPath = uMaxPath;

    // Save file specification and file mask

    STRCPY( m_pszFileSpec, lpszFileSpec );

    m_pszFileMask = FileNameSkipDirectory( m_pszFileSpec );

    if ( _T('\\') == *m_pszFileMask )
    {
        m_pszFileMask++;
    }

    // Get long and short versions of starting directory

    STRCPY( (LPTSTR)m_pszShortDir, m_pszFileSpec );
    FileNameStrip( (LPTSTR)m_pszShortDir );

    dwChars = ::GetFullPathName( m_pszShortDir,
                                 m_uMaxPath,
                                 (LPTSTR)m_pszLongDir,
                                 &lpszDummyParam );

    if ( ( 0 == dwChars ) || ( m_uMaxPath < dwChars ) )
        goto BailOut;

    dwChars = ::FileNameGetShortPathName( m_pszLongDir, (LPTSTR)m_pszShortDir, m_uMaxPath );

    if ( ( 0 == dwChars ) || ( m_uMaxPath < dwChars ) )
        goto BailOut;

    dwChars = ::FileNameGetLongPathName( m_pszLongDir, (LPTSTR)m_pszLongDir, m_uMaxPath );

    if ( ( 0 == dwChars ) || ( m_uMaxPath < dwChars ) )
        goto BailOut;

    // Initialize helper class for first level directory.
    // Need to request all files in directory and screen later
    // so that all directories are reported.

    lpszAllFilesSpec = (LPTSTR)MemAllocPtr( GMEM_MOVEABLE, ( STRLEN( m_pszFileSpec ) + STRLEN( szAllFiles ) + 1 ) * sizeof( TCHAR ) );

    if ( NULL == lpszAllFilesSpec )
        goto BailOut;

    STRCPY( lpszAllFilesSpec, m_pszFileSpec );
    FileNameStrip( lpszAllFilesSpec );
    FileNameCat( lpszAllFilesSpec, szAllFiles );

    m_pDirNode = new CDirNode;

    if ( NULL == m_pDirNode )
        goto BailOut;

    nResult = m_pDirNode->m_EnumDir.Init( lpszAllFilesSpec );

BailOut:
    if ( NO_ERROR != nResult )
    {
        nResult = GetLastError();

        CloseAll();
        Clear();
    }

    return ( nResult );
} // Init()


//************************************************************************
// CEnumFiles::Next()
//
// This routine is called repeatedly to step through all files which match
// the input specification.
//
// Parameters:
//      None.
//
// Returns:
//      NO_ERROR                        On success.
//      ERROR_NO_MORE_FILES             If no more files matching spec.
//************************************************************************
// 03/14/1997 DALLEE, created.
//************************************************************************

int CEnumFiles::Next()
{
    auto    LPCTSTR lpszOemReplacementChars;
    auto    UINT    uBogusAttr;
    auto    int     nResult;


    if ( NULL != m_pDirNode )
    {
        for (;;)
        {
            // Get next entry until error, a directory is found, or
            // a filename is found that matches the filemask.

            nResult = m_pDirNode->m_EnumDir.Next();

            if ( NO_ERROR != nResult )
            {
                break;
            }
            else if ( FILE_ATTRIBUTE_DIRECTORY & m_pDirNode->m_EnumDir.m_rFind.dwFileAttributes )
            {
                if ( STRCMP( _T("."), GetLongFile() ) &&
                     STRCMP( _T(".."), GetLongFile() ) )
                {
                    break;
                }
            }
            else
            {
                if ( FileNameMatchSection( m_pszFileMask, GetLongFile() ) ||
                     FileNameMatchSection( m_pszFileMask, GetShortFile() ) )
                {
                    break;
                }
            }
        }

        memcpy( (LPFINDDATA)&m_rFind, &m_pDirNode->m_EnumDir.m_rFind, sizeof( m_rFind ) );

        if ( NO_ERROR == nResult )
        {
            if ( m_uMaxPath <= ( STRLEN( m_pszLongDir )  + STRLEN( GetLongFile() ) ) ||
                 m_uMaxPath <= ( STRLEN( m_pszShortDir ) + STRLEN( GetShortFile() ) ) )
            {
                // Too large for specified buffers.

                SetLastError( ERROR_INSUFFICIENT_BUFFER );
                nResult = ERROR_INSUFFICIENT_BUFFER;
            }
            else
            {
                // Store new full filenames.

                STRCPY( m_pszLongName,  m_pszLongDir );
                STRCPY( m_pszShortName, m_pszShortDir );

                FileNameCat( m_pszLongName,  GetLongFile() );
                FileNameCat( m_pszShortName, GetShortFile() );

                // ANSI/OEM klugery here:
                // Try to detect names that don't translate from ANSI to OEM.
                // In this case if we're running in OEM mode, the LFN will
                // contain bogus replacement characters and the SFN must
                // be used instead. Need to do a fix-up for directories since
                // we'll be using the LFN in calls to Enter().
                // And no, we can't use the SFN in Enter() since the
                // new LFN FindFirstFile API in OEM mode doesn't work w/
                // SFN's on NetWare.

                if ( FILE_ATTRIBUTE_DIRECTORY & m_rFind.dwFileAttributes )
                {
                    // Get set of characters used to replace those which
                    // don't translate from ANSI to OEM.

                    switch ( SystemGetWindowsType() )
                    {
                    default:
                    case SYM_SYSTEM_WIN_NT:
                        lpszOemReplacementChars = s_szOemReplacementCharsNT;
                        break;

                    case SYM_SYSTEM_WIN_95:
                        lpszOemReplacementChars = s_szOemReplacementCharwW95;
                        break;
                    }

                    // Check the LFN if there is an alternate name present,
                    // and the suspect characters are present in it.

                    if ( m_rFind.szAlternateFileName[0] &&
                         STRPBRK( GetLongFile() , lpszOemReplacementChars ) &&
                         ( NOERR != FileGetAttr( m_pszLongName, &uBogusAttr ) ) )
                    {
                        // LFN name seemed bogus, so use SFN for this section.

                        STRCPY( m_pszLongName,  m_pszLongDir );
                        FileNameCat( m_pszLongName, GetShortFile() );
                    }
                }
            }
        }
        else
        {
            *m_pszLongName  = \
            *m_pszShortName = _T('\0');
        }
    }
    else
    {
        nResult = -1;
    }

    return ( nResult );
} // Next()


//************************************************************************
// CEnumFiles::Reset()
//
// This routine restarts the enumeration at the beginning of the current
// directory.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/14/1997 DALLEE, created.
//************************************************************************

int CEnumFiles::Reset()
{
    auto    int     nResult;

    if ( NULL != m_pDirNode )
    {
        nResult = m_pDirNode->m_EnumDir.Reset();

        memcpy( (LPFINDDATA)&m_rFind, &m_pDirNode->m_EnumDir.m_rFind, sizeof( m_rFind ) );

        *m_pszLongName  = \
        *m_pszShortName = _T('\0');
    }
    else
    {
        nResult = -1;
    }

    return ( nResult );
} // Reset()


//************************************************************************
// CEnumFiles::Enter()
//
// This routine enters the current item found (if it is a directory) and
// sets up to begin an enumeration of files in that directory.
//
// Parameters:
//      None.
//
// Returns:
//
//************************************************************************
// 04/18/1997 DALLEE, created.
//************************************************************************

int CEnumFiles::Enter()
{
    auto    CDirNode    *pNode;
    auto    LPTSTR      lpszFileSpec;
    auto    int         nResult;

    // Initialize locals.

    pNode        = NULL;
    lpszFileSpec = NULL;
    nResult      = -1;

    // Verify we have a current item, that it's a directory, and that
    // the new filespec is within max path size.

    if ( ( NULL == m_pDirNode ) ||
         !( FILE_ATTRIBUTE_DIRECTORY & m_rFind.dwFileAttributes ) )
    {
        SetLastError( ERROR_BAD_PATHNAME );
        goto BailOut;
    }

    if ( m_uMaxPath < STRLEN( GetLongName() ) + STRLEN( szAllFiles ) )
    {
        SetLastError( ERROR_INSUFFICIENT_BUFFER );
        goto BailOut;
    }

    // Create and initialize a new single directory enumerator.

    pNode = new CDirNode;

    lpszFileSpec = (LPTSTR) MemAllocPtr( GMEM_MOVEABLE, m_uMaxPath * sizeof( TCHAR ) );

    if ( NULL == lpszFileSpec )
        goto BailOut;

    STRCPY( lpszFileSpec, GetLongName() );
    FileNameCat( lpszFileSpec, szAllFiles );

    if ( NO_ERROR != pNode->m_EnumDir.Init( lpszFileSpec ) )
        goto BailOut;

    // Set up new current directory names.

    FileNameCat( m_pszLongDir,  GetLongFile() );
    FileNameCat( m_pszShortDir, GetShortFile() );

    // Set directory enumerator to our new node.

    pNode->m_pNext = m_pDirNode;
    m_pDirNode = pNode;
    pNode = NULL;

    memcpy( (LPFINDDATA)&m_rFind, &m_pDirNode->m_EnumDir.m_rFind, sizeof( m_rFind ) );

    *m_pszLongName  = \
    *m_pszShortName = _T('\0');

    nResult = NO_ERROR;

BailOut:
    if ( NULL != lpszFileSpec )
        MemFreePtr( lpszFileSpec );

    if ( NULL != pNode )
        delete pNode;

    return ( nResult );
} // Enter()


//************************************************************************
// CEnumFiles::Leave()
//
// This routine closes the current directory being enumerated and jumps
// back to the previous parent directory being enumerated.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/14/1997 DALLEE, created.
//************************************************************************

int CEnumFiles::Leave()
{
    auto    CDirNode    *pNode;
    auto    int         nResult;

    // Verify we have a current directory and parent directory.

    if ( ( NULL != m_pDirNode ) &&
         ( NULL != m_pDirNode->m_pNext ) )
    {
        pNode = m_pDirNode;
        m_pDirNode = pNode->m_pNext;
        pNode->m_pNext = NULL;

        delete pNode;

        // Reset filename data.

        FileNameStrip( m_pszLongDir );
        FileNameStrip( m_pszShortDir );

        memcpy( (LPFINDDATA)&m_rFind, &m_pDirNode->m_EnumDir.m_rFind, sizeof( m_rFind ) );

        STRCPY( m_pszLongName,  m_pszLongDir );
        STRCPY( m_pszShortName, m_pszShortDir );

        FileNameCat( m_pszLongName,  GetLongFile() );
        FileNameCat( m_pszShortName, GetShortFile() );

        nResult = NO_ERROR;
    }
    else
    {
        nResult = -1;
    }

    return ( nResult );
} // Leave()


//************************************************************************
// CEnumFiles::GetLongName()
//
// This routine returns the long name with path of the last item to
// match the file specification.
//
// Parameters:
//      None.
//
// Returns:
//      LPCTSTR     Name of last file found.
//      NULL        Next() not yet called, or had error on last call.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

LPCTSTR CEnumFiles::GetLongName()
{
    return ( m_pszLongName );
} // GetLongName()


//************************************************************************
// CEnumFiles::GetShortName()
//
// This routine returns the short name with path of the last item to
// match the file specification.
//
// Parameters:
//      None.
//
// Returns:
//      LPCTSTR     Name of last file found.
//      NULL        Next() not yet called, or had error on last call.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

LPCTSTR CEnumFiles::GetShortName()
{
    return ( m_pszShortName );
} // GetShortName()


//************************************************************************
// CEnumFiles::GetLongFile()
//
// This routine returns the long name without path of the last item to
// match the file specification.
//
// Parameters:
//      None.
//
// Returns:
//      LPCTSTR     Name of last file found.
//      NULL        Next() not yet called, or had error on last call.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

LPCTSTR CEnumFiles::GetLongFile()
{
    return ( NULL != m_pDirNode ? m_pDirNode->m_EnumDir.GetLongFile()
                                : NULL );
} // GetLongFile()


//************************************************************************
// CEnumFiles::GetShortFile()
//
// This routine returns the short name without path of the last item to
// match the file specification.
//
// Parameters:
//      None.
//
// Returns:
//      LPCTSTR     Name of last file found.
//      NULL        Next() not yet called, or had error on last call.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

LPCTSTR CEnumFiles::GetShortFile()
{
    return ( NULL != m_pDirNode ? m_pDirNode->m_EnumDir.GetShortFile()
                                : NULL );
} // GetShortFile()


//************************************************************************
// CEnumFiles::GetLongDir()
//
// This routine returns the long name of the current directory.
//
// Parameters:
//      None.
//
// Returns:
//      LPCTSTR     Name of last file found.
//      NULL        Next() not yet called, or had error on last call.
//************************************************************************
// 03/14/1997 DALLEE, created.
//************************************************************************

LPCTSTR CEnumFiles::GetLongDir()
{
    return ( m_pszLongDir );
} // GetLongDir()


//************************************************************************
// CEnumFiles::GetShortDir()
//
// This routine returns the short name of the current directory.
//
// Parameters:
//      None.
//
// Returns:
//      LPCTSTR     Name of last file found.
//      NULL        Next() not yet called, or had error on last call.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

LPCTSTR CEnumFiles::GetShortDir()
{
    return ( m_pszShortDir );
} // GetShortDir()


//************************************************************************
// CEnumFiles::GetFileSpec()
//
// This routine gets the filespec currently being enumerated.
// This is the filespec last passed to Init().
//
// Parameters:
//      None.
//
// Returns:
//      LPCTSTR     Filespec currently being enumerated.
//      NULL        Class has not yet been initialized.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

LPCTSTR CEnumFiles::GetFileSpec()
{
    return ( NULL != m_pszFileSpec ? m_pszFileSpec
                                   : NULL );
} // GetFileSpec()


//************************************************************************
// Implementation of CDirNode helper class.

//************************************************************************
// CDirNode::CDirNode()
//
// Constructor which initializes all members to default empty values.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

CDirNode::CDirNode()
{
    m_pNext = NULL;
} // CDirNode()


//************************************************************************
// CDirNode::~CDirNode()
//
// Destructor for CDirNode.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

CDirNode::~CDirNode()
{
    if ( NULL != m_pNext )
    {
        delete m_pNext;
    }
} // ~CDirNode()
