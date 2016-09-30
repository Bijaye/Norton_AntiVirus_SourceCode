/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/quarantineitem.cpv   1.47   08 Jul 1998 21:06:02   jtaylor  $
/////////////////////////////////////////////////////////////////////////////
//
// QuarantineItem.cpp: implementation of the CQuarantineItem class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/quarantineitem.cpv  $
// 
//    Rev 1.47   08 Jul 1998 21:06:02   jtaylor
// Updated to Xor the contents of Quarantine files by 0xff.  No longer delete non Qfiles in Incoming directory.
//
//    Rev 1.46   08 Jul 1998 12:25:48   jtaylor
// Changed the Quarantine file signature and updated the code to work with both signatures and upgrade the old signature.
//
//    Rev 1.45   01 Jul 1998 17:29:30   jtaylor
// Updated the functions to prevent file time manipulation from failing a task.
//
//    Rev 1.44   22 Jun 1998 21:48:58   jtaylor
// Changed the current filename field in the header to be initialized by the IQuarantineItem::Initialize call.  This prevents it from getting out of sync.
//
//    Rev 1.43   22 Jun 1998 20:50:06   jtaylor
// Added support for clearing the virus information field if no virus is detected.
//
//    Rev 1.42   11 Jun 1998 13:46:30   DBuches
// Updated calls to TransferFile().  Now needs filenames passed in.
//
//    Rev 1.40   08 Jun 1998 19:48:32   SEDWARD
// Added support for progress callback function in ForwardToServer().
//
//    Rev 1.39   05 Jun 1998 20:13:06   jtaylor
// Changed the last scan time to GMT.
//
//    Rev 1.38   28 May 1998 21:54:26   jtaylor
// Fixed logic for return value in ScanRepairItem.
//
//    Rev 1.37   21 May 1998 12:49:40   jtaylor
// Added support for returning unrepairable virus found from ScanRepair item.
//
//    Rev 1.36   20 May 1998 22:07:32   jtaylor
// Fixed the filename extension to that of the #define value.
//
//    Rev 1.35   18 May 1998 21:22:20   jtaylor
// Removed duplicate code, improved modularity and readability.
//
//    Rev 1.34   15 May 1998 18:40:40   jtaylor
// Cleaned up code, moved security functions to global location.
// Moved compressed file check to global location.
//
//    Rev 1.33   13 May 1998 19:30:28   DBuches
// Got rid of 80k stack allocation in UnpackageItem().
//
//    Rev 1.32   12 May 1998 13:44:16   DBuches
// Added security descriptor preservation to UnpackageItem().
//
//    Rev 1.31   12 May 1998 13:09:34   DBuches
// 1st pass at support for NT file security.
//
//    Rev 1.30   28 Apr 1998 12:17:50   jtaylor
// Improved SaveItem functions tracking and freeing of the pBuff variable.
//
//    Rev 1.29   27 Apr 1998 22:41:50   jtaylor
// Added more debugging code.
//
//    Rev 1.28   26 Apr 1998 00:00:16   jtaylor
// Added support for preserving the original files date/times.
//
//    Rev 1.27   24 Apr 1998 21:04:24   jtaylor
// Added new functions for getting/setting original file date/time
// Added code to check to see if the file is compressed.
//
//    Rev 1.26   20 Apr 1998 22:28:06   SEDWARD
// Include 'QuarLink.h', use the QuarLink object in ForwardFileToServer().
//
//    Rev 1.25   15 Apr 1998 15:19:20   JTaylor
// Added an extra save to make sure the virus contents get updated (N30 struct).
//
//    Rev 1.24   10 Apr 1998 20:04:46   JTaylor
// Added a new success code for returning from a scan when no virus was found.
//
//    Rev 1.23   10 Apr 1998 18:24:24   JTaylor
// Added Get/setFileType
// Fixed the declaration of all methods in QuarantineItem.h to use STDMTHD
// Added Type maintentance code to ScanAndRepairItem.  Added Repairable bit
// maintenence to the function as well. (Needed for Scan and Deliver).
//
//    Rev 1.22   10 Apr 1998 12:36:18   SEDWARD
// Updated HRESULT values.
//
//    Rev 1.21   10 Apr 1998 00:04:58   SEDWARD
// Include 'FSLink.h', fixed SYM_ASSERT bug in SaveItem(), more work on
// ForwardToServer function.
//
//    Rev 1.20   07 Apr 1998 22:36:12   SEDWARD
// Partially filled in ForwardFile; include FileLink.h and LinkInfo.h
//
//    Rev 1.19   02 Apr 1998 19:26:00   JTaylor
// Added support for variable length header buffer
//
//    Rev 1.18   30 Mar 1998 19:52:16   JTaylor
// Moved an ensure release  to actually have it release the interface pointer.
//
//    Rev 1.17   30 Mar 1998 10:47:44   JTaylor
// Added a whole lot of error checking code.
//
//    Rev 1.16   25 Mar 1998 21:49:40   JTaylor
// Added Quar32.h to the include list.  Added HRESULT errors for GetQuarantinePath failure.
//
//    Rev 1.15   13 Mar 1998 18:07:48   JTaylor
// Changed the time structure for adding to the Local Time.
//
//    Rev 1.14   11 Mar 1998 18:46:34   SEDWARD
// Removed appID access functions.
//
//    Rev 1.13   11 Mar 1998 18:13:38   SEDWARD
// 'RestoreItem' now accepts target path and filename; removed the
// 'SplitPathAndFilename' function.
//
//    Rev 1.12   11 Mar 1998 15:44:04   JTaylor
// Updated ScanRepairItem to update the file scan time and defs date and update the file status to repaired.
//
//    Rev 1.11   10 Mar 1998 23:39:32   JTaylor
// Added implementation of ScanItem and RepairItem.  Added ScanRepairItem internal function.  Renamed ScanForVirus to ScanItem.
//
//    Rev 1.10   10 Mar 1998 20:09:24   SEDWARD
// 'UnpackageItem' returns the filename in an 'out' buffer if the filename is
// not specified by the caller.
//
//    Rev 1.9   10 Mar 1998 18:39:54   SEDWARD
// Now use 'GetQuarantinePath'; 'UnpackageItem' now takes a tempFile flag arg;
// replaced 'lstrcpy' calls with 'strcpy' calls in the access functions.
//
//    Rev 1.8   08 Mar 1998 13:57:52   SEDWARD
// Implemented SaveItem() and DeleteItem(), added OriginalFileExists()
// and SplitPathAndFilename().
//
//    Rev 1.7   07 Mar 1998 21:24:18   SEDWARD
// Finished 'Initialize()', filled out 'UnpackageItem()', added
// 'IsQuarantineFile()' and 'IsInitialized()'.
//
//    Rev 1.6   04 Mar 1998 17:32:10   DBuches
// Fixed implementation of GetN30StructPointer().
//
//    Rev 1.5   04 Mar 1998 16:14:26   DBuches
// Changed DWORD date fields to SYSTEMTIME structures.
//
//    Rev 1.4   03 Mar 1998 15:10:16   DBuches
// We now close the files we open.
//
//    Rev 1.3   02 Mar 1998 21:11:14   SEDWARD
// Added buffer size arguments to 'Get' access functions, DWORDs now use
// pointers instead of references.
//
//    Rev 1.2   02 Mar 1998 15:48:46   SEDWARD
// Added more access functions.
//
//    Rev 1.1   02 Mar 1998 14:51:32   SEDWARD
// Changed 'ForwareToServer' to 'ForwardToServer'; filled out the 'Initialize'
// function; added access functions.
//
//    Rev 1.0   24 Feb 1998 17:33:12   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <winbase.h>
#include "stdafx.h"
#include "Quar32.h"
#include "QuarantineItem.h"
#include "QuarAdd.h"
#include "LinkInfo.h"
#include "FsLink.h"
#include "FileLink.h"
#include "QuarLink.h"
#include "er.h"
#include "xapi.h"
#include "global.h"

