////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include ".\testengine.h"
#include "ccLibStaticLink.h"
#define INITIIDS
#include "ccSymStringImpl.h"
#include "SymInterface.h"
#include "SymInterfaceLoader.h"
#include "ccStringInterface.h"
#include "ccTrace.h"
#include "ccSymDebugOutput.h"
#include "ccLibStd.h"



#include "ExclusionInterface.h"
#include "ExclusionManagerInterface.h"
#include "ExclusionFileLoader.h"

#include "ccSymMemoryStreamImpl.h"
//#include "ccValueCollectionInterface.h"

#include "ccSymKeyValueCollectionImpl.h"
#include "ccMemoryInterface.h"

#include "time.h"
#include <vector>

ccSym::CDebugOutput g_DebugOutput(_T("N32EXCLUtest"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

using namespace NavExclusions;

IExclusionManagerPtr m_mgrPtr;
CSymInterfaceLoader m_csil;

ccSettings::ISettingsManagerPtr pSetMgr;
ccSettings::IEnumValuesPtr pEnumSettings;
ccSettings::ISettingsPtr pSettings;
ccSettings::CSettingsManagerHelper cSetManHelper;
DWORD dwEnumCurrent;

std:: vector <CString> m_strVector;
std:: vector <std::vector <CString> > m_vVector;

bool enumLoadExclusionData(cc::IKeyValueCollection* &pData);
void printMyStuff(cc::IKeyValueCollection* &pData);

TestEngine::TestEngine(void)
{
	fprintf(stdout, "+--------------------------------------------------------------+\n");
	fprintf(stdout, "|Exclusion Engine Test                                         |\n");
	fprintf(stdout, "|NAV 2006  V2.0                                                |\n");
	fprintf(stdout, "+--------------------------------------------------------------+\n");
	if (CoInitializeEx(NULL, COINIT_APARTMENTTHREADED) == S_OK)
	{
		fprintf(stdout,"STARTING COM...\n");
	}
	else{
		fprintf(stdout, "Can not start COM, exiting... \n");
		exit(0);
	}
	//if (SYM_OK == m_csil.Initialize("D:\\NAV2006_CM\\Norton_AntiVirus\\Consumer\\src\\BIN.IRA\\N32Exclu.dll"))
	if (SYM_OK == m_csil.Initialize("N32Exclu.dll"))
					fprintf(stdout, "Initialized DLL \n");
	else{
		fprintf(stdout,"Can not initialize DLL, exiting... \n");
		exit(0);
	}
	
	if (SYM_OK == m_csil.CreateObject(IID_IExclusionManager, IID_IExclusionManager, (void**)&m_mgrPtr))
				fprintf(stdout,"Create IExculsion Manager... \n");
	else{
		fprintf(stdout,"Can not create exclusion manager, existing... \n");
		exit(0);
	}

	if(Success == m_mgrPtr->initialize(false))
					fprintf(stdout, "Init manager ptr... \n");
	else{
		fprintf(stdout, "Can not initialize manager pointer, exiting... \n");
		exit(0);
	}
}

TestEngine::~TestEngine(void)
{
	m_mgrPtr.Release();
	CoUninitialize();
}

bool TestEngine::Addfs(char* strPath, time_t tTime, int iState, bool bIsOem)
{
	time_t tNowTime = time(NULL) ;
	if (tTime == 0)
		tNowTime = 0;
	IExclusion::ExclusionState eState = (IExclusion::ExclusionState) iState;

	ExclusionId tempId; 
	//tempId = m_mgrPtr->addExclusionFS(strPath, (tNowTime+tTime),  eState, bIsOem);
	cc::IStringPtr tempPath;
	CString cstrPath (strPath);
	tempPath.Attach(ccSym::CStringImpl::CreateStringImpl());

	tempPath->SetStringW(CA2W(cstrPath));
	tempId = m_mgrPtr->addExclusionFS(tempPath, (tNowTime+tTime),  eState);

	if (tempId != _INVALID_ID)
	{
		fprintf(stdout, "	Added Item number: %d\n", tempId);
		return true;
	}
	else
	{
		fprintf(stdout, "	Faile to add this exclusion \n");
		return false;
	}
}
bool TestEngine::Adda(ULONG ulVid, char* cGuid, time_t tTime, char* cName, int iState)
{
	cc::IStringPtr pStrGuid;
	cc::IStringPtr pStrName;

	CString strGuid(cGuid);
	pStrGuid.Attach(ccSym::CStringImpl::CreateStringImpl());	
	if (!strGuid.Compare(""))
		pStrGuid=NULL;
	else
		pStrGuid->SetStringW(CA2W(strGuid));

	CString strName(cName);
	pStrName.Attach(ccSym::CStringImpl::CreateStringImpl());
	pStrName->SetStringW(CA2W(strName));

	time_t tNowTime = time(NULL) ;
	if (tTime == 0)
		tNowTime = 0;
	IExclusion::ExclusionState eState = (IExclusion::ExclusionState) iState;

	ExclusionId tempId = m_mgrPtr->addExclusionAnomaly(ulVid, pStrGuid, (tNowTime+tTime), pStrName, eState);
	if (tempId != _INVALID_ID)
	{
		printf("	Added Item number: %d\n", tempId);
		return true;
	}
	else
	{
		printf("	Failed to add this exclusion \n");
		return false;
	}
}
bool TestEngine::Findfs (char *strPath, bool isOem)
{
	ExclusionId tempId = 0;
	tempId = m_mgrPtr->findExclusionFS(strPath, false);//need to add testing for bIsoem,
	if (tempId != _INVALID_ID)
	{
		printf("	Found Item number: %d\n", tempId);
		return true;
	}
	else
	{
		printf("	Exclusion not found!\n");
		return false;
	}
}
bool TestEngine::Finda(ULONG ulVid, char* cGuid)
{
	ExclusionId tempId = 0;
	cc::IStringPtr pStrGuid;
	CString strGuid(cGuid);
		
	pStrGuid.Attach(ccSym::CStringImpl::CreateStringImpl());
	if (!strGuid.Compare(_T("")))
		pStrGuid=NULL;
	else
		pStrGuid->SetStringW(CA2W(strGuid));

	tempId = m_mgrPtr->findExclusionAnomaly(ulVid, pStrGuid);

	if (tempId != _INVALID_ID)
	{	
		printf("	Found Anomaly with ID: %d\n", tempId);
		return true;
	}
	else
	{
		printf("	Exclusion not found!\n");
		return false;
	}
}
bool TestEngine::Get(ULONG tempId)
{
	IExclusion *ie = NULL;
	ExclusionId iTempId = (ExclusionId) tempId;
	IExclusion::ExclusionType type;
	cc::IStringPtr pStrGuid;
	cc::IStringPtr pGuid;
	cc::IStringPtr pDisplayName;
	cc::IStringPtr pPath;

	if(Success == m_mgrPtr->getExclusion( iTempId, ie))
	{
		ie->getDisplayName(pDisplayName);
		type = ie->getExclusionType();
		ie->getExclusionGuid(pStrGuid);
			
		wprintf(L"  Exclusion:  %s\n", pStrGuid->GetStringW());
		wprintf(L"	Display Name:  %s\n", pDisplayName->GetStringW());
		wprintf(L"	ID:  %u \n",ie->getID());
		wprintf(L"	State: %u \n", ie->getState());
				
		if (type==1){
			wprintf(L"	Type: File System \n");
			IFileSystemExclusionQIPtr ifse = ie;
			if(ifse != NULL)
			{
				if (Success == ifse->getFSPath(pPath))
					wprintf(L"	Path: %s \n", pPath->GetStringW()); 
			}
		}
		else if (type==2){
			wprintf(L"	Type: Anomaly \n");
			IAnomalyExclusionQIPtr iae = ie;
			if (iae !=NULL){
				wprintf(L"	VID: %u \n" , iae->getVid());
			if(Success == iae->getAnomalyGuid(pGuid))
				wprintf(L"	Anomaly GUID: %s \n", pGuid->GetStringW());
			}
		}
		else
			wprintf(L"	Type: Invaild \n");
			
		if (Success==ie->isExpired())
			wprintf(L"	This Exclusion is EXPIRED!\n");
		if (Success==ie->isDeleted())
			wprintf(L"	This Exclusion is marked DELETED! \n");
		if (Success==ie->isActive())
			wprintf(L"	This Exclusion is ACTIVE! \n");
		if (Success==ie->isValidForMatch())
			wprintf(L"	This Exclusion is Valid For Match! \n");
		return true;
			
	}else{
		printf("	Fail to get exclusion #%u \n", iTempId);
		return false;
	}
}
bool TestEngine::Isfs(char* strPath, bool isOem, int iState)
{
	IExclusion::ExclusionState state = m_mgrPtr->isExcludedFS(strPath, isOem, (IExclusion::ExclusionState)iState);

	if ( (state & IExclusion::NOT_EXCLUDED))
			printf("	NOT EXCLUDED");
	if ( (state & IExclusion::EXCLUDE_VIRAL))
			printf("	EXCLUDE_VIRAL");
	if ( (state & IExclusion::EXCLUDE_NONVIRAL))
			printf("	EXCLUDE_NONVIRAL");
	if ( (state & IExclusion::EXCLUDE_SUBDIRS_VIRAL))
			printf("	EXCLUDE_SUBDIRS_VIRAL");
	if ( (state & IExclusion::EXCLUDE_SUBDIRS_NONVIRAL))
			printf("	EXCLUDE_SUBDIRS_NONVIRAL");

	printf("	%d\n", state);

	return true;
}
bool TestEngine::Isa(ULONG ulVid, char* cGuid, int iState)
{
	cc::IStringPtr pStrGuid;
	CString strGuid(cGuid);
	pStrGuid.Attach(ccSym::CStringImpl::CreateStringImpl());
	if (!strGuid.Compare(_T("")))
		pStrGuid=NULL;
	else
	pStrGuid->SetStringW(CA2W(strGuid));

	IExclusion::ExclusionState state = m_mgrPtr->isExcludedAnomaly(ulVid, pStrGuid, IExclusion::ExclusionState (iState));

	if ( (state & IExclusion::NOT_EXCLUDED))
			printf("	NOT EXCLUDED");
	if ( (state & IExclusion::EXCLUDE_VIRAL))
			printf("	EXCLUDE_VIRAL");
	if ( (state & IExclusion::EXCLUDE_NONVIRAL))
			printf("	EXCLUDE_NONVIRAL");
	if ( (state & IExclusion::EXCLUDE_SUBDIRS_VIRAL))
			printf("	EXCLUDE_SUBDIRS_VIRAL");
	if ( (state & IExclusion::EXCLUDE_SUBDIRS_NONVIRAL))
			printf("	EXCLUDE_SUBDIRS_NONVIRAL");
	
	printf("	%d\n", state);

	return true;

}
bool TestEngine::Remove(ULONG tempId)
{
	ExclusionId iTempId = (ExclusionId) tempId;

	if (m_mgrPtr->removeExclusion(iTempId))
	{
		printf("	Exclusion # %u has been removed \n", iTempId);
		return true;
	}
	else
	{
		printf("	Fail to remove # %u \n", iTempId);
		return false;
	}
}
bool TestEngine::List()
{
	int iTemp=0;
	IExclusion *ie = NULL;
	ExclusionId tempId;
			 
	IExclusion::ExclusionType type;
	cc::IStringPtr pStrGuid;
	cc::IStringPtr pGuid;
	cc::IStringPtr pDisplayName;
	cc::IStringPtr pPath;

	m_mgrPtr->enumReset();
		
	while ( tempId = m_mgrPtr->enumGetNextID()!= _INVALID_ID)
	{
		iTemp++;
	}
	printf("Total number of items:  %d \n", iTemp);

	m_mgrPtr->enumReset();
	while (( m_mgrPtr->enumGetNextExclusionObj(ie)== Success))
	{	
		ie->getDisplayName(pDisplayName);
		type = ie->getExclusionType();
		ie->getExclusionGuid(pStrGuid);
	
		wprintf(L"  Exclusion:  %s\n", pStrGuid->GetStringW());
		wprintf(L"    Display Name:  %s\n", pDisplayName->GetStringW());
		wprintf(L"    ID:  %u \n",ie->getID());
		wprintf(L"    State: %u \n", ie->getState());
		time_t tTime = ie->getExpiryStamp();
		printf("    Time expiry: %s ", ctime(&tTime));
		
		if (type==1){
			wprintf(L"   Type: File System \n");
			IFileSystemExclusionQIPtr ifse = ie;
			if(ifse != NULL)
			{
				if (Success == ifse->getFSPath(pPath))
					wprintf(L"    Path: %s \n", pPath->GetStringW()); 
			}
		}
		else if (type==2){
			wprintf(L"   Type: Anomaly \n");
			IAnomalyExclusionQIPtr iae = ie;
			if (iae !=NULL){
	            wprintf(L"    VID: %u \n" , iae->getVid());
				if(Success == iae->getAnomalyGuid(pGuid))
				{
					wprintf(L"    Anomaly GUID: %s \n", pGuid->GetStringW());
				}
				else
					wprintf(L"    Anomaly GUID:  \n");
					}
			}
		else
			wprintf(L"    Type: Invaild \n");
	}

	return true;
}
int TestEngine::GetType(ULONG tempId)
{
	ExclusionId iTempId = (ExclusionId) tempId;
	IExclusion *ie = NULL;

	
	if(Success == m_mgrPtr->getExclusion( iTempId, ie))
	{
		int type = ie->getExclusionType();

		return type;
	}

	return -1;
}
bool TestEngine::ReplaceFS(ULONG tempId, char *strPath, time_t tTime, int iState)
{
	ExclusionId iTempId = (ExclusionId) tempId;
	IExclusion *ie = NULL;
	time_t tNowTime = time(NULL) ;
	if (tTime == 0)
		tNowTime = 0;

	cc::IStringPtr pStrPath;
	

	if(Success == m_mgrPtr->getExclusion( iTempId, ie))
	{
		IFileSystemExclusionQIPtr ifse = ie;
		pStrPath.Attach(ccSym::CStringImpl::CreateStringImpl());
		pStrPath->SetStringW(CA2W(strPath));

		if (ifse->setFSPath(pStrPath)!= Success)
			fprintf(stdout, "error with new path! \n");
		if (ifse->setExpiryStamp(tNowTime+tTime)!= Success)
			fprintf(stdout, "error with time stamp! \n");
		if (ifse->setState(IExclusion::ExclusionState(iState)) !=Success)
			fprintf(stdout, "error states! \n");


		if (Success != m_mgrPtr->replaceExclusion(iTempId,ie))
		{
			fprintf(stdout, "	Fail to replace this exclusion! \n");
			return false;
		}
		else
		{
			fprintf(stdout, "	Replaced exclusion # %u\n", iTempId);
			return true;
		}
	}else
	{
		return false;
	}
}
bool TestEngine::ReplaceA(ULONG tempId, ULONG ulVid, char* cGuid, time_t tTime, char* cName, int iState)
{
	ExclusionId iTempId = (ExclusionId) tempId;
	IExclusion *ie = NULL;

	time_t tNowTime = time(NULL) ;
	if (tTime == 0)
		tNowTime = 0;
	cc::IStringPtr pStrGuid;
	cc::IStringPtr pDisplayName;
	cc::IStringPtr pStrName;

	if(Success == m_mgrPtr->getExclusion( iTempId, ie))
	{
		fprintf(stdout, "Replacing Anomaly Exclusion \n");
		IAnomalyExclusionQIPtr ifa = ie;

		if (Success!=ifa->setVid(ulVid))
			fprintf(stdout, "	error with VID! \n");

		CString strGuid(cGuid);
		pStrGuid.Attach(ccSym::CStringImpl::CreateStringImpl());	
		if (!strGuid.Compare(""))
			pStrGuid=NULL;
		else
			pStrGuid->SetStringW(CA2W(strGuid));
		if (Success!=ifa->setAnomalyGuid(pStrGuid))
			fprintf(stdout, "	error with GUID! \n");

		if (tTime == 0)
			tNowTime = 0;

		if (ifa->setExpiryStamp(tNowTime+tTime)!= Success)
			fprintf(stdout, "	error with time stamp! \n");

		CString strName(cName);
		pStrName.Attach(ccSym::CStringImpl::CreateStringImpl());
		pStrName->SetStringW(CA2W(strName));

		if (Success !=ifa->setDisplayName(pStrName))
			fprintf(stdout, "	error with Display Name! \n");

		if (ifa->setState(IExclusion::ExclusionState(iState)) !=Success)
			fprintf(stdout, "	error states! \n");


		if (Success != m_mgrPtr->replaceExclusion(iTempId,ie))
		{
			fprintf(stdout, "	Fail to replace this exclusion! \n");
			return false;
		}
		else
		{
			fprintf(stdout, "	Replaced exclusion # %u\n", iTempId);
			return true;
		}
	}
	else
	{
		return false;
	}
}
bool TestEngine::Reload()
{
	if(Success == m_mgrPtr->reloadState())
	{
		printf("	Reloaded states... \n");
		return true;
	}
	else
	{
		printf("	Fail to reload states! \n");
		return false;
	}
}
bool TestEngine::Save()
{
	if(Success == m_mgrPtr->saveState())
	{
		printf("	Saved states... \n");
		return true;
	}
	else
	{
		printf("	Fail to save states! \n");
		return false;
	}
}
bool TestEngine::ToFile(char* cFilePath)
{
	cc::IStringPtr pStrFilePath;

	CString strFilePath(cFilePath);

	pStrFilePath.Attach(ccSym::CStringImpl::CreateStringImpl());
	if (!strFilePath.Compare(_T("")))
		pStrFilePath=NULL;
	else
		pStrFilePath->SetStringW(CA2W(strFilePath));

	if (Success == m_mgrPtr->saveToFile(pStrFilePath))
	{
		fprintf(stdout, "	Saved into file %s \n", cFilePath);
		return true;
	}
	else
	{
		fprintf(stdout, "	Fail to save into file %s !\n", cFilePath);
		return false;
	}
}
bool TestEngine::FromFile(char* cFilePath, int iState, int iType)
{
	CString strFilePath(cFilePath);

	CExclusionFileLoader fLoader;

	if (Success != fLoader.initialize(strFilePath))
	{
		fprintf(stdout, "	Fail to initialize saved file! \n");
		return false;
	}

	cc::IKeyValueCollectionPtr pKeyValue = NULL;

	if (Success == fLoader.LoadAndFilter(IExclusion::ExclusionState(iState), (IExclusion::ExclusionType) iType))
	{
		fLoader.uninitialize();
		fprintf(stdout, "	Successfully loaded from file %s \n",strFilePath);
		return true;
	}
	else
	{
		fLoader.uninitialize();
		fprintf(stdout, "	Fail to loaded from file %s \n!", strFilePath);
		return false;
	}
}
bool TestEngine::SetAuto(bool isAuto)
{
	if (m_mgrPtr->setAutoCommit(isAuto) == Success)
	{
		fprintf(stdout, "	AutoCommit is set to %d \n", isAuto );
		return true;
	}
	else
	{
		fprintf(stdout, "	Fail to set AutoCommit to %d \n" , isAuto);
		return false;
	}
}
bool TestEngine::IsAuto()
{
	if (m_mgrPtr->isAutoCommit())
	{
		printf("	AutoCommit is ON \n");
		return true;
	}
	else
	{
		printf("	AutoCommit is OFF \n");
		return true;
	}
}
bool TestEngine::SetStateFlag(ULONG tempId, int iState, bool bFlag)
{
	ExclusionId iTempId = (ExclusionId) tempId;
	IExclusion *ie = NULL;

	if(Success == m_mgrPtr->getExclusion( iTempId, ie))
	{
		if (Success == ie->setStateFlag((IExclusion::ExclusionState)iState, bFlag))
		{
			if (Success == m_mgrPtr->replaceExclusion(iTempId,ie))
			{			
				fprintf(stdout, "	Successfuly updated state!\n");
				return true;	

			}else
			{
				fprintf(stdout, "	Fail to update state!\n");
				return false;
			}

		}else
		{
			fprintf(stdout, "	Fail to update state!\n");
			return false;
		}
				
	}else{
			
		fprintf(stdout, "	Can not find this exclusion with id: %u\n", iTempId);
		return false;
	}
}
bool TestEngine::Default(int iState, int iType)
{

	if (Success == m_mgrPtr->resetToDefaults((IExclusion::ExclusionState)iState, (IExclusion::ExclusionType)iType))
	{
		fprintf(stdout, "	Successfully loaded default settings! \n");
		return true;
	}
	else
	{
		fprintf(stdout, "	Fail to loaded default settings! \n");
		return false;
	}
}
bool TestEngine::ViewSettings()
{
	cc::IKeyValueCollectionPtr kvc;
	
	SYMRESULT symRes;

	BOOL bActive = cSetManHelper.IsSettingsManagerActive();

	if ( bActive != TRUE )
	{
		ccLib::CEvent eventNothing;
		ccLib::CMessageLock msgLock (TRUE, TRUE);

		for ( DWORD dwWaitLoop = 0; 
				  dwWaitLoop < 180000; 
				  dwWaitLoop += 1000 )
		{
			eventNothing.Create ( NULL, FALSE, FALSE, NULL, FALSE);
			msgLock.Lock ( eventNothing, 1000 );

			if (ccSettings::CSettingsManagerHelper::IsSettingsManagerActive())
				break;
		}

		if(!ccSettings::CSettingsManagerHelper::IsSettingsManagerActive())
		{
			printf("ERROR: Could not locate CC Settings Manager.\n");
			return false;
		}
	
	}

	symRes = cSetManHelper.Create(pSetMgr.m_p);
	if (SYM_FAILED(symRes) || pSetMgr == NULL)
	{
		printf("CExclusionSettingsManager::SettingsReady() : "\
		"ccSettings.Create() != SYM_OK, 0x%08X\n", symRes);
		return false;
	}

	symRes = pSetMgr->CreateSettings(L"Norton AntiVirus\\Exclusions", &(pSettings));
	if (SYM_FAILED(symRes))
	{
		printf("CExclusionSettingsManager::Load() : "\
		"pSettingsManager->GetSettings() != SYM_OK, 0x%08X\n", 
		symRes);
		return false;
	}

	if(SYM_FAILED(pSettings->EnumValues(&(pEnumSettings.m_p))))
	{
		return false;
	}

	dwEnumCurrent = 0;
	while(enumLoadExclusionData(kvc))
	{
		if(kvc)
		{
			//Print stuff
			printMyStuff(kvc);
			kvc.Release();
		}
	}

	dwEnumCurrent = 0;
	pEnumSettings.Release();
	pSettings.Release();
	pSetMgr.Release();
	return true;
}
bool enumLoadExclusionData(cc::IKeyValueCollection* &pData)
{
	
    DWORD dwCount = 0;

    if(pData != NULL)
    {
        printf("CExclusionSettingsManager::enumLoadExclusionData - "\
            "Received a non-NULL KVC object. Autoreleasing.");
        pData->Release();
        pData = NULL;
    }

    if(pEnumSettings == NULL)
    { return false; }

    if(SYM_FAILED(pEnumSettings->GetCount(dwCount)))
    { return false; }

    if(dwEnumCurrent >= dwCount)
    { return false; }

    CStringW strBuffer;
    DWORD dwSize, dwType, dwActualSize;
    SYMRESULT sr;
    dwSize = dwType = dwActualSize = 0;
    sr = pEnumSettings->GetItem(dwEnumCurrent, (LPWSTR)NULL, 
        dwSize, dwType);
    if(SYM_FAILED(sr) && sr != SYM_SETTINGS_ERROR_BUFFER_TOO_SMALL)
    { return false; }

    if(dwSize <= 0)
    {
        dwEnumCurrent++;
        return true;
    }

    dwSize += 1*sizeof(wchar_t); // Add 1 for null char.
    if(SYM_FAILED(pEnumSettings->GetItem(dwEnumCurrent,
        strBuffer.GetBuffer((int)dwSize),
        dwSize, dwType)))
    {
        strBuffer.ReleaseBuffer();
        return false;
    }
    strBuffer.ReleaseBuffer();

    dwSize = 0;
    sr = pSettings->GetBinary(strBuffer.GetString(), NULL, dwSize);
    if(SYM_FAILED(sr) && sr != SYM_SETTINGS_ERROR_BUFFER_TOO_SMALL)
    { return false; }

    if(dwSize <= 0)
    {
        dwEnumCurrent++;
        return false;
    }

    ccSym::CMemoryStreamImplPtr pMemStream;
    pMemStream.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());

    if(pMemStream == NULL)
    {
        return false;
    }

    if(pMemStream->GetMemory().NewAlloc(dwSize) == NULL)
    {
        return false;
    }

    if(SYM_FAILED(pSettings->GetBinary(strBuffer.GetString(), 
        (LPVOID)pMemStream->GetMemory(), 
        dwSize)))
    {
        return false;
    }

    // Create the collection
    pData = ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl();
    if( !pData )
    {
        return false;
    }
    pData->AddRef();

    cc::ISerializeQIPtr pSerialized = pData;
    if( !pSerialized )
    {
        pData->Release();
        pData = NULL;
        return false;
    }

    // Load up the data
    if( !pSerialized->Load(pMemStream) )
    {
        pData->Release();
        pData = NULL;
        return false;
    }

    dwEnumCurrent++;
    return true;
}
void printMyStuff(cc::IKeyValueCollection* &pData)
{
    ULONG ul;
    cc::IStringPtr sp;

    if(pData->GetValue(IExclusion::Property::EGUID, (ISymBase*&)sp))
    {
        wprintf(L"Exclusion %s\n", sp->GetStringW());
        sp.Release();
    }

    if(pData->GetValue(IExclusion::Property::EID, ul))
    {
        printf("  Exclusion ID: %d\n", ul);
    }

    if(pData->GetValue(IExclusion::Property::EThreatName, (ISymBase*&)sp))
    {
        wprintf(L"  Threat Name: %s\n", sp->GetStringW());
        sp.Release();
    }

    if(pData->GetValue(IExclusion::Property::EExpiryStamp, ul))
    {
        printf("  Expiry Stamp: %d\n", ul);
    }

    if(pData->GetValue(IExclusion::Property::EFlags, ul))
    {
        printf("  Flags: %d\n", ul);
    }

    if(pData->GetValue(IExclusion::Property::EState, ul))
    {
        printf("  State: %d\n", ul);
    }
    
    if(pData->GetValue(IExclusion::Property::EType, ul))
    {
        if(ul == IExclusion::ExclusionType::INVALID)
            printf("  Type: INVALID\n");
        else if(ul == IExclusion::ExclusionType::ANOMALY)
        {
            printf("  Type: Anomaly\n");
            if(pData->GetValue(IAnomalyExclusion::Property::Vid, ul))
            {
                printf("  VID: %d\n", ul);
            }
            if(pData->GetValue(IAnomalyExclusion::Property::AnomalyGuid, (ISymBase*&)sp))
            {
                wprintf(L"  Anomaly GUID: %s\n", sp->GetStringW());
            }
        }
        else if(ul == IExclusion::ExclusionType::FILESYSTEM)
        {
            printf("  Type: FileSystem\n");
            if(pData->GetValue(IFileSystemExclusion::Property::FSPath, (ISymBase*&)sp))
            {
                wprintf(L"  Path: %s\n", sp->GetStringW());
            }
        }
    }

}

bool TestEngine::ReadFromFile(CString strInputFile)
{

	FILE * pFile;
	CString strTemp;

	pFile = fopen(strInputFile, "r");
	
	if (pFile == NULL)
	{
		fprintf( stdout, "can not open input file! \n");
		return false;
	}else{
		
		while (1){
			char charTemp = (char) fgetc(pFile);
		
			if (feof(pFile))
			{
				m_strVector.push_back(strTemp);
				strTemp = "";
				m_vVector.push_back(m_strVector);
				m_strVector.clear();
				break;
			}

			if (charTemp != ' ' && charTemp != '\0' && charTemp != '\n')
			{
				strTemp.AppendChar(charTemp);

			}else if (charTemp == ' ')
			{
			   m_strVector.push_back(strTemp);
			   strTemp = "";
			}
			else if (charTemp == '\n')
			{
				m_strVector.push_back(strTemp);
				strTemp = "";
				m_vVector.push_back(m_strVector);
				m_strVector.clear();
			}
		}

		fclose (pFile);

		return true;
	}

}
bool TestEngine::Process()
{
	char* pEnd;
	//char* cFilePath;
	int k = m_vVector.size();
	
	for (int i=0; i<m_vVector.size(); i++)
	{
		m_strVector = m_vVector.at(i);
		fprintf(stdout, "Executing line #%d ...", i+1);
		for (int k=0; k<m_strVector.size(); k++)
		{
			fprintf(stdout, " %s ", m_strVector.at(k));
		}
		fprintf(stdout, "\n");

		if (!m_strVector.at(0).Compare("addfs"))
		{
			
			if ( m_strVector.size() == 5){
				if (Addfs(m_strVector.at(1).GetBuffer(), strtoul(m_strVector.at(2), &pEnd, 0), atoi(m_strVector.at(3)), atoi(m_strVector.at(4))))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
			
		}
		if (!m_strVector.at(0).Compare("adda"))
		{
			if (m_strVector.size() == 6){
				if(Adda(strtoul(m_strVector.at(1),&pEnd,0),m_strVector.at(2).GetBuffer(),strtoul(m_strVector.at(3),&pEnd,0),m_strVector.at(4).GetBuffer(), atoi(m_strVector.at(5))))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("findfs"))
		{
			if(m_strVector.size() == 3){
				if(Findfs(m_strVector.at(1).GetBuffer(), atoi(m_strVector.at(2))))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("finda"))
		{
			if(m_strVector.size() == 3){
				if(Finda(strtoul(m_strVector.at(1), &pEnd, 0), m_strVector.at(2).GetBuffer()))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("get"))
		{
			if(m_strVector.size() == 2){
				if(Get(strtoul(m_strVector.at(1), &pEnd, 0)))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("isfs"))
		{
			if(m_strVector.size() == 4){
				if(Isfs(m_strVector.at(1).GetBuffer(),atoi(m_strVector.at(2)), atoi(m_strVector.at(3))))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("isa"))
		{
			if(m_strVector.size() == 4){
				if(Isa(strtoul(m_strVector.at(1),&pEnd,0), m_strVector.at(2).GetBuffer() , atoi(m_strVector.at(3))))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		CString temp = m_strVector.at(0);
		if (!m_strVector.at(0).Compare("list"))
		{
			if(m_strVector.size() == 1){
				if(List())
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("remove"))
		{
			if(m_strVector.size() == 2){
				if(Remove(strtoul(m_strVector.at(1), &pEnd, 0)))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("replace"))
		{
			if (GetType(strtoul(m_strVector.at(1),&pEnd,0)) == 1)//file system
			{
				if(m_strVector.size() == 5)
				{
					if(ReplaceFS(strtoul(m_strVector.at(1),&pEnd,0), m_strVector.at(2).GetBuffer(),strtoul(m_strVector.at(3),&pEnd,0),atoi(m_strVector.at(4))))
						fprintf(stdout, "	Done executing line #%d \n\n", i+1);
					else
						fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
				}else{
					fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
				}
			}else if(GetType(strtoul(m_strVector.at(1),&pEnd,0)) == 2) //anonmaly
			{
				if(m_strVector.size() == 7)
				{
					if(ReplaceA(strtoul(m_strVector.at(1),&pEnd,0),strtoul(m_strVector.at(2),&pEnd,0),m_strVector.at(3).GetBuffer(),strtoul(m_strVector.at(4),&pEnd,0),m_strVector.at(5).GetBuffer(),atoi(m_strVector.at(6))))
						fprintf(stdout, "	Done executing line #%d \n\n", i+1);
					else
						fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
				}else{
					fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
				}
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}

		}
		if (!m_strVector.at(0).Compare("reload"))
		{
			if(m_strVector.size() == 1)
			{
				if(Reload())
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("save"))
		{
			if(m_strVector.size() == 1)
			{
				if(Save())
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("stateflag"))
		{
			if(m_strVector.size() == 4)
			{
				if(SetStateFlag(strtoul(m_strVector.at(1),&pEnd,0),atoi(m_strVector.at(2)), atoi(m_strVector.at(3))))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("tofile"))
		{
			if(m_strVector.size() == 2)
			{
				if(ToFile((m_strVector.at(1).GetBuffer())))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("fromfile"))
		{
			if(m_strVector.size() == 4)
			{
				if(FromFile(m_strVector.at(1).GetBuffer(),atoi(m_strVector.at(2)),atoi(m_strVector.at(3))))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("default"))
		{
			if(m_strVector.size() == 3)
			{
                if(Default(atoi(m_strVector.at(1)),atoi(m_strVector.at(2))))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("setauto+"))
		{
			if(m_strVector.size()==1)
			{
                if(SetAuto(true))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("setauto-"))
		{
			if(m_strVector.size()==1)
			{
                if(SetAuto(false))
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("isauto"))
		{
			if(m_strVector.size()==1)
			{
                if(IsAuto())
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
		if (!m_strVector.at(0).Compare("viewsettings"))
		{
			if(m_strVector.size()==1)
			{
                if(ViewSettings())
					fprintf(stdout, "	Done executing line #%d \n\n", i+1);
				else
					fprintf(stdout, "	Failed on line #%d of the input file \n\n" , i+1);
	
			}else{
				fprintf(stdout, "	Error(s) on line #%d of the input file \n\n" , i+1);
			}
		}
	}

	return true;
}

SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()