////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\addsubmission.h"
#include "ccSymKeyvalueCollectionImpl.h"
#include "Sample.h"
#include "Detection.h"
#include "avqbackuptypes.h"
#include "SubmissionEngineLoader.h"
#include "ccSymStringImpl.h"
#include "AvSubmitRes.h"
#include "Utils.h"

namespace AVSubmit {

CAddSubmission::CAddSubmission(void) : m_bManual(false), m_dwType(IAVSubmissionManager::eAll)
{
	CCTRACEI(CCTRCTX);
}

CAddSubmission::~CAddSubmission(void)
{
	CCTRACEI(CCTRCTX);
}

bool CAddSubmission::Initialize(AVModule::IAVMapBase* pData, cc::IString* pProduct, cc::IString* pVer, cc::IString* pActor)
{
	m_pData = pData;
	m_pStrData = pData;
	m_pProduct = pProduct;
	m_pVersion = pVer;
	m_pActor = pActor;
	return SUCCEEDED(CheckInit());
}

void CAddSubmission::Execute(HANDLE hAbort) throw()
{
	if(::WaitForSingleObject(hAbort, 0) == WAIT_TIMEOUT)
		Process();
}

HRESULT CAddSubmission::Process()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << CheckInit();

		CCTRCTXI1(_T("Flags = 0x%08X"), m_dwType);
		cc::IKeyValueCollectionPtr pAttribSample = CreateKVC();
		cc::IKeyValueCollectionPtr pAttribDetection = CreateKVC();

		if(pAttribSample == NULL || pAttribDetection == NULL)
			hrx << E_OUTOFMEMORY;

		hr = GetThreatInfo(pAttribSample);

		if(hr == HRESULT_FROM_WIN32(ERROR_DS_CHILDREN_EXIST))
		{
			CCTRCTXI1(_T("Create child submissions: %d"), hr);
			return S_OK;
		}
		
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to get threat info: 0x%08X"), hr);
			hrx << hr;
		}

		if(m_bManual)
		{
			hrx << CreateSubmission(pAttribSample, eManual);
		}
		else
		{

			hrx << GetThreatInfo(pAttribDetection);


			HRESULT hrD = E_FAIL;
			if((m_dwType&IAVSubmissionManager::eDetection) == IAVSubmissionManager::eDetection)
				hrD = CreateSubmission(pAttribDetection, eDetection);

			HRESULT hrS = E_FAIL;
			if((m_dwType&IAVSubmissionManager::eSample) == IAVSubmissionManager::eSample)
				hrS = CreateSubmission(pAttribSample, eSample);

			if(FAILED(hrD) && FAILED(hrS) || m_submissions.size() < 1)
			{
				CCTRCTXE3(_T("hrD = 0x%08X, hrS=0x%08X, size=%d"), hrD, hrS, m_submissions.size());
				hrx << HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
			}
		}
		hrx << AddSubmissions();
	}
	CATCH_RETURN(hr);
}

HRESULT CAddSubmission::CreateSubmission(cc::IKeyValueCollection* pData, SubmissionType eType)
{
	HRESULT hr = S_OK;
	CSubmissionBase::Ptr p;
	ccLib::CString sDescription;

	switch(eType)
	{
	case eManual:
	case eSample:
		{
			CSymPtr<CSample> s;
			if(CSample::Allocate(s.m_p))
			{
				p = s;
				sDescription.LoadString(eType == eManual ? IDS_MANUAL : IDS_SAMPLE);
			}
		}
		break;
	case eDetection:
		{
			CSymPtr<CDetection> d;
			if(CDetection::Allocate(d.m_p))
			{
				sDescription.LoadString(IDS_DETECTION);
				p = d;
			}
		}
		break;
	default:
		CCTRCTXE1(_T("Unknown type %d"), eType);
		hr = E_INVALIDARG;
		break;
	}

	if(SUCCEEDED(hr) && p == NULL)
		hr = E_OUTOFMEMORY;

	cc::IStringPtr pDesc;
	pDesc.Attach(ccSym::CStringImpl::CreateStringImpl(sDescription));
	if(pDesc == NULL || !pData->SetValue(clfs::ISubmission::eDescription, pDesc))
		hr = E_OUTOFMEMORY;

	//construction failed
	if(FAILED(hr))
		return hr;


	hr = p->Initialize(pData);
	if(SUCCEEDED(hr))
	{
		clfs::ISubmissionQIPtr pSub = p;
		if(pSub != NULL)
		{
			m_submissions.push_back(SubmissionRec(pSub, eType)); 
		}
		else
		{
			CCTRCTXE0(_T("Failed to QI for ISubmission"));
			hr = E_NOINTERFACE;
		}
	}
	else
		CCTRCTXE1(_T("Init failse: 0x%08X"), hr);
	return hr;
}