// Global object count
extern LONG g_dwObjs;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
// Function name	: CQuarantineItem::CQuarantineItem
//
// Description	    : Constructor
//
////////////////////////////////////////////////////////////////////////////
// 2/24/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CQuarantineItem::CQuarantineItem()
                                        // This sets up a reference to the FileInfo
                                        // field of the file header.
       : m_fileInfoStruct ( m_fileHeader.FileInfo )
{
    InterlockedIncrement( &g_dwObjs );
    m_dwRef = 0;
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CQuarantineItem::~CQuarantineItem
//
// Description	    : Destructor
//
////////////////////////////////////////////////////////////////////////////
// 2/24/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
CQuarantineItem::~CQuarantineItem()
{
    InterlockedDecrement( &g_dwObjs );
}

////////////////////////////////////////////////////////////////////////////
// IUnknown Implementation

////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQuarantineItem::QueryInterface
// Description	    : This function will return a requested COM interface
// Return type		: STDMETHODIMP
// Argument         : REFIID riid - REFIID of interface requested
// Argument         : void** ppv - pointer to requested interface
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQuarantineItem::QueryInterface( REFIID riid, void** ppv )
{
    *ppv = NULL;

    if( IsEqualIID( riid, IID_IUnknown )||
        IsEqualIID( riid, IID_QuarantineItem) )
        *ppv = this;

    if( *ppv )
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return ResultFromScode( S_OK );
        }

    return ResultFromScode( E_NOINTERFACE );
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQuarantineItem::AddRef()
// Description	    : Increments reference count for this object
// Return type		: ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CQuarantineItem::AddRef()
{
    return ++m_dwRef;
}


////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CQuarantineItem::Release()
// Description	    : Decrements reference count for this object
// Return type		: ULONG
//
////////////////////////////////////////////////////////////////////////////////
// 8/28/97 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) CQuarantineItem::Release()
{
    if( --m_dwRef )
        return m_dwRef;

    delete this;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// IQuarantineItem  implementation (COM interface functions)


// ==== SaveItem ==========================================================
//
//  This function writes the current contents of the file info structure to
//  the Quarantine file.
//
//  Input:  -- nothing
//
//  Output: -- S_OK if all goes well
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// 4/28/98 - JTAYLOR Improved tracking and deleting on the pBuff variable.
// ========================================================================

STDMETHODIMP CQuarantineItem::SaveItem( void )
{
    auto    BOOL            bResult = TRUE;
    auto    char            szCharBuf[MAX_QUARANTINE_FILENAME_LEN];
    auto    DWORD           dwBytesWritten = 0;
    auto    HANDLE          hQFile = INVALID_HANDLE_VALUE;
    auto    HRESULT         retVal = S_OK;
    auto    int             nResult = 0;
    auto    LPBYTE          pBuff = NULL;
    auto    int             nBufferLeft = 0;

    // open the file
    bResult = GetQuarantinePath(szCharBuf, MAX_QUARANTINE_FILENAME_LEN, TARGET_QUARANTINE_PATH);
    if (FALSE == bResult)
        {
        retVal = E_UNABLE_TO_GET_QUARANTINE_PATH;
        goto  Exit_Function;
        }

    // Make sure that the header will fit in the current header field of the file
    SYM_ASSERT(m_fileHeader.dwHeaderBytes >= sizeof(m_fileHeader));
    if( m_fileHeader.dwHeaderBytes < sizeof(m_fileHeader) )
        {
        retVal = E_UNEXPECTED;
        goto Exit_Function;
        }

    NameAppendFile(szCharBuf, m_fileInfoStruct.szCurrentFilename);

    hQFile = CreateFile( szCharBuf                          // filename
                            , GENERIC_WRITE                 // access
                            , 0                             // don't share
                            , NULL                          // don't allow handle inheritance
                            , OPEN_EXISTING                 // only open if it exists
                            , FILE_FLAG_SEQUENTIAL_SCAN     // attributes and flags
                            , NULL );                       // no template handle

    if( hQFile == INVALID_HANDLE_VALUE )
        {
        retVal = E_FILE_CREATE_FAILED;
        goto  Exit_Function;
        }

    // write out the curent contents of this object's header
    nResult = WriteFile( hQFile, &m_fileHeader, sizeof(m_fileHeader), &dwBytesWritten, NULL);
    if (0 == nResult)
        {
        retVal = E_FILE_WRITE_FAILED;
        goto  Exit_Function;
        }

    // If there is more header buffer than the header requires....
    if( m_fileHeader.dwHeaderBytes > sizeof(m_fileHeader))
        {
        pBuff = new BYTE[m_fileHeader.dwHeaderBytes];
        if( NULL == pBuff )
            {
            retVal = E_OUTOFMEMORY;
            goto Exit_Function;
            }

        // Zero out the buffer
        memset(pBuff,0,m_fileHeader.dwHeaderBytes);

        nBufferLeft = m_fileHeader.dwHeaderBytes - sizeof(m_fileHeader);

        // write out the rest of the header buffer
        nResult = WriteFile( hQFile, pBuff, nBufferLeft, &dwBytesWritten, NULL);
        if (0 == nResult)
            {
            retVal = E_FILE_WRITE_FAILED;
            goto  Exit_Function;
            }

        // Free the buffer
        if( NULL != pBuff )
            {
            delete []pBuff;
            pBuff = NULL;
            }
        }

Exit_Function:

    // Free the buffer
    if( NULL != pBuff )
        {
        delete []pBuff;
        pBuff = NULL;
        }

    // close the file
    if ( INVALID_HANDLE_VALUE != hQFile )
        {
        CloseHandle( hQFile );
        }

    return ( retVal );

}  // end of "CQuarantineItem::SaveItem"



// ==== DeleteItem ========================================================
//
//  This function deletes a file (as specified by this object's current
//  filename).
//
//  Input:  -- nothing
//
//  Output: -- S_OK if all goes well, E_UNEXPECTED if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineItem::DeleteItem( void )
{
    auto    BOOL        bResult = TRUE;
    auto    char        szCharBuf[MAX_QUARANTINE_FILENAME_LEN];
    auto    HRESULT     retVal = S_OK;
    auto    int         nResult = 0;

    bResult = GetQuarantinePath(szCharBuf, MAX_QUARANTINE_FILENAME_LEN, TARGET_QUARANTINE_PATH);
    if (FALSE == bResult)
        {
        retVal = E_UNABLE_TO_GET_QUARANTINE_PATH;
        goto  Exit_Function;
        }

    NameAppendFile(szCharBuf, m_fileInfoStruct.szCurrentFilename);

    nResult = DeleteFile(szCharBuf);
    if (0 == nResult)
        {
        retVal = E_FILE_DELETE_FAILED;
        goto  Exit_Function;
        }

Exit_Function:
    return ( retVal );

}  // end of "CQuarantineItem::DeleteItem"



// ==== RepairItem =======================================================
//
//  This function will scan a file to determine if it has a virus.  It will
//  update and save the virus information structure.  It will then attempt
//  to repair the file.
//
//  Input:
//      IUnknown * pUnk  -- Unknown interface.  This is used to get the scanning
//                          context.
//      char * szDestBuf -- Character buffer to store the path and filename
//                          of the repaired file if repairing.
//      DWORD bufSize    -- The size of the buffer
//  Output:
//      HRESULT
//
// ========================================================================
// 3/11/98 Function created: JTAYLOR
// ========================================================================
STDMETHODIMP CQuarantineItem::RepairItem(IUnknown * pUnk, char*  lpszDestBuf, DWORD  bufSize)
    {
    return ScanRepairItem(pUnk, lpszDestBuf, bufSize, TRUE);
    }  // end of "CQuarantineItem::RepairItem"



// ==== ScanItem ====================================================
//
//  This function will scan a file to determine if it has a virus.  It will
//  update and save the virus information structure.
//
//  Input:
//      IUnknown * pUnk  -- Unknown interface.  This is used to get the scanning
//                          context.
//  Output:
//      HRESULT
//
// ========================================================================
// 3/11/98 Function created: JTAYLOR
// ========================================================================
STDMETHODIMP CQuarantineItem::ScanItem(IUnknown * pUnk)
    {
    return ScanRepairItem(pUnk, NULL, 0, FALSE);
    }  // end of "CQuarantineItem::ScanForVirus"



// ==== ForwardToServer ===================================================
//
//  This function forwards the current file to the server Quarantine.  Once
//  the file has been successfully forwarded to the server, the original is
//  deleted from the client Quarantine.
//
//  Input:
//      lpszServerName      -- a string specifying the target server; the transfer
//                             protocol is automatically selected at run-time
//
//      lpfnForwardProgress -- a pointer to the optional callback function,
//                             should the caller wish to display a progress
//                             control while files are being forwarded;
//                             if the caller does not implement this function
//                             its value should be NULL
//
//      dwPtrBytesSent      -- a pointer to the number of bytes forwarded
//                             so far
//
//      dwPtrTotalBytes     -- a pointer to the total number of bytes to be
//                             forwarded
//
//  Output:
//      S_OK if all goes well
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineItem::ForwardToServer(LPSTR  lpszServerName
                                        , FPFORWARDPROGRESS  lpfnForwardProgress
                                        , DWORD*  dwPtrBytesSent
                                        , DWORD*  dwPtrTotalBytes)
{
    auto    BOOL                    bResult = TRUE;
    auto    char                    szCharBuf[MAX_QUARANTINE_FILENAME_LEN];
    auto    CQuarantineFileTransfer cFileTransferLink;
    auto    HRESULT                 retVal = S_OK;
    auto    RPC_LINK_INFO           rLinkInfo;
    auto    TCHAR                   tempBuf[MAX_PATH];


    // initialize the link information structure
    rLinkInfo.pszProtocol = tempBuf;
    rLinkInfo.pszAddress = lpszServerName;
    rLinkInfo.pszEndpoint = NULL;

    // dynamically select a protocol to work with
    bResult = SelectProtocol(&rLinkInfo, ServerPing);
    if (FALSE == bResult)
        {
        retVal = E_UNABLE_TO_SELECT_QUARANTINE_ITEM_PROTOCOL;
        goto  Exit_Function;
        }

    // initialize the link object
    cFileTransferLink.SetTargetName(rLinkInfo.pszProtocol, rLinkInfo.pszAddress
                                                        , rLinkInfo.pszEndpoint);

    // derive a fully-qualified path to the Quarantine file we want to forward
    bResult = GetQuarantinePath(szCharBuf, MAX_QUARANTINE_FILENAME_LEN
                                                , TARGET_QUARANTINE_PATH);
    if (FALSE == bResult)
        {
        retVal = E_UNABLE_TO_GET_QUARANTINE_PATH;
        goto  Exit_Function;
        }
    else
        {
        NameAppendFile(szCharBuf, m_fileInfoStruct.szCurrentFilename);
        }

    // send the file on its way
    if (cFileTransferLink.SendQuarantineFile(szCharBuf, lpfnForwardProgress
                                        , dwPtrBytesSent, dwPtrTotalBytes))
        {
        // we successfully forwarded the file, so now delete the original from
        // the client Quarantine
        auto    HRESULT     tempHR;
        tempHR = DeleteItem();
        if( FAILED(tempHR))
            {
            retVal = E_FILE_DELETE_FAILED;
            }
        }
    else
        {
        retVal = E_UNABLE_TO_FORWARD_QUARANTINE_ITEM;
        goto  Exit_Function;
        }


Exit_Function:
    return (retVal);

}  // end of "CQuarantineItem::ForwardToServer"



// ==== RestoreItem =======================================================
//
//  This function restores a Quarantine item to a location outside of the
//  Quarantine area.  The caller must always supply a destination path, and
//  has the option to supply a filename (if one is not specified, the
//  original will be used).  Once the item has been successfully restored
//  to the outside location, it will be removed from the Quarantine area.
//
//  Input:
//      lpszDestPath        -- the destination path for the file; the caller
//                             must specify this
//
//      lpszDestFilename    -- the name of the destination file; if the caller
//                             does not specify this, the original filename
//                             is used as a default
//
//      bOverwrite          -- TRUE to overwrite the file if it exists, FALSE
//                             to leave it alone
//
//  Output:
//      S_OK if all goes well
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineItem::RestoreItem( LPSTR lpszDestPath
                                                    , LPSTR lpszDestFilename
                                                    , BOOL  bOverwrite )

{
    auto    char            szFileNameBuf[MAX_QUARANTINE_FILENAME_LEN];
    auto    HRESULT         retVal = S_OK;
    auto    UINT            nResult = 0;


    // make sure the caller has specified a valid path
    if (( NULL == lpszDestPath )  ||  ( NULL == *lpszDestPath ))
        {
        retVal = E_INVALIDARG;
        goto  Exit_Function;
        }

    // get the filename -- if no filename was specified, we assume we can
    // use the original
    if (( NULL == lpszDestFilename )  ||  ( NULL == *lpszDestFilename ))
        {
        nResult = NameReturnFile(m_fileInfoStruct.szOriginalAnsiFilename
                                                            , szFileNameBuf);
        if (NOERR != nResult)
            {
            retVal = E_FILE_FILENAME_MANIPULATION_FAILED;
            goto  Exit_Function;
            }
        }
    else
        {
        // use the caller-supplied filename
        STRCPY(szFileNameBuf, lpszDestFilename);
        }

    // unpackage the file, remove the Quarantine item and return
    retVal = UnpackageItem(lpszDestPath, szFileNameBuf, NULL, 0, bOverwrite
                                                                    , FALSE);
    if (SUCCEEDED(retVal))
        {
        retVal = DeleteItem();
        }

Exit_Function:

    return ( retVal );

}  // end of "CQuarantineItem::RestoreItem"



// ==== Initialize ========================================================
//
//  This function opens the target Quarantine file and reads the file header.
//  If we succeed, and the file is indeed a Quarantine file, the
//  "m_fileHeader" member is initialized and the file is closed.
//
//  Input:
//      lpszFileName    -- the complete path+file specification to the
//                         target file in Quarantine
//
//  Output:
//      S_OK if all goes well
//
// ========================================================================
//  Function created: 2/98, SEDWARD
//  06/22/98 - JTAYLOR - Changed current filename member to be the name
//                       of the file that was initialized.
// ========================================================================

STDMETHODIMP CQuarantineItem::Initialize( LPSTR lpszFileName )
{
    auto    BOOL            bResult;
    auto    DWORD           dwNumBytesRead;
    auto    HRESULT         retVal = S_OK;

    // initialize the file header structure
    memset( (void*) &m_fileHeader, '\0', sizeof(m_fileHeader) );

    // open the file and read in the header
    m_fileHandle = CreateFile( lpszFileName                         // filename
                                , GENERIC_READ                      // access
                                , FILE_SHARE_READ                   // don't share
                                , NULL                              // don't allow handle inheritance
                                , OPEN_EXISTING                     // only open if it exists
                                , FILE_FLAG_SEQUENTIAL_SCAN         // attributes and flags
                                , NULL                              // no template handle
                             );

    if (INVALID_HANDLE_VALUE == m_fileHandle)
        {
        retVal = E_FILE_CREATE_FAILED;
        goto  Exit_Function;
        }

    // read in the file header
    bResult = ReadFile( m_fileHandle                        // file handle
                            , (LPVOID) &m_fileHeader        // address of destination buffer
                            , sizeof(m_fileHeader)          // number of bytes to read
                            , (LPDWORD) &dwNumBytesRead     // number of bytes read
                            , NULL                          // no overlapped structure used
                      );

    // return if we failed to read the file
    if ( 0 == bResult )
        {
        retVal = E_FILE_READ_FAILED;
        goto  Exit_Function;
        }

    // Copy the filename of the file into the current filename location.
    if( NOERR != NameReturnFile( lpszFileName, m_fileInfoStruct.szCurrentFilename ) )
        {
        retVal = E_FILE_FILENAME_MANIPULATION_FAILED;
        goto  Exit_Function;
        }

    // make sure the file is a bona-fide Quarantine file
    if (FALSE == IsQuarantineFile())
        {
        retVal = E_INVALID_QUARANTINE_FILE;
        goto  Exit_Function;
        }

Exit_Function:
    if (INVALID_HANDLE_VALUE != m_fileHandle)
        {
        CloseHandle( m_fileHandle );
        m_fileHandle = INVALID_HANDLE_VALUE;
        }

    return (retVal);

}  // end of "CQuarantineItem::Initialize"



// ==== UnpackageItem =====================================================
//
//  This function unpackages a Quarantine file to the specified location.
//  At a minimum, the caller needs to provide a destination directory.
//  The caller also has the option of providing a target filename, and a
//  flag to specify whether or not to overwrite the file if it exists.
//
//  Input:
//      lpszDestinationFolder       -- the path where the unpackaged file
//                                     is to be deposited
//      lpszDestinationFileName     -- the name of the unpackaged file
//      lpszUnpackagedFileName      -- pointer to buffer that will hold the
//                                     unpackaged filename (this is only
//                                     used if no filename is supplied by
//                                     the caller)
//      dwBufSize                   -- size of the "lpszUnpackagedFileName"
//                                     buffer
//      bUseTemporaryFile           -- have CreateFile() create the
//                                     unpackaged file with the
//                                     temporary attribute
//      bOverwrite                  -- TRUE if we're to overwrite an
//                                     existing file, FALSE if not
//
//  Output:
//      S_OK if all goes well
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// 4/25/98 - JTAYLOR - Added support for restoring the files original date
//                     time values.
// 5/20/98 JTAYLOR - Fixed Quarantine extension lenght to MAX_QUARANTINE_EXTENSION_LENGTH
// 7/01/98 JTAYLOR - Changed the file time functions to not fail the process
// ========================================================================

STDMETHODIMP CQuarantineItem::UnpackageItem( LPSTR lpszDestinationFolder    // [in]
                                            , LPSTR lpszDestinationFileName // [in]
                                            , LPSTR lpszUnpackagedFileName  // [out]
                                            , DWORD dwBufSize               // [in]
                                            , BOOL bOverwrite               // [in]
                                            , BOOL bUseTemporaryFile )      // [in]
{
    auto    BOOL            bResult = TRUE;
    auto    char            szFileName[MAX_QUARANTINE_FILENAME_LEN];
    auto    char            szDestFilename[MAX_QUARANTINE_FILENAME_LEN];
    auto    DWORD           dwBytesRead = 0;
    auto    DWORD           dwBytesWritten = 0;
    auto    DWORD           dwCreationMode = 0;
    auto    DWORD           dwFlagsAndAttributes = 0;
    auto    HANDLE          destFileHandle = INVALID_HANDLE_VALUE;
    auto    HRESULT         retVal = S_OK;
    auto    int             nPathLen = 0;
    auto    FILETIME        ftCreation;
    auto    FILETIME        ftAccess;
    auto    FILETIME        ftLastWrite;
    auto    FILETIME        ftZero;
    auto    LPBYTE          lpSecurityDesc = NULL;
    auto    LPBYTE          tempByteBuf = NULL;

    // if we're not properly initialized, return
    if (FALSE == IsInitialized())
        {
        retVal = E_UNINITIALIZED;
        goto  Exit_Function;
        }


    // Allocate transfer buffer.
    tempByteBuf = new BYTE[QUARANTINE_TRANSFER_BUFFER_SIZE];
    if( tempByteBuf == NULL )
        return E_OUTOFMEMORY;


    // if the caller did not supply a valid destination directory, return
    if (( NULL == lpszDestinationFolder )  ||  ( NULL == *lpszDestinationFolder ))
        {
        retVal = E_INVALIDARG;
        goto  Exit_Function;
        }


    // open the source file
    bResult = GetQuarantinePath( szFileName, MAX_QUARANTINE_FILENAME_LEN
                                                    , TARGET_QUARANTINE_PATH );
    if (FALSE == bResult)
        {
        retVal = E_UNABLE_TO_GET_QUARANTINE_PATH;
        goto  Exit_Function;
        }

    NameAppendFile(szFileName, m_fileInfoStruct.szCurrentFilename);
    m_fileHandle = CreateFile( szFileName                // filename
                                , GENERIC_READ          // access
                                , FILE_SHARE_READ       // Allow read access to others
                                , NULL                  // don't allow handle inheritance
                                , OPEN_EXISTING         // only open if it exists
                                , FILE_ATTRIBUTE_NORMAL // attributes and flags
                                , NULL );               // no template handle
    if (INVALID_HANDLE_VALUE == m_fileHandle)
        {
        retVal = E_FILE_CREATE_FAILED;
        goto  Exit_Function;
        }

    // Save off security attributes.
    if( FAILED( GetFileSecurityDesc( szFileName, &lpSecurityDesc ) ) )
        {
        retVal = E_FILE_CREATE_FAILED;
        goto  Exit_Function;
        }


    // open the destination file...

    // if the input filename is NULL, we assume the caller doesn't care and we
    // create a file with a unique filename (however, we must keep the current
    // file extension)
    if (( NULL == lpszDestinationFileName )  ||  ( NULL == *lpszDestinationFileName ))
        {
        auto    TCHAR       szExtension[MAX_QUARANTINE_EXTENSION_LENGTH];
        if (NOERR != NameReturnExtensionEx(m_fileInfoStruct.szOriginalAnsiFilename
                                                            , (LPTSTR)szExtension, MAX_QUARANTINE_EXTENSION_LENGTH))
            {
            retVal = E_FILE_FILENAME_MANIPULATION_FAILED;
            goto  Exit_Function;
            }

        // create the file with a unique filename
        destFileHandle = CreateUniqueFile(lpszDestinationFolder, szExtension, szDestFilename);
        if (INVALID_HANDLE_VALUE == destFileHandle)
            {
            retVal = E_FILE_CREATE_FAILED;
            goto  Exit_Function;
            }

        // save the unique filename
        if (NULL != lpszUnpackagedFileName)
            {
            STRNCPY(lpszUnpackagedFileName, szDestFilename, dwBufSize);
            }
        }
    else    // the caller supplied a target filename, so use it
        {
        STRCPY(szDestFilename, lpszDestinationFolder);
        NameAppendFile(szDestFilename, lpszDestinationFileName);

        if (TRUE == bOverwrite)
            {
            // always overwrite
            dwCreationMode = CREATE_ALWAYS;
            }
        else
            {
            // if the file exists, the create function will fail
            dwCreationMode = CREATE_NEW;
            }

        // set "temporary" file attribute
        if (TRUE == bUseTemporaryFile)
            {
            dwFlagsAndAttributes = FILE_ATTRIBUTE_TEMPORARY;
            }
        else
            {
            dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
            }

        destFileHandle = CreateFile( szDestFilename             // filename
                                        , GENERIC_WRITE         // access
                                        , 0                     // don't share
                                        , NULL                  // don't allow handle inheritance
                                        , dwCreationMode        // creation mode
                                        , dwFlagsAndAttributes  // attributes and flags
                                        , NULL );               // no template handle
        if ( INVALID_HANDLE_VALUE == destFileHandle )
            {
            retVal = E_FILE_CREATE_FAILED;
            goto  Exit_Function;
            }
        }

    // Set proper security attributes.
    SetFileSecurityDesc( szDestFilename , lpSecurityDesc );

    // position the source file so we're pointing at file data (not the header)
    dwBytesRead = SetFilePointer(m_fileHandle, m_fileHeader.dwHeaderBytes
                                                                , NULL
                                                                , FILE_BEGIN);
    if (0xFFFFFFFF == dwBytesRead)
        {
        retVal = E_FILE_SET_POSITON_FAILED;
        goto  Exit_Function;
        }

    // Transfer the contents of the Quarantined file to the destination file.
    retVal = TransferFile( szFileName, m_fileHandle, szDestFilename, destFileHandle );
    if( FAILED( retVal ) )
        {
        goto  Exit_Function;
        }

    // Create a zero file time
    ftZero.dwLowDateTime = 0;
    ftZero.dwHighDateTime = 0;

    // Write the file date/times
    bResult = SystemTimeToFileTime( &m_fileInfoStruct.stOriginalFileDateCreated,  &ftCreation );
    bResult = SystemTimeToFileTime( &m_fileInfoStruct.stOriginalFileDateAccessed, &ftAccess );
    bResult = SystemTimeToFileTime( &m_fileInfoStruct.stOriginalFileDateWritten,  &ftLastWrite );

    // Set the file time.  If any of the times is 0, then do not set that value.
    bResult = SetFileTime( destFileHandle,
                           ( 0 != CompareFileTime( &ftCreation, &ftZero ) )  ? &ftCreation : NULL,
                           ( 0 != CompareFileTime( &ftAccess, &ftZero ) )    ? &ftAccess : NULL,
                           ( 0 != CompareFileTime( &ftLastWrite, &ftZero ) ) ? &ftLastWrite : NULL );

Exit_Function:
    // close files
    if (INVALID_HANDLE_VALUE != m_fileHandle)
        {
        CloseHandle( m_fileHandle );
        m_fileHandle = INVALID_HANDLE_VALUE;
        }

    if (INVALID_HANDLE_VALUE != destFileHandle)
        {
        CloseHandle( destFileHandle );
        }

    // Cleanup
    if( lpSecurityDesc )
        {
        delete [] lpSecurityDesc;
        lpSecurityDesc = NULL;
        }

    if( tempByteBuf )
        {
        delete [] tempByteBuf;
        tempByteBuf = NULL;
        }

    return (retVal);

}  // end of "CQuarantineItem::UnpackageItem"


////////////////////////////////////////////////////////////////////////////////
// CQuarantineItem COM access functions

STDMETHODIMP CQuarantineItem::GetUniqueID( UUID&  uuid )
{
    uuid = m_fileInfoStruct.uniqueID;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetUniqueID( UUID  newID )
{
    m_fileInfoStruct.uniqueID = newID;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetFileStatus( DWORD*  fileStatus )
{
    *fileStatus = m_fileInfoStruct.dwFileStatus;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetFileStatus( DWORD  newFileStatus )
{
    m_fileInfoStruct.dwFileStatus = newFileStatus;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetFileType( DWORD*  fileType )
{
    *fileType = m_fileInfoStruct.dwFileType;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetFileType( DWORD  newFileType )
{
    m_fileInfoStruct.dwFileType = newFileType;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetDateQuarantined( SYSTEMTIME*  dateQuarantined )
{
    *dateQuarantined = m_fileInfoStruct.stDateQuarantined;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetDateQuarantined( SYSTEMTIME*  newQuarantineDate )
{
    m_fileInfoStruct.stDateQuarantined = *newQuarantineDate;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                                    SYSTEMTIME* pstOriginalFileDateAccessed,
                                                    SYSTEMTIME* pstOriginalFileDateWritten )
{
    *pstOriginalFileDateCreated  = m_fileInfoStruct.stOriginalFileDateCreated;
    *pstOriginalFileDateAccessed = m_fileInfoStruct.stOriginalFileDateAccessed;
    *pstOriginalFileDateWritten  = m_fileInfoStruct.stOriginalFileDateWritten;

    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                                    SYSTEMTIME* pstOriginalFileDateAccessed,
                                                    SYSTEMTIME* pstOriginalFileDateWritten )
{
    m_fileInfoStruct.stOriginalFileDateCreated  = *pstOriginalFileDateCreated;
    m_fileInfoStruct.stOriginalFileDateAccessed = *pstOriginalFileDateAccessed;
    m_fileInfoStruct.stOriginalFileDateWritten  = *pstOriginalFileDateWritten;

    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetDateOfLastScan( SYSTEMTIME*  dateOfLastScanned )
{
    *dateOfLastScanned = m_fileInfoStruct.stDateOfLastScan;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetDateOfLastScan( SYSTEMTIME*  newDateLastScan )
{
    m_fileInfoStruct.stDateOfLastScan = *newDateLastScan;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetDateOfLastScanDefs( SYSTEMTIME*  dateOfLastScannedDefs )
{
    *dateOfLastScannedDefs = m_fileInfoStruct.stDateOfLastScanDefs;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetDateOfLastScanDefs( SYSTEMTIME*  newDateLastScanDefs )
{
    m_fileInfoStruct.stDateOfLastScanDefs = *newDateLastScanDefs;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetDateSubmittedToSARC( SYSTEMTIME*  dateSubmittedToSARC )
{
    *dateSubmittedToSARC = m_fileInfoStruct.stDateSubmittedToSARC;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetDateSubmittedToSARC( SYSTEMTIME*  newDateSubmittedToSARC )
{
    m_fileInfoStruct.stDateSubmittedToSARC = *newDateSubmittedToSARC;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetOriginalFilesize( DWORD*  originalFileSize )
{
    *originalFileSize = m_fileInfoStruct.dwOriginalFileSize;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetOriginalFilesize( DWORD  newOriginalFileSize )
{
    m_fileInfoStruct.dwOriginalFileSize = newOriginalFileSize;
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetCurrentFilename( char*  szDestBuf, DWORD  bufSize )
{
    strncpy( szDestBuf, m_fileInfoStruct.szCurrentFilename, bufSize );
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetCurrentFilename( char*  szNewCurrentFilename )
{
    strcpy( m_fileInfoStruct.szCurrentFilename, szNewCurrentFilename );
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetOriginalAnsiFilename( char*  szDestBuf, DWORD  bufSize )
{
    strncpy( szDestBuf, m_fileInfoStruct.szOriginalAnsiFilename, bufSize );
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetOriginalAnsiFilename( char*  szNewOriginalAnsiFilename )
{
    strcpy( m_fileInfoStruct.szOriginalAnsiFilename, szNewOriginalAnsiFilename );
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetOriginalOwnerName( char*  szDestBuf, DWORD  bufSize )
{
    strncpy( szDestBuf, m_fileInfoStruct.szOriginalOwnerName, bufSize );
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetOriginalOwnerName( char*  szNewOwnerName )
{
    strcpy( m_fileInfoStruct.szOriginalOwnerName, szNewOwnerName );
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetOriginalMachineDomain( char*  szDestBuf, DWORD  bufSize )
{
    strncpy( szDestBuf, m_fileInfoStruct.szOriginalMachineDomain, bufSize );
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetOriginalMachineDomain( char*  szNewMachineDomain )
{
    strcpy( m_fileInfoStruct.szOriginalMachineDomain, szNewMachineDomain );
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetOriginalMachineName( char*  szDestBuf, DWORD  bufSize )
{
    strncpy( szDestBuf, m_fileInfoStruct.szOriginalMachineName, bufSize );
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::SetOriginalMachineName( char*  szNewMachineName )
{
    strcpy( m_fileInfoStruct.szOriginalMachineName, szNewMachineName );
    return (S_OK);
}

STDMETHODIMP CQuarantineItem::GetN30StructPointer( N30**  n30Ptr )
{
    *n30Ptr = &(m_fileInfoStruct.VirusInfo);
    return (S_OK);
}


// ==== OriginalFileExists ================================================
//
//  This function tests to see if the original file exists at the original
//  location.
//
//  Input:
//      bResult -- a pointer to a BOOL
//
//  Output: a value of TRUE if the file with the original path+filename
//          already exists, FALSE if it does not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

STDMETHODIMP CQuarantineItem::OriginalFileExists( BOOL*  bResult )
{
    auto    DWORD       dwTempAttributes = 0;
    auto    HRESULT     retVal = S_OK;

    dwTempAttributes = GetFileAttributes(m_fileInfoStruct.szOriginalAnsiFilename);
    if (0xFFFFFFFF == dwTempAttributes)
        {
        *bResult = FALSE;
        }
    else
        {
        *bResult = TRUE;
        }

    return (retVal);

}  // end of "CQuarantineItem::OriginalFileExists"



// ==== ScanRepairItem ====================================================
//
//  This function will scan a file to determine if it has a virus.  It will
//  update and save the virus information structure.  It will then attempt
//  to repair the file.
//
//  Input:
//      IUnknown * pUnk  -- Unknown interface.  This is used to get the scanning
//                          context.
//      char * szDestBuf -- Character buffer to store the path and filename
//                          of the repaired file if repairing.
//      DWORD bufSize    -- The size of the buffer
//      BOOL bRepair     -- Whether or not we are repairing the virus
//  Output:
//      HRESULT
//      There are two success codes for this function.
//        If there was a virus found then return S_OK
//        If there was no virus found return S_NO_VIRUS_FOUND
//        If there was a non-deletable repairable virus found E_AVAPI_VS_CANT_REPAIR
//        If there was a non-deletable non-repairable virus found S_UNDELETABLE_VIRUS_FOUND
//
// ========================================================================
// 3/11/98 Function created: JTAYLOR
// 5/21/98 JTAYLOR -- Added support for undeletable virus found return code.
// 6/05/98 JTAYLOR -- Changed the time for last scan to GMT.
// 6/22/98 JTAYLOR -- If no virus detected, then clear the virus info.
// ========================================================================
HRESULT CQuarantineItem::ScanRepairItem(IUnknown * pUnk,
                                        char * lpszDestBuf,
                                        DWORD bufSize,
                                        BOOL bRepair)
    {
    char szGeneratedFilename[MAX_PATH]  = "";
    char szFolder[MAX_PATH]             = "";
    DWORD dwFileType                    = 0;
    IQuarantineDLL *pIQuarDll           = NULL;
    HVCONTEXT hContext                  = NULL;
    HVLVIRUS hVirus;
    HRESULT  hr;
    HRESULT  hRetVal                    = E_UNEXPECTED;
    VSTATUS result;
    BOOL bFoundVirus                    = FALSE;
    BOOL bCanDeleteItem                 = TRUE;

    // Save the current scan time and definitions data
    SYSTEMTIME tTemp;

    // Get the interface
    hr = pUnk->QueryInterface( IID_QuarantineDLL, (LPVOID*) &pIQuarDll );
    if( FAILED(hr))
        return hr;

    CEnsureRelease er( pIQuarDll );

    // Get the virus scanning context
    hr = pIQuarDll->GetScanningContext(&hContext);
    if( FAILED(hr))
        return hr;

    // Unpackage the file to the temporary directory
    BOOL bSuccess = GetQuarantinePath( szFolder, MAX_PATH, TARGET_QUARANTINE_TEMP_PATH );

    if( FALSE == bSuccess || szFolder[0] == '\0')
        {
        return E_UNABLE_TO_GET_QUARANTINE_PATH;
        }

    hr = UnpackageItem(szFolder,szGeneratedFilename, szGeneratedFilename, MAX_PATH,TRUE,TRUE);
    if( FAILED(hr))
        return hr;

    hr = pIQuarDll->GetCurrentDefinitionsDate(&tTemp);
    if( FAILED(hr))
        {
        hRetVal = hr;
        goto Exit_Function;
        }

    result = VLScanFile( hContext,
                         szGeneratedFilename,
                         &hVirus );

    // If the VLScanFile returned and error, then return its error
    if( result != VS_OK )
        {
        hRetVal = MapVStatus( result );
        goto Exit_Function;
        }

    // get a pointer to the N30 struct for this file
    N30 *pN30;
    if(FAILED(GetN30StructPointer(&pN30)))
        {
        hRetVal = E_UNEXPECTED;
        goto Exit_Function;
        }

    // If the file was successfully scanned, updated the date/times
    if( result == VS_OK )
        {
        hr = SetDateOfLastScanDefs( &tTemp );
        if(FAILED(hr))
            {
            hRetVal = hr;
            goto Exit_Function;
            }

        GetSystemTime( &tTemp );
        hr = SetDateOfLastScan( &tTemp );
        if(FAILED(hr))
            {
            hRetVal = hr;
            goto Exit_Function;
            }
        }

    // Check to see if a virus was found
    if( result == VS_OK && hVirus != NULL )
        {
        BOOL bRepairSuccessfull = FALSE;
        bFoundVirus = TRUE;

        // Update the virus information
        *pN30 = hVirus->stVirusInfoBlock;

        // Find out if the file is supposed to be repairable.
        hr = GetFileType(&dwFileType);
        if( FAILED(hr))
            {
            hRetVal = hr;
            goto Exit_Function;
            }

        bCanDeleteItem = VLCanDeleteItem( hVirus );

        result = VLRepairItem( hVirus, NULL );
        if( result != VS_OK )
            {
            // If the repair fails, and the file is supposed to be repairable
            // Mark it unrepairable.
            if( dwFileType & QFILE_TYPE_REPAIRABLE )
                {
                dwFileType = dwFileType - QFILE_TYPE_REPAIRABLE;

                // Since we have already encountered an error, do not report other errors
                // Update the type and save the new header.
                SetFileType(dwFileType);
                }

            SaveItem();
            hRetVal = MapVStatus( result );
            goto Exit_Function;
            }

        bRepairSuccessfull = TRUE;

        // Add the repairable bit
        dwFileType = dwFileType | QFILE_TYPE_REPAIRABLE;

        hr = SetFileType(dwFileType);
        if( FAILED(hr))
            {
            hRetVal = hr;
            goto Exit_Function;
            }

        hr = SaveItem();
        if( FAILED(hr))
            {
            hRetVal = hr;
            goto Exit_Function;
            }

        if(bRepair)
            {
            // Copy the repaired filename
            STRNCPY(lpszDestBuf,szGeneratedFilename,bufSize);

            hr = SetFileStatus(QFILE_STATUS_BACKUP_FILE);
            if(FAILED(hr))
                {
                hRetVal = hr;
                goto Exit_Function;
                }
            }
        else
            {
            // If the user did not want a repaired copy, then delete the
            // repaired file
            result = VLDeleteFile(hVirus);
            if( result != VS_OK )
                {
                hRetVal = MapVStatus( result );
                goto Exit_Function;
                }
            }

        // Release the virus handle
        result = VLReleaseVirusHandle( hVirus );
        if( result != VS_OK )
            {
            hRetVal = MapVStatus( result );
            goto Exit_Function;
            }
        }
    else
        {
        // We cannot detect a virus, Use a wipe on it.
        BOOL bDeleteSuccessfull = VLFileDelete(szGeneratedFilename);
        if( FALSE == bDeleteSuccessfull )
            {
            hRetVal = E_FILE_DELETE_FAILED;
            goto Exit_Function;
            }

        // Clear out the N30 structure for the file since we cannot detect a virus
        memset( pN30, 0, sizeof( N30 ) );
        }

    // Udpate the header.
    hr = SaveItem();
    if( FAILED(hr))
        {
        hRetVal = hr;
        goto Exit_Function;
        }

    // If there was a virus found then return S_OK
    // If there was no virus found return S_NO_VIRUS_FOUND
    if( FALSE == bCanDeleteItem )
        {
        return S_UNDELETABLE_VIRUS_FOUND;
        }
    else if( TRUE == bFoundVirus )
        {
        return S_OK;
        }

    return S_NO_VIRUS_FOUND;

Exit_Function:

    // There was an error, wipe the virus infected file.
    VLFileDelete(szGeneratedFilename);

    // If the error is that we cannot repair the item, and we cannot
    // delete the item, then return that we cannot delete the item.
    if( ( E_AVAPI_VS_CANT_REPAIR == hRetVal ) &&
        ( FALSE == bCanDeleteItem ) )
        {
        return S_UNDELETABLE_VIRUS_FOUND;
        }

    return hRetVal;
    } // end of CQuarantineItem::ScanRepairItem






////////////////////////////////////////////////////////////////////////////////
// CQuarantineItem non-COM functions

// ==== IsQuarantineFile ==================================================
//
//  This function tests to see if the current object ("this") is a valid
//  Quarantine file.
//
//  Input:  nothing
//
//  Output: a value of TRUE if "this" is a valid Quarantine file, FALSE if
//          it is not
//
// ========================================================================
//  Function created: 2/98, SEDWARD
// ========================================================================

BOOL    CQuarantineItem::IsQuarantineFile( void )
{
    if( QUARANTINE_SIGNATURE == m_fileHeader.dwSignature )
        return TRUE;

    return FALSE;
}  // end of "CQuarantineItem::IsQuarantineFile"



// ==== IsInitialized =====================================================
//
//  This function tests to see if the current object ("this") has been
//  properly initialized.  Currently we just check to see if it's a
//  Quarantine file, but we can get more sophisticated if need be.
//
//  Input:  nothing
//
//  Output: a value of TRUE if "this" has been properly initialized,
//          FALSE if it has not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CQuarantineItem::IsInitialized( void )
{
    return (IsQuarantineFile());

}  // end of "CQuarantineItem::IsInitialized"

