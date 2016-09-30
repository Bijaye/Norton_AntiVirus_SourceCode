//****************************************************************************
//****************************************************************************
#include "stdafx.h"
#include "HPPAppCommands.h"

#include "shellapi.h"
#include "Winuser.h"
#include "HPPAppWnd.h"

#include "ccEventManagerHelper.h"
#include "ccAlertInterface.h"
#include "ccAlertLoader.h"

#include "HPPSessionApp.h"

#include "resource.h"

using namespace cc;
using namespace HPPAPPWND;

#ifndef _DEBUG
#define OutputDebugString AtlTraceNull
#endif // _DEBUG

namespace HPPAPPWND
{
    const unsigned int RM_TRAY                       = ::RegisterWindowMessage(_T("{15918834-B104-4c50-AFCE-9C73E8E6D8EB}"));
    const unsigned int RM_TASKBARCREATED             = ::RegisterWindowMessage(_T("_TaskbarCreated_"));
    const unsigned int RM_SHOWTRAYICON               = ::RegisterWindowMessage(_T("_ShowTrayIcon_"));

    const unsigned int RM_CCEVTISDOWN                = ::RegisterWindowMessage(_T("_RM_CCEVTISDOWN_"));
}

//****************************************************************************
//  c'tor
//****************************************************************************
CHPPAppWnd::CHPPAppWnd() :
    m_ccEvtMgrReconnectEvent(NULL)
{
    
}

//****************************************************************************
//  d'tor
//****************************************************************************

CHPPAppWnd::~CHPPAppWnd()
{

}

//****************************************************************************
//****************************************************************************
BOOL CHPPAppWnd::PreTranslateMessage(MSG* pMsg)
{
    return FALSE;//IsDialogMessage(pMsg);
}

//****************************************************************************
//****************************************************************************
BOOL CHPPAppWnd::OnIdle()
{
    return FALSE;
}

//****************************************************************************
//****************************************************************************
LRESULT CHPPAppWnd::OnCreate( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{

    return 0;
}

//****************************************************************************
//****************************************************************************
LRESULT CHPPAppWnd::OnCCEvtIsDown( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
    CCTRACEI(_T("CHPPAppWnd::OnCCEvtIsDown - starting monitor for ccEventManager status."));
    SetTimer(TIMERID_CCEVTCHK, 5000);

    return 0;
}

//****************************************************************************
//****************************************************************************
LRESULT CHPPAppWnd::OnDestroy( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
       
    PostQuitMessage(NULL);
    return 0;
}

//****************************************************************************
//****************************************************************************
LRESULT CHPPAppWnd::OnTimer( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
    switch(wParam)
    {
        case TIMERID_CCEVTCHK:
            OnTimerCheckEventManagerStatus();
            break;
    }

    return 0;
}

//****************************************************************************
//****************************************************************************
void CHPPAppWnd::OnTimerCheckEventManagerStatus()
{
    CCTRACEI(_T("CHPPAppWnd::OnTimerCheckEventManagerStatus - enter."));

    if(ccEvtMgr::CEventManagerHelper::IsEventManagerActive())
    {
        CCTRACEI(_T("CHPPAppWnd::OnTimerCheckEventManagerStatus - ccEvtMgr appears to be up."));
        
        KillTimer(TIMERID_CCEVTCHK);
        
        IHPPSessionAppInterfaceQIPtr spApp = m_spUnknownApp;
        if(spApp)
        {
            spApp->ResetEventManagerConnections();
        }
    }

    return;
}



//****************************************************************************
//****************************************************************************
void CHPPAppWnd::OnFinalMessage( HWND hWnd )
{

}

//****************************************************************************
//****************************************************************************
LRESULT CHPPAppWnd::OnQueryEndSession( UINT nMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled )
{
    PostMessage(WM_CLOSE);
    bHandled = FALSE;
    return TRUE;
}
