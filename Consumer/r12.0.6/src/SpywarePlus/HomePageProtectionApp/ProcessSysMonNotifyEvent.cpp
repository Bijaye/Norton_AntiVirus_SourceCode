#include "StdAfx.h"

#include "SSPRunnableJob.h"

#include "ccSerializableEventHelper.h"
#include "ccSubscriberHelper.h"

#include "HPPEventsInterface.h"
#include "HPPAppInterface.h"

#include "BBSystemMonitorEventsInterface.h"

#include "ProcessSysMonNotifyEvent.h"

extern const GUID CLSID_HPPSysMonClient;

CProcessSysMonNotifyEvent::CProcessSysMonNotifyEvent(void)
{
}

CProcessSysMonNotifyEvent::~CProcessSysMonNotifyEvent(void)
{
}

HRESULT CProcessSysMonNotifyEvent::Run()
{
    HRESULT hrReturn = S_OK;
    CCTRACEI(_T("CProcessSysMonNotifyEvent::Run() BEGIN.\r\n"));

    //
    //  Protect ourselves from the evil that
    //   we are capable of.
    //
    try
    {

    }
    catch(...)
    {
        CCTRACEE("CProcessSysMonNotifyEvent::Run() - Unhandled exception caught.\r\n");
    }

    CCTRACEI(_T("CProcessSysMonNotifyEvent::Run() END.\r\n"));

    return hrReturn;
}
