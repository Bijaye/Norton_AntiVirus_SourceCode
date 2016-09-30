////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "resource.h"
#include "..\SNLogRes\ResResource.h"

#include "ccEventId.h"
#include "ccLogViewerPluginId.h"
#include "NisEvtNisRes.h"
#include "winsock.h"

#include "ccSerializableEventEx.h"

#include "ccOSInfo.h"
#include "SNLogLogViewerSymNetiCategory.h"
#include "SNLogLogViewerSymNetiEvent.h"
#include "ccSymCommonClientInfo.h"
#include "ISSharedLVPluginId.h"
#include "ccSymModuleLifetimeMgrHelper.h"   // GETMODULEMGR
#include "ccResourceLoader.h"

// ISDataClient & UI Element related
#include "ISDataClientLoader.h"
#include "uiNISDataElementGuids.h"
#include "uiNumberDataInterface.h"

#include "SymHelpLauncher.h"    // NAVToolbox
#include "ISNames.h"

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

bool ValidateUser ();

BOOL CLogViewerSymNetiCategory::m_bInit = FALSE;
BOOL CLogViewerSymNetiCategory::m_bUserColumn = FALSE;
DWORD CLogViewerSymNetiCategory::m_dwUserLevel = ISUSERTYPE_RESTRICTED;
DWORD CLogViewerSymNetiCategory::m_dwProductType = IS_PRODUCT_NIS;

CLogViewerSymNetiCategory::CLogViewerSymNetiCategory(long nEventType,
                                                     long nCategoryId,
													 long nHelpId)
{
    m_hImageList = NULL;
    m_nCategoryId = nCategoryId;
	m_nHelpId = nHelpId;
    m_nEventType = nEventType;
	m_uImageListId = IDB_EVENTICONS ;
	m_bInit = FALSE;
}

CLogViewerSymNetiCategory::~CLogViewerSymNetiCategory()
{
    Destroy();
}


BOOL CLogViewerSymNetiCategory::Create()
{
    CCTRACEI(_T("CLogViewerSymNetiCategory::Create()"));

	Destroy();


	// Init only once for all instances
    if (m_bInit == FALSE)
    {
		ccLib::COSInfo OSInfo;
		
		m_bUserColumn = FALSE;
		m_dwUserLevel = OSInfo.IsAdministrator() ? ISUSERTYPE_SUPERVISOR : ISUSERTYPE_NORMAL;
		m_dwProductType = 0;

        m_EventFactoryCreator = new CSymMultiEventFactoryHelper;
        if(!m_EventFactoryCreator)
        {
            CCTRACEE(_T(__FUNCTION__) _T(" - Failed to allocate event factory creator"));
            return FALSE;
        }

        m_bInit = TRUE;
    }

    BOOL bRet = TRUE;
    for (;;)
    {
		TCHAR szPath[_MAX_PATH] = {0};
		DWORD dwSize = _MAX_PATH;
		if (SymNetDrv::GetComponentLocation(SymNetDrv::eSymNeti, szPath, &dwSize) != ERROR_SUCCESS)
		{
			bRet = FALSE;
			break;
		}

		TCHAR* szEvtStringArray[] = {szPath};

		BOOL bRet = TRUE;

		CSNDHelper::CreateSNDHelper(SND_CLIENT_NAV, m_sndhelper);

   		if(m_EventFactoryCreator->Create(szEvtStringArray, 1) == FALSE)
		{
			//Error Factory not created
			bRet = FALSE;
			return bRet;
			
		}

        // Load image list
        if (m_uImageListId != 0)
        {
            m_hImageList = ImageList_LoadBitmap(g_ResLoader.GetResourceInstance(), 
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
    return CLogViewerCategoryHelper::Create(m_EventFactoryCreator, m_nEventType);
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
    if(NULL != m_EventFactoryCreator.m_p)
       m_EventFactoryCreator->Destroy();    // Free library
}

HRESULT CLogViewerSymNetiCategory::LaunchHelp()
{
	// See SNLogViewerSymNetiCategory.h for help Id's
	CCTRACEI(_T("LaunchHelp: %d, Category: %d"), m_nHelpId, m_nCategoryId);

	isshared::CHelpLauncher Help;
	Help.LaunchHelp(m_nHelpId);
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
												{IDS_COLTEXT_MESSAGE, static_cast<DWORD>(-1), EV_STRING}}},
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
												{IDS_COLTEXT_MESSAGE, static_cast<DWORD>(-1), EV_STRING}}},
	{CC_SYMNETDRV_EVENT_LOG_IDS,			3, {{IDS_COLTEXT_DATE, 0, EV_STRING},
												{IDS_COLTEXT_USER, 0, EV_STRING},
												{IDS_COLTEXT_MESSAGE}, static_cast<DWORD>(-1), EV_STRING}},
	{CC_SYMNETDRV_EVENT_LOG_DEBUG,		2,     {{IDS_COLTEXT_DATE, 0, EV_STRING},
												{IDS_COLTEXT_USER, 0, EV_STRING}}},
};


