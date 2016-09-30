////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\eraserbase.h"
#include "ccmodulenames.h"
#include "NAVDefutilsloader.h"
#include "ccSymStringImpl.h"
#include "AVSubmissionLoader.h"
#include "avQBackupTypes.h"
#include "ccSymMemoryStreamImpl.h"
#include "ccsymindexvaluecollectionimpl.h"
#include <time.h>
#include "ccScanwInterface.h"
#include "ccScanwLoader.h"

#include "SmartHandleT.h"

#include <wincrypt.h>

namespace {
	class CHCRYPTPROVDtor
	{
	public:
		HRESULT Destroy(StahlSoft::CSmartHandleT<HCRYPTPROV,CHCRYPTPROVDtor>* pT)
		{
			HCRYPTPROV h = pT->Detach();
			if(h)
				::CryptReleaseContext(h, 0);
			return S_OK;
		}
	};

	typedef StahlSoft::CSmartHandleT<HCRYPTPROV,CHCRYPTPROVDtor> CSmartCryptProv;
}

static const char* const g_pszFileData = "a test file.\r\nrand=0x%08X\r\n";
static const wchar_t* const g_pszRegKey = L"HKEY_LOCAL_MACHINE\\Software\\MalWare.com\\child_key_%08X";
static const wchar_t* const g_szVirusName = L"a virus name like Blookhound.Exploit.1";

namespace {
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
}

CEraserBase::CEraserBase(const wchar_t* name, const wchar_t* descr) : ccUnit::CTestCaseImpl(name, descr)
{
}

CEraserBase::~CEraserBase(void)
{
	if(m_pBackup)
		m_pBackup->Uninitialize();
}


bool CEraserBase::LoadEraser()
{

	if(m_pEraser != NULL) return true;

	AVModule::mgdAVLoader_IAVGlobalScanner gLdr;
	gLdr.CreateObject(GETMODULEMGR(), m_pGlobal.m_p);

	DefUtilsLoader duLdr;
	IDefUtilsPtr pDefUtils;
	wchar_t buf[MAX_PATH];
	wchar_t buf2[MAX_PATH];

	CCUNIT_TEST_CONDITION_RETURN(duLdr.CreateObject(pDefUtils.m_p) == SYM_OK);
	CCUNIT_TEST_CONDITION_RETURN(pDefUtils->InitWindowsApp(L"AVDEFMGR"));
	CCUNIT_TEST_CONDITION_RETURN(pDefUtils->SetDoAuthenticationCheck(false));
	CCUNIT_TEST_CONDITION_RETURN(pDefUtils->GetCurrentDefs(buf, MAX_PATH*sizeof(wchar_t)));
	wcscpy(buf2, buf);
	PathAppend(buf, cc::sz_ccEraser_dll);
	CCUNIT_TEST_CONDITION_RETURN(GETMODULEMGR()->CreateObject(buf, ccEraser::IID_Eraser, ccEraser::IID_Eraser4, (void**)&m_pEraser) == SYM_OK);	
	CCUNIT_TEST_CONDITION_RETURN(ccScanw::mgdccScan_IScannerw::CreateObject(GETMODULEMGR(), m_pScanner.m_p) == SYM_OK);

	ScannerStuff::Ptr pStuff = new ScannerStuff;

	wchar_t szTempDir[MAX_PATH];
	GetTempPathW(MAX_PATH, szTempDir);		
	ccScanw::SCANWSTATUS stat = m_pScanner->Initialize(buf2, szTempDir, 3, ccScanw::IScannerw::ISCANNERW_USE_DEF_PATH);
	CCUNIT_TEST_CONDITION_RETURN(stat == ccScanw::SCANW_OK);
	CCUNIT_TEST_CONDITION_RETURN(m_pEraser->Initialize(m_pScanner, pStuff, pStuff, ccEraser::CurrentVersion) == ccEraser::Success);
	return true;
}

bool CEraserBase::LoadQBackup()
{
	if(m_pBackup != NULL) return true;

	ccLib::CStringW sPath;
	CCUNIT_TEST_CONDITION_RETURN(ccSym::CInstalledApps::GetCCDirectory(sPath));
	PathAppend(sPath.GetBuffer(MAX_PATH), L"Qbackup.dll"); sPath.ReleaseBuffer();
	CCUNIT_TEST_CONDITION_RETURN(GETMODULEMGR()->CreateObject(sPath, QBackup::SYMOBJECT_QBACKUP, QBackup::IID_QBackup, (void**)&m_pBackup) == SYM_OK);
	CCUNIT_TEST_CONDITION_RETURN(m_pBackup->Initialize(NULL) == S_OK);
	return true;
}

