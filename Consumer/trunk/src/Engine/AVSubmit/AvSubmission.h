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
#include "Allocator.h"
#include "ccSerializeInterface.h"
#include "ccSymSerialize.h"
#include "ccEraserInterface.h"
#include "SubmissionTranslatorInterface.h"

#include <comdef.h>

namespace AVSubmit {

class CAvSubmission :
	public clfs::ISubmission,
	public ISymBaseImpl<CSymThreadSafeRefCount>,
	public cc::ISerialize,
	public ccSym::CSerialize
{
public:
	typedef CSymPtr<CAvSubmission> Ptr;

	CAvSubmission(void);
	virtual ~CAvSubmission(void);

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(clfs::IID_Submission, clfs::ISubmission)
		SYM_INTERFACE_ENTRY(cc::IID_Serialize, cc::ISerialize)
	SYM_INTERFACE_MAP_END()

	virtual HRESULT Initialize(AVSubmit::IAVSubmission*) throw();
	virtual HRESULT AddDataItem(size_t nIndex, ccEraser::eObjectType eType) throw();
	virtual HRESULT LoadAttributes() throw();

	//ISubmission
	virtual HRESULT GetAttributes(cc::IKeyValueCollection*& pAttrib) const throw();
	virtual HRESULT GetSubmissionData(const cc::IKeyValueCollection* pCriteria, cc::IStream*& pData) throw();
	virtual HRESULT ProcessResponse(const cc::IKeyValueCollection* pResponse) throw();
	
	//ISerialize
	CCSYM_IMPLEMENT_SERIALIZE(ccSym::CSerialize);

	//ccSym::CSerialize
	virtual bool Save(ccLib::CArchive&) const throw();
	virtual bool Load(ccLib::CArchive&) throw();

protected:

	virtual HRESULT IsSubmittable(cc::IStream* pStream) const throw();

	HRESULT GetFileData(ccEraser::IRemediationAction*&, cc::IStream*&) throw();
	HRESULT LoadDescription() throw();
	HRESULT GetThreatCats() throw();
	HRESULT ClearReadOnly(LPCTSTR pszFile) throw();

	enum {eVersion = 0x1};

	//the original submission
	AVSubmit::IAVSubmissionQIPtr m_pAVSubmission;
	clfs::ISubmissionQIPtr m_pSubmission;
	cc::IKeyValueCollectionPtr m_pAttrib;

	struct Data
	{
		DWORD m_dwIndex;
		DWORD m_dwType;
		Data() : m_dwIndex(0), m_dwType(0) {}
		Data(DWORD d0, DWORD d1) : m_dwIndex(d0), m_dwType(d1) {}
	};

	typedef std::vector<Data> DataList;
	DataList m_list;

	typedef std::vector<ccLib::CString> FileSet;
	FileSet m_tempFiles;
};

////////////////////////////////////////////////////////
// Object IDs for internal child submission objects

// {4EE20F0C-E76B-49e3-8218-6E3566583D94}
SYM_DEFINE_GUID(CLSID_AvSubmissionDetectionChild, 0x4ee20f0c, 0xe76b, 0x49e3, 0x82, 0x18, 0x6e, 0x35, 0x66, 0x58, 0x3d, 0x94);

// {BA9CFBE3-66BE-4a8d-8370-0D252E51D64B}
SYM_DEFINE_GUID(CLSID_AvSubmissionSampleChild, 0xba9cfbe3, 0x66be, 0x4a8d, 0x83, 0x70, 0xd, 0x25, 0x2e, 0x51, 0xd6, 0x4b);

} //namespace AVSubmit

