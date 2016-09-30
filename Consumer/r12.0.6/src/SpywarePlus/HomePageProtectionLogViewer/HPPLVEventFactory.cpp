#include "StdAfx.h"
#include "HPPLVEventFactory.h"

CHPPLVEventFactory::CHPPLVEventFactory(void)
{
}

CHPPLVEventFactory::~CHPPLVEventFactory(void)
{
}

HRESULT CHPPLVEventFactory::Create(ccEvtMgr::CEventEx* pEvent, cc::ILogViewerEvent*& pILogViewerEvent)
{
    return E_NOTIMPL;
}

HRESULT CHPPLVEventFactory::Destroy(cc::ILogViewerEvent* pILogViewerEvent)
{
    return E_NOTIMPL;
}

