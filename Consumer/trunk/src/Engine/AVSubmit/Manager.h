////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AVSubmissionInterface.h"
#include "ccTaskThread.h"
#include "AddSubmission.h"
#include "SymSingletonInterface.h"
#include "SubmissionManagerInternalInterface.h"
#include "QBackupLoader.h"
#include "AVInterfaceLoader.h"
#include "ccScanwLoader.h"
#include "SubmissionEngineLoader.h"

namespace AVSubmit {


class CManager :
	public ISymBaseSingletonImpl<CManager>,
	public IAVSubmissionManager,
	public mgr::IManager
{

	
	struct LowPriorityTaskThread : ccLib::CTaskThread
	{
		BOOL InitInstance();
	};
public:
	CManager(void);
	virtual ~CManager(void);

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(IID_AVSubmissionManager, IAVSubmissionManager)
		SYM_INTERFACE_ENTRY(mgr::IID_IManager, mgr::IManager);
	SYM_INTERFACE_MAP_CHAIN(ISymBaseSingletonImpl<CManager>)

	virtual HRESULT Initialize(Mode eMode) throw();
	virtual HRESULT Add(AVModule::IAVMapBase* pData, DWORD dwType = eAll) throw();
	virtual HRESULT ManualAdd(REFGUID, REFGUID) throw();
	virtual HRESULT ManualAdd(AVModule::IAVMapBase* pData) throw();

	virtual HRESULT GetScanner(ccScanw::IScannerw*& pScanner) throw();
	virtual HRESULT GetEraser(ccEraser::IEraser4*& pEraser) throw();
	virtual HRESULT GetAVDefInfo(AVModule::IAVDefInfo*& pDefInfo) throw();
	virtual HRESULT GetSubmissionEngine(clfs::ISubmissionEngine*& pSubmissionEngine) throw();
	virtual HRESULT GetAVThreatInfo(AVModule::IAVThreatInfo*& pThreatInfo) throw();
	virtual HRESULT GetQBackup(QBackup::IQBackup*& pBackup) throw();

protected:
	HRESULT PostTask(CAddSubmission::AutoPtr& p);
	HRESULT GetActor(cc::IString*& pString);
	bool AutoSubmitEnabled() const;

	static HRESULT GetVersion(ccLib::CStringW& ) throw(std::bad_alloc, ATL::CAtlException);
	static HRESULT GetProductName(ccLib::CStringW& ) throw(std::bad_alloc, ATL::CAtlException);

	cc::IStringPtr m_pProduct;
	cc::IStringPtr m_pVersion;
	cc::IStringPtr m_pActor;

	Mode m_eMode;
	bool m_bInitialized;

	LowPriorityTaskThread m_thread;

	ccLib::CCriticalSection m_objectLock;

	//loaders for local objects.
	//using unmanaged loaders due to race condition in ISymBase::Release() 
	//when using managed loaders

	//BEGIN - unmanaged loaders
	QBackup::qb_IQBackup m_ldr_backup;
	AVModule::AVLoader_IAVThreatInfo m_ldr_threatinfo;
	AVModule::AVLoader_IAVDefInfo m_ldr_definfo;
	ccScanw::ccScan_IScannerw m_ldr_ccscanw;
	clfs::subeng_ISubmissionEngine m_ldr_subeng;
	::CSymInterfaceLoader m_ldr_eraser;
	//END - unmanaged loaders

	//BEGIN - unmanaged objects
	AVModule::IAVDefInfoPtr m_pDefInfo;
	ccEraser::IEraser4Ptr m_pEraser;
	ccScanw::IScannerwPtr m_pScanner;
	clfs::ISubmissionEnginePtr m_pSubmissionEngine;
	AVModule::IAVThreatInfoPtr m_pThreatInfo;
	QBackup::IQBackupPtr m_pBackup;
	//END - unmanaged objects

};

} //namespace AVSubmit
