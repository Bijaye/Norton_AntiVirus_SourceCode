#include "StdAfx.h"

#include "SSPRunnableJob.h"

#include "ccSerializableEventHelper.h"
#include "ccSubscriberHelper.h"

#include "HPPEventsInterface.h"
#include "HPPAppInterface.h"

#include "SSPProcessEventBase.h"
#include "SSPGenericProcessEvent.h"
#include "SSPProcessHomePageChangedEvent.h"


CSSPGenericProcessEvent::CSSPGenericProcessEvent(void)
{
}

CSSPGenericProcessEvent::~CSSPGenericProcessEvent(void)
{
}

HRESULT CSSPGenericProcessEvent::Run()
{
	HRESULT hrReturn = S_OK;
    CCTRACEI(_T("CSSPGenericProcessEvent::Run() BEGIN.\r\n"));

	//
	//  Protect ourselves from the evil that
	//   we are capable of.
	//
	try
	{
        IHPPSessionAppInterfaceQIPtr spApp = m_spUnknownApp;
        if(spApp)
        {
            spApp->ProcessEvent(m_spEventObj);
        }

	}
	catch(...)
	{
		CCTRACEE("CSSPGenericProcessEvent::Run() - Unhandled exception caught.\r\n");
	}

    CCTRACEI(_T("CSSPGenericProcessEvent::Run() END.\r\n"));

	return hrReturn;
}

