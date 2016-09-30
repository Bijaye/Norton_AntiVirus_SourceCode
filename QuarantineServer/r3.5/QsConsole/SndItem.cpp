/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

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
#include "vpcommon.h"
#include "vpstrutils.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSndItem::CSndItem( CItemData* pItemData, 
                    LPCTSTR szFile, 
                    CSnapInItem* pItem )
{
    HRESULT hr = S_OK;
	int i = 0;
	// 
    // Set defaults.
    // 
    m_dwRef = 1;

    // 
    // Save off item data.
    // 
    m_pItem = pItemData;
    m_sFileName = szFile;
	
	CQSConsoleData* pNode = (CQSConsoleData*) pItem;
	_ASSERTE (pNode);
	_ASSERTE (pNode->m_cQSInterfaces);
	if(pNode != NULL && pNode->m_cQSInterfaces != NULL)
	{
		
		if(pNode->GetAttached()== CONNECTION_BROKEN)
		{
			hr = pNode->ReAttach();
		}

		while (TRUE)
		{
			try
			{
				// 
				// Get Quarantine server interface and set the proxy blanket.
				// 
				hr = pNode->m_cQSInterfaces->m_pQServer->GetQuarantineItem( m_pItem->m_dwItemID, &m_pQserverItem );
				if( FAILED( hr ) )
				{
				_com_issue_error( hr );
				}
				hr = pNode->m_cQSInterfaces->m_pQServer.DCO_SetProxyBlanket( m_pQserverItem );
				if( FAILED( hr ) )
				{
				_com_issue_error( hr );
				}
				break;
			}

			catch( _com_error e )
			{
				
				if (i++ >= 1)
				{
					hr = e.Error();
					break;
				}
				hr = pNode->ReAttach();
			}
		}
	}
	// 
	// Allocate n30 struct
	// 
/*    m_pN30data = new N30;
    ZeroMemory( m_pN30data, sizeof( N30 ) );
*/
}

