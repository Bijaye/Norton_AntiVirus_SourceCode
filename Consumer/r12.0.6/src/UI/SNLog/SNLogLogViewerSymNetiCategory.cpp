#include "StdAfx.h"

#include "resource.h"
#include "..\SNLogRes\ResResource.h"

#include "ccEventId.h"
#include "ccLogViewerPluginId.h"
#include "NisEvtNisRes.h"
#include "winsock.h"

// IAM.h uses #ifndef HMODULE and SDK uses typedef
// Define HMODULE to prevent headers from redefining 
// standard Windows types.
// #define HMODULE
#include "rdefs.h"
#include "SymNetDriverAPI.h"
#include "ccSerializableEventEx.h"
#undef HMODULE

#include "ccOSInfo.h"
#include "nisumutils.h"
#include "SymNetDrvEventHelper.h"
#include "SNLogLogViewerSymNetiCategory.h"
#include "SNLogLogViewerSymNetiEvent.h"
#include "SymNetAPIVer.h"
#include "ccSymCommonClientInfo.h"
#include "ISSharedLVPluginId.h"
#include "ccSymModuleLifetimeMgrHelper.h"   // GETMODULEMGR
#include "ccResourceLoader.h"

#include "IWPPrivateSettingsInterface.h"
#include "IWPSettingsInterface.h"
#include "IWPPrivateSettingsLoader.h"
#include "IWPSettingsLoader.h"

#include "NAVHelpLauncher.h"    // NAVToolbox

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

#ifndef DEBUG_NEW
    #include <new>
    #include <new.h>
    // Use debug version of operator new if _DEBUG
    #ifdef _DEBUG
        #include <crtdbg.h>
        #define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
    #else // _DEBUG
        #define DEBUG_NEW new
    #endif // _DEBUG
#endif // DEBUG_NEW

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

//THIS SHOULD BE THE LAST INCLUDE
#include "ccCatch.h"

using namespace ccEvtMgr;
using namespace ccLib;
using namespace NisEvt;
using namespace NISUMUtils;

const LPCTSTR CLogViewerSymNetiCategory::m_szSymNetiDLL = _T("SymNeti.dll");
const LPCSTR CLogViewerSymNetiCategory::m_szSymNetiCreateFactory = "symneti_CreateEventFactory";
const LPCSTR CLogViewerSymNetiCategory::m_szSymNetiDeleteFactory = "symneti_DeleteEventFactory";

BOOL CLogViewerSymNetiCategory::m_bInit = FALSE;
BOOL CLogViewerSymNetiCategory::m_bUserColumn = FALSE;
DWORD CLogViewerSymNetiCategory::m_dwUserLevel = UT_RESTRICTED;
DWORD CLogViewerSymNetiCategory::m_dwProductType = IS_PRODUCT_NIS;


// Defined here to prevent include warning problems
static SNCALLTABLE m_SymNetiCallTable = {0};

CLogViewerSymNetiCategory::CLogViewerSymNetiCategory(long nEventType,
                                                     long nCategoryId,
													 long nHelpId)
{
    m_hSymNeti = NULL;
    m_hImageList = NULL;
    m_nCategoryId = nCategoryId;
	m_nHelpId = nHelpId;
    m_nEventType = nEventType;
	m_uImageListId = IDB_EVENTICONS ;
}

CLogViewerSymNetiCategory::~CLogViewerSymNetiCategory()
{
    Destroy();
}

BOOL CLogViewerSymNetiCategory::IsIWPFirewallSettingEnabled()
{
	// assume firewall is on
	BOOL bFirewallOn = TRUE;

	// initialize COM on this thread because CC Settings needs it and IWP uses CC Settings
	HRESULT hrCoInit = CoInitialize(NULL);
	if(FAILED(hrCoInit))
	{
		return FALSE;
	}

	try
	{
		IWP::IIWPPrivateSettingsPtr spIWPPrivateSettings;

		// load IWPPrivateSettings
        if(SYM_SUCCEEDED(IWP_IWPPrivateSettings::CreateObject(GETMODULEMGR(), spIWPPrivateSettings.m_p)) && spIWPPrivateSettings != NULL)
		{
			// check if the settings show that a legacy product used to be installed.
			DWORD dwFirewallOn = 0;
			if(SYM_FAILED(spIWPPrivateSettings->GetValue(spIWPPrivateSettings->IWPFirewallOn, dwFirewallOn)))
			{
				// if we can't determine whether a legacy client was installed or not, we assume is was not for safety.
				CCTRACEE(_T("CInstallWatcher::CInstallWatcher() - Could not determine if the firewall is on. Assuming it is.\n"));
			}				

			bFirewallOn = (dwFirewallOn != 0);
		}
	}
	catch(...)
	{
		CCTRACEE(_T("CInstallWatcher::CInstallWatcher() - Unknown Exception occurred."));
	}

	//  match up a CoUninitialize() with a CoInitialize()
	if(SUCCEEDED(hrCoInit))
		CoUninitialize();

	return bFirewallOn;
}