HRESULT CAddSubmission::GetThreatInfo(cc::IKeyValueCollection* pAttrib)
{
	utils::BoolX brx;
	STAHLSOFT_HRX_TRY(hr)
	{
		DWORD dwVid = 0;
		cc::IStringPtr pName;
		GUID threatId = {0};
		DWORD dwTime = DWORD(time(NULL));

		hrx << (pAttrib == NULL ? E_INVALIDARG : S_OK);
		if(m_pData != NULL)
		{
			DWORD dwType = DWORD(-1);
			if(FAILED(m_pData->GetValue(AVModule::ThreatTracking::TrackingData_THREAT_TYPE, dwType)))
			{
				CCTRCTXW0(_T("No theat type available"));
				dwType = DWORD(-1);
			}

			//since these keys overlap, we need to check keys and types
			if(dwType == AVModule::ThreatTracking::ThreatType_CleanStealthFile)
			{
				CCTRCTXI0(_T("Clean Stealth File"));
				hrx << m_pData->GetValue(AVModule::ThreatTracking::TrackingData_THREATID, threatId);
				hrx << m_pData->GetValue(AVModule::ThreatTracking::TrackingData_DETECTION_TIME, dwTime);
				dwVid = CSubmissionBase::eVID_VXMS;
				pName.Attach(ccSym::CStringImpl::CreateStringImpl(L"Bloodhound.CC.Rootkit"));
				CCTHROW_BAD_ALLOC(pName);
			}
			else if(CheckType(m_pData, AVModule::AutoProtect::APProcessingComplete_AllThreatsDetected, AVModule::AVDataTypes::eTypeArray))
			{
				ISymBasePtr pBase;
				//first check if there's any threat tracking data available
				CCTRCTXI0(_T("Checking for AP threat tracking data"));
				HRESULT hr2 = m_pData->GetValue(AVModule::AutoProtect::APProcessingComplete_AllThreatsDetected, AVModule::AVDataTypes::eTypeArray, pBase.m_p);
				if(SUCCEEDED(hr2) && pBase != NULL)
				{
					AVModule::IAVArrayDataQIPtr pArray = pBase;
					if(pArray != NULL)
					{
						DWORD dwCount = 0;
						hr2 = pArray->GetCount(dwCount);
						if(SUCCEEDED(hr2) && dwCount > 0)
						{
							CCTRCTXI1(_T("Adding %d AP Samples"), dwCount);
							hrx << AddAPSamples(pArray);
						}
						else
							CCTRCTXW2(_T("No sample data: size = %d, res = 0x%08X"), dwCount, hr2);
					}
					else
						CCTRCTXE0(_T("Failed to QI for array data"));
				}
				else
					CCTRCTXE1(_T("Failed to get AP Data: 0x%08X"), hr2);

				CCTRCTXI0(_T("No threat tracking data: Just Adding AP Detection"));
				hrx << m_pData->GetValue(AVModule::AutoProtect::APProcessingComplete_VID, dwVid);

				//hrx << m_pData->GetValue(AVModule::AutoProtect::APProcessingComplete_FileName, pPath);

			}
			else if(CheckType(m_pData, AVModule::ThreatTracking::TrackingData_VID, AVModule::AVDataTypes::eTypeDWORD))
			{
				CCTRCTXI0(_T("Threat Tracking: VID"));
				hrx << m_pData->GetValue(AVModule::ThreatTracking::TrackingData_VID, dwVid);
				hrx << m_pData->GetValue(AVModule::ThreatTracking::TrackingData_NAME, pName.m_p);
				hrx << m_pData->GetValue(AVModule::ThreatTracking::TrackingData_THREATID, threatId);
				hrx << m_pData->GetValue(AVModule::ThreatTracking::TrackingData_DETECTION_TIME, dwTime);
			}
			else if(!m_bManual && CheckType(m_pData, AVModule::AutoProtect::APBlocked_VID, AVModule::AVDataTypes::eTypeDWORD))
			{
				CCTRCTXI0(_T("Blocked: VID"));

				cc::IStringPtr pPath;
				hrx << m_pData->GetValue(AVModule::AutoProtect::APBlocked_VID, dwVid);
				hrx << m_pData->GetValue(AVModule::AutoProtect::APBlocked_Name, pName.m_p);

				//file name is not relevant, since its gone anyways.
				//hrx << m_pData->GetValue(AVModule::AutoProtect::APBlocked_FileName, pPath.m_p);
				//brx << pAttrib->SetValue(IAVSubmission::eFilePath, pPath);

				//optimization: this is just a detection
				m_dwType = IAVSubmissionManager::eDetection;
			}
			else if(m_bManual && CheckType(m_pData, AVModule::ThreatTracking::TrackingData_THREATID, AVModule::AVDataTypes::eTypeGUID))
			{
				CCTRCTXI0(_T("Manual, TreatID"));
				hrx << m_pData->GetValue(AVModule::ThreatTracking::TrackingData_THREATID, threatId);

				HRESULT hr2 = m_pData->GetValue(AVModule::ThreatTracking::TrackingData_DETECTION_TIME, dwTime);
				if(FAILED(hr2))
				{
					CCTRCTXW1(_T("Failed to get detection time: 0x%08X"), hr2);
				}
				if(CheckType(m_pData, AVModule::ThreatTracking::TrackingData_NAME, AVModule::AVDataTypes::eTypeSTRING))
				{
					hr2 = m_pData->GetValue(AVModule::ThreatTracking::TrackingData_NAME, pName.m_p);
					if(FAILED(hr2))
					{
						CCTRCTXW1(_T("Failed to get threat name: 0x%08X.  Using default"), hr2);
					}
				}
				if(pName == NULL)
				{
					pName.Attach(ccSym::CStringImpl::CreateStringImpl(L"Manual.Sample"));
					CCTHROW_BAD_ALLOC(pName);
				}
			}
			else
			{
				CCTRCTXE0(_T("Threat is not std detection or AP Block"));
				hrx << HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
			}

			if(dwVid == 0 && !m_bManual)
			{
				CCTRCTXE0(_T("Failed to get vid.  not processing"));
				hrx << HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
			}
		}
		else if(m_pStrData != NULL)
		{
			if(m_pStrData->Contains(AVModule::EmailScanner::AV_EMAILOEH_WORMPATH))
			{
				pName.Attach(ccSym::CStringImpl::CreateStringImpl(L"OEH.Worm"));
				CCTHROW_BAD_ALLOC(pName);


				if(m_pStrData->Contains(AVModule::EmailScanner::AV_EMAILOEH_QUARRESULT))
				{
					//TODO: Get Threat Id once available from AVComponent
					hrx << m_pStrData->GetValue(AVModule::EmailScanner::AV_EMAILOEH_QBACKUPID, threatId);
				}
				if(m_pStrData->Contains(AVModule::EmailScanner::AV_EMAILOEH_WORMPATH))
				{
					cc::IStringPtr pPath;
					hrx << m_pStrData->GetValue(AVModule::EmailScanner::AV_EMAILOEH_WORMPATH, pPath.m_p);
					brx << pAttrib->SetValue(IAVSubmission::eFilePath, pPath);
				}
			}
			else if(m_pStrData->Contains(AVModule::EmailScanner::AV_EMAILTHREATSDETECTED_THREATS))
			{
				hrx << AddEmail();
				hrx << HRESULT_FROM_WIN32(ERROR_DS_CHILDREN_EXIST);
			}
			else
			{
				CCTRCTXE0(_T("Unknown string data map contents"));
				hrx << HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
			}
		}
		else
		{
			CCTRCTXE0(_T("No data"));
			hrx << HRESULT_FROM_WIN32(ERROR_NO_DATA);
		}

		CCTRCTXI2(_T("VID %d, name %ls"), dwVid, (pName != NULL ? pName->GetStringW() : L"<null vname>" ));
		CCTRACEI(CCTRCTX _T("Threat ID: ") SYMGUID_FORMAT_STR((&threatId)));
		brx << pAttrib->SetValue(IAVSubmission::eVID, dwVid);
		if(pName != NULL)
			brx << pAttrib->SetValue(IAVSubmission::eVirusName, pName);
		if(threatId != GUID_NULL)
			brx << pAttrib->SetValue(IAVSubmission::eSetId, threatId);
		brx << pAttrib->SetValue(IAVSubmission::eDateDetected, dwTime);
	}
	CATCH_RETURN(hr);
}

