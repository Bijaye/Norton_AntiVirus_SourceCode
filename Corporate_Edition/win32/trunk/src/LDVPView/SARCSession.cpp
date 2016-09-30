// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//////////////////////////////////////////////////////////////////////
// SarcSession.cpp: implementation of the CSarcSession class.
// 12 Jan 2005 Rev 1.0	KTALINKI	Created
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <atlbase.h>
#include "ldvpview.h"
#include "SarcSession.h"
#include "SarcItem.h"
#include "iquaran.h"
#include "io.h"
#include "vpcommon.h"
#include "quar32.h"
#include "time.h"
#include "veapi.h"
#include "ccEraserInterface.h"
#include "SymSaferRegistry.h"
#include "AnomalyEventInfo.h"
#include "AnomalyProcessor.h"

using namespace ccEraser;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define REGKEY_QUARANTINE_TEMP_PATH         _T("SOFTWARE\\Symantec\\Symantec AntiVirus\\Quarantine")
#define REGVAL_QUARANTINE_TEMP_PATH         _T("TempPath")
//#define MAX_QUARANTINE_FILENAME_LEN         MAX_PATH+16

const char STR_SYSTEM_SNAPSHOT_TEMP_FILE[] = "SysSnapShot.txt";

///////////////////////////////////////////////////////////////////////////////
// Local function prototypes

void GetTempFileName(LPTSTR lpDir, LPTSTR lpPrefix, LPTSTR lpName);