BOOL CLogViewerSymNetiCategory::VerifyShouldCreate()
{
	BOOL bReturn = TRUE;
	
	switch(m_nCategoryId)
	{ 
		case CC_NIS_CAT_CONNECTIONS_ID:
		case CC_NIS_CAT_FIREWALL_ID:
		case CC_NIS_CAT_SYSTEM_ID:
			bReturn = IsIWPFirewallSettingEnabled();
			break;
		
		default:
			bReturn = TRUE;
			break;
	}

	return bReturn;
}

BOOL CLogViewerSymNetiCategory::Create()
{
    CCTRACEI(_T("CLogViewerSymNetiCategory::Create()"));

	Destroy();

	if(!VerifyShouldCreate())
	{
		return FALSE;
	}
    
	// Init only once for all instances
    if (m_bInit == FALSE)
    {
		ccLib::COSInfo OSInfo;
		
		m_bUserColumn = FALSE;
		m_dwUserLevel = OSInfo.IsAdministrator() ? UT_SUPERVISOR : UT_NORMAL;
		m_dwProductType = 0;

        m_bInit = TRUE;
    }

    BOOL bRet = TRUE;
    for (;;)
    {
        // Get system folder
        TCHAR szPath[_MAX_PATH];
        if (GetSystemDirectory(szPath, CCDIMOF(szPath)) == 0)
        {
            CCTRACEW(_T("GetSystemDirectory failed"));

            // Try the local path
            szPath[0] = _T('\0');
        }
        else
        {
            // Add trailing "\"
            if (szPath[lstrlen(szPath) - 1] != _T('\\'))
            {
                lstrcat(szPath, _T("\\"));
            }
        }

        // Add "SymNeti.dll"
        lstrcat(szPath, m_szSymNetiDLL);
		TCHAR* szEvtStringArray[] = {szPath};

        // Load SymNeti DLL
        m_hSymNeti = LoadLibrary(szPath);
        if (m_hSymNeti == NULL)
        {
            CCTRACEE(_T("Fail to load %s"), szPath);
            bRet = FALSE;
            break;
        }

		BOOL bRet = TRUE;

   		if(m_EventFactoryCreator.Create(szEvtStringArray, 1) == FALSE)
		{
			//Error Factory not created
			bRet = FALSE;
			return bRet;
			
		}

		// Fill in the SymNeti call table
		m_SymNetiCallTable.pSNInit = (SNINIT) GetProcAddress(m_hSymNeti, (LPCSTR)eSNInit);
		m_SymNetiCallTable.pSNUnInit = (SNUNINIT) GetProcAddress(m_hSymNeti, (LPCSTR)eSNUnInit);
		m_SymNetiCallTable.pSNAddressToName = (SNADDTONAME) GetProcAddress(m_hSymNeti, (LPCSTR)eSNAddressToName);
		m_SymNetiCallTable.pSNUnlockIDSConfig = (SNUNLOCKIDSCONFIG) GetProcAddress(m_hSymNeti, (LPCSTR)eSNUnlockIDSConfig);
		m_SymNetiCallTable.pSNLockIDSConfig = (SNLOCKIDSCONFIG) GetProcAddress(m_hSymNeti, (LPCSTR)eSNLockIDSConfig);
		m_SymNetiCallTable.pSNGetIDSSignature = (SNGETIDSSIGNATURE) GetProcAddress(m_hSymNeti, (LPCSTR)eSNGetIDSSignature);

		ASSERT(m_SymNetiCallTable.pSNGetIDSSignature);

		if (m_SymNetiCallTable.pSNInit)
		{
			DWORD dwIgnore=0;
			m_SymNetiCallTable.pSNInit(MAKELONG(VER_SN_MINORNUM, VER_SN_MAJORNUM), &dwIgnore);
		}


        // Load image list
        if (m_uImageListId != 0)
        {
            m_hImageList = ImageList_LoadBitmap(g_hInst, 
                                                MAKEINTRESOURCE(m_uImageListId), 
                                                16, 
                                                0, 
                                                RGB(255,0,255));
            if (m_hImageList == NULL)
            {
                CCTRACEE(_T("Fail to load bitmap"));
                bRet = FALSE;
                break;
            }
        }

        break;
    }

    if (bRet == FALSE)
    {
        Destroy();
        return FALSE;
    }
    return CLogViewerCategoryHelper::Create(&m_EventFactoryCreator, m_nEventType);
}

