#include "StdAfx.h"

#include "SSPProcessEventBase.h"

CSSPProcessEventBase::CSSPProcessEventBase(void)
{
}

CSSPProcessEventBase::~CSSPProcessEventBase(void)
{
}

HRESULT CSSPProcessEventBase::SetEventObject(ccEvtMgr::CEventEx *pEventObj)
{
	
	m_spEventObj = pEventObj;
	
    return S_OK;
}

HRESULT CSSPProcessEventBase::SetUnknownApp(ISymBase* pUnknownApp)
{

    m_spUnknownApp = pUnknownApp;

    return S_OK;
}

