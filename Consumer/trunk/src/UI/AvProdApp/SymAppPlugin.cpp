////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "SymAppPlugin.h"

#include "ccCoInitialize.h"
#include "ccHandle.h"

#include <hrx.h>

#include "AppManager.h"

#include <SessionAppClientImpl.h>

using namespace AvProdApp;

CSymAppPlugin::CSymAppPlugin(void)
{
}

CSymAppPlugin::~CSymAppPlugin(void)
{
}

//****************************************************************************
//****************************************************************************
void CSymAppPlugin::Run( ICCApp* pCCApp )
{
    HANDLE hccEvtMgrReconnectEvent = NULL;
    HRESULT hrResult = E_FAIL;

    // Need to init COM on this thread
    ccLib::CCoInitialize coInit;
    
    StahlSoft::HRX hrx;
    try
    {
		// Check to see if another copy of AvProdApp is running in this session.
		ccLib::CHandle hMutex;

		if(false == hMutex.Attach(CreateMutex(NULL, TRUE, _T("AvProdAppMutex"))))
		{
			CCTRCTXE0(_T("Failed to attache to mutex"));
			hrx << E_UNEXPECTED;
		}

		if(hMutex == NULL)
		{
			CCTRCTXE1(_T("Failed to open instance mutex: %d"), GetLastError());
			hrx << E_FAIL;
		}

		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			CCTRCTXE0(_T("Already loaded"));
			hrx << HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
		}

        // CoInit this thread
        hrx << coInit.Initialize( ccLib::CCoInitialize::eMTAModel );
        
        // Create event to wait on.
        if( FALSE == m_endSessionEvent.Create( NULL, TRUE, FALSE, NULL, FALSE ))
        {
            DWORD dwErr = GetLastError();
            CCTRCTXE1(_T("Failed to create EndSession event object. ccLib::CEvent::Create() returned: 0x%08X"), dwErr);
            throw _com_error(HRESULT_FROM_WIN32(dwErr));
        }
        
        // Create event to wait on.
        ccLib::CEvent m_shAppManagerExitRequestEvent;
        if( FALSE == m_shAppManagerExitRequestEvent.Create( NULL, TRUE, FALSE, NULL, FALSE ))
        {
            DWORD dwErr = GetLastError();
            CCTRCTXE1(_T("Failed to create AppManagerExitRequest event object. ccLib::CEvent::Create() returned: 0x%08X"), dwErr);
            throw _com_error(HRESULT_FROM_WIN32(dwErr));
        }

        // Insure there is a AppManager
        hrx << CAppManagerSingleton::Initialize();

        // Initialize the AppManager
        CAppManagerPtr spAppManager = CAppManagerSingleton::GetAppManager();
        hrx << spAppManager->Initialize();
        spAppManager->SetShutdownRequest(m_shAppManagerExitRequestEvent);

        // Wait up here and process messages.
        ccLib::CMessageLock lock( TRUE, TRUE );

        // Wait indefinitely
        DWORD dwRet;
        BOOL bRunning = TRUE;
        HANDLE hWaitHandles[] = {m_endSessionEvent.GetHandle(), m_shAppManagerExitRequestEvent.GetHandle()};    
        while(bRunning)
        {
            dwRet = lock.Lock( CCDIMOF(hWaitHandles), hWaitHandles, FALSE, INFINITE, FALSE );
            switch( dwRet )
            {
            case WAIT_OBJECT_0:
                // A WM_ENDSESSION message was posted.  We need to bail out.
                CCTRCTXI0( _T("m_endSessionEvent event triggered.  Exiting.") );
                bRunning = FALSE;
                break;

            case WAIT_OBJECT_0 + 1:
                // App Manager requested an exit
                CCTRCTXI0( _T("AppManagerExitRequest event triggered.  Exiting.") );
                bRunning = FALSE;
                break;

            case WAIT_OBJECT_0 + CCDIMOF(hWaitHandles):
                // A WM_QUIT message was posted.  We need to bail out.
                CCTRCTXI0( _T("CSymAppPlugin::Run - WM_QUIT posted.  Exiting.") );
                bRunning = FALSE;
                break;

            default:
                CCTRCTXI0( _T("Unexpected response from CMessageLock.Lock().") );
                bRunning = FALSE;
                break;
            }
        }
        spAppManager->Destroy();
        CAppManagerSingleton::Destroy();
    }    
    catch(_com_error& e)
    {
        hrResult = e.Error();
        CCTRCTXE1(_T("Exception handled: hr=0x%08X"), hrResult);
    }

    // Shouldn't need this anymore
    m_endSessionEvent.Destroy();
    
    coInit.CoUninitialize();
    
    CCTRCTXI0( _T("Exiting...") );
    return;
}

//****************************************************************************
//****************************************************************************
bool CSymAppPlugin::CanStopNow()
{

    return TRUE;
}

//****************************************************************************
//****************************************************************************
void CSymAppPlugin::RequestStop()
{
    m_endSessionEvent.SetEvent();

    return;
}

//****************************************************************************
//****************************************************************************
const char * CSymAppPlugin::GetObjectName()
{
    return " SymApp Plugin";
}

//****************************************************************************
//****************************************************************************
unsigned long CSymAppPlugin::OnMessage( int iMessage, unsigned long ulParam )
{
    return 0;
}