HRESULT CLogViewerSymNetiCategory::GetColumnCount(int& iColumnCount)
{
    iColumnCount = 0;

	for (int i=0; i < CCDIMOF(aLogTypeLookupTable); i++)
		if (m_nEventType == static_cast<long>(aLogTypeLookupTable[i].dwSNType))
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
		if (m_nEventType == static_cast<long>(aLogTypeLookupTable[i].dwSNType))
		{
			if (!m_bUserColumn && iColumn > 0)
				iColumn++;

			if (iColumn >= static_cast<int>(aLogTypeLookupTable[i].dwColumnCount))
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

	if (m_dwUserLevel != ISUSERTYPE_SUPERVISOR &&
        m_dwUserLevel != ISUSERTYPE_NORMAL)
    {

        return E_ACCESSDENIED;
    }

	return CLogViewerCategoryHelper::GetEventCount(iEventCount);
}

HRESULT CLogViewerSymNetiCategory::SetLogFileSize(int iLogFileSize)
{
    // Validate user rights

	if (m_dwUserLevel != ISUSERTYPE_SUPERVISOR)
    {
		if(!ValidateUser())
			return E_ACCESSDENIED;
		else
			m_dwUserLevel = ISUSERTYPE_SUPERVISOR;

    }

	return CLogViewerCategoryHelper::SetLogFileSize(iLogFileSize);
}

HRESULT CLogViewerSymNetiCategory::SetLoggingState(BOOL bEnabled)
{
    // Validate user rights

	if (m_dwUserLevel != ISUSERTYPE_SUPERVISOR)
    {
		if(!ValidateUser())
			return E_ACCESSDENIED;
		else
			m_dwUserLevel = ISUSERTYPE_SUPERVISOR;
    }

	return CLogViewerCategoryHelper::SetLoggingState(bEnabled);
}

HRESULT CLogViewerSymNetiCategory::ClearCategory()
{
    // Validate user rights

	if (m_dwUserLevel != ISUSERTYPE_SUPERVISOR)
    {
		if(!ValidateUser())
			return E_ACCESSDENIED;
		else
			m_dwUserLevel = ISUSERTYPE_SUPERVISOR;
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
    CExceptionInfo exceptionInfo;
    exceptionInfo.SetFunction(_T(__FUNCTION__));
    try
    {
        pLogViewerSymNetiEvent = new CLogViewerSymNetiEvent;
    }
    CCCATCH(exceptionInfo);
    if (exceptionInfo.IsException())
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

	pLogViewerSymNetiEvent->m_sndhelper = m_sndhelper;
	pLogViewerSymNetiEvent->m_pLogEvent = static_cast<SymNeti::CSNLogEventEx *>(pEvent);
	pLogViewerSymNetiEvent->m_pLogEvent->AddRef();
	pLogViewerSymNetiEvent->m_bShowUser = m_bUserColumn;

    return TRUE;
}
