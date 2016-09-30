////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Provider.h"

#include "..\NavLogVRes\resource.h"

#include "Hrx.h"
#include "srx.h"

//#include "AVNamesMap.h"
//#include "AVccModuleId.h"
#include "AllNAVEvents.h"
#include "NAVEventFactoryLoader.h"
#include "NAVInfo.h"        // toolbox
#include "NAVEventCommon.h"

#include "StockCategory.h"
#include "StockLists.h"

#include "AvDataIds.h"
#include "AVccModuleID.h"

#include "Message.h"
#include "MessageDetails.h"
#include "SearchContextHelper.h"

#include "AvDefines.h"

#include "QuarantineMenu.h"
#include "SecurityRisksMenu.h"
#include "ScanResultsMenu.h"
#include "UnresolvedSecurityRisksMenu.h"
#include "SuspiciousEmailMenu.h"

#include <isDataClientLoader.h>
#include <uiNISDataElementGuids.h>

#include "ISCanIRun.h"
#include <smartptr.h>

using namespace ccEvtMgr;

enum{ MCF_CATEGORY_THREATS_FOUND = 0xFFFF1001, MCF_CATEGORY_SCANNER_RESULTS, MCF_CATEGORY_OEH_ACTIVITY, MCF_CATEGORY_QUARANTINE_ITEMS, MCF_CATEGORY_UNRESOLVED_ACTIVITY };

#define HRBK(hr, exp, opr) \
    (hr) = (exp); \
    if (FAILED((hr))) \
        opr;

// Event subscription data...
CSubscriberHelper::SubscriberInfo _SubscribedEvents[] =
{
    //  {Event ID, Priority, Event States, Event States count, ProviderID, Monitor only}           
    {AV::Event_ID_Threat, 0,  NULL, 0, 0, false},
    {AV::Event_ID_ScanAction, 0,  NULL, 0, 0, false},
};

#define RENDERER_ENTRY(_MessageType, _RenderFormat, _Class) \
    if(_MessageType == qdwMessageType && IsEqualGUID(guidRenderFormat, _RenderFormat)) \
    { \
        _Class* pDetails; \
        hr = _Class::CreateInstance(pDetails); \
        if(S_OK == hr && pDetails) \
        { \
            symhtml::ISymHTMLDocumentPtr spDetailsDoc; \
            hr = _Class::CreateObjectFromInstance(pDetails, spDetailsDoc); \
            if(S_OK == hr && spDetailsDoc) \
            { \
                pDetails->m_spMessage = pMessage; \
                pDetails->m_spMessageProviderSink = m_pMessageProviderSink; \
                if(!pDetails->m_spMessage) \
                { \
                    CCTRCTXE0(_T("Unable to get internal message interface.")); \
                    hrReturn = E_FAIL; \
                    break; \
                } \
                SYMRESULT sr = spDetailsDoc->QueryInterface(IID_SymBase, (void**)&pRenderer); \
                hrReturn = MCF::HRESULT_FROM_SYMRESULT(sr); \
            } \
        } \
        break; \
    }


