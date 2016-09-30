// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
#pragma once

#include "SubmissionEngineInterface.h"
#include "AVSubmissionInterface.h"
#include "ccSymSerialize.h"
#include "ccScanwInterface.h"
#include "ccEraserInterface.h"

namespace SAVSubmission {

class IAVSubmissionInternal : public ISymBase
{
public:
	virtual HRESULT SetSubmitted(bool bSubmitted) throw() = 0;
};

SYM_DEFINE_INTERFACE_ID(IID_AVSubmissionInternal, 0xc5ef1b5a, 0xc97d, 0x4b5c, 0xa3, 0xf8, 0x89, 0xdd, 0x29, 0x6e, 0xe6, 0x2c);
TYPEDEF_SYM_POINTERS(IAVSubmissionInternal, IID_AVSubmissionInternal);


class CSubmissionBase : 
	//public IContentFilter,	TODO:JJM: must hook filtering up
	public AVSubmit::IAVSubmission,
	public IAVSubmissionInternal,
	public clfs::ISubmission,
	public cc::ISerialize,
	//public CContentFilter,
	public ccSym::CSerialize,
	public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:

	typedef CSymPtr<CSubmissionBase> Ptr;

	CSubmissionBase(ccScanw::IScannerw* pScanner);
	virtual ~CSubmissionBase(void);

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(AVSubmit::IID_AVSubmission, IAVSubmission)
		SYM_INTERFACE_ENTRY(IID_AVSubmissionInternal, IAVSubmissionInternal)
		//SYM_INTERFACE_ENTRY(IID_ContentFilter, IContentFilter)
		SYM_INTERFACE_ENTRY(clfs::IID_Submission, clfs::ISubmission)
		SYM_INTERFACE_ENTRY(cc::IID_Serialize, cc::ISerialize)
	SYM_INTERFACE_MAP_END()

	virtual HRESULT Initialize(const cc::IKeyValueCollection* pAttrib) throw();

	//IAVSubmissionInternal
	virtual HRESULT SetSubmitted(bool bSubmitted) throw();

	//IContentFilter
	//IMPLEMENT_CONTENTFILTER(CContentFilter);

	//ISubmission
	virtual HRESULT GetAttributes(cc::IKeyValueCollection*& pAttrib) const throw();
	virtual HRESULT GetSubmissionData(const cc::IKeyValueCollection* pCriteria, cc::IStream*& pData) throw();
	virtual HRESULT ProcessResponse(const cc::IKeyValueCollection* pResponse) throw();

	// IAVSubmission
	virtual HRESULT GetAnomaly(ccEraser::IAnomaly*& pAnomaly) const throw();
	virtual HRESULT GetRemediationCount(size_t& nSize) const throw();
	virtual HRESULT GetRemediation(size_t nIndex, ccEraser::IRemediationAction*& pRemediation, cc::IStream*& pStream) const throw();

	//ISerialize
	CCSYM_IMPLEMENT_SERIALIZE(ccSym::CSerialize);
	// GetObjectId() left up to derived

	//ccSym::CSerialize
	virtual bool Save(ccLib::CArchive&) const;
	virtual bool Load(ccLib::CArchive&);

protected:
	HRESULT SetDescription(cc::IKeyValueCollectionPtr& pAttrib, UINT uiStringResId) const throw();
	HRESULT GetPVA(cc::IStringPtr& pProduct, cc::IStringPtr& pVersion, cc::IStringPtr& pActor) const throw();

	HRESULT GetDefinitionsInfo(SYSTEMTIME& sDefDate, DWORD& dwRev, DWORD& dwSeq) const throw ();
	HRESULT SetDefDateAndSeqAttributes() const throw();

	HRESULT GetVirusName(DWORD dwVid, ccLib::CStringW& sVirusName) const throw();
	HRESULT SetVirusNameAttribute(DWORD dwVid) const throw();

	virtual HRESULT SetAV() throw();
	virtual HRESULT SetAVCompressed() throw();
	virtual HRESULT CheckBloodhound(DWORD) const throw();
	virtual HRESULT SetDetails() throw();

	HRESULT GetScanner(ccScanw::IScannerw*& pScanner) const;
	HRESULT GetCOHVer(ccLib::CStringW&) const ;
	HRESULT GetAVVer(ccLib::CStringW&) const ;
	bool IsComponentSet() const;
	HRESULT SetComponent(const wchar_t* pszComp, const wchar_t* pszVer, AVSubmit::IAVSubmission::Category cat, const wchar_t* pszSubComp = NULL);
	HRESULT SetSubmittedKey();
	HRESULT SetThreatCatProperties(const ccEraser::IAnomaly* ptrAnomaly) throw();

	cc::IKeyValueCollectionPtr m_pAttributes;
	ccScanw::IScannerwPtr m_ptrScanner;

private:
	CSubmissionBase();
	CSubmissionBase(const CSubmissionBase&);
	const CSubmissionBase& operator= (const CSubmissionBase&);
	bool operator== (const CSubmissionBase&);

	static const DWORD kdwVersion = 0x1;

};

//TODO:JJM: what are these for?
// {0820BE07-E4F8-4fe6-8B53-B5AD1FAF619D}
DEFINE_GUID(GT_SubmitOEHSet, 0x820be07, 0xe4f8, 0x4fe6, 0x8b, 0x53, 0xb5, 0xad, 0x1f, 0xaf, 0x61, 0x9d);
// {C61AF80A-4A33-474f-96C4-DBD73AB124F7}
DEFINE_GUID(GT_SubmitUnremediated, 0xc61af80a, 0x4a33, 0x474f, 0x96, 0xc4, 0xdb, 0xd7, 0x3a, 0xb1, 0x24, 0xf7);


} // namespace SAVSubmission