HRESULT CAddSubmission::AddSubmissions()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(m_submissions.size() < 1)
		{
			CCTRCTXE0(_T("m_submission.size() < 1"));
			hrx << E_FAIL;
		}
		mgr::IManagerPtr pManager; //controls lifetime of subeng.dll

		clfs::ISubmissionEnginePtr pEngine;
		hrx << GetObjectManager(pManager.m_p);
		hrx << pManager->GetSubmissionEngine(pEngine.m_p);

		for(SubmissionList::iterator i = m_submissions.begin(), e = m_submissions.end(); i!=e; ++i)
		{
			if(i->pSubmission == NULL)
			{
				CCTRCTXW0(_T("i->pSubmission == NULL"));
				continue;
			}

			GUID guid = GUID_NULL;
			HRESULT hr2 = pEngine->Create(i->pSubmission, guid);
			if(FAILED(hr2))
			{
				CCTRCTXI1(_T("Failed to create submission: 0x%08X"), hr2);
				hrx << hr2;
			}

			if(i->eType == eDetection)
				continue;
            
			//could also use dynamic_cast, but that would require RTTI...
			IAVSubmission2QIPtr pAVSubmission = i->pSubmission;
			if(pAVSubmission != NULL)
			{
				CCTRCTXI0(_T("Flagging submission as submitted"));
				hr2 = pAVSubmission->SetSubmitted(true);
				if(FAILED(hr2))
				{
					CCTRCTXE1(_T("Failed to flag as submitted: 0x%08X"), hr2);
					//non-fatal error
				}
			}
			else
			{
				CCTRCTXE0(_T("pAVSubmission != NULL"));
			}
		}
	}
	CATCH_NO_RETURN(hr);
	m_submissions.clear();
	return hr;
}

