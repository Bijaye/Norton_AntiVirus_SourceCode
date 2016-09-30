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
#include ".\avsubmissiondetection.h"
#include "utils.h"

namespace AVSubmit {

CAvSubmissionDetection::CAvSubmissionDetection(void)
{
}

CAvSubmissionDetection::~CAvSubmissionDetection(void)
{
}

HRESULT CAvSubmissionDetection::GetTypeId(GUID& typeId) const throw()
{
	typeId = clfs::DIS::TYPEID_DIS_AVPING_TRANSLATED;
	return S_OK;
}
bool CAvSubmissionDetection::GetObjectId(SYMOBJECT_ID& oid) const throw()
{
	oid = CLSID_AvSubmissionDetectionChild;
	return true;
}

HRESULT CAvSubmissionDetection::GetSubmissionData(const cc::IKeyValueCollection* pCriteria, cc::IStream*& pData) throw()
{
	HRESULT hr = CAvSubmission::GetSubmissionData(pCriteria, pData);
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Failed: 0x%08X - returning successful null stream"), hr);
		hr = S_OK;
	}
	return hr;
}

}
