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
#include "Resource.h"
#include "SubmissionBase.h"
#include "ComponentNames.h"
#include "ccSymKeyValueCollectionImpl.h"
#include "ccModuleNames.h"
#include "ccSingleLock.h"
#include "ccStreamInterface.h"
//#include "ccSymMemoryStreamImpl.H"
#include "ccSymStringImpl.h"
#include "ccSymInstalledApps.h"
#include "ccVersionInfo.h"
#include "ccSymDebugOutput.h"
#include "ccEraserInterface.h"
#include "ccScanwInterface.h"
#include "ccScanwLoader.h"
#include "ccTrace.h"
#include "ccLib.h"
#include "Utils.h"

#include "atltime.h"
#include "hrx.h"
#include "srx.h"

using namespace clfs;

namespace SAVSubmission {

inline bool ScanwSuccess(ccScanw::SCANWSTATUS status)
{
	return status == ccScanw::SCANW_OK || status == ccScanw::SCANW_OK_OLD_DEFINITIONS;
}

CSubmissionBase::CSubmissionBase(ccScanw::IScannerw* pScanner) :
				m_ptrScanner(pScanner)
{
}

CSubmissionBase::~CSubmissionBase(void)
{
	m_pAttributes.Release();
}

HRESULT CSubmissionBase::Initialize(const cc::IKeyValueCollection* pAttrib) throw()
{
	return S_OK;	//TODO:JJM: remove this function?
}

HRESULT CSubmissionBase::SetDescription(cc::IKeyValueCollectionPtr& pAttrib, UINT uiStringResId) const
{
	HRESULT hResult = S_OK;
	ccLib::CString sDescription;
	cc::IStringPtr pDesc;

	if (sDescription.LoadString(uiStringResId))
	{
		pDesc.Attach(ccSym::CStringImpl::CreateStringImpl(sDescription));
		if (pDesc == NULL || !pAttrib->SetValue(clfs::ISubmission::eDescription, pDesc))
			hResult = E_OUTOFMEMORY;
	}
	else
	{
		hResult = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_RESOURCE_DATA_NOT_FOUND);
	}
	return hResult;
}

HRESULT CSubmissionBase::GetPVA(cc::IStringPtr& pProduct, cc::IStringPtr& pVersion, cc::IStringPtr& pActor) const throw()
{
	pProduct.Attach(ccSym::CStringImpl::CreateStringImpl());
	pVersion.Attach(ccSym::CStringImpl::CreateStringImpl());
	pActor.Attach(ccSym::CStringImpl::CreateStringImpl());
	if (pProduct != NULL && pVersion != NULL && pActor != NULL)
	{
		//TODO:JJM: fix these to not be hard coded
		if (pProduct->SetString(_T("Symantec Antivirus")) &&
			pVersion->SetString(_T("11.0")) &&
			pActor->SetString(_T("Actor??!!")))
		{
			return S_OK;
		}
		else
		{
			return E_OUTOFMEMORY;
		}
	 }
	else
	{
		return E_OUTOFMEMORY;
	}
}

// IAVSubmissionInternal
HRESULT CSubmissionBase::SetSubmitted(bool bSubmitted) throw()
{
	if(bSubmitted == false)
	{
		CCTRCTXE0(_T("Cannot set non-submitted"));
		return E_NOTIMPL;
	}
	return SetSubmittedKey();
}

//ISubmission
HRESULT CSubmissionBase::GetAttributes(cc::IKeyValueCollection*& pAttrib) const throw()
{
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	utils::FreeObject(pAttrib);
	pAttrib = m_pAttributes;
	if(pAttrib != NULL)
		pAttrib->AddRef();
	return pAttrib == NULL ? E_POINTER : S_OK;
}

HRESULT CSubmissionBase::GetSubmissionData(const cc::IKeyValueCollection* pCriteria, cc::IStream*& pData) throw()
{
	//ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	//nothing to do for now...
	//we will implement this once these submissions no longer need translation (new version of DIS, etc...)
	return E_NOTIMPL;
}

HRESULT CSubmissionBase::ProcessResponse(const cc::IKeyValueCollection* pResponse) throw()
{
	//ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	return S_OK; //nothing to do for now...
}