HRESULT CAddSubmission::AddEmail()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		AVModule::IAVMapGuidDataQIPtr pMapThreats;
		ISymBasePtr pSymBase;
		DWORD dwCount = 0;
		
		hrx << m_pStrData->GetValue(AVModule::EmailScanner::AV_EMAILTHREATSDETECTED_THREATS, AVModule::AVDataTypes::eTypeMapGuid, pSymBase.m_p);
		pMapThreats = pSymBase;
		hrx << (pMapThreats == NULL ? E_POINTER : S_OK);
		hrx << pMapThreats->GetCount(dwCount);

		CCTRCTXI1(_T("%d threats in email"), dwCount);

		DWORD dwAdded = 0;

		for(DWORD i = 0; i < dwCount; ++i)
		{
			pSymBase.Release();
			HRESULT hr2 = pMapThreats->GetValueByIndex(i, AVModule::AVDataTypes::eTypeMapDword, pSymBase.m_p);
			if(FAILED(hr2) || pSymBase == NULL)
			{
				CCTRCTXE2(_T("GetValueByIndex(%d) failed: 0x%08X"), i, hr2);
				continue;
			}

			AVModule::IAVMapDwordDataQIPtr pThreatData = pSymBase;
			if(pThreatData == NULL)
			{
				CCTRCTXE0(_T("pThreatData == NULL"));
				continue;
			}

			CAddSubmission asLocal;
			if(!asLocal.Initialize(pThreatData, m_pProduct, m_pVersion, m_pActor))
			{
				CCTRCTXE0(_T("asLocal.Initialize(pThreatData, m_pProduct, m_pVer, m_pActor"));
				continue;
			}
			hr2 = asLocal.Process();
			if(FAILED(hr2))
			{
				CCTRCTXE1(_T("Failed to process threat at index %d"), i);
				continue;
			}
			++dwAdded;
		}
		if(dwAdded == 0)
			hrx << HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
	}
	CATCH_RETURN(hr);
}

