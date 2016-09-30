////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "manager.h"
#include "AVInterfaces.h"
#include "AVSubmissionLoader.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include "SubmissionEngineLoader.h"
#include "AddSubmission.h"
#include "ccSymStringImpl.h"
#include "navver.h"
#include "ccFile.h"
#include "SmartModuleHandle.h"
#include "ISVersion.h"
#include "NAVSettingsHelperEx.h"
#include "OptNames.h"
#include "ccScanwInterface.h"
#include "Utils.h"

namespace AVSubmit {

namespace utils {
namespace {
	static ccLib::CCriticalSection s_cs;
	static bool s_bAutoSubmit = false;
	static DWORD s_dwLast = 0;
}

	//throttle this call so we dont check all the time when processing large volumes of submissions.
	//Should still be responsive enough for user's sake.

	static bool CanAutoSubmit()
	{
		ccLib::CSingleLock lk(&s_cs, INFINITE, false);
		
		DWORD tkc = ::GetTickCount();
		if(s_dwLast == 0 || (tkc - s_dwLast > 60000UL)) //1min
		{
			s_dwLast = tkc;

			//this object currently will throw() on allocation failure
			CNAVOptSettingsEx navSettings;
			if( navSettings.Init() )
			{
				DWORD dwVal = 0;
				HRESULT hr = navSettings.GetValue(SUBMISSIONS_FeatureEnabled, dwVal, 0);
				if(SUCCEEDED(hr))
				{
					s_bAutoSubmit = dwVal != 0;
					CCTRCTXI1(_T("Automatic Submissions are %s"), s_bAutoSubmit ? _T("Enabled") : _T("Disabled"));
				}
				else
					CCTRCTXE2(_T("Failed to get %s: 0x%08X"), SUBMISSIONS_FeatureEnabled, hr);
			}
			else
				CCTRCTXE0(_T("Failed to init options"));
		}
		return s_bAutoSubmit;
	}

