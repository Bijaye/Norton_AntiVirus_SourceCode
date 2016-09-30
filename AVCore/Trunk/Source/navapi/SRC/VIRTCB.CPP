 // Copyright 1998 Symantec Corporation.
//*************************************************************************
// virtcb.cpp - created 6/9/98 11:30:05 AM
//
// $Header:   S:/NAVAPI/VCS/virtcb.cpv   1.12   15 Sep 1998 14:13:20   MKEATIN  $
//
// Description:
//      Routines for supporting both NAVAPI client-defined virtual file IO
//      and standard IO with a single set of engine callbacks.
//
//      The VirtFileXXX() functions determine the proper type of routine
//      to call (client-defined or standard) based on input specifiction,
//      or file handle.
//      All engile file callbacks will go through these VirtFileXXX()
//      functions.
//
// Contains:
//      EncodeVirtFileId()
//
//      VirtFileOpen()
//      VirtFileCreate()
//      VirtGetTempFile()
//      VirtFileClose()
//      VirtFileRead()
//      VirtFileWrite()
//      VirtFileSeek()
//      VirtFileLength()
//      VirtFileGetDateTime()
//      VirtFileSetDateTime()
//      VirtFileDelete()
//      VirtFileGetAttr()
//      VirtFileSetAttr()
//*************************************************************************
// $Log:   S:/NAVAPI/VCS/virtcb.cpv  $
//
//    Rev 1.12   15 Sep 1998 14:13:20   MKEATIN
// Changed bExecutable to lpstFileExt
//
//    Rev 1.11   01 Sep 1998 11:52:16   dhertel
// Fixed VirtGetTempFile() for NLM; was deleting existing temp files
// used by other threads.
//
//    Rev 1.10   24 Aug 1998 18:37:02   dhertel
// Fixed VirtGetTempFile() for NLM (again).  Removed 0x80000000 attribute
// bit which was causing creates to fail.
//
//    Rev 1.9   21 Aug 1998 18:00:24   dhertel
// Implemented VirtGetTempFile for NLM.
//
//    Rev 1.8   30 Jul 1998 21:51:56   DHERTEL
// Changes for NLM packaging of NAVAPI.
//
//    Rev 1.7   29 Jul 1998 18:33:50   DHERTEL
// DX and NLM changes
//
//    Rev 1.6   28 Jul 1998 15:14:20   MKEATIN
// Now using strtoul instead of sscanf - strtoul is multi-platform friendly.
//
//    Rev 1.5   27 Jul 1998 18:31:56   MKEATIN
// SSCANF is sscanf for all non WIN32 platforms.
//
//    Rev 1.4   27 Jul 1998 18:25:12   dhertel
// Added handle table stuff for 16 bit platforms
//
//    Rev 1.3   24 Jun 1998 13:38:38   DALLEE
// Added legit implementation of virtual client-defined FileDelete.
//
//    Rev 1.2   22 Jun 1998 17:00:16   DALLEE
// Go through new client Get/SetFileDateTime() callbacks.
//
//    Rev 1.1   18 Jun 1998 12:29:32   DALLEE
// Fixed problem in FileSize() implementation -
// Was leaving file pointer at start of file, not current position.
//
//    Rev 1.0   11 Jun 1998 17:38:40   DALLEE
// Initial revision.
//*************************************************************************

#include "platform.h"
#include "stdio.h"
#include "stdlib.h"
#include "xapi.h"
#include "file.h"

#include "callbk15.h"
#include "nlm_nav.h"
#include "navapi.h"
#include "virtcb.h"

#if defined(SYM_NLM)
#include <dos.h>    // the offical include file for netware is NLMDOS.H though
#endif

//*************************************************************************
// Local data types
//*************************************************************************

// VIRTOPENFILEINFO is a structure allocated to track the type of
// file handle returned from FileOpen() callbacks.
//
// Platforms with 32-bit HFILE:
//  VirtFileOpen() will return a pointer to a VIRTOPENFILEINFO struct.
// Platforms with 16-bit HFILE:
//  VirtFileOpen() will return a 16 bit cookie or key that can be used to
//  retrieve the 32-bit pointer to a VIRTOPENFILEINFO struct.
// All platforms:
//  Callers of VirFileOpen() should make no assumptions about the HFILE value.
//  The VirtFile functions will always use GetOpenFileInfo() to convert
//  the HFILE into an LPVIRTOPENFILEINFO.
//  dwFileHandle will contain either a standard file handle for the
//  current platform, or a 32-bit value returned from a client virtual
//  file callback.
//  FileRead/Write/GetAttr/etc() will determine the handle type based on
//  the nType member and call either a platform specific, or client virtual
//  file callback.

typedef enum VIRTOPENFILETYPE
{
    VOFT_STDIO,                         // Platform standard open file
    VOFT_VIRTUAL                        // Virtual file using client callbacks
} VIRTOPENFILETYPE;

typedef struct tagVIRTOPENFILEINFO
{
    VIRTOPENFILETYPE    nType;
    LPVOID              lpvHandle;
} VIRTOPENFILEINFO, FAR *LPVIRTOPENFILEINFO;

#if defined(SYM_NLM)
// Used by NLM_SetPurgeFlag for arguments to SetFileInfo
typedef struct
{ 
    struct _DOSDate date;
    struct _DOSTime time;
} DOSDateTime;
#endif

//*************************************************************************
// Global data
//*************************************************************************

// Encoded virtual file ID pointers:
// Strings for constructing string representations of virtual file ID
// input.
// Format is:
//      <s_szVirtFilenameTag> + "\" +
//      <8 character uppercase ASCII hex representation of file ID pointer> + "." +

#if defined(SYM_UNIX)
static TCHAR s_szVirtFilenameTag [] = _T("/18328BE0-FF99-11D1-803F-00104B252EE6");
#else
static TCHAR s_szVirtFilenameTag [] = _T("0:\\18328BE0-FF99-11D1-803F-00104B252EE6");
#endif

#define STRLEN_VIRTFILENAMETAG ((sizeof(s_szVirtFilenameTag)/sizeof(*s_szVirtFilenameTag))-1)

