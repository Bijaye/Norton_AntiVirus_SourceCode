// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Message.h"

#include <memory>

#include "StockData.h"
#include <string>

#include "AVccModuleID.h"
#include "AvDataIds.h"

#include "AVNamesMap.h"
#include "ThreatCatInfo.h"

#include "AvDataObjects.h"
#include "isResource.h"
#include "ISVersion.h"

#include "..\navlogvres\resource.h"

#include <uiAvProviderElements.h>

#include <ccSerializeInterface.h>
#include <ccSymMemoryStreamIMpl.h>

#include <ccEraserInterface.h>

struct GUID_DWORD {SYMGUID guidDataId; LONG lAvEventId;};

enum{ MCF_CATEGORY_SECURITY_RISKS = 0xFFFF1001, MCF_CATEGORY_SCANNER_RESULTS, MCF_CATEGORY_OEH_ACTIVITY, MCF_CATEGORY_QUARANTINE_ITEMS, MCF_CATEGORY_UNRESOLVED_ACTIVITY };

CAVFeatureNameMap CAntivirusMessageBase::m_Features; 
AvProdLogging::CAvProdLoggingACP CAntivirusMessageBase::m_AccessControl;
CThreatCatInfo CAntivirusMessageBase::m_ThreatInfo;

#define GUID_FORMAT_STR(xGUID) L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", \
    xGUID.Data1, xGUID.Data2, xGUID.Data3, \
    xGUID.Data4[0], xGUID.Data4[1], xGUID.Data4[2], xGUID.Data4[3], \
    xGUID.Data4[4], xGUID.Data4[5], xGUID.Data4[6], xGUID.Data4[7]

#define HRBK(hr, exp, opr) \
    (hr) = (exp); \
    if (FAILED((hr))) \
        opr;

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************

//****************************************************************************
//****************************************************************************
HRESULT CAntivirusMessageBase::CreateTimeData(const SYMGUID &guidDataId, LONG lItemBagIndex, CManagedEventData* pEventData)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        CAvItemBagDateData* pDateData;
        CAvItemBagDateData::CreateInstance(pDateData);
        if(!pDateData)
            throw _com_error(E_UNEXPECTED);

        ui::IDataPtr spData;
        CAvItemBagDateData::CreateObjectFromInstance(pDateData, spData);
        if(!spData)
            throw _com_error(E_UNEXPECTED);

        pDateData->SetDataSource(guidDataId, pEventData, lItemBagIndex);
        hrReturn = SetData(spData);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAntivirusMessageBase::CreateStringData(const SYMGUID &guidDataId, LONG lItemBagIndex, CManagedEventData* pEventData)
{
    LPCWSTR szValue = NULL;
	if (!pEventData->GetData(lItemBagIndex, szValue))
    {
		CCTRACEE((_T(__FUNCTION__) _T("Data missing in IData element!")));
		return E_FAIL;
    }
	return SetString(guidDataId, szValue);
}

//****************************************************************************
//****************************************************************************
HRESULT CAntivirusMessageBase::CreateNumberData(const SYMGUID &guidDataId, LONG lItemBagIndex, CManagedEventData* pEventData)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        CAvItemBagNumberData* pNumberData;
        CAvItemBagNumberData::CreateInstance(pNumberData);
        if(!pNumberData)
            throw _com_error(E_UNEXPECTED);

        ui::IDataPtr spData;
        CAvItemBagNumberData::CreateObjectFromInstance(pNumberData, spData);
        if(!spData)
            throw _com_error(E_UNEXPECTED);

        pNumberData->SetDataSource(guidDataId, pEventData, lItemBagIndex);
        hrReturn = SetData(spData);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAntivirusMessageBase::CreateTimeData(const SYMGUID &guidDataId, const SYSTEMTIME& stTime)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        ui::IDateDataPtr spTimeData;
        hrx << MCF::CStockDateData::CreateObject(spTimeData);

        MCF::IDataInternalQIPtr spDataInternal = spTimeData;
        if(!spDataInternal)
            throw _com_error(E_NOINTERFACE);

        hrx << spDataInternal->SetID(guidDataId);
        hrx << spTimeData->SetDate(stTime);

        ui::IDataQIPtr spData = spTimeData;
        hrReturn = SetData(spData);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAntivirusMessageBase::CreateStringData(const SYMGUID &guidDataId, LPCWSTR wszString)
{
    return SetString(guidDataId, wszString);
}

//****************************************************************************
//****************************************************************************
HRESULT CAntivirusMessageBase::CreateNumberData(const SYMGUID &guidDataId, LONGLONG qdwNumber)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        ui::INumberDataPtr spNumberData;
        hrx << MCF::CStockNumberData::CreateObject(spNumberData);

        MCF::IDataInternalQIPtr spDataInternal = spNumberData;
        if(!spDataInternal)
            throw _com_error(E_UNEXPECTED);

        hrx << spDataInternal->SetID(guidDataId);
        hrx << spNumberData->SetNumber(qdwNumber);

        ui::IDataQIPtr spData = spNumberData;
        hrReturn = SetData(spData);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAntivirusMessageBase::CreateTimeData(const SYMGUID &guidDataId, DWORD dwIndex, AVModule::IAVMapDwordData* pEventData)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        CAVDwordMapDateData* pStringData;
        CAVDwordMapDateData::CreateInstance(pStringData);
        if(!pStringData)
            throw _com_error(E_UNEXPECTED);

        ui::IDataPtr spData;
        CAVDwordMapDateData::CreateObjectFromInstance(pStringData, spData);
        if(!spData)
            throw _com_error(E_UNEXPECTED);

        pStringData->SetDataSource(guidDataId, pEventData, dwIndex);
        hrReturn = SetData(spData);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAntivirusMessageBase::CreateStringData(const SYMGUID &guidDataId, DWORD dwIndex, AVModule::IAVMapDwordData* pEventData)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        CAVDwordMapStringData* pStringData;
        CAVDwordMapStringData::CreateInstance(pStringData);
        if(!pStringData)
            throw _com_error(E_UNEXPECTED);

        ui::IDataPtr spData;
        CAVDwordMapStringData::CreateObjectFromInstance(pStringData, spData);
        if(!spData)
            throw _com_error(E_UNEXPECTED);

        pStringData->SetDataSource(guidDataId, pEventData, dwIndex);
        hrReturn = SetData(spData);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAntivirusMessageBase::CreateNumberData(const SYMGUID &guidDataId, DWORD dwIndex, AVModule::IAVMapDwordData* pEventData)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        CAVDwordMapNumberData* pStringData;
        CAVDwordMapNumberData::CreateInstance(pStringData);
        if(!pStringData)
            throw _com_error(E_UNEXPECTED);

        ui::IDataPtr spData;
        CAVDwordMapNumberData::CreateObjectFromInstance(pStringData, spData);
        if(!spData)
            throw _com_error(E_UNEXPECTED);

        pStringData->SetDataSource(guidDataId, pEventData, dwIndex);
        hrReturn = SetData(spData);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAntivirusMessageBase::CreateTimeData(const SYMGUID &guidDataId, LPCWSTR wszIndex, AVModule::IAVMapStrData* pEventData)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        CAVStrMapDateData* pStringData;
        CAVStrMapDateData::CreateInstance(pStringData);
        if(!pStringData)
            throw _com_error(E_UNEXPECTED);

        ui::IDataPtr spData;
        CAVStrMapDateData::CreateObjectFromInstance(pStringData, spData);
        if(!spData)
            throw _com_error(E_UNEXPECTED);

        pStringData->SetDataSource(guidDataId, pEventData, wszIndex);
        hrReturn = SetData(spData);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAntivirusMessageBase::CreateStringData(const SYMGUID &guidDataId, LPCWSTR wszIndex, AVModule::IAVMapStrData* pEventData)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        CAVStrMapStringData* pStringData;
        CAVStrMapStringData::CreateInstance(pStringData);
        if(!pStringData)
            throw _com_error(E_UNEXPECTED);

        ui::IDataPtr spData;
        CAVStrMapStringData::CreateObjectFromInstance(pStringData, spData);
        if(!spData)
            throw _com_error(E_UNEXPECTED);

        pStringData->SetDataSource(guidDataId, pEventData, wszIndex);
        hrReturn = SetData(spData);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAntivirusMessageBase::CreateNumberData(const SYMGUID &guidDataId, LPCWSTR wszIndex, AVModule::IAVMapStrData* pEventData)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        CAVStrMapNumberData* pStringData;
        CAVStrMapNumberData::CreateInstance(pStringData);
        if(!pStringData)
            throw _com_error(E_UNEXPECTED);

        ui::IDataPtr spData;
        CAVStrMapNumberData::CreateObjectFromInstance(pStringData, spData);
        if(!spData)
            throw _com_error(E_UNEXPECTED);

        pStringData->SetDataSource(guidDataId, pEventData, wszIndex);
        hrReturn = SetData(spData);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CThreatActivityMessage::CThreatActivityMessage(void)
{
}