	template <typename T, typename LDR> HRESULT LoadAVObject(T*& t, T*& pOut, LDR& ldr) throw()
	{
		utils::FreeObject(pOut);
		HRESULT hr = S_OK;
		if(t == NULL)
		{
			SYMRESULT sr = ldr.CreateObject(t);
			if(SYM_FAILED(sr))
			{
				CCTRCTXE1(_T("Failed to create object: 0x%08X"), sr);
				hr = HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
			}
		}
		if(SUCCEEDED(hr))
		{
			pOut = t;
			if(pOut != NULL)
				pOut->AddRef();
			else
			{
				CCTRCTXE0(_T("pOut ==  NULL"));
				hr = E_POINTER;
			}
		}
		return hr;
	}
} //namespace utils
namespace {
	inline bool ScanwSuccess(ccScanw::SCANWSTATUS status)
	{
		return status == ccScanw::SCANW_OK || status == ccScanw::SCANW_OK_OLD_DEFINITIONS;
	}

/***
	struct ScannerStuff : ccScanw::IScanwSink, ccScanw::IScanwProperties, ISymBaseImpl<CSymThreadSafeRefCount>
	{
		typedef CSymPtr<ScannerStuff> Ptr;

		SYM_INTERFACE_MAP_BEGIN()
			SYM_INTERFACE_ENTRY(ccScanw::IID_ScanwSink, ccScanw::IScanwSink)
			SYM_INTERFACE_ENTRY(ccScanw::IID_ScanwProperties, ccScanw::IScanwProperties)
		SYM_INTERFACE_MAP_END()


		virtual ccScanw::SCANWSTATUS OnBusyW() throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnError( ccScanw::IScanwError* pError ) throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnNewItem( const wchar_t * pszItem ) throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnInfectionFound( ccScanw::IScanwInfection* pInfection ) throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnInfectionHandlingComplete( ccScanw::IScanwInfection* pInfection) throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnRepairFailed( ccScanw::IScanwInfection* pInfection ) throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnRemoveMimeComponent( ccScanw::IScanwInfection* pInfection, wchar_t * pszMessage, int iMessageBufferSize ) throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS LockVolume( const wchar_t* szVolume, bool bLock ) throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnCleanFile( const wchar_t * pszFileName, const wchar_t * pszTempFileName, ccScanw::ScanwFileAttribute lAttribute, ccScanw::IScanwCompressedFile*) throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnBeginThreadW() throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnEndThreadW() throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnNewFile( const wchar_t * pszFileName, ccScanw::ScanwFileAttribute lAttribute ) throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnNewDirectory( const wchar_t * pszDirectoryName, ccScanw::ScanwFileAttribute lAttribute ) throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnDeleteFailed( ccScanw::IScanwInfection* pInfection ) throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnPostProcessContainer( const wchar_t * pszFileName ) throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS OnNewCompressedChild( ::decABI::IDecomposerContainerObject * pIDecomposerContainerObject, const wchar_t * pszChildName ) throw() {return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS GetProperty( const wchar_t * szProperty, int iDefault, int &riProperty ) const throw() {riProperty = iDefault; return ccScanw::SCANW_OK;}
		virtual ccScanw::SCANWSTATUS GetProperty( const wchar_t * szProperty, const wchar_t * szDefault, const wchar_t *&prwszProperty ) const throw() { prwszProperty = szDefault; return ccScanw::SCANW_OK;}
	};
***/
}


CManager::CManager(void) : m_eMode(eSynchronous), m_bInitialized(false)
{
	//intentionally empty...
}

CManager::~CManager(void)
{
	//ccLib::CThread will be sure the thread is valid before waiting on it...
	if(m_thread.IsThread())
	{
		::SetThreadPriority(m_thread.GetHandle(), THREAD_PRIORITY_NORMAL);
		if(m_thread.Terminate(5000) == FALSE)
		{
			CCTRCTXE0(_T("m_thread.Terminate(5000) == FALSE"));
		}
	}

	if(m_pBackup != NULL)
	{
		HRESULT hr = m_pBackup->Uninitialize();
		if(FAILED(hr))
			CCTRCTXE1(_T("QBackup::Uninit == 0x%08X"), hr);
		m_pBackup.Release();
	}
}

HRESULT CManager::GetScanner(ccScanw::IScannerw*& pScan) throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		ccLib::CSingleLock lk(&m_objectLock, INFINITE, false);

		utils::FreeObject(pScan);
		
		if(m_pScanner == NULL)
		{
			ccScanw::IScannerwPtr pScanner;
			AVModule::IAVDefInfoPtr pInfo;
			cc::IStringPtr pPath;
			hrx << GetAVDefInfo(pInfo.m_p);
			hrx << pInfo->GetDefsDir(pPath.m_p);
			ccLib::CStringW sPath = pPath->GetStringW();

			SYMRESULT sr = m_ldr_ccscanw.CreateObject(pScanner.m_p);//ccScanw::mgdccScan_IScannerw::CreateObject(GETMODULEMGR(), pScanner.m_p);
			if(SYM_FAILED(sr) || pScanner == NULL)
			{
				CCTRCTXE1(_T("Failed to load scanner: 0x%08X"), sr);
				hrx << HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
			}
			wchar_t szTempDir[MAX_PATH];
			GetTempPathW(MAX_PATH, szTempDir);
			
			ccScanw::SCANWSTATUS stat = pScanner->Initialize(pPath->GetStringW(), szTempDir, 3, ccScanw::IScannerw::ISCANNERW_USE_DEF_PATH);
			if(!ScanwSuccess(stat))
			{
				CCTRCTXE1(_T("Failed to init scanner: %d"), stat);
				hrx << E_FAIL;
			}
			m_pScanner = pScanner;
		}
		pScan = m_pScanner;
		pScan->AddRef();
	}
	CATCH_RETURN(hr);
}

HRESULT CManager::GetAVDefInfo(AVModule::IAVDefInfo*& pInfo) throw()
{
	ccLib::CSingleLock lk(&m_objectLock, INFINITE, false);

	HRESULT hr = utils::LoadAVObject(m_pDefInfo.m_p, pInfo, m_ldr_definfo);
	if(FAILED(hr))
		CCTRCTXE1(_T("Failed to load def info: 0x%08X"), hr);
	return hr;
}