bool CEraserBase::LoadAVSubmit()
{
	if(m_pSubmit != NULL) return true;
	CCUNIT_TEST_CONDITION_RETURN(AVSubmit::AVSubmit_IAVSubmissionManagerMgd::CreateObject(GETMODULEMGR(), m_pSubmit.m_p) == SYM_OK);
	CCUNIT_TEST_CONDITION_RETURN(m_pSubmit->Initialize(AVSubmit::IAVSubmissionManager::eSynchronous) == S_OK);
	return true;
}

bool CEraserBase::CreateQBackupSet(QBackup::IQBackupSet*& pSet, REFGUID givenType)
{
	GUID id = GUID_NULL;
	CCUNIT_TEST_CONDITION_RETURN(m_pBackup->CreateBackupSet(pSet) == S_OK);
	CCUNIT_TEST_CONDITION_RETURN(pSet->SetGivenType(givenType) == S_OK);
	CCUNIT_TEST_CONDITION_RETURN(m_pBackup->StoreBackupSet(pSet, &id, false) == S_OK);
	return true;
}

bool CEraserBase::CreateFileRemediation(ccEraser::IRemediationAction*& pRemediation, LPCTSTR pszFile )
{
	cc::IStringPtr pString;
	cc::IKeyValueCollectionPtr pProps;
	ccEraser::IRemediationActionPtr pRem;
	if(pszFile == NULL)
	{
		ABORT_ON_FAIL(CreateTempFile(pString.m_p));
	}
	else
	{
		pString.Attach(ccSym::CStringImpl::CreateStringImpl(pszFile));

	}
	ccEraser::eResult eRes = m_pEraser->CreateObject(ccEraser::FileRemediationActionType, ccEraser::IID_RemediationAction, (void**)&pRem) ;
	CCTRCTXE1(L"CreateObject = %d", eRes);
	CCUNIT_TEST_CONDITION_RETURN(eRes == ccEraser::Success);
	CCUNIT_TEST_CONDITION_RETURN(pRem->GetProperties(pProps.m_p) == ccEraser::Success);
	CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(ccEraser::IRemediationAction::State, DWORD(ccEraser::IRemediationAction::NotRemediated)));
	CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(ccEraser::IRemediationAction::Operation, DWORD(ccEraser::IRemediationAction::Delete)));
	CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(ccEraser::IRemediationAction::Path, pString));
	pRemediation = pRem.Detach();
	return true;
}

bool CEraserBase::GenerateAnomaly(ccEraser::IAnomaly*& pAnomaly)
{
	cc::IStringPtr pFile;
	ccEraser::IEraser4QIPtr pEraser4 = m_pEraser;
	CCUNIT_TEST_CONDITION_RETURN(pEraser4 != NULL);
	ABORT_ON_FAIL(CreateTempFile(pFile.m_p));
	CCUNIT_TEST_CONDITION_RETURN(ccEraser::Succeeded(pEraser4->GenerateAnomaly(pFile, true, pAnomaly)));
	ABORT_ON_FAIL(InitializeAnomaly(pAnomaly));
	return true;
}

