/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/
// HiddenWndMmcThread.cpp : implementation file
//

#include "stdafx.h"
//#include "qscon.h"
//#include "qscon.h"
//#include <atlsnap.h>
//#include "QSconsole.h"
//#include "qsconsoledata.h"
//#include "qscommon.h"
#include "ServerConfigData.h"
#include "HiddenWndMmcThread.h"

extern HWND g_hMainWnd;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHiddenWndMmcThread


BEGIN_MESSAGE_MAP(CHiddenWndMmcThread, CWnd)
	//{{AFX_MSG_MAP(CHiddenWndMmcThread)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




// CREAT THE HIDDEN WINDOW TO HANDLE WRITING TO REMOTE REGISTRY
BOOL CServerConfigData::CreateHiddenWndThread(CServerConfigData* pSConfigData)
{
    BOOL bRet = FALSE;

    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    
    if (!m_hwndManage)
    {
        //CHiddenWndMmcThread* pWnd   = new CHiddenWndMmcThread;
        m_pWnd  = new CHiddenWndMmcThread;
        if( m_pWnd == NULL )
            return(FALSE);

        bRet = m_pWnd->Create( NULL,
                             NULL,
                             WS_OVERLAPPED,
                             CRect( 0, 0, 10, 10 ),
                             CWnd::FromHandle(g_hMainWnd),    //m_hMainWnd
                             0 );
        ASSERT( bRet );
        if (bRet)
        {
            m_hwndManage = m_pWnd->GetSafeHwnd();
            m_pWnd->m_pSConfigData = pSConfigData;
            //m_pWnd = pWnd;                 //pSConfigData->m_pWnd = pWnd;
            //pWnd->m_pWnd = pWnd;

            // REGISTER WINDOW MESSAGE
            if( dwWmsgSConfigWriteData == 0 )
                dwWmsgSConfigWriteData = RegisterWindowMessage(REGISTERED_WMSG_SCONFIG_WRITE_DATA);

        }
    }

    // DEBUG Check current thread 12/29/99
    m_pWnd->m_dwEntryThreadID = SaveCurrentThreadId();

  	fWidePrintString("CServerConfigData::CreateHiddenWndThread CHiddenWndMmcThread");

    return bRet;
}

CHiddenWndMmcThread::CHiddenWndMmcThread()
{
    m_dwEntryThreadID     = 0;    
    m_dwRegWriteThreadID  = 0; 
    m_pSConfigData        = NULL;
    //m_pWnd                = NULL;
}

CHiddenWndMmcThread::~CHiddenWndMmcThread()
{
}


HRESULT  CServerConfigData::WriteConfigDataToServer()
{
    HRESULT hr    = S_OK;

    if( m_hwndManage != NULL )
    {
        m_bWriteDataRoutineDone = FALSE;
    	fWidePrintString("CServerConfigData::WriteConfigDataToServer Post Message to hidden window");
        ::PostMessage( m_hwndManage, WM_USER, dwWmsgSConfigWriteData, 0 );  //1/2/00
        //::SendMessage( m_hwndManage, WM_USER, dwWmsgSConfigWriteData, 0 );   // causes a crash
    	fWidePrintString("CServerConfigData::WriteConfigDataToServer message sent");

        // WAIT MAX 2 sec
        for( int i = 0; !m_bWriteDataRoutineDone && i < 20; i++ )
        {
            Sleep(100);
        }

//        Sleep(3000);
    	fWidePrintString("CServerConfigData::WriteConfigDataToServer Sleep is done");
    }

    return(hr);
}



// DESTROY THE HIDDEN WINDOW 12/29/00
BOOL CServerConfigData::ReleaseHiddenWndThread()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

   	fWidePrintString("CServerConfigData::ReleaseHiddenWndThread");
    return(TRUE);  // 1/2/00 jhill test


//     if( m_pWnd != NULL)
//     {
//         CHiddenWndMmcThread* pWnd = (CHiddenWndMmcThread*) m_pWnd;
//         delete pWnd;
//         m_pWnd = NULL;
//         return(TRUE);
//     }

    // DESTROY THE HIDDEN WINDOW 12/29/00
    if( m_hwndManage != NULL )
    {
//        CHiddenWndMmcThread* pWnd = (CHiddenWndMmcThread*) m_pWnd;
//        if( pWnd != NULL )
//        {
//            pWnd->DestroyWindow();   // CWnd::DestroyWindow() cause an Access violation
//            delete pWnd;
//            m_pWnd = NULL;
//        }
//        return(TRUE);


    //     CHiddenWndMmcThread*  pTempWin = new CWnd;
    //     if( pTempWin!=NULL )
    //         pTempWin->Attach(m_hwndManage);



    //     if( m_pWnd != NULL )
    //     {
    //         DestroyWindow()
    //         delete m_pWnd;
    //         m_pWnd = NULL;
    //     }


        //CHiddenWndMmcThread* pWnd = (CHiddenWndMmcThread*) m_pWnd;
        if( m_pWnd != NULL )
        {
            //delete m_pWnd;
//          pWnd->DestroyWindow();
            m_pWnd->SendMessage( WM_DESTROY, 0, 0 );  // WM_DESTROY  WM_CLOSE
        }
        else
            ::PostMessage( m_hwndManage, WM_DESTROY, 0, 0 );  // WM_DESTROY  WM_CLOSE

        m_hwndManage = NULL;
        m_pWnd = NULL;
    }

    return(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CHiddenWndMmcThread message handlers


LRESULT CHiddenWndMmcThread::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
    HRESULT hr    = S_OK;

    // IS THIS FOR US?
    if( message == WM_USER &&  wParam == dwWmsgSConfigWriteData && dwWmsgSConfigWriteData != 0 )
    {
        // DEBUG Check current thread 12/29/99
        m_dwRegWriteThreadID = SaveCurrentThreadId();
    	fWidePrintString("CHiddenWndMmcThread::WindowProc start writing");

        if( m_pSConfigData != NULL )
            hr = m_pSConfigData->WorkerWriteConfigDataToServer();
        m_pSConfigData->m_bWriteDataRoutineDone = TRUE;


    	fWidePrintString("CHiddenWndMmcThread::WindowProc done writing");
    	//fWidePrintString("CHiddenWndMmcThread::WindowProc called. Caller's ObjectID= %d", m_iExtDataObjectID);
    }
	return CWnd::WindowProc(message, wParam, lParam);
}




// BOOL CHiddenWndMmcThread::DestroyWindow() 
// {
// 	// TODO: Add your specialized code here and/or call the base class
// 	return CWnd::DestroyWindow();
// }


int CHiddenWndMmcThread::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	//ReadTimerIntervalFromReg();
	
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//m_hTimer = SetTimer(0, REFRESH_TIME_INTERVELBASE, NULL);	
	return 0;
}


// void CHiddenWndMmcThread::OnDestroy() 
// {
//    	fWidePrintString("CHiddenWndMmcThread::OnDestroy");
// 
// 	SaveCurrentThreadId();	
// 	CWnd::OnDestroy();
// 	
// 	// TODO: Add your message handler code here
// 	
// }



