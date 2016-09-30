/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// FetchFileData.cpp: implementation of the CSampleRead class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "atlsnap.h"
#include "resource.h"
#include "process.h"
#import "qserver.tlb" no_namespace
#include "macros.h"
#include "qscon.h"
#include "qsconsoledata.h"
#include "QSConsole.h"
#include "samplereadthread.h"
#include "dataobj.h"
#include "itemdata.h"
#include <new>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CSampleRead::CSampleRead
//
// Description   : 
//
// Return type   : 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CSampleRead::CSampleRead() 
{
    m_dwThread = INVALID_THREAD;
    m_hAbort = ::CreateEvent( NULL, FALSE, FALSE, NULL );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CSampleRead::~CSampleRead
//
// Description   : 
//
// Return type   : 
//
//
///////////////////////////////////////////////////////////////////////////////
// 2/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CSampleRead::~CSampleRead()
{


	// 
	// Clean up event resource.
	// 
	::CloseHandle( m_hAbort );

}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CSampleRead::GetFile
//
// Description   : 
//
// Return type   : HRESULT 
//
// Argument      : DWORD dwFileID 
// Argument      : LPUNKNOWN pUnk
// Argument      : LPCTSTR szDestPath
// Argument      : LPFNFETCHCALLBACK pCallback
//
///////////////////////////////////////////////////////////////////////////////
// 2/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HRESULT CSampleRead::ReadAllSamples(CSnapInItem* pRootNode)
{
    HRESULT hr = S_OK;
	UINT uThreadID;
	
	
	
	try
	{
		
		m_pRootNode = (CQSConsoleData *) pRootNode;

		_ASSERTE(m_pRootNode->m_cQSInterfaces);
		if (m_pRootNode->m_cQSInterfaces != NULL)
		{
			m_pIdentity = m_pRootNode->m_cQSInterfaces->m_pQServer.DCO_GetIdentity();
			// allocate list of list data pointers grow by 10
			m_pRootNode->m_pListofDataList = new CPtrList(10);
			// 
			// Marshal QServer interface.
			// 
			hr = CoMarshalInterThreadInterfaceInStream( __uuidof( IQuarantineServer), m_pRootNode->m_cQSInterfaces->m_pQServer, &m_pQSStream);
			if( FAILED( hr ) )
				return hr;
			
			// 
			// Fire of background thread.
			// 
			m_dwThread = _beginthreadex(NULL, 0,&CSampleRead::ThreadFunc, this,0, &uThreadID);
			if( m_dwThread == 0 )
				return E_FAIL;
			SetThreadPriority( (HANDLE)m_dwThread, THREAD_PRIORITY_BELOW_NORMAL );
		}
		else
		{
			hr = E_FAIL;
			_com_issue_error( hr );
		}
	}
	catch(...)
	{
		hr = E_FAIL;
	}
	m_pRootNode->m_dwThread = m_dwThread;
	return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name : CSampleRead::ThreadFunc
//
// Description   : 
//
// Return type   : void 
//
// Argument      : void* pData
//
///////////////////////////////////////////////////////////////////////////////
// 2/23/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
unsigned int __stdcall CSampleRead::ThreadFunc( void* pData )
{
    BOOL bError = FALSE;
	HRESULT hr = S_OK;    
    CComPtr<IEnumQserverItemsVariant> pEnum; 
    CComPtr< IQuarantineServer> pQServer;
	ULONG ulPercentRead =0 ;
	CString sStatus;
	int i = 0;
	BOOL bAbort = FALSE;
	POSITION pos = 0;
	CoInitialize( NULL );
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	// 
	// Get the array of attributes required.
	// 
	SAFEARRAY* psa = CItemData::GetDataMemberArray();
	SafeArrayLock( psa );
	UINT uArraySize = SafeArrayGetElemsize(psa);
    // 
    // Save off data 
    // 
    CSampleRead* pThis = ( CSampleRead* ) pData;
	
    // 
    // Unmarshal our QServer interface.
    // 
    if( FAILED( CoGetInterfaceAndReleaseStream( pThis->m_pQSStream, __uuidof( IQuarantineServer), (LPVOID*)&pQServer) ) )
	{
		goto EXIT;
	}
	
	
    // 
    // Need to set proxy blanket
    //
    if( FAILED( CoSetProxyBlanket( pQServer,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CONNECT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		(_COAUTHIDENTITY*) pThis->m_pIdentity,
		EOAC_NONE) ) )
	{
		goto EXIT;
	}
	
	
	
	while (TRUE)
	{
		if( psa == NULL )
		{
			CoUninitialize();
			return 1;
		}
		try
		{
			ULONG fetched = 0;
			
			
			ULONG ulDone=0;

			// 
			// Set up variant array
			// 
			VARIANT vArray;
			VariantInit( &vArray );
			vArray.vt = VT_ARRAY;
			vArray.parray = psa;
			ULONG ulNoItems=0;
			
			sStatus.Format(IDS_READ_STATUS,1);
			sStatus.Insert(sStatus.Find(_T("|"))+1,_T("%"));
			pThis->m_pRootNode->SetStatusBarProgress(sStatus);
			PostMessage(pThis->m_pRootNode->m_hwndManage, CQSConsoleData::m_msgDisplayProgressText,0,0);
			sStatus.Empty();

			// 
			// Get the enumeration interface.
			// 
			// how many items?
			
			hr = pQServer->EnumItemsVariant( &vArray, &pEnum );
			if( FAILED( CoSetProxyBlanket( pEnum,
				RPC_C_AUTHN_WINNT,
				RPC_C_AUTHZ_NONE,
				NULL,
				RPC_C_AUTHN_LEVEL_CONNECT,
				RPC_C_IMP_LEVEL_IMPERSONATE,
				(_COAUTHIDENTITY*) pThis->m_pIdentity,
				EOAC_NONE) ) )
			{
				_com_issue_error( hr );
			}
			

			hr = pQServer->GetItemCount(&ulNoItems);
			// 
			// Begin enumeration.
			// 
			VARIANT items[READ_NO_SAMPLES];

			while(  pEnum->Next( READ_NO_SAMPLES, items, &fetched )== S_OK )
			{
				
				pThis->m_pDataList = new CMapPtrToPtr;
				for( ULONG i = 0; i < fetched; i++ )
				{
					//  
					// Add elements.
					// 
					CItemData* pData = NULL;
					try
					{

						pData = new CItemData;
					}
					catch (std::bad_alloc &){}

					// 
					// Initialize safe array construct
					// 
					hr = pData->Initialize( items[i].parray );
					if( FAILED( hr ) )
					{
						delete pData;
						VariantClear( &items[i] );
						continue;
					}
					
					// 
					// Add to list
					// 
					pThis->m_pDataList->SetAt( pData, pData );
					
					//  
					// Clean up returned safe array array.
					//
					VariantClear( &items[i] );
				}
				ulDone += fetched;
				if(ulNoItems != 0)
				{
					ulPercentRead = (ulDone*100/ulNoItems);
					sStatus.Format(IDS_READ_STATUS,ulPercentRead);
					sStatus.Insert(sStatus.Find(_T("|"))+1,_T("%"));
					pThis->m_pRootNode->SetStatusBarProgress(sStatus);
					PostMessage(pThis->m_pRootNode->m_hwndManage, CQSConsoleData::m_msgDisplayProgressText,0,0);
					sStatus.Empty();
				}
				fetched = 0;

                if( WaitForSingleObject( pThis->m_hAbort, 0 ) != WAIT_TIMEOUT )
                    {
                    bAbort = TRUE;
					pThis->m_pRootNode->DestroyData(pThis->m_pDataList);
					delete pThis->m_pDataList;
                    break; 
                   }
				else
				{
					pos = pThis->m_pRootNode->m_pListofDataList->AddTail((LPVOID)pThis->m_pDataList);
					PostMessage(pThis->m_pRootNode->m_hwndManage, CQSConsoleData::m_msgRefresh,0,(LPARAM)(LPVOID)(pThis->m_pDataList));
				}

			}// end while
			if (bError != TRUE && bAbort == FALSE)
			{
				pThis->m_pRootNode->SetStatusSampleCountText(ulNoItems);
			}
			break;
		}
		catch( _com_error e )
		{
			if (++i == 2)
			{
				bError = TRUE;
				break;
			}
//			else
//			{
//				pThis->m_pRootNode->ReAttach();
//
//			}
		}
		catch( ... )
		{
			bError = TRUE;
			break;
		} 
		}
		
		if (/*bError != TRUE && */bAbort == FALSE)
			PostMessage(pThis->m_pRootNode->m_hwndManage, CQSConsoleData::m_msgDisplayRecordText,0,0);
		
EXIT:
		// 
		// Clean up allocated safe array.
		// 
		SafeArrayUnlock( psa );
		SafeArrayDestroy( psa );   
		
		if (bError != TRUE && bAbort == FALSE)
			PostMessage(pThis->m_pRootNode->m_hwndManage, CQSConsoleData::m_msgReadFinished,0,0);
		else
			PostMessage(pThis->m_pRootNode->m_hwndManage, CQSConsoleData::m_msgReadFinished,TRUE,0);
		
//		delete pThis;
		CoUninitialize();
		_endthreadex(0);
		return 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// Function name : CSampleRead::Abort
//
// Description   : Aborts the sample read.
//
// Return type   : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/2/99 - DBUCHES: Function created / header added 
///////////////////////////////////////////////////////////////////////////////
void CSampleRead::Abort()
{
    // 
    // Set abort event.
    // 
    SetEvent( m_hAbort );

    // 
    // Wait here forever for thread to stop.
    // 
    WaitForSingleObject( (HANDLE) m_dwThread, INFINITE );

}
