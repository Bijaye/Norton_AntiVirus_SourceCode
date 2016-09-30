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


#include "ccEraserInterface.h"
#include "AvSubmission.h"
#include "AvSubmissionInterface.h"
#include "SubmissionTranslatorInterface.h"
#include <comdef.h>

namespace AVSubmit {

class CAvTranslator : 
	public clfs::ITranslator,
	public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
	CAvTranslator(void);
	virtual ~CAvTranslator(void);

	SYM_INTERFACE_MAP_BEGIN()
		SYM_INTERFACE_ENTRY(clfs::IID_Translator, clfs::ITranslator)
	SYM_INTERFACE_MAP_END()

	virtual HRESULT Initialize(clfs::ISubmission* pSubmission, clfs::ITranslatorContext* pContext) throw();
	virtual HRESULT GetSubmission(clfs::ISubmission*&) throw();
	virtual HRESULT GetAttributeName(size_t nKey, cc::IString*& pName, DWORD& dwFlags) const throw();

protected:
	struct submit_data
	{
		enum {eTypeUnknown = 0xf00d};
		typedef std::vector<submit_data> List;

		List m_children;
		ccEraser::IRemediationActionPtr pRem;
		cc::IStreamPtr pStream;

		long eType;
		size_t nIndex;

		submit_data() : eType(eTypeUnknown), nIndex(size_t(-1)) {}
		submit_data(size_t i) : eType(eTypeUnknown), nIndex(i) {}
	};
	typedef std::vector<submit_data> SubmitList;
	typedef std::vector<clfs::ISubmissionPtr> Submissions;

	HRESULT LoadRemediations() throw();
	HRESULT MergeRemediations() throw();
	HRESULT CreateSubmissions() throw();

protected:
	clfs::ISubmissionPtr m_pSubmission;
	AVSubmit::IAVSubmissionQIPtr m_pAVSubmission;
	clfs::ITranslatorContextPtr m_pContext;

	SubmitList m_raw;
	SubmitList m_files;
	SubmitList m_registry;
	Submissions m_submissions;

protected:
	struct CreateSubPred
	{
		mutable CAvTranslator* pParent;
		long type;
		mutable bool bPingSubmitted;
		CreateSubPred(CAvTranslator* p, long t) : pParent(p), type(t), bPingSubmitted(false) {}
		bool operator()(const CAvTranslator::submit_data& s) const throw(std::bad_alloc, _com_error);
	};

private:
	CAvTranslator(const CAvTranslator&);
	const CAvTranslator& operator = (const CAvTranslator&);

};

class CAvPingTranslator : public CAvTranslator
{
public:
	CAvPingTranslator() {CCTRACEI(CCTRCTX);}
	virtual ~CAvPingTranslator() {CCTRACEI(CCTRCTX);}
	virtual HRESULT GetType(clfs::ITranslator::SubmissionType& eType) const throw()
	{
		eType = clfs::ITranslator::eTypeAvPing;
		return S_OK;
	}
};

class CAvSampleTranslator : public CAvTranslator
{
public:
	CAvSampleTranslator() {CCTRACEI(CCTRCTX);}
	virtual ~CAvSampleTranslator() {CCTRACEI(CCTRCTX);}
	virtual HRESULT GetType(clfs::ITranslator::SubmissionType& eType) const throw()
	{
		eType = clfs::ITranslator::eTypeAvSample;
		return S_OK;
	}
};

} //namespace AVSubmit
