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
#include "SAVSubmitterImpl.h"
#include "SAVQuarantineSubmission.h"
#include "SAVAVDetection.h"
#include "SAVCOHSample.h"
#include "SubmissionEngineLoader.h"
#include "Allocator.h"
#include "utils.h"

#include "ccScanwInterface.h"
#include "ccSymStringImpl.h"
#include "ccSymKeyValueCollectionImpl.h"
#include "ccTrace.h"
#include "ccLib.h"

namespace SAVSubmission {

CSAVSubmitterImpl::CSAVSubmitterImpl(void)
{
}

CSAVSubmitterImpl::~CSAVSubmitterImpl(void)
{
}

HRESULT CSAVSubmitterImpl::GetSubmissionEngine(clfs::ISubmissionEnginePtr& ptrEngine) const throw()
{
	HRESULT hResult = S_OK;

	if (m_ptrSubmissionEngine == NULL)
	{
		SYMRESULT sr = clfs::subeng_ISubmissionEngineMgd::CreateObject(ptrEngine.m_p);
		if (SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to load subeng: 0x%08X"), sr);
			hResult = HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
		}
		else
		{
			hResult = ptrEngine->Initialize(NULL);
		}
	}
	else
		ptrEngine = m_ptrSubmissionEngine;
	return hResult;
}

HRESULT CSAVSubmitterImpl::Submit(clfs::ISubmission* pSubmission) const throw()
{
	clfs::ISubmissionEnginePtr ptrSubmissionEngine;

	HRESULT hResult = GetSubmissionEngine(ptrSubmissionEngine);
	if (SUCCEEDED(hResult))
	{
		GUID id;
		hResult = ptrSubmissionEngine->Create(pSubmission, id);
		if (FAILED(hResult))
		{
			g_DebugOutput.DebugOutputError(_T("Failed to send: 0x%08X\n"), hResult);
		}
	}
	else
	{
		g_DebugOutput.DebugOutputError(_T("Failed to get submission engine: 0x%08X\n"), hResult);
	}
	return hResult;
}

/*
* Manual submissions must include the following properties:
*	ISubmission::eProductName
*	ISubmission::eProductVersion
*	ISubmission::eActor
*	ISubmission::eDescription
*	IAVSubmission::eManual
*	IAVSubmission::eScanResult (eHueristic)
*	IAVSubmission::eDateQuarantined
* If the AV engines detect something in the submission (the VID is not 0):
*	IAVSubmission::eVirusName
*	IAVSubmission::eVID
*	ISubmission::eComponentName ("AV Engine")
*	ISubmission::eComponentVersion (version of NAVENG.SYS)
*	IAVSubmission::eSampleCategory (IAVSubmission::eCategoryAntiVirus)
* Otherwise:
*	ISubmission::eComponentName ("Manual")
*	ISubmission::eComponentVersion ("1.0")
*	IAVSubmission::eSampleCategory (IAVSubmission::eCategoryHueristic)
*/
HRESULT CSAVSubmitterImpl::SubmitQuarantinedSample(IQuarantineItem2* pQuarantineItem, ccScanw::IScannerw* pScanner) throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
			CSymPtr<CSAVQuarantineSubmission> ptrQuarantineSubmission;
			if (CSAVQuarantineSubmission::Allocate(ptrQuarantineSubmission.m_p, pScanner))
			{
				hrx << ptrQuarantineSubmission->Initialize(pQuarantineItem);
				hrx << Submit(ptrQuarantineSubmission);
			}
			else
				hrx <<  E_OUTOFMEMORY;
	}
	CATCH_RETURN(hr);
}

HRESULT CSAVSubmitterImpl::SubmitCOHSample(const ccEraser::IAnomaly* pAnomaly, ccScanw::IScannerw* pScanner) throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		CSymPtr<CSAVCOHSample> ptrCOHSample;
		if (CSAVCOHSample::Allocate(ptrCOHSample.m_p, pScanner))
		{
			hrx << ptrCOHSample->Initialize(pAnomaly);
			hrx << Submit(ptrCOHSample);
		}
		else
			hrx <<  E_OUTOFMEMORY;
	}
	CATCH_RETURN(hr);
}

HRESULT CSAVSubmitterImpl::SubmitAVDetection(const ccEraser::IAnomaly* pAnomaly, ccScanw::IScannerw* pScanner) throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		CSymPtr<CSAVAVDetection> ptrAVDetection;
		if (CSAVAVDetection::Allocate(ptrAVDetection.m_p, pScanner))
		{
			hrx << ptrAVDetection->Initialize(pAnomaly);
			hrx << Submit(ptrAVDetection);
		}
		else
			hrx <<  E_OUTOFMEMORY;
	}
	CATCH_RETURN(hr);
}

} //namespace SAVSubmission
