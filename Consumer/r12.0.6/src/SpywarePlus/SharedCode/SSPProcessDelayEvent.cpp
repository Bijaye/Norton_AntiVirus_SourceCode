#include "StdAfx.h"

#include "SSPRunnableJob.h"

#include "ccSerializableEventHelper.h"
#include "ccSubscriberHelper.h"

#include "HPPEventsInterface.h"
#include "HPPAppInterface.h"

#include "SSPProcessDelayEvent.h"

CSSPProcessDelayEvent::CSSPProcessDelayEvent(void)
{
    m_nMilliseconds = 0;
}

CSSPProcessDelayEvent::~CSSPProcessDelayEvent(void)
{
}

void CSSPProcessDelayEvent::SetDelayValue(INT nMilliseconds)
{
    m_nMilliseconds = nMilliseconds;
}

HRESULT CSSPProcessDelayEvent::Run()
{
    HRESULT hrReturn = S_OK;

    //
    //  Protect ourselves from the evil that
    //   we are capable of.
    //
    try
    {
        Sleep(m_nMilliseconds);
        IHPPSessionAppInterfaceQIPtr spApp = m_spUnknownApp;
        if(!spApp)
        {
            CCTRACEE(_T("CSSPProcessIEStartupEvent::Run - unable to QI m_spUnknownApp for IHPPSessionAppInterface.\n"));
            return S_OK;
        }
        
        spApp->PushIncomingEventToQueue(m_spEventObj);
    }
    catch(...)
    {
        CCTRACEE("CSSPChangeRegKey::Run() - Unhandled exception caught.\r\n");
    }

    return hrReturn;
}