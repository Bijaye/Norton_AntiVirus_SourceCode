////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "avQBackupTypes.h"
#include "ccSymMemoryStreamImpl.h"
#include ".\sample.h"
#include "Utils.h"

namespace AVSubmit {

CSample::CSample(void) : m_bLoadingFromAnomaly(false)
{
	CCTRACEI(CCTRCTX);
}

CSample::~CSample(void)
{
	CCTRACEI(CCTRCTX);
}

HRESULT CSample::GetTypeId(GUID& typeId) const throw()
{
	typeId = TYPEID_AV_SAMPLE;
	return S_OK;
}
bool CSample::GetObjectId(SYMOBJECT_ID& oid) const throw()
{
	oid = CLSID_AVSampleSubmission;
	return true;
}

//HRESULT CSample::SetAVCompressed() throw()
//{
//	CCTRCTXW0(_T("Sample type is AV Compressed. Not Submitting"));
//	return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
//}

HRESULT CSample::LoadSubmissionData() throw()
{
	TRACE_SCOPE(s0);
	HRESULT hr = S_OK;
	for(;;)
	{
		hr = CSubmissionBase::LoadSubmissionData();
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to load submission data: 0x%08X"), hr);
			break;
		}
		if(m_data.list.size() < 1)
		{
			if(m_bLoadingFromAnomaly)
			{
				hr = HRESULT_FROM_WIN32(ERROR_NO_DATA);
				CCTRCTXE1(_T("No data, already attempted loading from anomaly. Failing: 0x%08X"), hr);
				break;
			}
			m_bLoadingFromAnomaly = true;
			hr = LoadFromAnomaly();
			if(FAILED(hr))
			{
				CCTRCTXE1(_T("Failed to load from anomaly: 0x%08X"), hr);
				CCTRCTXE1(_T("No data found - required for sample submission: 0x%08X"), hr);
				break;
			}
		}

		if(IsManual())
		{
			CCTRCTXI0(_T("Manual Submission: Ignoring SubmitData property"));
			hr = S_OK;
			break;
		}

		if(m_data.pAnomaly == NULL)
		{
			CCTRCTXI0(_T("No anomaly, not checking SubmitData flag"));
			hr = S_OK;
			break;
		}

		
		//COH Submissions are detected by VID.  
		//This flag is only set by eraser for "hueristic submissions" (aka COH).  
		//Inspecting this flag for other VIDs would return false, 
		//and would prevent desired submissions from being generated.
		//That is why the processing of the flag was commented out.

#if 0
#pragma message("TODO: Re-Enable IAnomaly::SubmitData flag")
		//we are looking for the SubmitData flag.
		//Absense of this flag means response does not want the data.
		//This only applies if an anomaly is associated with the submission.

		hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

		cc::IKeyValueCollectionPtr pProps;
		ccEraser::eResult eRes = m_data.pAnomaly->GetProperties(pProps.m_p);
		if(ccEraser::Failed(eRes) || pProps == NULL)
		{	
			CCTRCTXE1(_T("Failed to get props: %d"), eRes);
			break;
		}

		bool bSubmitData = false;
		if(!pProps->GetValue(ccEraser::IAnomaly::SubmitData, bSubmitData))
		{
			CCTRCTXW0(_T("Faield to get SubmitData property"));
			break;
		}
		if(bSubmitData == false)
		{
			CCTRCTXW0(_T("Not submitting data for this anomaly - SubmitData == false"));
			break;
		}
#endif

		hr = S_OK;
		break;
	}

	return hr;
}

