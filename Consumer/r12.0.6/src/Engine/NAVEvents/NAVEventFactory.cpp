// NAVEventFactory.cpp: implementation of the CNAVEventFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// Include all the events that this factory can handle.
//
#include "NAVEventFactory.h"
#include "AllNAVEvents.h"


using namespace ccEvtMgr;
using namespace ccLib;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const long CNAVEventFactory::m_EventTypeArray[] = 
{
    AV::Event_ID_Threat,          // Virus alert events
    AV::Event_ID_ScanAction,          // Scanner started/stopped
    AV::Event_ID_Error,                // Error occured
    AV::Event_ID_StatusRequest,         // Request for current status data from client
    AV::Event_ID_StatusALU,             // Status update event for Automatic LiveUpdate
    AV::Event_ID_StatusAP,              // Status update event for Auto-Protect
    AV::Event_ID_StatusDefs,            // Status update event for virus definitions
    AV::Event_ID_StatusFSS,             // Status update event for Full System Scan
    AV::Event_ID_StatusIWP,             // Status update event for IWP
    AV::Event_ID_StatusEmail,           // Status update event for Email
    AV::Event_ID_StatusLicensing,       // Status update event for Licensing
    AV::Event_ID_StatusIMScan,          // Status update event for IM Scanner
    AV::Event_ID_StatusSpyware          // Status update event for Spyware category
};

CNAVEventFactory::CNAVEventFactory() : CEventFactoryHelper(m_EventTypeArray, CCDIMOF(m_EventTypeArray))
{
}

CNAVEventFactory::~CNAVEventFactory()
{
}


CError::ErrorType CNAVEventFactory::CopyEvent(const CEventEx& eventSrc /*in*/, CEventEx*& peventDst /*out*/)
{
    //::MessageBox ( ::GetDesktopWindow(), "navevents", "copyevent C2C", MB_OK);
    // Create a C++ class based on the C++ class
    CExceptionInfo exceptionInfo;
    exceptionInfo.SetFunction(_T("CNAVEventFactory::CopyEvent(C,C)"));

    try
    {
        // Call the copy constructor
        peventDst = new CNAVEventCommon(dynamic_cast<const CNAVEventCommon&>(eventSrc));
        peventDst->AddRef();
    }
    CCCATCH(exceptionInfo);
    if (exceptionInfo.IsException())
    {
        return CError::eFatalError;
    }
    return CError::eNoError;
}

CError::ErrorType CNAVEventFactory::CopyEvent(IEventEx* pieventSrc /*in*/, CEventEx*& peventDst /*out*/)
{
    //::MessageBox ( ::GetDesktopWindow(), "navevents", "copyevent I2C", MB_OK);
    peventDst = NULL;

    // Create a C++ class based on the COM class
    CExceptionInfo exceptionInfo;
    exceptionInfo.SetFunction(_T("CNAVEventFactory::CopyEvent(I,C)"));

    try
    {
        // Get type
        long nType;
        HRESULT hRes = pieventSrc->GetType(&nType);
        if (FAILED(hRes))
        {
            CCTRACEE(_T("CNAVEventFactory::Copy() : pieventSrc->GetType(&nType) != S_OK\n"));
            _com_issue_error(hRes);
        }

        INAVEventCommonIntPtr piEvent(pieventSrc);

        if (piEvent == NULL)
        {
            _com_issue_error(E_NOINTERFACE);
        }
        
        // Allocate the new class
        CNAVEventCommon* pEvent = new CNAVEventCommon;
        pEvent->AddRef();

        // Copy IEvent properties
        hRes = CSerializableEventHelper::Copy(piEvent, *pEvent);
        if (FAILED(hRes))
        {
            _com_issue_error(hRes);
        }
        
        peventDst = pEvent;
        
        if (FAILED(hRes))
        {
            _com_issue_error(hRes);
        }
    }
    CCCATCH(exceptionInfo);
    if (exceptionInfo.IsException())
    {
        return CError::eFatalError;
    }
    return CError::eNoError;
}

CError::ErrorType CNAVEventFactory::CopyEvent(const CEventEx& eventSrc /*in*/, IEventEx*& pieventDst /*out*/)
{
    //::MessageBox ( ::GetDesktopWindow(), "navevents", "copyevent C2I", MB_OK);
    pieventDst = NULL;

    // Create a COM class based on the C++ class
    CExceptionInfo exceptionInfo;
    exceptionInfo.SetFunction(_T("CNAVEventFactory::CopyEvent(C,I)"));

    try
    {
        // Allocate the new class
		CComObject<CNAVEventCommonInt>* piEvent = ccLib::CInstanceHelper::CreateInstance<CNAVEventCommonInt>();

        // Use the = operator
        piEvent->m_C = dynamic_cast<const CNAVEventCommon&>(eventSrc);

        // Get the interface
		ccLib::CPropertyHelper::GetInterfaceEx(piEvent, pieventDst);
    }
    CCCATCH(exceptionInfo);
    if (exceptionInfo.IsException())
    {
        return CError::eFatalError;
    }
    return CError::eNoError;
}

CError::ErrorType CNAVEventFactory::DeleteEvent(CEventEx* pEvent)
{
    if ( pEvent )
    {
        pEvent->Release();
        pEvent = NULL;
        return CError::eNoError;
    }

    return CError::eBadParameterError;
}

CError::ErrorType CNAVEventFactory::DeleteEvent(IEventEx* pEvent)
{
    if ( pEvent )
    {
        pEvent->Release();
        pEvent = NULL;
        return CError::eNoError;
    }
    return CError::eBadParameterError;
}

ccEvtMgr::CError::ErrorType CNAVEventFactory::NewEvent(long nEventType,
                                                       ccEvtMgr::CEventEx*& pEvent)
{
    CExceptionInfo exceptionInfo;
    exceptionInfo.SetFunction(_T("CNAVEventFactory::NewEvent()"));

    try
    {
        // Call the constructor
        pEvent = new CNAVEventCommon ();
        pEvent->AddRef();
       }
    CCCATCHMEM(exceptionInfo);
    if (exceptionInfo.IsException())
    {
        return ccEvtMgr::CError::eFatalError;
    }
    return ccEvtMgr::CError::eNoError;
}

