////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "AvProdClientActivity.h"

#include "AVInterfaces.h"
#include "AvInterfaceLoader.h"

#include "ccSerializeInterface.h"
#include "ccSymMemoryStreamIMpl.h"

#include "ccEraserInterface.h"
#include "NAVEventFactoryLoader.h"
#include "AvEvents.h"
#include "AllNAVEvents.h"
#include "AvDefines.h"
#include "AvccModuleId.h"

#include "ccSymModuleLifetimeMgrHelper.h"
#include <ccInstanceFactory.h>
#include "ThreatCatInfo.h"
#include "AvProdWidgets.h"
#include "..\navlogvres\resource.h"
#include "ISVersion.h"

void AvActivityDataFromAvModuleOnApComplete(AVModule::IAVMapDwordData* pAvModuleActivity, CEventData& AvActivity);
void AvOEHDetectionFromAvModuleOEHDetection(AVModule::IAVMapStrData* pOEHInfo, DWORD dwOEHAction, CEventData& AvActivity);
void AvScanCompleteDataFromAvModuleOnApComplete(AVModule::IAVMapStrData* pAvModuleActivity, CEventData& AvActivity);
void AddUserNameToEventData(CEventData& EventData, DWORD dwProperty);
void AddTerminalSvcsSessionIdToEventData(CEventData& EventData, DWORD dwProperty);
void AvActivityDataFromAvModuleOnApThreatBlocked(AVModule::IAVMapDwordData* pAvModuleActivity, CEventData& AvActivity);
void SingleAvModuleThreatDataToAvProdFileData(AVModule::IAVMapDwordData* pAvModuleActivity, AVModule::IAVArrayData* pArrDispInfo, CEventData& AvActivity);
DWORD GetCategoriesStringFromThreatTrackInfo(AVModule::IAVMapDwordData* pThreatTrack, DWORD dwCategoryListIndex, CString& cszCategories);
void AvModuleDisplayInfoToEventData(AVModule::IAVArrayData* pArrDispInfo, DWORD dwPropertyId, CEventData& AvActivity);
void AvSetUniqueID(CEventData& AvActivity);
void AvModuleScanTypeToAvScanType(DWORD dwScanType, DWORD dwPropertyId, CEventData& AvActivity);
void AvModuleScanResultToAvScanResult(DWORD dwResult, DWORD dwPropertyId, CEventData& AvActivity);
void AvScanCompleteEraserStatsToAvProdLog(AVModule::IAVMapStrData* pAvModuleActivity, LPCWSTR wszElementId, CEventData& AvActivity);
bool ThreatHasBeenResolved(AVModule::IAVMapDwordData* pThreatTrackInfo);
void AvScanCompleteToAvProdSummary(GUID& guidScanInstance, CEventData& AvActivity);
ccEraser::IAnomaly::Anomaly_Category GroupCategory(ccEraser::IAnomaly::Anomaly_Category eCat);

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAutoProtectClientActivity::CAutoProtectClientActivity(void)
{
}

