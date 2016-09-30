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
#include ".\avsubmissionsample.h"
#include "utils.h"
#include "RegKeyFormat.h"
#include "ccSymStringImpl.h"
#include "ccVersionInfo.h"

namespace dis = clfs::DIS;

namespace AVSubmit {

namespace {
/*
	AvSubmission: X-SCAN-VIRUS-NAME
	AvSubmission: X-SCAN-RESULT
	AVSubmission: X-SCAN-VIRUS-IDENTIFIER
	AVSubmission: X-SCAN-SIGNATURES-VERSION
	AVSubmission: X-SCAN-SIGNATURES-SEQUENCE
	DIS: X-SAMPLE-TYPE - hardcode to "file"
	DIS: X-SAMPLE-STATUS - hardcode to 0
	DIS: X-SAMPLE-SIZE
	Detection: X-SAMPLE-REASON "hueristic,manual,unk"
	DIS: X-SAMPLE-PRIORITY - hardcode to 500
	Detection: X-SAMPLE-FILE
	Detection: X-SAMPLE-EXTENSION
	DIS: X-SAMPLE-CHECKSUM
	DIS: X-Sample-HeuristicVector
	DIS: X-Sample-HeuristicScore 
	DIS: X-Sample-Registry
		===X-Platform* omitted - handled  by DIS object===
	DIS: X-DATE-SUBMITTED
	DIS: X-DATE-SAMPLE-STATUS //set to todays date
	Detection: X-DATE-QUARANTINED
	Detection: X-DATE-MODIFIED
	Detection: X-DATE-CREATED
	Detection: X-DATE-ACCESSED
		==X-Customer* omitted==
		==X-Content* omitted==
	Detection: X-CLIENT-SCAN-TECHNOLOGY
	Detection: X-CLIENT-SCAN-VIRUS-TYPE
	Detection: X-CLIENT-SCAN-VIRUS-NAME
	Detection: X-CLIENT-SCAN-VIRUS-IDENTIFIER
	Detection: X-CLIENT-SCAN-SIGNATURES-VERSION
	Detection: X-CLIENT-SCAN-SIGNATURES-SEQUENCE
	Detection: X-CLIENT-SCAN-RESULT
	DIS: X-CHECKSUM-METHOD
*/

} //namspace ''

CAvSubmissionSample::CAvSubmissionSample(void)
{
}

CAvSubmissionSample::~CAvSubmissionSample(void)
{
}

HRESULT CAvSubmissionSample::GetTypeId(GUID& typeId) const throw()
{
	typeId = dis::TYPEID_DIS_AVSAMPLE_TRANSLATED;
	return S_OK;
}
bool CAvSubmissionSample::GetObjectId(SYMOBJECT_ID& oid) const throw()
{
	oid = CLSID_AvSubmissionSampleChild;
	return true;
}

HRESULT CAvSubmissionSample::LoadAttributes() throw()
{
	utils::BoolX brx(HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
	STAHLSOFT_HRX_TRY(hr)
	{
		//call base first...
		hrx << CAvSubmission::LoadAttributes();

		cc::IStringPtr pString;
		cc::IStringPtr pOutString;

		ccLib::CStringW sTemp;
		cc::IKeyValueCollectionPtr pAttrib = m_pAttrib;
		DWORD dw = 0;
		SYSTEMTIME st = {0};
		bool b = false;

		hrx << (pAttrib == NULL ? E_POINTER : S_OK);
		if(!pAttrib->GetCloneInterface())
		{
			CCTRCTXW0(_T("Setting pAttrib->CloneInterface(true)"));
			brx << pAttrib->SetCloneInterface(true);
		}
		pString.Attach(ccSym::CStringImpl::CreateStringImpl());
		CCTHROW_BAD_ALLOC(pString);

		brx << pAttrib->GetValue(AVSubmit::IAVSubmission::eScanResult, dw);
		brx << pAttrib->GetValue(AVSubmit::IAVSubmission::eManual, b);
		if(b) 
			brx << pString->SetStringW(L"manual");
		else if(dw == AVSubmit::IAVSubmission::eHueristic)
			brx << pString->SetStringW(L"heurisitc");
		else
			brx << pString->SetStringW(L"unknown");
		brx << pAttrib->SetValue(dis::eSampleReason, pString);



		CCTRCTXI3(_T("Setting QDate: %04d-%02d-%02d"), st.wYear, st.wMonth, st.wDay);
		brx << pAttrib->GetValue(AVSubmit::IAVSubmission::eDateQuarantined, st);
		brx << pAttrib->SetValue(dis::eSampleDateQuarantined, st);

		CCTRCTXI0(_T("Setting priority"));
		brx << pAttrib->SetValue(dis::eSamplePriority, DWORD(500));

		CCTRCTXI0(_T("Setting sample-type to file"));
		pString->SetStringW(L"file");
		brx << pAttrib->SetValue(dis::eSampleType, pString);

		hrx << SetFileInfo(pAttrib);
		hrx << SetClientScanInfo(pAttrib);

		//these are optional...
		(void) GetHueristicData(pAttrib);
		(void) GetRegistryData(pAttrib);
		(void) GetCustomerData(pAttrib);
	}
	CATCH_RETURN(hr);
}

HRESULT CAvSubmissionSample::SetClientScanInfo(cc::IKeyValueCollection* pAttrib) throw()
{
	utils::BoolX brx;
	STAHLSOFT_HRX_TRY(hr)
	{
		cc::IStringPtr pOutString;
		DWORD dw = 0;

		brx << utils::GetString(ISubmission::eComponentName, pAttrib, pOutString.m_p);
		brx << pAttrib->SetValue(dis::eClientScanTechnology, pOutString);

		brx << pAttrib->GetValue(dis::eScanVID, dw);
		brx << pAttrib->SetValue(dis::eClientScanVID, dw);

		brx << utils::GetString(dis::eScanVirusName, pAttrib, pOutString.m_p);
		brx << pAttrib->SetValue(dis::eClientScanVirusName, pOutString);

		brx << utils::GetString(dis::eSignaturesVersion, pAttrib, pOutString.m_p);
		brx << pAttrib->SetValue(dis::eClientSigVersion, pOutString);

		brx << pAttrib->GetValue(dis::eSignaturesSequence, dw);
		brx << pAttrib->SetValue(dis::eClientSigSequence, dw);

		brx << utils::GetString(dis::eScanResult, pAttrib, pOutString.m_p);
		brx << pAttrib->SetValue(dis::eClientScanResult, pOutString);

	}
	CATCH_RETURN(hr)
}

HRESULT CAvSubmissionSample::SetFileInfo(cc::IKeyValueCollection* pAttrib) throw()
{
	utils::BoolX brx;
	STAHLSOFT_HRX_TRY(hr)
	{
		cc::IStringPtr pOutString, pString;
		ccLib::CStringW sTemp;
		ccEraser::IRemediationActionPtr pFile;
		cc::IKeyValueCollectionPtr pFileProps;
		cc::IStreamPtr pStream;
		ccEraser::eResult eRes = ccEraser::Fail;

		pString.Attach(ccSym::CStringImpl::CreateStringImpl());
		CCTHROW_BAD_ALLOC(pString);

		hrx << GetFileData(pFile.m_p, pStream.m_p);
		
		if(pFile != NULL)
		{
			if(utils::GetString(AVSubmit::IAVSubmission::eFilePath, pAttrib, pOutString.m_p) && pOutString->GetLength() > 0)
			{
				//primarily for clean stealth file
				CCTRCTXI1(_T("Using path = eFilePath, %ld"), pOutString->GetStringW());
			}
			else
			{
				eRes = pFile->GetProperties(pFileProps.m_p);
				if(ccEraser::Failed(eRes) || pFileProps == NULL)
				{
					CCTRCTXE1(_T("Failed to get props: %d"), eRes);
					hrx << E_FAIL;
				}
				//path
				brx << utils::GetString(ccEraser::IRemediationAction::Path, pFileProps, pOutString.m_p);
				CCTRCTXI1(_T("Eraser File Path = %ls"), pOutString->GetStringW());
			}

			ccLib::CStringW sPath = pOutString->GetStringW();

			brx << pAttrib->SetValue(dis::eSampleFile, pOutString);
			//brx << pAttrib->SetValue(ISubmission::eDetails, pOutString);

			//extension
			ccLib::CSplitPath splitter;
			brx << splitter.SplitPath(ATL::CW2T(pOutString->GetStringW()));
			sTemp = splitter.GetExtension();
			sTemp.TrimLeft(L'.');
			brx << pString->SetStringW(sTemp);
			brx << pAttrib->SetValue(dis::eSampleExtension, pString);
			
			hrx << GetFileInfo(pFile, sPath, pAttrib);
		}
		else if(pStream != NULL)
		{
			//TODO: get stream name from quarantine...!?!
			//TODO: What is the format of a manually added file in quarantine?
			//		Just another Remediation?
			//sPath = L"c:\\unknown.file";
			hrx << HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
		}
		else
		{
			CCTRCTXE0(_T("No remeidation or data stream"));
			hrx << HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
		}
	}
	CATCH_RETURN(hr);
}

HRESULT CAvSubmissionSample::GetFileInfo(ccEraser::IRemediationAction* pFile, const WCHAR* pszPath, cc::IKeyValueCollection* pAttr)
{
	utils::BoolX brx;
	STAHLSOFT_HRX_TRY(hr)
	{
		cc::IStringPtr pPath;
		cc::IKeyValueCollectionPtr pProps;
		bool bPresent = false;

		WIN32_FILE_ATTRIBUTE_DATA attr = {0};
		SYSTEMTIME stModified, stCreated, stAccessed;
		GetSystemTime(&stModified);
		stCreated = stAccessed = stModified;

		hrx << (pFile == NULL ? E_INVALIDARG : S_OK);
		hrx << ((pszPath && *pszPath ) ? S_OK : E_INVALIDARG);
	    
		if(ccEraser::Succeeded(pFile->IsPresent(bPresent)) && bPresent)
		{
			if(::GetFileAttributesExW(pszPath, GetFileExInfoStandard, &attr))
			{
				FileTimeToSystemTime(&attr.ftCreationTime, &stCreated);
				FileTimeToSystemTime(&attr.ftLastAccessTime, &stAccessed);
				FileTimeToSystemTime(&attr.ftLastWriteTime, &stModified);
			}
			else
			{
				CCTRCTXW1(_T("Failed to get attrib: %d"), GetLastError());
			}
		}
		brx << pAttr->SetValue(dis::eSampleDateAccessed, stAccessed);
		brx << pAttr->SetValue(dis::eSampleDateCreated, stCreated);
		brx << pAttr->SetValue(dis::eSampleDateModified, stModified);
	}
	CATCH_RETURN(hr);
}

HRESULT CAvSubmissionSample::GetHueristicData(cc::IKeyValueCollection* pAttrib)
{
	//TODO: load COH Vector / Score
	HRESULT hr = S_OK;
	for(;;)
	{
		ccEraser::IAnomalyPtr pAnomaly;
		ccEraser::eResult eRes = ccEraser::Fail;
		cc::IKeyValueCollectionPtr pProps;

		hr = m_pAVSubmission->GetAnomaly(pAnomaly.m_p);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to get anomaly: 0x%08X"), hr);
			break;
		}

		if(pAnomaly == NULL)
		{
			CCTRCTXE0(_T("pAnomaly == NULL"));
			hr = E_POINTER;
			break;
		}

		eRes = pAnomaly->GetProperties(pProps.m_p);
		if(ccEraser::Failed(eRes) || pProps == NULL)
		{
			CCTRCTXE1(_T("Failed to get props: %d"), eRes);
			hr = E_FAIL;
			break;
		}

		DWORD dwScore = 0;
		ISymBasePtr pBase;

		if(pProps->GetValue(ccEraser::IAnomaly::HeuristicVector, pBase.m_p) == false || pBase == NULL)
		{
			CCTRCTXE0(_T("Faled to get hueristic data"));
			hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
			break;
		}

		if(pAttrib->SetValue(dis::eHueristicVector, pBase) == false)
		{
			CCTRCTXE0(_T("Failed to set hueristic data"));
			hr = E_UNEXPECTED;
			break;
		}

		if(pProps->GetValue(ccEraser::IAnomaly::HeuristicRank, dwScore) == false)
		{
			CCTRCTXW0(_T("Failed to get rank"));
		}
		
		if(pAttrib->SetValue(dis::eHuerisitcScore, dwScore) == false)
		{
			CCTRCTXW0(_T("Failed to set hueristic score"));
		}
		
		break;

	}
	return hr;
}

HRESULT CAvSubmissionSample::GetRegistryData(cc::IKeyValueCollection* pAttrib)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		utils::CRegKeyFormat rk;
		CCTRCTXI1(_T("Data size = %d"), m_list.size());
		for(DataList::iterator i = m_list.begin(), e = m_list.end(); i!=e; ++i)
		{
			if(i->m_dwType != ccEraser::RegistryRemediationActionType)
			{
				CCTRCTXI2(_T("type != registry (%d != %d)"), i->m_dwType, ccEraser::RegistryRemediationActionType);
				continue;
			}
			
			cc::IStreamPtr pStream;
			ccEraser::IRemediationActionPtr pAction;
			HRESULT hr2 = m_pAVSubmission->GetRemediation(i->m_dwIndex, pAction.m_p, pStream.m_p);
			if(FAILED(hr2))
			{
				CCTRCTXE2(_T("Failed to load remeidation @ %d: 0x%08X"), i->m_dwIndex, hr2);
				continue;
			}
			hr2 = rk.AddObject(pAction, pStream);
			if(FAILED(hr2))
			{
				CCTRCTXE2(_T("Failed to add object @ %d: 0x%08X"), i->m_dwIndex, hr2);
				continue;
			}
		}

