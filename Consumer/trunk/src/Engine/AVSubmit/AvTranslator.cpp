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

#include "StdAfx.h"
#include "AvTranslator.h"
#include <srx.h>
#include "Utils.h"
#include "AvSubmissionSample.h"
#include "AvSubmissionDetection.h"
#include "ccInstanceFactory.h"
#include <algorithm>

namespace AVSubmit {
namespace {

	template<typename T>
	void createSubmission(CAvSubmission*& pOut) throw(std::bad_alloc)
	{
		utils::FreeObject(pOut);
		CSymPtr<T> p;
		T::Allocate(p.m_p);
		CCTHROW_BAD_ALLOC(p);
		pOut = p.Detach();
	}
}

CAvTranslator::CAvTranslator(void)
{
}

CAvTranslator::~CAvTranslator(void)
{
}

HRESULT CAvTranslator::Initialize(clfs::ISubmission* pSubmission, clfs::ITranslatorContext* pContext) throw()
{
	HRESULT hr = S_OK;
	m_pContext = pContext; //Save context
	if(pSubmission != NULL && m_pContext != NULL)
	{
		m_pSubmission = pSubmission;
		m_pAVSubmission = pSubmission;
		
		if(m_pAVSubmission == NULL)
		{
			CCTRCTXE0(_T("Failed to QI for AVSubmission"));
			hr = E_NOINTERFACE;
		}
	}
	else
	{
		CCTRCTXE0(_T("pSubmission != NULL && pContext != NULL"));
		hr = E_INVALIDARG;
	}
	return hr;
}

HRESULT CAvTranslator::GetAttributeName(size_t nKey, cc::IString*& pString, DWORD& dwFlags) const throw()
{
	CCTRCTXI1(_T("nKey = %Iu"), nKey);
	return E_NOTIMPL;
}

HRESULT CAvTranslator::GetSubmission(clfs::ISubmission*& pSubmission) throw()
{
	utils::FreeObject(pSubmission);
	STAHLSOFT_HRX_TRY(hr)
	{
		if(m_pAVSubmission == NULL || m_pSubmission == NULL || m_pContext == NULL)
			hrx << E_POINTER;

		hrx << LoadRemediations();
		hrx << MergeRemediations();
		hrx << CreateSubmissions();

		if(m_submissions.size() == 1)
		{
			CCTRCTXI0(_T("forwarding 1 submission"));
			clfs::ISubmissionPtr pSub = *m_submissions.begin();
			pSubmission = pSub.Detach(); //1-to-1 translation...
		}
		else
		{
			CCTRCTXI1(_T("Creating %d  child submissions"), m_submissions.size());
			for(Submissions::iterator i = m_submissions.begin(), e = m_submissions.end(); i!=e; ++i)
			{
				hrx << m_pContext->CreateChildSubmission(*i);
			}

			//multiple file submissions not supported!
			//so we created the child ones above...
			hrx << HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
		}
	}
	CATCH_RETURN(hr);
}

HRESULT CAvTranslator::LoadRemediations() throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		size_t nSize = 0;
		hrx << m_pAVSubmission->GetRemediationCount(nSize);
		for(size_t i = 0; i<nSize; ++i)
		{
			submit_data r(i);
			hrx << m_pAVSubmission->GetRemediation(i, r.pRem.m_p, r.pStream.m_p);
			if(r.pRem != NULL)
			{
				ccEraser::eObjectType eType;
				ccEraser::eResult eRes = r.pRem->GetType(eType);
				if(ccEraser::Failed(eRes))
				{
					CCTRCTXE1(_T("Failed to get type: %d"), eRes);
					hrx << E_FAIL;
				}
				r.eType = eType;
				switch(r.eType)
				{
				case ccEraser::FileRemediationActionType:
				case ccEraser::InfectionRemediationActionType:
					CCTRCTXI0(_T("File"));
					m_files.push_back(r);
					break;
				case ccEraser::RegistryRemediationActionType:
					CCTRCTXI0(_T("Registry"));
					m_registry.push_back(r);
					break;
				default:
					CCTRCTXE1(_T("Unsupported type: %d"), r.eType);
					break;
				}
			}
			else if(r.pStream != NULL)
			{
				//just raw data...
				CCTRCTXI0(_T("Raw Sample"));
				m_raw.push_back(r);
			}
		}
	}
	CATCH_RETURN(hr);
}

HRESULT CAvTranslator::MergeRemediations() throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(m_registry.size() == 0)
			return S_OK;

		for(SubmitList::iterator i = m_files.begin(), e = m_files.end(); i!=e; ++i)
		{
			submit_data& d = *i;
			d.m_children.resize(m_registry.size());
			std::copy(m_registry.begin(), m_registry.end(), d.m_children.begin());
		}
	}
	CATCH_RETURN(hr);
}

HRESULT CAvTranslator::CreateSubmissions() throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		SubmissionType t;
		GetType(t);
		if(t == eTypeAvPing && m_files.size() == 0)
		{
			CAvSubmission::Ptr pSub;
			createSubmission<CAvSubmissionDetection>(pSub.m_p);
			hrx << pSub->Initialize(m_pAVSubmission);
			hrx << pSub->LoadAttributes();
			clfs::ISubmissionQIPtr p = pSub;
			if(p == NULL)
				hrx << E_NOINTERFACE;
			m_submissions.push_back(p);
		}
		else
		{
			CreateSubPred pred(this, t);
			std::for_each(m_files.begin(), m_files.end(), pred);
			std::for_each(m_raw.begin(), m_raw.end(), pred);
		}
	}
	CATCH_RETURN(hr);
}

bool CAvTranslator::CreateSubPred::operator () (const CAvTranslator::submit_data& d) const throw(std::bad_alloc, _com_error)
{
	CAvSubmission::Ptr pSub;
	if(type == ITranslator::eTypeAvSample)
	{
		CCTRCTXI0(_T("Createing Sample Child Submission"));
		createSubmission<CAvSubmissionSample>(pSub.m_p);
	}
	else if(type == ITranslator::eTypeAvPing)
	{
		if(bPingSubmitted)
			return true;
		CCTRCTXI0(_T("Createing Detection Child Submission"));
		createSubmission<CAvSubmissionDetection>(pSub.m_p);
	}
	else
	{
		CCTRCTXE1(_T("Unknown t yep %d"), type);
		throw _com_error(E_UNEXPECTED);
	}
	HRESULT hr = pSub->Initialize(pParent->m_pAVSubmission);
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Failed to init submission: 0x%08X"), hr);
		return false;
	}
	hr = pSub->AddDataItem(d.nIndex, static_cast<ccEraser::eObjectType>(d.eType));
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Failed to add item: 0x%08X"), hr);
		return false;
	}
	for(CAvTranslator::submit_data::List::const_iterator i = d.m_children.begin(); i!= d.m_children.end(); ++i)
	{
		hr = pSub->AddDataItem(i->nIndex, static_cast<ccEraser::eObjectType>(i->eType));
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to add child item: 0x%08X"), hr);
			return false;
		}
	}

	hr = pSub->LoadAttributes();
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Failed to load attrib for submission: 0x%08X"), hr);
		return false;
	}

	clfs::ISubmissionQIPtr p = pSub;
	if(p == NULL)
		throw _com_error(E_NOINTERFACE);

	pParent->m_submissions.push_back(p);
	if(type == ITranslator::eTypeAvPing)
		bPingSubmitted = true; //there can be only one
	return true;
}

} //namespace AVSubmit

