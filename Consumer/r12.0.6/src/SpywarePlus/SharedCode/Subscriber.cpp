#include "StdAfx.h"

#include <process.h>   
#include "ccCoInitialize.h"
#include "ccCatch.h"
#include "ccSyncLock.h"
#include "ccEventId.h"
#include "ccTrace.h"

#include "SSPWorkerThread.h"

#include "ccSerializableEventHelper.h"
#include "HPPAppInterface.h"
#include "HPPEventsInterface.h"
#include "HPPEventHelperLoader.h"

#include "BBEventID.h"
#include "SymProtectEventsLoader.h"

#include "Subscriber.h"

using namespace cc;
using namespace StahlSoft;
using namespace ccEvtMgr;
using namespace ccLib;

// Event subscription data...
CSubscriberHelper::SubscriberInfo _SubscribedEvents[] =
{
//  {Event ID, Priority, Event States, Event States count, ProviderID, Monitor only}           
	{HPP::Event_ID_HPPNotifyHomePage, 0,  NULL, 0, 0, false},
    {HPP::Event_ID_IE_Started, 0,  NULL, 0, 0, false},
    {HPP::Event_ID_HPPChangeHomePage, 0,  NULL, 0, 0, false},
	{HPP::Event_ID_OptionsChanged, 0, NULL, 0, 0, false},
    {CC_SM_NOTIFICATION_EVENT, 0,  NULL, 0, 0, false},
};

CSubscriber::CSubscriber(void)
{

}

CSubscriber::~CSubscriber(void)
{
}

bool CSubscriber::Initialize(ISymBase *pUnknownApp)
{
    // Load the path to the event factory - SPPEvents.dll
    char* EVENT_FACTORY_ARRAY[2];
    
    ATL::CString cszHomePageEventsDll, cszSysMonEventsDll;

#pragma message(AUTO_FUNCNAME "TODO: fix up PathProvider to pick up correct dir for NAV")
    size_t iSize = MAX_PATH;
    ccSym::CModulePathProvider::GetPath(cszHomePageEventsDll.GetBuffer(iSize), iSize);
    cszHomePageEventsDll.ReleaseBuffer();

    DWORD dwSize = iSize + _tcslen(HPP::sz_HPPEventHelper_dll) + 1;
    PathAppend(cszHomePageEventsDll.GetBuffer(dwSize), HPP::sz_HPPEventHelper_dll);

    iSize = MAX_PATH;
    ccSym::CBBPathProvider::GetPath(cszSysMonEventsDll.GetBuffer(iSize), iSize);
    cszSysMonEventsDll.ReleaseBuffer();

    dwSize = iSize + _tcslen(SymProtectEvt::sz_SymProtectEvents_dll) + 1;
    PathAppend(cszSysMonEventsDll.GetBuffer(dwSize), SymProtectEvt::sz_SymProtectEvents_dll);

    EVENT_FACTORY_ARRAY[0] = (LPTSTR)(LPCTSTR)cszHomePageEventsDll;
    EVENT_FACTORY_ARRAY[1] = (LPTSTR)(LPCTSTR)cszSysMonEventsDll;


    // Connect to the Event Manager
	BOOL bAllSucceeded = TRUE;

	for(;;)
	{
        // Load the Event factories
        if(m_EventFactories.Create(EVENT_FACTORY_ARRAY, CCDIMOF(EVENT_FACTORY_ARRAY)) == FALSE)
	    {
		    bAllSucceeded = FALSE;
		    break;
	    }

        // Create a ProxyFactory       
        if(SYM_FAILED(m_CProxyFactoryCreatorEx.CreateObject(&m_pCProxyFactoryEx)))
	    {
		    bAllSucceeded = FALSE;
		    break;
	    }
  
		// Create an instance of the event manager with this factory
		if(m_EventManagerHelper.Create(&m_EventFactories, m_pCProxyFactoryEx) == FALSE)
		{
			bAllSucceeded = FALSE;
			break;
		}

		// Register all the events for this factory
        if(CSubscriberHelper::Create(&m_EventManagerHelper, _SubscribedEvents, CCDIMOF(_SubscribedEvents)) == FALSE)
        {
            bAllSucceeded = FALSE;
            break;
        }

		break;
	}

	if(!bAllSucceeded)
	{
		Destroy();
        return false;
	}
    
    m_spUnknownApp = pUnknownApp;

    return true;
}

void CSubscriber::Destroy()
{
    // Destroy all our objects
    CSubscriberHelper::Destroy();
    m_EventManagerHelper.Destroy();
    
    if(m_pCProxyFactoryEx)
        m_pCProxyFactoryEx.Release();

	m_EventFactories.Destroy();
}

CError::ErrorType CSubscriber::OnEvent(long nSubscriberId, 
                                      const CEventEx& Event,
                                      EventAction& eAction)
{
    PushAlertToQueue(&Event);
    return CError::eNoError;
}

CError::ErrorType CSubscriber::OnShutdown()
{
    // The Event manager is going away, lets remove all our connections
    CCTRACEI(_T("CSubscriber::OnShutdown - ccEvtMgr shutting down."));
    
    IHPPSessionAppInterfaceQIPtr spApp = m_spUnknownApp;
    if(spApp)
    {
        spApp->HandleEventManagerShutdown();
    }

    return CError::eNoError;
}


bool CSubscriber::PushAlertToQueue(const CEventEx* pEvent)
{
	CEventExPtr spEventCopy = NULL;
    if(m_EventFactories.CopyEvent(*pEvent, spEventCopy) != CError::eNoError)
		return false;

	// Push the copy to the queue
    CExceptionInfo exceptionInfo;
    try
    {
        IHPPSessionAppInterfaceQIPtr spApp = m_spUnknownApp;
        if(spApp)
        {
            spApp->PushIncomingEventToQueue(spEventCopy);
        }
        else
        {
            if(!pEvent->GetBroadcast())
            {
                CommitEvent(*(spEventCopy.m_p));
            }
        }

    }
    CCCATCHMEM(exceptionInfo);
    if(exceptionInfo.IsException() != false)
    {
        return false;
    }

	return true;
}
