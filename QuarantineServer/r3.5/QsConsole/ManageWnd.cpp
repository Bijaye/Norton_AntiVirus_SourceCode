/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// ManageWnd.cpp : implementation file
//

#include "stdafx.h"
#include "qscon.h"
#include "qscon.h"
#include <atlsnap.h>
#include "QSconsole.h"
#include "qsconsoledata.h"
#include "ManageWnd.h"
#include "qscommon.h"
#include "qsregval.h"
#include "SampleReadThread.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
UINT    CQSConsoleData::m_msgDisplayProgressText= ::RegisterWindowMessage( _T("SYMANTEC_QS_DISPLAY_PROGRESS") );
UINT    CQSConsoleData::m_msgDisplayRecordText = ::RegisterWindowMessage( _T("SYMANTEC_QS_DISPLAY_RECORD_COUNT") );
UINT    CQSConsoleData::m_msgRefresh = ::RegisterWindowMessage( _T("SYMANTEC_QS_REFRESH_QS_SAMPLES") );
UINT    CQSConsoleData::m_msgReadFinished = ::RegisterWindowMessage( _T("SYMANTEC_QS_SAMPLE_READ_FINISHED") );

/////////////////////////////////////////////////////////////////////////////
// CManageWnd

BOOL CQSConsoleData::CreateManageWnd(CQSConsoleComponent* pCQSConsoleComponent)
{
    BOOL bRet = FALSE;
	
    if (!m_hwndManage)
    {
        CManageWnd* pWnd = new CManageWnd();
		pWnd->m_pRootNode = this;
		pWnd->m_pCQSConsoleComponent = pCQSConsoleComponent;
        bRet = pWnd->Create( NULL,
                             NULL,
                             WS_OVERLAPPED,
                             CRect( 0, 0, 10, 10 ),
                             CWnd::FromHandle(m_hMainWnd),
                             0 );
        ASSERT( bRet );
        if (bRet)
		{
            m_hwndManage = pWnd->GetSafeHwnd();
			m_pHiddenWnd = pWnd;
//			if (NeedAttention())
//			{
//				pWnd->m_dwIconIndex = 1;
//			}
		}
    }

    return bRet;
}

CManageWnd::CManageWnd()
{
	m_hTimer=0;
	m_dwPasses = 0;
	m_dwIconIndex = 0;
	m_pRootNode = NULL;
	m_pCQSConsoleComponent=NULL;
	m_dwTimerIntervel = REFRESH_TIME_DEFAULT;
	m_bSetImage=FALSE;
}

CManageWnd::~CManageWnd()
{
}


BEGIN_MESSAGE_MAP(CManageWnd, CWnd)
	//{{AFX_MSG_MAP(CManageWnd)
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CManageWnd message handlers

void CManageWnd::OnTimer(UINT nIDEvent) 
{
	if (m_pRootNode->m_dwRefreshInProgress == FALSE)
	{
		if (m_dwPasses++ >= m_dwTimerIntervel)
		{
			if (m_pRootNode->GetAttached()== TRUE)
			{

				m_pRootNode->OnNotifyRefresh(m_pRootNode->m_pConsole,m_pCQSConsoleComponent);
				ReadTimerIntervalFromReg();
			}
			m_dwPasses = 0;
		}
	}
	CWnd::OnTimer(nIDEvent);
}

BOOL CManageWnd::DestroyWindow() 
{
	BOOL bRc;
	bRc = CWnd::DestroyWindow();
	return bRc;
}


int CManageWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	ReadTimerIntervalFromReg();
	
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_hTimer = SetTimer(0, REFRESH_TIME_INTERVELBASE, NULL);	
	return 0;
}

void CManageWnd::ReadTimerIntervalFromReg(void)
{
	CRegKey reg;
	LONG lResult;
	// 
	// Read in values from registry
	// 
	lResult = reg.Open( HKEY_LOCAL_MACHINE,
						REGKEY_QSCONSOLE);
    if( ERROR_SUCCESS != lResult )
	{
		lResult = reg.Create( HKEY_LOCAL_MACHINE,
							REGKEY_QSCONSOLE);
		if(ERROR_SUCCESS == lResult )
			reg.SetDWORDValue (REGVAL_REFRESH_INTERVAL, REFRESH_TIME_DEFAULT);
	}
	lResult = reg.QueryDWORDValue(REGVAL_REFRESH_INTERVAL, m_dwTimerIntervel);
	if(ERROR_SUCCESS != lResult )
	{
		m_dwTimerIntervel = REFRESH_TIME_DEFAULT;
		reg.SetDWORDValue (REGVAL_REFRESH_INTERVAL, REFRESH_TIME_DEFAULT );
	}
	reg.Close();
}

void CManageWnd::OnDestroy() 
{

	KillTimer(m_hTimer);	
	CleanupDataLists();
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

BOOL CManageWnd::NeedChangeQSIcon(void)
{
	BOOL bRc = FALSE;

	if (!m_pRootNode->NeedAttention())
	{
		if(m_dwIconIndex != 0)
		{
			bRc = TRUE;
			m_dwIconIndex = 0;
		}
	}
	else
	{
		if (m_dwIconIndex !=1)
		{
			m_dwIconIndex = 2;
			bRc = TRUE;
		}
	}

	return bRc;
}


BOOL CManageWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::OnNotify(wParam, lParam, pResult);
}

