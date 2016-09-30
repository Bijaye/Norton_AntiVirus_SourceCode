////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "logviewereventfactory.h"

using namespace cc;

CLogViewerEventFactory::CLogViewerEventFactory(void)
{ 
}

CLogViewerEventFactory::~CLogViewerEventFactory(void)
{
    // Make sure we destroy the helper
    if(NULL != m_LogViewerHelper.m_p)
        m_LogViewerHelper->Destroy();
}

HRESULT CLogViewerEventFactory::Create(ccEvtMgr::CEventEx* pEvent, cc::ILogViewerEvent*& pILogViewerEvent)
{
    pILogViewerEvent = NULL;

    
    m_LogViewerHelper = new CLogViewerSymNetiCategoryHelper;
    if(!m_LogViewerHelper)
    {
        CCTRACEE(_T(__FUNCTION__) _T(" - Failed to allocate helper"));
        return E_OUTOFMEMORY;
    }
    // Create the category helper
    if (m_LogViewerHelper->Create() == FALSE)
    {
        CCTRACEE(_T("CLogViewerEventFactory::Create() : m_LogViewerHelper.Create() == FALSE\n"));
        return E_FAIL;
    }

    // Wrap the event
    if (m_LogViewerHelper->WrapEvent(pEvent,
                                    pILogViewerEvent) == FALSE)
    {
        CCTRACEE(_T("CLogViewerEventFactory::Create() : m_LogViewerHelper.WrapEvent() == FALSE\n"));
        m_LogViewerHelper->Destroy();
		return E_FAIL;
    }
    return S_OK;

}

HRESULT CLogViewerEventFactory::Destroy(ILogViewerEvent* pILogViewerEvent)
{
    pILogViewerEvent->Release();
    pILogViewerEvent = NULL;
    
    if(NULL != m_LogViewerHelper.m_p)
       m_LogViewerHelper->Destroy();

    return S_OK;
}