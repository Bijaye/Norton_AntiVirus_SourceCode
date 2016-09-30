// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SarcItem.cpp: implementation of the CSarcItem class.
//
//  12 Jan 2005 KTALINKI
//	Enhanced for the support of Anomaly / Extended Threats
//	Added IQuarantineItem2 Methods. Added functionality to support 
//	Remediations, SnapShot items.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atlbase.h>
#include "ldvpview.h"
#include "SarcItem.h"
#include "n30type.h"
#include "iquaran.h"
#include "io.h"
#include "vpcommon.h"
#include "quar32.h"
#include "time.h"
#include "veapi.h"
#include "SavrtModuleInterface.h"
#include "EraserRemediationVBinRecord.h"
#include "EraserRemediationVBinRecordExtractor.h"
#include <new>
#include "SymSaferStrings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////
// Local function prototypes

void GetTempFileName(LPTSTR lpDir, LPTSTR lpPrefix, LPTSTR lpName, DWORD dwNameSizeBytes);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSarcItem::CSarcItem()
{
    m_pN30data = NULL;
	m_pVBin = NULL;
    m_pEventBlk = NULL;
    m_dwRecordId = 0;
	m_dwSessionId = 0;
	m_pVBinSes = NULL;
	m_strRemediationFile = _T("");
	m_strRemedActionFile = _T("");
	m_strRemedUndoFile   = _T("");
	m_dwRef = 0;
}

CSarcItem::~CSarcItem()
{
    m_strComputerName.Empty();

    if (m_pN30data)
        free(m_pN30data);

    if (m_pEventBlk)
        m_pVBin->DestroyEvent(m_pEventBlk);

    if (m_pVBin)
        m_pVBin->Release();
	if (m_pVBinSes)
		m_pVBinSes->Release();
}

///////////////////////////////////////////////////////////////////////////////
//
// Description  : Methods required by COM
//                QueryInterface(), AddRef(), and Release()
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Functions created / header added 
// 03/09/90 - KTALINKI: Updated for IQuarantineItem2 interface
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcItem::QueryInterface( REFIID riid, void** ppv )
{
#if 0

    *ppv = NULL;

    if( IsEqualIID( riid, IID_IUnknown ) 
        || IsEqualIID( riid, IID_QuarantineItem) || IsEqualIID( riid, IID_QuarantineItem2) )
        *ppv = this;

    if( *ppv )
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return ResultFromScode( S_OK );
        }

    return ResultFromScode( E_NOINTERFACE );

#endif

    return ResultFromScode( S_OK );
}


STDMETHODIMP_(ULONG) CSarcItem::AddRef()
{
    return ++m_dwRef;
}


