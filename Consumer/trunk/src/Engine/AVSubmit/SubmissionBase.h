////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SubmissionEngineInterface.h"
#include "AVSubmissionInterface.h"
#include "AVSubmissionLoader.h"
#include "Allocator.h"
#include "ccSymSerialize.h"
#include "AVInterfaces.h"
#include "ccScanwInterface.h"
#include "QBackupInterfaces.h"
#include "ccEraserInterface.h"
#include "ContentFilter.h"
#include "SubmissionManagerInternalInterface.h"
#include "utils.h"

namespace AVSubmit {

//Yes, this loads an object from this DLL
//Instead of using managed loader, could call GetFactory Directly.
//Instead of using module mgr, could pass in __ImageBase directly to unmanaged loader
//Need to be sure to use singleton instance factory for caching to work.
//the lifetime of this object will be controlled by either:
// - the calling object, which is also exported via this DLL
// - A client object directly creating an instance of the IAVSubmissionManager.

static HRESULT GetObjectManager(mgr::IManager*& pManager)
{
	utils::FreeObject(pManager);
	AVSubmit::IAVSubmissionManagerPtr pMgr;
	SYMRESULT sr = AVSubmit::AVSubmit_IAVSubmissionManagerMgd::CreateObject(GETMODULEMGR(), pMgr.m_p);
	if(SYM_FAILED(sr))
	{
		CCTRCTXE1(_T("Failed to load submisison mgr: 0x%08X"), sr);
		return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
	}
	sr = pMgr->QueryInterface(mgr::IID_IManager, (void**)&pManager);
	if(SYM_FAILED(sr))
	{
		CCTRCTXE0(_T("Failed to QI for IManager"));
		return E_NOINTERFACE;
	}
	return S_OK;
}

class CSubmissionBase : 
	public IAVSubmission,
	public IContentFilter,
	public IAVSubmission2,
	public clfs::ISubmission,
	public cc::ISerialize,
	public CContentFilter,
	public ccSym::CSerialize,
	public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:

	typedef CSymPtr<CSubmissionBase> Ptr;

	CSubmissionBase(void);
	virtual ~CSubmissionBase(void);

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(IID_AVSubmission, IAVSubmission)
		SYM_INTERFACE_ENTRY(IID_AVSubmission2, IAVSubmission2)
		SYM_INTERFACE_ENTRY(IID_ContentFilter, IContentFilter)
		SYM_INTERFACE_ENTRY(clfs::IID_Submission, clfs::ISubmission)
		SYM_INTERFACE_ENTRY(cc::IID_Serialize, cc::ISerialize)
	SYM_INTERFACE_MAP_END()

	virtual HRESULT Initialize(const cc::IKeyValueCollection* pAttrib) throw();

	//IAVSubmission
	virtual HRESULT GetAnomaly(ccEraser::IAnomaly*& pAnomaly) const throw();
	virtual HRESULT GetRemediationCount(size_t& nSize) const throw();
	virtual HRESULT GetRemediation(size_t nIndex, ccEraser::IRemediationAction*& pRemediation, cc::IStream*& pStream) const throw();

	//IAVSubmission2
	virtual HRESULT SetSubmitted(bool bSubmitted) throw();

	//IContentFilter
	IMPLEMENT_CONTENTFILTER(CContentFilter);

	//ISubmission
	virtual HRESULT GetAttributes(cc::IKeyValueCollection*& pAttrib) const throw();
	virtual HRESULT GetSubmissionData(const cc::IKeyValueCollection* pCriteria, cc::IStream*& pData) throw();
	virtual HRESULT ProcessResponse(const cc::IKeyValueCollection* pResponse) throw();

	//ISerialize
	CCSYM_IMPLEMENT_SERIALIZE(ccSym::CSerialize);
	// GetObjectId() left up to derived

	//ccSym::CSerialize
	virtual bool Save(ccLib::CArchive&) const throw();
	virtual bool Load(ccLib::CArchive&) throw();

	enum SpecialVIDs
	{
		//these are the VIDs eraser is using...
		eVID_COH_Positive = 0x4ad6,
		eVID_COH_Suspicious = 0x4ad7,

		//these are reserved by response, but not used by eraser
		eVID_COH_PWSteal = 0x4adb,
		eVID_COH_Trojan = 0x4adc,
		eVID_COH_Keylogger = 0x4add,
		eVID_COH_Spyware = 0x4ade,
		eVID_VXMS = 0x4adf,
		