HRESULT CManager::GetAVThreatInfo(AVModule::IAVThreatInfo*& pInfo) throw()
{
	ccLib::CSingleLock lk(&m_objectLock, INFINITE, false);

	HRESULT hr = utils::LoadAVObject(m_pThreatInfo.m_p, pInfo, m_ldr_threatinfo);
	if(FAILED(hr))
		CCTRCTXE1(_T("Failed to load threat info: 0x%08X"), hr);
	return hr;
}

HRESULT CManager::GetEraser(ccEraser::IEraser4*& pe) throw()
{
	STAHLSOFT_HRX_TRY(hr)
	{
		ccLib::CSingleLock lk(&m_objectLock, INFINITE, false);

		utils::FreeObject(pe);
		if(m_pEraser == NULL)
		{
			AVModule::IAVDefInfoPtr pInfo;
			cc::IStringPtr pPath;
			ccEraser::IEraser4Ptr pEraser;
			
			hrx << GetAVDefInfo(pInfo.m_p);
			hrx << pInfo->GetDefsDir(pPath.m_p);

			ccLib::CStringW sPath = pPath->GetStringW();
			PathAppendW(sPath.GetBuffer(MAX_PATH), ATL::CT2W(cc::sz_ccEraser_dll));
			sPath.ReleaseBuffer();
			
			SYMRESULT sr = m_ldr_eraser.Initialize(sPath);
			if(SYM_FAILED(sr))
			{
				CCTRCTXE1(_T("Failed to load eraser: 0x%08X"), sr);
				hrx << HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
			}
			sr = m_ldr_eraser.CreateObject(ccEraser::IID_Eraser, ccEraser::IID_Eraser4, (void**)&pEraser);
			if(SYM_FAILED(sr) || pEraser == NULL)
			{
				CCTRCTXE1(_T("Faield to create eraser: 0x%08X"), sr);			
				hrx << E_NOINTERFACE;
			}

			//ccScanw::IScannerwPtr pScanner;
			//hrx << GetScanner(pScanner.m_p);
			//ScannerStuff::Ptr pScanSinks = new ScannerStuff;
			//CCTHROW_BAD_ALLOC(pScanSinks);

			//TODO: provide scan sink & props...
			//this is probably bad...

			//ccEraser::eResult res = pEraser->Initialize(pScanner, pScanSinks, pScanSinks, ccEraser::CurrentVersion);
			ccEraser::eResult res = pEraser->Initialize(NULL, NULL, NULL, ccEraser::CurrentVersion);
			if(ccEraser::Failed(res))
			{
				CCTRCTXE1(_T("Eraser init fails: %d"), res);
				hrx << E_FAIL;
			}
			m_pEraser = pEraser;
		}
		pe = m_pEraser;
		pe->AddRef();
	}
	CATCH_RETURN(hr);
}

HRESULT CManager::GetSubmissionEngine(clfs::ISubmissionEngine*& pSubmissionEngine) throw()
{
	ccLib::CSingleLock lk(&m_objectLock, INFINITE, false);

	utils::FreeObject(pSubmissionEngine);

	HRESULT hr = S_OK;

	if(m_pSubmissionEngine == NULL)
	{
		clfs::ISubmissionEnginePtr pEngine;
		SYMRESULT sr = m_ldr_subeng.CreateObject(pEngine.m_p);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(_T("Failed to load subeng: 0x%08X"), sr);
			hr = HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
		}
		else
		{
			hr = pEngine->Initialize(NULL);
			if(FAILED(hr))
				CCTRCTXE1(_T("failed to init subeng: 0x%08X"), hr);
			else
				m_pSubmissionEngine = pEngine;
		}
	}
	if(SUCCEEDED(hr))
	{
		pSubmissionEngine = m_pSubmissionEngine;
		if(pSubmissionEngine != NULL)
			pSubmissionEngine->AddRef();
		else
			hr = E_POINTER;
	}
	return hr;
}