bool CEraserBase::CreateTempFile(cc::IString*& pString)
{
	ccLib::CFile file;
	wchar_t buf[MAX_PATH]; size_t sz = MAX_PATH;
	CCUNIT_TEST_CONDITION_RETURN(ccSym::CModulePathProvider::GetPath(buf, sz));
	ccLib::CStringW sfmt; sfmt.Format(L"SYMTEST-AVSubmit_UnitTest-%d-%p.bin", GetTickCount(), buf);
	PathAppend(buf, sfmt);
	pString = ccSym::CStringImpl::CreateStringImpl(buf);
	CCUNIT_TEST_CONDITION_RETURN(pString != NULL);
	ccLib::CStringA sData;

	ccLib::CMemory mem;
	//BYTE* b = (BYTE*)mem.NewAlloc(0x100000);
	//CCUNIT_TEST_CONDITION_RETURN(b != NULL);
	//memset(b, 0xfe, 0x100000);

	Sleep(10);

	sData.Format(g_pszFileData, GetTickCount());
	IMAGE_DOS_HEADER dos_hdr = {0};
	IMAGE_NT_HEADERS nt_hdr = {0};
	dos_hdr.e_magic = IMAGE_DOS_SIGNATURE;
	dos_hdr.e_lfanew = 0x420; //sizeof(IMAGE_DOS_SIGNATURE) would work as well...
	nt_hdr.Signature = IMAGE_NT_SIGNATURE;

	CCUNIT_TEST_CONDITION_RETURN(file.Open(buf, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL) != FALSE);
	CCUNIT_TEST_CONDITION_RETURN(file.Write(&dos_hdr, sizeof(dos_hdr)) != FALSE);
	CCUNIT_TEST_CONDITION_RETURN(file.Write(0x420, &nt_hdr, sizeof(nt_hdr)) != FALSE);
	CCUNIT_TEST_CONDITION_RETURN(file.Write(0x800, sData, sData.GetLength()) != FALSE);
	//CCUNIT_TEST_CONDITION_RETURN(file.Write(b, 0x100000) != FALSE);
	CCUNIT_TEST_CONDITION_RETURN(file.Close() != FALSE);
	return true;
}

bool CEraserBase::CreateRegistryRemediation(ccEraser::IRemediationAction*& pRemediation)
{
	ccEraser::IRemediationActionPtr pRem;
	cc::IKeyValueCollectionPtr pProps;
	cc::IStringPtr pName;
	ccLib::CStringW sName;

	Sleep(10);
	sName.Format(g_pszRegKey, GetTickCount());
	pName.Attach(ccSym::CStringImpl::CreateStringImpl(sName));

	CCUNIT_TEST_CONDITION_RETURN(m_pEraser->CreateObject(ccEraser::RegistryRemediationActionType, ccEraser::IID_RemediationAction, (void**)&pRem) == ccEraser::Success);
	CCUNIT_TEST_CONDITION_RETURN(pRem->GetProperties(pProps.m_p) == ccEraser::Success);
	CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(ccEraser::IRemediationAction::State, DWORD(ccEraser::IRemediationAction::NotRemediated)));
	CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(ccEraser::IRemediationAction::Operation, DWORD(ccEraser::IRemediationAction::Delete)));
	CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(ccEraser::IRemediationAction::KeyName, pName));

	pRemediation = pRem.Detach();
	return true;
}

bool CEraserBase::InitializeAnomaly(ccEraser::IAnomaly* pAnomaly)
{
	cc::IKeyValueCollectionPtr pProps;
	cc::IIndexValueCollectionPtr pData;
	pData.Attach(ccSym::CIndexValueCollectionImpl::CreateIndexValueCollectionImpl());
	CSmartCryptProv hProv;
	CCUNIT_TEST_CONDITION_RETURN(CryptAcquireContext(&hProv.m_h, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) != FALSE);
	CCUNIT_TEST_CONDITION_RETURN(hProv != NULL);
	BYTE buffer[0x200];
	for(size_t i = 0; i<30; ++i)
	{
		CCUNIT_TEST_CONDITION_RETURN(CryptGenRandom(hProv, 0x200, buffer) != FALSE);
		pData->AddValue(buffer, 0x200);
	}

	CCUNIT_TEST_CONDITION_RETURN(pAnomaly->GetProperties(pProps.m_p) == ccEraser::Success);
	CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(ccEraser::IAnomaly::SubmitData, true));
	CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(ccEraser::IAnomaly::HeuristicRank, DWORD(ccEraser::IAnomaly::Positive)));
	CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(ccEraser::IAnomaly::HeuristicVector, pData));
	return true;
}