CThreatActivityMessage::~CThreatActivityMessage(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CThreatActivityMessage::Initialize(REFGUID guidProviderId, CNAVEventCommon*& spNavEventCommon, const SYSTEMTIME& stTimestamp)
{
    HRESULT hrReturn = MCF::S_NORESULTS;
    do
    {
        m_spNavEventCommon = spNavEventCommon;
        m_spEventData = new CManagedEventData(&(spNavEventCommon->props));

        HRBK(hrReturn, CreateCategoryList(), break);
        
        // Set Event Type
        HRBK(hrReturn, CreateNumberData(AV::AVDATA_MESSAGE_TYPE, (LONGLONG)CAntivirusMessageBase::eAutoProtectResults), break);

        //
        // Set Provider Id
        memcpy(&m_guidProviderId, &guidProviderId, sizeof(GUID));

        //
        // Set Timestamp
        HRBK(hrReturn, CreateTimeData(MCF::MCFDATA_TIMESTAMP, stTimestamp), break);
        
        //
        // Populate Data
        HRBK(hrReturn, GetThreatData(), break);

        //
        // Populate Short Description
        HRBK(hrReturn, CreateShortDescription(MCF::MCFDATA_DESCRIPTION_SHORT), break);

        hrReturn = S_OK;
    } while (false);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CThreatActivityMessage::CreateShortDescription(const SYMGUID &guidDataId)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        bool bRet;

        LONG lComponentId;
        bRet = m_spNavEventCommon->props.GetData(AV::Event_Threat_propFeature, lComponentId);
        if(!bRet)
            throw _com_error(E_UNEXPECTED);
        
        CString cszComponentName = m_Features.GetFeatureName(lComponentId);

        CEventData eventFileList;
        bRet = m_spNavEventCommon->props.GetNode( AV::Event_Threat_propSubFileData, eventFileList );
        if(!bRet)
            throw _com_error(E_UNEXPECTED);

        long lFileCountCount = eventFileList.GetCount();
        for(long lFileIndex = 0; lFileIndex < lFileCountCount; lFileIndex++)
        {
            CEventData eventFileData;
            bRet = eventFileList.GetNode(lFileIndex, eventFileData);
            if(!bRet)
                continue;
            
            CEventData eventThreatList;
            bRet = eventFileData.GetNode(AV::Event_ThreatSubFile_propThreatData, eventThreatList);
            if(!bRet)
                continue;

            long lThreatCount = eventThreatList.GetCount();
            for(long lThreatIndex = 0; lThreatIndex < lThreatCount; lThreatIndex++)
            {
                CEventData eventThreat;
                bRet = eventThreatList.GetNode(lThreatIndex, eventThreat);
                if(!bRet)
                    continue;

                long lActionCount = 0;
                bRet = eventThreat.GetData(AV::Event_ThreatEntry_propActionCount, lActionCount);

                //
                // Virus name
                std::wstring wstrVirusName;
                bRet = eventThreat.GetData(AV::Event_ThreatEntry_propVirusName, wstrVirusName );
                if(!bRet)
                    continue;
                
                CString cszShortDescription;
		        static WCHAR szTemp [1024] = {0}; // Once string is loaded this is const so we avoid reloading.
		        if (0 == *szTemp)
                    ::LoadStringW( _Module.GetResourceInstance(), IDS_TEXT_HAS_DETECTED, szTemp, sizeof(szTemp)/sizeof(WCHAR));
                
                cszShortDescription.Format(_T("%s %s %s"), cszComponentName,szTemp, wstrVirusName.c_str());
                hrx << CreateStringData(guidDataId, cszShortDescription);
                hrx << CreateStringData(AV::AVDATA_RISK_NAME, wstrVirusName.c_str());

                hrReturn = S_OK;
                break;
            }

        }

    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CThreatActivityMessage::CreateCategoryList()
{
    if(!m_spNavEventCommon)
        return E_UNEXPECTED;

    DWORD dwEventId = m_spNavEventCommon->GetType();
    if(NULL == dwEventId)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        MCF::ICategoryInternalPtr spCategory;
        MCF::ICategoryListInternalPtr spCategoryList;
        hrx << MCF::CStockCategoryList::CreateObject(spCategoryList);
        
        switch(dwEventId)
        {
            case AV::Event_ID_Threat:
			{
				hrx << MCF::CStockCategory::CreateObject(spCategory);
				hrx << spCategory->SetCategoryId(MCF_CATEGORY_SECURITY_RISKS);
				
		        static WCHAR szTemp [1024] = {0}; // Once string is loaded this is const so we avoid reloading.
		        if (0 == *szTemp)
                    ::LoadStringW( _Module.GetResourceInstance(), IDS_MCFCATEGORYDESC_SECURITY_RISKS, szTemp, sizeof(szTemp)/sizeof(WCHAR));

				hrx << spCategory->SetDescription(szTemp, NULL);
				hrx << spCategoryList->Add(spCategory);
				break;
			}
            default:
                throw _com_error(E_UNEXPECTED);
        };

        DWORD dwCategoryListCount = NULL;
        hrx << spCategoryList->GetCount(dwCategoryListCount);
        if(NULL != dwCategoryListCount)
        {
            hrReturn = SetCategories(spCategoryList);
        }
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CThreatActivityMessage::GetThreatData()
{
    // Is this a supported type?
    //
    long lType = 0;
    m_spEventData->GetData(AV::Event_Base_propType, lType );
    if ( lType != AV::Event_ID_Threat )
        return E_INVALIDARG;

    // Get message GUID
    GUID msgGUID;
    long len = sizeof(GUID);
    if(!m_spEventData->GetData(AV::Event_Base_propUniqueID, (BYTE*)&msgGUID, len))
    {
        CCTRCTXE0(_T("unable to get event GUID"));
        return E_INVALIDARG;
    }
    HRESULT hr = SetMessageId(msgGUID);
    if(FAILED(hr))
    {
        CCTRCTXE1(_T("unable to set event GUID:0x%08x"), hr);
        return hr;
    }

    // Get the sub type
    long lSubType = 0;

    m_spEventData->GetData ( AV::Event_Threat_propEventSubType, lSubType );

    CManagedEventDataPtr spEventDataAllFiles = new CManagedEventData;
    if (!m_spEventData->GetNode ( AV::Event_Threat_propSubFileData, *(spEventDataAllFiles.m_p) ))
    {
        CCTRACEE (_T("CVirusAlertCategory::processLogEvents - no files!"));
        return E_INVALIDARG;
    }

    long lNumSubFiles = spEventDataAllFiles->GetCount();

    // strHelpers
    CAVActionTakenMap ActionTaken;
    ATL::CString cszTemp;
    std::string strTemp;

    // Scope Commmon data work
    {
        // User Name
        //
        hr = CreateStringData(AV::AVDATA_USER_NAME, AV::Event_Base_propUserName, m_spEventData);
        if(FAILED(hr))
        {
            CCTRACEW( CCTRCTX _T("Unable to create User Name. hr=0x%08X"), hr);
        }

        // Product Name
        //
        hr = CreateStringData(AV::AVDATA_PRODUCT_NAME, AV::Event_Threat_propProductName, m_spEventData);
        if(FAILED(hr))
        {
            CCTRACEW( CCTRCTX _T("Unable to create Product Name. hr=0x%08X"), hr);
        }

        // Product Version
        //
        hr = CreateStringData(AV::AVDATA_PRODUCT_VERSION, AV::Event_Threat_propProductVersion, m_spEventData);
        if(FAILED(hr))
        {
            CCTRACEW( CCTRCTX _T("Unable to create Product Version. hr=0x%08X"), hr);
        }

        // Feature name
        //
        hr = CreateNumberData(AV::AVDATA_COMPONENT_ID, (LONG)AV::Event_Threat_propFeature, m_spEventData);
        if(FAILED(hr))
        {
            CCTRACEW( CCTRCTX _T("Unable to create Component Id. hr=0x%08X"), hr);
        }

        // Feature version
        //
        hr = CreateStringData(AV::AVDATA_COMPONENT_VERSION, (LONG)AV::Event_Threat_propFeatureVersion, m_spEventData);
        if(FAILED(hr))
        {
            CCTRACEW( CCTRCTX _T("Unable to create Component Id. hr=0x%08X"), hr);
        }

        // Defs Revision - YYYYMMDDRRRR
        //
        hr = CreateStringData(AV::AVDATA_DEFINITIONS_VERSION, AV::Event_Threat_propDefsRevision, m_spEventData);
        if(FAILED(hr))
        {
            CCTRACEW( CCTRCTX _T("Unable to create defs version. hr=0x%08X"), hr);
        }

        long lTemp = 0;
        if(m_spEventData->GetData(AV::Event_Threat_propFeature, lTemp))
        {
            hr = CreateStringData(AV::AVDATA_COMPONENT_NAME, m_Features.GetFeatureName(lTemp));
            if(FAILED(hr))
            {
                CCTRACEW( CCTRCTX _T("Unable to create Component Name. hr=0x%08X"), hr);
            }
        }

        // Item type
        //
        lTemp = -1;
        m_spEventData->GetData(AV::Event_Threat_propObjectType, lTemp );
        switch (lTemp)
        {
        case AV::Event_Threat_ObjectType_File :
            cszTemp = CResourceHelper::LoadResourceStringW ( IDS_ITEM_TYPE_FILE, _Module.GetResourceInstance() ).c_str();
            break;
        case AV::Event_Threat_ObjectType_BootRecord :
            cszTemp = CResourceHelper::LoadResourceStringW ( IDS_ITEM_TYPE_BR, _Module.GetResourceInstance() ).c_str();
            break;
        case AV::Event_Threat_ObjectType_MasterBootRecord :
            cszTemp = CResourceHelper::LoadResourceStringW ( IDS_ITEM_TYPE_MBR, _Module.GetResourceInstance() ).c_str();
            break;
        case AV::Event_Threat_ObjectType_Memory :
            cszTemp = CResourceHelper::LoadResourceStringW ( IDS_ITEM_TYPE_MEMORY, _Module.GetResourceInstance() ).c_str();
            break;
        default:
            cszTemp = CResourceHelper::LoadResourceStringW ( IDS_ITEM_TYPE_UNKNOWN, _Module.GetResourceInstance() ).c_str();
            break;
        }
    
        hr = CreateNumberData(AV::AVDATA_OBJECT_TYPE_ID, (LONG)AV::Event_Threat_propObjectType, m_spEventData);
        if(FAILED(hr))
        {
            CCTRACEW( CCTRCTX _T("Unable to create object type. hr=0x%08X"), hr);
        }

        hr = CreateStringData(AV::AVDATA_OBJECT_TYPE_NAME, cszTemp);
        if(FAILED(hr))
        {
            CCTRACEW( CCTRCTX _T("Unable to create object type. hr=0x%08X"), hr);
        }


    }

    // 
    // END COMMON DATA
    // **********************************************

    // Loop through the sub files and make one event for each file/VID/action
    //
    long lSubFile = 0;

    // If this is a container scan, skip the first file. We don't use
    // the parent data in the logger.
    // 
    if ( AV::Event_Threat_Container == lSubType )
        lSubFile = 1;
    _ASSERTE(lNumSubFiles < 2);

    for (; lSubFile < lNumSubFiles; lSubFile++)
    {

        CManagedEventDataPtr spEventDataSubFile(new CManagedEventData);
        if ( !spEventDataAllFiles->GetNode ( lSubFile, *(spEventDataSubFile) ))
        {
            CCTRACEE (_T("CVirusAlertCategory::processLogEvents - error getting subfile"));
            continue;
        }

        // File path
        //
        hr = CreateStringData(AV::AVDATA_FILE_NAME, AV::Event_ThreatSubFile_propFileName, spEventDataSubFile);
        if(FAILED(hr))
        {
            CCTRACEW( CCTRCTX _T("Unable to create file path. hr=0x%08X"), hr);
        }

        // File description (for compressed files, etc.)
        //
        hr = CreateStringData(AV::AVDATA_FILE_DESC, AV::Event_ThreatSubFile_propFileDescription, spEventDataSubFile);
        if(FAILED(hr))
        {
            CCTRACEW( CCTRCTX _T("Unable to create file description. hr=0x%08X"), hr);
        }

        // Loop for each Threat Entry
        //

        CManagedEventDataPtr spEventAllThreats(new CManagedEventData);
        if ( spEventDataSubFile->GetNode ( AV::Event_ThreatSubFile_propThreatData, *(spEventAllThreats.m_p) ))
        {
            long lThreatCount = spEventAllThreats->GetCount();

            for ( long lThreatIndex = 0; lThreatIndex < lThreatCount; lThreatIndex++)
            {
                CManagedEventDataPtr spEventThreat(new CManagedEventData);
                if ( spEventAllThreats->GetNode ( lThreatIndex, *(spEventThreat.m_p) ))
                {
                    long lActionCount = 0;
                    spEventThreat->GetData ( AV::Event_ThreatEntry_propActionCount, lActionCount );

                    // Virus name
                    //
                    hr = CreateStringData(AV::AVDATA_RISK_NAME, AV::Event_ThreatEntry_propVirusName, spEventThreat);
                    if(FAILED(hr))
                    {
                        CCTRACEW( CCTRCTX _T("Unable to create risk name. hr=0x%08X"), hr);
                    }

                    // Threat category RAW
                    //
                    hr = CreateStringData(AV::AVDATA_RISK_CATEGORIES, AV::Event_ThreatEntry_propThreatCategories, spEventThreat);
                    if(FAILED(hr))
                    {
                        CCTRACEW( CCTRCTX _T("Unable to create threat category. hr=0x%08X"), hr);
                    }

                    // Threat category STRING
                    //
                    tstring strThreatCats;
                    if ( spEventThreat->GetData(AV::Event_ThreatEntry_propThreatCategories, strThreatCats ))
                    {
                        TCHAR szCategoryList[512] = {NULL};
                        m_ThreatInfo.GetCategoryText ( strThreatCats.c_str(), szCategoryList, 512 );
                        hr = CreateStringData(AV::AVDATA_RISK_CATEGORIES_STRING, CT2W(szCategoryList));
                        if(FAILED(hr))
                        {
                            CCTRACEW( CCTRCTX _T("Unable to create threat category string. hr=0x%08X"), hr);
                        }
                    }

                    // Virus ID link
                    //
                    // VID is actually an unsigned long which the CEventData class does not support
                    // natively, but some casting should be OK here to get back to the correct unsigned value
                    long lVirusID = 0;
                    if ( spEventThreat->GetData(AV::Event_ThreatEntry_propVirusID, lVirusID ) )
                    {
                        unsigned long ulVirusID = static_cast<unsigned long>(lVirusID);

						static ccLib::CStringW cszResponseLinkFmt;
						CString cszResponseLink;
						if (cszResponseLinkFmt.IsEmpty())
							CISVersion::LoadStringW(cszResponseLinkFmt, IDS_BRANDING_THREAT_INFO_URL);
                        cszResponseLink.Format(cszResponseLinkFmt, ulVirusID);
                        
                        hr = CreateStringData(AV::AVDATA_RESPONSE_THREATINFO_URL, cszResponseLink);
                        if(FAILED(hr))
                        {
                            CCTRACEW( CCTRCTX _T("Unable to create response URL. hr=0x%08X"), hr);
                        }
                    }

                    //
                    // End Threat Entry common data
                    // ********************************

                    long lStillPresent = 1;
                    spEventThreat->GetData(AV::Event_ThreatEntry_propStillPresent, lStillPresent);

                    ::std::vector<BYTE> vecActions;
                    vecActions.resize (lActionCount);
                    long lSize = lActionCount;
                    spEventThreat->GetData ( AV::Event_ThreatEntry_propActionData, &vecActions[0], lSize );
                    tstring strFinalAction;
					long lAction = 0;
                    for ( long lActionIndex = 0; lActionIndex < lActionCount; lActionIndex++)
                    {
                        // Action name
                        //
                        lAction = vecActions[lActionIndex];

                        // Use the ActionTaken map!
                        //
                        strFinalAction = CResourceHelper::LoadResourceStringW (ActionTaken.Names [lAction], _Module.GetResourceInstance());
                    
                    } // end action
					
					//
					// Set Status
					hr = CreateStringData(MCF::MCFDATA_STATUS, _S(IDS_MCFSTATUS_BLOCKED));  // since we're only handling blocked, then this will work
					if(FAILED(hr))
					{
						CCTRACEW( CCTRCTX _T("Unable to create status. hr=0x%08X"), hr);
					}

                    //
					// Set action taken
					hr = CreateStringData(AV::AVDATA_ACTION_TAKEN, CT2W(strFinalAction.c_str()));
                    if(FAILED(hr))
                    {
                        CCTRACEW( CCTRCTX _T("Unable to create final action. hr=0x%08X"), hr);
                    }
                    
                    tstring strThreatMatrixOverall;
                    bool bGetData = spEventThreat->GetData(AV::Event_ThreatEntry_propThreatMatrixOverall, strThreatMatrixOverall);
					if(!bGetData)
					{
                        CCTRACEW( CCTRCTX _T("Unable to get data of Event_ThreatEntry_propThreatMatrixOverall"));
                    }	

                    // Threat Matrix Overall
                    //
                    hr = CreateNumberData(AV::AVDATA_THREAT_MATRIX_OVERALL, (DWORD)_ttoi(strThreatMatrixOverall.c_str()));
                    if(FAILED(hr))
                    {
                        CCTRACEW( CCTRCTX _T("Unable to create threat matrix overall. hr=0x%08X"), hr);
                    }

                    // Performance
                    hr = CreateStringData(AV::AVDATA_THREAT_MATRIX_PERFORMANCE, AV::Event_ThreatEntry_propThreatMatrixPerformance, spEventThreat);
                    if(FAILED(hr))
                    {
                        CCTRACEW( CCTRCTX _T("Unable to create threat matrix performance. hr=0x%08X"), hr);
                    }

                    // Privacy
                    hr = CreateStringData(AV::AVDATA_THREAT_MATRIX_PRIVACY, AV::Event_ThreatEntry_propThreatMatrixPrivacy, spEventThreat);
                    if(FAILED(hr))
                    {
                        CCTRACEW( CCTRCTX _T("Unable to create threat matrix privacy. hr=0x%08X"), hr);
                    }

                    // Removal
                    hr = CreateStringData(AV::AVDATA_THREAT_MATRIX_REMOVAL, AV::Event_ThreatEntry_propThreatMatrixRemoval, spEventThreat);
                    if(FAILED(hr))
                    {
                        CCTRACEW( CCTRCTX _T("Unable to create threat matrix removal. hr=0x%08X"), hr);
                    }

                    // Stealth
                    hr = CreateStringData(AV::AVDATA_THREAT_MATRIX_STEALTH, AV::Event_ThreatEntry_propThreatMatrixStealth, spEventThreat);
                    if(FAILED(hr))
                    {
                        CCTRACEW( CCTRCTX _T("Unable to create threat matrix stealth. hr=0x%08X"), hr);
                    }
                    
                    LPTSTR szStop = NULL;
                    DWORD dwOverallThreatMatrix = _tcstol(strThreatMatrixOverall.c_str(), &szStop, 10);
                    
                    if(0 == lStillPresent)
                    {
                        hr = CreateNumberData(MCF::MCFDATA_PRIORITY, (LONGLONG)MCF::IMessage::eLowPriority);
                    }

                    if(0 == dwOverallThreatMatrix)
                    {
                        //  Set Risk Level
                        hr = CreateNumberData(MCF::MCFDATA_RISKLEVEL, (LONGLONG)MCF::IMessage::eLowRisk);
                        hr = CreateStringData(MCF::MCFDATA_RECOMMENDED_ACTION, _S(IDS_RECOMMENDED_ACTION_DO_NOTHING));

                        // Set priority
                        if(0 != lStillPresent)
                        {
                            hr = CreateNumberData(MCF::MCFDATA_PRIORITY, (LONGLONG)MCF::IMessage::eMediumPriority);
                        }

                    }
                    else if(1 == dwOverallThreatMatrix)
                    {
                        //  Set Risk Level
                        hr = CreateNumberData(MCF::MCFDATA_RISKLEVEL, (LONGLONG)MCF::IMessage::eMediumRisk);

                        // Set priority
                        if(0 != lStillPresent)
                        {
                            hr = CreateNumberData(MCF::MCFDATA_PRIORITY, (LONGLONG)MCF::IMessage::eHighPriority);
                            hr = CreateStringData(MCF::MCFDATA_RECOMMENDED_ACTION, _S(IDS_RECOMMENDED_ACTION_RUN_FSS_SCAN));
                        }
                        else
                        {
                            hr = CreateStringData(MCF::MCFDATA_RECOMMENDED_ACTION, _S(IDS_RECOMMENDED_ACTION_DO_NOTHING));
                        }
                        
                    }
                    else if(2 == dwOverallThreatMatrix)
                    {
                        //  Set Risk Level
                        hr = CreateNumberData(MCF::MCFDATA_RISKLEVEL, (LONGLONG)MCF::IMessage::eHighRisk);

                        // Set priority
                        if(0 != lStillPresent)
                        {
                            hr = CreateNumberData(MCF::MCFDATA_PRIORITY, (LONGLONG)MCF::IMessage::eHighPriority);
                            hr = CreateStringData(MCF::MCFDATA_RECOMMENDED_ACTION, _S(IDS_RECOMMENDED_ACTION_RUN_FSS_SCAN));
                        }
                        else
                        {
                            hr = CreateStringData(MCF::MCFDATA_RECOMMENDED_ACTION, _S(IDS_RECOMMENDED_ACTION_DO_NOTHING));
                        }

                    }
                    else
                    {
                        //  Set Risk Level
                        hr = CreateNumberData(MCF::MCFDATA_RISKLEVEL, (LONGLONG)MCF::IMessage::eInvalidRiskLevel);

                        // Set priority
                        if(0 != lStillPresent)
                        {
                            hr = CreateNumberData(MCF::MCFDATA_PRIORITY, (LONGLONG)MCF::IMessage::eInvalidPriority);
                            hr = CreateStringData(MCF::MCFDATA_RECOMMENDED_ACTION, _S(IDS_RECOMMENDED_ACTION_RUN_FSS_SCAN));
                        }
                        else
                        {
                            hr = CreateStringData(MCF::MCFDATA_RECOMMENDED_ACTION, _S(IDS_RECOMMENDED_ACTION_DO_NOTHING));
                        }

                    }


                } // end threat

            }// end all theats

        } // end subfiles
    }
    
    CreateNumberData(AV::AVDATA_SUPPORTS_HELP_AND_SUPPORT, (LONGLONG)1);
    CreateNumberData(AV::AVDATA_SUPPORTS_OPTIONS, CAntivirusMessageBase::m_AccessControl.HasAdminPrivilege() ? (LONGLONG)1 : (LONGLONG)0);
    CreateNumberData(AV::AVDATA_HAS_NO_ACTIONS, (LONGLONG)1);

    return S_OK;
};

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CScanResultsMessage::CScanResultsMessage(void)
{
}

CScanResultsMessage::~CScanResultsMessage(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CScanResultsMessage::Initialize(REFGUID guidProviderId, CNAVEventCommon*& spNavEventCommon, const SYSTEMTIME& stTimestamp)
{
    HRESULT hrReturn = MCF::S_NORESULTS;
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        m_spNavEventCommon = spNavEventCommon;
        m_spEventData = new CManagedEventData(&(spNavEventCommon->props));

        hrx << CreateCategoryList();

        //
        //  Set Priority
        hrx << CreateNumberData(MCF::MCFDATA_PRIORITY, (LONGLONG)MCF::IMessage::eLowPriority);

        //  Set Risk Level
        hrx << CreateNumberData(MCF::MCFDATA_RISKLEVEL, (LONGLONG)MCF::IMessage::eLowRisk);

        // Set Event Type
        hrx << CreateNumberData(AV::AVDATA_MESSAGE_TYPE, (LONGLONG)CAntivirusMessageBase::eManualScanResults);

        // Set Recommended Action
        hrx << CreateStringData(MCF::MCFDATA_RECOMMENDED_ACTION, _S(IDS_RECOMMENDED_ACTION_DO_NOTHING));

        //
        // Set Provider Id
        memcpy(&m_guidProviderId, &guidProviderId, sizeof(GUID));

        //
        // Set Timestamp
        hrx << CreateTimeData(MCF::MCFDATA_TIMESTAMP, stTimestamp);

        //
        // Populate Data
        hrx << GetScanData();

        //
        // Populate Short Description
        hrx << CreateShortDescription(MCF::MCFDATA_DESCRIPTION_SHORT);

        hrReturn = S_OK;
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanResultsMessage::CreateShortDescription(const SYMGUID &guidDataId)
{
    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
		static WCHAR szTemp [1024] = {0}; // Once string is loaded this is const so we avoid reloading.
		if (0 == *szTemp)
            ::LoadStringW(_Module.GetResourceInstance(), IDS_FMTTEXT_RESULTS, szTemp, sizeof(szTemp)/sizeof(WCHAR));
        
        CString cszDescription;
		CFormatDataToString::DataToString(this, AV::AVDATA_SCANRESULTS_TASK_NAME, szTemp, cszDescription);

        hrx << CreateStringData(guidDataId, cszDescription);
        hrReturn = S_OK;
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanResultsMessage::CreateCategoryList()
{
    if(!m_spNavEventCommon)
        return E_UNEXPECTED;

    HRESULT hrReturn = E_FAIL; 
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        MCF::ICategoryInternalPtr spCategory;
        MCF::ICategoryListInternalPtr spCategoryList;
        hrx << MCF::CStockCategoryList::CreateObject(spCategoryList);

        hrx << MCF::CStockCategory::CreateObject(spCategory);
        hrx << spCategory->SetCategoryId(MCF_CATEGORY_SCANNER_RESULTS);

		static WCHAR szTemp [1024] = {0}; // Once string is loaded this is const so we avoid reloading.
		if (0 == *szTemp)
            ::LoadStringW ( _Module.GetResourceInstance(), IDS_MCFCATEGORYDESC_SCAN_RESULT, szTemp, sizeof(szTemp)/sizeof(WCHAR));

        hrx << spCategory->SetDescription(szTemp, NULL);
        hrx << spCategoryList->Add(spCategory);
        hrx << SetCategories(spCategoryList);

        hrReturn = S_OK;
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CScanResultsMessage::GetScanData()
{
    // Is this a supported type?
    //
    long lType = 0;
    m_spEventData->GetData(AV::Event_Base_propType, lType );
    if ( lType != AV::Event_ID_ScanAction )
        return E_INVALIDARG;


    // Get message GUID
    GUID msgGUID;
    long len = sizeof(GUID);
    if(!m_spEventData->GetData(AV::Event_Base_propUniqueID, (BYTE*)&msgGUID, len))
    {
        CCTRCTXE0(_T("unable to get event GUID"));
        return E_INVALIDARG;
    }
    HRESULT hr = SetMessageId(msgGUID);
    if(FAILED(hr))
    {
        CCTRCTXE1(_T("unable to set event GUID:0x%08x"), hr);
        return hr;
    }

    // User Name
    //
    hr = CreateStringData(AV::AVDATA_USER_NAME, AV::Event_Base_propUserName, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create User Name. hr=0x%08X"), hr);
    }

    // Product Name
    //
    hr = CreateStringData(AV::AVDATA_PRODUCT_NAME, AV::Event_ScanAction_propProductName, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create Product Name. hr=0x%08X"), hr);
    }

    // Product Version
    //
    hr = CreateStringData(AV::AVDATA_PRODUCT_VERSION, AV::Event_ScanAction_propProductVersion, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create Product Version. hr=0x%08X"), hr);
    }

    // Feature name
    //
    hr = CreateNumberData(AV::AVDATA_COMPONENT_ID, (LONG)AV::Event_ScanAction_propFeature, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create Component Id. hr=0x%08X"), hr);
    }

    // Feature version
    //
    hr = CreateStringData(AV::AVDATA_COMPONENT_VERSION, (LONG)AV::Event_ScanAction_propFeatureVersion, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create Component Id. hr=0x%08X"), hr);
    }

    // Defs Revision - YYYYMMDDRRRR
    //
    hr = CreateStringData(AV::AVDATA_DEFINITIONS_VERSION, AV::Event_ScanAction_propDefsRevision, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create defs version. hr=0x%08X"), hr);
    }

    long lTemp = 0;
    if(m_spEventData->GetData(AV::Event_ScanAction_propFeature, lTemp))
    {
        hr = CreateStringData(AV::AVDATA_COMPONENT_NAME, m_Features.GetFeatureName(lTemp));
        if(FAILED(hr))
        {
            CCTRACEW( CCTRCTX _T("Unable to create Component Name. hr=0x%08X"), hr);
        }
    }

    //
    // Set Status
    if(m_spEventData->GetData(AV::Event_ScanAction_propAction, lTemp))
    {
        UINT nStatusTextId = IDS_MCFSTATUS_UNKNOWN;
        
        if(AV::Event_ScanAction_ScanCompleted == lTemp)
            nStatusTextId = IDS_MCFSTATUS_COMPLETED;
        else if (AV::Event_ScanAction_ScanAborted == lTemp)
            nStatusTextId = IDS_MCFSTATUS_ABORTED;

        hr = CreateStringData(MCF::MCFDATA_STATUS, _S(nStatusTextId));
        if(FAILED(hr))
        {
            CCTRACEW( CCTRCTX _T("Unable to set status. hr=0x%08X"), hr);
        }
    }


    // Task Name
    //
    hr = CreateStringData(AV::AVDATA_SCANRESULTS_TASK_NAME, (LONG)AV::Event_ScanAction_propTaskName, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create task name. hr=0x%08X"), hr);
    }

    // MBRs Repaired
    //
    hr = CreateNumberData(AV::AVDATA_SCANRESULTS_MBRs_REPAIRED, (LONG)AV::Event_ScanAction_MBRsRepaired, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create MBRs repaired. hr=0x%08X"), hr);
    }

    // Total MBRs Infected
    //
    hr = CreateNumberData(AV::AVDATA_SCANRESULTS_MBRs_TOTAL_INFECTED, (LONG)AV::Event_ScanAction_MBRsTotalInfected, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create total MBRs Infected. hr=0x%08X"), hr);
    }

    // Total MBRs Scanned
    //
    hr = CreateNumberData(AV::AVDATA_SCANRESULTS_MBRs_TOTAL_SCANNED, (LONG)AV::Event_ScanAction_MBRsTotalScanned, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create total MBRs scanned. hr=0x%08X"), hr);
    }

    // Total Boot Records Repaired
    //
    hr = CreateNumberData(AV::AVDATA_SCANRESULTS_BOOT_RECS_REPAIRED, (LONG)AV::Event_ScanAction_BootRecsRepaired, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create total boot records repaired. hr=0x%08X"), hr);
    }

    // Total Boot Records Infected
    //
    hr = CreateNumberData(AV::AVDATA_SCANRESULTS_BOOT_RECS_TOTAL_INFECTED, (LONG)AV::Event_ScanAction_BootRecsTotalInfected, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create total boot records repaired. hr=0x%08X"), hr);
    }

    // Total Boot Records Scanned
    //
    hr = CreateNumberData(AV::AVDATA_SCANRESULTS_BOOT_RECS_TOTAL_SCANNED, (LONG)AV::Event_ScanAction_BootRecsTotalScanned, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create total boot records scanned. hr=0x%08X"), hr);
    }

    // Total Files repaired
    //
    hr = CreateNumberData(AV::AVDATA_SCANRESULTS_FILES_REPAIRED, (LONG)AV::Event_ScanAction_FilesRepaired, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create total files repaired. hr=0x%08X"), hr);
    }
        
    // Total Files quarantined
    //
    hr = CreateNumberData(AV::AVDATA_SCANRESULTS_FILES_QUARANTINED, (LONG)AV::Event_ScanAction_FilesQuarantined, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create total files quarantined. hr=0x%08X"), hr);
    }
             
    // Total Files deleted
    //
    hr = CreateNumberData(AV::AVDATA_SCANRESULTS_FILES_DELETED, (LONG)AV::Event_ScanAction_FilesDeleted, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create total files deleted. hr=0x%08X"), hr);
    }
        
    // Total Files infected
    //
    hr = CreateNumberData(AV::AVDATA_SCANRESULTS_FILES_TOTAL_INFECTED, (LONG)AV::Event_ScanAction_FilesTotalInfected, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create total files infected. hr=0x%08X"), hr);
    }
        
    // Total Files scanned
    //
    hr = CreateNumberData(AV::AVDATA_SCANRESULTS_FILES_TOTAL_SCANNED, (LONG)AV::Event_ScanAction_FilesTotalScanned, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create total files scanned. hr=0x%08X"), hr);
    }
                
    // Total Files excluded
    //
    hr = CreateNumberData(AV::AVDATA_SCANRESULTS_FILES_EXCLUDED, (LONG)AV::Event_ScanAction_FilesExcluded, m_spEventData);
    if(FAILED(hr))
    {
        CCTRACEW( CCTRCTX _T("Unable to create total files excluded. hr=0x%08X"), hr);
    }
             
	// Viruses detected
	//
	hr = CreateNumberData(AV::AVDATA_SCANRESULTS_VIRUSES_DETECTED, (LONG)AV::Event_ScanAction_VirusesDetected, m_spEventData);
	if(FAILED(hr))
	{
		CCTRACEW( CCTRCTX _T("Unable to create total viruses detected. hr=0x%08X"), hr);
	}

	// Viruses removed
	//
	hr = CreateNumberData(AV::AVDATA_SCANRESULTS_VIRUSES_REMOVED, (LONG)AV::Event_ScanAction_VirusesRemoved, m_spEventData);
	if(FAILED(hr))
	{
		CCTRACEW( CCTRCTX _T("Unable to create total viruses removed. hr=0x%08X"), hr);
	}

	// NonViruses Detected
	//
	hr = CreateNumberData(AV::AVDATA_SCANRESULTS_NONVIRUSES_DETECTED, (LONG)AV::Event_ScanAction_NonVirusesDetected, m_spEventData);
	if(FAILED(hr))
	{
		CCTRACEW( CCTRCTX _T("Unable to create total non-viruses detected. hr=0x%08X"), hr);
	}

	// NonViruses removed
	//
	hr = CreateNumberData(AV::AVDATA_SCANRESULTS_NONVIRUSES_REMOVED, (LONG)AV::Event_ScanAction_NonVirusesRemoved, m_spEventData);
	if(FAILED(hr))
	{
		CCTRACEW( CCTRCTX _T("Unable to create total non-viruses removed. hr=0x%08X"), hr);
	}

	// Total items scanned
	//
	hr = CreateNumberData(AV::AVDATA_SCANRESULTS_ITEMS_TOTAL_SCANNED, (LONG)AV::Event_ScanAction_ItemTotalScanned, m_spEventData);
	if(FAILED(hr))
	{
		CCTRACEW( CCTRCTX _T("Unable to create total items scanned. hr=0x%08X"), hr);
	}

	// Total Scan Time
	//
	hr = CreateTimeData(AV::AVDATA_SCANRESULTS_SCAN_TIME, (LONG)AV::Event_ScanAction_ScanTime, m_spEventData);
	if(FAILED(hr))
	{
		CCTRACEW( CCTRCTX _T("Unable to create total scan time. hr=0x%08X"), hr);
	}
    
    { // intentionally scoped
        // Eraser Stats
        // 
        GUID_DWORD eraserStatsList[] = { AV::AVDATA_ERASERSTAT_REGISTRY_DETECTION_ACTION, ccEraser::RegistryDetectionActionType, 
            AV::AVDATA_ERASERSTAT_FILE_DETECTION_ACTION, ccEraser::FileDetectionActionType, 
            AV::AVDATA_ERASERSTAT_PROCESS_DETECTION_ACTION, ccEraser::ProcessDetectionActionType, 
            AV::AVDATA_ERASERSTAT_BATCH_DETECTION_ACTION, ccEraser::BatchDetectionActionType, 
            AV::AVDATA_ERASERSTAT_INI_DETECTION_ACTION, ccEraser::INIDetectionActionType, 
            AV::AVDATA_ERASERSTAT_SERVICE_DETECTION_ACTION, ccEraser::ServiceDetectionActionType, 
            AV::AVDATA_ERASERSTAT_STARTUP_DETECTION_ACTION, ccEraser::StartupDetectionActionType, 
            AV::AVDATA_ERASERSTAT_COM_DETECTION_ACTION, ccEraser::COMDetectionActionType, 
            AV::AVDATA_ERASERSTAT_HOSTS_DETECTION_ACTION, ccEraser::HostsDetectionActionType,
            AV::AVDATA_ERASERSTAT_DIRECTORY_DETECTION_ACTION, ccEraser::DirectoryDetectionActionType,
            AV::AVDATA_ERASERSTAT_LSP_DETECTION_ACTION, ccEraser::LSPDetectionActionType,
            AV::AVDATA_ERASERSTAT_BROWSER_CACHE_DETECTION_ACTION, ccEraser::BrowserCacheDetectionActionType,
            AV::AVDATA_ERASERSTAT_COOKIE_DETECTION_ACTION, ccEraser::CookieDetectionActionType,
            AV::AVDATA_ERASERSTAT_APPHEURISTIC_DETECTION_ACTION, ccEraser::ApplicationHeuristicDetectionActionType };

        CEventData AvDataEraserStats;
        m_spEventData->GetNode(AV::Event_ScanAction_EraserScanResults, AvDataEraserStats);
        m_spEraserStats = new CManagedEventData(&AvDataEraserStats);
        if(m_spEraserStats)
        {
            DWORD dwIndex = 0, dwCount = _countof(eraserStatsList);
            for(DWORD dwIndex=0; dwIndex < dwCount; dwIndex++)
            {
                CreateNumberData(eraserStatsList[dwIndex].guidDataId, eraserStatsList[dwIndex].lAvEventId, m_spEraserStats);
            }
        }
    }

    { // intentionally scoped
        // Resolved by Category
        // 
        GUID_DWORD ResolvedByCategoryList[] = { AV::AVDATA_RESOLVEDSTATS_VIRAL, ccEraser::IAnomaly::Viral, 
            AV::AVDATA_RESOLVEDSTATS_MALICIOUS, ccEraser::IAnomaly::Malicious,
            AV::AVDATA_RESOLVEDSTATS_RESERVED_MALICIOUS, ccEraser::IAnomaly::ReservedMalicious,
            AV::AVDATA_RESOLVEDSTATS_HEURISTIC, ccEraser::IAnomaly::Heuristic,
            AV::AVDATA_RESOLVEDSTATS_SECURITY_RISK, ccEraser::IAnomaly::SecurityRisk,
            AV::AVDATA_RESOLVEDSTATS_HACKTOOL, ccEraser::IAnomaly::Hacktool,
            AV::AVDATA_RESOLVEDSTATS_SPYWARE, ccEraser::IAnomaly::SpyWare,
            AV::AVDATA_RESOLVEDSTATS_TRACKWARE, ccEraser::IAnomaly::Trackware,
            AV::AVDATA_RESOLVEDSTATS_DIALER, ccEraser::IAnomaly::Dialer,
            AV::AVDATA_RESOLVEDSTATS_REMOTE_ACCESS, ccEraser::IAnomaly::RemoteAccess,
            AV::AVDATA_RESOLVEDSTATS_ADWARE, ccEraser::IAnomaly::Adware,
            AV::AVDATA_RESOLVEDSTATS_JOKE, ccEraser::IAnomaly::Joke,
            AV::AVDATA_RESOLVEDSTATS_CLIENT_COMPLIANCY, ccEraser::IAnomaly::ClientCompliancy,
            AV::AVDATA_RESOLVEDSTATS_GLP, ccEraser::IAnomaly::GenericLoadPoint,
            AV::AVDATA_RESOLVEDSTATS_APPHEURISTIC, ccEraser::IAnomaly::ApplicationHeuristic,
            AV::AVDATA_RESOLVEDSTATS_COOKIE, ccEraser::IAnomaly::Cookie };

        CEventData AvDataResolvedByCategory;
        m_spEventData->GetNode(AV::Event_ScanAction_ResolvedByCategory, AvDataResolvedByCategory);
        m_spResolvedByCategory = new CManagedEventData(&AvDataResolvedByCategory);
        if(m_spResolvedByCategory)
        {
            DWORD dwIndex = 0, dwCount = _countof(ResolvedByCategoryList);
            for(DWORD dwIndex=0; dwIndex < dwCount; dwIndex++)
            {
                CreateNumberData(ResolvedByCategoryList[dwIndex].guidDataId, ResolvedByCategoryList[dwIndex].lAvEventId, m_spResolvedByCategory);
            }
        }
    }

    { // intentionally scoped
        // Unresolved by Category
        // 
        GUID_DWORD UnresolvedByCategoryList[] = { AV::AVDATA_UNRESOLVEDSTATS_VIRAL, ccEraser::IAnomaly::Viral, 
            AV::AVDATA_UNRESOLVEDSTATS_MALICIOUS, ccEraser::IAnomaly::Malicious,
            AV::AVDATA_UNRESOLVEDSTATS_RESERVED_MALICIOUS, ccEraser::IAnomaly::ReservedMalicious,
            AV::AVDATA_UNRESOLVEDSTATS_HEURISTIC, ccEraser::IAnomaly::Heuristic,
            AV::AVDATA_UNRESOLVEDSTATS_SECURITY_RISK, ccEraser::IAnomaly::SecurityRisk,
            AV::AVDATA_UNRESOLVEDSTATS_HACKTOOL, ccEraser::IAnomaly::Hacktool,
            AV::AVDATA_UNRESOLVEDSTATS_SPYWARE, ccEraser::IAnomaly::SpyWare,
            AV::AVDATA_UNRESOLVEDSTATS_TRACKWARE, ccEraser::IAnomaly::Trackware,
            AV::AVDATA_UNRESOLVEDSTATS_DIALER, ccEraser::IAnomaly::Dialer,
            AV::AVDATA_UNRESOLVEDSTATS_REMOTE_ACCESS, ccEraser::IAnomaly::RemoteAccess,
            AV::AVDATA_UNRESOLVEDSTATS_ADWARE, ccEraser::IAnomaly::Adware,
            AV::AVDATA_UNRESOLVEDSTATS_JOKE, ccEraser::IAnomaly::Joke,
            AV::AVDATA_UNRESOLVEDSTATS_CLIENT_COMPLIANCY, ccEraser::IAnomaly::ClientCompliancy,
            AV::AVDATA_UNRESOLVEDSTATS_GLP, ccEraser::IAnomaly::GenericLoadPoint,
            AV::AVDATA_UNRESOLVEDSTATS_APPHEURISTIC, ccEraser::IAnomaly::ApplicationHeuristic,
            AV::AVDATA_UNRESOLVEDSTATS_COOKIE, ccEraser::IAnomaly::Cookie };

        CEventData AvDataUnresolvedByCategory;
        m_spEventData->GetNode(AV::Event_ScanAction_UnResolvedByCategory, AvDataUnresolvedByCategory);
        m_spUnresolvedByCategory = new CManagedEventData(&AvDataUnresolvedByCategory);
        if(m_spUnresolvedByCategory)
        {
            DWORD dwIndex = 0, dwCount = _countof(UnresolvedByCategoryList);
            for(DWORD dwIndex=0; dwIndex < dwCount; dwIndex++)
            {
                CreateNumberData(UnresolvedByCategoryList[dwIndex].guidDataId, UnresolvedByCategoryList[dwIndex].lAvEventId, m_spUnresolvedByCategory);
            }
        }
    }

    CreateNumberData(AV::AVDATA_SUPPORTS_HELP_AND_SUPPORT, (LONGLONG)1);
    CreateNumberData(AV::AVDATA_SUPPORTS_OPTIONS, CAntivirusMessageBase::m_AccessControl.HasAdminPrivilege() ? (LONGLONG)1 : (LONGLONG)0);
    CreateNumberData(AV::AVDATA_HAS_NO_ACTIONS, (LONGLONG)1);

    return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CAvModuleItemMessage::CAvModuleItemMessage(void)
{
}

CAvModuleItemMessage::~CAvModuleItemMessage(void)
{
}

//****************************************************************************
//****************************************************************************
HRESULT CAvModuleItemMessage::Initialize(REFGUID guidProviderId, AVModule::IAVMapDwordData* pThreatData)
{
    HRESULT hrReturn = MCF::S_NORESULTS;
    STAHLSOFT_HRX_TRY_NO_DECLARE_HR
    {
        m_spThreatData = pThreatData;

        //
        // Populate Data
        hrx << GetQuarantineData();

        // Set Event Type
        hrx << CreateNumberData(AV::AVDATA_MESSAGE_TYPE, (LONGLONG)CAntivirusMessageBase::eAvModuleItem);

        //
        // Set Provider Id
        memcpy(&m_guidProviderId, &guidProviderId, sizeof(GUID));

        // Set message GUID
        GUID msgGUID;
        hrx << m_spThreatData->GetValue(AVModule::ThreatTracking::TrackingData_THREATID, msgGUID);
        SetMessageId(msgGUID);

        hrReturn = S_OK;
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hrReturn);

    return hrReturn;
}

//****************************************************************************
//****************************************************************************
HRESULT CAvModuleItemMessage::GetQuarantineData()
{
    
    HRESULT hr;
	CCalculateThreatData cThreatDataHelper(m_spThreatData);

    CreateStringData(AV::AVDATA_THREATTRACK_ID, cThreatDataHelper.GetThreatTrackIdDisplayString());
	CreateStringData(AV::AVDATA_USER_SID_STRING, AVModule::ThreatTracking::TrackingData_SID, m_spThreatData);
	CreateNumberData(AV::AVDATA_COMPONENT_ID, (LONGLONG)cThreatDataHelper.GetComponentId());
    CreateStringData(AV::AVDATA_COMPONENT_NAME, cThreatDataHelper.GetComponentDisplayName());
    
    CreateNumberData(AV::AVDATA_SHOW_AUTOPROTECT_OPTIONS, (DWORD)cThreatDataHelper.AllowShowAutoProtectOptions());
    CreateNumberData(AV::AVDATA_SHOW_MANUAL_SCAN_OPTIONS, (DWORD)cThreatDataHelper.AllowShowScanOptions());
    CreateNumberData(AV::AVDATA_SHOW_EMAIL_SCAN_OPTIONS,  (DWORD)cThreatDataHelper.AllowShowEmailOptions());
    CreateNumberData(AV::AVDATA_SHOW_EXCLUSION_OPTIONS,   (DWORD)cThreatDataHelper.AllowShowExcludeOptions());

    CreateNumberData(AV::AVDATA_OBJECT_TYPE_ID, AVModule::ThreatTracking::TrackingData_THREAT_TYPE, m_spThreatData);
    CreateStringData(AV::AVDATA_OBJECT_TYPE_NAME, cThreatDataHelper.GetThreatTypeDisplayName());

    CreateStringData(AV::AVDATA_THREAT_STATE_NAME, cThreatDataHelper.GetThreatStateDisplayString());

    CreateStringData(AV::AVDATA_DEFINITIONS_VERSION, AVModule::ThreatTracking::TrackingData_VirusDefsVersion, m_spThreatData);
    CreateStringData(AV::AVDATA_ERASER_VERSION, AVModule::ThreatTracking::TrackingData_EraserEngVersion, m_spThreatData);

    CreateStringData(AV::AVDATA_RISK_NAME, AVModule::ThreatTracking::TrackingData_NAME, m_spThreatData);
    CreateNumberData(AV::AVDATA_THREAT_MATRIX_OVERALL, AVModule::ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, m_spThreatData);
	
	if(cThreatDataHelper.HasDependencyFlag())
		CreateNumberData(AV::AVDATA_THREAT_DEPENDENCIES, (DWORD)1);
    
    if(cThreatDataHelper.AllowShowFileList() || cThreatDataHelper.AllowShowViewDetails())
    {
        CreateStringData(AV::AVDATA_EMAIL_SUBJECT, AVModule::ThreatTracking::TrackingData_Email_Subject, m_spThreatData);
        CreateStringData(AV::AVDATA_EMAIL_SENDER, AVModule::ThreatTracking::TrackingData_Email_Sender, m_spThreatData);
        CreateStringData(AV::AVDATA_EMAIL_RECIPIENT, AVModule::ThreatTracking::TrackingData_Email_Recipient, m_spThreatData);
    }
	
	CreateStringData(AV::AVDATA_THREAT_MATRIX_OVERALL_STRING, cThreatDataHelper.GetRiskLevelOverallDisplayString());

	CreateTimeData(MCF::MCFDATA_TIMESTAMP, AVModule::ThreatTracking::TrackingData_DETECTION_TIME, m_spThreatData);

	CreateStringData(AV::AVDATA_RISK_CATEGORIES_STRING, cThreatDataHelper.GetThreatCatDisplayString());
	CreateStringData(MCF::MCFDATA_DESCRIPTION_SHORT, cThreatDataHelper.GetShortDescription());

	CreateNumberData(MCF::MCFDATA_RISKLEVEL, (LONGLONG)cThreatDataHelper.GetMCFRiskLevel());
	CreateStringData(MCF::MCFDATA_RECOMMENDED_ACTION, cThreatDataHelper.GetRecommendedActionDisplayString());
	CreateStringData(MCF::MCFDATA_STATUS, cThreatDataHelper.GetMCFStatusDisplayString());
	CreateNumberData(MCF::MCFDATA_PRIORITY, (LONGLONG)cThreatDataHelper.GetMCFPriority());

	CreateNumberData(AV::AVDATA_SUPPORTS_DELETE_ITEM, (DWORD)cThreatDataHelper.AllowShowDeleteItem());
	CreateNumberData(AV::AVDATA_SUPPORTS_DELETE_DATA, (DWORD)cThreatDataHelper.AllowShowDeleteData());
	CreateNumberData(AV::AVDATA_SUPPORTS_RESTORE,     (DWORD)cThreatDataHelper.AllowShowRestoreItem());
	CreateNumberData(AV::AVDATA_SUPPORTS_SUBMISSION,  (DWORD)cThreatDataHelper.AllowShowSubmitItem());

	CreateNumberData(AV::AVDATA_SUPPORTS_REMOVENOW,         (DWORD)cThreatDataHelper.AllowShowRemoveNow());
	CreateNumberData(AV::AVDATA_SUPPORTS_QUICKSCAN,   (DWORD)cThreatDataHelper.AllowShowQuickScan());
	CreateNumberData(AV::AVDATA_SUPPORTS_EXCLUDE,     (DWORD)cThreatDataHelper.AllowShowExcludeItem());
	CreateNumberData(AV::AVDATA_SUPPORTS_VIEW_DETAILS,(DWORD)cThreatDataHelper.AllowShowViewDetails());
	CreateNumberData(AV::AVDATA_SUPPORTS_REVIEW_RISK_DETAILS,(DWORD)cThreatDataHelper.AllowShowReviewRiskDetails());
	CreateNumberData(AV::AVDATA_RECOMMENDED_ACTION,   (DWORD)cThreatDataHelper.GetRecommendedActionId());
    CreateNumberData(AV::AVDATA_SUPPORTS_HELP_AND_SUPPORT, (LONGLONG)1);
    CreateNumberData(AV::AVDATA_SUPPORTS_OPTIONS, (DWORD)cThreatDataHelper.AllowShowOptionsSection());
    CreateNumberData(AV::AVDATA_HAS_NO_ACTIONS, (DWORD)cThreatDataHelper.ThreatHasNoActions());
    
    if(cThreatDataHelper.ThreatIsAppHeuristic())
        CreateStringData(AV::AVDATA_HEURISTIC_RANK_STRING, cThreatDataHelper.GetHeuristicRankDisplayString());

	if(cThreatDataHelper.AllowShowFileList())
	{
		do
		{
			ISymBasePtr spTempObj;
			hr = m_spThreatData->GetValue(AVModule::ThreatTracking::TrackingData_Compressed_COMPONENTS, AVModule::AVDataTypes::eTypeArray, spTempObj);
			if(FAILED(hr) || !spTempObj)
			{
				CCTRACEE( CCTRCTX _T("Problem getting compressed components array. hr=0x%08X"), hr);
				break;
			}

			AVModule::IAVArrayDataQIPtr spCompressedComponents = spTempObj;
			DWORD dwArraySize = NULL, dwArrayIndex = NULL;
			hr = spCompressedComponents->GetCount(dwArraySize);
			if(FAILED(hr) || !dwArraySize)
			{
				CCTRACEE( CCTRCTX _T("Problem getting compressed components array size. hr=0x%08X, dwArraySize=%d"), hr, dwArraySize);
				break;
			}

			IStringListDataPtr spStringList;
			hr = CStockStringListData::CreateObject(spStringList);
			if(FAILED(hr) || !spStringList)
			{
				CCTRACEE( CCTRCTX _T("Problem creating string list. hr = 0x%08X"), hr);
				break;
			}

			MCF::IDataInternalQIPtr spDataInternal = spStringList;
			if(!spDataInternal)
			{
				CCTRACEE( CCTRCTX _T("Problem getting string list internal data."));
				break;
			}
			
			spDataInternal->SetID(AV::AVDATA_FILE_LIST);

			CString cszContainerString;
			for(dwArrayIndex = 0; dwArrayIndex < dwArraySize; dwArrayIndex++)
			{
				cc::IStringPtr spComponentPath;
				hr = spCompressedComponents->GetValue(dwArrayIndex, spComponentPath);
				if(FAILED(hr) || !spComponentPath)
				{
					CCTRACEE( CCTRCTX _T("Problem getting string from array. dwArrayIndex=%d, hr = 0x%08X"), dwArrayIndex, hr);
					continue;
				}

				if(1 == dwArraySize)
				{
					cszContainerString = CW2T(spComponentPath->GetStringW());
					break;
				}

				CString cszTempFileName;
				cszTempFileName.Format(_S(IDS_FMT_COMPRESSED_FILE), CW2T(spComponentPath->GetStringW()));

				if(!cszContainerString.GetLength())
				{
					cszContainerString = cszTempFileName;
					continue;
				}

				cszContainerString += _S(IDS_COMPRESSED_COMPONENT);
				cszContainerString += cszTempFileName;
			}

			spStringList->AddString(cszContainerString);

			ui::IDataQIPtr spData = spStringList;
			if(!spData)
			{
				CCTRACEE( CCTRCTX _T("Problem creating string list."));
				break;
			}

			SetData(spData);

		}while(false);
	}
	
    
    // Virus ID link
    //
    DWORD dwVID = NULL;
    hr = m_spThreatData->GetValue(AVModule::ThreatTracking::TrackingData_VID, dwVID);
    if(SUCCEEDED(hr))
    {
        static ccLib::CStringW cszResponseLinkFmt;
        if (cszResponseLinkFmt.IsEmpty())
            CISVersion::LoadStringW(cszResponseLinkFmt, IDS_BRANDING_THREAT_INFO_URL);

        CString cszResponseLink;
        cszResponseLink.Format(cszResponseLinkFmt, dwVID);

        hr = CreateStringData(AV::AVDATA_RESPONSE_THREATINFO_URL, cszResponseLink);
        if(FAILED(hr))
        {
            CCTRACEW( CCTRCTX _T("Unable to create response URL. hr=0x%08X"), hr);
        }
    }
    
    MCF::ICategoryListInternalPtr spCategoryList;
    MCF::CStockCategoryList::CreateObject(spCategoryList);

	if(cThreatDataHelper.IsMCFQuarantineItem())
    {
        MCF::ICategoryInternalPtr spCategory;
        MCF::CStockCategory::CreateObject(spCategory);
        spCategory->SetCategoryId(MCF_CATEGORY_QUARANTINE_ITEMS);
        spCategory->SetDescription(_S(IDS_MCFCATEGORYDESC_QUARANTINE_ITEMS), NULL);
        spCategoryList->Add(spCategory);
    }

	if(cThreatDataHelper.IsMCFSuspiciousEmail())
    {
        MCF::ICategoryInternalPtr spCategory;
        MCF::CStockCategory::CreateObject(spCategory);
        spCategory->SetCategoryId(MCF_CATEGORY_OEH_ACTIVITY);
        spCategory->SetDescription(_S(IDS_MCFCATEGORYDESC_SCAN_RESULT), NULL);
        spCategoryList->Add(spCategory);
    }

    if(cThreatDataHelper.IsMCFSecurityRiskItem())
    {
        MCF::ICategoryInternalPtr spCategory;
        MCF::CStockCategory::CreateObject(spCategory);
        spCategory->SetCategoryId(MCF_CATEGORY_SECURITY_RISKS);
        spCategory->SetDescription(_S(IDS_MCFCATEGORYDESC_SECURITY_RISKS), NULL);
        spCategoryList->Add(spCategory);
    }

	if(cThreatDataHelper.IsMCFUnresolvedRiskItem())
	{
        MCF::ICategoryInternalPtr spCategory;
		MCF::CStockCategory::CreateObject(spCategory);
		spCategory->SetCategoryId(MCF_CATEGORY_UNRESOLVED_ACTIVITY);
		static WCHAR szTemp[1024] = {0};
		if (0 == szTemp[0])
			::LoadString ( _Module.GetResourceInstance(), IDS_MCCATEGORY_UNRESOLVED_ACTIVITY, szTemp, sizeof(szTemp)/sizeof(WCHAR));
		spCategory->SetDescription(szTemp, NULL);
		spCategoryList->Add(spCategory);
	}

    SetCategories(spCategoryList);

    return S_OK;
}

//****************************************************************************
//****************************************************************************
//****************************************************************************
//****************************************************************************
CCalculateThreatData::CCalculateThreatData(AVModule::IAVMapDwordData* pThreatInfo)
{
	m_spThreatInfo = pThreatInfo;

	m_guidThreatTrackId       = GUID_NULL;
	m_dwRiskLevelOverallId    = -1;
	m_dwThreatStateId         = -1;
	m_dwThreatTypeId          = -1;
	m_dwComponentId           = -1;
	m_dwMCFRiskLevel          = MCF::IMessage::eInvalidRiskLevel;
	m_dwMCFPriority           = MCF::IMessage::eInvalidPriority;
	m_dwRecommendedActionId   = AV::RecommendedAction::eInvalid;
	m_dwHasAtLeastOneUndo     = -1;
    m_dwHasViralCategory      = -1;
	m_dwDependencyFlag        = -1;
    m_dwHeuristicRankId       = -1;
	m_dwRequiresReboot        = -1;
	m_dwRequiresProcTerm      = -1;

	m_bAllowShowAutoProtectOptions  = false;
	m_bAllowShowScanOptions         = false;
	m_bAllowShowEmailOptions        = false;
	m_bAllowShowExcludeOptions      = false;
	m_bAllowShowViewDetails         = false;
    m_bThreatIsAppHeuristic         = false;
}

CCalculateThreatData::~CCalculateThreatData()
{

}

//****************************************************************************
//****************************************************************************
GUID& CCalculateThreatData::GetThreatTrackId()
{
	if(m_guidThreatTrackId != GUID_NULL)
		return m_guidThreatTrackId;

	HRESULT hr;
	hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_THREATID, m_guidThreatTrackId);
	if(FAILED(hr))
	{
		CCTRACEE( CCTRCTX _T("Problem retrieving threat track id. hr=0x%08X"));
	}
	
	return m_guidThreatTrackId;
}

//****************************************************************************
//****************************************************************************
const CString&  CCalculateThreatData::GetThreatTrackIdDisplayString()
{
	if(!m_cszThreatTrackId.IsEmpty())
		return m_cszThreatTrackId;

	m_cszThreatTrackId.Format(GUID_FORMAT_STR(GetThreatTrackId()));
	
	return m_cszThreatTrackId;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcComponentId()
{
	DWORD dwComponentId = NULL;
	HRESULT hr;
	hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_SCANNERTYPES_DETECTED_BY, dwComponentId);
	if(FAILED(hr))
	{
		CCTRACEE( CCTRCTX _T("Problem retrieving component id. hr=0x%08X"));
	}
	
	m_cszComponentDisplayName.Empty();
	
	if( dwComponentId & AVModule::ThreatTracking::ScannerTypeManualScanner)
	{
		m_dwComponentId = AV_MODULE_ID_NAVW;
		m_cszComponentDisplayName = CAntivirusMessageBase::m_Features.GetFeatureName(m_dwComponentId);
		m_bAllowShowScanOptions = true;
	}
	else if( dwComponentId & AVModule::ThreatTracking::ScannerTypeAutoProtect)
	{
		m_dwComponentId = SAVRT_MODULE_ID_NAVAPSVC;
		m_cszComponentDisplayName = CAntivirusMessageBase::m_Features.GetFeatureName(m_dwComponentId);
		m_bAllowShowAutoProtectOptions = true;
	}
	else if( dwComponentId & AVModule::ThreatTracking::ScannerTypeEmailScanner)
	{
		m_dwComponentId = AV_MODULE_ID_EMAIL_SCAN;
		m_cszComponentDisplayName = CAntivirusMessageBase::m_Features.GetFeatureName(m_dwComponentId);
		m_bAllowShowEmailOptions = true;
	}
	else if( dwComponentId & AVModule::ThreatTracking::ScannerTypeManualQuarantine)
	{
		m_dwComponentId = AV_MODULE_ID_QUARANTINE;
		m_cszComponentDisplayName = CAntivirusMessageBase::m_Features.GetFeatureName(m_dwComponentId);
	}
	else if(m_cszComponentDisplayName.IsEmpty())
	{
		m_dwComponentId = NULL;
		m_cszComponentDisplayName = CAntivirusMessageBase::m_Features.GetFeatureName(m_dwComponentId);
	}

	m_bAllowShowExcludeOptions = true;
	
	return;
}

//****************************************************************************
//****************************************************************************
DWORD CCalculateThreatData::GetComponentId()
{
	if(m_dwComponentId == (DWORD)-1)
		CalcComponentId();

	return m_dwComponentId;
}

//****************************************************************************
//****************************************************************************
const CString&  CCalculateThreatData::GetComponentDisplayName()
{
	if(m_cszComponentDisplayName.IsEmpty())
		CalcComponentId();
	
	return m_cszComponentDisplayName;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcThreatType()
{
	HRESULT hr;
	hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_THREAT_TYPE, m_dwThreatTypeId);
	if(FAILED(hr))
	{
		CCTRACEE( CCTRCTX _T("Problem retrieving threat type id. hr=0x%08X"), hr);
		return;
	}

	switch(m_dwThreatTypeId)
	{
		case AVModule::ThreatTracking::ThreatType_Anomaly:
			m_cszThreatTypeDisplayName = _S(IDS_TEXT_FILE_BASED);
			m_bAllowShowViewDetails = true; // avScanUI view details supported
			break;

		case AVModule::ThreatTracking::ThreatType_Compressed:
			m_cszThreatTypeDisplayName = _S(IDS_TEXT_COMPRESSED_FILE);
			m_bAllowShowViewDetails = true; // avScanUI view details supported
			break;

		case AVModule::ThreatTracking::ThreatType_Email:
			m_cszThreatTypeDisplayName = _S(IDS_TEXT_EMAIL);
			break;

		case AVModule::ThreatTracking::ThreatType_OEH:
			m_cszThreatTypeDisplayName = _S(IDS_TEXT_EMAIL_HEURISTICS);
			break;

		case AVModule::ThreatTracking::ThreatType_BR:
			m_cszThreatTypeDisplayName = _S(IDS_TEXT_BOOT_RECORD);
			break;

		case AVModule::ThreatTracking::ThreatType_MBR:
			m_cszThreatTypeDisplayName = _S(IDS_TEXT_MASTER_BOOT_RECORD);
			break;

        case AVModule::ThreatTracking::ThreatType_CleanStealthFile:
            m_cszThreatTypeDisplayName = _S(IDS_TEXT_CLEAN_STEALTH_FILE);
            break;

		default:
			m_cszThreatTypeDisplayName = _S(IDS_TEXT_UNKNOWN_THREAT_TYPE);
			break;
	}

	return;
}

//****************************************************************************
//****************************************************************************
DWORD CCalculateThreatData::GetThreatTypeId()
{
	if(m_dwThreatTypeId == (DWORD)-1)
		CalcThreatType();

	return m_dwThreatTypeId;
}

//****************************************************************************
//****************************************************************************
const CString&  CCalculateThreatData::GetThreatTypeDisplayName()
{
	if(m_cszThreatTypeDisplayName.IsEmpty())
		CalcThreatType();

	return m_cszThreatTypeDisplayName;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcThreatState()
{
	// Get Threat State
	HRESULT hr;
	hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_CURRENT_STATE, m_dwThreatStateId);
	if(FAILED(hr))
	{
		CCTRACEE( CCTRCTX _T("Problem retrieving threat state. hr=0x%08X"));
		return;
	}

	switch(m_dwThreatStateId)
	{
		case AVModule::ThreatTracking::ThreatState_FullyRemoved:
			m_cszThreatStateDisplayName = _S(IDS_TEXT_FULLY_REMOVED);
			break;

		case AVModule::ThreatTracking::ThreatState_PartiallyRemoved:
			m_cszThreatStateDisplayName = _S(IDS_TEXT_PARTIALLY_REMOVED);
			break;

		case AVModule::ThreatTracking::ThreatState_RemoveNotAttempted:
			m_cszThreatStateDisplayName = _S(IDS_TEXT_REMOVAL_NOT_ATTEMPTED);
			break;

		case AVModule::ThreatTracking::ThreatState_CompressedInProcessing:
			m_cszThreatStateDisplayName = _S(IDS_TEXT_COMPRESSED_IN_PROCESSING);
			break;

		case AVModule::ThreatTracking::ThreatState_Excluded:
			m_cszThreatStateDisplayName = _S(IDS_TEXT_EXCLUDED);
			break;

		case AVModule::ThreatTracking::ThreatState_RemoveFailed:
			m_cszThreatStateDisplayName = _S(IDS_TEXT_REMOVAL_FAILED);
			break;

		case AVModule::ThreatTracking::ThreatState_DoNotDelete:
			m_cszThreatStateDisplayName = _S(IDS_TEXT_NOT_SAFE_TO_REMOVE);
			break;

		case AVModule::ThreatTracking::ThreatState_BackupOnly:
			m_cszThreatStateDisplayName = _S(IDS_TEXT_BACKUP_ONLY);
			break;

		case AVModule::ThreatTracking::ThreatState_RemoveOnRebootFailed:
			m_cszThreatStateDisplayName = _S(IDS_TEXT_REMOVE_ON_REBOOT_FAILED);
			break;

		case AVModule::ThreatTracking::ThreatState_CannotRemediate:
			m_cszThreatStateDisplayName = _S(IDS_TEXT_CANNOT_REMEDIATE);
			break;

		default:
			m_cszThreatStateDisplayName = _S(IDS_TEXT_UNKNOWN_THREAT_TYPE);
			break;
	}
	
	return;
}

//****************************************************************************
//****************************************************************************
DWORD CCalculateThreatData::GetThreatStateId()
{
	if(m_dwThreatStateId == (DWORD)-1)
		CalcThreatState();

	return m_dwThreatStateId;
}

//****************************************************************************
//****************************************************************************
const CString&  CCalculateThreatData::GetThreatStateDisplayString()
{
	if(m_cszThreatStateDisplayName.IsEmpty())
		CalcThreatState();

	return m_cszThreatStateDisplayName;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcRiskLevelOverall()
{
	//  Set Risk Level
	HRESULT hr;
	hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_OVERALL_THREAT_LEVEL, m_dwRiskLevelOverallId);
	if(FAILED(hr))
	{
		CCTRACEE( CCTRCTX _T("Problem retrieving overall threat matrix value, assuming high-risk. hr=0x%08X"));
		m_dwRiskLevelOverallId = 3; // assume high-risk	
	}

	UINT pRiskLevelMap[3] = {IDS_TEXT_LOW, IDS_TEXT_MEDIUM, IDS_TEXT_HIGH};
	if(m_dwRiskLevelOverallId >= 0 && m_dwRiskLevelOverallId <= 2)
		m_cszRiskLevelOverallDisplayString = _S(pRiskLevelMap[m_dwRiskLevelOverallId]);

	return;
}

//****************************************************************************
//****************************************************************************
DWORD CCalculateThreatData::GetRiskLevelOverallId()
{
	if(!(m_dwRiskLevelOverallId >= 0 && m_dwRiskLevelOverallId <= 2))
		CalcRiskLevelOverall();

	return m_dwRiskLevelOverallId;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcDependency()
{
	ISymBasePtr spTempObj;
	HRESULT hr;
	hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_INDIVIDUAL_THREAT_LEVELS, AVModule::AVDataTypes::eTypeMapDword, spTempObj);
	if(FAILED(hr) || !spTempObj)
		return;

	AVModule::IAVMapDwordDataQIPtr spMapCurThreat = spTempObj;
	if(!spTempObj)
		return;

	spMapCurThreat->GetValue(ccEraser::IAnomaly::Dependency, m_dwDependencyFlag);
	return;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::HasDependencyFlag()
{
	if(m_dwDependencyFlag == (DWORD)-1)
		CalcDependency();

	return m_dwDependencyFlag != 0 && m_dwDependencyFlag != (DWORD)-1;
}

//****************************************************************************
//****************************************************************************
const CString&  CCalculateThreatData::GetRiskLevelOverallDisplayString()
{
	if(m_cszRiskLevelOverallDisplayString.IsEmpty())
		CalcRiskLevelOverall();

	return m_cszRiskLevelOverallDisplayString;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcThreatCat()
{
	ISymBasePtr spTempObj;
	HRESULT hr;
	hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_THREATCATEGORIES, AVModule::AVDataTypes::eTypeArray, spTempObj);
	if(FAILED(hr) || !spTempObj)
	{
		CCTRACEE( CCTRCTX _T("Problem retrieving threat cat info. hr=0x%08X"));
		return;
	}

	AVModule::IAVArrayDataQIPtr spCategories = spTempObj;
	if(spCategories)
	{
		// Fill in the threat categories
		CString cszCategories;
		DWORD dwCatCount = 0;
		spCategories->GetCount(dwCatCount);
		for(DWORD dwCatIndex=0; dwCatIndex < dwCatCount; dwCatIndex++ )
		{
			DWORD dwCurCategory = 0;
			hr = spCategories->GetValue(dwCatIndex, dwCurCategory);
			if(SUCCEEDED(hr))
			{
				CString cszTemp;
				cszTemp.Format(_T("%u "), dwCurCategory);
				cszCategories += cszTemp;
			    
                if(ccEraser::IAnomaly::ApplicationHeuristic == dwCurCategory)
                    m_bThreatIsAppHeuristic = true;
            }
		}

		if(!cszCategories.IsEmpty())
		{
			CThreatCatInfo ThreatInfo;
			TCHAR szCategoryList[512] = {NULL};
			CAntivirusMessageBase::m_ThreatInfo.GetCategoryText ( cszCategories, szCategoryList, 512 );
			m_cszThreatCatDisplayString = szCategoryList;
		}
	}

	return;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcTheatHasViralCategory()
{
    HRESULT hr;
    hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_HASVIRALCATEGORY, m_dwHasViralCategory);
    if(FAILED(hr))
    {
        m_dwHasViralCategory = 0;
    }

    return;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::ThreatHasViralCategory()
{
    if((DWORD)-1 == m_dwHasViralCategory)
        CalcTheatHasViralCategory();

    return 0 != m_dwHasViralCategory ? true : false;
}

//****************************************************************************
//****************************************************************************
const CString&  CCalculateThreatData::GetThreatCatDisplayString()
{
	if(m_cszThreatCatDisplayString.IsEmpty())
		CalcThreatCat();

	return m_cszThreatCatDisplayString;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcThreatName()
{
	cc::IStringPtr spThreatName;
	HRESULT hr;
	hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_NAME, spThreatName);
	if(FAILED(hr) || !spThreatName)
	{
		CCTRACEE( CCTRCTX _T("Problem retrieving threat name. 0x%08X"), hr);
		m_cszThreatName = _S(IDS_ITEM_TYPE_UNKNOWN);
	    return;
	}

	m_cszThreatName = spThreatName->GetStringW();
	return;
}

//****************************************************************************
//****************************************************************************
const CString& CCalculateThreatData::GetThreatName()
{
	if(m_cszThreatName.IsEmpty())
		CalcThreatName();

	return m_cszThreatName;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcShortDescription()
{
	
	if(!GetThreatName().IsEmpty() && !GetComponentDisplayName().IsEmpty())
	{
		m_cszShortDescription.FormatMessage(_S(IDS_MCFFMT_SHORT_DESCRIPTION), (LPCTSTR)GetThreatName(), (LPCTSTR)GetComponentDisplayName());
	}
	else if(!GetThreatName().IsEmpty())
	{
		m_cszShortDescription = GetThreatName();
	}
	else
	{
		m_cszShortDescription = _S(IDS_ITEM_TYPE_UNKNOWN);
	}

	return;
}

//****************************************************************************
//****************************************************************************
const CString& CCalculateThreatData::GetShortDescription()
{
	if(m_cszShortDescription.IsEmpty())
		CalcShortDescription();

	return m_cszShortDescription;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcMCFRiskLevel()
{
	if(ThreatHasBeenResolved())
		m_dwMCFRiskLevel = MCF::IMessage::eLowRisk;
	else
		m_dwMCFRiskLevel = MCF::IMessage::eLowRisk - GetRiskLevelOverallId();

	return;
}

//****************************************************************************
//****************************************************************************
DWORD CCalculateThreatData::GetMCFRiskLevel()
{
	if(MCF::IMessage::eInvalidRiskLevel == m_dwMCFRiskLevel)
		CalcMCFRiskLevel();

	return m_dwMCFRiskLevel;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcRecommendedAction()
{
	if(    ThreatHasBeenResolved() || ThreatHasBeenIgnored()
		|| AVModule::ThreatTracking::ThreatLevel_Low == GetRiskLevelOverallId() 
		|| AVModule::ThreatTracking::ThreatType_Email == GetThreatTypeId() )
	{
		m_dwRecommendedActionId = AV::RecommendedAction::eDoNothing;
		m_cszRecommendedActionDisplayString = _S(IDS_RECOMMENDED_ACTION_DO_NOTHING);
	}
	else if(ManualRemovalRequired())
	{
		m_dwRecommendedActionId = AV::RecommendedAction::eManualRemovalRequired;
		m_cszRecommendedActionDisplayString = _S(IDS_RECOMMENDED_ACTION_MANUAL_REMOVAL_REQUIRED);
	}
	else if(ThreatRequiresReboot())
	{
		m_dwRecommendedActionId = AV::RecommendedAction::eRebootRequired;
		m_cszRecommendedActionDisplayString = _S(IDS_RECOMMENDED_ACTION_REBOOT);
	}
    else if(AVModule::ThreatTracking::ThreatType_Anomaly == GetThreatTypeId())
    {
        m_dwRecommendedActionId = AV::RecommendedAction::eRemoveNow;
        m_cszRecommendedActionDisplayString = _S(IDS_RECOMMENDED_ACTION_REMOVENOW);
    }
	else
	{
		m_dwRecommendedActionId = AV::RecommendedAction::eRunQuickScan;
		m_cszRecommendedActionDisplayString = _S(IDS_RECOMMENDED_ACTION_RUN_QUICK_SCAN);
	}
	
	return;
}

//****************************************************************************
//****************************************************************************
DWORD CCalculateThreatData::GetRecommendedActionId()
{
	if(AV::RecommendedAction::eInvalid == m_dwRecommendedActionId)
		CalcRecommendedAction();

	return m_dwRecommendedActionId;
}

//****************************************************************************
//****************************************************************************
const CString& CCalculateThreatData::GetRecommendedActionDisplayString()
{
	if(m_cszRecommendedActionDisplayString.IsEmpty())
		CalcRecommendedAction();

	return m_cszRecommendedActionDisplayString;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::ThreatHasBeenResolved()
{
	bool bThreatHasBeenResolved;
	if(   ( ThreatHasBeenRemoved() 
		||  AVModule::ThreatTracking::ThreatState_BackupOnly == GetThreatStateId() 
		||  AVModule::ThreatTracking::ThreatType_Email == GetThreatTypeId() 
        ||  ThreatHasBeenExcluded() )
		&& !ThreatRequiresReboot() 
		&& !ThreatRequiresProcTerm() )

	{
		bThreatHasBeenResolved = true;
	}
	else
	{
		bThreatHasBeenResolved = false;
	}

	return bThreatHasBeenResolved;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::ThreatHasBeenRemoved()
{
	return AVModule::ThreatTracking::ThreatRemoved(GetThreatStateId()) && !ThreatHasBeenExcluded();
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::ThreatIsManualQuarantineItem()
{
	return AV_MODULE_ID_QUARANTINE == GetComponentId();
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::ThreatHasBeenIgnored()
{
	return    !ThreatHasBeenResolved() 
		   && ( (AVModule::ThreatTracking::ThreatState_RemoveNotAttempted  == GetThreatStateId()) && (AVModule::ThreatTracking::ThreatLevel_Low == GetRiskLevelOverallId()) )  
		   && !ManualRemovalRequired() 
		   && !HasDependencyFlag() 
		   && !ThreatRequiresReboot() 
		   && !ThreatRequiresProcTerm();
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::ManualRemovalRequired()
{
	return !ThreatHasBeenResolved() && (AVModule::ThreatTracking::ThreatState_DoNotDelete == GetThreatStateId());
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::ThreatHasBeenExcluded()
{
	return AVModule::ThreatTracking::ThreatState_Excluded == GetThreatStateId();
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcMCFPriority()
{
	if(    ThreatHasBeenResolved() 
		|| ThreatHasBeenIgnored() && AVModule::ThreatTracking::ThreatLevel_Low == GetRiskLevelOverallId())
		m_dwMCFPriority = MCF::IMessage::eLowPriority;
	else
		m_dwMCFPriority = MCF::IMessage::eHighPriority;
	
	return;
}

//****************************************************************************
//****************************************************************************
DWORD CCalculateThreatData::GetMCFPriority()
{
	if(MCF::IMessage::eInvalidPriority == m_dwMCFPriority)
		CalcMCFPriority();
	
	return m_dwMCFPriority;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcCanRestoreItem()
{
	HRESULT hr;
	hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_HasAtLeastOneUndo, m_dwHasAtLeastOneUndo);
	if(FAILED(hr))
	{
		m_dwHasAtLeastOneUndo = 0;
	}

	return;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcThreatRequiresReboot()
{
	HRESULT hr;
	hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_REBOOTREQUIRED, m_dwRequiresReboot);
	if(FAILED(hr))
	{
		m_dwRequiresReboot = 0;
	}
	
	return;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::ThreatRequiresReboot()
{
	if((DWORD)-1 == m_dwRequiresReboot)
		CalcThreatRequiresReboot();

	return 0 != m_dwRequiresReboot ? true : false;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcThreatRequiresProcTerm()
{
	HRESULT hr;
	hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_REBOOTREQUIRED, m_dwRequiresProcTerm);
	if(FAILED(hr))
	{
		m_dwRequiresProcTerm = 0;
	}

	return;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::ThreatRequiresProcTerm()
{
	if((DWORD)-1 == m_dwRequiresProcTerm)
		CalcThreatRequiresProcTerm();

	return 0 != m_dwRequiresProcTerm ? true : false;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::CanRestoreItem()
{
	if(m_dwHasAtLeastOneUndo == (DWORD)-1)
		CalcCanRestoreItem();

	return 0 != m_dwHasAtLeastOneUndo ? true : false;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::IsMCFQuarantineItem()
{
	if( ( (0 != CanRestoreItem() && AVModule::ThreatTracking::ThreatRemoved(GetThreatStateId()))
		|| ThreatIsManualQuarantineItem() )
        && !IsMCFDoNotDisplay() )
		return true;

	return false;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::IsMCFSuspiciousEmail()
{
	if((AVModule::ThreatTracking::ThreatType_OEH == GetThreatTypeId()) && !IsMCFDoNotDisplay())
		return true;

	return false;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::IsMCFSecurityRiskItem()
{
	if(!IsMCFSuspiciousEmail() && (ThreatHasBeenResolved() || ThreatHasBeenIgnored()) && !IsMCFDoNotDisplay())
		return true;

	return false;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::IsMCFUnresolvedRiskItem()
{
	if(!IsMCFSuspiciousEmail() && !ThreatHasBeenResolved() && !ThreatHasBeenIgnored() && !IsMCFDoNotDisplay())
		return true;

	return false;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::IsMCFDoNotDisplay()
{
    if(AVModule::ThreatTracking::ThreatType_CleanStealthFile == GetThreatTypeId())
		return true;

	return false;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcMCFStatus()
{
	if(ThreatIsManualQuarantineItem())
		m_cszMCFStatusDisplayString = _S(IDS_MCFSTATUS_QUARANTINED);
	else if(ThreatHasBeenResolved() && ThreatHasBeenRemoved())
		m_cszMCFStatusDisplayString = _S(IDS_MCFSTATUS_REMOVED);
    else if(ThreatHasBeenResolved() && ThreatHasBeenExcluded())
        m_cszMCFStatusDisplayString = _S(IDS_MCFSTATUS_EXCLUDED);
	else if(ThreatHasBeenResolved())
		m_cszMCFStatusDisplayString = _S(IDS_MCFSTATUS_RESOLVED);
	else if(ThreatHasBeenIgnored())
		m_cszMCFStatusDisplayString = _S(IDS_MCFSTATUS_IGNORED);
	else if(ManualRemovalRequired())
		m_cszMCFStatusDisplayString = _S(IDS_MCFSTATUS_MANUAL_REMOVAL_REQUIRED);
	else if(ThreatRequiresReboot())
		m_cszMCFStatusDisplayString = _S(IDS_MCFSTATUS_REBOOT_REQUIRED);
	else
		m_cszMCFStatusDisplayString = _S(IDS_MCFSTATUS_UNRESOLVED);

	return;
}

//****************************************************************************
//****************************************************************************
const CString& CCalculateThreatData::GetMCFStatusDisplayString()
{
	if(m_cszMCFStatusDisplayString.IsEmpty())
		CalcMCFStatus();
	
	return m_cszMCFStatusDisplayString;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcHeuristicRank()
{
    HRESULT hr;
    hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_Anomaly_HeuristicRank, m_dwHeuristicRankId);
    if(FAILED(hr))
        return; // do nothing, this value may not exist

    if(0 == m_dwHeuristicRankId)
        m_cszHeuristicRankDisplayString = _S(IDS_TEXT_APPHEUR_RANK_SUSPICIOUS);
    else if(1 == m_dwHeuristicRankId)
        m_cszHeuristicRankDisplayString = _S(IDS_TEXT_APPHEUR_RANK_POSITIVE);

    return;
}

//****************************************************************************
//****************************************************************************
void CCalculateThreatData::CalcSIDString()
{
    HRESULT hr;
    cc::IStringPtr spSIDString;
    hr = m_spThreatInfo->GetValue(AVModule::ThreatTracking::TrackingData_SID, spSIDString);
    if(FAILED(hr) || !spSIDString)
    {
        CCTRACEE( CCTRCTX _T("Problem getting SID string. hr=0x%08X"), hr);
        return;
    }

    m_cszSIDString = spSIDString->GetStringW();
    return;
}

//****************************************************************************
//****************************************************************************
const CString& CCalculateThreatData::GetSIDString()
{
    if(m_cszSIDString.IsEmpty())
        CalcSIDString();

    return m_cszSIDString;
}

//****************************************************************************
//****************************************************************************
DWORD CCalculateThreatData::GetHeuristicRankId()
{
    if((DWORD)-1 == m_dwHeuristicRankId)
        CalcHeuristicRank();

    return m_dwHeuristicRankId;
}

//****************************************************************************
//****************************************************************************
CString& CCalculateThreatData::GetHeuristicRankDisplayString()
{
    if((DWORD)-1 == m_dwHeuristicRankId)
        CalcHeuristicRank();
    
    return m_cszHeuristicRankDisplayString;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::ThreatIsAppHeuristic()
{
    // This value is calc'd during category enum
    if(m_cszThreatCatDisplayString.IsEmpty())
        CalcThreatCat(); 
    
    return m_bThreatIsAppHeuristic;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowReviewRiskDetails()
{
	bool bFunctionalControl = ManualRemovalRequired() && !ThreatRequiresReboot();
	if(!bFunctionalControl)
		return false;
	
	return true;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowDeleteData()
{
	// don't currently have functionality for this feature
	return false;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowDeleteItem()
{
	bool bFunctionalControl = (ThreatHasBeenResolved() || ThreatHasBeenIgnored()) && !ThreatRequiresReboot();
	if(!bFunctionalControl)
		return false;
	// Moved Access Control Check to AVProdLoggingACP.cpp for the Vista Related Defect #797431
	return true;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowRestoreItem()
{
	bool bFunctionalControl = ThreatHasBeenResolved() && CanRestoreItem();
	if(!bFunctionalControl)
		return false;
    // Moved Access Control Check to AVProdLoggingACP.cpp for the Vista Related Defect #797431  
	return true;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowSubmitItem()
{
	return AllowShowRestoreItem();
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowRemoveNow()
{
    bool bFunctionalControl =   !ThreatHasBeenResolved() 
                              && AVModule::ThreatTracking::ThreatType_Anomaly == GetThreatTypeId() 
                              && !ThreatRequiresReboot();
    if(!bFunctionalControl)
        return false;

    return true;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowQuickScan()
{
	// This functionality is disabled in lieu of AllowShowRemoveNow().
    bool bFunctionalControl = false; // !ThreatHasBeenResolved() && AVModule::ThreatTracking::ThreatType_Anomaly == GetThreatTypeId();
	if(!bFunctionalControl)
		return false;

	return true;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowExcludeItem()
{
	bool bFunctionalControl = !ThreatHasBeenResolved() && !ThreatHasViralCategory() && !ThreatRequiresReboot();
	if(!bFunctionalControl)
		return false;
    // Moved Access Control Check to AVProdLoggingACP.cpp for the Vista Related Defect #797431
    return true;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowViewDetails()
{
	bool bFunctionalControl = AVModule::ThreatTracking::ThreatType_Anomaly == GetThreatTypeId() || AVModule::ThreatTracking::ThreatType_Compressed == GetThreatTypeId();
	if(!bFunctionalControl)
		return false;

	return true;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowFileList()
{
    bool bFunctionalControl = !AllowShowViewDetails();
    if(!bFunctionalControl)
        return false;

    return true;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowOptionsSection()
{
	bool bFunctionalControl = !ThreatIsManualQuarantineItem();
	if(!bFunctionalControl)
		return false;

	return true;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowAutoProtectOptions()
{
    if(m_dwComponentId == (DWORD)-1)
        CalcComponentId();

    return m_bAllowShowAutoProtectOptions;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowScanOptions()
{
    if(m_dwComponentId == (DWORD)-1)
        CalcComponentId();

    return m_bAllowShowScanOptions;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowEmailOptions()
{
    if(m_dwComponentId == (DWORD)-1)
        CalcComponentId();

    return m_bAllowShowEmailOptions;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::AllowShowExcludeOptions()
{
    if(m_dwComponentId == (DWORD)-1)
        CalcComponentId();

    return m_bAllowShowExcludeOptions;
}

//****************************************************************************
//****************************************************************************
bool CCalculateThreatData::ThreatHasNoActions()
{
    if(   !AllowShowExcludeItem() 
       && !AllowShowRestoreItem()
       && !AllowShowQuickScan()
       && !AllowShowRemoveNow()
       && !AllowShowDeleteData()
       && !AllowShowDeleteItem()
       && !AllowShowReviewRiskDetails())
       return true;

    return false;
}

