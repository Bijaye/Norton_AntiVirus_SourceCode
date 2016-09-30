#include "logviewereventfactory.h"

using namespace cc;

CLogViewerEventFactory::CLogViewerEventFactory(void)
{ 
}

CLogViewerEventFactory::~CLogViewerEventFactory(void)
{
    // Make sure we destroy the helper
    m_LogViewerHelper.Destroy();
}

HRESULT CLogViewerEventFactory::Create(ccEvtMgr::CEventEx* pEvent, cc::ILogViewerEvent*& pILogViewerEvent)
{
    pILogViewerEvent = NULL;

    // Create the category helper
    if (m_LogViewerHelper.Create() == FALSE)
    {
        CCTRACEE(_T("CLogViewerEventFactory::Create() : m_LogViewerHelper.Create() == FALSE\n"));
        return E_FAIL;
    }

    // Wrap the event
    if (m_LogViewerHelper.WrapEvent(pEvent,
                                    pILogViewerEvent) == FALSE)
    {
        CCTRACEE(_T("CLogViewerEventFactory::Create() : m_LogViewerHelper.WrapEvent() == FALSE\n"));
        m_LogViewerHelper.Destroy();
		return E_FAIL;
    }
    return S_OK;

}

HRESULT CLogViewerEventFactory::Destroy(ILogViewerEvent* pILogViewerEvent)
{
    pILogViewerEvent->Release();
    pILogViewerEvent = NULL;
    
    m_LogViewerHelper.Destroy();

    return S_OK;
}