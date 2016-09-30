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

#include "SAVQuarantineExtractor.h"
#include "Utils.h"
#include "EraserUtils.h"

#include "windows.h"
#include "IQUARAN.h"

#include "ccKeyValueCollectionInterface.h"
#include "ccTrace.h"
#include "ccLib.h"

using namespace clfs;

namespace SAVSubmission {

CSAVQuarantineExtractor::CSAVQuarantineExtractor() :
			m_ptrAnomaly(NULL),
			m_pQuarItem(NULL)
{
	CCTRACEI(CCTRCTX);
}

CSAVQuarantineExtractor::~CSAVQuarantineExtractor(void)
{
	CCTRACEI(CCTRCTX);

	//orderly destruction
	m_data.reset();

	if (m_pQuarItem != NULL)
		m_pQuarItem->Release();
}

// IAVSubmission
HRESULT CSAVQuarantineExtractor::GetAnomaly(ccEraser::IAnomaly*& pAnomaly) const throw()
{
	utils::FreeObject(pAnomaly);
	HRESULT hr = E_POINTER;
	pAnomaly = m_data.pAnomaly;
	if(pAnomaly != NULL)
	{
		pAnomaly->AddRef();
		hr = S_OK;
	}
	return hr;
}

HRESULT CSAVQuarantineExtractor::GetRemediationCount(size_t& nSize) const throw()
{
	nSize = m_data.list.size();
	CCTRCTXI1(_T("data size = %Iu"), nSize);
	return S_OK;
}

HRESULT CSAVQuarantineExtractor::GetRemediation(size_t nIndex, ccEraser::IRemediationAction*& pRem, cc::IStream*& pStream) const throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		CCTRCTXI1(_T("Load Remediation @ %d"), nIndex);
		if(m_data.list.size() <= nIndex || nIndex < 0)
		{
			hrx << HRESULT_FROM_WIN32(ERROR_INVALID_INDEX);
		}

		utils::FreeObject(pRem);
		utils::FreeObject(pStream);

		const Remediation& r = m_data.list[nIndex];
		
		pRem = r.pRemediation;
		pStream = r.pStream;
		
		hrx << (pRem == NULL && pStream == NULL ? E_POINTER : S_OK);

		if(pRem != NULL) pRem->AddRef();
		if(pStream != NULL) pStream->AddRef();

		CCTRCTXI2(_T("r: %p s: %p"), pRem, pStream);
		DumpRemediation(nIndex, pRem);
	}
	CATCH_RETURN(hr);
}

HRESULT CSAVQuarantineExtractor::Initialize(IQuarantineItem2* pQuarItem)
{
	if (pQuarItem == NULL)
		return E_INVALIDARG;

	if (m_pQuarItem != NULL)
		m_pQuarItem->Release();

	m_pQuarItem = pQuarItem;

	if (m_pQuarItem != NULL)
		m_pQuarItem->AddRef();
	return S_OK;
}

} //namespace SAVSubmission