void CManageWnd::PostNcDestroy() 
{
	delete this;
	
	CWnd::PostNcDestroy();
}

LRESULT CManageWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
    if (message == CQSConsoleData::m_msgDisplayProgressText)
	{
        OnDisplayProgressText( wParam, lParam );
    }

    if (message == CQSConsoleData::m_msgDisplayRecordText)
    {
        OnDisplayRecordCount( wParam, lParam );
    }

    if (message == CQSConsoleData::m_msgRefresh)
	{
        OnNotifyRefresh( wParam, lParam );
    }
	
    if (message == CQSConsoleData::m_msgReadFinished)
	{
        OnNotifyReadFinished( wParam, lParam );
    }

	return CWnd::WindowProc(message, wParam, lParam);
}


HRESULT CManageWnd::OnDisplayProgressText( WPARAM wParam, LPARAM lParam )
{
	CString sStatus;
	USES_CONVERSION;
    CComPtr< IConsole > pc = (IConsole*)m_pCQSConsoleComponent->m_spConsole;
	CComQIPtr< IConsole2 > pc2 = pc;

 
	if (pc2 != NULL)
	{
	sStatus = m_pRootNode->GetStatusBarProgress();
	pc2->SetStatusText(T2OLE(sStatus.GetBuffer(0)));
	}
	return S_OK;
}

HRESULT CManageWnd::OnDisplayRecordCount( WPARAM wParam, LPARAM lParam )
{
	CString sStatus;
	USES_CONVERSION;    
	CComPtr< IConsole > pc = (IConsole*)m_pCQSConsoleComponent->m_spConsole;
	CComQIPtr< IConsole2 > pc2 = pc;

	if (pc2 != NULL)
	{
		sStatus = m_pRootNode->GetStatusBarVirusDefDateText() + m_pRootNode->GetStatusSampleCountText();
		pc2->SetStatusText(T2OLE(sStatus.GetBuffer(0)));
	}	
	return S_OK;
}

HRESULT CManageWnd::OnNotifyRefresh( WPARAM wParam, LPARAM lParam )
{

	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	POSITION pos;
	m_dwPasses = 0;
	CMapPtrToPtr *pDataList=NULL;
	if (lParam != NULL)
	{
		pDataList = (CMapPtrToPtr*)lParam;
	}
	else
	{
		return E_FAIL;
	}
	if (m_bSetImage == FALSE)
	{
		CComQIPtr<IImageList, &IID_IImageList> pIImageList(m_pRootNode->m_pConsole);
		m_pRootNode->OnNotifyAddImages(pIImageList);
		m_bSetImage = TRUE;
	}
	HRESULT hr = m_pRootNode->SyncDataLists(pDataList);
	hr = m_pRootNode->UpdateResultItems (m_pRootNode->m_pConsole,m_pCQSConsoleComponent);
	pos = m_pRootNode->m_pListofDataList->Find((LPVOID)pDataList);
	m_pRootNode->DestroyData(pDataList);
	m_pRootNode->m_pListofDataList->RemoveAt(pos);
	delete pDataList;
	pDataList = NULL;
	return S_OK;
}

HRESULT CManageWnd::OnNotifyReadFinished( WPARAM wParam, LPARAM lParam )
{
	HRESULT hr ;
    // 
    // Wait here forever for thread to stop.
    // 
    WaitForSingleObject( (HANDLE) m_pRootNode->m_dwThread, INFINITE);
	CloseHandle((HANDLE)m_pRootNode->m_dwThread);
	m_pRootNode->m_dwThread = 0xffffffff;
	m_bSetImage = FALSE;
	if (wParam == FALSE)
		hr = m_pRootNode->PurgeMarkedItems (m_pRootNode->m_pConsole,m_pCQSConsoleComponent);

	if(NeedChangeQSIcon())
		m_pRootNode->ChangeIcon(m_pRootNode->m_pConsole,m_dwIconIndex);
	m_dwPasses = 0;
	CleanupDataLists();

	return S_OK;
}

void CManageWnd::CleanupDataLists( void)
{
	CMapPtrToPtr *pDataList=NULL;

	//Window closing need to destroy all pending data lists.
	
		// all done with update release flag
	if(m_pRootNode->m_dwRefreshInProgress)
		InterlockedDecrement(&m_pRootNode->m_dwRefreshInProgress);
	if(m_pRootNode->m_pListofDataList != NULL)
	{
		POSITION pos =m_pRootNode->m_pListofDataList->GetHeadPosition();

		while( pos != NULL )
		{
			pDataList= (CMapPtrToPtr *) m_pRootNode->m_pListofDataList->GetNext( pos );
			if (pDataList != NULL)
			{
				m_pRootNode->DestroyData(pDataList);
				delete pDataList;
				pDataList = NULL;
			} 
		}
		m_pRootNode->m_pListofDataList->RemoveAll();
		delete m_pRootNode->m_pListofDataList;
		m_pRootNode->m_pListofDataList = NULL;
	}
	if (m_pRootNode->m_pSRead != NULL)
	{
		delete m_pRootNode->m_pSRead;
		m_pRootNode->m_pSRead = NULL;
	}
	_heapmin();
}