// Copyright 1999 Symantec Corporation.
//*************************************************************************
// QsSnd.cpp created 3/9/98 
//
// $Header$
//
// Description:
//
// Contains:
//
//*************************************************************************
// $Log$
//*************************************************************************

#include <stdlib.h>
#include <malloc.h> 
#include <stdio.h>
#include "stdafx.h"
#include "QsSnd.h"
#include "iQuaran.h"
#include "qspak.h"
#include "qsfields.h"
#include "n30type.h"
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


extern LONG g_dwObjs;


///////////////////////////////////////////////////////////////////////////////
// Local function prototypes

void GetTempFileName(LPTSTR lpDir, LPTSTR lpPrefix, LPTSTR lpName);


///////////////////////////////////////////////////////////////////////////////
//
// Description  : CQsSnd constructor and destructor
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CQsSnd::CQsSnd()
{
    m_hQSItem = NULL;
    m_hQsPak = NULL;
    m_pN30data = NULL;
    m_dwRef = 0;
    InterlockedIncrement( &g_dwObjs );
}

CQsSnd::~CQsSnd()
{
    if (m_pN30data)
        free(m_pN30data);

    if (m_hQSItem)
        pfnQsPakReleaseItem(m_hQSItem);

    if (m_hQsPak)
        FreeLibrary(m_hQsPak);

    InterlockedDecrement( &g_dwObjs );
}

///////////////////////////////////////////////////////////////////////////////
//
// Description  : Methods required by COM
//                QueryInterface(), AddRef(), and Release()
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Functions created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQsSnd::QueryInterface( REFIID riid, void** ppv )
{
    *ppv = NULL;

    if( IsEqualIID( riid, IID_IUnknown ) 
        || IsEqualIID( riid, IID_QuarantineItem) )
        *ppv = this;

    if( *ppv )
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return ResultFromScode( S_OK );
        }

    return ResultFromScode( E_NOINTERFACE );
}


STDMETHODIMP_(ULONG) CQsSnd::AddRef()
{
    return ++m_dwRef;
}