STDMETHODIMP_(ULONG) CSarcItem::Release()
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
// Description  : 
//
// Return type  : HRESULT S_OK
//                HRESULT S_FALSE
//
// Argument     : [ in ] LPSTR lpszFileName - The file name is in 'SessionID\RecordID' format 
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
// 01/07/05 - KTALINKI: Modified to call InitializeItem with SessionID and RecordID
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcItem::Initialize( LPSTR lpszFileName)
{
	if(NULL != lpszFileName && 0x00 != lpszFileName[0])
	{
		if(NULL == strchr(lpszFileName,'\\'))	
		{
			// '\' not found, It is not SessionID\RecordID, so legacy threat			
			return InitializeItem(NULL, (LPCSTR) lpszFileName);
		}
		else	
		{
			//Initialize is called with 'SessionID\Record' Pattern
			//TODO - Check the length of these args and return error.
			TCHAR lpszSesID[MAX_PATH], lpszRecID[MAX_PATH];	//Session and Record IDs are DWORD so not bigger than 32

			lpszSesID[0] = lpszRecID[0] = 0x00;
			_stscanf(A2T(lpszFileName), "%s\\%s", lpszSesID, lpszRecID);
			return InitializeItem(T2A(lpszSesID), T2A(lpszRecID));
		}
	}
	else
		return E_INVALIDARG;
	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////
//
// Function name: InitializeItem
//
// Description  : Initializes the IQuarantineItem2
//
// Return type  : HRESULT S_OK
//                HRESULT S_FALSE
//
// Argument     : [ in ] LPCSTR lpszSessionID - SessionID
//				  [ in ] LPCSTR lpszRecordID - RecordID
//
///////////////////////////////////////////////////////////////////////////////
// 01/07/05 - KTALINKI: Method Created
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcItem::InitializeItem(LPCSTR lpszSessionID, LPCSTR lpszRecordID)
{
    SCODE			sCode = S_FALSE;
    DWORD			dwBufferSize = sizeof(DWORD);
    DWORD			dwError;
	
	// Create an instance of Cliscan. It gets released in the destructor.
	if( SUCCEEDED(sCode =  CoCreateLDVPObject( CLSID_CliProxy, IID_IVBin2, (void**)&m_pVBin ) ) )
	{		
		if(NULL == lpszSessionID && NULL != lpszRecordID && 0x00 != lpszRecordID[0])
		{
			// Only RecordID is supplied, so legacy threat
			// Convert lpszRecordID to m_dwRecordId
			_stscanf(A2T((LPSTR)lpszRecordID),"%8X", &m_dwRecordId);
		    
			// Get the VBinInfo data for this item
			dwError = m_pVBin->GetInfo(m_dwRecordId, &m_stVBinInfo);

			if ( ERROR_SUCCESS == dwError )
			{
				// Create the event block for this item. It has all 
				// the interesting data. It gets released in the destructor.
				dwError = m_pVBin->CreateEvent(&m_pEventBlk, m_stVBinInfo.LogLine);

				if ( dwError ==ERROR_SUCCESS && CheckStatus(m_pEventBlk) )
				{
					// Lame! We should not have N30 dependencies, 
					// but SCANDLVR.DLL requires a pointer to one.

					// Get an N30 structure
					try
					{
						m_pN30data =  malloc(sN30);
					}
					catch(std::bad_alloc&)
					{
						return E_OUTOFMEMORY;
					}
					memset(m_pN30data, 0x00, sizeof(sN30));

					// Get the Virus ID from the event block
					((LPN30)m_pN30data)->wVirID = LOWORD(m_pEventBlk->VirusID);

					sCode = S_OK;
				}
			}
			else
				sCode = E_FAIL;
		}	
		else
		if(NULL != lpszSessionID && NULL != lpszRecordID && 0x00 != lpszSessionID[0] && 0x00 != lpszRecordID[0])
		{	
			// Convert lpszSessionID and lpszRecordID to m_dwSessionID and m_dwRecordId
			_stscanf(A2T((LPSTR)lpszSessionID),"%8X",&m_dwSessionId);

			_stscanf(A2T((LPSTR)lpszRecordID),"%8X",&m_dwRecordId);
		    
			//Open the session with Session ID
			dwError = m_pVBin->OpenSession(m_dwSessionId, &m_pVBinSes);

			if(ERROR_SUCCESS != dwError)
			{
				return E_FAIL;
			}

			// Get the VBinInfo data 
			dwError = m_pVBinSes->GetItemInfo(m_dwRecordId, &m_stVBinInfo);

			if ( ERROR_SUCCESS == dwError)
			{				
				if(m_stVBinInfo.dwRecordType == VBIN_RECORD_LEGACY_INFECTION)
				{
					// Create the event block for this item. It has all 
					// the interesting data. It get released ub the destructor.
					dwError = m_pVBin->CreateEvent(&m_pEventBlk, m_stVBinInfo.LogLine);

					if (dwError == ERROR_SUCCESS && CheckStatus(m_pEventBlk) )
					{
						// Lame! We should not have N30 dependencies, 
						// but SCANDLVR.DLL requires a pointer to one.

						// Get an N30 structure
						// Get an N30 structure
						try
						{
							m_pN30data =  malloc(sN30);
						}
						catch(std::bad_alloc&)
						{
							return E_OUTOFMEMORY;
						}
						memset(m_pN30data, 0, sN30);

						// Get the Virus ID from the event block
						((LPN30)m_pN30data)->wVirID = LOWORD(m_pEventBlk->VirusID);

						sCode = S_OK;
					}
					else
						sCode = E_FAIL;
				}
			}
			else
				sCode = E_FAIL;
		}
		else
			return E_INVALIDARG;
	}
    return ResultFromScode( sCode );
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: SaveItem
//
// Description  : This can always return S_OK because all VBin functions 
//                are atomic.
//
// Return type  : HRESULT S_OK
//
// Argument     : None
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcItem::SaveItem( void )
{
    EVENTBLOCK  log = {0};
    CString     szLogInfo;
    int         iLen = 0;

	if(m_stVBinInfo.dwRecordType == VBIN_RECORD_LEGACY_INFECTION)
	{
		if(NULL == m_pEventBlk)
			return E_ITEM_NOT_INITIALIZED;

		// Update the log line in VBININFO
		m_pVBin->CreateLogLine(m_stVBinInfo.LogLine, m_pEventBlk);

		// Save the updated VBin info
		m_pVBin->SetInfo(m_dwRecordId, &m_stVBinInfo);

		// Log the event.
		szLogInfo.Format( LPCTSTR(m_szLogString), m_pEventBlk->Description );
		iLen = szLogInfo.GetLength();

		log.Description = szLogInfo.GetBuffer(iLen);
		log.logger = m_pEventBlk->logger;
		log.Category = GL_CAT_SUMMARY;
		log.Event = GL_EVENT_SCANDLVR;

		m_pVBin->LogEvent(&log);
	}
    return ResultFromScode( S_OK );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: UnpackageItem
//
// Description  : This function unpackages a Quarantine Item which can be a legacy 
//					threat item or a Remediation or a System Snapshot, to the specified 
//					location. At a minimum, the caller needs to provide a destination directory. 
//					The caller also has the option of providing a target filename, 
//					and a flag to specify whether or not to overwrite the file if it exists
//
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
// 01/07/05	- KTALINKI: Enhanced for Extended Threats or Anomalies
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcItem::UnpackageItem( LPSTR lpszDestinationFolder                  // [in]
                                            , LPSTR lpszDestinationFileName         // [in]
                                            , LPSTR lpszUnpackagedFileName          // [out]
                                            , DWORD dwBufSize                       // [in]
                                            , BOOL bOverwrite                       // [in]
                                            , BOOL bUseTemporaryFile )              // [in]

{
    TCHAR   szDestNameBuffer[MAX_PATH*2] = {0};
    DWORD   dwError = ERROR_SUCCESS;
    DWORD   dwFileAttributes = 0;
	HRESULT	sCode = S_OK;
	
    if (bUseTemporaryFile)
    {
        GetTempFileName(A2T(lpszDestinationFolder), _T("snd"), szDestNameBuffer, sizeof(szDestNameBuffer));
    }
    else
    {
		if(NULL != lpszDestinationFileName && 0x00 != lpszDestinationFileName[0])
		{
			if(NULL != lpszDestinationFolder && 0x00 != lpszDestinationFolder[0])
			{
				if(lpszDestinationFolder[strlen(lpszDestinationFolder)-1] == '\\' )
					sssnprintf(szDestNameBuffer, sizeof(szDestNameBuffer), "%s%s", A2T(lpszDestinationFolder), A2T(lpszDestinationFileName) );
				else//Destination folder doesn't have a '\' in the end.
					sssnprintf(szDestNameBuffer, sizeof(szDestNameBuffer), "%s\\%s", A2T(lpszDestinationFolder), A2T(lpszDestinationFileName) );
			}
			else//Destination name buffer is NULL
				_tcscpy(szDestNameBuffer, A2T(lpszDestinationFileName));			
		}
		else
			return E_INVALIDARG;
    }

	SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );
	if( pSAVRT.Get() != NULL )
	{
		pSAVRT->UnProtectProcess();
	}

	//Legacy Infection Item
	if(VBIN_RECORD_LEGACY_INFECTION == m_stVBinInfo.dwRecordType)
	{
		
		if(m_pVBinSes && m_dwSessionId)	//This Legacy Infection Item belongs to a VBinSession
			dwError = m_pVBinSes->ExtractItem(m_dwRecordId, T2A(szDestNameBuffer), FALSE);
		else
			dwError = m_pVBin->Extract(m_dwRecordId, T2A(szDestNameBuffer), FALSE);

		// Make sure the temp file is *NOT* read-only
		dwFileAttributes = GetFileAttributes(szDestNameBuffer);
		dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
		SetFileAttributes(szDestNameBuffer, dwFileAttributes);		
	}
	else	//Remediation Action and Undo
	if(VBIN_RECORD_REMEDIATION == m_stVBinInfo.dwRecordType )	
	{
		dwError = m_pVBinSes->ExtractItem_Raw(m_dwRecordId, szDestNameBuffer, FALSE);			
	}
	else
	if(VBIN_RECORD_SYSTEM_SNAPSHOT == m_stVBinInfo.dwRecordType)
	{
		//Extract the system snapshot to the Destination file name
		dwError = m_pVBinSes->ExtractItem(m_dwRecordId, szDestNameBuffer, false);		
	}
	if ( ERROR_SUCCESS  == dwError)
	{
		_tcscpy(A2T(lpszUnpackagedFileName), szDestNameBuffer);
		sCode = S_OK;
	}
	else
		sCode = E_FAIL;

	if( pSAVRT.Get() != NULL )
	{
			pSAVRT->ProtectProcess();
	}

	return ResultFromScode(sCode);
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetFileType
//
// Description  : Get the equivalent of the Atomic Quarantine file type
//
// Return type  : HRESULT S_OK
//
// Argument     : [ out ] DWORD*  fileType - recieves the file type
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
// 01/07/05	- KTALINKI: Enhanced for Extended Threats or Anomalies
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcItem::GetFileType( DWORD*  fileType )
{
	if(VBIN_RECORD_LEGACY_INFECTION == m_stVBinInfo.dwRecordType)
	{
		if(NULL == m_pEventBlk)
			return E_ITEM_NOT_INITIALIZED;

		if ( m_pEventBlk->pdfi->bIsCompressed )
			*fileType = QFILE_TYPE_COMPRESSED;
		else if ( m_pEventBlk->CleanType == VECLEANABLE )
			*fileType = QFILE_TYPE_REPAIRABLE;
		else
			*fileType = QFILE_TYPE_NORMAL;
	}
	else
		*fileType = QFILE_TYPE_NORMAL;

    return ResultFromScode( S_OK );
}
 

///////////////////////////////////////////////////////////////////////////////
//
// Function name: SetDateSubmittedToSARC
//
// Description  : Set the submitted date in the VBinInfo structure.
//
// Return type  : HRESULT S_OK
//
// Argument     : [ in ] SYSTEMTIME*  newDateSubmittedToSARC - Submission date
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcItem::SetDateSubmittedToSARC( SYSTEMTIME*  newDateSubmittedToSARC )
{
    UNREFERENCED_PARAMETER(newDateSubmittedToSARC);
	if(m_stVBinInfo.dwRecordType == VBIN_RECORD_LEGACY_INFECTION)
	{
		// Save the current time
		time(&m_stVBinInfo.ExtraInfo.stSNDTime);
	}

    return ResultFromScode( S_OK );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetOriginalAnsiFilename
//
// Description  : Return the file name from the new QS-type package
//
//	Return type  : HRESULT S_OK
//                HRESULT S_FALSE
//
// Argument     : [ out ] char*  szDestBuf - Original file name
// Argument     : [ in/out ] DWORD  bufSize - sizr of szDestBuf
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
// 01/07/05	- KTALINKI: Enhanced for Extended Threats or Anomalies
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcItem::GetOriginalAnsiFilename( char*  szDestBuf, DWORD  bufSize )
{
    DWORD       dwBufferSize = bufSize;
    DWORD       dwError = ERROR_SUCCESS;
     
	if(NULL == szDestBuf)
		return E_INVALIDARG;

	if(NULL == m_pEventBlk)
			return E_ITEM_NOT_INITIALIZED;

	if(strlen(m_pEventBlk->Description) >= bufSize)
		return DISP_E_BUFFERTOOSMALL;

    if(VBIN_RECORD_LEGACY_INFECTION == m_stVBinInfo.dwRecordType )
	{
		try
		{
		// Get the file name from the event block
		_tcscpy(A2T(szDestBuf), A2T(m_pEventBlk->Description));
		}
		catch(...)
		{
			return E_UNEXPECTED;
		}
	}

    return ResultFromScode( S_OK );
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
STDMETHODIMP CSarcItem::GetOriginalMachineName( char*  szDestBuf, DWORD  bufSize )
{
	if(NULL == szDestBuf)
		return E_INVALIDARG;

	try
	{
		memcpy(szDestBuf, m_strComputerName.GetBuffer(bufSize), bufSize);
	}
	catch(...)
	{
		return E_UNEXPECTED;
	}

    return ResultFromScode( S_OK );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: SetOriginalMachineName
//
// Description  : Set the machine name
//
// Return type  : HRESULT S_OK
//
// Argument     : [ in ] char*  szDestBuf - Original machine name
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcItem::SetOriginalMachineName( char*  szNewMachineName )
{
	if(NULL == szNewMachineName)
		return E_INVALIDARG;

    m_strComputerName.Empty();
    m_strComputerName = szNewMachineName;
    return ResultFromScode( S_OK );
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
STDMETHODIMP CSarcItem::GetN30StructPointer( N30**  n30Ptr )
{
	if(NULL ==  n30Ptr)
		return E_INVALIDARG;

    if (m_pN30data)
    {
		try
		{
			*n30Ptr = (N30*)m_pN30data;
		}
		catch(...)
		{
			return E_UNEXPECTED;
		}
        return ResultFromScode( S_OK );
    }
    else
        return ResultFromScode( S_FALSE );        
		

	return ResultFromScode( S_OK );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: GetFileStatus
//
// Description  : 
//
// Return type  : HRESULT is always S_OK
//
// Argument     : [ in ] DWORD  fileStatus
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcItem::GetFileStatus( DWORD*  fileStatus )
{
    fileStatus = 0;

    return ResultFromScode( S_OK );
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
// 01/07/05	- KTALINKI: As part of the enhancment to support Extended Threats,
//						Item is a child of session and so user will not be setting 
//						the status on the item.
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcItem::SetFileStatus( DWORD  newFileStatus )
{
	/*
    if ( m_pEventBlk )
    {
        if ( newFileStatus == QFILE_STATUS_SUBMITTED )
        {
            m_pEventBlk->SNDStatus = SND_OK;
        }
    }
	*/
    return ResultFromScode( E_NOTIMPL );
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
//              : [ out ] LPTSTR lpName  - Full path of the temp file
//              : [ in ] DWORD dwNameSizeBytes- Size of temp file string buffer in bytes
//
///////////////////////////////////////////////////////////////////////////////
// 03/09/90 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void GetTempFileName(LPTSTR lpDir, LPTSTR lpPrefix, LPTSTR lpName, DWORD dwNameSizeBytes)
{
    int iCounter = 0;
    
    do
       {
          ++iCounter;
          sssnprintf(lpName, dwNameSizeBytes, "%s%s%d", lpDir, lpPrefix, iCounter );
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
STDMETHODIMP CSarcItem::GetUniqueID( UUID&  uuid )
{
    DWORD       dwBufferSize = sizeof(UUID);
    DWORD       dwError = ERROR_SUCCESS;     
    
    // Get the GUID from from VBININFO
    uuid = m_stVBinInfo.ExtraInfo.stUniqueId;

	return ResultFromScode( S_OK );
}


void CSarcItem::SetLogString(LPCTSTR lpLogString)
{
    m_szLogString = lpLogString; 
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CheckStatus
//
// Description  : Check the Scan & Deliver status for this item. 
//
// Return type  : BOOL - returns TRUE if it's ok to send this item based on
//                       the current status and the Resubmit reg value.
//
// Argument     : [ in ] PEVENTBLOCK lpEventBlock - Pointer to this items
//                                                  event block
//
///////////////////////////////////////////////////////////////////////////////
// 04/26/98 - TCASHIN: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CSarcItem::CheckStatus(PEVENTBLOCK lpEventBlock)
{
    CRegKey     reg;
    CString     strKey;
    long        lResult;
    BOOL        bRet = FALSE;
    DWORD       dwResubmit = 0;

    if ( !lpEventBlock )
        return bRet;

    if ( lpEventBlock->SNDStatus == SND_NONE || lpEventBlock->SNDStatus == SND_FAILED )
    {
        bRet = TRUE;
    }
    else
    {   
        strKey.Format("%s\\%s", szReg_Key_Main, szReg_Key_Quarantine); 

        if( ERROR_SUCCESS == reg.Open( HKEY_LOCAL_MACHINE, LPCTSTR(strKey), KEY_READ ) )
        {
            lResult = reg.QueryValue( dwResubmit, szReg_Val_ScanDeliverResubmit );

            if ( lResult == ERROR_SUCCESS )
            {
                if ( dwResubmit )
                    bRet = TRUE;
            }

            reg.Close();
        }

    }

    return bRet;
}


//////////////////////////////////////////////////////////////////////////////
//
// The following methods are not used. That is, they are not called
// by DeliverFilesToSarc() in Scan & Deliver. They are required to 
// make the class implementation of the interface complete.
//
//////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcItem::DeleteItem( void )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::RepairItem(IUnknown * pUnk, char*  lpszDestBuf, DWORD  bufSize)
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::ScanItem(IUnknown * pUnk)
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::ForwardToServer(LPSTR  lpszServerName
                                        , FPFORWARDPROGRESS  lpfnForwardProgress
                                        , DWORD*  dwPtrBytesSent
                                        , DWORD*  dwPtrTotalBytes)
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::RestoreItem( LPSTR lpszDestPath
                                                    , LPSTR lpszDestFilename
                                                    , BOOL  bOverwrite )

{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::OriginalFileExists( BOOL*  bResult )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::SetUniqueID( UUID  newID )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::SetFileType( DWORD  newFileType )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::GetDateQuarantined( SYSTEMTIME*  dateQuarantined )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::SetDateQuarantined( SYSTEMTIME*  newQuarantineDate )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::GetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                                    SYSTEMTIME* pstOriginalFileDateAccessed,
                                                    SYSTEMTIME* pstOriginalFileDateWritten )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::SetOriginalFileDates( SYSTEMTIME* pstOriginalFileDateCreated,
                                                    SYSTEMTIME* pstOriginalFileDateAccessed,
                                                    SYSTEMTIME* pstOriginalFileDateWritten )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::GetDateOfLastScan( SYSTEMTIME*  dateOfLastScanned )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::SetDateOfLastScan( SYSTEMTIME*  newDateLastScan )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::GetDateOfLastScanDefs( SYSTEMTIME*  dateOfLastScannedDefs )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::SetDateOfLastScanDefs( SYSTEMTIME*  newDateLastScanDefs )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::GetDateSubmittedToSARC( SYSTEMTIME*  dateSubmittedToSARC )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::GetOriginalFilesize( DWORD*  originalFileSize )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::SetOriginalFilesize( DWORD  newOriginalFileSize )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::GetCurrentFilename( char*  szDestBuf, DWORD  bufSize )
{
    return E_NOTIMPL;
}

STDMETHODIMP CSarcItem::SetCurrentFilename( char*  szNewCurrentFilename )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::SetOriginalAnsiFilename( char*  szNewOriginalAnsiFilename )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::GetOriginalOwnerName( char*  szDestBuf, DWORD  bufSize )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::SetOriginalOwnerName( char*  szNewOwnerName )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::GetOriginalMachineDomain( char*  szDestBuf, DWORD  bufSize )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcItem::SetOriginalMachineDomain( char*  szNewMachineDomain )
{
    return E_NOTIMPL;
}

STDMETHODIMP CSarcItem::GetItemInfo( PVBININFO pvbiSessionInfo)
{
	if(NULL == pvbiSessionInfo)
		return E_INVALIDARG;
	try
	{
		memcpy(pvbiSessionInfo, &m_stVBinInfo, sizeof(VBININFO));
		return S_OK;
	}
	catch(...)
	{
		return E_UNEXPECTED;
	}
}

STDMETHODIMP CSarcItem::GetItemType( DWORD *lpdwType)
{
	if(NULL == lpdwType)
		return E_INVALIDARG;
	try
	{
		*lpdwType = m_stVBinInfo.dwRecordType;
		return S_OK;
	}
	catch(...)
	{
		return E_UNEXPECTED;
	}
}

//Method to get the Remediation Type for the Remediation Items
STDMETHODIMP CSarcItem::GetRemediationType( DWORD *lpdwType)
{
	if(NULL == lpdwType)
		return E_INVALIDARG;
	try
	{
		if(VBIN_RECORD_REMEDIATION != m_stVBinInfo.dwRecordType)
			return E_ITEM_NOT_A_REMEDIATION;

		*lpdwType = m_stVBinInfo.dwRemediationType;
		return S_OK;
	}
	catch(...)
	{
		return E_UNEXPECTED;
	}
}

//Method to get the Remediation Action File for a Remediation Record File
STDMETHODIMP CSarcItem::GetRemediationAction(LPCSTR lpszRemediationFile,  LPCSTR lpszRemedActionFile)
{
	if(NULL == lpszRemediationFile || NULL == lpszRemedActionFile ) 
	{
		return E_INVALIDARG;
	}

	if(!(lpszRemediationFile && lpszRemediationFile[0] && lpszRemedActionFile && lpszRemedActionFile[0]))
		return E_INVALIDARG;

	CEraserRemediationVBinRecordExtractor cExtractor;

	// Extract the 2 pieces of information.
	DWORD sCode = cExtractor.ExtractRemediationObject(lpszRemediationFile, lpszRemedActionFile);
	if(ERROR_SUCCESS != sCode)
		return HRESULT_FROM_WIN32(sCode);

	//Look for the presence of the file
	if(!PathFileExists(lpszRemedActionFile))
		return E_FAIL;
	return S_OK;
}

//Method to get the Remediation Undo for the Remediation Items
STDMETHODIMP CSarcItem::GetRemediationUndo(LPCSTR lpszRemediationFile, LPCSTR lpszRemedUndoFile) 
{
	if(NULL == lpszRemediationFile || NULL == lpszRemedUndoFile ) 
	{
		return E_INVALIDARG;
	}

	if(!(lpszRemediationFile && lpszRemediationFile[0] && lpszRemedUndoFile && lpszRemedUndoFile[0]))
		return E_INVALIDARG;

	CEraserRemediationVBinRecordExtractor cExtractor;
	DWORD sCode = cExtractor.ExtractUndoInformation(lpszRemediationFile, lpszRemedUndoFile);
	if(ERROR_SUCCESS != sCode)
		return HRESULT_FROM_WIN32(sCode);

	//Look for the presence of the file
	if(!PathFileExists(lpszRemedUndoFile))
		return E_FAIL;

	return S_OK;
}

/******************************************************************************
Name		:	GetVirusID
Description :	Method to get the Virus ID of the threat.
Parameters	:	[OUT] lpdwVID - DWORD pointer
Returns		:	HRESULT
******************************************************************************/
//Method to get the Virus ID of the Item
STDMETHODIMP CSarcItem::GetVirusID( DWORD *lpdwVID) 
{
	HRESULT hrRet = S_OK;
	if(NULL == lpdwVID)
		return E_INVALIDARG;
	
	if(NULL == m_pEventBlk)
		return E_ITEM_NOT_INITIALIZED;

	*lpdwVID = m_pEventBlk->VirusID;

	return hrRet;
}

/******************************************************************************
Name		:	GetVirusType
Description :	Method to get the Virus Type of the threat.
Parameters	:	[OUT] lpdwType - DWORD pointer
Returns		:	HRESULT
******************************************************************************/
STDMETHODIMP CSarcItem::GetVirusType( DWORD *lpdwVType) 
{
	HRESULT hrRet = S_OK;
	if(NULL == lpdwVType)
		return E_INVALIDARG;
	
	if(NULL == m_pEventBlk)
		return E_ITEM_NOT_INITIALIZED;

	*lpdwVType = m_pEventBlk->VirusType;

	return hrRet;
}

/******************************************************************************
Name		:	GetVirusName
Description :	Method to get the Virus Name of the threat.
Parameters	:	[OUT]	lpszVirusName - char ptr to which virus name is copied
				[IN/OUT]lpdwSize - virus name string size will be copied
Returns		:	HRESULT
******************************************************************************/
STDMETHODIMP CSarcItem::GetVirusName( LPSTR lpszVirusName, DWORD	*lpdwSize)
{
	//TODO - Finish the method.
	HRESULT hrRet = S_OK;
	if(NULL == lpdwSize)
		return E_INVALIDARG;
	
	if(NULL == m_pEventBlk)
		return E_ITEM_NOT_INITIALIZED;

	if(NULL == lpszVirusName)
	{
		//Copy the size of the virus name size to lpdwSize
		if(NULL == m_pEventBlk->VirusName)
			*lpdwSize = 0;
		else
			*lpdwSize = strlen(m_pEventBlk->VirusName) + 1;

		hrRet = DISP_E_BUFFERTOOSMALL;
	}
	else
	{
		if(NULL == m_pEventBlk->VirusName)
			*lpdwSize = 0;
		else if(strlen(m_pEventBlk->VirusName) >= *lpdwSize)
		{
			*lpdwSize = strlen(m_pEventBlk->VirusName) + 1;
			hrRet = DISP_E_BUFFERTOOSMALL;
		}
		else
		{
			strcpy(lpszVirusName, m_pEventBlk->VirusName);
			*lpdwSize = strlen(m_pEventBlk->VirusName) + 1;
		}
	}

	return hrRet;
}