		cc::IStringPtr pData;
		hrx << rk.ToString(pData.m_p);
		if(!pAttrib->SetValue(dis::eSampleRegistry, pData))
			hrx << E_OUTOFMEMORY;
	}
	CATCH_RETURN(hr);
}

HRESULT CAvSubmissionSample::GetCustomerData(cc::IKeyValueCollection* pAttrib) throw()
{
	utils::BoolX brx;
	STAHLSOFT_HRX_TRY(hr)
	{
		#define CreateString(x) (pString.Attach(ccSym::CStringImpl::CreateStringImpl((const WCHAR*)(x))), pString)
		cc::IStringPtr pString;

		brx << pAttrib->SetValue(dis::eCustomerContactEmail, CreateString(L"consumer@symantec.com"));
		brx << pAttrib->SetValue(dis::eCustomerContactName, CreateString(L"Symantec Customer"));
		brx << pAttrib->SetValue(dis::eCustomerContactPhone, CreateString(L"1-800-555-1212"));
		brx << pAttrib->SetValue(dis::eCustomerName, CreateString(L"Consumer"));
		brx << pAttrib->SetValue(dis::eCustomerIdentifier, CreateString(L"1"));
		brx << pAttrib->SetValue(dis::eCustomerType, CreateString(L"retail"));

		#undef CreateString
	}
	CATCH_RETURN(hr);
}