//
// Handle management:
//
// On platforms were HFILE's are 16 bits, we need to map HFILEs into
// 32 bit pointers.  This is done with a fixed size array called the
// Open File Table.  An HFILE value is just an index into this table.
// An 'in use' table entry points to a VIRTOPENFILEINFO structure
// dynamically allocated from the heap.  'Unused' table entries
// have integer values that link the entries into a free list.
//
// If the fixed size table is unsatisfactory, I suggest changing
// to a linked list of handle tables, and replacing the free list
// with just NULL values (using loops to find free entries).  An
// HVALUE could quickly be converted into a number of table links
// to follow and a fixed index into the subtable.
//

// Define the symbol USEFILETABLE for platforms where
// sizeof(HFILE) < sizeof(LPVIRTOPENFILEINFO)

#if defined(SYM_DOS) || defined(SYM_WIN16)
#define USEFILETABLE
#endif

#if defined(USEFILETABLE)

// The number of open files could be quite large if a server product
// is using multiple threads to scan files.

#define MAXOPENFILES 4096

// Pointer to dynamically allocated Open File Table
static LPVIRTOPENFILEINFO FAR* s_lpOpenFileTable = NULL;

// First available HFILE
int s_nFirstFreeHandle = -1;

#endif


//*************************************************************************
// Local prototypes
//*************************************************************************

static BOOL GetVirtFileId ( LPTSTR       lpszFileName,
                            LPLPVOID FAR *lpvFileId );

static HFILE AllocOpenFileInfoHandle ( LPVIRTOPENFILEINFO lpOpenFileInfo );
static LPVIRTOPENFILEINFO GetOpenFileInfo ( HFILE hFile );
static void FreeOpenFileInfoHandle ( HFILE hFile );
#if defined(SYM_NLM)
BOOL WINAPI NLM_SetPurgeFlag( LPCTSTR lpszFileName );
#endif


//*************************************************************************
// Code
//*************************************************************************

//*************************************************************************
// EncodeVirtFileId()
//
// void EncodeVirtFileId (
//      LPTSTR  lpszEncodedId,           [out] Buffer for encoded path, SYM_MAX_PATH chars.
//      LPVOID  lpvFileId,               [in] Pointer to client-define file ID.
//      LPCTSTR lpstFileExt )            [in] File extention (EXT, COM, DOT, or NULL)
//
// This routine encodes a pointer to a client-defined file ID into
// string form which can be passed to AVAPI.
// Later decoded by GetVirtFileId().
//
// See note on "Encoded virtual file ID pointers" above.
//
// Internal use by NAVScanFile() only.
//
// Returns:
//      Nothing, lpszEncodedId has string with encoded file ID pointer.
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

void EncodeVirtFileId ( LPTSTR  lpszEncodedId,
                        LPVOID  lpvFileId,
                        LPCTSTR lpstFileExt)
{
#if defined(SYM_400)
    auto    TCHAR       szEncodedPointer [ 32 + 1 ];
    auto AS400PTR as400union;
    as400union.lpVoid = lpvFileId;
#else
    auto    TCHAR       szEncodedPointer [ 8 + 1 ];
#endif

    auto    int         extlen;

    SYM_ASSERT( lpszEncodedId );

    STRCPY( lpszEncodedId, s_szVirtFilenameTag );

#if defined(SYM_400)
    for (int i = 0; i < 4; i++)
    {
        SPRINTF( &szEncodedPointer[8*i], "%08lX", as400union.uVoid[i] );
    }
#else
    SPRINTF( szEncodedPointer, "%08lX", lpvFileId );
#endif

    NameAppendFile( lpszEncodedId, szEncodedPointer );

    // append the passed file extention
    // The extension is only needed by individual engines for specific
    // extensions.  In order to prevent a buffer overflow we just nuke
    // the extension if it's longer then anything we currently look for
    // (which is 4 'html' plus a nice delta to give us some breathing room.
    // append "NAV" as the file extention if lpstFileExt is NULL

    if (lpstFileExt)
    {
        extlen = STRLEN(lpstFileExt);
    }
    else
    {
        extlen = 0;
    }

    if (extlen > 20 || extlen == 0)
    { 
        NameAppendExtension( lpszEncodedId, _T("NAV") );
    } 
    else
    {
        NameAppendExtension( lpszEncodedId,lpstFileExt );
    }
} // EncodeVirtFileId()


//*************************************************************************
// GetVirtFileId()
//
// static BOOL GetVirtFileId(
//      LPTSTR       lpszFilename,      [in] Filename, possibly an encoded virtual file ID pointer.
//      LPLPVOID FAR *lplpvFileId )     [out] on exit, virtual file ID pointer, or NULL.
//
// This routine determines whether the filename passed in is an encoded
// virtual file ID pointer. If it is, a pointer to the client-defined
// file ID input is returned.
//
// See note on "Encoded virtual file ID pointers" above.
//
// Returns:
//      TRUE    if lpszFileName is an encoded virtual file ID pointer.
//                  *lplpvFileId is set to the virutal file ID pointer value.
//      FALSE   if lpszFileName is a normal filename.
//                  *lplpvFileId is set to NULL.
//*************************************************************************
// 6/9/98 DALLEE, created.
//*************************************************************************