HRESULT CManager::GetQBackup(QBackup::IQBackup*& pBackup) throw()
{
	ccLib::CSingleLock lk(&m_objectLock, INFINITE, false);
	STAHLSOFT_HRX_TRY(hr)
	{
		utils::FreeObject(pBackup);
		if(m_pBackup == NULL)
		{
			QBackup::IQBackupPtr p;
			SYMRESULT sr = m_ldr_backup.CreateObject(p.m_p);//QBackup_IQbackupMgd::CreateObject(GETMODULEMGR(), p.m_p);
			if(FAILED(sr) || p == NULL)
			{
				CCTRCTXE1(_T("Faield to load qbackup: 0x%08X"), sr);
				hrx << HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
			}
			hr = p->Initialize();
			if(FAILED(sr))
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

HRESULT CManager::Initialize(Mode eMode) throw()
{
	if(m_bInitialized)
		return S_OK;

	STAHLSOFT_HRX_TRY(hr)
	{
		ccSym::CStringImplPtr pProduct, pVersion, pActor;

		m_eMode = eMode;

		if(m_eMode == eAsynchronous)
		{
			if(!m_thread.Create(NULL, 0, 0))
			{
				CCTRCTXE1(_T("Failed to create task thread: %d"), GetLastError());
				hrx << HRESULT_FROM_WIN32(ERROR_MAX_THRDS_REACHED);
			}
		}

		pProduct.Attach(ccSym::CStringImpl::CreateStringImpl());
		pVersion.Attach(ccSym::CStringImpl::CreateStringImpl());
		pActor.Attach(ccSym::CStringImpl::CreateStringImpl());

		CCTHROW_BAD_ALLOC(pProduct);
		CCTHROW_BAD_ALLOC(pVersion);
		CCTHROW_BAD_ALLOC(pActor);

		hrx << GetVersion(pVersion->GetStringW());
		hrx << GetProductName(pActor->GetStringW());
		pProduct->GetStringW().Format(L"%S", VER_STR_PRODUCT_SIRNAME);

		m_pProduct = pProduct;
		m_pVersion = pVersion;
		m_pActor = pActor;

		m_bInitialized = true;
	}
	CATCH_RETURN(hr);
}

HRESULT CManager::Add(AVModule::IAVMapBase* pData, DWORD dwType) throw()
{
	if(!AutoSubmitEnabled())
	{
		CCTRCTXW0(_T("Automatic submissions are disabled.  Not Submitting"));
		return S_OK;
	}

	CAddSubmission::AutoPtr p(new (std::nothrow) CAddSubmission);
	if(p.get() == NULL)
		return E_OUTOFMEMORY;

	if(!p->Initialize(pData, m_pProduct, m_pVersion, m_pActor))
	{
		CCTRCTXE0(_T("Failed to init submission task"));
		return E_FAIL;
	}
	p->SetType(dwType);
	
	return PostTask(p);
}

HRESULT CManager::ManualAdd(REFGUID setId, REFGUID itemId) throw()
{
	CCTRCTXE0(_T("Not Implemented"));
	return E_NOTIMPL;
}

HRESULT CManager::ManualAdd(AVModule::IAVMapBase* pData) throw()
{
	CAddSubmission::AutoPtr p(new (std::nothrow) CAddSubmission);
	if(p.get() == NULL)
		return E_OUTOFMEMORY;

	cc::IStringPtr pActor;
	HRESULT hr = GetActor(pActor.m_p);
	if(FAILED(hr))
		return hr;

	if(!p->Initialize(pData, m_pProduct, m_pVersion, pActor))
	{
		CCTRCTXE0(_T("Failed to initialize add submission task"));
		return E_FAIL;
	}
	p->SetType(IAVSubmissionManager::eSample);
	p->SetManual();
	return PostTask(p);
}

HRESULT CManager::GetActor(cc::IString*& pString)
{
	cc::IStringPtr pActor = m_pActor;
	WCHAR szBuf[MAX_PATH] = {0};
	DWORD dwSize = MAX_PATH-1;
	if(GetUserNameW(szBuf, &dwSize))
	{
		pActor.Attach(ccSym::CStringImpl::CreateStringImpl(szBuf));
		if(pActor == NULL)
			return E_OUTOFMEMORY;
	}
	pString = pActor.Detach();
	return S_OK;
}

HRESULT CManager::PostTask(CAddSubmission::AutoPtr& p)
{
	HRESULT hr = S_OK;
	if(m_eMode == eSynchronous)
	{
		hr = p->Process();
	}
	else
	{
		if(m_thread.Add(p.get(), NULL, /* bAutoDelete = */ true))
		{
			p.release(); // release ownership, since this is an auto-delete task
		}
		else
		{
			CCTRCTXE0(_T("Failed to add task to thread"));
			hr = E_FAIL;
		}
	}
	return hr;
}

bool CManager::AutoSubmitEnabled() const
{
	bool bRes = false;
	ccLib::CExceptionInfo exInfo;
	try
	{
		bRes = utils::CanAutoSubmit();
	}
	CCCATCHMEM(exInfo);
	return bRes;
}

HRESULT CManager::GetVersion(ccLib::CStringW& sVersion) throw(std::bad_alloc, ATL::CAtlException)
{
	HRESULT hr = E_FAIL;

	for(;;)
	{
		const WCHAR* pszVersionKey = L"InternalRevision";
		const TCHAR* pszFileName = _T("VERSION.DAT");
		ccLib::CString sVersionFile;

		if(!ccSym::CInstalledApps::GetNAVDirectory(sVersionFile))
		{
			CCTRCTXE0(_T("Failed to find app install directory for NAV - not installed"));
			hr = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
			break;
		}

		PathAppend(sVersionFile.GetBuffer(MAX_PATH), pszFileName);
		sVersionFile.ReleaseBuffer();
		if(::GetFileAttributes(sVersionFile) == INVALID_FILE_ATTRIBUTES)
		{
			CCTRCTXE1(_T("Failed to find %s"), sVersionFile);
			hr = HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
			break;
		}

		const int nStringVersionSize = 64;
		WCHAR szStringVersion[nStringVersionSize]; szStringVersion[0] = 0;
		::GetPrivateProfileStringW(L"Versions", pszVersionKey, L"", szStringVersion, nStringVersionSize, ATL::CT2W(sVersionFile));

		if(szStringVersion[0] == 0)
		{
			CCTRCTXE0(_T("version is empty"));
			hr = HRESULT_FROM_WIN32(ERROR_NO_DATA);
			break;
		}

		sVersion = szStringVersion;
		CCTRACEI(CCTRCTX _T("Version = %ls"), szStringVersion);

		hr = S_OK;
		break;
	}
	if(FAILED(hr)) //just default to what we were  built with...
	{
		sVersion.Format(L"%d.%d.%d.%d", VER_NUM_PRODVERMAJOR, VER_NUM_PRODVERMINOR, VER_NUM_PRODVERSUBMINOR, VER_NUM_BUILDNUMBER);
		CCTRCTXW2(_T("Defaulting version to  buld version: %s due to error 0x%08X"), sVersion, hr);
		hr = S_OK;
	}
	return hr;
}

//will throw on alloc failure...
HRESULT CManager::GetProductName(ccLib::CStringW& sProd) throw(std::bad_alloc, ATL::CAtlException)
{
	sProd = CISVersion::GetProductName();
	CCTRCTXI2(_T("[%ls] length %d"), sProd, sProd.GetLength());
	return sProd.GetLength() > 0 ? S_OK : E_FAIL;
}


BOOL CManager::LowPriorityTaskThread::InitInstance()
{
	BOOL bRes = ccLib::CTaskThread::InitInstance();
	if(bRes != FALSE)
	{
		BOOL b = SetPriority(THREAD_PRIORITY_LOWEST);
		if(b == FALSE)
		{
			CCTRCTXE1(_T("Failed to set priority: %d"), GetLastError());
		}
		else
			CCTRCTXI0(_T("Successfully set thread priority to THREAD_PRIORITY_LOWEST"));
	}
	return bRes;
}

} //namespace AVSubmit