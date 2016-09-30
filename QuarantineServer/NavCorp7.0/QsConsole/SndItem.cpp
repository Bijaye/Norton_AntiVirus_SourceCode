// SndItem.cpp: implementation of the CSndItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qscon.h"
#include "qsconsole.h"
#include "QSConsoleData.h"
#include "n30type.h"
#define INITGUID
#include "SndItem.h"
#include "qsfields.h"
#include "filenameparse.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSndItem::CSndItem( CItemData* pItemData, 
                    LPCTSTR szFile, 
                    CSnapInItem* pItem )
{
    // 
    // Set defaults.
    // 
    m_dwRef = 1;

    // 
    // Save off item data.
    // 
    m_pItem = pItemData;
    m_sFileName = szFile;

    // 
    // Get Quarantine server interface and set the proxy blanket.
    // 
    ((CQSConsoleData*) pItem)->m_pQServer->GetQuarantineItem( m_pItem->m_dwItemID, &m_pQserverItem );
    ((CQSConsoleData*) pItem)->m_pQServer.DCO_SetProxyBlanket( m_pQserverItem );

    // 
    // Allocate n30 struct
    // 
    m_pN30data = new N30;
    ZeroMemory( m_pN30data, sizeof( N30 ) );
}

CSndItem::~CSndItem()
{
    if( m_pN30data )
        {
        delete m_pN30data;
        m_pN30data = NULL;
        }
}


///////////////////////////////////////////////////////////////////////////////
//
// Description  : Methods required by COM
//                QueryInterface(), AddRef(), and Release()
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Functions created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSndItem::QueryInterface( REFIID riid, void** ppv )
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


STDMETHODIMP_(ULONG) CSndItem::AddRef()
{
    return ++m_dwRef;
}