HRESULT CAddSubmission::AddAPSamples(AVModule::IAVArrayData* pData)
{
	HRESULT hr = S_OK;
	for(;;)
	{
		DWORD dwCount = 0;
		hr = pData->GetCount(dwCount);
		if(FAILED(hr) || dwCount < 1)
		{
			CCTRCTXE2(_T("NoData: %d 0x%08X"), dwCount, hr);
			hr = HRESULT_FROM_WIN32(ERROR_NO_DATA);
			break;
		}

		DWORD dwChildren = 0;
		for(DWORD i = 0; i < dwCount; ++i)
		{
			HRESULT hr2 = S_OK;
			ISymBasePtr pBase;
			AVModule::IAVMapDwordDataQIPtr pQI;
			hr2 = pData->GetValue(i, AVModule::AVDataTypes::eTypeMapDword, pBase.m_p);
			if(FAILED(hr2) || pBase == NULL)
			{
				CCTRCTXE2(_T("No threat data @ %d - 0x%08X"), i, hr2);
				continue;
			}

			pQI = pBase;
			if(pQI == NULL)
			{
				CCTRCTXE0(_T("Type mismatch"));
				continue;
			}

			CAddSubmission s0;
			if(!s0.Initialize(pQI, m_pProduct, m_pVersion, m_pActor))
			{
				CCTRCTXE0(_T("Failed to init child detection"));
				continue;
			}

			//s0.SetType(IAVSubmissionManager::eSample);
			hr2 = s0.Process();
			if(FAILED(hr2))
			{
				CCTRCTXE1(_T("Failed to process child: 0x%08X"), hr2);
				continue;
			}
			++dwChildren;
		}

		if(dwChildren > 0)
		{
			CCTRCTXI1(_T("%d children created"), dwChildren);
			hr = HRESULT_FROM_WIN32(ERROR_DS_CHILDREN_EXIST);
			break;
		}

		hr = S_OK;
		CCTRCTXI0(_T("No children added"));
		break;
	}
	return hr;
}

inline HRESULT CAddSubmission::CheckInit() 
{
	if(m_pProduct == NULL || m_pVersion == NULL || m_pActor == NULL)
	{
		CCTRCTXE0(_T("m_pProduct == NULL || m_pVersion == NULL || m_pActor == NULL"));
		return E_INVALIDARG;
	}
	if(m_pData == NULL && m_pStrData == NULL)
	{
		CCTRCTXE0(_T("!m_bManual && m_pData == NULL && m_pStrData == NULL"));
		return E_INVALIDARG;
	}
	CCTRCTXI0(_T("Success"));
	return S_OK;
}

inline cc::IKeyValueCollectionPtr CAddSubmission::CreateKVC() throw(_com_error)
{
	cc::IKeyValueCollectionPtr pAttrib;
	ccSym::CStringImplPtr pString;
	pString.Attach(ccSym::CStringImpl::CreateStringImpl());
	pAttrib.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
	if(pAttrib == NULL || pString == NULL)
		throw _com_error(E_OUTOFMEMORY);
	pAttrib->SetCloneInterface(true);
	pAttrib->SetValue(clfs::ISubmission::eProductName, m_pProduct);
	pAttrib->SetValue(clfs::ISubmission::eProductVersion, m_pVersion);
	pAttrib->SetValue(clfs::ISubmission::eActor, m_pActor);
	pAttrib->SetValue(IAVSubmission::eManual, m_bManual);

	return pAttrib;
}

inline bool CAddSubmission::CheckType(AVModule::IAVMapDwordData* pData, DWORD dwIndex, DWORD dwType)
{
	bool bRes = false;
	for(;;)
	{
		if(!pData->Contains(dwIndex))
			break;

		AVModule::AVDataTypes::eDataTypes eType;
		HRESULT hr = pData->GetItemValueType(dwIndex, eType);
		if(FAILED(hr))
			break;

		if(DWORD(eType) != dwType)
			break;

		bRes = true;
		break;
	}
	return bRes;
}

}