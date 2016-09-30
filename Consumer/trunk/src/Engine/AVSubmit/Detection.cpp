////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\detection.h"
#include "Utils.h"

namespace AVSubmit {

CDetection::CDetection(void)
{
	CCTRACEI(CCTRCTX);
}

CDetection::~CDetection(void)
{
	CCTRACEI(CCTRCTX);
}

HRESULT CDetection::GetTypeId(GUID& typeId) const throw()
{
	typeId = TYPEID_AV_DETECTION;
	return S_OK;
}
bool CDetection::GetObjectId(SYMOBJECT_ID& oid) const throw()
{
	oid = CLSID_AVDetectionSubmission;
	return true;
}

HRESULT CDetection::LoadSubmissionData() throw()
{
	TRACE_SCOPE(s0);
	if(IsManual())
		return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

	HRESULT hr = CSubmissionBase::LoadData();
	if(FAILED(hr))
	{
		CCTRCTXE1(_T("Detection can proceed w/o a sample: 0x%08X"),  hr);
		hr = SetAV();
	}
	return hr;
}

HRESULT CDetection::CheckBloodhound(DWORD dwVid) const throw()
{
	TRACE_SCOPE(s0);
	HRESULT hr = CSubmissionBase::CheckBloodhound(dwVid);
	if(FAILED(hr) && hr == HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED))
	{
		CCTRCTXI1(_T("VID %d not bloodhound, but still submitting detection ping"), dwVid);
		hr = S_OK;
		m_pAttributes->SetValue(eScanResult, DWORD(eCompleted));
	}
	return hr;
}

HRESULT CDetection::SetDetails() throw()
{
	TRACE_SCOPE(s0);
	HRESULT hr = CSubmissionBase::SetDetails();

	cc::IStringQIPtr pDesc;
	ISymBasePtr pBase;

	bool bHasDescr = false;
	if(m_pAttributes->GetValue(eDetails, pBase.m_p) && pBase != NULL)
	{
		pDesc = pBase;
		if(pDesc != NULL && pDesc->GetLength() > 0)
			bHasDescr = true;
	}
	if(!bHasDescr)
	{
		pBase.Release();
		pDesc.Release();
		m_pAttributes->GetValue(eFilePath, pBase.m_p);
		pDesc = pBase;
		if(pDesc)
			m_pAttributes->SetValue(eDetails, pDesc);
	}
	return hr;
}

} //namespace AVSubmit