STDMETHODIMP_(ULONG) CSndItem::Release()
{
    if( --m_dwRef )
        return m_dwRef;

    delete this;
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CSndItem::Initialize
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
// Argument     : LPSTR lpszFileName
//
///////////////////////////////////////////////////////////////////////////////
// 3/12/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSndItem::Initialize( LPSTR lpszFileName )
{
    // 
    // Do nothing
    // 
    return E_NOTIMPL;
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
STDMETHODIMP CSndItem::SaveItem( void )
{
    return m_pQserverItem->Commit();
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
STDMETHODIMP CSndItem::UnpackageItem( LPSTR lpszDestinationFolder                     // [in]
                                            , LPSTR lpszDestinationFileName         // [in]
                                            , LPSTR lpszUnpackagedFileName          // [out]
                                            , DWORD dwBufSize                       // [in]
                                            , BOOL bOverwrite                       // [in]
                                            , BOOL bUseTemporaryFile )              // [in]

{
USES_CONVERSION;
    TCHAR   szDestNameBuffer[MAX_PATH*2] = {0};

    if (bUseTemporaryFile)
        {
        GetTempFileName( A2T( lpszDestinationFolder ), _T("Q"), 0, szDestNameBuffer );
        }

    if( CopyFile( m_sFileName, szDestNameBuffer, FALSE ) == FALSE )
        return E_FAIL;

    strncpy(lpszUnpackagedFileName, T2A( szDestNameBuffer ), dwBufSize );

    return S_OK;

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
STDMETHODIMP CSndItem::GetFileType( DWORD*  fileType )
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
STDMETHODIMP CSndItem::SetDateSubmittedToSARC( SYSTEMTIME*  newDateSubmittedToSARC )
{
USES_CONVERSION;

    // 
    // Save off new date values locally.
    //                             
    SystemTimeToTzSpecificLocalTime( NULL,
                                     newDateSubmittedToSARC,
                                     &m_pItem->m_stSubmitted );
    m_pItem->m_sSubmittedDateStr = m_pItem->GetDateString( &m_pItem->m_stSubmitted );
    m_pItem->m_dwStatus = STATUS_SUBMITTED;
    m_pItem->m_dwSubmittedBy = SUBMISSION_SCANANDDELIVER;
    m_pItem->SetStatusText();
    m_pItem->SetSubmittedText();

    // 
    // Need to save off to quarantine server.
    //
    VARIANT v;
    v.vt = VT_DATE;
    SystemTimeToVariantTime( newDateSubmittedToSARC, &v.date );
    HRESULT hr = m_pQserverItem->SetValue( A2BSTR( QSERVER_ITEM_INFO_SUBMIT_QDATE ), v );
    if( FAILED( hr ) )
        return hr;

    // 
    // Mark this item as submitted by Scan and Deliver.
    // 
    VariantClear( &v );
    v.vt = VT_UI4;
    v.ulVal = SUBMISSION_SCANANDDELIVER;
    hr = m_pQserverItem->SetValue( A2BSTR( QSERVER_ITEM_INFO_SUBMISSION_ROUTE ), v );
    if( FAILED( hr ) )
        return hr;

    VariantClear( &v );
    v.vt = VT_UI4;
    v.ulVal = STATUS_SUBMITTED;
    return m_pQserverItem->SetValue( A2BSTR( QSERVER_ITEM_INFO_STATUS ), v );
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
STDMETHODIMP CSndItem::GetOriginalAnsiFilename( char*  szDestBuf, DWORD  bufSize )
{
USES_CONVERSION;    
    CString s;
    if( lstrcmpi( m_pItem->m_sFilePath, m_pItem->m_sFileName ) == 0 )
        s = m_pItem->m_sFileName;
    else
        s = CFileNameParse::AppendFile( m_pItem->m_sFilePath, m_pItem->m_sFileName );
    strncpy( szDestBuf, T2A( s ), bufSize );
    return S_OK;
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
STDMETHODIMP CSndItem::GetOriginalMachineName( char*  szDestBuf, DWORD  bufSize )
{
USES_CONVERSION;    
    strncpy( szDestBuf, T2A( m_pItem->m_sMachineName ), bufSize );
    return S_OK;    
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
STDMETHODIMP CSndItem::GetN30StructPointer( N30**  n30Ptr )
{
    if (m_pN30data)
        {
        *n30Ptr = (N30*)m_pN30data;
        ((N30*)m_pN30data)->wVirID = (WORD)m_pItem->m_dwVirusID;
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
STDMETHODIMP CSndItem::SetFileStatus( DWORD  newFileStatus )
{
    return ResultFromScode( S_OK );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CSndItem::GetUniqueID
//
// Description  : 
//
// Return type  : STDMETHODIMP 
//
// Argument     : UUID&  uuid
//
///////////////////////////////////////////////////////////////////////////////
// 3/15/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSndItem::GetUniqueID( UUID&  uuid )
{
    uuid = m_pItem->m_ItemGUID;
    return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
//
// The following methods are not used. That is, they are not called
// by DeliverFilesToSarc() in Scan & Deliver. They are required to 
// make the class implementation of the interface complete.
//
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSndItem::DeleteItem( void )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::RepairItem(IUnknown * pUnk, char*  lpszDestBuf, DWORD  bufSize)
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::ScanItem(IUnknown * pUnk)
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::ForwardToServer(LPSTR  lpszServerName
                                        , FPFORWARDPROGRESS  lpfnForwardProgress
                                        , DWORD*  dwPtrBytesSent
                                        , DWORD*  dwPtrTotalBytes)
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::RestoreItem( LPSTR lpszDestPath
                                                    , LPSTR lpszDestFilename
                                                    , BOOL  bOverwrite )

{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::OriginalFileExists( BOOL*  bResult )
{
    return E_NOTIMPL;
}




STDMETHODIMP CSndItem::SetUniqueID( UUID  newID )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::GetFileStatus( DWORD*  fileStatus )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::SetFileType( DWORD  newFileType )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::GetDateQuarantined( SYSTEMTIME*  dateQuarantined )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::SetDateQuarantined( SYSTEMTIME*  newQuarantineDate )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::GetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                                    SYSTEMTIME* pstOriginalFileDateAccessed,
                                                    SYSTEMTIME* pstOriginalFileDateWritten )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::SetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                                    SYSTEMTIME* pstOriginalFileDateAccessed,
                                                    SYSTEMTIME* pstOriginalFileDateWritten )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::GetDateOfLastScan( SYSTEMTIME*  dateOfLastScanned )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::SetDateOfLastScan( SYSTEMTIME*  newDateLastScan )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::GetDateOfLastScanDefs( SYSTEMTIME*  dateOfLastScannedDefs )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::SetDateOfLastScanDefs( SYSTEMTIME*  newDateLastScanDefs )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::GetDateSubmittedToSARC( SYSTEMTIME*  dateSubmittedToSARC )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::GetOriginalFilesize( DWORD*  originalFileSize )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::SetOriginalFilesize( DWORD  newOriginalFileSize )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::GetCurrentFilename( char*  szDestBuf, DWORD  bufSize )
{
    return E_NOTIMPL;
}

STDMETHODIMP CSndItem::SetCurrentFilename( char*  szNewCurrentFilename )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::SetOriginalAnsiFilename( char*  szNewOriginalAnsiFilename )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::GetOriginalOwnerName( char*  szDestBuf, DWORD  bufSize )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::SetOriginalOwnerName( char*  szNewOwnerName )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::GetOriginalMachineDomain( char*  szDestBuf, DWORD  bufSize )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::SetOriginalMachineDomain( char*  szNewMachineDomain )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSndItem::SetOriginalMachineName( char*  szNewMachineName )
{
    return E_NOTIMPL;
}


