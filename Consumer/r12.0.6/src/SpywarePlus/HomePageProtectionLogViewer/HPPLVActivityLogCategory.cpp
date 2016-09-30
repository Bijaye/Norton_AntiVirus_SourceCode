#include "StdAfx.h"
#include "resource.h"

#include "HPPLVActivityLogCategory.h"
#include "HPPLVActivityLogEvent.h"

#include "NAVHelpLauncher.h"    // NAVToolbox

#include "SymHelp.h"

const UINT CHPPLVActivityLogCategory::m_ColNameIds[] = { IDS_CAT_ACTIVITY_COLNAME_TIME, 
                                                        IDS_CAT_ACTIVITY_COLNAME_PROCNAME, 
                                                        IDS_CAT_ACTIVITY_COLNAME_ACTION };

CHPPLVActivityLogCategory::CHPPLVActivityLogCategory(void)
{
}

CHPPLVActivityLogCategory::~CHPPLVActivityLogCategory(void)
{
    Destroy();
}

HRESULT CHPPLVActivityLogCategory::GetImageList(HIMAGELIST& hImageList)
{
    return E_NOTIMPL;
}

HRESULT CHPPLVActivityLogCategory::GetColumnCount(int& iColumnCount)
{
    iColumnCount = _countof(m_ColNameIds);

    return S_OK;
}

HRESULT CHPPLVActivityLogCategory::GetCategoryID(int& iCategoryID)
{
    iCategoryID = CategoryId;

    return S_OK;
}

HRESULT CHPPLVActivityLogCategory::LaunchHelp()
{
	NAVToolbox::CNAVHelpLauncher NAVHelp;
	
	if ( NAVHelp.LaunchHelp ( IDH_NAVW_CCLOGVIEW_HOME_PAGE_PROTECTION ))
		return S_OK;

	return E_FAIL;
}

BOOL CHPPLVActivityLogCategory::CreateEx()
{
    SYMRESULT sr;

    sr = m_EventFactoryLoader.Initialize();
    if(SYM_FAILED(sr))
    {
        return FALSE;
    }

    ccEvtMgr::CEventFactoryExPtr spEventFactory;
    sr = m_EventFactoryLoader.CreateObject(&spEventFactory);
    if(SYM_FAILED(sr))
    {
        return FALSE;
    }

    return CLogViewerCategoryHelper::Create(spEventFactory, HPP::Event_ID_LogEntry_HomePageActivity);
}

HRESULT CHPPLVActivityLogCategory::GetCategoryName(HMODULE& hModule, UINT& nId)
{
    hModule = g_Resources.GetResourceInstance();
    nId = IDS_CATNAME_ACTIVITY;

    return S_OK;
}

HRESULT CHPPLVActivityLogCategory::GetCategoryDescription(HMODULE& hModule, UINT& nId)
{
    hModule = g_Resources.GetResourceInstance();
    nId = IDS_CATDESC_ACTIVITY;
    
    return S_OK;
}

HRESULT CHPPLVActivityLogCategory::GetColumnName(int iColumn, HMODULE& hModule, UINT& nId)
{
    if(iColumn >= 0 && iColumn < _countof(m_ColNameIds))
    {
		hModule = g_Resources.GetResourceInstance();
        nId = m_ColNameIds[iColumn];
        return S_OK;
    }

    return E_FAIL;
}

HRESULT CHPPLVActivityLogCategory::CopyEvent(const ccEvtMgr::CEventEx& pEvent, 
    const ccEvtMgr::CLogFactoryEx::EventInfo& Info,
    cc::ILogViewerEvent*& pLogViewerEvent)
{
    CHPPLVActivityLogEvent *pActLogEvent = new CHPPLVActivityLogEvent;    
    CSymPtr<CHPPLVActivityLogEvent> spActivityLogEntry = pActLogEvent;
    
    CHPPEventCommonInterfaceQIPtr spCommon = &pEvent;
    if(!spCommon)
        return E_FAIL;

    pActLogEvent->SetEventData(spCommon);

    HRESULT hr = spActivityLogEntry->QueryInterface(cc::IID_LogViewerEvent, (void**)&pLogViewerEvent);

    return hr;
}