CSndItem::~CSndItem()
{
/*    if( m_pN30data )
        {
        delete m_pN30data;
        m_pN30data = NULL;
        }
*/
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

	vpstrncpy(lpszUnpackagedFileName,T2A( szDestNameBuffer ),dwBufSize );

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
//	memcpy (&m_pItem->m_stSubmitted, newDateSubmittedToSARC, sizeof(SYSTEMTIME));
	// added by terrym 10-11-00  as part of Win9x support.  SystemTimeToTz... not supported on win9x
//	CItemData::ConvertTimeFromUTC2LT(&m_pItem->m_stSubmitted );				
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
	vpstrncpy(szDestBuf, T2A( s.GetBuffer(0) ), bufSize );
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
	vpstrncpy(szDestBuf, T2A(  m_pItem->m_sMachineName.GetBuffer(0) ), bufSize );
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
    // 3-1-05  tmarle.  The is more information in the uppper word now of the dword.
	// we just can't hack it off.  The N30Struct pointer is oblsolete.
    return E_NOTIMPL;
/*
	if (m_pN30data)
        {
        *n30Ptr = (N30*)m_pN30data;
        ((N30*)m_pN30data)->wVirID = (WORD)m_pItem->m_dwVirusID;
        return ResultFromScode( S_OK );
        }
    else
        return ResultFromScode( S_FALSE );        
*/
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


///////////////////////////////////////////////////////////////////////////////
//
// Function name: InitializeItem
//
// Description  : Init method to get things started.  We are going to continue to use 
//				the IQuarantineItem Init method
//
// Return type  : E_NOTIMPL
//
///////////////////////////////////////////////////////////////////////////////
// 03/01/05 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSndItem::InitializeItem(LPCSTR lpszSessionID, LPCSTR lpszRecordID)
{
    return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetItemInfo
//
// Description  : Method to get th3e VBININFO of an QuarantineItem.  QServer 
//				does not know anything about VBININFO, so we are not going to 
//				implement this method
//
// Return type  : HRESULT E_NOTIMPL
//
// Argument     : [ out ] DWORD*  fileType - recieves the file type
//
///////////////////////////////////////////////////////////////////////////////
// 03/01/05 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSndItem::GetItemInfo( PVBININFO pvbiSessionInfo)
{
	//TODO TMARLES	make sure if memory is allocated before this pointer is handed 
	// in
	// zero out the structure to make sure it is not used on return.
	memset(pvbiSessionInfo, 0, sizeof(VBININFO));
    return E_NOTIMPL;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetItemType
//
// Description  : Looks up to see if this is a legecy type item or a newer item
//
// Return type  : HRESULT S_OK
//
// Argument     : [ out ] DWORD*  dwType receives the type of item
//				VBIN_RECORD_LEGACY_INFECTION = 0, 
//				VBIN_RECORD_SESSION_OBJECT = 1, VBIN_RECORD_REMEDIATION = 2
//
///////////////////////////////////////////////////////////////////////////////
// 03/01/05 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSndItem::GetItemType( DWORD *lpdwType)
{
	HRESULT hr = E_FAIL;
	
	if (lpdwType == NULL)
		return E_INVALIDARG;

	USES_CONVERSION;
	
    VARIANT v;
	VariantInit(&v);

	if (m_pQserverItem)
	{
		// Check to see if the package type is set... If it is then we have a new type of package.
		if (SUCCEEDED(m_pQserverItem->GetValue(A2BSTR(QSERVER_ITEM_PACKAGE_TYPE),&v)))
		{
			if (v.vt == VT_EMPTY)
			{
				*lpdwType = VBIN_RECORD_LEGACY_INFECTION;
			}
			else
			{
				*lpdwType = VBIN_RECORD_REMEDIATION;
			}

			hr = S_OK;
		}
	}
	VariantClear (&v);
	return hr;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetRemediationType
//
// Description  : Get the Remediation Type for the Remediation Items.  CQ does 
//				not know about Remediation so don't implement
//
// Return type  : HRESULT E_NOTIMPL
//
// Argument     : [ out ] DWORD*  lpdwType - recieves the remediation type
//
///////////////////////////////////////////////////////////////////////////////
// 03/01/05 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSndItem::GetRemediationType( DWORD *lpdwType)
{
    return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetRemediationAction
//
// Description  : Get the Remediation Action for the Remediation Items.  CQ does 
//				not know about Remediation so don't implement
//
// Return type  : HRESULT E_NOTIMPL
//
//
///////////////////////////////////////////////////////////////////////////////
// 03/01/05 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSndItem::GetRemediationAction(LPCSTR lpszRemediationFile,  LPCSTR lpszRemedActionFile)
{
    return E_NOTIMPL;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetRemediationUndo
//
// Description  : Method to get the remediation Undo for the remediation items
//				CQ does not know anything about this type of information so we 
//				don't implement
//
// Return type  : HRESULT E_NOTIMPL
//
//
///////////////////////////////////////////////////////////////////////////////
// 03/01/05 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSndItem::GetRemediationUndo(LPCSTR lpszRemediationFile, LPCSTR lpszRemedUndoFile)
{
    return E_NOTIMPL;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetVirusID
//
// Description  : Method to get the Virus ID
//
// Return type  : HRESULT S_OK
//
// Argument     : [ out ] DWORD*  lpdwVID - recieves the virus ID
//
///////////////////////////////////////////////////////////////////////////////
// 03/01/05 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSndItem::GetVirusID( DWORD *lpdwVID)
{
	HRESULT hr = S_OK;

	if (lpdwVID == NULL)
		return E_INVALIDARG;

	*lpdwVID = m_pItem->m_dwVirusID;

	return hr;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetVirusType
//
// Description  : Method to get the Virus Type or Threat Category
//
// Return type  : HRESULT S_OK
//
// Argument     : [ out ] DWORD*  lpdwVType - recieves the virus type
//
///////////////////////////////////////////////////////////////////////////////
// 03/01/05 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSndItem::GetVirusType( DWORD *lpdwVType)
{
	HRESULT hr = S_OK;
	USES_CONVERSION;
	VARIANT v;

	if (lpdwVType == NULL)
		return E_INVALIDARG;

	VariantInit(&v);

	hr = m_pQserverItem->GetValue(A2BSTR(QSERVER_ITEM_INFO_VIRUSTYPE), &v);
	if (SUCCEEDED (hr) && v.vt == VT_UI4)
	{
		*lpdwVType = v.ulVal;
	}
	else
	{
		*lpdwVType = 0;
		hr = E_FAIL;
	}


	return hr;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetVirusName
//
// Description  : Get the equivalent of the Atomic Quarantine file type
//                We want to allows return QFILE_TYPE_NORMAL (0).
//
// Return type  : HRESULT S_OK
//
// Argument     : [ out ] LPSTR   lpszVirusName - recieves the virusname
//				  [ out ] DWORD * lpdwSize buffer size.			
//
///////////////////////////////////////////////////////////////////////////////
// 03/01/05 - TMARLES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSndItem::GetVirusName(LPSTR		lpszVirusName,		//[OUT]		Buffer
									DWORD		*lpdwSize			//[IN/OUT]	Buffer Size
									)
{
	USES_CONVERSION;
	if (lpszVirusName == NULL || lpdwSize == NULL)
		return E_INVALIDARG;
	vpstrncpy(lpszVirusName,T2A(m_pItem->m_sVirusName.GetBuffer()),*lpdwSize);
	return S_OK;
}