HRESULT CSample::LoadFromAnomaly() throw()
{
	TRACE_SCOPE(s0);
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_NO_DATA);
	GUID setId = GUID_NULL;
	for(;;)
	{
		ccEraser::IAnomalyPtr pAnomaly = m_data.pAnomaly;
		if(pAnomaly == NULL)
		{
			CCTRCTXE0(_T("pAnomaly == NULL"));
			break;
		}
		m_data.reset();

		CCTRCTXI0(_T("No data in quarantine, manually quarantining from anomaly"));

		ccEraser::IRemediationActionListPtr pList;
		ccEraser::eResult eRes = pAnomaly->GetRemediationActions(pList.m_p);
		if(ccEraser::Failed(eRes) || pList == NULL)
		{
			CCTRCTXE1(_T("No Remediation Actions: %d"), eRes);
			break;
		}
		size_t nCount = 0;
		eRes = pList->GetCount(nCount);
		if(ccEraser::Failed(eRes) || nCount < 1)
		{
			CCTRCTXE2(_T("No actions in list, eResult=%d, size=%Iu"), eRes, nCount);
			break;
		}

		CCTRCTXI1(_T("%Iu Remediations"), nCount);

		QBackup::IQBackupPtr pBackup;
		HRESULT hrLocal = GetBackup(pBackup.m_p);
		if(FAILED(hrLocal))
		{
			CCTRCTXE1(_T("Failed to get QBackup: 0x%08X"), hrLocal);
			break;
		}
		QBackup::IQBackupSetPtr pSet;
		HRESULT sr = pBackup->CreateBackupSet(pSet.m_p);
		if(FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to create backup set: 0x%08X"), sr);
			break;
		}
		sr = pBackup->StoreBackupSet(pSet, &setId, false);
		if(FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to store set: 0x%08X"), sr);
			break;
		}

		sr = pSet->SetGivenType(GT_SubmitUnremediated);
		if(FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to set given type: 0x%08X"), sr);
			break;
		}

		//Back this thing up...
		DWORD dwSucceeded = 0;
		for(size_t i = 0; i < nCount; ++i)
		{
			ccEraser::IRemediationActionPtr pAction;
			eRes = pList->GetItem(i, pAction.m_p);
			if(ccEraser::Failed(eRes) || pAction == NULL)
			{
				//weird, but not fatal
				CCTRCTXE2(_T("Failed to get item @ %Iu, eResult = %d"), i, eRes);
				continue;
			}
			DumpRemediation(i, pAction);
			hrLocal = BackupRemediation(pSet, pAction);
			if(FAILED(hrLocal))
			{
				//keep on trying
				CCTRCTXE2(_T("Failed to backup Remediation #%Iu: 0x%08X"), i, hrLocal);
				continue;
			}
			++dwSucceeded;
		}

		if(dwSucceeded < 1)
		{
			CCTRCTXE0(_T("Fatal: No remediations backed up!"));
			break;
		}

		ccSym::CMemoryStreamImplPtr pMemoryStream;
		pMemoryStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		if(pMemoryStream == NULL)
		{
			hr = E_OUTOFMEMORY;
			break;
		}

		cc::ISerializeQIPtr pSerializeAnomaly = pAnomaly;
		if(pSerializeAnomaly == NULL)
		{
			CCTRCTXE0(_T("pSerializeAnomaly == NULL"));
			hr = E_NOINTERFACE;
			break;
		}

		ULONGLONG ullSize = 0;
		if(!pSerializeAnomaly->GetSize(ullSize) || !pMemoryStream->SetSize(ullSize))
		{
			CCTRCTXE0(_T("GetSize/SetSize failed"));
			hr = E_OUTOFMEMORY;
			break;
		}

		if(!pSerializeAnomaly->Save(pMemoryStream))
		{
			CCTRCTXE0(_T("Save == false"));
			hr = E_UNEXPECTED;
			break;
		}

		if(!pMemoryStream->SetPosition(0))
		{
			CCTRCTXE0(_T("SetPos(0) failed"));
			hr = E_UNEXPECTED;
			break;
		}

		QBackup::IQBackupItemBasePtr pItemBase;
		hr = pSet->CreateBackupItem(QBackup::itemStream, pItemBase.m_p);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("CreateBackupIte failed 0x%08X"), hr);
			break;
		}

		QBackup::IQBackupItemStreamQIPtr pStreamBackup = pItemBase;
		if(pStreamBackup == NULL)
		{
			hr = E_NOINTERFACE;
			CCTRCTXE0(_T("pSterambackup == NULL"));
			break;
		}

		hr = pItemBase->SetGivenType(avQBTypes::GT_AVANOMALY);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Set GT failed: 0x%08X"), hr);
			break;
		}

		hr = pStreamBackup->Backup(pMemoryStream);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Backup failed: 0x%08X"), hr);
			break;
		}

		GUID anomId = GUID_NULL;
		hr = pSet->StoreBackupItem(pItemBase, &anomId);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to store backup item: 0x%08X"), hr);
			break;
		}

		cc::IKeyValueCollectionPtr pProps;
		hr = pSet->GetProperties(pProps.m_p);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("GetProps failed: 0x%08X"), hr);
			break;
		}
		if(pProps == NULL)
		{
			CCTRCTXE0(_T("pProps == NULL"));
			hr = E_POINTER;
			break;
		}

		if(!pProps->SetValue(avQBTypes::avASProp_SerializedAnomalyBackupID, anomId))
		{
			CCTRCTXE0(_T("SetValue failed"));
			hr = E_OUTOFMEMORY;
			break;
		}

		HRESULT hr = pSet->Flush();
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to flush set: 0x%08X"), hr);
			break;
		}

		if(m_pAttributes->SetValue(eSetId, setId) == false)
		{
			CCTRCTXE0(_T("Failed to set setId"));
			break;
		}
		
		CCTRACEI(CCTRCTX _T("Created new backup set: ") SYMGUID_FORMAT_STR((&setId)));
		hr = LoadSubmissionData();
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Re-Load submisison data failed: 0x%08X"), hr);
			break;
		}

		CCTRCTXI0(_T("Success"));
		hr = S_OK;
		break;
	}

	//cleanup on failure

	if(FAILED(hr))
	{
		QBackup::IQBackupPtr pBackup;
		HRESULT hr2 = GetBackup(pBackup.m_p);
		if(SUCCEEDED(hr2))
		{
			hr2 = pBackup->RemoveBackupSet(setId);
			if(FAILED(hr2))
				CCTRCTXE1(_T("Remove failed: 0x%08x"), hr2);
		}
		else
			CCTRCTXE1(_T("Load QB failed: 0x%08X"), hr2);
	}
	return hr;
}

} //namespace AVSubmit