#pragma once

#include "ccEventManagerEx.h"
#include "ccLogManagerEx.h"
#include "ccEventManagerModuleEx.h"

#include "HPPEventFactory.h"
#include "HPPLogFactory.h"

class CHPPUEMModule :
	public ccEvtMgr::CEventManagerModuleEx,
	public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    CHPPUEMModule(void);
    virtual ~CHPPUEMModule(void);

    SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(ccEvtMgr::IID_EventManagerModuleEx, CEventManagerModuleEx)
	SYM_INTERFACE_MAP_END()

    public:
	// CEventManagerModuleEx override.
	virtual ccEvtMgr::CError::ErrorType Create(long nModuleId,
		ccEvtMgr::CEventManagerEx* pEventManager, ccEvtMgr::CLogManagerEx* pLogManager) throw();

	// CEventManagerModuleEx override.
    virtual ccEvtMgr::CError::ErrorType Destroy() throw();
	
	// CEventManagerModuleEx override.
    virtual ccEvtMgr::CError::ErrorType PrepareDestroy() throw();

private:
	// Module ID.
    long m_nModuleId;

	// Event manager object.
    ccEvtMgr::CEventManagerExPtr m_pEventManager;

	// Log manager object.
    ccEvtMgr::CLogManagerExPtr m_pLogManager;

	// Event factory for events.
    CSymStaticRefCount<CHPPEventFactory> m_EventFactory;

	// Log factory for events.
	CSymStaticRefCount<CHPPLogFactory> m_LogFactory;

};
