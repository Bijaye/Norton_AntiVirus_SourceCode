////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "SessionAppWindow.h"
#include "SessionAppWindowThread.h"
#include "SessionAppServerImpl.h"

#include <ccCoInitialize.h>

using namespace SessionApp;

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAppWindowThread::CAppWindowThread(void) :
m_pSessionAppServer(NULL)
{
    m_hRunning = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CAppWindowThread::~CAppWindowThread(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAppWindowThread::IsRunning()
{
    HRESULT hrReturn = S_OK;

    DWORD dwRet = WaitForSingleObject(m_hRunning, 0);
    if(WAIT_TIMEOUT == dwRet)
    {
        hrReturn = S_OK;
    }
    else if(WAIT_OBJECT_0 == dwRet)
    {
        hrReturn = S_FALSE;
    }
    else
    {
        hrReturn = E_FAIL;
    }

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAppWindowThread::RequestExit()
{
    BOOL bFailed = SetEvent(m_hRunning);

    return bFailed ? E_FAIL : S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAppWindowThread::Run()
{
    HRESULT hrResult = E_FAIL;

    // Need to init COM on this thread
    ccLib::CCoInitialize coInit(ccLib::CCoInitialize::eMTAModel);

    StahlSoft::HRX hrx;
    try
    {
        if(NULL == m_wndAppWindow.Create(NULL))
        {
            CCTRCTXE0(_T("Failed to create App Window. Exiting..."));
            throw _com_error(E_FAIL);
        }
        
        CCTRCTXI0(_T("AppWindow successfully created.") );
        m_pSessionAppServer->OnTrayAppReady();

        // Wait up here and process messages.
        ccLib::CMessageLock lock( TRUE, TRUE );

        // Wait indefinitely
        DWORD dwRet;
        BOOL bRunning = TRUE;
        HANDLE hWaitHandles[] = {m_hRunning.m_h};    
        while(bRunning)
        {
            dwRet = lock.Lock( CCDIMOF(hWaitHandles), hWaitHandles, FALSE, INFINITE, FALSE );
            switch( dwRet )
            {
            case WAIT_OBJECT_0:
                // An exit request was posted.  We need to bail out.
                CCTRCTXI0( _T("m_hRunning event triggered.  Destroying SysTrayWindow...") );
                m_wndAppWindow.DestroyWindow();
                ResetEvent(m_hRunning);
                hrResult = S_OK;
                bRunning = TRUE;
                break;

            case WAIT_OBJECT_0 + 1:
                // A WM_QUIT message was posted.  We need to bail out.
                CCTRCTXI0( _T("WM_QUIT posted.  Exiting...") );
                if(S_FALSE == m_wndAppWindow.GetExitCode())
                    hrResult = S_FALSE;
                else
                    hrResult = S_OK;
                bRunning = FALSE;
                break;

            default:
                CCTRCTXI0( _T("Unexpected response from CMessageLock::Lock().  Exiting...") );
                hrResult = E_UNEXPECTED;
                bRunning = FALSE;
                break;
            }
        }

        m_wndAppWindow.m_pSessionAppServer = NULL;
    }    
    catch(_com_error& e)
    {
        hrResult = e.Error();
        CCTRCTXE1(_T("Exception handled: hr=0x%08X"), hrResult);
    }

    CCTRCTXI1( _T("Exiting... hrResult = 0x%08X"), hrResult);
    return hrResult;
}