bool CEraserBase::CreateAnomaly(ccEraser::IAnomaly*& pAnomaly)
{
	cc::IKeyValueCollectionPtr pProps;
	cc::IIndexValueCollectionPtr pCats;
	cc::IStringPtr pString;
	pString.Attach(ccSym::CStringImpl::CreateStringImpl(L"6.0.0.6"));
	pCats.Attach(ccSym::CIndexValueCollectionImpl::CreateIndexValueCollectionImpl());

	pCats->AddValue(DWORD(ccEraser::IAnomaly::Hacktool));
	pCats->AddValue(DWORD(ccEraser::IAnomaly::Heuristic));
	pCats->AddValue(DWORD(ccEraser::IAnomaly::SecurityRisk));

	CCUNIT_TEST_CONDITION_RETURN(m_pEraser->CreateObject(ccEraser::AnomalyType, ccEraser::IID_Anomaly, (void**)&pAnomaly) == ccEraser::Success);
	ABORT_ON_FAIL(InitializeAnomaly(pAnomaly));
	CCUNIT_TEST_CONDITION_RETURN(pAnomaly->GetProperties(pProps.m_p) == ccEraser::Success);
	CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(ccEraser::IAnomaly::HeuristicEngineVersion, pString));
	CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(ccEraser::IAnomaly::Categories, pCats));
	return true;
}
bool CEraserBase::CreateBackupStream(QBackup::IQBackupSet* pSet, QBackup::IQBackupItemStream*& pStream, REFGUID type)
{
	QBackup::IQBackupItemBasePtr pItem;
	GUID id = GUID_NULL;
	CCUNIT_TEST_CONDITION_RETURN(pSet->CreateBackupItem(QBackup::itemStream, pItem.m_p) == S_OK);
	QBackup::IQBackupItemStreamQIPtr pStreamBackup = pItem;
	CCUNIT_TEST_CONDITION_RETURN(pStreamBackup != NULL);
	CCUNIT_TEST_CONDITION_RETURN(pItem->SetGivenType(type) == S_OK);
	CCUNIT_TEST_CONDITION_RETURN(pSet->StoreBackupItem(pItem, &id) == S_OK);
	pStream = pStreamBackup.Detach();
	return true;
}

bool CEraserBase::SaveAnomaly(QBackup::IQBackupSet* pSet, ccEraser::IAnomaly* pAnomaly)
{
	CCUNIT_TEST_CONDITION_RETURN(pSet != NULL && pAnomaly != NULL);
	
	QBackup::IQBackupItemStreamPtr pBackup;
	ccSym::CMemoryStreamImplPtr pStream;
	pStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
	cc::ISerializeQIPtr pSerialize = pAnomaly;
	cc::IKeyValueCollectionPtr pProps;
	GUID id = {0};
	
	CCUNIT_TEST_CONDITION_RETURN(pStream != NULL);
	CCUNIT_TEST_CONDITION_RETURN(pSerialize != NULL);
	CCUNIT_TEST_CONDITION_RETURN(CreateBackupStream(pSet, pBackup.m_p, avQBTypes::GT_AVANOMALY));
	CCUNIT_TEST_CONDITION_RETURN(pSerialize->Save(pStream));
	CCUNIT_TEST_CONDITION_RETURN(pBackup->Backup(pStream) == S_OK);

	CCUNIT_TEST_CONDITION_RETURN(pBackup->GetID(id) == S_OK);
	CCUNIT_TEST_CONDITION_RETURN(pSet->GetProperties(pProps.m_p) == S_OK);
	CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(avQBTypes::avASProp_SerializedAnomalyBackupID, id));

	CCUNIT_TEST_CONDITION_RETURN(pSet->Flush() == S_OK);
	return true;

}

bool CEraserBase::SaveRemediation(QBackup::IQBackupSet* pSet, ccEraser::IRemediationAction* pRem, bool bSaveUndo)
{
	CCUNIT_TEST_CONDITION_RETURN(pSet != NULL && pRem != NULL);
	
	QBackup::IQBackupItemStreamPtr pBackup;
	ccSym::CMemoryStreamImplPtr pStream;
	pStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
	cc::ISerializeQIPtr pSerialize = pRem;
	
	CCUNIT_TEST_CONDITION_RETURN(pStream != NULL);
	CCUNIT_TEST_CONDITION_RETURN(pSerialize != NULL);
	CCUNIT_TEST_CONDITION_RETURN(CreateBackupStream(pSet, pBackup.m_p, avQBTypes::GT_AVREMEDIATIONACTION));
	CCUNIT_TEST_CONDITION_RETURN(pSerialize->Save(pStream));
	CCUNIT_TEST_CONDITION_RETURN(pBackup->Backup(pStream) == S_OK);
	if(bSaveUndo)
	{
		QBackup::IQBackupItemStreamPtr pBackupStream;
		cc::IKeyValueCollectionPtr pProps;
		GUID undoId;
		pStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		CCUNIT_TEST_CONDITION_RETURN(pStream != NULL);
		CCUNIT_TEST_CONDITION_RETURN(CreateBackupStream(pSet, pBackupStream.m_p, avQBTypes::GT_AVREMEDIATIONACTIONUNDO));
		CCUNIT_TEST_CONDITION_RETURN(pRem->GetUndoInformation(pStream) == ccEraser::Success);
		CCUNIT_TEST_CONDITION_RETURN(pBackupStream->Backup(pStream) == S_OK);
		CCUNIT_TEST_CONDITION_RETURN(pBackupStream->GetID(undoId) == S_OK);
		CCUNIT_TEST_CONDITION_RETURN(pBackup->GetProperties(pProps.m_p) == S_OK);
		CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(avQBTypes::avRAProp_MatchingUndoID, undoId));
		pProps.Release();
		CCUNIT_TEST_CONDITION_RETURN(pBackupStream->GetProperties(pProps.m_p) == S_OK);
		CCUNIT_TEST_CONDITION_RETURN(pBackup->GetID(undoId) == S_OK);
		CCUNIT_TEST_CONDITION_RETURN(pProps->SetValue(avQBTypes::avRAUProp_MatchingRemActID, undoId));

	}
	CCUNIT_TEST_CONDITION_RETURN(pSet->Flush() == S_OK);
	return true;
}

