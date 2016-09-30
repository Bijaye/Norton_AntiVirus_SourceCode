#pragma once

#include "NAVEventFactory.h"
#include "NAVEventLogFactory.h"

#include "ccEventManagerModuleEx.h"

class CNAVEventFactoryModule :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public ccEvtMgr::CEventManagerModuleEx
{
public:
    CNAVEventFactoryModule(void);
    ~CNAVEventFactoryModule(void);

	SYM_INTERFACE_MAP_BEGIN()               
		SYM_INTERFACE_ENTRY(ccEvtMgr::IID_EventManagerModuleEx, ccEvtMgr::CEventManagerModuleEx)
	SYM_INTERFACE_MAP_END()                 

public:
    virtual ccEvtMgr::CError::ErrorType Create(long nModuleId,
                                               ccEvtMgr::CEventManagerEx* pEventManager, 
                                               ccEvtMgr::CLogManagerEx* pLogManager) throw();
    virtual ccEvtMgr::CError::ErrorType Destroy() throw();
    virtual ccEvtMgr::CError::ErrorType PrepareDestroy() throw();

protected:
    long m_nModuleId;
    ccEvtMgr::CEventManagerExPtr m_pEventManager;
    ccEvtMgr::CLogManagerExPtr m_pLogManager;
    CNAVEventFactory m_EventFactory;
    CNAVEventLogFactory m_LogFactory;
};