void CLogViewerSymNetiCategory::Destroy()
{
    CLogViewerCategoryHelper::Destroy();

    // Destroy image list
    if (m_hImageList != NULL)
    {
		ImageList_Destroy(m_hImageList);
        m_hImageList = NULL;
    }
    
    // Delete event factory
	m_EventFactoryCreator.Destroy();    // Free library

    if (m_hSymNeti != NULL)
    {
		if (m_SymNetiCallTable.pSNUnInit)
			m_SymNetiCallTable.pSNUnInit();

        FreeLibrary(m_hSymNeti);
        m_hSymNeti = NULL;
    }
}

HRESULT CLogViewerSymNetiCategory::LaunchHelp()
{
	// See SNLogViewerSymNetiCategory.h for help Id's
	CCTRACEI("LaunchHelp: %d, Category: %d", m_nHelpId, m_nCategoryId);

	NAVToolbox::CNAVHelpLauncher Help;
	if (Help.LaunchHelp(m_nHelpId) == NULL)
	{
		CCTRACEE("Fail to LaunchHelp: %d. Try NAV Help", m_nHelpId);
		if (Help.LaunchHelp(0) != NULL)
		{
			CCTRACEE("Fail to NAV Help");
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLogViewerSymNetiCategory::GetImageList(HIMAGELIST& hImageList)
{
    hImageList = m_hImageList;
    return S_OK;
}


#define SN_MAX_COLUMNS 9
enum
{
	EV_STRING,
	EV_DWORD,
	EV_CY
};

typedef struct tagLOGTYPELOOKUP
{
	DWORD dwSNType;
	DWORD dwColumnCount;
	struct 
	{
		DWORD dwColumnNameID;		// Column heading string ID
		DWORD dwColumnDataPosition;	// Position, within log event data list for this column
		DWORD dwVTDataType;			// How to represent the data
	} aColumnInfo[SN_MAX_COLUMNS];
} SNLOGTYPELOOKUP, *PSNLOGTYPELOOKUP; 

static const SNLOGTYPELOOKUP aLogTypeLookupTable[] = 
{
	{CC_SYMNETDRV_EVENT_LOG_SYSTEM,		3,     {{IDS_COLTEXT_DATE, 0, EV_STRING},
												{IDS_COLTEXT_USER, 0, EV_STRING},
												{IDS_COLTEXT_MESSAGE, -1, EV_STRING}}},
	{CC_SYMNETDRV_EVENT_LOG_FIREWALL,	3,     {{IDS_COLTEXT_DATE, 0, EV_STRING},
												{IDS_COLTEXT_USER, 0, EV_STRING},
												{IDS_COLTEXT_MESSAGE, 0, EV_STRING}}},
	{CC_SYMNETDRV_EVENT_LOG_CONNECTIONS,	9, {{IDS_COLTEXT_DATE, 0, EV_STRING},
												{IDS_COLTEXT_USER, 0, EV_STRING},
												{IDS_COLTEXT_LOCAL_IP, 3, EV_STRING},
												{IDS_COLTEXT_LOCAL_PORT, 4, EV_STRING},
												{IDS_COLTEXT_REMOTE_IP,   0, EV_STRING},
												{IDS_COLTEXT_REMOTE_PORT,   1, EV_STRING},
												{IDS_COLTEXT_SENT, 5, EV_DWORD},
												{IDS_COLTEXT_RECV, 6, EV_DWORD},
												{IDS_COLTEXT_CONNTIME, 7, EV_CY}}},
	{CC_SYMNETDRV_EVENT_LOG_ALERTS,		3,	   {{IDS_COLTEXT_DATE, 0, EV_STRING},
												{IDS_COLTEXT_USER, 0, EV_STRING},
												{IDS_COLTEXT_MESSAGE, -1, EV_STRING}}},
	{CC_SYMNETDRV_EVENT_LOG_IDS,			3, {{IDS_COLTEXT_DATE, 0, EV_STRING},
												{IDS_COLTEXT_USER, 0, EV_STRING},
												{IDS_COLTEXT_MESSAGE}, -1, EV_STRING}},
	{CC_SYMNETDRV_EVENT_LOG_DEBUG,		2,     {{IDS_COLTEXT_DATE, 0, EV_STRING},
												{IDS_COLTEXT_USER, 0, EV_STRING}}},
};


HRESULT CLogViewerSymNetiCategory::GetColumnCount(int& iColumnCount)
{
    iColumnCount = 0;

	for (int i=0; i < CCDIMOF(aLogTypeLookupTable); i++)
		if (m_nEventType == aLogTypeLookupTable[i].dwSNType)
		{
			iColumnCount = aLogTypeLookupTable[i].dwColumnCount;
			if (!m_bUserColumn)
				(iColumnCount)--;
			return S_OK;
		}

	return E_INVALIDARG;
}

HRESULT CLogViewerSymNetiCategory::GetCategoryID(int& iCategoryID)
{
    iCategoryID = m_nCategoryId;
    return S_OK;
}

HRESULT CLogViewerSymNetiCategory::GetCategoryName(HMODULE& hModule, 
                                                   UINT& nId)
{
    // Load category names from this DLL
    nId = 0;
	hModule = g_ResLoader.GetResourceInstance();
    switch (m_nCategoryId)
    {
    case CC_NIS_CAT_SYSTEM_ID:
        nId = IDS_SYSTEM_CATEGORY;
        break;
    case CC_NIS_CAT_FIREWALL_ID:
        nId = IDS_FIREWALL_CATEGORY;
        break;
    case CC_NIS_CAT_CONNECTIONS_ID:
        nId = IDS_CONNECTIONS_CATEGORY;
        break;
    case CC_NIS_CAT_ALERTS_ID:
        nId = IDS_ALERTS_CATEGORY;
        break;
    case CC_NIS_CAT_INTRUSIONDETECTION_ID:
        nId = IDS_INTRUSIONDETECTION_CATEGORY;
        break;
    case CC_NIS_CAT_DEBUG_ID:
        nId = IDS_DEBUG_CATEGORY;
        break;
    default:
        ASSERT(FALSE);
        return E_FAIL;
    }

    return S_OK;;
}

HRESULT CLogViewerSymNetiCategory::GetCategoryDescription(HMODULE& hModule, 
                                                          UINT& nId)
{
    // Load category descriptions from this DLL
    nId = 0;
    hModule = g_ResLoader.GetResourceInstance();
    switch (m_nCategoryId)
    {
    case CC_NIS_CAT_SYSTEM_ID:
        nId = IDS_SYSTEM_CATEGORY_DESCRIPTION;
        break;
    case CC_NIS_CAT_FIREWALL_ID:
        nId = IDS_FIREWALL_CATEGORY_DESCRIPTION;
        break;
    case CC_NIS_CAT_CONNECTIONS_ID:
        nId = IDS_CONNECTIONS_CATEGORY_DESCRIPTION;
        break;
    case CC_NIS_CAT_ALERTS_ID:
        nId = IDS_ALERTS_CATEGORY_DESCRIPTION;
        break;
    case CC_NIS_CAT_INTRUSIONDETECTION_ID:
        nId = IDS_INTRUSIONDETECTION_CATEGORY_DESCRIPTION;
        break;
    case CC_NIS_CAT_DEBUG_ID:
        nId = IDS_DEBUG_CATEGORY_DESCRIPTION;
        break;
    default:
        ASSERT(FALSE);
        return E_FAIL;
    }

    return S_OK;;
}

HRESULT CLogViewerSymNetiCategory::GetColumnName(int iColumn, 
                                                 HMODULE& hModule, 
                                                 UINT& nId)
{
	for (int i=0; i < CCDIMOF(aLogTypeLookupTable); i++)
		if (m_nEventType == aLogTypeLookupTable[i].dwSNType)
		{
			if (!m_bUserColumn && iColumn > 0)
				iColumn++;

			if (iColumn >= aLogTypeLookupTable[i].dwColumnCount)
				return E_INVALIDARG;

			nId = aLogTypeLookupTable[i].aColumnInfo[iColumn].dwColumnNameID;
			hModule = g_ResLoader.GetResourceInstance();
			return S_OK;
		}

	return E_FAIL;
}

HRESULT CLogViewerSymNetiCategory::GetEventCount(int& iEventCount)
{
    // Validate user rights

	// Allow all user types of SCF to view the events.
	if (m_dwProductType != IS_PRODUCT_SCF)
	{
		if (m_dwUserLevel != UT_SUPERVISOR &&
			m_dwUserLevel != UT_NORMAL)
		{
			return E_ACCESSDENIED;
		}
	}

    return CLogViewerCategoryHelper::GetEventCount(iEventCount);
}

HRESULT CLogViewerSymNetiCategory::SetLogFileSize(int iLogFileSize)
{
    // Validate user rights
    if (m_dwUserLevel != UT_SUPERVISOR)
    {
        return E_ACCESSDENIED;
    }

    return CLogViewerCategoryHelper::SetLogFileSize(iLogFileSize);
}

HRESULT CLogViewerSymNetiCategory::SetLoggingState(BOOL bEnabled)
{
    // Validate user rights
    if (m_dwUserLevel != UT_SUPERVISOR)
    {
        return E_ACCESSDENIED;
    }

    return CLogViewerCategoryHelper::SetLoggingState(bEnabled);
}

HRESULT CLogViewerSymNetiCategory::ClearCategory()
{
    // Validate user rights
    if (m_dwUserLevel != UT_SUPERVISOR)
    {
        return E_ACCESSDENIED;
    }

    return CLogViewerCategoryHelper::ClearCategory();
}

HRESULT CLogViewerSymNetiCategory::CopyEvent(const CEventEx& Event, 
                                             const CLogFactoryEx::EventInfo& Info,
                                             cc::ILogViewerEvent*& pLogViewerEvent)
{
    pLogViewerEvent = NULL;

    if (m_pEventFactory == NULL)
    {
        ASSERT(FALSE);
        return E_FAIL;
    }

    // Make a copy of the event
    CEventEx* pEvent = NULL;
    if (m_pEventFactory->CopyEvent(Event, pEvent) != CError::eNoError)
    {
		CCTRACEE(_T("CLogViewerSymNetiCategory::CopyEvent() : EventFactory->CopyEvent failed"));
        return E_FAIL;
    }

    // Wrap the event
    if (WrapEvent(pEvent, pLogViewerEvent) == FALSE)
    {
        CCTRACEE(_T("CLogViewerSymNetiCategory::CopyEvent() : WrapEvent() failed"));
        m_pEventFactory->DeleteEvent(pEvent);
        pEvent = NULL;
        return E_FAIL;
    }

    return S_OK;
}

BOOL CLogViewerSymNetiCategory::WrapEvent(ccEvtMgr::CEventEx* pEvent, 
                                          cc::ILogViewerEvent*& pLogViewerEvent)
{
    pLogViewerEvent = NULL;

    // Allocate the proxy event
    CLogViewerSymNetiEvent* pLogViewerSymNetiEvent = NULL;
    CExceptionInfo exceptionInfo(_T("CLogViewerSymNetiCategory::WrapEvent()"));
    try
    {
        pLogViewerSymNetiEvent = new CLogViewerSymNetiEvent;
    }
    CCCATCH(exceptionInfo);
    if (exceptionInfo != FALSE)
    {
        return FALSE;
    }

    // Return the log viewer interface
    SYMRESULT hRes = pLogViewerSymNetiEvent->QueryInterface(cc::IID_LogViewerEvent, 
                                                  (void **)&pLogViewerEvent);
    if (SYM_FAILED(hRes) ||
        pLogViewerEvent == NULL)
    {
        CCTRACEE(_T("CLogViewerSymNetiCategory::WrapEvent() : QueryInterface() failed"));
        delete pLogViewerSymNetiEvent;
        pLogViewerSymNetiEvent = NULL;
        pLogViewerEvent = NULL;
        return FALSE;
    }

	ASSERT(m_SymNetiCallTable.pSNAddressToName);

	pLogViewerSymNetiEvent->m_pSymNetiCallTable = &(m_SymNetiCallTable);
	pLogViewerSymNetiEvent->m_pLogEvent = static_cast<SymNeti::CSNLogEventEx *>(pEvent);
	pLogViewerSymNetiEvent->m_pLogEvent->AddRef();
	pLogViewerSymNetiEvent->m_bShowUser = m_bUserColumn;

    return TRUE;
}