		eVID_COL = 0xffffbaad, //?: not defined yet
	};

protected:

	enum {eVersion = 0x1};


	struct Remediation {
		typedef std::vector<Remediation> List;
		cc::IStreamPtr pStream;
		ccEraser::IRemediationActionPtr pRemediation;
		GUID guidUndo;
		Remediation() : guidUndo(GUID_NULL) {}
	};

	struct Data {
		Remediation::List list;
		ccEraser::IAnomalyPtr pAnomaly;
		bool bInit;
		Data() : bInit(false) {}
		void reset()
		{
			list.clear();
			pAnomaly.Release();
			bInit = false;
		}
	};

	virtual HRESULT LoadData() throw();
	virtual HRESULT LoadRemediations(QBackup::IQBackupSet*) throw();
	virtual HRESULT LoadSubmissionData() throw();
	virtual HRESULT CheckDefDate() throw();
	virtual HRESULT SetAV() throw();
	virtual HRESULT SetOEH() throw();
	virtual HRESULT SetAVCompressed() throw();
	virtual HRESULT SetEmail() throw();
	virtual HRESULT CheckBloodhound(DWORD) const throw();
	virtual bool MustBePresent() const throw() = 0;
	virtual HRESULT SetDetails() throw();
	virtual HRESULT SetVXMS(QBackup::IQBackupSet* pSet) throw();
	virtual HRESULT SetManual() throw();

	HRESULT GetEraser(ccEraser::IEraser4*&) const ;
	HRESULT GetDefManager(AVModule::IAVDefInfo*&) const ;
	HRESULT GetBackup(QBackup::IQBackup*&) const ;
	HRESULT GetManager(mgr::IManager*&) const;
	HRESULT GetAVThreatInfo(AVModule::IAVThreatInfo*&) const;
	HRESULT RestoreStream(QBackup::IQBackupSet*, REFGUID, REFGUID, cc::IStream*&, size_t = -1);
	HRESULT GetCOHVer(ccLib::CStringW&) const ;
	HRESULT GetCOLVer(ccLib::CStringW&) const ;
	HRESULT GetAVVer(ccLib::CStringW&) const ;
	bool IsComponentSet() const;
	HRESULT SetComponent(const wchar_t* pszComp, const wchar_t* pszVer, Category cat, const wchar_t* pszSubComp = NULL);
	bool IsManual() const;
	HRESULT LoadDate(QBackup::IQBackupSet* pSet);
	HRESULT BackupSample(GUID& guid);
	HRESULT BackupRemediation(QBackup::IQBackupSet* pSet, ccEraser::IRemediationAction* pRemediation) throw();
	HRESULT SetSubmittedKey(bool bSubmitted);
	HRESULT GetThreatCats() throw();
	void DumpRemediation(size_t nIndex, const ccEraser::IRemediationAction* pRemediation) const throw();
	void DumpAnomaly(const ccEraser::IAnomaly* pAnomaly) const throw();

private:

	//the manager will control the lifetime of the following objects
	//it must be destroyed last
	mutable mgr::IManagerPtr m_pManager;

	//BEGIN - objects managed by mgr::IManager
	mutable QBackup::IQBackupPtr m_pBackup;
	mutable AVModule::IAVDefInfoPtr m_pDefInfo;
	mutable ccEraser::IEraser4Ptr m_pEraser;
	mutable ccScanw::IScannerwPtr m_pScanner;
	mutable AVModule::IAVThreatInfoPtr m_pThreatInfo;
	//END - objects managed by mgr::IManager

protected:
	cc::IKeyValueCollectionPtr m_pAttributes;
	Data m_data;
	bool m_bInitializing;

private:

	CSubmissionBase(const CSubmissionBase&);
	const CSubmissionBase& operator = (const CSubmissionBase&);

};

// {0820BE07-E4F8-4fe6-8B53-B5AD1FAF619D}
DEFINE_GUID(GT_SubmitOEHSet, 0x820be07, 0xe4f8, 0x4fe6, 0x8b, 0x53, 0xb5, 0xad, 0x1f, 0xaf, 0x61, 0x9d);
// {C61AF80A-4A33-474f-96C4-DBD73AB124F7}
DEFINE_GUID(GT_SubmitUnremediated, 0xc61af80a, 0x4a33, 0x474f, 0x96, 0xc4, 0xdb, 0xd7, 0x3a, 0xb1, 0x24, 0xf7);


}
