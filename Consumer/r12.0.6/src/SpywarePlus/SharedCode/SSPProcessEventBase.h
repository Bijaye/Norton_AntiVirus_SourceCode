#pragma once

#include "SSPRunnableJob.h"
#include "ccSerializableEventHelper.h"

class CSSPProcessEventBase : public CSSPRunnableJob
{
public:
	CSSPProcessEventBase(void);
	virtual ~CSSPProcessEventBase(void);

	STDMETHOD(Run)() = 0;

    HRESULT SetEventObject(ccEvtMgr::CEventEx *pEventObj);
    HRESULT SetUnknownApp(ISymBase* pUnknownApp);

protected:
	ccEvtMgr::CEventExPtr m_spEventObj;
    CSymQIPtr<ISymBase, &IID_SymBase> m_spUnknownApp;

    HWND m_hwndCommands;

};