STDMETHODIMP_(ULONG) CQsSnd::Release()
{
    if( --m_dwRef )
        return m_dwRef;

    delete this;
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: Initialize
//
// Description  : Initialize the DLL by loading the qspak32 dll, get the 
//                function pointers we need and opening the new QS-type
//                quarantine package.
//
// Return type  : HRESULT S_OK
//                HRESULT S_FALSE
//
// Argument     : [ in ] LPSTR lpszFileName - File to open
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQsSnd::Initialize( LPSTR lpszFileName )
{
    HRESULT     hRet = S_FALSE;
    DWORD       dwVirusId = 0;
    DWORD       dwBufferSize = sizeof(DWORD);
    QSPAKSTATUS qsError = 0;
  
    // Load the packaging DLL
    m_hQsPak = LoadLibrary( _T("QsPak32.DLL"));

    if (m_hQsPak)
    {
        // Get pointers to the functions we'll use
        pfnQsPakSetItemValue   = (PFNQsPakSetItemValue) GetProcAddress( m_hQsPak, _T( "QsPakSetItemValue") );
        pfnQsPakQueryItemValue = (PFNQsPakQueryItemValue) GetProcAddress( m_hQsPak, _T( "QsPakQueryItemValue") );
        pfnQsPakIsQserverFile  = (PFNQsPakIsQserverFile) GetProcAddress( m_hQsPak, _T( "QsPakIsQserverFile") );
        pfnQsPakOpenItem       = (PFNQsPakOpenItem) GetProcAddress( m_hQsPak, _T( "QsPakOpenItem") );
        pfnQsPakReleaseItem    = (PFNQsPakReleaseItem) GetProcAddress( m_hQsPak, _T( "QsPakReleaseItem") );
        pfnQsPakUnpackageItem  = (PFNQsPakUnpackageItem) GetProcAddress( m_hQsPak, _T( "QsPakUnpackageItem") );
        pfnQsPakSaveItem       = (PFNQsPakSaveItem) GetProcAddress( m_hQsPak, _T( "QsPakSaveItem") );

        // Make sure it's a valid QS package
        if ( QSPAKSTATUS_OK == pfnQsPakIsQserverFile(lpszFileName) )
        {
            // Open it
            if ( QSPAKSTATUS_OK == pfnQsPakOpenItem(lpszFileName, &m_hQSItem) )
            {
                // Lame! We should not have N30 dependencies, 
                // but SCANDLVR.DLL requires a pointer to one.

                // Get an N30 structure
                m_pN30data =  malloc(sN30);
                memset(m_pN30data, 0, sN30);

                // Get the Virus ID from the package
                qsError = pfnQsPakQueryItemValue(m_hQSItem, 
                                                 QSERVER_ITEM_INFO_VIRUSID,
                                                 QSPAK_TYPE_DWORD,
                                                 (void*)&dwVirusId,
                                                 &dwBufferSize);

                // If there is one, put it in our N30 structure
                if (qsError == QSPAKSTATUS_OK)
                    ((LPN30)m_pN30data)->wVirID = LOWORD(dwVirusId);

                hRet = S_OK;
            }
        }

        if ( hRet == S_FALSE )
            FreeLibrary(m_hQsPak);
    }

    return ResultFromScode( hRet );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: SaveItem
//
// Description  : Save/commit the new QS-type package.
//
// Return type  : HRESULT S_OK
//                HRESULT S_FALSE
//
// Argument     : None
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQsSnd::SaveItem( void )
{
    QSPAKSTATUS qsError = pfnQsPakSaveItem(m_hQSItem);

    if (qsError == QSPAKSTATUS_OK)
        return ResultFromScode( S_OK );
    else
        return ResultFromScode( S_FALSE );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: UnpackageItem
//
// Description  : This function unpackages a Quarantine file to the specified 
//                location by calling QsPakUnpackageItem. At a minimum, the caller 
//                needs to provide a destination directory. The caller also has 
//                the option of providing a target filename, and a flag to specify 
//                whether or not to overwrite the file if it exists

// Return type  : HRESULT S_OK
//                HRESULT S_FALSE
//
// Arguments    : [ in ]  lpszDestinationFolder    -- the path where the unpackaged file
//                                                    is to be deposited
//              : [ in ]  lpszDestinationFileName  -- the name of the unpackaged file
//              : [ out ] lpszUnpackagedFileName   -- pointer to buffer that will hold the
//                                                    unpackaged filename (this is only
//                                                    used if no filename is supplied by
//                                                    the caller)
//              : [ in ]  dwBufSize                -- size of the "lpszUnpackagedFileName"
//                                                    buffer
//              : [ in ]  bUseTemporaryFile        -- have CreateFile() create the
//                                                    unpackaged file with the
//                                                    temporary attribute
//              : [ in ] bOverwrite                -- TRUE if we're to overwrite an an
//                                                    existing file, FALSE if not
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQsSnd::UnpackageItem( LPSTR lpszDestinationFolder                     // [in]
                                            , LPSTR lpszDestinationFileName         // [in]
                                            , LPSTR lpszUnpackagedFileName          // [out]
                                            , DWORD dwBufSize                       // [in]
                                            , BOOL bOverwrite                       // [in]
                                            , BOOL bUseTemporaryFile )              // [in]

{
    TCHAR   szDestNameBuffer[MAX_PATH*2] = {0};

    if (bUseTemporaryFile)
    {
        GetTempFileName(lpszDestinationFolder, QSSND_PREFIX, szDestNameBuffer);
    }
    else
    {
        wsprintf(szDestNameBuffer, "%s\\%s", lpszDestinationFolder, lpszDestinationFileName );
    }

    if ( QSPAKSTATUS_OK == pfnQsPakUnpackageItem(m_hQSItem, szDestNameBuffer) )
    {
        _tcscpy(lpszUnpackagedFileName, szDestNameBuffer);
        return ResultFromScode(S_OK);
    }
    else
        return ResultFromScode(S_FALSE);
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetFileType
//
// Description  : Get the equivalent of the Atomic Quarantine file type
//                We want to allows return QFILE_TYPE_NORMAL (0).
//
// Return type  : HRESULT S_OK
//
// Argument     : [ out ] DWORD*  fileType - recieves the file type
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQsSnd::GetFileType( DWORD*  fileType )
{

    *fileType = 0;

    return ResultFromScode( S_OK );
}
 

///////////////////////////////////////////////////////////////////////////////
//
// Function name: SetDateSubmittedToSARC
//
// Description  : Set the QSERVER_ITEM_INFO_SUBMIT_QDATE field in the
//                new QS-type package.
//
// Return type  : HRESULT S_OK
//                HRESULT S_FALSE
//
// Argument     : [ in ] SYSTEMTIME*  newDateSubmittedToSARC - Submission date
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQsSnd::SetDateSubmittedToSARC( SYSTEMTIME*  newDateSubmittedToSARC )
{
    QSPAKDATE   stSubmitDate = {0};;
    DWORD       dwBufferSize = sizeof(QSPAKDATE);

    stSubmitDate.byMonth  = (BYTE)newDateSubmittedToSARC->wMonth;
    stSubmitDate.byDay    = (BYTE)newDateSubmittedToSARC->wDay;
    stSubmitDate.wYear    = newDateSubmittedToSARC->wYear;
    stSubmitDate.byHour   = (BYTE)newDateSubmittedToSARC->wHour;
    stSubmitDate.byMinute = (BYTE)newDateSubmittedToSARC->wMinute;
    stSubmitDate.bySecond = (BYTE)newDateSubmittedToSARC->wSecond;


    QSPAKSTATUS qsError = pfnQsPakSetItemValue(m_hQSItem, 
                                  QSERVER_ITEM_INFO_SUBMIT_QDATE,
                                  QSPAK_TYPE_DATE,
                                  (void*)&stSubmitDate,
                                  dwBufferSize);

    if (qsError == QSPAKSTATUS_OK)
        return ResultFromScode( S_OK );
    else
        return ResultFromScode( S_FALSE );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetOriginalAnsiFilename
//
// Description  : Return the file name from the new QS-type package
//
// Return type  : HRESULT S_OK
//                HRESULT S_FALSE
//
// Argument     : [ out ] char*  szDestBuf - Original file name
// Argument     : [ in/out ] DWORD  bufSize - sizr of szDestBuf
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQsSnd::GetOriginalAnsiFilename( char*  szDestBuf, DWORD  bufSize )
{
    DWORD dwBufferSize = bufSize;
     
    QSPAKSTATUS qsError = pfnQsPakQueryItemValue(m_hQSItem, 
                                  QSERVER_ITEM_INFO_FILENAME,
                                  QSPAK_TYPE_STRING,
                                  (void*)szDestBuf,
                                  &dwBufferSize);

    if (qsError == QSPAKSTATUS_OK)
        return ResultFromScode( S_OK );
    else
        return ResultFromScode( S_FALSE );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetOriginalMachineName
//
// Description  : Return the machine name from the new QS-type package
//
// Return type  : HRESULT S_OK
//                HRESULT S_FALSE
//
// Argument     : [ out ] char*  szDestBuf - Original machine name
// Argument     : [ in/out ] DWORD  bufSize - sizr of szDestBuf
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQsSnd::GetOriginalMachineName( char*  szDestBuf, DWORD  bufSize )
{
    DWORD dwBufferSize = bufSize;
     
    QSPAKSTATUS qsError = pfnQsPakQueryItemValue(m_hQSItem, 
                                  QSERVER_ITEM_INFO_MACHINENAME,
                                  QSPAK_TYPE_STRING,
                                  (void*)szDestBuf,
                                  &dwBufferSize);

    if (qsError == QSPAKSTATUS_OK)
        return ResultFromScode( S_OK );
    else
        return ResultFromScode( S_FALSE );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetN30StructPointer
//
// Description  : 
//
// Return type  : HRESULT S_OK
//                HRESULT S_FALSE
//
// Argument     : [ out ] N30**  n30Ptr
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQsSnd::GetN30StructPointer( N30**  n30Ptr )
{
    if (m_pN30data)
        {
        *n30Ptr = (N30*)m_pN30data;
        return ResultFromScode( S_OK );
    }
    else
        return ResultFromScode( S_FALSE );        
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: SetFileStatus
//
// Description  : 
//
// Return type  : HRESULT is always S_OK
//
// Argument     : [ in ] DWORD  newFileStatus
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQsSnd::SetFileStatus( DWORD  newFileStatus )
{
    return ResultFromScode( S_OK );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetTempFileName
//
// Description  : 
//
// Return type  : void
//
// Argument     : [ in ] LPTSTR lpDir    - Directory to for the temp file
//              : [ in ] LPTSTR lpPrefix - File name prefix
//              : [ out ] LPTSTR lpDir   - Full path of the temp file
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void GetTempFileName(LPTSTR lpDir, LPTSTR lpPrefix, LPTSTR lpName)
{
    int iCounter = 0;
    
    do
       {
          ++iCounter;
          wsprintf(lpName, "%s\\%s%d", lpDir, lpPrefix, iCounter );
       }

       while (_access(lpName,0) == 0); /* While file already exists */
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetUniqueId
//
// Description  : 
//
// Return type  : 
//
// Argument     : [ out ] UUID&  uuid - Address to return this item's GUID
//
///////////////////////////////////////////////////////////////////////////////
// 03/12/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQsSnd::GetUniqueID( UUID&  uuid )
{
    DWORD dwBufferSize = sizeof(UUID);
     
    QSPAKSTATUS qsError = pfnQsPakQueryItemValue(m_hQSItem, 
                                  QSERVER_ITEM_INFO_UUID,
                                  QSPAK_TYPE_STRING,
                                  (void*)&uuid,
                                  &dwBufferSize);

    if (qsError == QSPAKSTATUS_OK)
        return ResultFromScode( S_OK );
    else
        return ResultFromScode( S_FALSE );
}


//////////////////////////////////////////////////////////////////////////////
//
// The following methods are not used. That is, they are not called
// by DeliverFilesToSarc() in Scan & Deliver. They are required to 
// make the class implementation of the interface complete.
//
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CQsSnd::DeleteItem( void )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::RepairItem(IUnknown * pUnk, char*  lpszDestBuf, DWORD  bufSize)
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::ScanItem(IUnknown * pUnk)
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::ForwardToServer(LPSTR  lpszServerName
                                        , FPFORWARDPROGRESS  lpfnForwardProgress
                                        , DWORD*  dwPtrBytesSent
                                        , DWORD*  dwPtrTotalBytes)
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::RestoreItem( LPSTR lpszDestPath
                                                    , LPSTR lpszDestFilename
                                                    , BOOL  bOverwrite )

{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::OriginalFileExists( BOOL*  bResult )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::SetUniqueID( UUID  newID )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::GetFileStatus( DWORD*  fileStatus )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::SetFileType( DWORD  newFileType )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::GetDateQuarantined( SYSTEMTIME*  dateQuarantined )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::SetDateQuarantined( SYSTEMTIME*  newQuarantineDate )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::GetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                                    SYSTEMTIME* pstOriginalFileDateAccessed,
                                                    SYSTEMTIME* pstOriginalFileDateWritten )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::SetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                                    SYSTEMTIME* pstOriginalFileDateAccessed,
                                                    SYSTEMTIME* pstOriginalFileDateWritten )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::GetDateOfLastScan( SYSTEMTIME*  dateOfLastScanned )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::SetDateOfLastScan( SYSTEMTIME*  newDateLastScan )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::GetDateOfLastScanDefs( SYSTEMTIME*  dateOfLastScannedDefs )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::SetDateOfLastScanDefs( SYSTEMTIME*  newDateLastScanDefs )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::GetDateSubmittedToSARC( SYSTEMTIME*  dateSubmittedToSARC )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::GetOriginalFilesize( DWORD*  originalFileSize )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::SetOriginalFilesize( DWORD  newOriginalFileSize )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::GetCurrentFilename( char*  szDestBuf, DWORD  bufSize )
{
    return E_NOTIMPL;
}

STDMETHODIMP CQsSnd::SetCurrentFilename( char*  szNewCurrentFilename )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::SetOriginalAnsiFilename( char*  szNewOriginalAnsiFilename )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::GetOriginalOwnerName( char*  szDestBuf, DWORD  bufSize )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::SetOriginalOwnerName( char*  szNewOwnerName )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::GetOriginalMachineDomain( char*  szDestBuf, DWORD  bufSize )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::SetOriginalMachineDomain( char*  szNewMachineDomain )
{
    return E_NOTIMPL;
}


STDMETHODIMP CQsSnd::SetOriginalMachineName( char*  szNewMachineName )
{
    return E_NOTIMPL;
}