static BOOL GetVirtFileId ( LPTSTR      lpszFilename,
                            LPVOID FAR *lplpvFileId )
{
    auto    TCHAR   szFile [ SYM_MAX_PATH + 1 ];
    auto    BOOL    bVirtFilename;

    SYM_ASSERT( lpszFilename );
    SYM_ASSERT( lplpvFileId );

    // Check for special tag indicating encoded file ID input.

    if ( 0 == STRNCMP( lpszFilename, s_szVirtFilenameTag, STRLEN_VIRTFILENAMETAG ) )
    {
        // Special encoded file ID input.

        // Get and verify encoded pointer portion of string.

        NameReturnFile( lpszFilename, szFile );
        NameStripExtension( szFile );

#if defined(SYM_400)
        if ( ( 32 == STRLEN( szFile ) ) &&
             ( 32 == STRSPN( szFile, _T("0123456789ABCDEF") ) ) )
        {
            auto AS400PTR as400union;

			for (int i = 0; i < 4; i++)
			{
			    as400union.uVoid[i] = strtoul( szFile, NULL, 0x10);	
			}
            *lplpvFileId = as400union.lpVoid;
#else   // !SYM400
        if ( ( 8 == STRLEN( szFile ) ) &&
             ( 8 == STRSPN( szFile, _T("0123456789ABCDEF") ) ) )
        {
            *lplpvFileId = (LPVOID)strtoul( szFile, NULL, 0x10 );
#endif  // (SYM_400)
            bVirtFilename = TRUE;
        }
        else
        {
            // Bad format on what we expected to be an 8 character
            // uppercase ASCII hex string representation of a pointer.

            SYM_ASSERT( FALSE );

            *lplpvFileId = NULL;
            bVirtFilename = FALSE;
        }
    }
    else
    {
        // Normal filename input.

        *lplpvFileId = NULL;
        bVirtFilename = FALSE;
    }

    return ( bVirtFilename );
} // GetVirtFileId()


//*************************************************************************
// AllocOpenFileInfoHandle()
//
// static HFILE AllocOpenFileInfoHandle(
//      LPVIRTOPENFILEINFO lpOpenFileInfo )	[in]
//
// This routine generates a unique HFILE value for an LPVIRTOPENFILEINFO
// pointer.  The HFILE may only be used for two purposes:
// 1) As an argument to GetOpenFileInfo() to retrieve the LPVIROPENFILEINFO
//    value.
// 2) As an argument to FreeOpenFileInfoHandle() to release the handle.
//
// Returns:
//      On success, an HFILE value other than HFILE_ERROR
//      On error, HFILE_ERROR.
//*************************************************************************
// 7/22/98 DHERTEL, created.
//*************************************************************************

static HFILE AllocOpenFileInfoHandle( LPVIRTOPENFILEINFO lpOpenFileInfo )
{
#if defined(USEFILETABLE)

    auto    int			i;
    auto    HFILE		hFile;

    // If there is no open file table allocated yet...
    if (s_lpOpenFileTable == NULL)
    {
        // Allocate the open file table.
        s_lpOpenFileTable = (LPVIRTOPENFILEINFO FAR*) MemAllocPtr( GHND, sizeof( LPVIRTOPENFILEINFO ) * MAXOPENFILES );
        if (s_lpOpenFileTable == NULL)
            return ( HFILE_ERROR );

        // Link the table entries into a list of free handle values.
        s_nFirstFreeHandle = 0;
        for ( i = 0; i < MAXOPENFILES - 1; i++)
            s_lpOpenFileTable[i] = (LPVIRTOPENFILEINFO) (i + 1);
        s_lpOpenFileTable[MAXOPENFILES - 1] = (LPVIRTOPENFILEINFO) -1;

        // I am assuming it is OK to leave the table allocated
	// until the program terminates and that the OS will
	// clean up the resources properly.  If this is not so,
	// then FreeOpenFileInfo() could free the table whenever
	// all handles are deleted (we could maintain a count).
    }

    // If there are no more free handles, return an error.
    if (s_nFirstFreeHandle == -1)
        return ( HFILE_ERROR );

    // Remove the handle from the head of the free handle list.
    hFile = (HFILE) s_nFirstFreeHandle;
    s_nFirstFreeHandle = (int) s_lpOpenFileTable[s_nFirstFreeHandle];

    // Assign the lpOpenFileInfo pointer to the hFile'th table entry.
    s_lpOpenFileTable[hFile] = lpOpenFileInfo;

    // Return the handle.
    return ( hFile );

#else

    // The HFILE value and the LPVIRTOPENFILEINFO pointer
    // are the same size and have the same value.
    return ( (HFILE) lpOpenFileInfo );

#endif

} // AllocOpenFileInfoHandle()


//*************************************************************************
// GetOpenFileInfo()
//
// static LPVIRTOPENFILEINFO GetOpenFileInfo (
//      HFILE hFile )                   [in]
//
// This routine retrieves the LPVIROPENFILEINFO pointer from an HFILE value.
// The HFILE must be from a call to AllocOpenFileInfoHandle(), which
// is used by VirtFileOpen(), etc.
//
// The VIRTOPENFILEINFO structure can be used to determine the type of
// acutal file handle (client-defined, or standard IO) and access the
// actual handle.
//
// Returns:
//	Pointer to VIRTOPENFILEINFO structure.
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

static LPVIRTOPENFILEINFO GetOpenFileInfo ( HFILE hFile )
{
#if defined(USEFILETABLE)

    return ( s_lpOpenFileTable[(int) hFile] );

#else

    // The HFILE value and the LPVIRTOPENFILEINFO pointer
    // are the same size and have the same value.
    return ( (LPVIRTOPENFILEINFO) hFile );

#endif
} // GetOpenFileInfo()


//*************************************************************************
// FreeOpenFileInfoHandle()
//
// static void FreeOpenFileInfoHandle(
//      HFILE hFile )			[in]
//
// This routine releases the specified HFILE value.
// It does NOT free the associated LPVIRTOPENFILEINFO pointer.
//
// Returns:
//	Nothing.
//*************************************************************************
// 7/22/98 DHERTEL, created.
//*************************************************************************

static void FreeOpenFileInfoHandle( HFILE hFile )
{
#if defined(USEFILETABLE)

    // Add the handle to the head of the free handle list.
    s_lpOpenFileTable[(int) hFile] = (LPVIRTOPENFILEINFO) s_nFirstFreeHandle;
    s_nFirstFreeHandle = (int) hFile;

#else

    // The HFILE value and the LPVIRTOPENFILEINFO pointer
    // are the same size and have the same value.
    // There is nothing to do in this case!

#endif
} // FreeOpenFileInfoHandle()


//*************************************************************************
// File IO (client/standard) covers
//*************************************************************************

//*************************************************************************
// VirtFileOpen()
//
// BOOL DOS_LOADDS WINAPI VirtFileOpen(
//      LPTSTR lpszFilename,  [in]  File to open, possibly an encoded virtual file ID pointer.
//      UINT   uMode,         [in]  Open mode - same as Core's FileOpen().
//      HFILE FAR *lphFile)   [out] Open file handle. NOTE: This is not a real
//                                  file handle; it can only be used with
//                                  GetOpenFileInfo() to retrieve a pointer
//                                  to an VIRTOPENFILEINFO structure.
//
// All NAVAPI's engine callback FileOpen()'s route through this function.
// This function determines whether the input filespec is an encoded pointer
// to a client-defined file ID, or a regular filename.
// The correct FileOpen (client callback, or standard platform IO) is called.
//
// Returns:
//      On success, a non-zero value.
//      On error, zero.
//*************************************************************************
// 6/9/98 DALLEE, created.
// 3/5/99 JimBrennan, added a BOOL return value and the lphFile parameter.
//*************************************************************************

BOOL DOS_LOADDS WINAPI VirtFileOpen (
    LPTSTR     lpszFilename,
    UINT       uMode,
    HFILE FAR *lphFile)
{
    auto    LPVOID              lpvFileId;
    auto    LPVIRTOPENFILEINFO  lpOpenFileInfo;
    auto    LPNAVFILEIO         lpFileCallbacks;
    auto    BOOL                bSuccess;

    SYM_ASSERT( lpszFilename );

    // Allocate a file info structure.

    lpOpenFileInfo = (LPVIRTOPENFILEINFO) MemAllocPtr( GHND, sizeof( VIRTOPENFILEINFO ) );

    if ( NULL == lpOpenFileInfo )
    {
        return ( FALSE );
    }

    // Allocate a corresponding handle.

    *lphFile = AllocOpenFileInfoHandle( lpOpenFileInfo );

    if ( *lphFile == HFILE_ERROR)
    {
        MemFreePtr( lpOpenFileInfo );
        return ( FALSE );
    }

    // Check for encoded virtual file ID...

    if ( GetVirtFileId( lpszFilename, &lpvFileId ) )
    {
        // Encoded virtual file ID.

        lpFileCallbacks = GetFileIoCallbacks();
        SYM_ASSERT( lpFileCallbacks->Open );

        lpOpenFileInfo->nType = VOFT_VIRTUAL;
        bSuccess = lpFileCallbacks->Open( lpvFileId, uMode, &lpOpenFileInfo->lpvHandle );

        if ( !bSuccess )
        {
            MemFreePtr( lpOpenFileInfo );
            FreeOpenFileInfoHandle( *lphFile );
            *lphFile = (HFILE)0;
            return ( FALSE );
        }
    }
    else
    {
        // Regular filename.

        lpOpenFileInfo->nType = VOFT_STDIO;

#ifndef SYM_NLM
        lpOpenFileInfo->lpvHandle = (LPVOID) FileOpen( lpszFilename, uMode );
#else
        lpOpenFileInfo->lpvHandle = (LPVOID) NV_FileOpen( lpszFilename, uMode );
#endif

        if ( (DWORD) HFILE_ERROR == (DWORD)lpOpenFileInfo->lpvHandle )
        {
            MemFreePtr( lpOpenFileInfo );
            FreeOpenFileInfoHandle( *lphFile );
            *lphFile = (HFILE)0;
            return ( FALSE );
        }
    }

    return ( TRUE );

} // VirtFileOpen()


//*************************************************************************
// VirtFileCreate()
//
// HFILE DOS_LOADDS WINAPI VirtFileCreate(
//      LPTSTR lpszFilename,            [in] File to open, possibly an encoded virtual file ID pointer.
//      UINT   uFlags )                 [in] Creation flags - same as Core's FileCreate().
//
// All NAVAPI's engine callback FileCreate()'s route through this function.
// This function determines whether the input filespec is an encoded pointer
// to a client-defined file ID, or a regular filename.
//
// For standard filenames, the file is created.
//
// For client-defined file ID's, or filenames based on client-defined file ID's,
// this function always returns error. In debug mode, this function will
// also fire an assertion - this should never be called in this manner.
//
// Returns:
//      On success, an HFILE value.
//          NOTE: This is not a real file handle; it can only be used with
//          GetOpenFileInfo() to retrieve a pointer to an VIRTOPENFILEINFO
//          structure.
//      On error, HFILE_ERROR.
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

HFILE DOS_LOADDS WINAPI VirtFileCreate (
    LPTSTR  lpszFilename,
    UINT    uFlags )
{
    auto    LPVOID              lpvFileId;
    auto    LPVIRTOPENFILEINFO  lpOpenFileInfo;
    auto    HFILE               hFile;

    SYM_ASSERT( lpszFilename );

    // Check for encoded virtual file ID...

    if ( GetVirtFileId( lpszFilename, &lpvFileId ) )
    {
        // Encoded virtual file ID.

        return ( HFILE_ERROR );
    }
    else
    {
        // Regular filename.

        // Allocate a file info structure.

        lpOpenFileInfo = (LPVIRTOPENFILEINFO) MemAllocPtr( GHND, sizeof( VIRTOPENFILEINFO ) );

        if ( NULL == lpOpenFileInfo )
        {
            return ( HFILE_ERROR );
        }

	// Allocate a corresponding handle.

        hFile = AllocOpenFileInfoHandle( lpOpenFileInfo );

        if ( hFile == HFILE_ERROR)
        {
            MemFreePtr( lpOpenFileInfo );
            return ( HFILE_ERROR );
        }


        lpOpenFileInfo->nType     = VOFT_STDIO;
        lpOpenFileInfo->lpvHandle = (LPVOID) FileCreate( lpszFilename, uFlags );

        if ( (DWORD) HFILE_ERROR == (DWORD)lpOpenFileInfo->lpvHandle )
        {
            MemFreePtr( lpOpenFileInfo );
            FreeOpenFileInfoHandle( hFile );
            return ( HFILE_ERROR );
        }
    }

    return ( hFile );
} // VirtFileCreate()


//*************************************************************************
// VirtGetTempFile()
//
// HFILE DOS_LOADDS WINAPI VirtGetTempFile (
//      LPTSTR  lpszFileName,           [out] Name of file created.
//      LPVOID  lpvCookie )             [in] unused since we can't currently get a value here from outside AVAPI
//
// This routine will create a temp file using standard (not client-defined) IO
// and return the name of and a handle to the file created.
//
// Returns:
//      On success, an HFILE value.
//          NOTE: This is not a real file handle; it can only be used with
//          GetOpenFileInfo() to retrieve a pointer to an VIRTOPENFILEINFO
//          structure.
//      On error, HFILE_ERROR.
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

HFILE DOS_LOADDS WINAPI VirtGetTempFile
(
    LPTSTR          lpszFileName,
    LPVOID          lpvCookie
)
{
    auto    LPVIRTOPENFILEINFO  lpOpenFileInfo;
    auto    HFILE     		    hFile;

    // Allocate a file info structure.

    lpOpenFileInfo = (LPVIRTOPENFILEINFO) MemAllocPtr( GHND, sizeof( VIRTOPENFILEINFO ) );

    if ( NULL == lpOpenFileInfo )
    {
        return ( HFILE_ERROR );
    }

    // Allocate a corresponding handle.

    hFile = AllocOpenFileInfoHandle( lpOpenFileInfo );

    if ( hFile == HFILE_ERROR)
    {
        MemFreePtr( lpOpenFileInfo );
        return ( HFILE_ERROR );
    }

#if defined(SYM_DOS) || defined(SYM_WIN) || defined(SYM_UNIX)

    lpOpenFileInfo->lpvHandle = (LPVOID) FileCreateTemporary( lpszFileName, FA_NORMAL );

#elif defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_NLM)
    {
    auto   TCHAR     szGeneratedFileName[13];
    auto   int       i, j, iNum;
 #if defined(SYM_NLM)
    static int       iFileNum = 0;
 #endif

    // Retry up to 10000 times.
    for ( i = 0; i < 9999; i++ )
    {
        // Choose a number to use in the filename.
 #if defined(SYM_NLM)
        iNum = iFileNum;
        iFileNum = (iFileNum + 1) % 10000;
 #else
        iNum = i;
 #endif

        // Generate a name of the form nnnnNAV~.TMP.
        for ( j = 3; j >= 0; j-- )
              szGeneratedFileName[j] = _T('0') + (iNum % 10), iNum /= 10;
        STRCPY( &szGeneratedFileName[4], _T("NAV~.TMP") );

        // Generate a full path name using the defs dir as the location.
        STRCPY( lpszFileName, GetDefsPath() );
        NameAppendFile( lpszFileName, szGeneratedFileName );

        // Create and open the file.
 #if defined(SYM_VXD) || defined(SYM_NTK)
        // x8000000 means use FILE_CREATE instead of FILE_OVERWRITE_IF.
        lpOpenFileInfo->lpvHandle = (LPVOID) FileCreate( lpszFileName, FA_NORMAL | 0x80000000L );
 #elif defined(SYM_NLM)
        // Fail if the file already exists.
        lpOpenFileInfo->lpvHandle = (LPVOID)open( lpszFileName,
            O_BINARY | O_CREAT | O_EXCL | O_RDWR | O_TRUNC,
            S_IREAD | S_IWRITE );
 #endif
        lpOpenFileInfo->nType = VOFT_STDIO;

        // If successful, stop retrying.
        if ( (DWORD)HFILE_ERROR != (DWORD)lpOpenFileInfo->lpvHandle )
            break;
    }
  }
#endif

    if ( (DWORD) HFILE_ERROR == (DWORD)lpOpenFileInfo->lpvHandle )
    {
        MemFreePtr( lpOpenFileInfo );
        FreeOpenFileInfoHandle( hFile );
        return ( HFILE_ERROR );
    }

    return ( hFile );

} // VirtGetTempFile()


//*************************************************************************
// VirtFileClose()
//
// HFILE DOS_LOADDS WINAPI VirtFileClose (
//      HFILE   hFile )
//
// Closes a file handle returned by VirtFileOpen().
//
// All NAVAPI's engine callback FileClose()'s route through this function.
// This function determines whether the input file handle represents
// a client-defined file handle, or a regular file handle.
// The correct FileClose (client callback, or standard platform IO) is called.
//
// Returns:
//      HFILE_ERROR on error.
//      Any other value on success.
//*************************************************************************
// 6/9/98 DALLEE, created.
//*************************************************************************

HFILE DOS_LOADDS WINAPI VirtFileClose (
    HFILE   hFile )
{
    auto    LPVIRTOPENFILEINFO  lpOpenFileInfo;
    auto    LPNAVFILEIO         lpFileCallbacks;
    auto    HFILE               hResult;

//  SYM_ASSERT( hFile );

    if ( HFILE_ERROR == hFile )
    {
        // I don't think we attempt to do this...
        // I do know that we shouldn't.
        SYM_ASSERT( FALSE );

        return ( HFILE_ERROR );
    }

    // Translate back to our open file info structure.
    // Call appropriate close function and free structure.

    lpOpenFileInfo = GetOpenFileInfo( hFile );

    switch ( lpOpenFileInfo->nType )
    {
    case VOFT_VIRTUAL:
        lpFileCallbacks = GetFileIoCallbacks();
        SYM_ASSERT( lpFileCallbacks->Close );

        hResult = lpFileCallbacks->Close( lpOpenFileInfo->lpvHandle ) ?
                    (HFILE) 0 : HFILE_ERROR;
        break;

    case VOFT_STDIO:
        hResult = FileClose( (HFILE) lpOpenFileInfo->lpvHandle );
        break;

    default:
        SYM_ASSERT( FALSE );

        hResult = HFILE_ERROR;
        break;
    }

    MemFreePtr( lpOpenFileInfo );
    FreeOpenFileInfoHandle( hFile );

    return ( hResult );
} // VirtFileClose()


//*************************************************************************
// VirtFileRead()
//
// UINT DOS_LOADDS WINAPI VirtFileRead (
//      HFILE   hFile,                  [in] handle returned by VirtFileOpen()
//      LPVOID  lpBuffer,               [out] buffer to receive data
//      UINT    uBytes )                [in] bytes to read.
//
// Reads from a file handle returned by VirtFileOpen().
//
// All NAVAPI's engine callback FileRead()'s route through this function.
// The correct FileRead (client callback, or standard platform IO) is called
// based on handle type passed.
//
// Returns:
//      Number of bytes read on success.
//      (UINT)-1 on error.
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

UINT DOS_LOADDS WINAPI VirtFileRead
(
    HFILE   hFile,
    LPVOID  lpBuffer,
    UINT    uBytes
)
{
    auto    LPVIRTOPENFILEINFO  lpOpenFileInfo;
    auto    LPNAVFILEIO         lpFileCallbacks;
    auto    UINT                uBytesRead;

    SYM_ASSERT( lpBuffer );
    SYM_ASSERT( uBytes );
//  SYM_ASSERT( hFile );

    if ( HFILE_ERROR == hFile )
    {
        SYM_ASSERT( FALSE );

        return ( (UINT) -1 );
    }

    // Translate back to our open file info structure.
    // Call appropriate read function.

    lpOpenFileInfo = GetOpenFileInfo( hFile );

    switch ( lpOpenFileInfo->nType )
    {
    case VOFT_VIRTUAL:
        lpFileCallbacks = GetFileIoCallbacks();
        SYM_ASSERT( lpFileCallbacks->Read );

        uBytesRead = lpFileCallbacks->Read( lpOpenFileInfo->lpvHandle, lpBuffer, uBytes );
        break;

    case VOFT_STDIO:
        uBytesRead = FileRead( (HFILE) lpOpenFileInfo->lpvHandle, lpBuffer, uBytes );
        break;

    default:
        SYM_ASSERT( FALSE );

        uBytesRead = (UINT) -1;
        break;
    }

    return ( uBytesRead );
} // VirtFileRead()


//*************************************************************************
// VirtFileWrite()
//
// UINT DOS_LOADDS WINAPI VirtFileWrite (
//      HFILE   hFile,                  [in] handle returned by VirtFileOpen()
//      LPVOID  lpBuffer,               [in] buffer with data to write
//      UINT    uBytes )                [in] bytes to write
//
// Writes to a file handle returned by VirtFileOpen().
//
// All NAVAPI's engine callback FileWrite()'s route through this function.
// The correct FileWrite (client callback, or standard platform IO) is called
// based on handle type passed.
//
// This function will terminate a file at the current position if called
// to write 0 bytes.
//
// Returns:
//      Number of bytes written on success.
//      (UINT)-1 on error.
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

UINT DOS_LOADDS WINAPI VirtFileWrite
(
    HFILE   hFile,
    LPVOID  lpBuffer,
    UINT    uBytes
)
{
    auto    LPVIRTOPENFILEINFO  lpOpenFileInfo;
    auto    LPNAVFILEIO         lpFileCallbacks;
    auto    UINT                uBytesWritten;

    SYM_ASSERT( lpBuffer || ( 0 == uBytes ) );
//  SYM_ASSERT( hFile );

    if ( HFILE_ERROR == hFile )
    {
        SYM_ASSERT( FALSE );

        return ( (UINT) -1 );
    }

    // Translate back to our open file info structure.
    // Call appropriate write function.

    lpOpenFileInfo = GetOpenFileInfo( hFile );

    switch ( lpOpenFileInfo->nType )
    {
    case VOFT_VIRTUAL:
        lpFileCallbacks = GetFileIoCallbacks();
        SYM_ASSERT( lpFileCallbacks->Write );

        uBytesWritten = lpFileCallbacks->Write( lpOpenFileInfo->lpvHandle, lpBuffer, uBytes );
        break;

    case VOFT_STDIO:
        uBytesWritten = FileWrite( (HFILE) lpOpenFileInfo->lpvHandle, lpBuffer, uBytes );
        break;

    default:
        SYM_ASSERT( FALSE );

        uBytesWritten = (UINT) -1;
        break;
    }

    return ( uBytesWritten );
} // VirtFileWrite()


//*************************************************************************
// VirtFileSeek()
//
// DWORD DOS_LOADDS WINAPI VirtFileSeek (
//      HFILE   hFile,                  [in] handle returned by VirtFileOpen()
//      LONG    lOffset,                [in] offset to seek
//      int     nFrom )                 [in] start position as CORE's FileSeek()
//
// Sets the file pointer for a file handle returned by VirtFileOpen().
//
// All NAVAPI's engine callback FileSeek()'s route through this function.
// The correct FileSeek (client callback, or standard platform IO) is called
// based on handle type passed.
//
// Returns:
//      New offset on success.
//      (DWORD) -1 on error.
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

DWORD DOS_LOADDS WINAPI VirtFileSeek
(
    HFILE   hFile,
    LONG    lOffset,
    int     nFrom
)
{
    auto    LPVIRTOPENFILEINFO  lpOpenFileInfo;
    auto    LPNAVFILEIO         lpFileCallbacks;
    auto    DWORD               dwPosition;

//  SYM_ASSERT( hFile );

    if ( HFILE_ERROR == hFile )
    {
        SYM_ASSERT( FALSE );

        return ( (DWORD) -1 );
    }

    // Translate back to our open file info structure.
    // Call appropriate seek function.

    lpOpenFileInfo = GetOpenFileInfo( hFile );

    switch ( lpOpenFileInfo->nType )
    {
    case VOFT_VIRTUAL:
        lpFileCallbacks = GetFileIoCallbacks();
        SYM_ASSERT( lpFileCallbacks->Seek );

        dwPosition = lpFileCallbacks->Seek( lpOpenFileInfo->lpvHandle, lOffset, nFrom );
        break;

    case VOFT_STDIO:
        dwPosition = FileSeek( (HFILE) lpOpenFileInfo->lpvHandle, lOffset, nFrom );
        break;

    default:
        SYM_ASSERT( FALSE );

        dwPosition = (DWORD) -1;
        break;
    }

    return ( dwPosition );
} // VirtFileSeek()


//*************************************************************************
// VirtFileLength()
//
// DWORD DOS_LOADDS WINAPI VirtFileLength (
//      HFILE   hFile )                 [in] Handle returned by VirtFileOpen().
//
// Sets the file pointer for a file handle returned by VirtFileOpen().
//
// All NAVAPI's engine callback FileLength()'s route through this function.
// The correct FileLength (client callback, or standard platform IO) is called
// based on handle type passed.
//
// Returns:
//      Length of file on success.
//      (DWORD) -1 on error.
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

DWORD DOS_LOADDS WINAPI VirtFileLength
(
    HFILE   hFile
)
{
    auto    LPVIRTOPENFILEINFO  lpOpenFileInfo;
    auto    LPNAVFILEIO         lpFileCallbacks;
    auto    DWORD               dwCurrentPosition;
    auto    DWORD               dwLength;

//  SYM_ASSERT( hFile );

    if ( HFILE_ERROR == hFile )
    {
        SYM_ASSERT( FALSE );

        return ( (DWORD) -1 );
    }

    // Translate back to our open file info structure.
    // Call appropriate length function.

    lpOpenFileInfo = GetOpenFileInfo( hFile );

    switch ( lpOpenFileInfo->nType )
    {
    case VOFT_VIRTUAL:
        lpFileCallbacks = GetFileIoCallbacks();
        SYM_ASSERT( lpFileCallbacks->Seek );

        dwCurrentPosition = lpFileCallbacks->Seek( lpOpenFileInfo->lpvHandle, 0, SEEK_CUR );

        if ( (DWORD) -1 != dwCurrentPosition )
        {
            dwLength = lpFileCallbacks->Seek( lpOpenFileInfo->lpvHandle, 0, SEEK_END );
            lpFileCallbacks->Seek( lpOpenFileInfo->lpvHandle, dwCurrentPosition, SEEK_SET );
        }
        else
        {
            dwLength = (DWORD) -1;
        }

        break;

    case VOFT_STDIO:
        dwLength = FileLength( (HFILE) lpOpenFileInfo->lpvHandle );
        break;

    default:
        SYM_ASSERT( FALSE );

        dwLength = (DWORD) -1;
        break;
    }

    return ( dwLength );
} // VirtFileLength()


//*************************************************************************
// VirtFileGetDateTime()
//
// BOOL DOS_LOADDS WINAPI VirtFileGetDateTime (
//      HFILE   hFile,                  [in] handle returned by VirtFileOpen()
//      UINT    uType,                  [in] type of file time to query
//      LPWORD  lpwDate,                [out] receives DOS date
//      LPWORD  lpwTime )               [out] receives DOS time
//
// Reads the date and time for a file handle returned by VirtFileOpen().
//
// All NAVAPI's engine callback FileGetDateTime()'s route through this function.
// The correct FileGetDateTime (client callback, or standard platform IO) is called
// based on handle type passed.
//
// Returns:
//      FALSE on error.
//      Non-zero on success.
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

BOOL DOS_LOADDS WINAPI VirtFileGetDateTime
(
    HFILE   hFile,
    UINT    uType,
    LPWORD  lpwDate,
    LPWORD  lpwTime
)
{
    auto    LPVIRTOPENFILEINFO  lpOpenFileInfo;
    auto    LPNAVFILEIO         lpFileCallbacks;
    auto    FILETIME            rFileTime = { 0 };
    auto    BOOL                bSuccess;

    (void) uType;

    SYM_ASSERT( lpwDate );
    SYM_ASSERT( lpwTime );
//  SYM_ASSERT( hFile );

    if ( HFILE_ERROR == hFile )
    {
        SYM_ASSERT( FALSE );

        return ( FALSE );
    }

    // Translate back to our open file info structure.
    // Call appropriate date time function.

    lpOpenFileInfo = GetOpenFileInfo( hFile );

    switch ( lpOpenFileInfo->nType )
    {
    case VOFT_VIRTUAL:
        lpFileCallbacks = GetFileIoCallbacks();
        SYM_ASSERT( lpFileCallbacks->GetDateTime );

        bSuccess = lpFileCallbacks->GetDateTime( lpOpenFileInfo->lpvHandle, uType, lpwDate, lpwTime );
        break;

    case VOFT_STDIO:
        FileGetTime( (HFILE) lpOpenFileInfo->lpvHandle, TIME_LASTWRITE, &rFileTime );
        FileTimeToDosTime( &rFileTime, lpwDate, lpwTime);

        bSuccess = TRUE;
        break;

    default:
        SYM_ASSERT( FALSE );

        bSuccess = FALSE;
        break;
    }

    return ( bSuccess );
} // VirtFileGetDateTime()


//*************************************************************************
// VirtFileSetDateTime()
//
// BOOL DOS_LOADDS WINAPI VirtFileSetDateTime (
//      HFILE   hFile,                  [in] handle returned by VirtFileOpen()
//      UINT    uType,                  [in] type of file time to set
//      WORD    wDate,                  [in] new DOS date
//      WORD    wTime )                 [in] new DOS time
//
// Sets the date and time for a file handle returned by VirtFileOpen().
//
// All NAVAPI's engine callback FileSetDateTime()'s route through this function.
// The correct FileSetDateTime (client callback, or standard platform IO) is called
// based on handle type passed.
//
// Returns:
//      FALSE on error.
//      Non-zero on success.
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

BOOL DOS_LOADDS WINAPI VirtFileSetDateTime
(
    HFILE   hFile,
    UINT    uType,
    WORD    wDate,
    WORD    wTime
)
{
    auto    LPVIRTOPENFILEINFO  lpOpenFileInfo;
    auto    LPNAVFILEIO         lpFileCallbacks;
    auto    FILETIME            rFileTime = { 0 };
    auto    BOOL                bSuccess;

    (void) uType;

//  SYM_ASSERT( hFile );

    if ( HFILE_ERROR == hFile )
    {
        SYM_ASSERT( FALSE );

        return ( FALSE );
    }

    // Translate back to our open file info structure.
    // Call appropriate date time function.

    lpOpenFileInfo = GetOpenFileInfo( hFile );

    switch ( lpOpenFileInfo->nType )
    {
    case VOFT_VIRTUAL:
        lpFileCallbacks = GetFileIoCallbacks();
        SYM_ASSERT( lpFileCallbacks->SetDateTime );

        bSuccess = lpFileCallbacks->SetDateTime( lpOpenFileInfo->lpvHandle, uType, wDate, wTime );
        break;

    case VOFT_STDIO:
        DosTimeToFileTime( wDate, wTime, &rFileTime );
        FileSetTime( (HFILE) lpOpenFileInfo->lpvHandle, TIME_LASTWRITE, &rFileTime );

        bSuccess = TRUE;
        break;

    default:
        SYM_ASSERT( FALSE );

        bSuccess = FALSE;
        break;
    }

    return ( bSuccess );
} // VirtFileSetDateTime()


//*************************************************************************
// VirtFileDelete()
//
// BOOL DOS_LOADDS WINAPI VirtFileDelete (
//      LPTSTR  lpszFilename )          [in] File to delete, possibly an encoded virtual file ID pointer.
//
// All NAVAPI's engine callback FileDelete()'s route through this function.
// This function determines whether the input filespec is an encoded pointer
// to a client-defined file ID, or a regular filename.
// For regular filenames, the file is deleted.
// For client-defined file ID's deletion is not supported and is treated as
// an error.
//
// Returns:
//      Non-zero for success.
//      FALSE on error.
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

BOOL DOS_LOADDS WINAPI VirtFileDelete
(
    LPTSTR  lpszFilename
)
{
    auto    LPVOID      lpvFileId;
    auto    LPNAVFILEIO lpFileCallbacks;
    auto    BOOL        bSuccess;

    SYM_ASSERT( lpszFilename );

    // Check for encoded virtual file ID...

    if ( GetVirtFileId( lpszFilename, &lpvFileId ) )
    {
        // Encoded virtual file ID.

        lpFileCallbacks = GetFileIoCallbacks();
        SYM_ASSERT( lpFileCallbacks->Delete );

        bSuccess = lpFileCallbacks->Delete( lpvFileId );
    }
    else
    {
        // Regular filename.

#ifndef SYM_NLM
        bSuccess = ( NOERR == FileWipe( lpszFilename, 1 ) );
#else
        // On Netware, we have to set a special flag to "purge" a file
        // when deleting it, otherwise its directory entry stays around
        // taking up space. This eventually fills with temporary files
        // and becomes a customer issue... (STS Incident #448968)
         NLM_SetPurgeFlag( lpszFilename );
        
        // for the time enabling FileWipe() in CORE opens too many problems
        bSuccess = ( NOERR == FileDelete( lpszFilename ) );
#endif
    }

    return ( bSuccess );
} // VirtFileDelete()

//*************************************************************************
// NLM_SetPurgeFlag()
//
// BOOL NLM_SetPurgeFlag (
//      LPTSTR      lpszFileName        [in] Filename to set purge attribute for
// )
//
// This routine will set the "purge immediately" flag for the given file. This
// is for Netware systems only, and is used before file deletion to make sure
// that its directory entry is properly erased when the file is deleted.
//
// Returns:
//  TRUE    for success
//  FALSE   error
//
#if defined(SYM_NLM)
BOOL WINAPI NLM_SetPurgeFlag
( 
    LPCTSTR lpszFileName
)
{
    // Code adapted from NAVCorp's SetPurgeFlag()...
    struct stat     Stats; 
    DOSDateTime     createDT;
    DOSDateTime     lastUpdateDT;
    DOSDateTime     lastArchiveDT;
    DOSDateTime     lastAccessDT;

    // Load file information

    if ( stat( lpszFileName, &Stats ) != 0 ) 
    {
        return FALSE; 
    }

    // Is purge flag already set? 

    if ( Stats.st_attr & _A_IMMPURG ) 
    {
        return TRUE; 
    }

    // Set purge flag to "purge immediately upon delete"

    Stats.st_attr |= _A_IMMPURG;

    // Convert calendar time to DOS style date and time 

    _ConvertTimeToDOS( Stats.st_ctime, &createDT.date,  &createDT.time);            // Create date and time 
    _ConvertTimeToDOS( Stats.st_atime, &lastAccessDT.date, &lastAccessDT.time);     // Last access date and time 
    _ConvertTimeToDOS( Stats.st_btime, &lastArchiveDT.date, &lastArchiveDT.time);   // Last archive date and time 
    _ConvertTimeToDOS( Stats.st_mtime, &lastUpdateDT.date, &lastUpdateDT.time);     // Last update/modify date and time 

    return ( SetFileInfo( lpszFileName,
                          0x06,         // Access all files including hidden and system
                          Stats.st_attr,
                          (char *)&createDT, 
                          (char *)&lastArchiveDT.date,
                          (char *)&lastUpdateDT,
                          (char *)&lastAccessDT,
                          Stats.st_uid) == 0 ? TRUE : FALSE ); 
}
#endif  // defined(SYM_NLM)


//*************************************************************************
// VirtFileGetAttr()
//
// UINT DOS_LOADDS WINAPI VirtFileGetAttr (
//      LPTSTR  lpszFilename,           [in] Filename, possibly an encoded virtual file ID pointer.
//      LPUINT  lpuAttr )               [out] (optional??) receives attributes.
//
// All NAVAPI's engine callback FileGetAttr()'s route through this function.
// This function determines whether the input filespec is an encoded pointer
// to a client-defined file ID, or a regular filename.
//
// For regular filenames, the attributes are returned.
// For client-defined file ID's 0 is returned always.
//
// Returns:
//      File attributes on success.
//      (UINT) -1 on error.
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

UINT DOS_LOADDS WINAPI VirtFileGetAttr
(
    LPTSTR  lpszFilename,
    LPUINT  lpuAttr
)
{
    auto    LPVOID      lpvFileId;
    auto    UINT        uAttributes;

    SYM_ASSERT( lpszFilename );

    // Check for encoded virtual file ID...

    if ( GetVirtFileId( lpszFilename, &lpvFileId ) )
    {
        // Encoded virtual file ID.

        if ( lpuAttr )
        {
            *lpuAttr = 0;
        }
        uAttributes = 0;
    }
    else
    {
        // Regular filename.

        uAttributes = FileGetAttr( lpszFilename, lpuAttr );
    }

    return ( uAttributes );
} // VirtFileGetAttr()


//*************************************************************************
// VirtFileSetAttr()
//
// UINT DOS_LOADDS WINAPI VirtFileSetAttr (
//      LPTSTR  lpszFilename,           [in] Filename, possibly an encoded virtual file ID pointer.
//      UINT    uAttr )                 [in] Attributes to set.
//
// All NAVAPI's engine callback FileSetAttr()'s route through this function.
// This function determines whether the input filespec is an encoded pointer
// to a client-defined file ID, or a regular filename.
//
// For regular filenames, the attributes are set.
// For client-defined file ID's no action is taken and success is returned always.
//
// Returns:
//      NOERR on success.
//      Non-zero on error.
//*************************************************************************
// 6/10/98 DALLEE, created.
//*************************************************************************

UINT DOS_LOADDS WINAPI VirtFileSetAttr
(
    LPTSTR  lpszFilename,
    UINT    uAttr
)
{
    auto    LPVOID  lpvFileId;
    auto    UINT    uResult;

    SYM_ASSERT( lpszFilename );

    // Check for encoded virtual file ID...

    if ( GetVirtFileId( lpszFilename, &lpvFileId ) )
    {
        // Encoded virtual file ID.

        uResult = NOERR;
    }
    else
    {
        // Regular filename.

        uResult = FileSetAttr( lpszFilename, uAttr );
    }

    return ( uResult );
} // VirtFileSetAttr()



