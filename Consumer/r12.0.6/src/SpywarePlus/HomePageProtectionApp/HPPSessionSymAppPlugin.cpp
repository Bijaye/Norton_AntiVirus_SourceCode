#include "StdAfx.h"

#include "ccCoInitialize.h"

#include "ccAppPlugin.h"

#include "SSPWorkerThread.h"
#include "Subscriber.h"

#include "HPPSessionSymAppPlugin.h"

#include "HPPAppWnd.h"
#include "HPPSessionApp.h"

#define CCTRACE_ENTER_EXIT(funcname)  \
    CCTRACE_ENTER_EXIT_IMPL ccTraceEntryExitImpl(funcname); \

class CCTRACE_ENTER_EXIT_IMPL
{
public:
    CCTRACE_ENTER_EXIT_IMPL();
    CCTRACE_ENTER_EXIT_IMPL(LPCTSTR szFuncName)
    {
        m_cszFuncName = szFuncName;
        CCTRACEI("%s - Enter", m_cszFuncName);
    }

    virtual ~CCTRACE_ENTER_EXIT_IMPL(void)
    {
        CCTRACEI("%s - Exit", m_cszFuncName);
    }

protected:
    CString m_cszFuncName;
};

CHPPSessionSymAppPlugin::CHPPSessionSymAppPlugin(void)
{
}

CHPPSessionSymAppPlugin::~CHPPSessionSymAppPlugin(void)
{
}

//****************************************************************************
//****************************************************************************
void CHPPSessionSymAppPlugin::Run( ICCApp* pCCApp )
{
    CCTRACE_ENTER_EXIT(_T("CHPPSessionSymAppPlugin::Run"));
    
    HRESULT hr;
    HANDLE hccEvtMgrReconnectEvent = NULL;

    // Need to init COM on this thread
    ccLib::CCoInitialize coInit;
    if( FAILED( coInit.Initialize( ccLib::CCoInitialize::eMTAModel ) ) )
    {
        CCTRACEE(_T("CHPPSessionSymAppPlugin::Run - Could not init COM."));
        return;
    }
    
    // Create event to wait on.
    if( m_endSessionEvent.Create( NULL, TRUE, FALSE, NULL, FALSE ) == FALSE )
    {
        CCTRACEE( _T("CHPPSessionSymAppPlugin::Run - Failed to create EndSession event object.") );
        return;
    }
    
    IHPPSessionAppInterfacePtr spSessionApp = new CHPPSessionApp;
    if(!spSessionApp)
    {
        CCTRACEE( _T("CHPPSessionSymAppPlugin::Run - Unable to create session app.  Bailing out.") );
        return;
    }
    
    hr = spSessionApp->Initialize();
    if(FAILED(hr))
    {
        CCTRACEE( _T("CHPPSessionSymAppPlugin::Run - Unable to initialize session app.  Bailing out.") );
        return;
    }
    
    // Wait here up and process messages.
    ccLib::CMessageLock lock( TRUE, TRUE );

    // Wait indefinitely
    DWORD dwRet;
    BOOL bRunning = TRUE;
    HANDLE hWaitHandles[] = {m_endSessionEvent.GetHandle()};    
    while(bRunning)
    {
        dwRet = lock.Lock( CCDIMOF(hWaitHandles), hWaitHandles, FALSE, INFINITE, FALSE );
        switch( dwRet )
        {
        case WAIT_OBJECT_0:
            // A WM_ENDSESSION message was posted.  We need to bail out.
            CCTRACEI( _T("CHPPSessionSymAppPlugin::Run - m_endSessionEvent event triggered.  Exiting.") );
            bRunning = FALSE;
            break;

        case WAIT_OBJECT_0 + 1:
            // A WM_QUIT message was posted.  We need to bail out.
            CCTRACEI( _T("CHPPSessionSymAppPlugin::Run - WM_QUIT posted.  Exiting.") );
            bRunning = FALSE;
            break;

        default:
            CCTRACEE( _T("Unexpected response from CMessageLock.Lock().") );
            bRunning = FALSE;
            break;
        }
    }

    
    spSessionApp->Destroy();

    // Shouldn't need this anymore
    m_endSessionEvent.Destroy();
    
    coInit.CoUninitialize();

    return;
}

//****************************************************************************
//****************************************************************************
bool CHPPSessionSymAppPlugin::CanStopNow()
{
    CCTRACE_ENTER_EXIT(_T("CHPPSessionSymAppPlugin::CanStopNow"));



    return TRUE;
}

//****************************************************************************
//****************************************************************************
void CHPPSessionSymAppPlugin::RequestStop()
{
    CCTRACE_ENTER_EXIT(_T("CHPPSessionSymAppPlugin::RequestStop"));

    m_endSessionEvent.SetEvent();

    return;
}

//****************************************************************************
//****************************************************************************
const char * CHPPSessionSymAppPlugin::GetObjectName()
{
    CCTRACE_ENTER_EXIT(_T("CHPPSessionSymAppPlugin::GetObjectName"));

    return _T("Home Page Protection Session SymApp Plugin");
}

//****************************************************************************
//****************************************************************************
unsigned long CHPPSessionSymAppPlugin::OnMessage( int iMessage, unsigned long ulParam )
{
    CCTRACE_ENTER_EXIT(_T("CHPPSessionSymAppPlugin::OnMessage"));

    return 0;
}
