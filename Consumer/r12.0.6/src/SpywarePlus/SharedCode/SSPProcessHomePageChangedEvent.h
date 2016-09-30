#pragma once

#include "SSPProcessEventBase.h"
#include "ccAlertInterface.h"

class CSSPProcessHomePageChangedEvent :
    public CSSPProcessEventBase,
    public cc::IAlertCallback,
    ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    CSSPProcessHomePageChangedEvent(void);
    virtual ~CSSPProcessHomePageChangedEvent(void);
    
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(cc::IID_AlertCallback, IAlertCallback)
    SYM_INTERFACE_MAP_END()

    STDMETHOD(Run)();

    virtual HRESULT AddAlertDetail(CHPPEventCommonInterface *pEvent, cc::IAlert2 *pAlert);
    virtual HRESULT AddAlertCommon(CHPPEventCommonInterface *pEvent, cc::IAlert2 *pAlert);
 
    // ICcAlertCallback methods
    virtual bool Run(HWND hWndParent, unsigned long nData, cc::IAlert* pAlert, cc::IAlertCallback::ALERTCALLBACK context);

};
