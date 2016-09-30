////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\submissionbase.h"
#include "ccSymKeyValueCollectionImpl.h"
#include "ccmodulenames.h"
#include "AVQBackupTypes.h"
#include "ccSymMemoryStreamImpl.H"
#include "ccSymStringImpl.h"
#include "ccVersionInfo.h"
#include "AVSubmitRes.h"
#include "ccSymDebugOutput.h"
#include "AVInterfaceLoader.h"
#include "Utils.h"

extern ccSym::CDebugOutput g_DebugOutput;

namespace AVSubmit {
namespace {

	template<typename T, const SYMGUID* _IID>
	HRESULT CreateEraserObject(ccEraser::IEraser4* pEraser, cc::IStream* pStream, T*& t)
	{
		utils::FreeObject(t);
		ISymBasePtr pBase;
		ccEraser::eResult eRes = pEraser->CreateObjectFromStream(pStream, NULL, pBase.m_p);
		if(ccEraser::Failed(eRes))
		{
			CCTRCTXE1(_T("Failed to create eraser obj: %d"), eRes);
			return E_FAIL;
		}
		SYMRESULT sr = pBase->QueryInterface(*_IID, (void**)&t);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE0(_T("Failed to QI for eraser obj"));
			return E_NOINTERFACE;
		}
		return S_OK;
	}
}

//some component names...
namespace components {
	static const WCHAR* const g_szOEH = L"OEH";
	static const WCHAR* const g_szVXMS = L"VxMS";
	static const WCHAR* const g_szAV = L"AV Engine";
	static const WCHAR* const g_szCOHTrojan = L"Trojan";
	static const WCHAR* const g_szCOHKeylogger = L"Keylogger";
	static const WCHAR* const g_szCOHSpyware = L"Spyware";
	static const WCHAR* const g_szCOHPWSteal = L"PWSteal";
	static const WCHAR* const g_szCOHGeneral = L"General";
	static const WCHAR* const g_szCOH = L"COH";
	static const WCHAR* const g_szCOL = L"COL";
	static const WCHAR* const g_szManual = L"Manual";

	//MSL specific
	static const TCHAR* const g_szMSLKey = _T("MSL");
	static const TCHAR* const g_szMSLDLL = _T("MSL.DLL");

	//COH specific
	static const TCHAR* const g_szCOHKey = _T("COHDIR");
	static const TCHAR* const g_szCOHDLL = _T("AHS.DLL");
}

CSubmissionBase::CSubmissionBase(void) : m_bInitializing(false)
{
}

CSubmissionBase::~CSubmissionBase(void)
{
}

HRESULT CSubmissionBase::Initialize(const cc::IKeyValueCollection* pAttrib) throw()
{
	TRACE_SCOPE(s0);
	STAHLSOFT_HRX_TRY(hr)
	{
		m_bInitializing = true;
		m_pAttributes = pAttrib;
		if(m_pAttributes == NULL)
		{
			CCTRCTXE0(_T("m_pAttributes == NULL"));
			hrx << E_POINTER;
		}
		m_pAttributes->SetValue(eScanResult, DWORD(eHueristic));
		hrx << LoadSubmissionData();
		hrx << CheckDefDate();
		SetDetails();
	}
	CATCH_RETURN(hr)
}

// IAVSubmission
HRESULT CSubmissionBase::GetAnomaly(ccEraser::IAnomaly*& pAnomaly) const throw()
{
	TRACE_SCOPE(s0);
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	if(m_pAttributes == NULL)
	{
		CCTRCTXE0(_T("m_pAttributes == NULL"));
		return E_POINTER;
	}

	utils::FreeObject(pAnomaly);
	HRESULT hr = const_cast<CSubmissionBase*>(this)->LoadSubmissionData();
	if(FAILED(hr))
		return hr;

	hr = E_POINTER;
	pAnomaly = m_data.pAnomaly;
	if(pAnomaly != NULL)
	{
		pAnomaly->AddRef();
		hr = S_OK;
	}
	return hr;
}

HRESULT CSubmissionBase::GetRemediationCount(size_t& nSize) const throw()
{
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	if(m_pAttributes == NULL)
	{
		CCTRCTXE0(_T("m_pAttributes == NULL"));
		return E_POINTER;
	}
	HRESULT hr = const_cast<CSubmissionBase*>(this)->LoadSubmissionData();
	if(SUCCEEDED(hr))
		nSize = m_data.list.size();
	else
		CCTRCTXE1(_T("Failed to laod data: 0x%08X"), hr);
	CCTRCTXI1(_T("data size = %Iu"), nSize);
	return hr;
}

