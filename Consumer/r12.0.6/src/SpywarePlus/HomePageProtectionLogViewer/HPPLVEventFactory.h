#pragma once

#include "ccLogViewerInterface.h"
#include "ccLogViewerEventFactory.h"
#include "HPPLVInterface.h"

class CHPPLVEventFactory : public cc::ILogViewerEventFactory,
    public ISymBaseImpl<CSymThreadSafeRefCount>

{
public:
    CHPPLVEventFactory(void);
    virtual ~CHPPLVEventFactory(void);

private:
    CHPPLVEventFactory& operator=(const CHPPLVEventFactory&);
    CHPPLVEventFactory(const CHPPLVEventFactory&);

public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(cc::IID_ILogViewerEventFactory, ILogViewerEventFactory)
    SYM_INTERFACE_MAP_END()


public:
    // ILogViewerEventFactory methods
    virtual HRESULT Create(ccEvtMgr::CEventEx* pEvent, cc::ILogViewerEvent*& pILogViewerEvent) throw();
    virtual HRESULT Destroy(cc::ILogViewerEvent* pILogViewerEvent) throw();

};