////////////////////////////////////////////////////////////////////////////
// Function name    : GetPathFromRegKey()
//
// Description      : This function reads a path in from the registry
//
// Arguments        :  HKEY   hInputKey     HKEY_LOCAL_MACHINE ...
//                     LPTSTR lpszKeyName   the name of the key
//                     LPTSTR lpszValueName The name of the value
//                     LPTSTR lpszOutBuf    a String buffer for the output
//                     DWORD  nBufSize      the sixe of the output buffer.
//
// Return type      :  True for success, false else.
//
////////////////////////////////////////////////////////////////////////////
// 4/16/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL GetPathFromRegKey(HKEY            hInputKey,
                       LPTSTR          lpszKeyName,
                       LPTSTR          lpszValueName,
                       LPTSTR          lpszOutBuf,
                       DWORD           nBufSize)
    {
    auto    BOOL        bRetVal = FALSE;
    auto    long        lResult;
    auto    HKEY        hKey;

    // Perform basic type checking
    if( ( NULL == lpszKeyName ) ||
        ( NULL == lpszValueName ) ||
        ( NULL == lpszOutBuf ) )
        {
        goto  Bail_Function;
        }

    // open the registry key
    lResult = RegOpenKeyEx( hInputKey, lpszKeyName, 0, KEY_READ, &hKey);
    if (ERROR_SUCCESS != lResult)
        {
        goto  Bail_Function;
        }

    // get the desired path value
    auto    DWORD       regValueType;
    lResult = SymSaferRegQueryValueEx( hKey, lpszValueName, 0, &regValueType, (LPBYTE)lpszOutBuf, &nBufSize );
    if ((ERROR_SUCCESS != lResult)  ||  (REG_SZ != regValueType))
        {
        goto  Bail_Function;
        }

    // close the registry key
    lResult = RegCloseKey( hKey );
    if (ERROR_SUCCESS != lResult)
        {
        goto  Bail_Function;
        }


    bRetVal = TRUE;

Bail_Function:
        return (bRetVal);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSarcSession::CSarcSession()
{
	m_dwRef = 0;
	m_pVBin = NULL;
	m_pVBinSes = NULL;
    m_pEventBlk = NULL;
	m_strComputerName = _T("");
	m_hResourceInstance = NULL;
	m_sTempFolder = _T("");
    m_strAnomalyID = _T("");
//	m_dwThreatCat = m_dwAnomalyCat = -1;
	m_dwSessionId = -1;
	m_szLogString = _T("");
	m_arIQuarItems.RemoveAll();
	//m_arItemInfo.RemoveAll();
	m_arUnpackedFileNames.RemoveAll();
	m_dwSampleType = -1;
}

// 01/09/05 KTALINKI : Method Created
CSarcSession::~CSarcSession()
{
	int iSz = 0;
    m_strComputerName.Empty();
	m_sTempFolder.Empty();
	m_strAnomalyID.Empty();
	m_szLogString.Empty();

	try
	{
		if (m_pEventBlk)
			m_pVBin->DestroyEvent(m_pEventBlk);

		if (m_pVBin)
			m_pVBin->Release();

		if(m_pVBinSes)
			m_pVBinSes->Release();

		iSz = m_arIQuarItems.GetSize();
		if(iSz)
		{
			//IQuarantineItem2 *pItem;
			CSarcItem *pItem;
			for(int iIdx=0; iIdx<iSz; iIdx++ )
			{
				pItem = m_arIQuarItems.GetAt(iIdx);
				if(pItem)
					delete pItem;
			}
		}
		m_arIQuarItems.RemoveAll();
		//m_arItemInfo.RemoveAll();
		m_arUnpackedFileNames.RemoveAll();
	}
	catch(...)
	{

	}
}

///////////////////////////////////////////////////////////////////////////////
//
// Description  : Methods required by COM
//                QueryInterface(), AddRef(), and Release()
//
///////////////////////////////////////////////////////////////////////////////
// 01/09/05 KTALINKI : Method Created
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::QueryInterface( REFIID riid, void** ppv )
{
#if 0

    *ppv = NULL;

    if( IsEqualIID( riid, IID_IUnknown ) 
        || IsEqualIID( riid, IID_QuarantineSession) )
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


STDMETHODIMP_(ULONG) CSarcSession::AddRef()
{
    return ++m_dwRef;
}


STDMETHODIMP_(ULONG) CSarcSession::Release()
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
// Argument     :	[IN] LPCSTR lpszSessionID - Session ID of the Session
//					[IN] LPCSTR lpszRecordID  - Record ID in case of Legacy Virus Item
//
///////////////////////////////////////////////////////////////////////////////
// 01/02/04 - KTALINKI: Method created. 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::Initialize( LPCSTR lpszSessionID, LPCSTR lpszRecordID )
{
    SCODE       sCode = S_FALSE;
    DWORD       dwBufferSize = sizeof(DWORD);
    DWORD       dwError;

	try
	{
		if(NULL == lpszSessionID && NULL == lpszRecordID)
			E_INVALIDARG;

	// Create an instance of Cliscan. It gets released in the destructor.
	if( SUCCEEDED( sCode = CoCreateLDVPObject( CLSID_CliProxy, IID_IVBin2, (void**)&m_pVBin ) ) )
	{
		if(lpszRecordID)	//Legacy Threat Item
		{
			DWORD dwRecordId = 0;

			sscanf(lpszRecordID,"%X", &dwRecordId);
			sCode = m_pVBin->GetInfo(dwRecordId, &m_stVBinInfo);
			if(ERROR_SUCCESS != sCode)
			{
				return E_FAIL;
			}

			CSarcItem				*ptrIQItem = NULL;
			//IQuarantineItem2		*ptrIQItem = NULL;
						
			/*if( FAILED( sCode = CoCreateLDVPObject( CLSID_Cliscan, IID_QuarantineItem2, (void**)&ptrIQItem ) ) )
			{
				return ResultFromScode(sCode);
			}
			*/
			try{ptrIQItem = new CSarcItem;}
			catch(std::bad_alloc&){return E_OUTOFMEMORY;}

			if(ptrIQItem)
			{
				//Legacy Threat Item, Initialize the IQuarantineItem2 with RecordId
				ptrIQItem->InitializeItem(NULL, (LPSTR)lpszRecordID);
				m_arIQuarItems.Add(ptrIQItem);
			}
			else
                return E_OUTOFMEMORY;
			// Create the event block for this item. It has all 
			// the interesting data. It get released in the destructor.
			dwError = m_pVBin->CreateEvent(&m_pEventBlk, m_stVBinInfo.LogLine);

			if (dwError == ERROR_SUCCESS && CheckStatus(m_pEventBlk) )
			{
				sCode = S_OK;
				m_bInitialized = TRUE;
			}
			else
				sCode = E_FAIL;
		}
		else
		{
			// Convert lpszSessionID to m_dwSessionId
			sscanf(lpszSessionID,"%X", &m_dwSessionId);
		    
			//Open VBINSession with the Session ID
			dwError = m_pVBin->OpenSession(m_dwSessionId, &m_pVBinSes);

			if ( ERROR_SUCCESS == dwError)
			{
				sCode = m_pVBinSes->GetSessionInfo(&m_stVBinInfo);
				if(ERROR_SUCCESS != sCode)
					return E_FAIL;

				if(m_stVBinInfo.dwRecordType == VBIN_RECORD_LEGACY_INFECTION)
				{
					// This is a Legacy Infection, User needs to send in the session id as record id.
					return E_INVALIDARG;
				}
				else if(m_stVBinInfo.dwRecordType == VBIN_RECORD_SESSION_OBJECT)
				{				
					VBININFO		stVBinInfo;
					HANDLE			hVBinFindSession = INVALID_HANDLE_VALUE;
					
					try
					{
						m_arIQuarItems.RemoveAll();

						hVBinFindSession = m_pVBinSes->FindFirstItem(&stVBinInfo);
						do
						{	
							CSarcItem *ptrIQItem = NULL;
							try{ptrIQItem = new CSarcItem;}
							catch(std::bad_alloc&)
							{
								for(int i=0; i<m_arIQuarItems.GetSize(); i++)
								{
									IQuarantineItem2* pItem = m_arIQuarItems.GetAt(i);
									pItem->Release();
								}
								m_arIQuarItems.RemoveAll();
								return E_OUTOFMEMORY;
							}
							
							if(NULL == ptrIQItem)
							{
								for(int i=0; i<m_arIQuarItems.GetSize(); i++)
								{
									IQuarantineItem2* pItem = m_arIQuarItems.GetAt(i);
									pItem->Release();
								}
								m_arIQuarItems.RemoveAll();
								return E_OUTOFMEMORY;
							}

							//Prepare the SessionID\RecordId string to initizlize IQuarantinItem
							CString szRecID = _T("");
							szRecID.Format("%08lX",stVBinInfo.RecordID);

							ptrIQItem->InitializeItem(lpszSessionID, ((LPCSTR)szRecID));
							m_arIQuarItems.Add(ptrIQItem);							
							//TODO - K Sackinger is going to change the return val  for VBinSes::GetNextItem
							//Need to change the return value check as per that.							
						}
						while(m_pVBinSes->FindNextItem(hVBinFindSession, &stVBinInfo));

						m_pVBinSes->FindClose(hVBinFindSession );
					}
					catch(...)
					{
						if(INVALID_HANDLE_VALUE != hVBinFindSession && m_pVBinSes)
							m_pVBinSes->FindClose(hVBinFindSession );
						return E_UNEXPECTED;
					}
				}

				// Create the event block for this item. It has all 
				// the interesting data. It get released in the destructor.
				dwError = m_pVBin->CreateEvent(&m_pEventBlk, m_stVBinInfo.LogLine);

				if (dwError == ERROR_SUCCESS && CheckStatus(m_pEventBlk) )
				{
					m_bInitialized = TRUE;
					sCode = S_OK;
				}
				else
					sCode = E_FAIL;
			}
			else
				sCode = E_FAIL;
		}
	}
	}	//Try Block
	catch(...)
	{
		return E_UNEXPECTED;
	}
    return ResultFromScode( sCode );
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: Unpackage
//
// Description  : This function unpackages a Quarantine Session, inturn calls Unpackage
//					on each child Quarantine Item. At a minimum, the caller 
//                needs to provide a destination directory. The caller also has 
//                the option of providing a flag to specify 
//                whether or not to overwrite the file if it exists

// Return type  : HRESULT S_OK
//                HRESULT S_FALSE
//
// Arguments    : [ in ]  lpszDestinationFolder    -- the path where the unpackaged file
//                                                    is to be deposited
//              
//              : [ in ] bOverwrite                -- TRUE if we're to overwrite an an
//                                                    existing file, FALSE if not
//
///////////////////////////////////////////////////////////////////////////////
// 01/09/05 - KTALINKI : Method Created
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::Unpackage( LPCSTR lpszDestinationFolder                  // [in]
                                            , BOOL bOverwrite                       // [in]
                                            )

{
    TCHAR   szDestNameBuffer[MAX_PATH*2] = {0};	
    DWORD   dwError = ERROR_SUCCESS;
    DWORD   dwFileAttributes = 0;
	CString	cstrDestFolder = _T("");
	TCHAR	szGeneratedFilename[MAX_PATH+16];
	BOOL	bResult = FALSE;
	HRESULT hr = S_OK;
	int	iRemedIdx = 0;	//Index for Remediation Records

	if(!IsInitialized())
		return E_SESSION_NOT_INITIALIZED;
	
	try
	{
		//Use user supplied destination folder, otherwise, get Temp folder and unpack to a sessionID sub folder
		if(lpszDestinationFolder != NULL && lpszDestinationFolder[0])
		{
			cstrDestFolder = lpszDestinationFolder;
		}
		else
		{
			// get the temporary Quarantine Directory for unpackaging
			bResult = GetPathFromRegKey( HKEY_LOCAL_MACHINE,
										REGKEY_QUARANTINE_TEMP_PATH,
										REGVAL_QUARANTINE_TEMP_PATH,
										m_sTempFolder.GetBuffer( MAX_PATH ),
										MAX_PATH );
			m_sTempFolder.ReleaseBuffer();
			if( FALSE == bResult )
			{
				// If no temp folder specified, use windows temp path.
				if( 0 == GetTempPath( MAX_PATH, m_sTempFolder.GetBuffer( MAX_PATH ) ) )
				{
					m_sTempFolder.ReleaseBuffer();
					hr = E_QUARANTINE_DIRECTORY_INVALID;
					return hr;
				}
				m_sTempFolder.ReleaseBuffer();
			}
			//Prepare sub folder name like Temp\SessionID
			cstrDestFolder = m_sTempFolder;
			cstrDestFolder.AppendFormat("%08lX", m_stVBinInfo.dwSessionID);
		}
	

		//Enumerate and extract each item in the array
		for(int iIdx=0; iIdx<m_arIQuarItems.GetSize(); iIdx++) 
		{
			VBININFO stVBinInfo;
			szGeneratedFilename[0] = 0x00;

			IQuarantineItem2* ptrItem = m_arIQuarItems.GetAt(iIdx);
			if(NULL == ptrItem)
				break;

			hr = ptrItem->GetItemInfo(&stVBinInfo);
			if(FAILED(hr))
				return hr;

			//Prepare the destination file name depending on the record type.
			if(stVBinInfo.dwRecordType == VBIN_RECORD_LEGACY_INFECTION)
			{
				hr = ptrItem->UnpackageItem( cstrDestFolder.GetBuffer(0),
												NULL,
												T2A(szGeneratedFilename),
												MAX_PATH,
												FALSE,
												TRUE );
			}
			else
			if(stVBinInfo.dwRecordType == VBIN_RECORD_SYSTEM_SNAPSHOT)
			{
				CString strSysSnapShot = STR_SYSTEM_SNAPSHOT_TEMP_FILE;
				hr = ptrItem->UnpackageItem( cstrDestFolder.GetBuffer(0),
											strSysSnapShot.GetBuffer(0),
											T2A(szGeneratedFilename),
											MAX_PATH,
											FALSE,
											TRUE );
				strSysSnapShot.ReleaseBuffer();
			}
			else
			if(stVBinInfo.dwRecordType == VBIN_RECORD_REMEDIATION)
			{
				CString strRemed = _T("");
				strRemed.Format("\\%08lX.RAU", iRemedIdx++);
				hr = ptrItem->UnpackageItem( cstrDestFolder.GetBuffer(0),
											strRemed.GetBuffer(0),
											T2A(szGeneratedFilename),
											MAX_PATH,
											FALSE,
											TRUE );
				strRemed.ReleaseBuffer();
			}
			//Copy the destination file name to array of Unpacked Files Names.
			m_arUnpackedFileNames.Add(szGeneratedFilename);
		}
	}	//End of TRY block
	catch(...)
	{
		return E_UNEXPECTED;
	}

    return ResultFromScode(S_OK);
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
// 01/09/05 - KTALINKI : Method Created
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::SetDateSubmittedToSARC( SYSTEMTIME*  newDateSubmittedToSARC )
{
    UNREFERENCED_PARAMETER(newDateSubmittedToSARC);

	if(!IsInitialized())
		return E_SESSION_NOT_INITIALIZED;
	
    // Save the current time
    time(&m_stVBinInfo.ExtraInfo.stSNDTime);

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
// 01/09/05 - KTALINKI: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetOriginalMachineName( LPSTR  szDestBuf, DWORD  bufSize )
{
	if(!IsInitialized())
		return E_SESSION_NOT_INITIALIZED;

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
// 01/09/05 - KTALINKI: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::SetOriginalMachineName( LPCSTR  lpszNewMachineName )
{
	if(!IsInitialized())
		return E_SESSION_NOT_INITIALIZED;

    m_strComputerName.Empty();
    m_strComputerName = lpszNewMachineName;
    return ResultFromScode( S_OK );
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
// 01/09/05 - KTALINKI: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetUniqueID( UUID&  uuid )
{   
     
    if(!IsInitialized())
		return E_SESSION_NOT_INITIALIZED;

    // Get the GUID from from VBININFO
    uuid = m_stVBinInfo.ExtraInfo.stUniqueId;

    return ResultFromScode( S_OK ); 
}

// 01/09/05 - KTALINKI: Function created / header added 
void CSarcSession::SetLogString(LPCTSTR lpLogString)
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
// 01/09/05 - KTALINKI: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
BOOL CSarcSession::CheckStatus(PEVENTBLOCK lpEventBlock)
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
// 01/09/05 - KTALINKI: Function created / header added 
STDMETHODIMP CSarcSession::DeleteSession( void )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcSession::RepairSession(IUnknown * pUnk, char*  lpszDestBuf, DWORD  bufSize)
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcSession::ScanSession(IUnknown * pUnk)
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcSession::ForwardToServer(LPSTR  lpszServerName
                                        , FPFORWARDPROGRESS  lpfnForwardProgress
                                        , DWORD*  dwPtrBytesSent
                                        , DWORD*  dwPtrTotalBytes)
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcSession::RestoreSession( LPSTR lpszDestPath
                                                    , LPSTR lpszDestFilename
                                                    , BOOL  bOverwrite )

{
    return E_NOTIMPL;
}

STDMETHODIMP CSarcSession::SetUniqueID( UUID  newID )
{
    return E_NOTIMPL;
}

STDMETHODIMP CSarcSession::GetDateQuarantined( SYSTEMTIME*  dateQuarantined )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcSession::SetDateQuarantined( SYSTEMTIME*  newQuarantineDate )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcSession::GetDateOfLastScan( SYSTEMTIME*  dateOfLastScanned )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcSession::SetDateOfLastScan( SYSTEMTIME*  newDateLastScan )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcSession::GetDateOfLastScanDefs( SYSTEMTIME*  dateOfLastScannedDefs )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcSession::SetDateOfLastScanDefs( SYSTEMTIME*  newDateLastScanDefs )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcSession::GetDateSubmittedToSARC( SYSTEMTIME*  dateSubmittedToSARC )
{
    return E_NOTIMPL;
}
STDMETHODIMP CSarcSession::SetOriginalOwnerName( LPCSTR  lpszNewOwnerName )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcSession::GetOriginalMachineDomain( LPSTR  lpszDestBuf, DWORD  bufSize )
{
    return E_NOTIMPL;
}


STDMETHODIMP CSarcSession::SetOriginalMachineDomain( LPCSTR  lpszNewMachineDomain )
{
    return E_NOTIMPL;
}

STDMETHODIMP CSarcSession::GetOriginalOwnerName( LPSTR  lpszDestBuf, DWORD  bufSize )
{
    return E_NOTIMPL;
}


//This is needed for Scan And Deliver to see whether one of the items is compressed.
//If one of the items is compressed, then it is not snubmittable
STDMETHODIMP CSarcSession::IsSubmittableToSARC(BOOL*  lpbSubmittable )
{
	HRESULT hr = S_OK;

	if(NULL == lpbSubmittable )
		return E_INVALIDARG;

	if(!IsInitialized())
		return E_SESSION_NOT_INITIALIZED;

	*lpbSubmittable = TRUE;

	//TODO - TBD - Whether To submit Sessions with compressed files 
	/*	
	for(int iIdx=0; iIdx < m_arIQuarItems.GetSize(); iIdx ++)
	{
        DWORD dwFileType;
		if(SUCCEEDED(hr = m_arIQuarItems.GetAt(iIdx)->GetFileType(&dwFileType)))
		{
			if(QFILE_TYPE_COMPRESSED == dwFileType)
			{
				*lpbSubmittable = FALSE;
				return S_OK;
			}
		}
		else
			return hr;
	}
	*/
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name: SaveSession
//
// Description  : This can always return S_OK because all VBin functions 
//                are atomic.
//
// Return type  : HRESULT S_OK
//
// Argument     : None
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::SaveSession( void )
{
    EVENTBLOCK  log = {0};
    CString     szLogInfo;
    int         iLen = 0;

	if(!IsInitialized() || NULL == m_pEventBlk)
		return E_SESSION_NOT_INITIALIZED;
	try
	{		
		// Update the log line in VBININFO
		m_pVBin->CreateLogLine(m_stVBinInfo.LogLine, m_pEventBlk);

		// Save the updated VBin info
		m_pVBin->SetInfo(m_dwSessionId, &m_stVBinInfo);

		// Log the event.
		szLogInfo.Format( LPCTSTR(m_szLogString), m_pEventBlk->Description );
		iLen = szLogInfo.GetLength();

		log.Description = szLogInfo.GetBuffer(iLen);
		log.logger = m_pEventBlk->logger;
		log.Category = GL_CAT_SUMMARY;
		log.Event = GL_EVENT_SCANDLVR;

		m_pVBin->LogEvent(&log);		
	}
	catch(...)
	{
		return E_UNEXPECTED;
	}
	
    return ResultFromScode( S_OK );
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetSessionInfo
//Desciprtion	:	Method to get VBININFO Structure associated with the session
//Parameters	:	[OUT] VBININFO *lpstVBinInfo
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetSessionInfo( VBININFO *lpstVBinInfo )
{
	if(NULL == lpstVBinInfo )
		return E_INVALIDARG;

	if(!IsInitialized())
		return E_SESSION_NOT_INITIALIZED;

	try
	{
		memcpy(lpstVBinInfo,&m_stVBinInfo, sizeof(VBININFO));
	}
	catch(...)
	{
		return E_UNEXPECTED;
	}
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetItemCount
//Desciprtion	:	Method to get number of items in the session
//Parameters	:	[OUT] DWORD *lpdwItemCount
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetItemCount( DWORD *lpdwItemCount)
{
	if(NULL == lpdwItemCount)
		return E_INVALIDARG;
	if(!IsInitialized())
		return E_SESSION_NOT_INITIALIZED;
	try
	{
		*lpdwItemCount = m_arIQuarItems.GetSize();
	}
	catch(...)
	{
		return E_UNEXPECTED;
	}
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetFirstItem
//Desciprtion	:	Method to get first Quarantine Item of the session
//Parameters	:	[OUT] IQuarantineItem2**  lplpIQItem
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetFirstItem( IQuarantineItem2**  lplpIQItem)
{
	if(NULL == lplpIQItem)
		return E_INVALIDARG;

	*lplpIQItem = NULL;

	if(!IsInitialized())
		return E_SESSION_NOT_INITIALIZED;

	*lplpIQItem = (IQuarantineItem2*)(m_arIQuarItems.GetAt(0));
	//(*lplpIQItem)->AddRef();

	m_iCurIdx = 0;
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetNextItem
//Desciprtion	:	Method to get Next Quarantine Item of the session
//Parameters	:	[OUT] IQuarantineItem2**  lplpIQItem
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetNextItem( IQuarantineItem2**  lplpIQItem)
{
	if(NULL == lplpIQItem)
		return E_INVALIDARG;
	*lplpIQItem  = NULL;

	if(!IsInitialized())
		return E_SESSION_NOT_INITIALIZED;

	if(++m_iCurIdx >= m_arIQuarItems.GetSize())
		return E_FAIL;

	*lplpIQItem = (IQuarantineItem2*)(m_arIQuarItems.GetAt(m_iCurIdx));
	//(*lplpIQItem)->AddRef();
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	IsInitialized
//Desciprtion	:	Helper Method to check whether the session is initailized or not
//Parameters	:	
//Returns		:	BOOL 
///////////////////////////////////////////////////////////////////////////////
BOOL CSarcSession::IsInitialized()
{
	return m_bInitialized;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetUnpackedFileNameForItem
//Desciprtion	:	Method to get file name of a specified unpackaged item
//Parameters	:	[IN] DWORD dwItemIndex	-	Item Index (ZERO Based)
//					[OUT] LPSTR  lpszUnpackedFileName - File Name
//					[IN] DWORD  bufSize  - File Name Buffer Size	
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetUnpackedFileNameForItem(DWORD dwItemIndex, LPSTR  lpszUnpackedFileName, DWORD  bufSize )
{	
	if(!IsInitialized())
		return E_SESSION_NOT_INITIALIZED;

	if(dwItemIndex < 0 || dwItemIndex > m_arUnpackedFileNames.GetSize() || NULL == lpszUnpackedFileName)
		return E_INVALIDARG;

	if((m_arUnpackedFileNames.GetAt(dwItemIndex)).GetLength() >= bufSize)
		return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);

	try
	{
		strcpy(lpszUnpackedFileName, m_arUnpackedFileNames.GetAt(dwItemIndex));
	}
	catch(...)
	{
		return E_UNEXPECTED;
	}

	return S_OK;
}


STDMETHODIMP CSarcSession::GetStatus( DWORD*  Status )
{
	Status = 0;

    return ResultFromScode( S_OK );
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	SetStatus
//Desciprtion	:	
//Parameters	:	[IN] DWORD  newStatus 
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::SetStatus( DWORD  newStatus )
{
	if(!IsInitialized() || NULL == m_pEventBlk)
		return E_SESSION_NOT_INITIALIZED;

	
    if ( newStatus == QFILE_STATUS_SUBMITTED )
		m_pEventBlk->SNDStatus = SND_OK;
    
	return ResultFromScode( S_OK );
}

STDMETHODIMP CSarcSession::GetAnomalyID( UUID& AnomalyID)
{
	return E_NOTIMPL;	
}

STDMETHODIMP CSarcSession::SetAnomalyID( UUID  AnomalyID)
{
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetAnomalyCat
//Desciprtion	:	Method to get the Anomaly Category
//Parameters	:	[OUT] DWORD  *lpdwAnomalyCat
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetAnomalyCat( DWORD  *lpdwAnomalyCat)
{
	if(NULL == lpdwAnomalyCat)
		return E_INVALIDARG;

	if(!IsInitialized() || NULL == m_pEventBlk)
		return E_SESSION_NOT_INITIALIZED;

	//Anomaly Category is not avialable directly from EventBlock Structure
	//VirusType is interpreted as AnomalyCategory and GenericLoadPoint is considered as default value
    //This needs to be changed once Anomlay Category is available thru VBINNFO or EventBlock
	switch(m_pEventBlk->VirusType & VE_THREAT_CAT_MASK)	
	{
		case 0:
			*lpdwAnomalyCat = 0;
			break;

		case VE_NON_VIRAL_MALICIOUS:
			*lpdwAnomalyCat = IAnomaly::Malicious;
			break;

		case VE_RESERVED_MALICIOUS:
			*lpdwAnomalyCat = IAnomaly::ReservedMalicious;
			break;

		case VE_HEURISTIC:
			*lpdwAnomalyCat = IAnomaly::Heuristic;
			break;

		case VE_SECURITY_RISK_ON:
			*lpdwAnomalyCat = IAnomaly::SecurityRisk;
			break;

		case VE_HACKER_TOOLS:
			*lpdwAnomalyCat = IAnomaly::Hacktool;
			break;

		case VE_SPYWARE:
			*lpdwAnomalyCat = IAnomaly::SpyWare;
			break;

		case VE_TRACKWARE:
			*lpdwAnomalyCat = IAnomaly::Trackware;
			break;

		case VE_DIALERS:
			*lpdwAnomalyCat = IAnomaly::Dialer;
			break;

		case VE_REMOTE_ACCESS:
			*lpdwAnomalyCat = IAnomaly::RemoteAccess;
			break;

		case VE_ADWARE:
			*lpdwAnomalyCat = IAnomaly::Adware;
			break;

		case VE_JOKE_PROGRAMS:
			*lpdwAnomalyCat = IAnomaly::Joke;
			break;
		
		default:
			//For now GenericLoadPoint is default
			*lpdwAnomalyCat = IAnomaly::GenericLoadPoint;
	}
	
	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////
//Name			:	SetAnomalyCat
//Desciprtion	:	Method to set Anomaly Cat 
//Parameters	:	[IN] DWORD  dwAnomalyCat
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::SetAnomalyCat( DWORD  dwAnomalyCat)
{
	//m_dwAnomalyCat = dwAnomalyCat;
	return E_NOTIMPL;	
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetVirusName
//Desciprtion	:	Method to get Virus Name
//Parameters	:	[OUT] LPSTR lpszVirusName
//					[IN,OUT] DWORD  *lpdwBufSize
//Returns		:	HRESULT
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetVirusName( LPSTR lpszVirusName,	//[OUT]
								DWORD  *lpdwBufSize		//[IN,OUT]
								)
{
	if(NULL == lpdwBufSize)
		return E_INVALIDARG;

	if(!IsInitialized() || NULL == m_pEventBlk)
		return E_SESSION_NOT_INITIALIZED;
	
	if(NULL == m_pEventBlk->VirusName)
	{
		*lpdwBufSize = 0;		
		return S_OK;
	}

	try
	{
		if(NULL == lpszVirusName || strlen(m_pEventBlk->VirusName) >= *lpdwBufSize)
		{
			*lpdwBufSize = strlen(m_pEventBlk->VirusName) + 1;
			return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
		}

		*lpdwBufSize = strlen(m_pEventBlk->VirusName) + 1;
		strncpy(lpszVirusName, m_pEventBlk->VirusName, *lpdwBufSize);
	}
	catch(...)
	{
		return E_UNEXPECTED;
	}
	return S_OK;
}
///////////////////////////////////////////////////////////////////////////////
//Name			:	SetVirusName
//Desciprtion	:	Method to set Virus Name
//Parameters	:	[OUT] LPCSTR lpszVirusName
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::SetVirusName( LPCSTR lpszVirusName)
{
	return E_NOTIMPL;
}
///////////////////////////////////////////////////////////////////////////////
//Name			:	GetThreatCat
//Desciprtion	:	Method to get Threat Category
//Parameters	:	[OUT] DWORD *lpdwThreatCat
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetThreatCat( DWORD *lpdwThreatCat)
{
	if(NULL == lpdwThreatCat)
		return E_INVALIDARG;
	
	if(!IsInitialized() || NULL == m_pEventBlk)
		return E_SESSION_NOT_INITIALIZED;

	*lpdwThreatCat = m_pEventBlk->VirusType & VE_THREAT_CAT_MASK;

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	SetThreatCat
//Desciprtion	:	Method to set Virus Threat Category
//Parameters	:	[IN] DWORD dwThreatCat
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::SetThreatCat( DWORD dwThreatCat)
{
	//m_dwThreatCat = dwThreatCat;
	//return S_OK;
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetUniqueID
//Desciprtion	:	Method to get Unique ID
//Parameters	:	[OUT] LPSTR  lpszuuid
//					[IN] DWORD  bufSize  - Buffer Size	
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetUniqueID( LPSTR  lpszuuid, DWORD dwBufSize)
{
	unsigned char *strBuf = NULL;
	HRESULT hr = S_OK;

	try
	{
		// Get the GUID from from VBININFO
		UUID uuid = m_stVBinInfo.ExtraInfo.stUniqueId;
		hr = UuidToString(&uuid, &strBuf);
		if( RPC_S_OK == hr )
		{
			if(dwBufSize > strlen((LPCSTR)strBuf))
				strcpy(lpszuuid, (LPCSTR)strBuf);
			else
				hr = DISP_E_BUFFERTOOSMALL;

			RpcStringFree(&strBuf);
		}
		else
			hr = E_FAIL;
	}
	catch(...)
	{
		return E_UNEXPECTED;
	}
	return hr;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	SetUniqueID
//Desciprtion	:	Method to set Unique ID
//Parameters	:	[IN] LPCSTR  lpszNewID
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::SetUniqueID( LPCSTR  lpszNewID )
{
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetAnomalyID
//Desciprtion	:	Method to get AnomalyID
//Parameters	:	[OUT] LPSTR  lpszAnomalyID
//					[IN] DWORD  bufSize  - Buffer Size	
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetAnomalyID( LPSTR  lpszAnomalyID, DWORD dwBufSize )
{
	if(NULL == lpszAnomalyID)
		return E_INVALIDARG;
	if(m_strAnomalyID.GetLength() >= dwBufSize)
		return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);

	strcpy(lpszAnomalyID, m_strAnomalyID.GetBuffer(0));
	m_strAnomalyID.ReleaseBuffer();
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	SetAnomalyID
//Desciprtion	:	Method to set AnomalyID in string fomrat
//Parameters	:	[IN] LPCSTR  lpszAnomalyID
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::SetAnomalyID(LPCSTR  lpszAnomalyID)
{
	m_strAnomalyID = lpszAnomalyID;
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetSampleType
//Desciprtion	:	Method to get Sample Type
//Parameters	:	[OUT] DWORD *lpdwSampleType
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetSampleType( DWORD *lpdwSampleType)
{
	if(NULL == lpdwSampleType)
		return E_INVALIDARG;

	*lpdwSampleType = m_dwSampleType;
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	SetSampleType
//Desciprtion	:	Method to set SampleType
//Parameters	:	[IN] DWORD dwSampleType
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::SetSampleType( DWORD dwSampleType)
{
	m_dwSampleType = dwSampleType;	
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetVirusID
//Desciprtion	:	Method to get Session's Virus ID
//Parameters	:	[OUT] DWORD *lpdwVirusID
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetVirusID( DWORD *lpdwVirusID)
{
	if(NULL == lpdwVirusID)
		return E_INVALIDARG;

	if(!IsInitialized() || NULL == m_pEventBlk)
		return E_SESSION_NOT_INITIALIZED;
	
	*lpdwVirusID = m_pEventBlk->VirusID;
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	SetVirusID
//Desciprtion	:	Method to set VirusID
//Parameters	:	[IN] DWORD dwVirusID
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::SetVirusID( DWORD dwVirusID)
{
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetDefinitionsDate
//Desciprtion	:	Method to get Virus Definitions Date
//Parameters	:	[OUT] SYSTEMTIME *lptTime 
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetDefinitionsDate( SYSTEMTIME *lptTime )
{
	//Function arguments validation
	if(NULL == lptTime)
		return E_INVALIDARG;

	if(!IsInitialized() || NULL == m_pEventBlk)
		return E_SESSION_NOT_INITIALIZED;

	lptTime->wDay = m_pEventBlk->DefVersionInfo.wDay;
	lptTime->wMonth = m_pEventBlk->DefVersionInfo.wMonth;
	lptTime->wYear = m_pEventBlk->DefVersionInfo.wYear;
	
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetItem
//Desciprtion	:	Method to get QuarantineItem interface pointer at a specified
//					index
//Parameters	:	[IN] int iItemIndex	-	Item Index (ZERO Based)
//					[OUT] IQuarantineItem2**  lplpIQItem 
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetItem( int iItemIndex, IQuarantineItem2**  lplpIQItem )
{
	if(NULL == lplpIQItem)
		return E_INVALIDARG;

	if(iItemIndex <0 || iItemIndex > m_arIQuarItems.GetSize())
		return E_INVALID_ITEM_INDEX;
	try
	{
		*lplpIQItem  = (IQuarantineItem2*)(m_arIQuarItems.GetAt(iItemIndex));
		//(*lplpIQItem)->AddRef();
	}
	catch(...)
	{
		return E_UNEXPECTED;
	}
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetDescription
//Desciprtion	:	Method to get the Description to display the Threats
//Parameters	:	[OUT] LPSTR lpszThreatDesc
//					[IN, OUT] DWORD  *lpdwBufSize
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetDescription( LPSTR lpszThreatDesc,DWORD  *lpdwBufSize)
{
	HRESULT hrRetVal = S_OK;

	if(NULL == lpdwBufSize)
		return E_INVALIDARG;

	if(!IsInitialized() || NULL == m_pEventBlk)
		return E_SESSION_NOT_INITIALIZED;	

	if(NULL == m_pEventBlk->Description)
	{
		*lpdwBufSize = 0;		
		return S_OK;
	}

	if(NULL == lpszThreatDesc || strlen(m_pEventBlk->Description) >= *lpdwBufSize)
	{
		*lpdwBufSize = strlen(m_pEventBlk->Description) + 1;
		return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
	}

	*lpdwBufSize = strlen(m_pEventBlk->Description) + 1;
	strcpy(lpszThreatDesc, m_pEventBlk->Description);
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
//Name			:	GetAnomalyCat
//Desciprtion	:	Method to get the anomaly categories in string format
//Parameters	:	[OUT] LPSTR lpszBuf
//					[IN, OUT] DWORD  *lpdwBufSize
//Returns		:	HRESULT 
///////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CSarcSession::GetAnomalyCat( LPSTR lpszBuf, DWORD  *lpdwBufSize)
{
	HRESULT hrRetVal = S_OK;
	EventCustomInfo*		eventInfo = NULL;
	CString strTmp = _T("");
	
	if(NULL == lpdwBufSize)
		return E_INVALIDARG;

	if(!IsInitialized() || NULL == m_pEventBlk)
		return E_SESSION_NOT_INITIALIZED;	

	//If EventBlock's EventData is empty then use VirusType to get the AnomalyCat
	if(m_pEventBlk->EventData == NULL || m_pEventBlk->EventData[0] == NULL)
	{
		DWORD	dwAnomalyCat = 0;
		hrRetVal = GetAnomalyCat(&dwAnomalyCat);	//GetAnomalyCat(DWORD) uses VirusType
		if(FAILED(hrRetVal))
			return hrRetVal;
		strTmp.Format("%lu", dwAnomalyCat);
		if(strTmp.GetLength() >= *lpdwBufSize)
		{
			*lpdwBufSize =  strTmp.GetLength() + 1;
			return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);			
		}
		*lpdwBufSize = strTmp.GetLength() + 1;
		strcpy(lpszBuf, (LPCSTR)(strTmp));
		return S_OK;
	}

	hrRetVal = EventCustomInfo_Create(m_pEventBlk->Event, (LPCTSTR)(m_pEventBlk->EventData), &eventInfo);
	if(FAILED(hrRetVal))
		return hrRetVal;

	CAnomalyEventInfo* anomalyEventInfo = event_custom_info_cast<CAnomalyEventInfo*>(eventInfo);

	if(NULL == anomalyEventInfo && anomalyEventInfo->m_strAnomalyCategories.empty())
	{
		*lpdwBufSize = 0;
		hrRetVal =  S_OK;
		goto ExitFn;
	}	
	
	strTmp = anomalyEventInfo->m_strAnomalyCategories.data();
	//Will be in the format <Number of categories>ANOMALY_CATEGORIES_DELIMITER<category1>ANOMALY_CATEGORIES_DELIMITER<category2>....
	strTmp = strTmp.Right(strTmp.GetLength() - strTmp.Find(ANOMALY_CATEGORIES_DELIMITER)-1);
	strTmp.Replace(ANOMALY_CATEGORIES_DELIMITER , " ");	

	if(strTmp.GetLength() >= *lpdwBufSize)
	{
		*lpdwBufSize =  strTmp.GetLength() + 1;
		hrRetVal = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
		goto ExitFn;
	}
	*lpdwBufSize = strTmp.GetLength() + 1;
	strcpy(lpszBuf, (LPCSTR)(strTmp));
	
ExitFn:
	if (eventInfo != NULL)
	{
		delete eventInfo;
		eventInfo = NULL;
	}
	return hrRetVal;
}