// Converts UTC SYSTEMTIME to Local SYSTEMTIME
//
bool UTCSystemTimeToLocalTime ( const SYSTEMTIME* psystimeUTC, SYSTEMTIME* psystimeLocal )
{
    // Log is in UTC System time. We need Variant Local Time. Ugh.
    //
    FILETIME ftimeUTC;
    FILETIME ftimeLocal;

    if ( SystemTimeToFileTime ( psystimeUTC, &ftimeUTC ) &&
        FileTimeToLocalFileTime ( &ftimeUTC, &ftimeLocal ) &&
        FileTimeToSystemTime ( &ftimeLocal, psystimeLocal ))
        return true;
    else 
        return false;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CMCAVProvider::CMCAVProvider(void) : m_bIsSafeMode(false), m_bEvtMgrDestroyed(false), m_bInitialized(false)
{
}

CMCAVProvider::~CMCAVProvider(void)
{
    m_LogManagerHelper.Destroy();

    // Delete the factories in reverse order before the loaders
    if ( m_pProxyFactory )
    {
        m_pProxyFactory.Release ();
    }

    if ( m_pEventFactory )
    {
        m_pEventFactory.Release ();
    }

    Uninitialize();
}

//****************************************************************************
//****************************************************************************
HRESULT CMCAVProvider::Initialize()
{
    if (m_bInitialized)
        return S_OK;
    HRESULT hrReturn = E_FAIL;
    StahlSoft::HRX hrx;
    SRX srx;
    try
    {   
        // We need to skip certain functionality in safe mode
        if(GetSystemMetrics(SM_CLEANBOOT))
        {
            m_bIsSafeMode = true;
            CCTRACEW( CCTRCTX _T("Running in safe mode. Loading CAV in stand-alone mode"));

            srx << m_avSafeModeLoader.Initialize();

            // Get an instance of the stand alone AV object
            srx << m_avSafeModeLoader.CreateObject(m_spStandAlone);
            if( !m_spStandAlone )
                throw _com_error(E_NOINTERFACE);

            hrx << m_spStandAlone->Initialize();
        }
        
        if(!m_bIsSafeMode)
        {
            // Check with product to see if it is OK to show UI
            hrx << VerifyConfigState();

            // Check with license state of the product
            hrx << VerifyLicenseState();

            // Load the factory and proxy DLLs
            AV::NAVEventFactory_CEventFactoryEx2 AvEventFactoryLoader;
            AV::NAVEventFactory_CProxyFactoryEx AvProxyFactoryLoader;
            srx << AvEventFactoryLoader.CreateObject(GETMODULEMGR(), &m_pEventFactory);
            srx << AvProxyFactoryLoader.CreateObject(GETMODULEMGR(), &m_pProxyFactory);
            if(!m_pEventFactory || !m_pProxyFactory)
                throw _com_error(E_UNEXPECTED);

            // These extra references help keep kooky ccEvtMgr::CSubscriberHelper from fouling up.
            // The main issue is asserting in debug but that's due to Release thinking it's been
            // called one to many times. Previously CSymStaticRefCount was used but it doesn't work
            // for this situation. m_EventManager *WILL* get destroyed regardless of ref count.
            m_EventManager.AddRef();
            m_EventManager.AddRef();
            // Create the Event Manager Helper object
            if(FALSE == m_EventManager.Create(m_pEventFactory, m_pProxyFactory))
                throw _com_error(E_UNEXPECTED);
            m_bEvtMgrDestroyed = false;

            // Create Log Helper object
            if (m_LogManagerHelper.Create(m_pEventFactory) == FALSE)
                throw _com_error(E_UNEXPECTED);

            // Register all the events for this factory
            if(CSubscriberHelper::Create(&m_EventManager, _SubscribedEvents, CCDIMOF(_SubscribedEvents)) == FALSE)
                throw _com_error(E_UNEXPECTED);
        }

        m_bInitialized = true;
        hrReturn = S_OK;
    }
    catch(srx_exception& e)
    {
        hrReturn = MCF::HRESULT_FROM_SYMRESULT(e.Error());
    }
    catch(_com_error& e)
    {
        hrReturn = e.Error();
    }


    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CMCAVProvider::Uninitialize()
{
    if (!m_bEvtMgrDestroyed)
    {
        // This removes ref counts to ourselves. We need to ensure proper behavior in all circumstances.
        SubscriberIdMap::iterator pos(m_SubscriberIdMap.begin());
        for (; pos != m_SubscriberIdMap.end(); pos++)
        {
            m_pEventManagerHelper->UnregisterSubscriber(pos->second);
        }
        m_SubscriberIdMap.clear();

        m_bEvtMgrDestroyed = true;
        m_bInitialized = false;
    }

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CMCAVProvider::GetCategories(MCF::ICategoryList*& pCategoriesList)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        MCF::ICategoryInternalPtr spCategory;
        MCF::ICategoryListInternalPtr spCategoryList;
        hrx << MCF::CStockCategoryList::CreateObject(spCategoryList);
        
        if(!m_bIsSafeMode)
        {
            //
            //  Security Risks Activity
            MCF::IMenuPtr spSecurityRisksMenu;
            hrx << CSecurityRisksMenu::CreateObject(spSecurityRisksMenu);
            hrx << MCF::CStockCategory::CreateObject(spCategory);
            hrx << spCategory->SetCategoryId(MCF_CATEGORY_THREATS_FOUND);
            hrx << spCategory->SetDescription(_S(IDS_MCCATEGORY_THREATS_FOUND), NULL);
	            hrx << spCategory->SetMenu(spSecurityRisksMenu);
            hrx << spCategory->SetDisplayFlag(MCF::ICategory::eSummaryList, true);
            hrx << spCategoryList->Add(spCategory);

            spCategory.Release();

            //
            //  Scanner Results
            MCF::IMenuPtr spScanResultsMenu;
            hrx << CScanResultsMenu::CreateObject(spScanResultsMenu);
            hrx << MCF::CStockCategory::CreateObject(spCategory);
            hrx << spCategory->SetCategoryId(MCF_CATEGORY_SCANNER_RESULTS);
            hrx << spCategory->SetDescription(_S(IDS_MCCATEGORY_SCANNER_RESULTS), NULL);
		        hrx << spCategory->SetMenu(spScanResultsMenu);
            hrx << spCategory->SetDisplayFlag(MCF::ICategory::eSummaryList, true);
            hrx << spCategoryList->Add(spCategory);

            spCategory.Release();

            //
            //  OEH Activity
			MCF::IMenuPtr spSuspiciousEmailMenu;
			hrx << CSuspiciousEmailMenu::CreateObject(spSuspiciousEmailMenu);
            hrx << MCF::CStockCategory::CreateObject(spCategory);
            hrx << spCategory->SetCategoryId(MCF_CATEGORY_OEH_ACTIVITY);
            hrx << spCategory->SetDescription(_S(IDS_MCCATEGORY_OEH_ACTIVITY), NULL);
				hrx << spCategory->SetMenu(spSuspiciousEmailMenu);
            hrx << spCategory->SetDisplayFlag(MCF::ICategory::eSummaryList, true);
            hrx << spCategoryList->Add(spCategory);

			spCategory.Release();

			//
			//  Unresolved Activity
			MCF::IMenuPtr spUnresolvedSecurityRisksMenu;
			hrx << CUnresolvedSecurityRisksMenu::CreateObject(spUnresolvedSecurityRisksMenu);
			hrx << MCF::CStockCategory::CreateObject(spCategory);
			hrx << spCategory->SetCategoryId(MCF_CATEGORY_UNRESOLVED_ACTIVITY);
			hrx << spCategory->SetDescription(_S(IDS_MCCATEGORY_UNRESOLVED_ACTIVITY), NULL);
			hrx << spCategory->SetMenu(spUnresolvedSecurityRisksMenu);
			hrx << spCategory->SetDisplayFlag(MCF::ICategory::eSummaryList, true);
			hrx << spCategoryList->Add(spCategory);

			spCategory.Release();
        }
        
        //
        //  Quarantine Items
        CQuarantineMenu* pQuarantineMenu;
        MCF::IMenuPtr spQuarantineMenu;
        hrx << CQuarantineMenu::CreateInstance(pQuarantineMenu);
        hrx << CQuarantineMenu::CreateObjectFromInstance(pQuarantineMenu, spQuarantineMenu);
        pQuarantineMenu->SetMessageProviderSink(m_pMessageProviderSink);
        hrx << MCF::CStockCategory::CreateObject(spCategory);
        hrx << spCategory->SetCategoryId(MCF_CATEGORY_QUARANTINE_ITEMS);
        hrx << spCategory->SetDescription(_S(IDS_MCFCATEGORYDESC_QUARANTINE_ITEMS), NULL);
        hrx << spCategory->SetMenu(spQuarantineMenu);
        hrx << spCategory->SetDisplayFlag(MCF::ICategory::eSummaryList, true);
        hrx << spCategoryList->Add(spCategory);

        spCategory.Release();

        SYMRESULT sr = spCategoryList->QueryInterface(MCF::IID_CategoryList, (void**)&pCategoriesList);
        hrx << MCF::HRESULT_FROM_SYMRESULT(sr);

        hrReturn = S_OK;
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);
    
    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CMCAVProvider::GetProviderId(GUID& guidProviderId)
{
    memcpy(&guidProviderId, &MCPROVIDERID_MCAntivirusProviderId, sizeof(GUID));
    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CMCAVProvider::RegisterProviderSink(MCF::IMessageProviderSink* pMessageProviderSink)
{
    m_pMessageProviderSink = pMessageProviderSink;
    return S_OK;
}


//****************************************************************************
//****************************************************************************
HRESULT CMCAVProvider::QueryMessages(MCF::ISearchContext* pSearchContext, MCF::IMessageList*& pMessageList)
{
    HRESULT hrReturn = MCF::S_NORESULTS;
    
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
#ifdef _DEBUG
        DWORD dwQueryStart = GetTickCount();
#endif
        DWORD dwSourceStart = NULL, dwSourceEnd = NULL;

        //
        // Create a results list
        MCF::IMessageListInternalPtr spResults;
        hrx << MCF::CStockMessageList::CreateObject(spResults);

        //
        //  Retrieve events from log
        MCF::IMessageListInternalPtr spEventList;
        hrx << MCF::CStockMessageList::CreateObject(spEventList);

        MCF::CSearchContextDomainHelper cSearchHelper;
        cSearchHelper.SetSearchContext(pSearchContext);

        if(!m_bIsSafeMode)
        {
            if(cSearchHelper.ShouldSearchCategory(MCF_CATEGORY_THREATS_FOUND))
            {
                dwSourceStart = GetTickCount();
                hrx << RetrieveEvents(AV::Event_ID_Threat, spEventList, cSearchHelper);
                dwSourceEnd = GetTickCount();
                CCTRACEI( CCTRCTX _T("<cclog_threats_time>%08d</cclog_threats_time>"), dwSourceEnd - dwSourceStart);
            }
            
            if(cSearchHelper.ShouldSearchCategory(MCF_CATEGORY_SCANNER_RESULTS))
            {
                dwSourceStart = GetTickCount();
                hrx << RetrieveEvents(AV::Event_ID_ScanAction, spEventList, cSearchHelper);
                dwSourceEnd = GetTickCount();
                CCTRACEI( CCTRCTX _T("<cclog_scans_time>%08d</cclog_scans_time>"), dwSourceEnd - dwSourceStart);
            }
        }
        
        if(   cSearchHelper.ShouldSearchCategory(MCF_CATEGORY_THREATS_FOUND)
           || cSearchHelper.ShouldSearchCategory(MCF_CATEGORY_OEH_ACTIVITY)
           || cSearchHelper.ShouldSearchCategory(MCF_CATEGORY_QUARANTINE_ITEMS)
           || cSearchHelper.ShouldSearchCategory(MCF_CATEGORY_UNRESOLVED_ACTIVITY)
           )
        {
            dwSourceStart = GetTickCount();
            hrx << RetrieveAvModuleItems(spEventList, cSearchHelper);
            dwSourceEnd = GetTickCount();
            CCTRACEI( CCTRCTX _T("<avmodule_items_time>%08d</avmodule_items_time>"), dwSourceEnd - dwSourceStart);
        }

        dwSourceStart = GetTickCount();
        //
        // Query for All Results
        MCF::IAllResultsSearchQIPtr spAllResults = pSearchContext;
        if(spAllResults)
        {
            hrx << spResults->Add(spEventList);
        }
        else
        {
            HRESULT hr;
            DWORD dwMessageCount = NULL;
            hrx << spEventList->GetCount(dwMessageCount);
            for(DWORD dwMessageIndex = 0; dwMessageIndex < dwMessageCount; dwMessageIndex++)
            {
                MCF::IMessagePtr spMessage;
                hr = spEventList->GetItem(dwMessageIndex, spMessage);
                if(FAILED(hr) || !spMessage)
                    continue;

                hr = spMessage->Compare(pSearchContext);
                if(S_OK == hr)
                    spResults->Add(spMessage);
            }
        }

        //
        //  Were there any results?
        DWORD dwResultsListCount = NULL;
        hrx << spResults->GetCount(dwResultsListCount);
        if(NULL != dwResultsListCount)
        {
            SYMRESULT sr = spResults->QueryInterface(MCF::IID_MessageList, (void**)&pMessageList);
            hrx << MCF::HRESULT_FROM_SYMRESULT(sr);
        }

        dwSourceEnd = GetTickCount();
        CCTRACEI( CCTRCTX _T("<results_query_time>%08d</results_query_time>"), dwSourceEnd - dwSourceStart);

        hrReturn = S_OK;
    
#ifdef _DEBUG
        DWORD dwQueryEnd = GetTickCount();
        CCTRACEI( CCTRCTX _T("<provider_query_time>%08d</provider_query_time>"), dwQueryEnd - dwQueryStart);
#endif
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CMCAVProvider::RenderMessage(REFGUID guidRenderFormat, MCF::IMessage* pMessage, ISymBase*& pRenderer)
{
    HRESULT hrReturn = S_FALSE, hr;
    for(;;)
    {
        // Sanity checks
        GUID guidProviderId;
        hr = pMessage->GetProviderId(guidProviderId);
        if(FAILED(hr))
        {
            CCTRCTXE1(_T("IMessage::GetProviderId() failed. hr = 0x%08X"), hr);
            break;
        }

        // Am I the provider for this message?
        if(!IsEqualIID(guidProviderId, MCPROVIDERID_MCAntivirusProviderId))
        {
            CCTRCTXE0(_T("Not my message"));
            break;
        }

        ui::IDataPtr spData;
        hr = pMessage->GetData(AV::AVDATA_COMPONENT_ID, spData);
        if(FAILED(hr))
            break;

        ui::INumberDataQIPtr spNumber = spData;
        if(!spNumber)
            break;

        LONGLONG qdwComponentId = NULL;
        hr = spNumber->GetNumber(qdwComponentId);
        if(FAILED(hr))
            break;

        spData.Release();
        spNumber.Release();

        hr = pMessage->GetData(AV::AVDATA_MESSAGE_TYPE, spData);
        if(FAILED(hr))
            break;

        spNumber = spData;
        if(!spNumber)
            break;

        LONGLONG qdwMessageType = NULL;
        hr = spNumber->GetNumber(qdwMessageType);
        if(FAILED(hr))
            break;

        ISymBaseQIPtr spRenderer;
        RENDERER_ENTRY(CAntivirusMessageBase::eAutoProtectResults, MCF::MCFRENDER_MessageCenterUI_Details, CAutoProtectDetails);
        RENDERER_ENTRY(CAntivirusMessageBase::eAutoProtectResults, MCF::MCFRENDER_MessageCenterUI_AdvancedDetails, CAutoProtectAdvancedDetails);
        RENDERER_ENTRY(CAntivirusMessageBase::eManualScanResults, MCF::MCFRENDER_MessageCenterUI_Details, CScanResultsDetails);
        RENDERER_ENTRY(CAntivirusMessageBase::eManualScanResults, MCF::MCFRENDER_MessageCenterUI_AdvancedDetails, CScanResultsAdvancedDetails);
        RENDERER_ENTRY(CAntivirusMessageBase::eAvModuleItem, MCF::MCFRENDER_MessageCenterUI_Details, CAvModuleItemDetails);
        RENDERER_ENTRY(CAntivirusMessageBase::eAvModuleItem, MCF::MCFRENDER_MessageCenterUI_AdvancedDetails, CAvModuleItemAdvancedDetails);

        break;
    }


    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CMCAVProvider::RetrieveAvModuleItems(MCF::IMessageListInternal* pEventList, MCF::CSearchContextDomainHelper& cSearchHelper)
{
    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
		// Need to take a LoadLibrary() lock on this to ensure
		// AvThreatInfo objects stick around.
		//  TODO: Figure out why MODULEMGR is not keeping them
		//        around.		
        AvThreatInfoLoader.Initialize();

        AVModule::IAVThreatInfoPtr spThreatInfo;
        AvThreatInfoLoader.CreateObject(spThreatInfo);
        if(!spThreatInfo)
        {
            CCTRACEW( CCTRCTX _T("AvModuleLoader::CreateObject() failed."));
            hrx << E_UNEXPECTED;
        }
        
        AVModule::AVLoader_IAVMapDwordData AVMapDwordDataLoader;
        AVModule::IAVMapDwordDataPtr spFilter;
        AVMapDwordDataLoader.CreateObject(spFilter);
        if(!spFilter)
        {
            CCTRACEW( CCTRCTX _T("AVLoader_IAVMapDwordData::CreateObject() failed."));
            hrx << E_UNEXPECTED;
        }
        
        // Only get resolved threats
        spFilter->SetValue(AVModule::ThreatTracking::GetThreatBy_RESOLVED_UNRESOLVED_FLAG, (DWORD)2);

        AVModule::IAVArrayDataPtr spThreatData;
        hrx << spThreatInfo->GetThreatsWithFilter(NULL, spThreatData);

        DWORD dwThreatInfoCount = NULL;
        hrx << spThreatData->GetCount(dwThreatInfoCount);

        for(DWORD dwThreatInfoIndex = 0; dwThreatInfoIndex < dwThreatInfoCount; dwThreatInfoIndex++)
        {
            HRESULT hr;
            ISymBasePtr spTempData;
            hr = spThreatData->GetValue(dwThreatInfoIndex, AVModule::AVDataTypes::eTypeMapDword, spTempData);
            if(FAILED(hr) || !spTempData)
                continue;

            AVModule::IAVMapDwordDataQIPtr spMapCurThreat = spTempData;
            if(!spMapCurThreat)
                continue;
            
            CAvModuleItemMessage* pQItemMessage = NULL;
            hr = CAvModuleItemMessage::CreateInstance(pQItemMessage);
            if(FAILED(hr))
                continue;

            MCF::IMessagePtr spMessage;
            hr = CAvModuleItemMessage::CreateObjectFromInstance(pQItemMessage, spMessage);
            if(FAILED(hr))
                continue;

            CCalculateThreatData cThreatData(spMapCurThreat);
            if(   (cSearchHelper.ShouldSearchCategory(MCF_CATEGORY_THREATS_FOUND) && cThreatData.IsMCFSecurityRiskItem())
               || (cSearchHelper.ShouldSearchCategory(MCF_CATEGORY_OEH_ACTIVITY) && cThreatData.IsMCFSuspiciousEmail())
               || (cSearchHelper.ShouldSearchCategory(MCF_CATEGORY_QUARANTINE_ITEMS) && cThreatData.IsMCFQuarantineItem())
               || (cSearchHelper.ShouldSearchCategory(MCF_CATEGORY_UNRESOLVED_ACTIVITY) && cThreatData.IsMCFUnresolvedRiskItem()) )
            {
                SYSTEMTIME stStartTime, stEndTime;
                if(cSearchHelper.GetDateRange(stStartTime, stEndTime))
                {
                    DWORD dwTime = NULL;
                    hr = spMapCurThreat->GetValue(AVModule::ThreatTracking::TrackingData_DETECTION_TIME, dwTime);
                    if(SUCCEEDED(hr))
                    {
                        CTime ctStartTime(stStartTime), ctEndTime(stEndTime);
                        CTime ctDetectionTime = CTime(dwTime);

                        if(!(ctDetectionTime > stStartTime && ctDetectionTime < ctEndTime))
                        {
                            // don't bother processing this item
                            continue;
                        }
                                                    
                    }
                }

                hr = pQItemMessage->Initialize(MCPROVIDERID_MCAntivirusProviderId, spMapCurThreat);
                if(SUCCEEDED(hr))
                    pEventList->Add(spMessage);
            }

        }

    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
        return E_FAIL;

    return S_OK;
}

//****************************************************************************
//****************************************************************************
HRESULT CMCAVProvider::RetrieveEvents(DWORD dwEventId, MCF::IMessageListInternal* pEventList, MCF::CSearchContextDomainHelper& cSearchHelper)
{
    // Search for events
    //
    ccEvtMgr::CError::ErrorType eError = ccEvtMgr::CError::eNoError;
    // 
    //  Initialize list
    HRESULT hr, hrReturn = MCF::S_NORESULTS;

    long nContextId = 0;
    long nFail = 0;
    long nOk = 0;
    SYSTEMTIME timeStart, timeStop;
    ZeroMemory(&timeStart, sizeof(timeStart));
    ZeroMemory(&timeStop, sizeof(timeStop));
    cSearchHelper.GetDateRange(timeStart, timeStop);

    ccEvtMgr::CLogManagerHelper::EventInfoVector cEventInfoVector;
    INT nEventCount;

    // ??? Add exception handling  ***TODO: Replace with vector, dangerous point code follows.
    CEventEx** pEventArray = NULL;

    for(;;)
    {
        eError = m_LogManagerHelper.SearchEvents( dwEventId,
                                                timeStart,
                                                timeStop,
                                                true,
                                                0,
                                                nContextId,
                                                cEventInfoVector);
        
        if (CError::eNoError != eError)
        {
            nEventCount = 0;

            CCTRACEE(_T("SearchEvents failed!"));
            hrReturn = MCF::S_NORESULTS;
            break;
        }
        
        nEventCount = long(cEventInfoVector.size());

        // Allocate memory to receive the CEventEx*
        // ??? Add exception handling  ***TODO: Replace with vector, dangerous point code follows.
        pEventArray = new CEventEx*[nEventCount];
        ZeroMemory(pEventArray, sizeof(CEventEx*) * nEventCount);

        // Load all the events at once
        eError = m_LogManagerHelper.LoadEventBatch( dwEventId,
                                                    cEventInfoVector,
                                                    nContextId,
                                                    pEventArray,
                                                    nEventCount);

        if(CError::eNoError != eError)
        {
            hrReturn = MCF::S_NORESULTS;
            break;
        }
        bool bResults = false;
        for(INT nEventIndex = 0; nEventIndex < nEventCount; nEventIndex++)
        {
            CNAVEventCommonQIPtr spNAVEvent(pEventArray[nEventIndex]);
            if(!spNAVEvent)
                continue;
            
            // Log entries are in UTC
            SYSTEMTIME stLocalTime;
            UTCSystemTimeToLocalTime ( &cEventInfoVector[nEventIndex].m_TimeStamp, &stLocalTime );

            if(AV::Event_ID_Threat == dwEventId)
            {
                LONG lSubType = NULL;
                spNAVEvent->props.GetData(AV::Event_Threat_propEventSubType, lSubType);

                if(AV::Event_APBlockedThreat == lSubType)
				{
					CThreatActivityMessage* pMessage = NULL;
                    HRBK(hr, CThreatActivityMessage::CreateInstance(pMessage), continue);
                    if(!pMessage)
                        break; // out of memory

                    MCF::IMessagePtr spMessage;
                    HRBK(hr, CThreatActivityMessage::CreateObjectFromInstance(pMessage, spMessage), continue);
                    HRBK(hr, pMessage->Initialize(MCPROVIDERID_MCAntivirusProviderId, spNAVEvent, stLocalTime), continue);
                    pEventList->Add(spMessage);
                    bResults = true;
                }
            }
            else if(AV::Event_ID_ScanAction == dwEventId)
            {
                CScanResultsMessage* pMessage = NULL;
                HRBK(hr, CScanResultsMessage::CreateInstance(pMessage), continue);
                if(!pMessage)
                    break; // out of memory

                MCF::IMessagePtr spMessage;
                HRBK(hr, CScanResultsMessage::CreateObjectFromInstance(pMessage, spMessage), continue);
                HRBK(hr, pMessage->Initialize(MCPROVIDERID_MCAntivirusProviderId, spNAVEvent, stLocalTime), continue);
                pEventList->Add(spMessage);
                bResults = true;
            }
        }

        hrReturn = bResults ? S_OK : MCF::S_NORESULTS;
        break;
    }

    // Delete temp events now that we have a local copy
    //
    if (pEventArray != NULL)
    {
        m_LogManagerHelper.DeleteBatchData(pEventArray, nEventCount);
        delete[] pEventArray;
        pEventArray = NULL;
    }
    

    if (nContextId != 0)
    {
        m_LogManagerHelper.DeleteSearchContext(dwEventId, nContextId);
        nContextId = 0;
    }
    
    return hrReturn;
}

CError::ErrorType CMCAVProvider::OnEvent(long nSubscriberId, const CEventEx& Event, EventAction& eAction)
{
    HRESULT hrReturn;
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        CEventExPtr spEventCopy;
        ccEvtMgr::CError::ErrorType eErr;
        eErr = m_EventManager.CopyEvent(Event, spEventCopy);
        if(ccEvtMgr::CError::eNoError != eErr || !spEventCopy)
            throw _com_error(E_UNEXPECTED);
        
        CNAVEventCommonQIPtr spNavEvent = spEventCopy;
        if(!spNavEvent)
            throw _com_error(E_NOINTERFACE);
        
        LONG lEventType = NULL;
        if(!spNavEvent->props.GetData(AV::Event_Base_propType, lEventType))
        {
            CCTRACEE( CCTRCTX _T("Unable to get AV::Event_Base_propType value from event."));
            throw _com_error(E_UNEXPECTED);
        }

        SYSTEMTIME stTimestamp;
        GetLocalTime(&stTimestamp);

        MCF::IMessagePtr spMessage;
        if(AV::Event_ID_Threat == lEventType)
        {
// intentionally removed temporarily while I
// move over to using AvModule for this data
//             CThreatActivityMessage* pMessage = NULL;
//             hrx << CThreatActivityMessage::CreateInstance(pMessage);
//             hrx << CThreatActivityMessage::CreateObjectFromInstance(pMessage, spMessage);
//             hrx << pMessage->Initialize(MCPROVIDERID_MCAntivirusProviderId, spNavEvent, stTimestamp);
        }
        else if(AV::Event_ID_ScanAction == lEventType)
        {
            CScanResultsMessage* pMessage = NULL;
            hrx << CScanResultsMessage::CreateInstance(pMessage);
            hrx << CScanResultsMessage::CreateObjectFromInstance(pMessage, spMessage);
            hrx << pMessage->Initialize(MCPROVIDERID_MCAntivirusProviderId, spNavEvent, stTimestamp);
        }

        if(spMessage && m_pMessageProviderSink)
            m_pMessageProviderSink->OnNewMessage(spMessage);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return CError::eNoError;
}

CError::ErrorType CMCAVProvider::OnShutdown()
{
    return CError::eNoError;
}

//****************************************************************************
//****************************************************************************
HRESULT CMCAVProvider::VerifyConfigState()
{
#ifdef _DEBUG
    return true;
#endif // _DEBUG

    HRESULT hrReturn = E_FAIL;

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        ISShared::ISShared_IProvider ProviderLoader;
        ui::IProviderPtr spISElementProvider;
        SYMRESULT sr = ProviderLoader.CreateObject(GETMODULEMGR(), &spISElementProvider);
        hrx << AvProdWidgets::HRESULT_FROM_SYMRESULT(sr);

        ui::IElementPtr spShowUIElement;
        hrx << spISElementProvider->GetElement(ISShared::CLSID_NIS_CanShowUI, spShowUIElement);

        ui::IDataPtr spData;
        hrx << spShowUIElement->GetData(spData);

        ui::INumberDataQIPtr spNumber = spData;
        if(!spNumber)
            hrx << E_UNEXPECTED;

        LONGLONG qdwNumber = NULL;
        hrx << spNumber->GetNumber(qdwNumber);
        
        if(ISShared::i64StateOn != qdwNumber)
        {
            CCTRACEW( CCTRCTX _T("Config wiz has not finished. ISShared::CLSID_NIS_CanShowUI == 0x0000"), (DWORD)qdwNumber);
            hrReturn = MCF::E_PROVIDER_NOT_READY_CFGWIZ;
        }
        else
            hrReturn = S_OK;        
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        CCTRCTXE1(_T("Unable to determine if UI should be shown. Error: %s"), exceptionInfo.GetDescription());
        hrReturn = MCF::E_PROVIDER_NOT_READY;
    }

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CMCAVProvider::VerifyLicenseState()
{
#ifdef _DEBUG
    return true;
#endif // _DEBUG

    HRESULT hrReturn = E_FAIL;

    StahlSoft::HRX hrx;
    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        // if license state is valid, set hrReturn == S_OK
        //  else if license state is expired, set hrReturn == MCF::E_PROVIDER_NOT_READY_LICENSE

		CProductCanIHelper CanIRunHelper;

		if(CanIRunHelper.CanIRun())
			hrReturn = S_OK;
		else
			hrReturn = MCF::E_PROVIDER_NOT_READY_LICENSE;


    }
    CCCATCHMEM(exceptionInfo)
    CCCATCHCOM(exceptionInfo);

    if(exceptionInfo.IsException())
    {
        CCTRCTXE1(_T("Unable to determine product license state. Error: %s"), exceptionInfo.GetDescription());
        hrReturn = MCF::E_PROVIDER_NOT_READY;
    }

    return hrReturn;
}
