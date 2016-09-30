#include "StdAfx.h"
#include "HPPUEMModule.h"

using namespace ccEvtMgr;

CHPPUEMModule::CHPPUEMModule(void)
{
    CCTRACEI(_T("CHPPUEMModule(0x%08X)::CHPPUEMModule - c'tor"), (DWORD_PTR)this);
}

CHPPUEMModule::~CHPPUEMModule(void)
{
    CCTRACEI(_T("CHPPUEMModule(0x%08X)::~CHPPUEMModule - d'tor"), (DWORD_PTR)this);
}

CError::ErrorType CHPPUEMModule::Create(long nModuleId,
                                        CEventManagerEx* pEventManager,
										CLogManagerEx* pLogManager)
{
    CCTRACEI(_T("CHPPUEMModule(0x%08X)::Create - creating Event Manager Plugin"), (DWORD_PTR)this);
	m_nModuleId = nModuleId;
	m_pEventManager = pEventManager;
	m_pLogManager = pLogManager;

    // Register the event factory.

    if (m_EventFactory.Register(nModuleId, pEventManager) == FALSE)
		return CError::eFatalError;

	// Register the log factory.

	if (m_LogFactory.Register(nModuleId, pLogManager, &m_EventFactory) == FALSE)
		return CError::eFatalError;

	return CError::eNoError;
}

CError::ErrorType CHPPUEMModule::Destroy()
{
    CCTRACEI(_T("CHPPUEMModule(0x%08X)::Destroy - destroying Event Manager Plugin"), (DWORD_PTR)this);
    
    // Unregister the event factory.
    if (m_EventFactory.Unregister() == FALSE)
		return CError::eFatalError;

	return CError::eNoError;
}

CError::ErrorType CHPPUEMModule::PrepareDestroy()
{
    CCTRACEI(_T("CHPPUEMModule(0x%08X)::PrepareDestroy - preparing to destroy Event Manager Plugin"), (DWORD_PTR)this);

    // Unregister the log factory.
	if (m_LogFactory.Unregister() == FALSE)
		return CError::eFatalError;

	return CError::eNoError;
}

