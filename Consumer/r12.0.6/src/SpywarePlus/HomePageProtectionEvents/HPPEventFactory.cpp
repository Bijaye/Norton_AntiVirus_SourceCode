#include "StdAfx.h"
#include "resource.h"
#include "HPPEventFactory.h"
#include "HPPEvents_h.h"

#include "HPPEventCommonImpl.h"

using namespace ccLib;
using namespace ccEvtMgr;

DECLARE_CEVENTEXINT_CLASS(HPPEventCommonImpl, LIBID_HPPEventsLib, IDR_HPPEVENTCOMMON)
                            
const long CHPPEventFactory::m_EventTypeArray[] = 
{
    HPP::Event_ID_HPPNotifyHomePage,
    HPP::Event_ID_IE_Started,
    HPP::Event_ID_LogEntry_HomePageActivity,
    HPP::Event_ID_LogEntry_AppActivity,
    HPP::Event_ID_HPPChangeHomePage,
	HPP::Event_ID_OptionsChanged
};

CHPPEventFactory::CHPPEventFactory(void) :
	CEventFactoryHelper(m_EventTypeArray, CCDIMOF(m_EventTypeArray)) 
{
}

CHPPEventFactory::~CHPPEventFactory(void)
{
}

DELETE_EVENT(CHPPEventFactory)

CError::ErrorType CHPPEventFactory::CopyEvent(const CEventEx& eventSrc /*in*/, CEventEx*& peventDst /*out*/)
{
    // Create a C++ class based on the C++ class
    CExceptionInfo exceptionInfo;

    try
    {
        // Call the copy constructor
        peventDst = new CHPPEventCommonImpl(dynamic_cast<const CHPPEventCommonImpl&>(eventSrc));
        peventDst->AddRef();
    }
    CCCATCH(exceptionInfo);
    if (exceptionInfo.IsException() != false)
    {
        return CError::eFatalError;
    }
    return CError::eNoError;
}

CError::ErrorType CHPPEventFactory::CopyEvent(IEventEx* pieventSrc /*in*/, CEventEx*& peventDst /*out*/)
{
    // Create a C++ class based on the COM class
    peventDst = NULL;

    CExceptionInfo exceptionInfo;
    try
    {
        // Get type
        long nType;
        HRESULT hRes = pieventSrc->GetType(&nType);
        if (FAILED(hRes))
        {
            CCTRACEE(_T("CHPPEventFactory::Copy() : pieventSrc->GetType(&nType) != S_OK\n"));
            _com_issue_error(hRes);
        }
         
        IHPPEventCommonImplPtr piEvent(pieventSrc);

        if (piEvent == NULL)
        {
            _com_issue_error(E_NOINTERFACE);
        }

        // Allocate the new class
        CHPPEventCommonImpl* pEvent = new CHPPEventCommonImpl;
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
    if (exceptionInfo.IsException() != false)
    {
        return CError::eFatalError;
    }
    return CError::eNoError;
}

CError::ErrorType CHPPEventFactory::CopyEvent(const CEventEx& eventSrc /*in*/, IEventEx*& pieventDst /*out*/)
{
    //::MessageBox ( ::GetDesktopWindow(), "navevents", "copyevent C2I", MB_OK);
    pieventDst = NULL;

    // Create a COM class based on the C++ class
    CExceptionInfo exceptionInfo;
    try
    {
        // Allocate the new class
        CComObject<CHPPEventCommonImplInt>* piEvent = CInstanceHelper::CreateInstance<CHPPEventCommonImplInt>();

        // Use the = operator
        piEvent->m_C = dynamic_cast<const CHPPEventCommonImpl&>(eventSrc);

        // Get the interface
        CPropertyHelper::GetInterfaceEx(piEvent, pieventDst);
    }
    CCCATCH(exceptionInfo);
    if (exceptionInfo.IsException() != false)
    {
        return CError::eFatalError;
    }
    return CError::eNoError;
}

ccEvtMgr::CError::ErrorType CHPPEventFactory::NewEvent(long nEventType,
                                                       ccEvtMgr::CEventEx*& pEvent)
{
#pragma message(AUTO_FUNCNAME "TODO: Code Review Item")
	// 
	// TODO: Code Review Item
	//   pEvent and spCommon should be tested for NULL before being used.
	//   if == NULL throw COM error change catcher to CCCATCH instead of CCCATCHMEM.
	//


	
	CExceptionInfo exceptionInfo;
    try
    {
        // Call the constructor
        pEvent = new CHPPEventCommonImpl();
		if(pEvent)
			pEvent->AddRef();

        CHPPEventCommonImplQIPtr spCommon = pEvent;
		if(spCommon)
			spCommon->SetType(nEventType);
        
    }
    CCCATCH(exceptionInfo);
    if (exceptionInfo.IsException() != false)
    {
        return ccEvtMgr::CError::eFatalError;
    }
    return ccEvtMgr::CError::eNoError;
}