bool CSubmissionBase::Save(ccLib::CArchive& Archive) const
{
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	bool bRes = false;
	HRESULT hr = S_OK;
	try
	{
		Archive << kdwVersion;
		if (ccSym::CPersist::Write(m_pAttributes, NULL, NULL, &Archive, NULL, NULL))
		{
			bRes = true;
		}
	}
	catch(ccLib::CArchive::CArchiveException&)
	{
		CCTRCTXE0(_T("Caught CArchiveException"));
	}
	CATCH_NO_RETURN(hr);

	return bRes;
}

bool CSubmissionBase::Load(ccLib::CArchive& Archive)
{
	ccLib::CSingleLock lk(GetLockObject(), INFINITE, false);
	bool bRes = false;
	HRESULT hr = S_OK;
	try
	{
		DWORD dwVer = 0;

		m_pAttributes.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
		CCTHROW_BAD_ALLOC(m_pAttributes);

		Archive >> dwVer;
		if (dwVer == kdwVersion)
		{
			if (ccSym::CPersist::Read(m_pAttributes, NULL, NULL, &Archive, NULL, NULL))
			{
				if(g_DebugOutput.IsOutputEnabled(ccLib::CDebugOutput::eInformationDebug))
				{
					DWORD dwVid = DWORD(-1);
					cc::IStringQIPtr pString;
					ISymBasePtr pBase;
					(void) m_pAttributes->GetValue(AVSubmit::IAVSubmission::eVID, dwVid);
					(void) m_pAttributes->GetValue(AVSubmit::IAVSubmission::eVirusName, pBase.m_p);
					pString = pBase;
					if (pString == NULL)
					{
						CCTRCTXI1(_T("VID %d: Name == NULL"), dwVid);
					}
					else
						CCTRCTXI2(_T("Vid %d [%ls]"), dwVid, pString->GetStringW());
				}
				bRes = true;
			}
			else
			{
				CCTRCTXE0(_T("Failed to read attributes"));
			}
		}
		else
		{
			CCTRCTXE1(_T("Cannot read version: 0x%08X"), dwVer);
		}
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

/*
HRESULT CSubmissionBase::GetEraser(ccEraser::IEraser4*& pe) const
{
	//TODO:JJM: must reimplement this
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
*/

HRESULT CSubmissionBase::GetScanner(ccScanw::IScannerw*& pScanner) const
{
	STAHLSOFT_HRX_TRY(hr)
	{
		utils::FreeObject(pScanner);
		if (m_ptrScanner != NULL)
		{
			pScanner = m_ptrScanner;
			pScanner->AddRef();
		}
		else
			hrx << E_NOINTERFACE;
	}
	CATCH_RETURN(hr);
}

/*
HRESULT CSubmissionBase::GetBackup(QBackup::IQBackup*& pBackup) const
{
	STAHLSOFT_HRX_TRY(hr)
	{
		utils::FreeObject(pBackup);
		if(m_pBackup == NULL)
		{
			QBackup::IQBackupPtr p;
			SYMRESULT sr = QBackup_IQbackupMgd::CreateObject(GETMODULEMGR(), p.m_p);
			if(SYM_FAILED(sr) || p == NULL)
			{
				CCTRCTXE1(_T("Faield to load qbackup: 0x%08X"), sr);
				hrx << HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
			}
			sr = p->Initialize();
			if(SYM_FAILED(sr))
			{
				CCTRCTXE1(_T("Failed to init qbackup: 0x%08X"), sr);
				hrx << E_FAIL;
			}
			m_pBackup = p;
		}
		pBackup = m_pBackup;
		pBackup->AddRef();
	}
	CATCH_RETURN(hr);
}
*/

/*
HRESULT CSubmissionBase::RestoreStream(QBackup::IQBackupSet* pSet, REFGUID id, REFGUID type, cc::IStream*& pOutStream, size_t nIndex)
{
	SRX srx;
	STAHLSOFT_HRX_TRY(hr)
	{
		utils::FreeObject(pOutStream);
		QBackup::IQBackupItemBasePtr pBase;
		ccEraser::IEraser4Ptr pEraser;
		
		hrx << GetEraser(pEraser.m_p);

		if(nIndex != -1)
		{
			GUID guid;
			srx << pSet->GetItem(nIndex, pBase.m_p, &guid);
		}
		else
		{
			srx << pSet->GetItemByID(id, pBase.m_p);
		}

		if(type != GUID_NULL)
		{
			//CCTRACEI(CCTRCTX _T(" Requested Type: ") SYMGUID_FORMAT_STR((&type)));
			GUID given = {0};
			srx << pBase->GetGivenType(given);
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

		srx << pStreamObj->Restore(pStream.m_p);
		pOutStream = pStream.Detach();
	}
	catch(srx_exception& s)
	{
		CCTRCTXE1(_T("Caught SRX Exception: 0x%08X"), s.Error());
		hr = E_FAIL;
	}
	CATCH_RETURN(hr);
}
*/

/*
TODO:JJM: remove this dead code
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
		SYMRESULT sr = pSet->GetItem(i, pItem.m_p, &itemId);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to get qbackup item: 0x%08X"), sr);
			continue;
		}
		cc::IStreamPtr pStream;
		if(FAILED(RestoreStream(pSet, itemId, avQBTypes::GT_AVREMEDIATIONACTION, pStream.m_p)))
			continue;

		cc::IKeyValueCollectionPtr pProps;
		sr = pItem->GetProperties(pProps.m_p);
		if(SYM_FAILED(sr) || pProps == NULL)
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
*/

HRESULT CSubmissionBase::SetAV() throw()
{
	// sort out component and version the submission came from...
	STAHLSOFT_HRX_TRY(hr)
	{
		if (this->IsComponentSet())
			return S_OK;

	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::SetAVCompressed() throw()
{
	//TODO:JJM: what's this really for? should we submit just the sample that is in an archive? or the whole archive?
	return SetAV();
}

HRESULT CSubmissionBase::GetCOHVer(ccLib::CStringW& sVer) const
{
	STAHLSOFT_HRX_TRY(hr)
	{
		//Get the COH Ver from AHS.dll
		CCTRCTXI0(_T("Loading COH (AHS.dll) Version"));
		ccLib::CString sPath;
		if(!ccSym::CInstalledApps::GetInstAppsDirectory(SubSDK::Components::g_szCOHKey, sPath))
		{
			CCTRCTXE1(_T("Failed to get instapps: %s"), SubSDK::Components::g_szCOHKey);
			hrx << HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
		}
		if(!ccLib::CStringConvert::AppendPath(sPath, SubSDK::Components::g_szCOHDLL))
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

HRESULT CSubmissionBase::GetVirusName(DWORD dwVid, ccLib::CStringW& sVirusName) const throw()
{
	sVirusName.Empty();

	ccScanw::IScannerwPtr pScanner;
	ccScanw::IScanwVirusInfoConstPtr pVirus;
	ccScanw::SCANWSTATUS stat;

	STAHLSOFT_HRX_TRY(hr)
	{
		hrx << GetScanner(pScanner.m_p);
		stat = pScanner->GetVirusInfo(dwVid, pVirus.m_p);
		if (!ScanwSuccess(stat))
		{
			CCTRCTXE1(_T("failed to get virus info: %d"), stat);
			hrx << E_FAIL;
		}

		wchar_t buf[0xff]; 
		DWORD dwLen = CCDIMOF(buf)-1;
		SecureZeroMemory(buf, sizeof(buf));

		stat = pVirus->GetName(buf, dwLen);
		if(ScanwSuccess(stat) && buf[0] != 0)
		{
			sVirusName = buf;
		}
		else
			CCTRCTXE2(_T("Failed to get virus name: %d len %d"), stat, dwLen);
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::SetVirusNameAttribute(DWORD dwVid) const throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		ccLib::CStringW strTemp;

		hrx << GetVirusName(dwVid, strTemp);

		cc::IStringPtr pString;
		pString.Attach(ccSym::CStringImpl::CreateStringImpl(strTemp));
		CCTHROW_BAD_ALLOC(pString);
		m_pAttributes->SetValue(AVSubmit::IAVSubmission::eVirusName, pString);
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::CheckBloodhound(DWORD dwVid) const throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		ccScanw::IScannerwPtr pScanner;
		ccScanw::IScanwVirusInfoConstPtr pVirus;
		ccScanw::SCANWSTATUS stat;
		bool bBloodhound = false;
		bool  bSubmit = false;

		hrx << GetScanner(pScanner.m_p);
		stat = pScanner->GetVirusInfo(dwVid, pVirus.m_p);
		if(!ScanwSuccess(stat))
		{
			CCTRCTXE1(_T("failed to get virus info: %d"), stat);
			hrx << E_FAIL;
		}

		//add virus name if it doesnt exist
		if(!m_pAttributes->GetExists(AVSubmit::IAVSubmission::eVirusName))
		{
			hrx << SetVirusNameAttribute(dwVid);
		}

		stat = pVirus->IsBloodhound(bBloodhound);
		if(!ScanwSuccess(stat))
		{
			CCTRCTXE1(_T("Failed to determine bloodhound status: %d"), stat);
			hrx << E_FAIL;
		}
		if(!bBloodhound)
		{
			CCTRCTXW1(_T("vid %d not bloodhound"), dwVid);
			// Response doesn't want non-bloodhound detections to be submitted
			hrx << HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
		}
		stat = pVirus->IsSubmittable(bSubmit);
		if(!ScanwSuccess(stat))
		{
			CCTRCTXE1(_T("Failed to get submit status: %d"), stat);
			hrx << E_FAIL;
		}
		if(!bSubmit)
		{
			CCTRCTXW1(_T("vid %d not submittable"), dwVid);
			hrx << HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
		}
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::GetDefinitionsInfo(SYSTEMTIME& sDefDate, DWORD& dwRev, DWORD& dwSeq) const throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		::SecureZeroMemory(&sDefDate, sizeof(sDefDate));
		dwRev = 0;
		dwSeq = 0;
		//load from scanner...
		ccScanw::IScannerwPtr pScanner;
		ccScanw::IScanwDefinitionsInfoPtr pInfo;
		
		hrx << GetScanner(pScanner.m_p);
		ccScanw::SCANWSTATUS stat = pScanner->GetDefinitionsInfoW(pInfo.m_p);
		if(!ScanwSuccess(stat))
		{
			CCTRCTXE0(_T("ccScanw::SCANW_OK != pScanner->GetDefinitionsInfoW(pInfo.m_p)"));
			hrx << E_FAIL;
		}
		time_t dt = 0;
		stat = pInfo->GetDate(dt);
		if(!ScanwSuccess(stat))
		{
			CCTRCTXE1(_T("get date failed: %d"), stat);
			hrx << E_FAIL;
		}
		stat = pInfo->GetRevision(dwRev);
		if(!ScanwSuccess(stat))
		{
			CCTRCTXE1(_T("get rev failed: %d"), stat);
			hrx << E_FAIL;
		}
		stat = pInfo->GetSequenceNumber(dwSeq);
		if(!ScanwSuccess(stat))
		{
			CCTRCTXE1(_T("get seq# failed: %d"), stat);
			hrx << E_FAIL;
		}
		ATL::CTime tm(dt);
		tm.GetAsSystemTime(sDefDate);

		/*
		ATL::CTime tm = ATL::CTime::GetCurrentTime();
		ATL::CTime tDefs(sDefDate);
		ATL::CTimeSpan ts = tm - tDefs;
		if(ts.GetDays() > 14)
		{
			CCTRCTXE1(_T("ET to great for defs: %d days"), ts.GetDays());
			hrx << E_FAIL;
		}
		*/
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::SetDefDateAndSeqAttributes() const throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		SYSTEMTIME sDefDate;
		DWORD dwRev;
		DWORD dwSeq;

		hrx << GetDefinitionsInfo(sDefDate, dwRev, dwSeq);

		m_pAttributes->SetValue(AVSubmit::IAVSubmission::eDefsDate, sDefDate);
		m_pAttributes->SetValue(AVSubmit::IAVSubmission::eDefsRev, dwRev);
		m_pAttributes->SetValue(AVSubmit::IAVSubmission::eDefsSeq, dwSeq);
	}
	CATCH_RETURN(hr);
}

HRESULT CSubmissionBase::GetAVVer(ccLib::CStringW& sVer) const
{
	STAHLSOFT_HRX_TRY(hr)
	{
		const wchar_t* pszDefsDir;
		ccScanw::IScannerwPtr pScanner;
		ccScanw::IScanwDefinitionsInfoPtr ptrDefsInfo;

		hrx << GetScanner(pScanner.m_p);

		ccScanw::SCANWSTATUS eScanStatus = pScanner->GetDefinitionsInfoW(ptrDefsInfo.m_p);
		if(!ScanwSuccess(eScanStatus))
		{
			CCTRCTXE0(_T("ccScanw::SCANW_OK != pScanner->GetDefinitionsInfoW(ptrDefsInfo.m_p)"));
			hrx << E_FAIL;
		}

		if (ptrDefsInfo->GetDefinitionsDirectory(pszDefsDir) != ccScanw::SCANW_OK)
			hrx << E_FAIL;
		hrx << (pszDefsDir == NULL ? E_POINTER : S_OK);

		//Get avengine version
		ccLib::CStringW sPath(pszDefsDir);
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

HRESULT CSubmissionBase::SetComponent(const wchar_t* pszComp, const wchar_t* pszVer, AVSubmit::IAVSubmission::Category cat, const wchar_t* pszSubComp)
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
		if(!m_pAttributes->SetValue(AVSubmit::IAVSubmission::eSampleCategory, DWORD(cat)))
			hrx << E_FAIL;

		//optional
		if(pszSubComp != NULL && *pszSubComp != NULL)
		{
			CCTRCTXI1(_T("SubComponent = %ls"), pszSubComp);
			pSubComp.Attach(ccSym::CStringImpl::CreateStringImpl(pszSubComp));
			CCTHROW_BAD_ALLOC(pSubComp);
			if(!m_pAttributes->SetValue(AVSubmit::IAVSubmission::eSubEngine, pSubComp))
				hrx << E_FAIL;
		}
	}
	CATCH_RETURN(hr);
}

/*
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
*/

/*
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
		hrx << pBackup->FlushBackupSets();
		brx << m_pAttributes->SetValue(eSetId, guid);
	}
	CATCH_RETURN(hr)
}
*/

/*
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
	
		SYMRESULT sr = pSet->CreateBackupItem(QBackup::itemStream, pItem.m_p);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to create backup item: 0x%08X"), sr);
			hr = E_FAIL;
			break;
		}
		pBackupStream = pItem;
		if(pBackupStream == NULL)
		{
			CCTRCTXE0(_T("pBackupStream == NULL"));
			hr = E_NOINTERFACE;
			break;
		}
		
		sr = pItem->SetGivenType(avQBTypes::GT_AVREMEDIATIONACTION);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to set GT: 0x%08X"), sr);
			hr = E_FAIL;
			break;
		}
		sr = pBackupStream->Backup(pMemory);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to backup action: 0x%08X"), sr);
			hr = E_FAIL;
			break;
		}
		sr = pSet->StoreBackupItem(pItem, &item);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to store backup item: 0x%08x"), sr);
			hr = E_FAIL;
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

		sr = pSet->CreateBackupItem(QBackup::itemStream, pItemUndo.m_p);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to create backup item: 0x%08X"), sr);
			hr = E_FAIL;
			break;
		}

		pBackupStream = pItemUndo;
		if(pBackupStream == NULL)
		{
			CCTRCTXE0(_T("pBackupStream == NULL"));
			hr = E_NOINTERFACE;
			break;
		}
		
		sr = pItemUndo->SetGivenType(avQBTypes::GT_AVREMEDIATIONACTIONUNDO);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Set GT failed: 0x%08X"), sr);
			hr = E_FAIL;
			break;
		}
		sr = pBackupStream->Backup(pUndo);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to save backup stream: 0x%08X"), sr);
			hr = E_FAIL;
			break;
		}

		sr = pSet->StoreBackupItem(pItemUndo, &itemUndo);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to store item: 0x%08X"), sr);
			hr = E_FAIL;
			break;
		}

		sr = pItem->GetProperties(pProps.m_p);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to get undo item props: 0x%08X"), sr);
			hr = E_FAIL;
			break;
		}

		if(pProps->SetValue(avQBTypes::avRAProp_MatchingUndoID, itemUndo) == false)
		{
			CCTRCTXE0(_T("Failed to set matching undo id"));
			hr = E_FAIL;
			break;
		}

		pProps.Release();

		sr = pItemUndo->GetProperties(pProps.m_p);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to get undo item props: 0x%08X"), sr);
			hr = E_FAIL;
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
		SYMRESULT sr = pSet->Flush();
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to flush set: 0x%08X"), sr);
			hr = E_FAIL;
		}
	}
	else
		CCTRCTXE1(_T("Failed 0x%08X - not flushing set"), hr);

	return hr;
}
*/

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
		brx << m_pAttributes->GetValue(AVSubmit::IAVSubmission::eVirusName, pBase.m_p);
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

		size_t nCount;
		hrx << GetRemediationCount(nCount);
		for (size_t i = 0; i < nCount; i++)
		{
			ccEraser::IRemediationActionPtr ptrRemediation;
			cc::IStreamPtr ptrStream;

			hrx << GetRemediation(i, ptrRemediation.m_p, ptrStream.m_p);

			pString.Release();
			pBase.Release();
			if(ptrRemediation == NULL)
				continue;

			ccEraser::eObjectType type;
			if(ccEraser::Failed(ptrRemediation->GetType(type)))
				continue;

			cc::IKeyValueCollectionPtr pProps;
			if(ccEraser::Failed(ptrRemediation->GetProperties(pProps.m_p)) || pProps == NULL)
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
			if(i != (nCount-1))
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

HRESULT CSubmissionBase::SetSubmittedKey()
{
	return E_NOTIMPL;	//TODO:JJM: reimplement this
	/*
	HRESULT hr = E_FAIL;

	for(;;)
	{
		if(!m_bInitializing)
		{
			hr = S_OK;
			break;
		}
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
		SYMRESULT sr = pBackup->RetrieveBackupSetByID(guid, pSet.m_p);
		if(SYM_FAILED(sr) || pSet == NULL)
		{
			CCTRCTXE1(_T("backup set not found"), sr);
			hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
			break;
		}

		cc::IKeyValueCollectionPtr pUserData;
		sr = pSet->GetUserData(pUserData.m_p);

		if(pUserData == NULL || SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("pUserData == NULL: 0x%08X"), sr);;
			//TEMPORARY WorkAround due to bug in QBackup
			cc::IKeyValueCollectionPtr pProps;
			pSet->GetProperties(pProps.m_p);
			if(pProps != NULL)
			{
				pUserData.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
				if(pUserData == NULL)
				{
					hr = E_OUTOFMEMORY;
					break;
				}
				if(!pProps->SetValue(500, pUserData))
				{
					CCTRCTXE0(_T("Failed to set userdata"));
					hr = E_FAIL;
					break;
				}
			}
			//END Temporary WorkAround

			//hr = E_FAIL;
			//break;
		}
		if(!pUserData->SetValue(AVSubmit::eQBackupSubmittedKey, true))
		{
			CCTRCTXE0(_T("Failed to set submitted key"));
			break;
		}

		sr = pSet->Flush();
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to flush backup set after setting submitted key"), sr);
			break;
		}
		
		CCTRCTXI0(_T("Set QBackup submitted key, flushed set"));
		hr = S_OK;
		break;			
	}
	return hr;
	*/
}

HRESULT CSubmissionBase::SetThreatCatProperties(const ccEraser::IAnomaly* pAnomaly) throw()
{
	HRESULT hr = S_OK;
	for(;;)
	{
		if(pAnomaly == NULL)
		{
			CCTRCTXE0(_T("pAnomaly == NULL"));
			hr = S_OK; //non-failure;
			break;
		}
		cc::IKeyValueCollectionConstPtr pProps;
		ccEraser::eResult eRes = pAnomaly->GetProperties(pProps.m_p);
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
		if(m_pAttributes->SetValue(AVSubmit::IAVSubmission::eThreatCats, pLocalCats) == false)
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


//IAVSubmission
HRESULT CSubmissionBase::GetAnomaly(ccEraser::IAnomaly*& pAnomaly) const throw()
{
	// Do nothing by default. Derived classes that provide anomalies will override this implementation.
	utils::FreeObject(pAnomaly);
	return S_OK;
}

HRESULT CSubmissionBase::GetRemediationCount(size_t& nSize) const throw()
{
	// Do nothing by default. Derived classes that provide anomalies will override this implementation.
	nSize = 0;
	return S_OK;
}

HRESULT CSubmissionBase::GetRemediation(size_t nIndex, ccEraser::IRemediationAction*& pRemediation, cc::IStream*& pStream) const throw()
{
	// Do nothing by default. Derived classes that provide anomalies will override this implementation.
	utils::FreeObject(pRemediation);
	utils::FreeObject(pStream);
	return S_OK;
}

} // namespace SAVSubmission