bool CEraserBase::CreateAVMap(QBackup::IQBackupSet* pSet, AVModule::IAVMapDwordData*& pMap)
{
	//need to get AVInterface to init app core via the ctor of another one of its objects...

	AVModule::mgdAVLoader_IAVMapDwordData ldr;
	AVModule::IAVMapDwordDataPtr pData;
	cc::IStringPtr pString;
	pString.Attach(ccSym::CStringImpl::CreateStringImpl(g_szVirusName));
	time_t t0 = time(NULL);
	GUID guid = GUID_NULL;
	

	CCUNIT_TEST_CONDITION_RETURN(pSet->GetID(guid) == S_OK);
	CCUNIT_TEST_CONDITION_RETURN(pString != NULL);
	CCUNIT_TEST_CONDITION_RETURN(ldr.CreateObject(GETMODULEMGR(), pData.m_p) == SYM_OK);
	CCUNIT_TEST_CONDITION_RETURN(pData->SetValue(AVModule::ThreatTracking::TrackingData_VID, DWORD(18973)) == S_OK); //Bloodhound.Exploit.1
	CCUNIT_TEST_CONDITION_RETURN(pData->SetValue(AVModule::ThreatTracking::TrackingData_NAME, g_szVirusName) == S_OK);
	CCUNIT_TEST_CONDITION_RETURN(pData->SetValue(AVModule::ThreatTracking::TrackingData_DETECTION_TIME, DWORD(t0)) == S_OK);
	CCUNIT_TEST_CONDITION_RETURN(pData->SetValue(AVModule::ThreatTracking::TrackingData_THREATID, guid) == S_OK);
	pMap = pData.Detach();
	return true;
}

bool CEraserBase::CreateAVStringMap(QBackup::IQBackupSet* pSet, AVModule::IAVMapStrData*& pMap)
{
	//need to get AVInterface to init app core via the ctor of another one of its objects...

	AVModule::mgdAVLoader_IAVMapStrData ldr;
	AVModule::IAVMapStrDataPtr pData;	
	GUID guid = GUID_NULL;

	if(pSet)
	{
		CCUNIT_TEST_CONDITION_RETURN(pSet->GetID(guid) == S_OK);
	}
	CCUNIT_TEST_CONDITION_RETURN(ldr.CreateObject(GETMODULEMGR(), pData.m_p) == SYM_OK);
	CCUNIT_TEST_CONDITION_RETURN(pData->SetValue(AVModule::EmailScanner::AV_EMAILOEH_QBACKUPID, guid) == S_OK);
	pMap = pData.Detach();
	return true;
};

bool CEraserBase::AutoSubmit(AVModule::IAVMapBase* pmap, DWORD dwType)
{
	CCUNIT_TEST_CONDITION_RETURN(m_pSubmit->Add(pmap, dwType) == S_OK);
	return true;
}

bool CEraserBase::ManualSubmit(AVModule::IAVMapDwordData* pmap)
{
	CCUNIT_TEST_CONDITION_RETURN(m_pSubmit->ManualAdd(pmap) == S_OK);
	return true;
}