CAutoProtectClientActivity::~CAutoProtectClientActivity(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAutoProtectClientActivity::Initialize() throw()
{
	HRESULT hrReturn = E_FAIL;

	// Create the logger object.
	SYMRESULT sr = AV::NAVEventFactory_IAVEventFactory::CreateObject(GETMODULEMGR(), &m_spLogger);
	if(SYM_SUCCEEDED(sr) && m_spLogger)
		hrReturn = S_OK;

	return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAutoProtectClientActivity::OnAutoProtectRemediationComplete(AVModule::IAVMapDwordData* pActivityData) throw()
{
	CEventData AvActivity;

	// Populate event
	AvActivity.SetData( AV::Event_Base_propType, AV::Event_ID_Threat );
	AvActivity.SetData( AV::Event_Threat_propProductName, CISVersion::GetProductName());
	AvActivity.SetData( AV::Event_Threat_propProductVersion, CISVersion::GetPublicRevision());
	AvActivity.SetData( AV::Event_Threat_propFeature, SAVRT_MODULE_ID_NAVAPSVC);
	AvActivity.SetData( AV::Event_Threat_propFeatureVersion, _S(IDS_NOT_APPLICABLE));
	AvActivity.SetData( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File);
	AvActivity.SetData( AV::Event_Threat_propEventSubType, AV::Event_Threat );

	AddUserNameToEventData(AvActivity, AV::Event_Base_propUserName);

	AvActivityDataFromAvModuleOnApComplete(pActivityData, AvActivity);

	AvSetUniqueID(AvActivity);

	// Serialize the AvModule Activity into the event log
	cc::ISerializeQIPtr spSerialize = pActivityData;
	if(spSerialize)
	{
		ccSym::CMemoryStreamImplPtr spStream;
		spStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		if(spStream)
		{
			if(spSerialize->Save(spStream))
			{
				ULONGLONG qdwLength;
				spStream->GetSize(qdwLength);
				AvActivity.SetData(AV::Event_Threat_propAvModuleData, (LPBYTE)spStream->GetMemory().GetMemory(), (LONG)qdwLength);
			}
		}

	}

	if(!m_spLogger)
	{
		Initialize();
	}

	if(m_spLogger)
		m_spLogger->BroadcastAvEvent(AvActivity);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CAutoProtectClientActivity::OnAutoProtectThreatBlocked(AVModule::IAVMapDwordData* pActivityData) throw()
{
	CEventData AvActivity;

	// Populate event
	AvActivity.SetData( AV::Event_Base_propType, AV::Event_ID_Threat );
	AvActivity.SetData( AV::Event_Threat_propProductName, CISVersion::GetProductName());
	AvActivity.SetData( AV::Event_Threat_propProductVersion, CISVersion::GetPublicRevision());
	AvActivity.SetData( AV::Event_Threat_propFeature, SAVRT_MODULE_ID_NAVAPSVC);
	AvActivity.SetData( AV::Event_Threat_propFeatureVersion, _S(IDS_NOT_APPLICABLE));
	AvActivity.SetData( AV::Event_Threat_propDefsRevision, _S(IDS_NOT_APPLICABLE));
	AvActivity.SetData( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File);
	AvActivity.SetData( AV::Event_Threat_propEventSubType, AV::Event_APBlockedThreat );

	AddUserNameToEventData(AvActivity, AV::Event_Base_propUserName);

	AvActivityDataFromAvModuleOnApThreatBlocked(pActivityData, AvActivity);

	AvSetUniqueID(AvActivity);

	// Serialize the AvModule Activity into the event log
	cc::ISerializeQIPtr spSerialize = pActivityData;
	if(spSerialize)
	{
		ccSym::CMemoryStreamImplPtr spStream;
		spStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		if(spStream)
		{
			if(spSerialize->Save(spStream))
			{
				ULONGLONG qdwLength;
				spStream->GetSize(qdwLength);
				AvActivity.SetData(AV::Event_Threat_propAvModuleData, (LPBYTE)spStream->GetMemory().GetMemory(), (LONG)qdwLength);
			}
		}

	}

	if(!m_spLogger)
	{
		Initialize();
	}

	if(m_spLogger)
		m_spLogger->BroadcastAvEvent(AvActivity);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CScanClientActivityBase::CScanClientActivityBase(void)
{
}

//****************************************************************************
//****************************************************************************
CScanClientActivityBase::~CScanClientActivityBase(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CScanClientActivityBase::Initialize() throw()
{
	// Create the logger object.
	SYMRESULT sr = SYMERR_UNKNOWN;
	sr = AV::NAVEventFactory_IAVEventFactory::CreateObject(GETMODULEMGR(), &m_spLogger);
	if(SYM_FAILED(sr) || !m_spLogger)
		return E_FAIL;

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanClientActivityBase::LogThreatsFound(const REFGUID guidScanInstance)
{
	StahlSoft::HRX hrx;
	ccLib::CExceptionInfo exceptionInfo;
	try
	{
		AVModule::AVLoader_IAVThreatInfo AvThreatInfoLoader;
		AvThreatInfoLoader.Initialize();

		AVModule::IAVThreatInfoPtr spThreatInfo;
		AvThreatInfoLoader.CreateObject(spThreatInfo);
		if(!spThreatInfo)
		{
			CCTRACEW( CCTRCTX _T("AvModuleLoader::CreateObject() failed."));
			hrx << E_UNEXPECTED;
		}

		AVModule::AVLoader_IAVMapDwordData AVMapDwordDataLoader;
		AVModule::IAVMapDwordDataPtr spFilter;
		AVMapDwordDataLoader.CreateObject(spFilter);
		if(!spFilter)
		{
			CCTRACEW( CCTRCTX _T("AVLoader_IAVMapDwordData::CreateObject() failed."));
			hrx << E_UNEXPECTED;
		}

		// Only get resolved threats
		spFilter->SetValue(AVModule::ThreatTracking::GetThreatBy_SCANINSTANCE, guidScanInstance);

		AVModule::IAVArrayDataPtr spThreatData;
		hrx << spThreatInfo->GetThreatsWithFilter(spFilter, spThreatData);

		DWORD dwThreatInfoCount = NULL;
		hrx << spThreatData->GetCount(dwThreatInfoCount);

		for(DWORD dwThreatInfoIndex = 0; dwThreatInfoIndex < dwThreatInfoCount; dwThreatInfoIndex++)
		{
			HRESULT hr;
			ISymBasePtr spTempData;
			hr = spThreatData->GetValue(dwThreatInfoIndex, AVModule::AVDataTypes::eTypeMapDword, spTempData);
			if(FAILED(hr) || !spTempData)
				continue;

			AVModule::IAVMapDwordDataQIPtr spMapCurThreat = spTempData;
			if(!spMapCurThreat)
				continue;

			GUID guidThreatId = GUID_NULL;
			hr = spMapCurThreat->GetValue(AVModule::ThreatTracking::TrackingData_THREATID, guidThreatId);
			if(FAILED(hr) || !spTempData)
			{
				CCTRACEE( CCTRCTX _T("Unable to retrieve threat id from threat track info. hr = 0x%08X"), hr);
				continue;
			}

			DWORD dwThreatType = NULL;
			hr = spMapCurThreat->GetValue(AVModule::ThreatTracking::TrackingData_THREAT_TYPE, dwThreatType);
			if(SUCCEEDED(hr) && AVModule::ThreatTracking::ThreatType_Anomaly == dwThreatType)
			{
				AVModule::IAVArrayDataPtr spArrDispInfo;
				hr = spThreatInfo->GetAnomalyDisplayData(guidThreatId, false, spArrDispInfo);
				if(FAILED(hr) || !spArrDispInfo)
				{
					CCTRACEE( CCTRCTX _T("Unable to retrieve display info for threat. hr = 0x%08X"), hr);
					// non fatal
				}

				LogThreatFound(spMapCurThreat, spArrDispInfo);
			}
			else
				LogThreatFound(spMapCurThreat);
		}

	}
	CCCATCHMEM(exceptionInfo)
	CCCATCHCOM(exceptionInfo);

	if(exceptionInfo.IsException())
		return E_FAIL;

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanClientActivityBase::LogThreatFound(AVModule::IAVMapDwordData* pActivityData, AVModule::IAVArrayData* pArrDispInfo) throw()
{
	CEventData AvActivity;

	// Populate event
	AvActivity.SetData( AV::Event_Base_propType, AV::Event_ID_Threat );
	AvActivity.SetData( AV::Event_Threat_propProductName, CISVersion::GetProductName());
	AvActivity.SetData( AV::Event_Threat_propProductVersion, CISVersion::GetPublicRevision());
	AvActivity.SetData( AV::Event_Threat_propFeature, m_lFeature);
	AvActivity.SetData( AV::Event_Threat_propFeatureVersion, _S(IDS_NOT_APPLICABLE));
	AvActivity.SetData( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File);
	AvActivity.SetData( AV::Event_Threat_propEventSubType, AV::Event_Threat );

	AddUserNameToEventData(AvActivity, AV::Event_Base_propUserName);

	SingleAvModuleThreatDataToAvProdFileData(pActivityData, pArrDispInfo, AvActivity);
	AvModuleDisplayInfoToEventData(pArrDispInfo, AV::Event_Threat_propRemediationData, AvActivity);

	AvSetUniqueID(AvActivity);

	// Serialize the AvModule Activity into the event log
	cc::ISerializeQIPtr spSerialize = pActivityData;
	if(spSerialize)
	{
		ccSym::CMemoryStreamImplPtr spStream;
		spStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		if(spStream)
		{
			if(spSerialize->Save(spStream))
			{
				ULONGLONG qdwLength;
				spStream->GetSize(qdwLength);
				AvActivity.SetData(AV::Event_Threat_propAvModuleData, (LPBYTE)spStream->GetMemory().GetMemory(), (LONG)qdwLength);
			}
		}
		spSerialize.Release();
	}

	// Serialize the AvModule Activity into the event log
	spSerialize = pArrDispInfo;
	if(spSerialize)
	{
		ccSym::CMemoryStreamImplPtr spStream;
		spStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		if(spStream)
		{
			if(spSerialize->Save(spStream))
			{
				ULONGLONG qdwLength;
				spStream->GetSize(qdwLength);
				AvActivity.SetData(AV::Event_Threat_propAvModuleDispInfo, (LPBYTE)spStream->GetMemory().GetMemory(), (LONG)qdwLength);
			}
		}
		spSerialize.Release();
	}

	if(!m_spLogger)
	{
		Initialize();
	}

	if(m_spLogger)
		m_spLogger->BroadcastAvEvent(AvActivity);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CManualScanClientActivity::CManualScanClientActivity(void)
{
	m_lFeature = AV_MODULE_ID_NAVW;
}

//****************************************************************************
//****************************************************************************
CManualScanClientActivity::~CManualScanClientActivity(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CManualScanClientActivity::Initialize() throw()
{
	HRESULT hr = CScanClientActivityBase::Initialize();
	if(FAILED(hr))
		return hr;

	AvSetUniqueID(m_AvActivity);

	// Populate event
	m_AvActivity.SetData( AV::Event_Base_propType, AV::Event_ID_ScanAction );
	m_AvActivity.SetData( AV::Event_ScanAction_propProductName, _S(IDS_NOT_APPLICABLE));
	m_AvActivity.SetData( AV::Event_ScanAction_propProductVersion, _S(IDS_NOT_APPLICABLE));
	m_AvActivity.SetData( AV::Event_ScanAction_propFeature, m_lFeature);
	m_AvActivity.SetData( AV::Event_ScanAction_propFeatureVersion, _S(IDS_NOT_APPLICABLE));
	m_AvActivity.SetData( AV::Event_ScanAction_propDefsRevision, _S(IDS_NOT_APPLICABLE));
	m_AvActivity.SetData( AV::Event_ScanAction_propAction, AV::Event_ScanAction_ScanCompleted);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CManualScanClientActivity::Commit() throw()
{
	if(!m_spLogger)
		return E_UNEXPECTED;

	m_spLogger->BroadcastAvEvent(m_AvActivity);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CManualScanClientActivity::SetScanResultByActivityData(AVModule::IAVMapStrData* pResults) throw()
{
	if(!pResults)
		return S_FALSE;

	AddUserNameToEventData(m_AvActivity, AV::Event_Base_propUserName);

	AvScanCompleteDataFromAvModuleOnApComplete(pResults, m_AvActivity);

	// Serialize the AvModule Activity into the event log
	cc::ISerializeQIPtr spSerialize = pResults;
	if(spSerialize)
	{
		ccSym::CMemoryStreamImplPtr spStream;
		spStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		if(spStream)
		{
			if(spSerialize->Save(spStream))
			{
				ULONGLONG qdwLength;
				spStream->GetSize(qdwLength);
				m_AvActivity.SetData(AV::Event_ScanAction_propAvModuleData, (LPBYTE)spStream->GetMemory().GetMemory(), (LONG)qdwLength);
			}
		}

	}

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CManualScanClientActivity::SetScanResultByProperty(DWORD dwPropertyId, DWORD dwValue) throw()
{
	m_AvActivity.SetData(dwPropertyId, dwValue);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CManualScanClientActivity::SetScanType(DWORD dwScanType) throw()
{
	AvModuleScanTypeToAvScanType(dwScanType, AV::Event_ScanAction_propTaskName, m_AvActivity);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CManualScanClientActivity::SetScanType(LPCWSTR wszScanType) throw()
{
	m_AvActivity.SetData(AV::Event_ScanAction_propTaskName, wszScanType);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CManualScanClientActivity::SetScanResult(DWORD dwResult) throw()
{
	AvModuleScanResultToAvScanResult(dwResult, AV::Event_ScanAction_propAction, m_AvActivity);
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CManualScanClientActivity::SetScanResultByCategory(DWORD dwPropertyId, DWORD dwCategoryId, DWORD dwValue) throw()
{
	CEventData ResultsByCategory;
	m_AvActivity.GetNode(dwPropertyId, ResultsByCategory);

	ResultsByCategory.SetData(dwCategoryId, dwValue);
	m_AvActivity.SetNode(dwPropertyId, ResultsByCategory);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CManualScanClientActivity::SetEraserScanResults(DWORD dwCategoryId, DWORD dwValue) throw()
{
	CEventData EraserScanResults;
	m_AvActivity.GetNode(AV::Event_ScanAction_EraserScanResults, EraserScanResults);

	EraserScanResults.SetData(dwCategoryId, dwValue);
	m_AvActivity.SetNode(AV::Event_ScanAction_EraserScanResults, EraserScanResults);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CManualScanClientActivity::SetScanInstanceId(REFGUID guidScanInstance) throw()
{
	m_AvActivity.SetData(AV::Event_Base_propUniqueID, (LPBYTE)&guidScanInstance, sizeof(GUID));

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CManualScanClientActivity::LogManualScanResults(REFGUID guidScanInstance) throw()
{
	LogThreatsFound(guidScanInstance);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CEmailScanClientActivity::CEmailScanClientActivity(void)
{
	m_lFeature = AV_MODULE_ID_EMAIL_SCAN;
}

//****************************************************************************
//****************************************************************************
CEmailScanClientActivity::~CEmailScanClientActivity(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CEmailScanClientActivity::Initialize() throw()
{
	HRESULT hrReturn = E_FAIL;

	// Create the logger object.
	SYMRESULT sr = AV::NAVEventFactory_IAVEventFactory::CreateObject(GETMODULEMGR(), &m_spLogger);
	if(SYM_SUCCEEDED(sr) && m_spLogger)
		hrReturn = S_OK;

	return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CEmailScanClientActivity::OnEmailThreatsDetected(AVModule::IAVMapStrData* pActivityData) throw()
{

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CEmailScanClientActivity::OnOEHDetection(AVModule::IAVMapStrData* pOEHInfo, DWORD dwOEHAction) throw()
{
	CEventData AvActivity;

	// Populate event
	AvActivity.SetData( AV::Event_Base_propType, AV::Event_ID_Threat );
	AvActivity.SetData( AV::Event_Threat_propProductName, CISVersion::GetProductName());
	AvActivity.SetData( AV::Event_Threat_propProductVersion, CISVersion::GetPublicRevision());
	AvActivity.SetData( AV::Event_Threat_propFeature, AV_MODULE_ID_OEH_SCAN);
	//    AvActivity.SetData( AV::Event_Threat_propFeatureVersion, _S(IDS_NOT_APPLICABLE));
	AvActivity.SetData( AV::Event_Threat_propDefsRevision, _S(IDS_NOT_APPLICABLE));
	AvActivity.SetData( AV::Event_Threat_propObjectType, AV::Event_Threat_ObjectType_File);
	AvActivity.SetData( AV::Event_Threat_propEventSubType, AV::Event_Threat );

	AddUserNameToEventData(AvActivity, AV::Event_Base_propUserName);

	AvOEHDetectionFromAvModuleOEHDetection(pOEHInfo, dwOEHAction, AvActivity);

	AvSetUniqueID(AvActivity);

	// Serialize the AvModule Activity into the event log
	cc::ISerializeQIPtr spSerialize = pOEHInfo;
	if(spSerialize)
	{
		ccSym::CMemoryStreamImplPtr spStream;
		spStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		if(spStream)
		{
			if(spSerialize->Save(spStream))
			{
				ULONGLONG qdwLength;
				spStream->GetSize(qdwLength);
				AvActivity.SetData(AV::Event_Threat_propAvModuleData, (LPBYTE)spStream->GetMemory().GetMemory(), (LONG)qdwLength);
			}
		}

	}

	if(!m_spLogger)
	{
		Initialize();
	}

	if(m_spLogger)
		m_spLogger->BroadcastAvEvent(AvActivity);

	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CEmailScanClientActivity::OnEmailScanComplete(AVModule::IAVMapStrData* pResults) throw()
{
	CEventData AvActivity;

	// Populate event
	AvActivity.SetData( AV::Event_Base_propType, AV::Event_ID_ScanAction );
	AvActivity.SetData( AV::Event_ScanAction_propProductName, _S(IDS_NOT_APPLICABLE));
	AvActivity.SetData( AV::Event_ScanAction_propProductVersion, _S(IDS_NOT_APPLICABLE));
	AvActivity.SetData( AV::Event_ScanAction_propFeature, AV_MODULE_ID_EMAIL_SCAN);
	AvActivity.SetData( AV::Event_ScanAction_propFeatureVersion, _S(IDS_NOT_APPLICABLE));
	AvActivity.SetData( AV::Event_ScanAction_propDefsRevision, _S(IDS_NOT_APPLICABLE));
	AvActivity.SetData( AV::Event_ScanAction_propAction, AV::Event_ScanAction_ScanCompleted);

	AddUserNameToEventData(AvActivity, AV::Event_Base_propUserName);

	AvScanCompleteDataFromAvModuleOnApComplete(pResults, AvActivity);

	AvSetUniqueID(AvActivity);

	// Serialize the AvModule Activity into the event log
	cc::ISerializeQIPtr spSerialize = pResults;
	if(spSerialize)
	{
		ccSym::CMemoryStreamImplPtr spStream;
		spStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		if(spStream)
		{
			if(spSerialize->Save(spStream))
			{
				ULONGLONG qdwLength;
				spStream->GetSize(qdwLength);
				AvActivity.SetData(AV::Event_ScanAction_propAvModuleData, (LPBYTE)spStream->GetMemory().GetMemory(), (LONG)qdwLength);
			}
		}

	}

	if(!m_spLogger)
	{
		Initialize();
	}

	if(m_spLogger)
		m_spLogger->BroadcastAvEvent(AvActivity);
	
	return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CEmailScanClientActivity::LogEmailScanResults(REFGUID guidScanInstance) throw()
{
	LogThreatsFound(guidScanInstance);

	return S_OK;
}


//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CThreatDataEz::CThreatDataEz(void)
{
}

CThreatDataEz::~CThreatDataEz(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CThreatDataEz::GetCategoriesString(AVModule::IAVMapDwordData* pThreatTrackInfo, cc::IString*& pCategoriesString) throw()
{
	CString cszCategories;
	GetCategoriesStringFromThreatTrackInfo(pThreatTrackInfo, AVModule::ThreatTracking::TrackingData_THREATCATEGORIES, cszCategories);

	cc::IStringPtr spCategoriesString;
	spCategoriesString.Attach((cc::IString*)ccLib::CInstanceFactory::CreateStringImpl());
	if(!spCategoriesString)
		return E_FAIL;

	CThreatCatInfo ThreatInfo;
	TCHAR szCategoryText[512] = {NULL};
	ThreatInfo.GetCategoryText ( cszCategories, szCategoryText, 512 );

	spCategoriesString->SetString(szCategoryText);

	SYMRESULT sr = spCategoriesString->QueryInterface(cc::IID_String, (void**)&pCategoriesString);
	return AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
//
// This is all temp code, just to get the feature up and running.
//  I'll refactor, someday...
//

void AddUserNameToEventData(CEventData& EventData, DWORD dwProperty)
{
	BOOL bRet;
	CString cszUserName; DWORD dwUserNameSize = 100;
	bRet = GetUserName(cszUserName.GetBuffer(dwUserNameSize), &dwUserNameSize);
	cszUserName.ReleaseBuffer();
	if(bRet)
	{
		EventData.SetData( dwProperty, cszUserName);
	}

	return;
}

void AddTerminalSvcsSessionIdToEventData(CEventData& EventData, DWORD dwProperty)
{
	// not implemented
	return;
}


void AvStrMapToAvEventData_STRING(AVModule::IAVMapStrData* pAvStrMap, LPCWSTR wszAvModuleDataId, CEventData& AvEventData, DWORD dwAvProdDataId)
{
	cc::IStringPtr spTempString;

	HRESULT hr = pAvStrMap->GetValue(wszAvModuleDataId, spTempString);
	if(SUCCEEDED(hr) && spTempString)
	{
		AvEventData.SetData(dwAvProdDataId, spTempString->GetStringW());
	}

	return;
}

void AvDwordMapToAvEventData_STRING(AVModule::IAVMapDwordData* pAvDwordMap, DWORD dwAvModuleDataId, CEventData& AvEventData, DWORD dwAvProdDataId)
{
	cc::IStringPtr spTempString;

	HRESULT hr = pAvDwordMap->GetValue(dwAvModuleDataId, spTempString);
	if(SUCCEEDED(hr) && spTempString)
	{
		AvEventData.SetData(dwAvProdDataId, spTempString->GetStringW());
	}

	return;
}

void AvDwordMapToAvEventData_DWORD(AVModule::IAVMapDwordData* pAvDwordMap, DWORD dwAvModuleDataId, CEventData& AvEventData, DWORD dwAvProdDataId)
{
	DWORD dwTemp;

	HRESULT hr = pAvDwordMap->GetValue(dwAvModuleDataId, dwTemp);
	if(SUCCEEDED(hr))
	{
		AvEventData.SetData(dwAvProdDataId, dwTemp);
	}

	return;
}

void AvDwordMapToAvEventData_DWORD2STRING(AVModule::IAVMapDwordData* pAvDwordMap, DWORD dwAvModuleDataId, CEventData& AvEventData, DWORD dwAvProdDataId)
{
	DWORD dwTemp;
	HRESULT hr = pAvDwordMap->GetValue(dwAvModuleDataId, dwTemp);
	if(SUCCEEDED(hr))
	{
		CString cszNumber;
		cszNumber.Format(_T("%d"), dwTemp);
		AvEventData.SetData(dwAvProdDataId, (LPCWSTR)cszNumber);
	}

	return;
}

void AvStringMapToAvEventData_DWORD(AVModule::IAVMapStrData* pAvStringMap, LPCWSTR wszAvModuleDataId, CEventData& AvEventData, DWORD dwAvProdDataId)
{
	DWORD dwTemp;

	HRESULT hr = pAvStringMap->GetValue(wszAvModuleDataId, dwTemp);
	if(SUCCEEDED(hr))
	{
		AvEventData.SetData(dwAvProdDataId, dwTemp);
	}

	return;
}

void XferThreatTrackThreatMatrixToThreatData(AVModule::IAVMapDwordData* pThreatTrack, CEventData& eventThreatData)
{
	HRESULT hr;
	ISymBasePtr spTempObj;
	hr = pThreatTrack->GetValue(AVModule::ThreatTracking::TrackingData_INDIVIDUAL_THREAT_LEVELS, AVModule::AVDataTypes::eTypeArray, spTempObj);
	if(FAILED(hr) || !spTempObj)
		return;

	AVModule::IAVMapDwordDataQIPtr spMapCurThreat = spTempObj;
	if(!spTempObj)
		return;

	AvDwordMapToAvEventData_DWORD2STRING(pThreatTrack, ccEraser::IAnomaly::Stealth, eventThreatData, AV::Event_ThreatEntry_propThreatMatrixStealth);
	AvDwordMapToAvEventData_DWORD2STRING(pThreatTrack, ccEraser::IAnomaly::Privacy, eventThreatData, AV::Event_ThreatEntry_propThreatMatrixPrivacy);
	AvDwordMapToAvEventData_DWORD2STRING(pThreatTrack, ccEraser::IAnomaly::Performance, eventThreatData, AV::Event_ThreatEntry_propThreatMatrixPerformance);
	AvDwordMapToAvEventData_DWORD2STRING(pThreatTrack, ccEraser::IAnomaly::Removal, eventThreatData, AV::Event_ThreatEntry_propThreatMatrixRemoval);
	AvDwordMapToAvEventData_DWORD2STRING(pThreatTrack, ccEraser::IAnomaly::Dependency, eventThreatData, AV::Event_ThreatEntry_propThreatMatrixDependency);

	return;
}

DWORD GetCategoriesStringFromThreatTrackInfo(AVModule::IAVMapDwordData* pThreatTrack, DWORD dwCategoryListIndex, CString& cszCategories)
{
	HRESULT hr;

	ISymBasePtr spTempObj;
	hr = pThreatTrack->GetValue(dwCategoryListIndex, AVModule::AVDataTypes::eTypeArray, spTempObj);
	if(FAILED(hr) || !spTempObj)
		return 0;

	AVModule::IAVArrayDataQIPtr spCategories = spTempObj;
	if(!spCategories)
		return 0;

	// Fill in the threat categories
	DWORD dwCatCount = 0;
	spCategories->GetCount(dwCatCount);
	for(DWORD dwCatIndex=0; dwCatIndex < dwCatCount; dwCatIndex++ )
	{
		DWORD dwCurCategory = 0;
		hr = spCategories->GetValue(dwCatIndex, dwCurCategory);
		if(SUCCEEDED(hr))
		{
			CString cszTemp;
			cszTemp.Format(_T("%u "), dwCurCategory);
			cszCategories += cszTemp;
		}
	}

	return dwCatIndex;
}

void XferCategoryListToThreatDataCategories(AVModule::IAVMapDwordData* pThreatTrack, DWORD dwCategoryListIndex, CEventData& eventThreatData)
{

	CString cszCategories;
	DWORD dwCategoryCount = GetCategoriesStringFromThreatTrackInfo(pThreatTrack, dwCategoryListIndex, cszCategories);

	if(!dwCategoryCount)
		return;

	eventThreatData.SetData ( AV::Event_ThreatEntry_propThreatCatCount, dwCategoryCount );
	eventThreatData.SetData ( AV::Event_ThreatEntry_propThreatCategories, (LPCTSTR)cszCategories );

	return;
}

void XferThreatTrackResultToThreatDataResult(AVModule::IAVMapDwordData* pThreatTrack, CEventData& eventThreatData)
{
	HRESULT hr;
	DWORD dwThreatTrackResult = NULL;
	hr = pThreatTrack->GetValue(AVModule::ThreatTracking::TrackingData_CURRENT_STATE, dwThreatTrackResult);
	if(SUCCEEDED(hr))
	{
		DWORD dwAvProdResult;
		DWORD dwStillPresent = 1;


		if(AVModule::ThreatTracking::ThreatRemoved(dwThreatTrackResult))
		{
			switch (dwThreatTrackResult)
			{
				/*The fact that the VC 7.1 compiler allowed this was a bug that we fixed for
				the 2005 version. Enums are not scope qualified according to both the C and
				C++ standards.

				Ronald Laeremans
				Visual C++ team*/
			case	AVModule::ThreatTracking::ThreatState_FullyRemoved:
				dwAvProdResult = AV::Event_Action_FullyRemoved;
				break;
			case	AVModule::ThreatTracking::ThreatState_PartiallyRemoved:
				dwAvProdResult = AV::Event_Action_PartiallyRemoved;
				break;
			case	AVModule::ThreatTracking::ThreatState_Excluded:
				dwAvProdResult = AV::Event_Action_Excluded;
				break;
			default:
				dwAvProdResult = AV::Event_Action_Unknown;
				break;
			}   
			dwStillPresent = 0;
		}
		else if(AVModule::ThreatTracking::ThreatNotRemoved(dwThreatTrackResult) && !AVModule::ThreatTracking::RemoveFailure(dwThreatTrackResult))
		{
			switch (dwThreatTrackResult)
			{
			case	AVModule::ThreatTracking::ThreatState_RemoveNotAttempted:
				dwAvProdResult = AV::Event_Action_RemoveNotAttempted;
				break;
			case	AVModule::ThreatTracking::ThreatState_CompressedInProcessing:
				dwAvProdResult = AV::Event_Action_CompressedInProcessing;
				break;
			case	AVModule::ThreatTracking::ThreatState_BackupOnly:
				dwAvProdResult = AV::Event_Action_BackupOnly;
				break;
			default:
				dwAvProdResult = AV::Event_Action_NoActionTaken;
				break;
			}

		}
		else if(AVModule::ThreatTracking::ThreatNotRemoved(dwThreatTrackResult) && AVModule::ThreatTracking::RemoveFailure(dwThreatTrackResult))
		{
			switch (dwThreatTrackResult)
			{
			case	AVModule::ThreatTracking::ThreatState_RemoveFailed:
				dwAvProdResult = AV::Event_Action_DeleteFailed;
				break;
			case	AVModule::ThreatTracking::ThreatState_DoNotDelete:
				dwAvProdResult = AV::Event_Action_DoNotDelete;
				break;
			case	AVModule::ThreatTracking::ThreatState_RemoveOnRebootFailed:
				dwAvProdResult = AV::Event_Action_RemoveOnRebootFailed;
				break;
			case	AVModule::ThreatTracking::ThreatState_CannotRemediate:
				dwAvProdResult = AV::Event_Action_CannotRemediate;
				break;
			default:
				dwAvProdResult = AV::Event_Action_Unknown;
				break;
			}
		}
		else
		{
			dwAvProdResult = AV::Event_Action_Unknown;
		}

		BYTE byAction = (BYTE)dwAvProdResult;
		eventThreatData.SetData ( AV::Event_ThreatEntry_propActionCount, 1 );
		eventThreatData.SetData ( AV::Event_ThreatEntry_propActionData, &byAction, 1 );
		eventThreatData.SetData ( AV::Event_ThreatEntry_propStillPresent, dwStillPresent);
	}
}

void AvModuleThreatTrackingToAvProdFileData(AVModule::IAVMapDwordData* pAvModuleActivity, DWORD dwThreatTrackDataId, CEventData& eventFileData)
{
	for(;;)
	{
		HRESULT hr;
		ISymBasePtr spTempObj;
		hr = pAvModuleActivity->GetValue(dwThreatTrackDataId, AVModule::AVDataTypes::eTypeArray, spTempObj);
		if(FAILED(hr) || !spTempObj)
			break;

		AVModule::IAVArrayDataQIPtr spAvModuleAllThreats = spTempObj;
		if(!spAvModuleAllThreats)
			break;

		spTempObj.Release();

		DWORD dwThreatEntryCount;
		hr = spAvModuleAllThreats->GetCount(dwThreatEntryCount);
		if(FAILED(hr))
		{
			CCTRACEE( CCTRCTX _T("IAVMapDwordData::GetCount() failed.  Unable to retrieve threat count. hr=0x%08X"), hr);
			break;
		}

		if(0 == dwThreatEntryCount)
		{
			CCTRACEE( CCTRCTX _T("No threat entry records!!"));
			break;
		}

		CEventData eventAllThreats;

		DWORD dwThreatEntryIndex;
		for(dwThreatEntryIndex = 0; dwThreatEntryIndex < dwThreatEntryCount; dwThreatEntryIndex++)
		{
			CEventData eventThreatData;
			ISymBasePtr spTempData;
			hr = spAvModuleAllThreats->GetValue(dwThreatEntryIndex, AVModule::AVDataTypes::eTypeMapDword, spTempData);
			if(FAILED(hr) || !spTempData)
				continue;

			AVModule::IAVMapDwordDataQIPtr spMapCurThreat = spTempData;
			if(!spMapCurThreat)
				continue;

			AvDwordMapToAvEventData_STRING(spMapCurThreat, AVModule::ThreatTracking::TrackingData_NAME, eventThreatData, AV::Event_ThreatEntry_propVirusName);
			AvDwordMapToAvEventData_DWORD(spMapCurThreat, AVModule::ThreatTracking::TrackingData_VID, eventThreatData, AV::Event_ThreatEntry_propVirusID);
			AvDwordMapToAvEventData_DWORD2STRING(spMapCurThreat, AVModule::ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, eventThreatData, AV::Event_ThreatEntry_propThreatMatrixOverall);

			XferThreatTrackThreatMatrixToThreatData(spMapCurThreat, eventThreatData);
			XferCategoryListToThreatDataCategories(spMapCurThreat, AVModule::ThreatTracking::TrackingData_THREATCATEGORIES, eventThreatData);
			XferThreatTrackResultToThreatDataResult(spMapCurThreat, eventThreatData);

			eventAllThreats.SetNode(0, eventThreatData);
		}

		eventFileData.SetNode(AV::Event_ThreatSubFile_propThreatData, eventAllThreats);
		break;
	}

	return;
}

void XFerFirstValueFromThreatArray(AVModule::IAVMapDwordData* pAvModuleActivity, DWORD dwThreatTrackDataId, DWORD dwThreatTrackValueId, CEventData& AvActivity, LONG lEventDataId)
{
	for(;;)
	{
		HRESULT hr;
		ISymBasePtr spTempObj;
		hr = pAvModuleActivity->GetValue(dwThreatTrackDataId, AVModule::AVDataTypes::eTypeArray, spTempObj);
		if(FAILED(hr) || !spTempObj)
			break;

		AVModule::IAVArrayDataQIPtr spAvModuleAllThreats = spTempObj;
		if(!spAvModuleAllThreats)
			break;

		spTempObj.Release();

		DWORD dwThreatEntryCount;
		hr = spAvModuleAllThreats->GetCount(dwThreatEntryCount);
		if(FAILED(hr))
		{
			CCTRACEE( CCTRCTX _T("IAVMapDwordData::GetCount() failed.  Unable to retrieve threat count. hr=0x%08X"), hr);
			break;
		}

		if(0 == dwThreatEntryCount)
		{
			CCTRACEE( CCTRCTX _T("No threat entry records!!"));
			break;
		}

		CEventData eventAllThreats;

		DWORD dwThreatEntryIndex;
		for(dwThreatEntryIndex = 0; dwThreatEntryIndex < dwThreatEntryCount; dwThreatEntryIndex++)
		{
			CEventData eventThreatData;
			ISymBasePtr spTempData;
			hr = spAvModuleAllThreats->GetValue(dwThreatEntryIndex, AVModule::AVDataTypes::eTypeMapDword, spTempData);
			if(FAILED(hr) || !spTempData)
				continue;

			AVModule::IAVMapDwordDataQIPtr spMapCurThreat = spTempData;
			if(!spMapCurThreat)
				continue;

			AvDwordMapToAvEventData_STRING(spMapCurThreat, dwThreatTrackValueId, AvActivity, lEventDataId);
			break;
		}

		break;
	}
}

void AvActivityDataFromAvModuleOnApComplete(AVModule::IAVMapDwordData* pAvModuleActivity, CEventData& AvActivity)
{
	XFerFirstValueFromThreatArray(pAvModuleActivity, AVModule::AutoProtect::APProcessingComplete_AllThreatsDetected, AVModule::ThreatTracking::TrackingData_VirusDefsVersion, AvActivity, AV::Event_Threat_propDefsRevision);

	CEventData eventFileData;
	AvDwordMapToAvEventData_STRING(pAvModuleActivity, AVModule::AutoProtect::APProcessingComplete_FileName, eventFileData, AV::Event_ThreatSubFile_propFileName);
	AvModuleThreatTrackingToAvProdFileData(pAvModuleActivity, AVModule::AutoProtect::APProcessingComplete_AllThreatsDetected, eventFileData);

	CEventData eventAllFiles;
	eventAllFiles.SetNode(0, eventFileData);

	AvActivity.SetNode(AV::Event_Threat_propSubFileData, eventAllFiles);

	return;
}

void XferFirstInfectedFile(AVModule::IAVArrayData* pArrDispInfo, DWORD dwAvDataId, CEventData& FileData)
{
	if(!pArrDispInfo)
		return;

	DWORD dwDispInfoCount = 0;
	if(pArrDispInfo->GetCount(dwDispInfoCount)!= S_OK)
	{
		CCTRACEE(_T("In XferFirstInfectedFile(), unable to retrieve the array size.  Unexpected."));
		return;
	}

	CString cszAlternateFileName;

	for(DWORD dwDispInfoIndex = 0; dwDispInfoIndex < dwDispInfoCount; dwDispInfoIndex++)
	{
		HRESULT hr;
		ISymBasePtr spTempData;
		hr = pArrDispInfo->GetValue(dwDispInfoIndex, AVModule::AVDataTypes::eTypeMapDword, spTempData);
		if(FAILED(hr) || !spTempData)
		{
			CCTRACEE(_T("In XferFirstInfectedFile(), unable to retrieve array info.  Unexpected."));
			continue;
		}

		AVModule::IAVMapDwordDataQIPtr spMapCurrentDispInfo = spTempData;
		if(!spMapCurrentDispInfo)
		{
			CCTRACEE(_T("In XferFirstInfectedFile(), unable to retrieve array type.  Unexpected."));
			continue;
		}

		DWORD dwRemediationType = NULL;
		spMapCurrentDispInfo->GetValue(AVModule::ThreatTracking::RemDisp_RemediationType, dwRemediationType);

		// If we found an infection remediation, take it and bail...
		if(ccEraser::InfectionRemediationActionType == dwRemediationType)        
		{
			AvDwordMapToAvEventData_STRING(spMapCurrentDispInfo, AVModule::ThreatTracking::RemDisp_Target, FileData, dwAvDataId);
			cszAlternateFileName.Empty(); // make sure this is cleared
			CCTRACEE(_T("In XferFirstInfectedFile(), an infection remediation is found."));
			break;
		}

		// ... otherwise, take what we check for an alternate, and continue loop for the infection remediation
		if(ccEraser::FileRemediationActionType == dwRemediationType && cszAlternateFileName.IsEmpty())
		{
			cc::IStringPtr spTempString;
			spMapCurrentDispInfo->GetValue(AVModule::ThreatTracking::RemDisp_RemediationType, spTempString);
			if(spTempString)
				cszAlternateFileName = spTempString->GetStringW();

			CCTRACEE(_T("In XferFirstInfectedFile(), an alternate is found."));
		}
		else if(ccEraser::CookieRemediationActionType == dwRemediationType && cszAlternateFileName.IsEmpty())
		{
			cc::IStringPtr spTempString;
			spMapCurrentDispInfo->GetValue(AVModule::ThreatTracking::RemDisp_RemediationType, spTempString);
			if(spTempString)
				cszAlternateFileName = spTempString->GetStringW();

			CCTRACEE(_T("In XferFirstInfectedFile(), an alternate is found."));
		}
	}

	if(!cszAlternateFileName.IsEmpty()) //If cszAlternateFileName is not empty, it means that we did not find an infection remediation, cszAlternateFileName should be used here.
		FileData.SetData(dwAvDataId, cszAlternateFileName);

	return;
}

void XferFileFromContainer(AVModule::IAVMapDwordData* pArrDispInfo, DWORD dwAvDataId, CEventData& FileData);

void SingleAvModuleThreatDataToAvProdFileData(AVModule::IAVMapDwordData* pAvModuleActivity, AVModule::IAVArrayData* pArrDispInfo, CEventData& AvActivity)
{

	AVModule::IAVMapDwordDataQIPtr spMapCurThreat = pAvModuleActivity;
	if(!spMapCurThreat)
		return;

	AvDwordMapToAvEventData_STRING(spMapCurThreat, AVModule::ThreatTracking::TrackingData_VirusDefsVersion, AvActivity, AV::Event_Threat_propDefsRevision);

	CEventData eventThreatData;
	AvDwordMapToAvEventData_STRING(spMapCurThreat, AVModule::ThreatTracking::TrackingData_NAME, eventThreatData, AV::Event_ThreatEntry_propVirusName);
	AvDwordMapToAvEventData_DWORD(spMapCurThreat, AVModule::ThreatTracking::TrackingData_VID, eventThreatData, AV::Event_ThreatEntry_propVirusID);
	AvDwordMapToAvEventData_DWORD2STRING(spMapCurThreat, AVModule::ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, eventThreatData, AV::Event_ThreatEntry_propThreatMatrixOverall);

	XferThreatTrackThreatMatrixToThreatData(spMapCurThreat, eventThreatData);
	XferCategoryListToThreatDataCategories(spMapCurThreat, AVModule::ThreatTracking::TrackingData_THREATCATEGORIES, eventThreatData);
	XferThreatTrackResultToThreatDataResult(spMapCurThreat, eventThreatData);

	CEventData eventAllThreats;
	eventAllThreats.SetNode(0, eventThreatData);

	CEventData eventFileData;
	eventFileData.SetNode(AV::Event_ThreatSubFile_propThreatData, eventAllThreats);

	DWORD dwThreatType = -1;
	pAvModuleActivity->GetValue(AVModule::ThreatTracking::TrackingData_THREAT_TYPE, dwThreatType);
	if(dwThreatType != -1)
	{
		if(AVModule::ThreatTracking::ThreatType_Anomaly == dwThreatType && pArrDispInfo)
			XferFirstInfectedFile(pArrDispInfo, AV::Event_ThreatSubFile_propFileName, eventFileData);
		else
			XferFileFromContainer(pAvModuleActivity, AV::Event_ThreatSubFile_propFileName, eventFileData);
	}

	CEventData eventAllFiles;
	eventAllFiles.SetNode(0, eventFileData);

	AvActivity.SetNode(AV::Event_Threat_propSubFileData, eventAllFiles);

	return;
}

void XferFileFromContainer(AVModule::IAVMapDwordData* pThreatInfo, DWORD dwAvDataId, CEventData& FileData)
{
	do
	{
		HRESULT hr;

		ISymBasePtr spTempObj;
		hr = pThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_Compressed_COMPONENTS, AVModule::AVDataTypes::eTypeArray, spTempObj);
		if(FAILED(hr) || !spTempObj)
		{
			CCTRACEE( CCTRCTX _T("Problem getting compressed components array. hr=0x%08X"), hr);
			break;
		}

		AVModule::IAVArrayDataQIPtr spCompressedComponents = spTempObj;
		DWORD dwArraySize = NULL, dwArrayIndex = NULL;
		hr = spCompressedComponents->GetCount(dwArraySize);
		if(FAILED(hr) || !dwArraySize)
		{
			CCTRACEE( CCTRCTX _T("Problem getting compressed components array size. hr=0x%08X, dwArraySize=%d"), hr, dwArraySize);
			break;
		}
		
		CString cszSourceString;
		for(dwArrayIndex = 0; dwArrayIndex < dwArraySize; dwArrayIndex++)
		{
			cc::IStringPtr spComponentPath;
			hr = spCompressedComponents->GetValue(dwArrayIndex, spComponentPath);
			if(FAILED(hr) || !spComponentPath)
			{
				CCTRACEE( CCTRCTX _T("Problem getting string from array. dwArrayIndex=%d, hr = 0x%08X"), dwArrayIndex, hr);
				continue;
			}

			if(1 == dwArraySize)
			{
				cszSourceString = CW2T(spComponentPath->GetStringW());
				break;
			}

			CString cszTempFileName;
			cszTempFileName.Format(_S(IDS_FMT_COMPRESSED_FILE), CW2T(spComponentPath->GetStringW()));

			if(!cszSourceString.GetLength())
			{
				cszSourceString = cszTempFileName;
				continue;
			}

			cszSourceString += _S(IDS_COMPRESSED_COMPONENT);
			cszSourceString += cszTempFileName;
		}

		if(cszSourceString.GetLength())
			FileData.SetData(dwAvDataId, (LPCTSTR)cszSourceString);

	}while(false);

	return;
}

void AvScanCompleteDataFromAvModuleOnApComplete(AVModule::IAVMapStrData* pAvModuleActivity, CEventData& AvActivity)
{
	DWORD dwAbort = NULL, dwItemCount = NULL, dwScanTime = NULL, dwVirsDetected = NULL, dwVirsRemoved = NULL, dwNonVirsDetected = NULL, dwNonVirsRemoved = NULL, dwScanType = NULL;

	pAvModuleActivity->GetValue(AVModule::ManualScanner::AV_SCANCOMPLETE_ABORT, dwAbort);
	pAvModuleActivity->GetValue(AVModule::ManualScanner::AV_SCANCOMPLETE_ITEMCOUNT, dwItemCount);
	pAvModuleActivity->GetValue(AVModule::ManualScanner::AV_SCANCOMPLETE_SCANTIME, dwScanTime);
	pAvModuleActivity->GetValue(AVModule::ManualScanner::AV_SCANCOMPLETE_VIRSDETECTED, dwVirsDetected);
	pAvModuleActivity->GetValue(AVModule::ManualScanner::AV_SCANCOMPLETE_VIRSREMOVED, dwVirsRemoved);
	pAvModuleActivity->GetValue(AVModule::ManualScanner::AV_SCANCOMPLETE_NONVIRALSDETECTED, dwNonVirsDetected);
	pAvModuleActivity->GetValue(AVModule::ManualScanner::AV_SCANCOMPLETE_NONVIRALSREMOVED, dwNonVirsRemoved);
	pAvModuleActivity->GetValue(AVModule::ManualScanner::AV_SCANCOMPLETE_SCANTYPE, dwScanType);


	AvActivity.SetData(AV::Event_ScanAction_ItemTotalScanned, dwItemCount);
	AvActivity.SetData(AV::Event_ScanAction_VirusesDetected, dwVirsDetected);
	AvActivity.SetData(AV::Event_ScanAction_VirusesRemoved, dwVirsRemoved);
	AvActivity.SetData(AV::Event_ScanAction_NonVirusesDetected, dwNonVirsDetected);
	AvActivity.SetData(AV::Event_ScanAction_NonVirusesRemoved, dwNonVirsRemoved);
	AvActivity.SetData(AV::Event_ScanAction_ScanTime, dwScanTime);

	AvModuleScanTypeToAvScanType(dwScanType, AV::Event_ScanAction_propTaskName, AvActivity);
	AvModuleScanResultToAvScanResult(dwAbort, AV::Event_ScanAction_propAction, AvActivity);

	AvScanCompleteEraserStatsToAvProdLog(pAvModuleActivity, AVModule::ManualScanner::AV_SCANCOMPLETE_ERASERSTATS, AvActivity);

	GUID guidScanInstance = GUID_NULL;
	HRESULT hr = pAvModuleActivity->GetValue(AVModule::ManualScanner::AV_SCANCOMPLETE_INSTANCEGUID, guidScanInstance);
	if(SUCCEEDED(hr))
	{
		AvActivity.SetData(AV::Event_Base_propUniqueID, (LPBYTE)&guidScanInstance, sizeof(GUID));
		AvScanCompleteToAvProdSummary(guidScanInstance, AvActivity);	
	}

	return;
}

void AvScanCompleteEraserStatsToAvProdLog(AVModule::IAVMapStrData* pAvModuleActivity, LPCWSTR wszElementId, CEventData& AvActivity)
{
	HRESULT hr;

	// Get the eraser stats object
	ISymBasePtr spAvModuleArray;
	hr = pAvModuleActivity->GetValue(wszElementId, AVModule::AVDataTypes::eTypeMapDword, spAvModuleArray);
	if(FAILED(hr) || !spAvModuleArray)
		return;

	AVModule::IAVMapDwordDataQIPtr spEraserStatsMap = spAvModuleArray;
	if(!spEraserStatsMap)
		return;

	CEventData EraserStats;
	for(DWORD dwIndex = ccEraser::FirstDetectionType; dwIndex < ccEraser::LastDetectionType; dwIndex++)
	{
		DWORD dwData = NULL;
		hr = spEraserStatsMap->GetValue(dwIndex, dwData);
		if(FAILED(hr))
			continue;

		EraserStats.SetData(dwIndex, dwData);
	}

	AvActivity.SetNode(AV::Event_ScanAction_EraserScanResults, EraserStats);
	return;
}

void AvScanCompleteToAvProdSummary(GUID& guidScanInstance, CEventData& AvActivity)
{
	StahlSoft::HRX hrx;
	ccLib::CExceptionInfo exceptionInfo;
	try
	{
		AVModule::AVLoader_IAVThreatInfo AvThreatInfoLoader;
		AvThreatInfoLoader.Initialize();

		AVModule::IAVThreatInfoPtr spThreatInfo;
		AvThreatInfoLoader.CreateObject(spThreatInfo);
		if(!spThreatInfo)
		{
			CCTRACEW( CCTRCTX _T("AvModuleLoader::CreateObject() failed."));
			hrx << E_UNEXPECTED;
		}

		AVModule::AVLoader_IAVMapDwordData AVMapDwordDataLoader;
		AVModule::IAVMapDwordDataPtr spFilter;
		AVMapDwordDataLoader.CreateObject(spFilter);
		if(!spFilter)
		{
			CCTRACEW( CCTRCTX _T("AVLoader_IAVMapDwordData::CreateObject() failed."));
			hrx << E_UNEXPECTED;
		}

		// Get threat list by scan instance
		spFilter->SetValue(AVModule::ThreatTracking::GetThreatBy_SCANINSTANCE, guidScanInstance);

		AVModule::IAVArrayDataPtr spThreatData;
		hrx << spThreatInfo->GetThreatsWithFilter(spFilter, spThreatData);

		DWORD dwThreatInfoCount = NULL;
		hrx << spThreatData->GetCount(dwThreatInfoCount);

		CEventData ResolvedByCategory, UnresolvedByCategory;
		for(DWORD dwThreatInfoIndex = 0; dwThreatInfoIndex < dwThreatInfoCount; dwThreatInfoIndex++)
		{
			HRESULT hr;
			ISymBasePtr spTempArray;
			hr = spThreatData->GetValue(dwThreatInfoIndex, AVModule::AVDataTypes::eTypeMapDword, spTempArray);
			if(FAILED(hr) || !spTempArray)
				continue;

			AVModule::IAVMapDwordDataQIPtr spThreatMap = spTempArray;
			if(!spThreatMap)
				continue;

			// Get the categories
			spTempArray.Release();
			hr = spThreatMap->GetValue(AVModule::ThreatTracking::TrackingData_THREATCATEGORIES, AVModule::AVDataTypes::eTypeArray, spTempArray);
			if(FAILED(hr) || !spTempArray)
				continue;

			AVModule::IAVArrayDataQIPtr spArrCats = spTempArray;
			if(!spArrCats)
				continue;

			// Get the category count and loop through them
			DWORD dwCats;
			hr = spArrCats->GetCount(dwCats);
			for(DWORD dwIdx = 0; dwIdx < dwCats; ++dwIdx)
			{
				ccEraser::IAnomaly::Anomaly_Category eCategory;
				if(SUCCEEDED(spArrCats->GetValue(dwIdx, (DWORD&)eCategory)))
				{
					// group the category
					eCategory = GroupCategory(eCategory);

					// Increment the correct counter
					if(ThreatHasBeenResolved(spThreatMap))
					{
						LONG lValue = 0;
						ResolvedByCategory.GetData(eCategory, lValue);
						lValue++;
						ResolvedByCategory.SetData(eCategory, lValue);
					}
					else
					{
						LONG lValue = 0;
						UnresolvedByCategory.GetData(eCategory, lValue);
						lValue++;
						UnresolvedByCategory.SetData(eCategory, lValue);
					}
				}
			}
		}

		AvActivity.SetNode(AV::Event_ScanAction_ResolvedByCategory, ResolvedByCategory);
		AvActivity.SetNode(AV::Event_ScanAction_UnResolvedByCategory, UnresolvedByCategory);
	}
	CCCATCHMEM(exceptionInfo)
		CCCATCHCOM(exceptionInfo);

	if(exceptionInfo.IsException())
		return;

	return;
}

void AvModuleScanTypeToAvScanType(DWORD dwScanType, DWORD dwPropertyId, CEventData& AvActivity)
{
	if(AVModule::ManualScanner::eScanType_Full == dwScanType)
	{
		AvActivity.SetData(dwPropertyId, _S(IDS_TEXT_FULL_SYSTEM_SCAN));
	}
	else if(AVModule::ManualScanner::eScanType_Context == dwScanType)
	{
		AvActivity.SetData(dwPropertyId, _S(IDS_TEXT_CONTEXT_SCAN));
	}
	else if(AVModule::ManualScanner::eScanType_Email == dwScanType)
	{
		AvActivity.SetData(dwPropertyId, _S(IDS_TEXT_EMAIL_SCAN));
	}

	return;
}

void AvModuleScanResultToAvScanResult(DWORD dwResult, DWORD dwPropertyId, CEventData& AvActivity)
{
	if(0 == dwResult)
	{
		AvActivity.SetData( dwPropertyId, AV::Event_ScanAction_ScanCompleted);
	}
	else
	{
		AvActivity.SetData( dwPropertyId, AV::Event_ScanAction_ScanAborted);
	}

	return;
}

void AvActivityDataFromAvModuleOnApThreatBlocked(AVModule::IAVMapDwordData* pAvModuleActivity, CEventData& AvActivity)
{

	CEventData eventThreatData;
	AvDwordMapToAvEventData_STRING(pAvModuleActivity, AVModule::AutoProtect::APBlocked_Name, eventThreatData, AV::Event_ThreatEntry_propVirusName);
	AvDwordMapToAvEventData_DWORD(pAvModuleActivity, AVModule::AutoProtect::APBlocked_VID, eventThreatData, AV::Event_ThreatEntry_propVirusID);

	do
	{
		HRESULT hr;
		DWORD dwVirusId = NULL;
		hr = pAvModuleActivity->GetValue(AVModule::AutoProtect::APBlocked_VID, dwVirusId);
		if(FAILED(hr) || !dwVirusId)
		{
			CCTRACEE( CCTRCTX, _T("Problem getting virus ID from AP blocked info. hr=0x%08X"), hr);
			break;
		}

		AVModule::AVLoader_IAVThreatInfo AvThreatInfoLoader;
		AvThreatInfoLoader.Initialize();

		AVModule::IAVThreatInfoPtr spThreatInfo;
		AvThreatInfoLoader.CreateObject(spThreatInfo);
		if(!spThreatInfo)
		{
			CCTRACEW( CCTRCTX _T("AvModuleLoader::CreateObject() failed."));
			break;
		}

		AVModule::AVLoader_IAVMapDwordData AVMapDwordDataLoader;
		AVModule::IAVMapDwordDataPtr spFilter;
		AVMapDwordDataLoader.CreateObject(spFilter);
		if(!spFilter)
		{
			CCTRACEW( CCTRCTX _T("AVLoader_IAVMapDwordData::CreateObject() failed."));
			break;
		}

		AVModule::IAVMapDwordDataPtr spDisplayData;
		hr = spThreatInfo->GetVIDDisplayData(dwVirusId, spDisplayData);
		if(!spDisplayData)
		{
			CCTRACEW( CCTRCTX _T("Problem getting VID display data. VID=0x%08X, hr=0x%08X"), dwVirusId, hr);
			break;
		}

		ISymBasePtr spTempData;
		hr = spDisplayData->GetValue(AVModule::ThreatProcessor::GetVIDDispOut_ThreatData, AVModule::AVDataTypes::eTypeMapDword, spTempData);
		if(FAILED(hr) || !spTempData)
		{
			CCTRACEW( CCTRCTX _T("Problem getting threat track info. VID=0x%08X, hr=0x%08X"), dwVirusId, hr);
			break;
		}

		AVModule::IAVMapDwordDataQIPtr spMapCurThreat = spTempData;
		if(!spMapCurThreat)
		{
			CCTRACEW( CCTRCTX _T("Problem getting threat track info map interface. VID=0x%08X"), dwVirusId);
			break;
		}

		AvDwordMapToAvEventData_DWORD2STRING(spMapCurThreat, AVModule::ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, eventThreatData, AV::Event_ThreatEntry_propThreatMatrixOverall);
		XferThreatTrackThreatMatrixToThreatData(spMapCurThreat, eventThreatData);

	}while(false);

	// Set the result
	BYTE byAction = (BYTE)AV::Event_Action_Blocked;
	eventThreatData.SetData ( AV::Event_ThreatEntry_propActionCount, 1 );
	eventThreatData.SetData ( AV::Event_ThreatEntry_propActionData, &byAction, 1 );
	eventThreatData.SetData ( AV::Event_ThreatEntry_propStillPresent, (LONG)0);

	XferCategoryListToThreatDataCategories(pAvModuleActivity, AVModule::AutoProtect::APBlocked_Categories, eventThreatData);

	CEventData eventAllThreats;
	eventAllThreats.SetNode(0, eventThreatData);

	CEventData eventFileData;
	AvDwordMapToAvEventData_STRING(pAvModuleActivity, AVModule::AutoProtect::APProcessingComplete_FileName, eventFileData, AV::Event_ThreatSubFile_propFileName);
	eventFileData.SetNode(AV::Event_ThreatSubFile_propThreatData, eventAllThreats);

	CEventData eventAllFiles;
	eventAllFiles.SetNode(0, eventFileData);

	AvActivity.SetNode(AV::Event_Threat_propSubFileData, eventAllFiles);

	return;
}

void AvOEHDetectionFromAvModuleOEHDetection(AVModule::IAVMapStrData* pOEHInfo, DWORD dwOEHAction, CEventData& AvActivity)
{
	AvStrMapToAvEventData_STRING(pOEHInfo, AVModule::EmailScanner::AV_EMAILOEH_SUBJECT, AvActivity, AV::Event_OEHDetection_propSubject);
	AvStrMapToAvEventData_STRING(pOEHInfo, AVModule::EmailScanner::AV_EMAILOEH_SENDER, AvActivity, AV::Event_OEHDetection_propSender);
	AvStrMapToAvEventData_STRING(pOEHInfo, AVModule::EmailScanner::AV_EMAILOEH_RECIPIENT, AvActivity, AV::Event_OEHDetection_propRecipient);
	AvStrMapToAvEventData_STRING(pOEHInfo, AVModule::EmailScanner::AV_EMAILOEH_WORMPATH, AvActivity, AV::Event_OEHDetection_propWormFile);
	AvStrMapToAvEventData_STRING(pOEHInfo, AVModule::EmailScanner::AV_EMAILOEH_VERSION, AvActivity, AV::Event_Threat_propFeatureVersion);
	AvActivity.SetData(AV::Event_OEHDetection_propAction, (LONG)dwOEHAction);

	GUID guidThreatTrackId;
	pOEHInfo->GetValue(AVModule::EmailScanner::AV_EMAILOEH_QBACKUPID, guidThreatTrackId);
	AvActivity.SetData(AV::Event_OEHDetection_propThreatTrackId, (LPBYTE)&guidThreatTrackId, sizeof(GUID));

	return;
}

void AvModuleDisplayInfoToEventData(AVModule::IAVArrayData* pArrDispInfo, DWORD dwPropertyId, CEventData& AvActivity)
{
	if(!pArrDispInfo)
		return;

	DWORD dwDispInfoCount = NULL;
	pArrDispInfo->GetCount(dwDispInfoCount);

	CEventData DispInfo;

	for(DWORD dwDispInfoIndex = 0; dwDispInfoIndex < dwDispInfoCount; dwDispInfoIndex++)
	{
		HRESULT hr;
		ISymBasePtr spTempData;
		hr = pArrDispInfo->GetValue(dwDispInfoIndex, AVModule::AVDataTypes::eTypeMapDword, spTempData);
		if(FAILED(hr) || !spTempData)
		{
			CCTRACEE(_T("Unable to retrieve array info.  Unexpected."));
			continue;
		}

		AVModule::IAVMapDwordDataQIPtr spMapCurrentDispInfo = spTempData;
		if(!spMapCurrentDispInfo)
		{
			CCTRACEE(_T("Unable to retrieve array type.  Unexpected."));
			continue;
		}

		CEventData CurrentDispInfo;
		AvDwordMapToAvEventData_DWORD(spMapCurrentDispInfo, AVModule::ThreatTracking::RemDisp_RemediationType, CurrentDispInfo, AVModule::ThreatTracking::RemDisp_RemediationType);
		AvDwordMapToAvEventData_STRING(spMapCurrentDispInfo, AVModule::ThreatTracking::RemDisp_Target, CurrentDispInfo, AVModule::ThreatTracking::RemDisp_Target);
		AvDwordMapToAvEventData_DWORD(spMapCurrentDispInfo, AVModule::ThreatTracking::RemDisp_OperationType, CurrentDispInfo, AVModule::ThreatTracking::RemDisp_OperationType);
		AvDwordMapToAvEventData_DWORD(spMapCurrentDispInfo, AVModule::ThreatTracking::RemDisp_State, CurrentDispInfo, AVModule::ThreatTracking::RemDisp_State);
		AvDwordMapToAvEventData_DWORD(spMapCurrentDispInfo, AVModule::ThreatTracking::RemDisp_RemediationResult, CurrentDispInfo, AVModule::ThreatTracking::RemDisp_RemediationResult);
		AvDwordMapToAvEventData_STRING(spMapCurrentDispInfo, AVModule::ThreatTracking::RemDisp_SecurityDescriptorString, CurrentDispInfo, AVModule::ThreatTracking::RemDisp_SecurityDescriptorString);
		AvDwordMapToAvEventData_DWORD(spMapCurrentDispInfo, AVModule::ThreatTracking::RemDisp_IsPresent, CurrentDispInfo, AVModule::ThreatTracking::RemDisp_IsPresent);
		AvDwordMapToAvEventData_DWORD(spMapCurrentDispInfo, AVModule::ThreatTracking::RemDisp_UndoeResult, CurrentDispInfo, AVModule::ThreatTracking::RemDisp_UndoeResult);
		AvDwordMapToAvEventData_DWORD(spMapCurrentDispInfo, AVModule::ThreatTracking::RemDisp_SupportsUndo, CurrentDispInfo, AVModule::ThreatTracking::RemDisp_SupportsUndo);

		DispInfo.SetNode(dwDispInfoIndex, CurrentDispInfo);
	}

	if(0 != DispInfo.GetCount())
	{
		AvActivity.SetNode(dwPropertyId, DispInfo);
	}

}

void AvSetUniqueID(CEventData& AvActivity)
{
	GUID evtGUID;
	HRESULT hr = ::CoCreateGuid(&evtGUID);
	if(SUCCEEDED(hr))
		AvActivity.SetData(AV::Event_Base_propUniqueID, (LPBYTE)&evtGUID, sizeof(GUID));
	else
		CCTRCTXE1(_T("unable to create event log GUID:0x%08x"), hr);
}

//****************************************************************************
//****************************************************************************
bool ThreatHasBeenResolved(AVModule::IAVMapDwordData* pThreatTrackInfo)
{
	if(!pThreatTrackInfo)
		return false; // should throw exception?

	DWORD dwThreatStateId = NULL, dwThreatTypeId = NULL;
	pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_CURRENT_STATE, dwThreatStateId);
	pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_THREAT_TYPE, dwThreatTypeId);

	// This logic is essentially mirrored in NAVLOGV/Message.cpp
	//  TODO: Should make a shared class so this doesn't get out of date...
	// 

	// Get threat resolved
	bool bThreatHasBeenResolved = false;
	if(  AVModule::ThreatTracking::ThreatRemoved(dwThreatStateId) 
		|| AVModule::ThreatTracking::ThreatState_BackupOnly == dwThreatStateId 
		|| AVModule::ThreatTracking::ThreatType_Email == dwThreatTypeId )
	{
		bThreatHasBeenResolved = true;
	}

	bool bManualRemovalRequired = false;
	bManualRemovalRequired = !bThreatHasBeenResolved && (AVModule::ThreatTracking::ThreatState_DoNotDelete == dwThreatStateId);

	HRESULT hr;
	ISymBasePtr spTempObj;

	bool bHasDependencyFlag = false;
	hr = pThreatTrackInfo->GetValue(AVModule::ThreatTracking::TrackingData_INDIVIDUAL_THREAT_LEVELS, AVModule::AVDataTypes::eTypeMapDword, spTempObj);
	if(SUCCEEDED(hr) && spTempObj)
	{
		AVModule::IAVMapDwordDataQIPtr spMapCurThreat = spTempObj;
		if(spTempObj)
		{
			DWORD dwDependencyFlag = (DWORD)-1;
			spMapCurThreat->GetValue(ccEraser::IAnomaly::Dependency, dwDependencyFlag);
			bHasDependencyFlag = dwDependencyFlag != 0 && dwDependencyFlag != (DWORD)-1;
		}
	}

	bool bThreatHasBeenIgnored = false;
	if( !bThreatHasBeenResolved 
		&& (AVModule::ThreatTracking::ThreatState_RemoveNotAttempted  == dwThreatStateId) 
		&& !bManualRemovalRequired && 
		!bHasDependencyFlag )
	{
		bThreatHasBeenIgnored = true;
	}

	return bThreatHasBeenResolved || bThreatHasBeenIgnored;
}

ccEraser::IAnomaly::Anomaly_Category GroupCategory(ccEraser::IAnomaly::Anomaly_Category eCat)
{
	switch(eCat)
	{
		// Malicious, Reserved Malicious, and Heuristic all display as "Virus" ... so we group them
	case ccEraser::IAnomaly::Malicious:
	case ccEraser::IAnomaly::ReservedMalicious:
	case ccEraser::IAnomaly::Heuristic:
		return ccEraser::IAnomaly::Viral;
	}

	// If it wasn't in a group (above), return it as is
	return eCat;
}