HRESULT CAvSubmissionSample::IsSubmittable(cc::IStream* pStream) const throw()
{
	//simple check for PE file format
	HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
	ULONGLONG old_pos = 0;
	for(;;)
	{
		if(pStream == NULL)
		{
			CCTRCTXE0(_T("pStream == NULL"));
			hr = E_INVALIDARG;
			break;
		}

		if(pStream->GetPosition(old_pos) == false)
		{
			CCTRCTXE0(_T("pStream->GetPosition(old_pos) == false"));
			hr = E_FAIL;
			break;
		}

		if(m_pAttrib == NULL)
		{
			CCTRCTXE0(_T("m_pAttrib == NULL"));
			hr = E_POINTER;
			break;
		}

		bool bManual = false;
		if(m_pAttrib->GetValue(AVSubmit::IAVSubmission::eManual, bManual) == false)
		{
			CCTRCTXW0(_T("Failed to get manual submission flag from attrib"));
		}

		if(bManual == true)
		{
			CCTRCTXI0(_T("Manual Submission, not checking for PE file"));
			hr = S_OK;
			break;
		}

		if(pStream->SetPosition(0) == false)
		{
			CCTRCTXE0(_T("pStream->SetPosition(0) == false"));
			break;
		}

		IMAGE_DOS_HEADER dos_hdr = {0};
		if(pStream->Read(&dos_hdr, sizeof(dos_hdr)) == false)
		{
			CCTRCTXE0(_T("Failed to read dos hdr"));
			break;
		}
		if(dos_hdr.e_magic != IMAGE_DOS_SIGNATURE)
		{
			CCTRCTXE1(_T("invalid sig: 0x%04X"), dos_hdr.e_magic);
			break;
		}
		//move to offset of NT header in the file
		if(pStream->SetPosition(dos_hdr.e_lfanew) == false)
		{
			CCTRCTXE1(_T("failed to seak to nt hdr @ %d"), dos_hdr.e_lfanew);
			break;
		}
		IMAGE_NT_HEADERS nt_hdr = {0};
		if(pStream->Read(&nt_hdr, sizeof(nt_hdr)) == false)
		{
			CCTRCTXE0(_T("Failed to read nt hdr"));
			break;
		}
		if(nt_hdr.Signature != IMAGE_NT_SIGNATURE)
		{
			CCTRCTXE1(_T("Invalid NT sig: 0x%08X"), nt_hdr.Signature);
			break;
		}

		CCTRCTXI0(_T("Success"));
		hr = S_OK;
		break;
	}
	if(pStream != NULL)
	{
		if(pStream->SetPosition(old_pos) == false)
			CCTRCTXE0(_T("pStream->SetPosition(0) == false"));
	}
	if(FAILED(hr))
	{
		CCTRCTXW1(_T("Submission is not a PE file: error 0x%08X"), hr);
		AVSubmit::IAVSubmission2QIPtr pAVSubmission = m_pAVSubmission;
		if(pAVSubmission != NULL)
		{
			HRESULT hr2 = pAVSubmission->SetSubmitted(false);
			if(FAILED(hr2))
				CCTRCTXE1(_T("Failed to unset submitted flag: 0x%08X"), hr2);
		}
		else
			CCTRCTXE0(_T("Failed to QI for IAVSubmission2"));
	}
	return hr;
}

}
