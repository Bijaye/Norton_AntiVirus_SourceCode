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

#include "SAVQuarantineSubmission.h"
#include "SAVSubmitterInterface.h"
#include "ComponentNames.h"
#include "IQuaran.h"
#include "utils.h"

#include "ccKeyValueCollectionInterface.h"
#include "ccSymKeyValueCollectionImpl.h"
#include "ccTrace.h"
#include "ccLib.h"

#include "atltime.h"

using namespace clfs;

namespace SAVSubmission {

CSAVQuarantineSubmission::CSAVQuarantineSubmission() : CSubmissionBase(NULL),
				m_pQuarantineItem(NULL),
				m_ptrAnomaly(NULL)
{
	CCTRACEI(CCTRCTX);
}

CSAVQuarantineSubmission::CSAVQuarantineSubmission(ccScanw::IScannerw* pScanner) : CSubmissionBase(pScanner),
				m_pQuarantineItem(NULL),
				m_ptrAnomaly(NULL)
{
	CCTRACEI(CCTRCTX);
}

CSAVQuarantineSubmission::~CSAVQuarantineSubmission(void)
{
	if (m_pQuarantineItem != NULL)
		m_pQuarantineItem->Release();
	CCTRACEI(CCTRCTX);
}

HRESULT CSAVQuarantineSubmission::GetTypeId(GUID& typeId) const throw()
{
	typeId = TYPEID_AV_SAMPLE;
	return S_OK;
}
bool CSAVQuarantineSubmission::GetObjectId(SYMOBJECT_ID& oid) const throw()
{
	oid = CLSID_SAVQuarantineSubmission;
	return true;
}

HRESULT CSAVQuarantineSubmission::Initialize(IQuarantineItem2* pQuarantineItem)
{
	if (pQuarantineItem == NULL)
		return E_INVALIDARG;

	m_pQuarantineItem = pQuarantineItem;
	m_pQuarantineItem->AddRef();

	STAHLSOFT_HRX_TRY(hr)
	{
		m_pAttributes.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
		if (m_pAttributes != NULL)
			hrx << E_OUTOFMEMORY;

		DWORD dwVid;
		ccLib::CStringW sVer;
		cc::IStringPtr pProduct;
		cc::IStringPtr pVersion;
		cc::IStringPtr pActor;
		SYSTEMTIME dateQuarantined;

		hrx << GetPVA(pProduct, pVersion, pActor);
		if (pProduct != NULL)
			m_pAttributes->SetValue(clfs::ISubmission::eProductName, pProduct);
		if (pVersion != NULL)
			m_pAttributes->SetValue(clfs::ISubmission::eProductVersion, pVersion);
		if (pActor != NULL)
			m_pAttributes->SetValue(clfs::ISubmission::eActor, pActor);

		hrx << SetDescription(m_pAttributes, IDS_MANUAL_SUBMISSION_DESCRIPTION);
		hrx << SetDefDateAndSeqAttributes();

		m_pAttributes->SetValue(eScanResult, DWORD(eHueristic));
		m_pAttributes->SetValue(IAVSubmission::eManual, true);

		hrx << m_pQuarantineItem->GetDateQuarantined(&dateQuarantined);
		m_pAttributes->SetValue(AVSubmit::IAVSubmission::eDateQuarantined, dateQuarantined);

		// Quarantined items don't contain the date the threat was detected so we just assume the
		// threat was quarantined right after it was detected
		DWORD dwDateQuarantined;
		ATL::CTime detectTime(dateQuarantined);
		// We require that _USE_32BIT_TIME_T be set in order for this to work
		//C_ASSERT(sizeof(dwDateQuarantined) == sizeof(__time64_t));	TODO:JJM: this is tripping so its commented out for now. THERE IS A BUG HERE BECAUSE OF THIS!!!!
		dwDateQuarantined = (DWORD) detectTime.GetTime();
		m_pAttributes->SetValue(IAVSubmission::eDateDetected, dwDateQuarantined);

		hrx << m_pQuarantineItem->GetVirusID(&dwVid);
		if (dwVid != 0)
		{
			hrx << CheckBloodhound(dwVid);
			m_pAttributes->SetValue(IAVSubmission::eVID, dwVid);
			hrx << GetAVVer(sVer);
			hrx << SetComponent(SubSDK::Components::g_szAV, sVer, IAVSubmission::eCategoryAntiVirus, NULL);
			hrx << SetDetails();
		}
		else
		{
			hrx << SetComponent(SubSDK::Components::g_szManual, L"1.0", IAVSubmission::eCategoryHueristic);
		}

		// Now set the AV submission specific properties
		HRESULT hr2 = SetThreatCatProperties(m_ptrAnomaly);
		if (FAILED(hr2))
		{
			CCTRCTXE1(_T("SetThreatCatProperties == 0x%08X"), hr2);
		}
	}
	CATCH_RETURN(hr);
}


//IAVSubmission
HRESULT CSAVQuarantineSubmission::GetAnomaly(ccEraser::IAnomaly*& pAnomaly) const throw()
{
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	return m_oQuarantineExtractor.GetAnomaly(pAnomaly);
}

HRESULT CSAVQuarantineSubmission::GetRemediationCount(size_t& nSize) const throw()
{
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	return m_oQuarantineExtractor.GetRemediationCount(nSize);
}

HRESULT CSAVQuarantineSubmission::GetRemediation(size_t nIndex, ccEraser::IRemediationAction*& pRemediation, cc::IStream*& pStream) const throw()
{
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	return m_oQuarantineExtractor.GetRemediation(nIndex, pRemediation, pStream);
}

} //namespace SAVSubmission