HRESULT CSubmissionBase::GetRemediation(size_t nIndex, ccEraser::IRemediationAction*& pRem, cc::IStream*& pStream) const throw()
{
	TRACE_SCOPE(s0);
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	STAHLSOFT_HRX_TRY(hr)
	{
		CCTRCTXI1(_T("Load Remediation @ %d"), nIndex);
		if(m_pAttributes == NULL)
		{
			CCTRCTXE0(_T("m_pAttributes == NULL"));
			hrx << E_POINTER;
		}
		hrx << const_cast<CSubmissionBase*>(this)->LoadSubmissionData();
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

// IAVSubmissionInternal
HRESULT CSubmissionBase::SetSubmitted(bool bSubmitted) throw()
{
	TRACE_SCOPE(s0);
	return SetSubmittedKey(bSubmitted);
}

//ISubmission
HRESULT CSubmissionBase::GetAttributes(cc::IKeyValueCollection*& pAttrib) const throw()
{
	TRACE_SCOPE(s0);
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	utils::FreeObject(pAttrib);
	pAttrib = m_pAttributes;
	if(pAttrib != NULL)
		pAttrib->AddRef();
	return pAttrib == NULL ? E_POINTER : S_OK;
}

HRESULT CSubmissionBase::GetSubmissionData(const cc::IKeyValueCollection* pCriteria, cc::IStream*& pData) throw()
{
	TRACE_SCOPE(s0);
	//ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	//nothing to do for now...
	//we will implement this once these submissions no longer need translation (new version of DIS, etc...)
	return E_NOTIMPL;
}

HRESULT CSubmissionBase::ProcessResponse(const cc::IKeyValueCollection* pResponse) throw()
{
	TRACE_SCOPE(s0);
	//ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	return S_OK; //nothing to do for now...
}

bool CSubmissionBase::Save(ccLib::CArchive& Archive) const throw()
{
	//should already be locked by parent class...
	bool bRes = false;
	utils::BoolX brx;
	HRESULT hr = S_OK;
	try
	{
		Archive << DWORD(eVersion);
		brx << ccSym::CPersist::Write(m_pAttributes, NULL, NULL, &Archive, NULL, NULL);
		bRes = true;
	}
	catch(ccLib::CArchive::CArchiveException&)
	{
		CCTRCTXE0(_T("Caught CArchiveException"));
	}
	CATCH_NO_RETURN(hr);

	return bRes;
}

bool CSubmissionBase::Load(ccLib::CArchive& Archive) throw()
{
	//should already be locked by parent class...
	bool bRes = false;
	utils::BoolX brx;
	HRESULT hr = S_OK;
	try
	{
		DWORD dwVer = 0;

		m_pAttributes.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
		CCTHROW_BAD_ALLOC(m_pAttributes);

		Archive >> dwVer;
		brx << ccSym::CPersist::Read(m_pAttributes, NULL, NULL, &Archive, NULL, NULL);
		if(g_DebugOutput.IsOutputEnabled(ccLib::CDebugOutput::eInformationDebug))
		{
			DWORD dwVid = DWORD(-1);
			cc::IStringQIPtr pString;
			ISymBasePtr pBase;
			(void) m_pAttributes->GetValue(eVID, dwVid);
			(void) m_pAttributes->GetValue(eVirusName, pBase.m_p);
			pString = pBase;
			if(pString == NULL)
			{
				CCTRCTXI1(_T("VID %d: Name == NULL"), dwVid);
			}
			else
				CCTRCTXI2(_T("Vid %d [%ls]"), dwVid, pString->GetStringW());
		}
		bRes = true;
	}
	catch(ccLib::CArchive::CArchiveException&)
	{
		CCTRCTXE0(_T("Caught CArchiveException"));
	}
	CATCH_NO_RETURN(hr);

	return bRes;
}
////////////////////////////////////////////////////////////////////
// Internal functions

HRESULT CSubmissionBase::LoadData()
{
	TRACE_SCOPE(s0);
	if(m_data.bInit)
		return S_OK;

	STAHLSOFT_HRX_TRY(hr)
	{
		ccLib::CStringW sComponent;
		ccLib::CStringW sVersion;
		AVModule::IAVDefInfoPtr pDefs;
		ccEraser::IEraser4Ptr pEraser;
		QBackup::IQBackupPtr pBackup;
		
		hrx << GetDefManager(pDefs.m_p);
		hrx << GetEraser(pEraser.m_p);
		hrx << GetBackup(pBackup.m_p);

		GUID guid = GUID_NULL;
		if(!m_pAttributes->GetValue(eSetId, guid))
		{
			int type = m_pAttributes->GetType(eSetId);
			CCTRCTXI1(_T("Type = %d"), type);
			//if the sample has not yet been archived, do so now.
			HRESULT hr2 = BackupSample(guid);
			if(FAILED(hr2) && hr2 == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) && !MustBePresent())
			{
				hrx << SetOEH();
				return S_OK;
			}
			else
				hrx << hr2;
		}

		CCTRACEI(CCTRCTX _T(" Processing QB Set: ") SYMGUID_FORMAT_STR((&guid)));

		QBackup::IQBackupSetPtr pSet;
		HRESULT sr = pBackup->RetrieveBackupSetByID(guid, pSet.m_p);
		if(FAILED(sr) || pSet == NULL)
		{

			CCTRCTXE1(_T("backup set not found"), sr);
			hrx << HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
		}

		GUID givenType = GUID_NULL;
		sr = pSet->GetGivenType(givenType);
		if(FAILED(sr) || (!IsManual() && givenType == GUID_NULL))
		{
			CCTRCTXE1(_T("Invalid bakup set type: 0x%08X"), sr);
			hrx << TYPE_E_TYPEMISMATCH;
		}
		hrx << LoadDate(pSet);
		if(givenType == avQBTypes::GT_AVANOMALYSET || givenType == GT_SubmitUnremediated)
		{
			//auto-protect, global & context scans.
			cc::IKeyValueCollectionPtr pProps;
			hrx << pSet->GetProperties(pProps.m_p);
			//non-fatal if anomaly is not present
			if(pProps->GetValue(avQBTypes::avASProp_SerializedAnomalyBackupID, guid))
			{
				CCTRCTXE0(_T("anomaly id found"));
				cc::IStreamPtr pStream;
				HRESULT hr2 = RestoreStream(pSet, guid, avQBTypes::GT_AVANOMALY, pStream.m_p);
				if(SUCCEEDED(hr2))
				{
					hr2 = CreateEraserObject<ccEraser::IAnomaly, &ccEraser::IID_Anomaly>(pEraser, pStream, m_data.pAnomaly.m_p);
					if(FAILED(hr2))
					{
						CCTRCTXE1(_T("Failed to create anomaly: 0x%08X"), hr2);
					}
					else
					{

						hr2 = GetThreatCats();
						if(FAILED(hr2))
						{
							CCTRCTXE1(_T("GetThreatCats == 0x%08X"), hr2);
						}
						DumpAnomaly(m_data.pAnomaly);
					}
				}
				else
					CCTRCTXE1(_T("Failed to restore anomaly stream: 0x%08X"), hr2);
			}
			else
				CCTRCTXW0(_T("No AnomalyID available"));

			if(IsManual())
			{
				//manual submissions will be an anomaly set..
				CCTRCTXI0(_T("Manual Submission"));
				hrx << SetManual();
			}
			else
			{
				hrx << SetAV();
			}
			hrx << LoadRemediations(pSet);
		}
		else if(givenType == avQBTypes::GT_AVCOMPRESSEDSET)
		{
			//currently, the extracted infected component of the archive is saved,
			//but only if backup is requested.
			hrx << SetAVCompressed();
			hrx << LoadRemediations(pSet);
		}
		else if(givenType == avQBTypes::GT_AVEMAILSET)
		{
			hrx << SetEmail();
			hrx << LoadRemediations(pSet);
		}
		else if(givenType == avQBTypes::GT_AVOEHSET || givenType == GT_SubmitOEHSet)
		{
			hrx << SetOEH();
			//we get one remediation / undo action pair...
			hrx << LoadRemediations(pSet);
		}
		else if(givenType == avQBTypes::GT_CLEANSTEALTHBACKUPSET)
		{
			hrx << SetVXMS(pSet);
			hrx << LoadRemediations(pSet);			
		}
		else
		{
			CCTRCTXI0(_T("Unknown givenType"));
			m_data.bInit = true;
			hrx << HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
		}
		m_data.bInit = true;
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::GetDefManager(AVModule::IAVDefInfo*& pInfo) const
{
	utils::FreeObject(pInfo);
	HRESULT hr = S_OK;
	if(m_pDefInfo == NULL)
	{
		mgr::IManagerPtr pManager;
		hr = GetManager(pManager.m_p);
		if(SUCCEEDED(hr))
			hr = pManager->GetAVDefInfo(m_pDefInfo.m_p);
	}
	if(SUCCEEDED(hr))
	{
		pInfo = m_pDefInfo;
		if(pInfo != NULL)
			pInfo->AddRef();
		else
			hr = E_POINTER;
	}
	return hr;
}

HRESULT CSubmissionBase::GetAVThreatInfo(AVModule::IAVThreatInfo*& pInfo) const
{
	utils::FreeObject(pInfo);
	HRESULT hr = S_OK;
	if(m_pThreatInfo == NULL)
	{
		mgr::IManagerPtr pManager;
		hr = GetManager(pManager.m_p);
		if(SUCCEEDED(hr))
			hr = pManager->GetAVThreatInfo(m_pThreatInfo.m_p);
	}
	if(SUCCEEDED(hr))
	{
		pInfo = m_pThreatInfo;
		if(pInfo != NULL)
			pInfo->AddRef();
		else
			hr = E_POINTER;
	}
	return hr;
}

HRESULT CSubmissionBase::GetEraser(ccEraser::IEraser4*& pe) const
{
	STAHLSOFT_HRX_TRY(hr)
	{
		utils::FreeObject(pe);
		if(m_pEraser == NULL)
		{
			mgr::IManagerPtr pManager;
			hrx << GetManager(pManager.m_p);
			hrx << pManager->GetEraser(m_pEraser.m_p);
		}
		pe = m_pEraser;
		pe->AddRef();
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::GetBackup(QBackup::IQBackup*& pBackup) const
{
	STAHLSOFT_HRX_TRY(hr)
	{
		utils::FreeObject(pBackup);
		if(m_pBackup == NULL)
		{
			mgr::IManagerPtr pManager;
			hrx << GetManager(pManager.m_p);
			hrx << pManager->GetQBackup(m_pBackup.m_p);
		}
		pBackup = m_pBackup;
		pBackup->AddRef();
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::GetManager(mgr::IManager*& pManager) const
{
	HRESULT hr = S_OK;
	if(m_pManager == NULL)
	{
		hr = GetObjectManager(m_pManager.m_p);
	}
	if(SUCCEEDED(hr))
	{
		utils::FreeObject(pManager);
		pManager = m_pManager;
		m_pManager.AddRef();
	}
	return hr;
}

HRESULT CSubmissionBase::RestoreStream(QBackup::IQBackupSet* pSet, REFGUID id, REFGUID type, cc::IStream*& pOutStream, size_t nIndex)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		utils::FreeObject(pOutStream);
		QBackup::IQBackupItemBasePtr pBase;
		ccEraser::IEraser4Ptr pEraser;
		
		hrx << GetEraser(pEraser.m_p);

		if(nIndex != -1)
		{
			GUID guid;
			hrx << pSet->GetItem(nIndex, pBase.m_p, &guid);
		}
		else
		{
			hrx << pSet->GetItemByID(id, pBase.m_p);
		}

		if(type != GUID_NULL)
		{
			//CCTRACEI(CCTRCTX _T(" Requested Type: ") SYMGUID_FORMAT_STR((&type)));
			GUID given = {0};
			hrx << pBase->GetGivenType(given);
			//CCTRACEI(CCTRCTX _T(" Actual Type: ") SYMGUID_FORMAT_STR((&given)));
			if(!::InlineIsEqualGUID(given, type))
				hrx << TYPE_E_TYPEMISMATCH;
		}

		QBackup::IQBackupItemStreamQIPtr pStreamObj = pBase;
		if(pStreamObj == NULL)
		{
			CCTRCTXE0(_T("Item is not a stream backup, serialize items not supported yet."));
			hrx << E_NOINTERFACE;
		}
		
		cc::IStreamPtr pStream;
		pStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		CCTHROW_BAD_ALLOC(pStream);

		hrx << pStreamObj->Restore(pStream.m_p);
		pOutStream = pStream.Detach();
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::LoadRemediations(QBackup::IQBackupSet* pSet) throw()
{
	TRACE_SCOPE(s0);
	size_t nSize = pSet->GetItemCount();
	if(nSize < 1)
		return S_OK;

	ccEraser::IEraser4Ptr pEraser;
	HRESULT hr = GetEraser(pEraser.m_p);
	if(FAILED(hr))
		return hr;

	m_data.list.clear();

	//load remedation objects first...
	for(size_t i = 0; i < nSize; ++i)
	{
		Remediation r;
		QBackup::IQBackupItemBasePtr pItem;
		GUID itemId = GUID_NULL;
		HRESULT sr = pSet->GetItem(i, pItem.m_p, &itemId);
		if(FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to get qbackup item: 0x%08X"), sr);
			continue;
		}
		cc::IStreamPtr pStream;
		if(FAILED(RestoreStream(pSet, itemId, avQBTypes::GT_AVREMEDIATIONACTION, pStream.m_p)))
			continue;

		cc::IKeyValueCollectionPtr pProps;
		sr = pItem->GetProperties(pProps.m_p);
		if(FAILED(sr) || pProps == NULL)
			continue;

		hr = CreateEraserObject<ccEraser::IRemediationAction, &ccEraser::IID_RemediationAction>(pEraser, pStream, r.pRemediation.m_p);
		if(FAILED(hr))
			continue;

		pProps->GetValue(avQBTypes::avRAProp_MatchingUndoID, r.guidUndo);

		DumpRemediation(i, r.pRemediation);
		
		ccLib::CExceptionInfo exInfo;
		try { m_data.list.push_back(r); }
		CCCATCHMEM(exInfo);
		if(exInfo.IsException())
			return E_OUTOFMEMORY;
	}

	//we'll not load up the undo streams on init.  We don't need them.
	if(m_bInitializing == false)
	{
		for(Remediation::List::iterator i = m_data.list.begin(), e = m_data.list.end(); i!=e; ++i)
		{
			if(i->guidUndo == GUID_NULL)
			{
				CCTRCTXW0(_T("Matching Undo ID is NULL"));
				continue;
			}
			HRESULT hr = RestoreStream(pSet, i->guidUndo, avQBTypes::GT_AVREMEDIATIONACTIONUNDO, i->pStream.m_p);
			if(FAILED(hr))
			{
				CCTRCTXE1(_T("Failed to restore undo action: 0x%08X"), hr);
				//just don't have one... keep going anywho..
			}
			else
			{
				CCTRCTXI1(_T("Restored stream : %p"), i->pStream.m_p);
			}
		}
	} 

	CCTRCTXI1(_T("Data Size %d"), m_data.list.size());
	//for now, this function will always succeed...
	//up to caller to determine if 0 remediations loaded means OK
	return S_OK;
}

HRESULT CSubmissionBase::LoadSubmissionData() throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << LoadData();
		//TODO: additional work pre/post data load...

	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::SetManual() throw()
{
	utils::BoolX brx;
	STAHLSOFT_HRX_TRY(hr)
	{
		if(IsComponentSet())
			return S_OK;

		DWORD dwVid = 0;
		brx << m_pAttributes->GetValue(eVID, dwVid);
		CCTRCTXI1(_T("VID = %d"), dwVid);
		
		if(dwVid != 0)
		{
			//make sure this item is submittable...
			//If its known, then Security Response doesnt want it.

			hrx << SetAV();
		}
		//TODO: manual / unknown category?!?
		hrx << SetComponent(components::g_szManual, L"1.0", eCategoryHueristic);
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::SetAV() throw()
{
	//TODO: sort out component and version the submission came from...
	STAHLSOFT_HRX_TRY(hr)
	{
		if(this->IsComponentSet())
			return S_OK;

		DWORD dwVid = 0;
		LPCWSTR sComponent = NULL;
		LPCWSTR sSubComponent = NULL;
		ccLib::CStringW sVer;
		bool bManual = IsManual();
		Category cat = eCategoryHueristic;

		if(!m_pAttributes->GetValue(eVID, dwVid))
		{
			if(!bManual)
			{
				CCTRCTXE0(_T("VID not found"));
				hrx << HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
			}
		}
		CCTRCTXI1(_T("VID = %d"), dwVid);

		switch(dwVid)
		{
		case eVID_COH_Positive:
		case eVID_COH_Suspicious:
			hrx << GetCOHVer(sVer);
			sSubComponent = components::g_szCOHGeneral;
			sComponent = components::g_szCOH;
			break;
		case eVID_COH_PWSteal:
			hrx << GetCOHVer(sVer);
			sSubComponent = components::g_szCOHPWSteal;
			sComponent = components::g_szCOH;
			break;
		case eVID_COH_Trojan:
			hrx << GetCOHVer(sVer);
			sSubComponent = components::g_szCOHTrojan;
			sComponent = components::g_szCOH;
			break;
		case eVID_COH_Keylogger:
			hrx << GetCOHVer(sVer);
			sSubComponent = components::g_szCOHKeylogger;
			sComponent = components::g_szCOH;
			break;
		case eVID_COH_Spyware:
			hrx << GetCOHVer(sVer);
			sSubComponent = components::g_szCOHSpyware;
			sComponent = components::g_szCOH;
			break;
		case eVID_COL:
			hrx << GetCOLVer(sVer);
			sComponent = components::g_szCOL;
			break;
		default:
			if(dwVid == 0 && bManual)
			{
				CCTRCTXI0(_T("Allowing VID==0 due to manual submission...."));
			}
			else
			{
				hrx << CheckBloodhound(dwVid);
			}
			hrx << GetAVVer(sVer);
			sComponent = components::g_szAV;
			cat = eCategoryAntiVirus;
			break;
		}
		hrx << SetComponent(sComponent, sVer, cat, sSubComponent);
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::SetAVCompressed() throw()
{
	return SetAV();
}

HRESULT CSubmissionBase::SetEmail() throw()
{
	//these are just compressed files in quarantine...
	return SetAVCompressed();
}

HRESULT CSubmissionBase::SetOEH() throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(this->IsComponentSet())
			return S_OK;

		typedef bool (*PathProvider)(TCHAR*, size_t&) throw();
		PathProvider p[] = {
			&AVModule::CAVPathProvider::GetPath,
			&ccSym::CCCPathProvider::GetPath,
			&ccSym::CNAVPathProvider::GetPath,
		};

		ccLib::CString sPath;
		for(int i = 0; i<CCDIMOF(p); ++i)
		{
			TCHAR buf[MAX_PATH]; size_t nSize = MAX_PATH;
			if((*p[i])(buf, nSize))
			{
				PathAppend(buf, cc::sz_OEHeur_dll);
				if(GetFileAttributes(buf) != INVALID_FILE_ATTRIBUTES)
				{
					sPath = buf;
					break;
				}
			}
		}
		//get OEH version...
		ccLib::CVersionInfo verInfo;
		if(!verInfo.Load(sPath))
			hrx << HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

		hrx << SetComponent(components::g_szOEH, ATL::CT2W(verInfo.GetFileVersion()), eCategoryAntiVirus);
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::SetVXMS(QBackup::IQBackupSet* pSet)
{
	utils::BoolX brx;
	STAHLSOFT_HRX_TRY(hr)
	{
		if(IsComponentSet())
			return S_OK;
		
		//Load File Name
		ISymBasePtr pBase;
		cc::IKeyValueCollectionPtr pProps;
		HRESULT sr = pSet->GetProperties(pProps.m_p);
		if(FAILED(sr) || pProps == NULL)
		{
			CCTRCTXE1(_T("Failed to get props: 0x%08X"), sr);
			hrx << E_FAIL;
		}

		CCTRCTXI0(_T("Retrieving CSF file name"));
		brx << pProps->GetValue(avQBTypes::avCSFProp_StealthFileName, pBase.m_p);

		cc::IStringQIPtr pFileName = pBase;
		if(pFileName == NULL)
		{
			CCTRCTXE0(_T("No File Name"));
			hrx << HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		}

		CCTRCTXI1(_T("CS File = %ls"), pFileName->GetStringW());

		brx << m_pAttributes->SetValue(eFilePath, pFileName);

		//Get the VxMS (MSL) Version
		CCTRCTXI0(_T("Loading MSL Version"));
		ccLib::CString sPath;
		brx << ccSym::CInstalledApps::GetInstAppsDirectory(components::g_szMSLKey, sPath);
		brx << ccLib::CStringConvert::AppendPath(sPath, components::g_szMSLDLL);
		CCTRCTXI1(_T("MSL = %s"), sPath);

		ccLib::CVersionInfo verInfo;
		brx << (verInfo.Load(sPath) != FALSE);
		CCTRCTXI1(_T("Ver = %s"), verInfo.GetFileVersion());

		hrx << SetComponent(components::g_szVXMS, ATL::CW2T(verInfo.GetFileVersion()), eCategoryHueristic);

	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::CheckDefDate() throw()
{
	utils::BoolX brx;
	STAHLSOFT_HRX_TRY(hr)
	{
		SYSTEMTIME defDate = {0};
		DWORD dwRev = 0;
		DWORD dwSeq = 0;
		if(!m_pAttributes->GetExists(eDefsSeq) || !m_pAttributes->GetExists(eDefsDate) || !m_pAttributes->GetExists(eDefsRev))
		{
			DWORD dwDate = 0;
			AVModule::IAVThreatInfoPtr pInfo;
			AVModule::IAVMapDwordDataPtr pData;
			hrx << GetAVThreatInfo(pInfo.m_p);
			hrx << pInfo->GetVIDSubmissionInfo(0, pData.m_p);

			hrx << pData->GetValue(AVModule::ThreatProcessor::GetVIDSubmitOut_DefsDate, dwDate);
			hrx << pData->GetValue(AVModule::ThreatProcessor::GetVIDSubmitOut_DefsRevision, dwRev);
			hrx << pData->GetValue(AVModule::ThreatProcessor::GetVIDSubmitOut_DefsSequence, dwSeq);

			ATL::CTime tm(static_cast<time_t>(dwDate));

			tm.GetAsSystemTime(defDate);

			brx << m_pAttributes->SetValue(eDefsDate, defDate);
			brx << m_pAttributes->SetValue(eDefsSeq, dwSeq);
			brx << m_pAttributes->SetValue(eDefsRev, dwRev);
		}
		else
		{
			brx << m_pAttributes->GetValue(eDefsSeq, dwSeq);
			brx << m_pAttributes->GetValue(eDefsDate, defDate);
			brx << m_pAttributes->GetValue(eDefsRev, dwRev);
		}
		
		ATL::CTime tm = ATL::CTime::GetCurrentTime();
		ATL::CTime tDefs(defDate);
		ATL::CTimeSpan ts = tm - tDefs;
		if(ts.GetDays() > 14)
		{
			CCTRCTXE1(_T("ET to great for defs: %d days"), ts.GetDays());
			hrx << E_FAIL;
		}
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::GetCOHVer(ccLib::CStringW& sVer) const
{
	utils::BoolX brx(HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND));
	STAHLSOFT_HRX_TRY(hr)
	{
#if 0
/**** No longer using ccEraser::IAnomaly::HueristicEngineVersion

		if(m_data.pAnomaly == NULL)
		{
			CCTRCTXE0(_T("m_data.pAnomaly == NULL"));
			hrx << E_POINTER;
		}

		cc::IKeyValueCollectionPtr pProps;
		ccEraser::eResult eRes = m_data.pAnomaly->GetProperties(pProps.m_p);
		if(ccEraser::Failed(eRes) || pProps == NULL)
		{
			CCTRCTXE1(_T("GetProperties() == %d || pProps == NULL"), eRes);
			hrx << E_FAIL;
		}

		ISymBasePtr pBase;
		if(!pProps->GetValue(ccEraser::IAnomaly::HeuristicEngineVersion, pBase.m_p))
		{
			CCTRCTXE0(_T("Failed to get H.E. Ver"));
			hrx << HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
		}

		cc::IStringQIPtr pString = pBase;
		if(pString == NULL)
		{
			CCTRCTXE0(_T("pString == NULL"));
			hrx << E_POINTER;
		}

		sVer = pString->GetStringW();
******/
#endif
		//Get the COH Ver from AHS.dll
		CCTRCTXI0(_T("Loading COH (AHS.dll) Version"));
		ccLib::CString sPath;
		if(!ccSym::CInstalledApps::GetInstAppsDirectory(components::g_szCOHKey, sPath))
		{
			CCTRCTXE1(_T("Failed to get instapps: %s"), components::g_szCOHKey);
			hrx << HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
		}
		if(!ccLib::CStringConvert::AppendPath(sPath, components::g_szCOHDLL))
		{
			CCTRCTXE0(_T("Failed to append path"));
			hrx << E_OUTOFMEMORY;
		}

		CCTRCTXI1(_T("COH = %s"), sPath);

		ccLib::CVersionInfo verInfo;
		if(verInfo.Load(sPath) == FALSE)
		{
			CCTRCTXE1(_T("Failed to load verinfo from %s"), sPath);
			hrx << HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		}
		CCTRCTXI1(_T("COH Ver = %s"), verInfo.GetFileVersion());
		sVer = verInfo.GetFileVersion();
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::GetCOLVer(ccLib::CStringW&) const
{
	return E_NOTIMPL;
}

HRESULT CSubmissionBase::CheckBloodhound(DWORD dwVID) const throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		AVModule::IAVThreatInfoPtr pInfo;
		AVModule::IAVMapDwordDataPtr pData;
		hrx << GetAVThreatInfo(pInfo.m_p);
		hrx << pInfo->GetVIDSubmissionInfo(dwVID, pData.m_p);
		hrx << (pData ==  NULL ? E_POINTER : S_OK);

		DWORD dwSubmit=0, dwBloodhound=0;


		//add virus name if it doesnt exist
		if(!m_pAttributes->GetExists(IAVSubmission::eVirusName))
		{
			cc::IStringPtr pName;
			HRESULT hr2 = pData->GetValue(AVModule::ThreatProcessor::GetVIDSubmitOut_ThreatName, pName.m_p);
			if(SUCCEEDED(hr2) && pName != NULL)
			{
				cc::IStringPtr pString;
				pString.Attach(ccSym::CStringImpl::CreateStringImpl(pName->GetStringW()));
				CCTHROW_BAD_ALLOC(pString);
				m_pAttributes->SetValue(IAVSubmission::eVirusName, pString);
			}
			else
				CCTRCTXE1(_T("Failed to get vname: 0x%08X"), hr2);
		}

		CCTRCTXI1(_T("Checking bloodhound for %u"), dwVID);

		hrx << pData->GetValue(AVModule::ThreatProcessor::GetVIDSubmitOut_IsBloodhound, dwBloodhound);
		
		if(dwBloodhound == 0)
		{
			CCTRCTXW1(_T("vid %d not bloodhound"), dwVID);
			hrx << HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
		}

		CCTRCTXI1(_T("Checking submittable for %u"), dwVID);

		hrx << pData->GetValue(AVModule::ThreatProcessor::GetVIDSubmitOut_IsSubmittable, dwSubmit);

		if(dwSubmit == 0)
		{
			CCTRCTXW1(_T("vid %d not submittable"), dwVID);
			hrx << HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
		}
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::GetAVVer(ccLib::CStringW& sVer) const
{
	STAHLSOFT_HRX_TRY(hr)
	{
		cc::IStringPtr pDefsPath;
		AVModule::IAVDefInfoPtr pDefs;
		hrx << GetDefManager(pDefs.m_p);
		hrx << pDefs->GetDefsDir(pDefsPath.m_p);
		hrx << (pDefsPath == NULL ? E_POINTER : S_OK);

		//Get avengine version
		ccLib::CStringW sPath = pDefsPath->GetStringW();
		PathAppendW(sPath.GetBuffer(MAX_PATH), L"NAVENG.SYS");
		sPath.ReleaseBuffer();
		ccLib::CVersionInfo verInfo;
		if(!verInfo.Load(ATL::CW2T(sPath)))
		{
			CCTRCTXE2(_T("Failed to get vernfo for %ls: %d"), sPath, GetLastError());
			hrx << E_FAIL;
		}
		sVer = verInfo.GetFileVersion();
	}
	CATCH_RETURN(hr)
}

bool CSubmissionBase::IsComponentSet() const
{
	return 
		m_pAttributes->GetExists(clfs::ISubmission::eComponentName) &&
		m_pAttributes->GetExists(clfs::ISubmission::eComponentVersion);
}

HRESULT CSubmissionBase::SetComponent(const wchar_t* pszComp, const wchar_t* pszVer, Category cat, const wchar_t* pszSubComp)
{
	STAHLSOFT_HRX_TRY(hr)
	{
		if(pszComp == NULL || pszVer == NULL)
		{
			CCTRCTXE0(_T("no component or version"));
			return E_INVALIDARG;
		}

		CCTRCTXI3(_T("%ls %ls (category %d)"), pszComp, pszVer, cat);

		cc::IStringPtr pVer, pComp, pSubComp;
		pComp.Attach(ccSym::CStringImpl::CreateStringImpl(pszComp));
		pVer.Attach(ccSym::CStringImpl::CreateStringImpl(pszVer));
		CCTHROW_BAD_ALLOC(pVer);
		CCTHROW_BAD_ALLOC(pComp);
		if(!m_pAttributes->SetValue(clfs::ISubmission::eComponentName, pComp))
			hrx << E_FAIL;
		if(!m_pAttributes->SetValue(clfs::ISubmission::eComponentVersion, pVer))
			hrx << E_FAIL;
		if(!m_pAttributes->SetValue(IAVSubmission::eSampleCategory, DWORD(cat)))
			hrx << E_FAIL;

		//optional
		if(pszSubComp != NULL && *pszSubComp != NULL)
		{
			CCTRCTXI1(_T("SubComponent = %ls"), pszSubComp);
			pSubComp.Attach(ccSym::CStringImpl::CreateStringImpl(pszSubComp));
			CCTHROW_BAD_ALLOC(pSubComp);
			if(!m_pAttributes->SetValue(IAVSubmission::eSubEngine, pSubComp))
				hrx << E_FAIL;
		}
	}
	CATCH_RETURN(hr);
}

inline bool CSubmissionBase::IsManual() const
{
	bool bManual = false;
	(void) m_pAttributes->GetValue(eManual, bManual);
	return bManual;
}

HRESULT CSubmissionBase::LoadDate(QBackup::IQBackupSet* pSet)
{
	if(pSet == NULL)
		return E_INVALIDARG;
	cc::IKeyValueCollectionPtr pProps;
	pSet->GetProperties(pProps);
	if(pProps == NULL)
		return E_POINTER;
	SYSTEMTIME st = {0};
	pProps->GetValue(QBackup::IQBackupItemBase::DateAdded, st);
	m_pAttributes->SetValue(eDateQuarantined, st);
	return S_OK;
}

HRESULT CSubmissionBase::BackupSample(GUID& guid)
{
	utils::BoolX brx;
	STAHLSOFT_HRX_TRY(hr)
	{
		ISymBasePtr pBase;
		cc::IStringQIPtr pString;

		brx << m_pAttributes->GetExists(eFilePath);
		brx << m_pAttributes->GetValue(eFilePath, pBase.m_p);

		pString = pBase;
		if(pString == NULL || pString->GetLength() < 1)
			hrx << HRESULT_FROM_WIN32(ERROR_NOT_FOUND);

		(void) m_pAttributes->Remove(eFilePath);

		ccEraser::IEraser4Ptr pEraser;
		ccEraser::IRemediationActionPtr pAction;
		ccEraser::eResult eRes = ccEraser::Success;
		QBackup::IQBackupPtr pBackup;
		QBackup::IQBackupSetPtr pSet;
		cc::IKeyValueCollectionPtr pProps;


		hrx << GetEraser(pEraser.m_p);
		hrx << GetBackup(pBackup.m_p);

		eRes = pEraser->CreateObject(ccEraser::FileRemediationActionType, ccEraser::IID_RemediationAction, (void**)&pAction.m_p);
		if(ccEraser::Failed(eRes))
		{
			CCTRCTXE1(_T("Failed to create remediation: %d"), eRes);
			brx << false;
		}
		eRes = pAction->GetProperties(pProps.m_p);
		if(ccEraser::Failed(eRes) || pProps == NULL)
		{
			CCTRCTXE1(_T("Failed to get props: %d"), eRes);
			brx << false;
		}

		CCTRCTXI1(_T("Backing Up %ls"), pString->GetStringW());

		//wierd: SetCloneInterface causes funny behaviour within ccEraser.
		//we'll clone this string here to workaround the first char of the path being reset to NULL

		//brx << pProps->SetCloneInterface(true);

		cc::IStringPtr pFilePath;
		pFilePath.Attach(ccSym::CStringImpl::CreateStringImpl(pString->GetStringW()));
		CCTHROW_BAD_ALLOC(pFilePath);

		brx << pProps->SetValue(ccEraser::IRemediationAction::Path, pFilePath);
		brx << pProps->SetValue(ccEraser::IRemediationAction::Operation, DWORD(ccEraser::IRemediationAction::Delete));
		brx << pProps->SetValue(ccEraser::IRemediationAction::State, DWORD(ccEraser::IRemediationAction::NotRemediated));

		bool bExists = false;
		eRes = pAction->IsPresent(bExists);
		if(ccEraser::Failed(eRes) || !bExists)
		{
			CCTRCTXE3(_T("Sample %ls not present: %d exists %s"), pString->GetStringW(), eRes, bExists ? _T("Yes") : _T("No"));
			hrx << HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		}

		hrx << pBackup->CreateBackupSet(pSet.m_p);
		hrx << pSet->SetGivenType(GT_SubmitOEHSet);
		hrx << BackupRemediation(pSet, pAction);
		hrx << pBackup->StoreBackupSet(pSet, &guid, false);
		CCTRACEI(CCTRCTX _T("Created new backup set: ") SYMGUID_FORMAT_STR((&guid)));
		hrx << pSet->Flush();
		CCTRCTXI0(_T("Flushed backup set"));
		brx << m_pAttributes->SetValue(eSetId, guid);
	}
	CATCH_RETURN(hr)
}

HRESULT CSubmissionBase::BackupRemediation(QBackup::IQBackupSet* pSet, ccEraser::IRemediationAction* pRemediation) throw()
{
	TRACE_SCOPE(s0);

	HRESULT hr = S_OK;
	QBackup::IQBackupItemBasePtr pItem;
	QBackup::IQBackupItemBasePtr pItemUndo;
	QBackup::IQBackupItemStreamQIPtr pBackupStream;
	ccSym::CMemoryStreamImplPtr pMemory, pUndo;
	cc::ISerializeQIPtr pSerialize;
	cc::IKeyValueCollectionPtr pProps;
	GUID item = GUID_NULL;
	GUID itemUndo = GUID_NULL;

	for(;;)
	{
		if(pSet == NULL)
		{
			hr = E_INVALIDARG;
			CCTRCTXE0(_T("pSet == NULL"));
			break;
		}
		if(pRemediation == NULL)
		{
			hr = E_INVALIDARG;
			CCTRCTXE0(_T("pRemediation == NULL"));
			break;
		}

		pMemory.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		if(pMemory == NULL)
		{
			CCTRCTXE0(_T("pMemory == NULL"));
			hr = E_OUTOFMEMORY;
			break;
		}

		pUndo.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		if(pUndo == NULL)
		{
			CCTRCTXE0(_T("pUndo == NULL"));
			hr = E_OUTOFMEMORY;
			break;
		}
		
		pSerialize = pRemediation;
		if(pSerialize == NULL)
		{
			CCTRCTXE0(_T("pSerialize == NULL"));
			hr = E_NOINTERFACE;
			break;
		}
		
		if(pSerialize->Save(pMemory) == false)
		{
			CCTRCTXE0(_T("pSerialize->Save(pMemory) == false"));
			hr = E_FAIL;
			break;
		}

		if(pMemory->SetPosition(0) == false)
		{
			CCTRCTXE0(_T("pMemory->SetPosition(0) == false"));
			hr = E_FAIL;
			break;
		}
	
		hr = pSet->CreateBackupItem(QBackup::itemStream, pItem.m_p);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to create backup item: 0x%08X"), hr);
			break;
		}
		pBackupStream = pItem;
		if(pBackupStream == NULL)
		{
			CCTRCTXE0(_T("pBackupStream == NULL"));
			hr = E_NOINTERFACE;
			break;
		}
		
		hr = pItem->SetGivenType(avQBTypes::GT_AVREMEDIATIONACTION);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to set GT: 0x%08X"), hr);
			break;
		}
		hr = pBackupStream->Backup(pMemory);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to backup action: 0x%08X"), hr);
			break;
		}
		hr = pSet->StoreBackupItem(pItem, &item);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to store backup item: 0x%08x"), hr);
			break;
		}

		pBackupStream.Release();

		bool bUndo = false;
		ccEraser::eResult eRes = pRemediation->SupportsUndo(bUndo);
		if(ccEraser::Failed(eRes))
		{
			CCTRCTXE1(_T("SupportsUndo failed: %d"), eRes);
			hr = E_FAIL;
			break;
		}
		if(bUndo == false)
		{
			CCTRCTXW0(_T("Success, No Undo Information"));
			hr = S_OK;
			break;
		}

		CCTRCTXI0(_T("Saving Undo Stream"));

		eRes = pRemediation->GetUndoInformation(pUndo.m_p);
		if(ccEraser::Failed(eRes))
		{
			CCTRCTXE1(_T("Failed to get undo stream: %d"), eRes);
			hr = E_FAIL;
			break;
		}

		if(pUndo->SetPosition(0) == false)
		{
			CCTRCTXE0(_T("pUndo->SetPosition(0) == false"));
			hr = E_FAIL;
			break;
		}

		hr = pSet->CreateBackupItem(QBackup::itemStream, pItemUndo.m_p);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to create backup item: 0x%08X"), hr);
			break;
		}

		pBackupStream = pItemUndo;
		if(pBackupStream == NULL)
		{
			CCTRCTXE0(_T("pBackupStream == NULL"));
			hr = E_NOINTERFACE;
			break;
		}
		
		hr = pItemUndo->SetGivenType(avQBTypes::GT_AVREMEDIATIONACTIONUNDO);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Set GT failed: 0x%08X"), hr);
			break;
		}
		hr = pBackupStream->Backup(pUndo);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to save backup stream: 0x%08X"), hr);
			break;
		}

		hr = pSet->StoreBackupItem(pItemUndo, &itemUndo);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to store item: 0x%08X"), hr);
			break;
		}

		hr = pItem->GetProperties(pProps.m_p);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to get undo item props: 0x%08X"), hr);
			break;
		}

		if(pProps->SetValue(avQBTypes::avRAProp_MatchingUndoID, itemUndo) == false)
		{
			CCTRCTXE0(_T("Failed to set matching undo id"));
			hr = E_FAIL;
			break;
		}

		pProps.Release();

		hr = pItemUndo->GetProperties(pProps.m_p);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to get undo item props: 0x%08X"), hr);
			break;
		}

		if(pProps->SetValue(avQBTypes::avRAUProp_MatchingRemActID, item) == false)
		{
			CCTRCTXE0(_T("Failed to set matching undo id"));
			hr = E_FAIL;
			break;
		}

		CCTRCTXI0(_T("Success"));
		hr = S_OK;
		break;
	}

	if(SUCCEEDED(hr))
	{
		CCTRCTXI0(_T("Flushing backup set"));
		hr = pSet->Flush();
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to flush set: 0x%08X"), hr);
		}
	}
	else
		CCTRCTXE1(_T("Failed 0x%08X - not flushing set"), hr);

	return hr;
}

HRESULT CSubmissionBase::SetDetails() throw()
{
	utils::BoolX brx(HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
	STAHLSOFT_HRX_TRY(hr)
	{
		cc::IStringQIPtr pString, pDescr;
		ISymBasePtr pBase;
		ccLib::CStringW sDescr, sFmt;
		sFmt.LoadString(IDS_DESCRIPTION_FMT);

		//prepare the description.
		brx << m_pAttributes->GetValue(eVirusName, pBase.m_p);
		pString = pBase;
		hrx << (pString == NULL ? E_NOINTERFACE : S_OK);
		pBase.Release();
		brx << m_pAttributes->GetValue(ISubmission::eDescription, pBase.m_p);
		pDescr =  pBase;
		hrx << (pDescr == NULL ? E_NOINTERFACE : S_OK);
		sDescr.Format(sFmt, pDescr->GetStringW(), pString->GetStringW());
		brx << pDescr->SetStringW(sDescr);
		brx << m_pAttributes->SetValue(ISubmission::eDescription, pDescr);

		//prepare the details
		sFmt.LoadString(IDS_DETAILS_SEPERATOR);
		sDescr = L"";
		for(Remediation::List::iterator i = m_data.list.begin(), e = m_data.list.end(); i!=e; ++i)
		{
			pString.Release();
			pBase.Release();
			if(i->pRemediation == NULL)
				continue;

			ccEraser::eObjectType type;
			if(ccEraser::Failed(i->pRemediation->GetType(type)))
				continue;

			cc::IKeyValueCollectionPtr pProps;
			if(ccEraser::Failed(i->pRemediation->GetProperties(pProps.m_p)) || pProps == NULL)
				continue;

			size_t key = 0;
			switch(type)
			{
			case ccEraser::FileRemediationActionType:
			case ccEraser::InfectionRemediationActionType:
				key = ccEraser::IRemediationAction::Path;
				break;
			case ccEraser::RegistryRemediationActionType:
				key = ccEraser::IRemediationAction::KeyName;
				break;
			default:
				break;
			}
			if(key == 0)
				continue;
			if(!pProps->GetValue(key, pBase.m_p) || pBase == NULL)
				continue;
			pString = pBase;
			if(pString == NULL)
				continue;
			sDescr += pString->GetStringW();
			if(i != (e-1))
				sDescr += sFmt;
		}
		if(sDescr.GetLength() > 1)
		{
			cc::IStringPtr p;
			p.Attach(ccSym::CStringImpl::CreateStringImpl(sDescr));
			CCTHROW_BAD_ALLOC(p);
			m_pAttributes->SetValue(clfs::ISubmission::eDetails, p);
		}
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::SetSubmittedKey(bool bSubmitted)
{
	HRESULT hr = E_FAIL;

	for(;;)
	{
		QBackup::IQBackupPtr pBackup;
		hr = GetBackup(pBackup.m_p);
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to get backup: 0x%08X"), hr);
			break;
		}

		GUID guid = GUID_NULL;
		if(!m_pAttributes->GetValue(eSetId, guid))
		{
			hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
			CCTRCTXW1(_T("set id not found: 0x%08X"), hr);
			break;
		}

		QBackup::IQBackupSetPtr pSet;
		hr = pBackup->RetrieveBackupSetByID(guid, pSet.m_p);
		if(FAILED(hr) || pSet == NULL)
		{
			CCTRCTXE1(_T("backup set not found"), hr);
			hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
			break;
		}

		cc::IKeyValueCollectionPtr pUserData;
		hr = pSet->GetProperties(pUserData.m_p);

		if(pUserData == NULL || FAILED(hr))
		{
			CCTRCTXE1(_T("pUserData == NULL: 0x%08X"), hr);;
			hr = E_FAIL;
			break;
		}
		if(!pUserData->SetValue(avQBTypes::avThreatProp_Submitted, bSubmitted))
		{
			CCTRCTXE0(_T("Failed to set submitted key"));
			break;
		}

		hr = pSet->Flush();
		if(FAILED(hr))
		{
			CCTRCTXE1(_T("Failed to flush backup set after setting submitted key"), hr);
			break;
		}
		
		CCTRCTXI0(_T("Set QBackup submitted key, flushed set"));
		hr = S_OK;
		break;			
	}
	return hr;
}

HRESULT CSubmissionBase::GetThreatCats() throw()
{
	HRESULT hr = S_OK;
	for(;;)
	{
		if(m_bInitializing == false)
		{
			CCTRCTXI0(_T("not initializing"));
			hr = S_OK;
			break;
		}
		if(m_data.pAnomaly == NULL)
		{
			CCTRCTXE0(_T("pAnomaly == NULL"));
			hr = S_OK; //non-failure;
			break;
		}
		cc::IKeyValueCollectionPtr pProps;
		ccEraser::eResult eRes = m_data.pAnomaly->GetProperties(pProps.m_p);
		if(ccEraser::Failed(eRes))
		{
			CCTRCTXE1(_T("Failed to get props: %d"), eRes);
			hr = E_FAIL;
			break;
		}
		if(pProps == NULL)
		{
			CCTRCTXE0(_T("pProps == NULL"));
			hr = E_POINTER;
			break;
		}
		
		cc::IIndexValueCollectionQIPtr pCats;
		ISymBasePtr pBase;
		if(pProps->GetValue(ccEraser::IAnomaly::Categories, pBase.m_p) == false)
		{
			CCTRCTXE0(_T("No categories"));
			hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
			break;
		}
		pCats = pBase;
		if(pCats == NULL)
		{
			CCTRCTXE0(_T("pCats == NULL"));
			hr = E_POINTER;
			break;
		}
		ISymBasePtr pLocalCats;
		cc::ICloneQIPtr pClone = pCats;
		if(pClone == NULL)
		{
			CCTRCTXE0(_T("pClone == NULL"));
			hr = E_NOINTERFACE;
			break;
		}
		if(pClone->Clone(pLocalCats.m_p) == false)
		{
			CCTRCTXE0(_T("Clone fails"));
			hr = E_FAIL;
			break;
		}
		if(m_pAttributes->SetValue(eThreatCats, pLocalCats) == false)
		{
			CCTRCTXE0(_T("SetValue fails"));
			hr = E_FAIL;
			break;
		}

		CCTRCTXI0(_T("Success"));

		hr = S_OK;
		break;
	}
	CCTRCTXI1(_T("Returning 0x%08X"), hr);
	return hr;
}

void CSubmissionBase::DumpRemediation(size_t nIndex, const ccEraser::IRemediationAction* pRem) const throw()
{
	if(pRem == NULL)
	{
		CCTRCTXE0(_T("pRem == NULL"));
		return;
	}
	cc::IStringPtr pDesc;
	ccEraser::eResult eRes = pRem->GetDescription(pDesc.m_p);
	if(ccEraser::Failed(eRes))
	{
		CCTRCTXE1(_T("No Description: %d"), eRes);
		return;
	}
	if(pDesc == NULL)
	{
		CCTRCTXE0(_T("pDesc == NULL"));
		return;
	}
	size_t nPropSize = 0;
	cc::IKeyValueCollectionPtr pProps;
	eRes = pRem->GetProperties(pProps.m_p);
	if(ccEraser::Succeeded(eRes))
		nPropSize = pProps->GetCount();
	else
		CCTRCTXE1(_T("Failed to get props: %d"), eRes);

	CCTRCTXI3(_T("Remediation %Iu: %ls (%Iu properties)"), nIndex, pDesc->GetStringW(), nPropSize);
}

void CSubmissionBase::DumpAnomaly(const ccEraser::IAnomaly* pAnomaly) const throw()
{
	ISymBasePtr pBase;
	cc::IStringQIPtr pString;
	cc::IKeyValueCollectionPtr pProps;

	if(!g_DebugOutput.IsOutputEnabled(ccLib::CDebugOutput::eInformationDebug))
		return;

	if(pAnomaly == NULL)
	{
		CCTRCTXE0(_T("pAnomaly == NULL"));
		return;
	}
	ccEraser::eResult eRes = pAnomaly->GetProperties(pProps.m_p);
	if(ccEraser::Failed(eRes))
	{
		CCTRCTXE1(_T("Failed to get props: %d"), eRes);
		return;
	}
	pProps->GetValue(ccEraser::IAnomaly::Name, pBase.m_p);
	pString = pBase;
	if(pString)
		CCTRCTXI1(_T("\tName == %ls"), pString->GetStringW());
	else
		CCTRCTXI0(_T("\tName == NULL"));

	DWORD dw = DWORD(-1);
	pProps->GetValue(ccEraser::IAnomaly::VID, dw);
	CCTRCTXI1(_T("\tVID == %d"), dw);
}

}
