////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AVccModuleID.h"
#include "threatsbyvid.h"
#include "ccCatch.h"
#include "..\navoptionsres\resource.h"
#include "ccModuleNames.h"
#include <algorithm>
#include "ccSymStringImpl.h"
#include "ccSymIndexValueCollectionImpl.h"
#include "AVInterfaces.h"
#include "AVInterfaceLoader.h"

bool CThreatsByVID::Compare_ByThreatName(CThreatItem Item1, CThreatItem Item2)
{
    if( Item1.m_bstrThreatName < Item2.m_bstrThreatName )
        return true;

    return false;
}

CThreatsByVID::CThreatItem::CThreatItem()
{
    m_dwVID = 0;
}

CThreatsByVID::CThreatItem::~CThreatItem()
{
}

bool CThreatsByVID::CThreatItem::InFilter(long lCategory)
{
    if( lCategory == AllNonVirals )
        return true;

    std::set<long>::iterator Iter = m_setCategories.find(lCategory);

    if( Iter != m_setCategories.end() )
        return true;

    return false;
}

CThreatsByVID::CThreatsByVID(void)
{
    m_CurFilter = AllNonVirals;
}

CThreatsByVID::~CThreatsByVID(void)
{
}

HRESULT CThreatsByVID::FinalConstruct()
{
    if( !Initialize() )
        CCTRACEE(_T("CThreatsByVID::FinalConstruct() - Failed to initialize."));

    return S_OK;
}

void CThreatsByVID::FinalRelease()
{
    if( m_spScanner )
    {
        unsigned long ulRef = m_spScanner.Release();
        CCTRACEI(_T("CThreatsByVID::FinalRelease() - After releasing IScanner4 ref count = %d."), ulRef);
    }

	if(m_spScanUIMisc)
	{
		m_spScanUIMisc.Release();
		CCTRCTXI0(_T("Manually released Scan UI."));
	}
}

bool CThreatsByVID::Initialize()
{
    cc::IStringPtr spStr;
    HRESULT hr;

    // See if we are already initialized
    if( !m_vItems.empty() )
        return true;

	ccScanw::SCANWSTATUS Status = ccScanw::SCANW_OK;
    SYMRESULT symRes = SYM_OK;
    // CAV DefInfo objects
    AVModule::AVLoader_IAVDefInfo AVDefInfoLoader;
    AVModule::IAVDefInfoPtr spAVDefInfo;

    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        exceptionInfo.SetFunction(_T("CThreatsByVID::Initialize()"));

        // Load AVDefInfo
        if(SYM_FAILED(symRes = AVDefInfoLoader.CreateObject(&spAVDefInfo)) || spAVDefInfo == NULL)
        {
            CCTRCTXE1(_T("Failed to create AV Def Info object. SR = 0x%X"), symRes);
            return false;
        }

        // Get the defs path
        if(FAILED(hr = spAVDefInfo->GetDefsDir(spStr)))
        {
            CCTRCTXE1(_T("Error getting Defs Directory. HR = 0x%X"), hr);
            return false;
        }

        // Load ccScan
        if( SYM_FAILED(symRes = m_ccScanLoader.CreateObject(&m_spScanner)) || m_spScanner == NULL )
        {
            CCTRACEE(_T("CThreatsByVID::Initialize() - Error creating the scanner. SYMRESULT = 0x%X"), symRes);
            return false;
        }

        // Initialize the scanning engine.
        WCHAR szTempDir[MAX_PATH];
        GetTempPathW(MAX_PATH, szTempDir);
        GetShortPathNameW(szTempDir, szTempDir, MAX_PATH);

        // This lets CAV manage the App IDs.
        m_spScanner->Initialize(spStr->GetStringW(), szTempDir, 0, ccScanw::IScannerw::ISCANNERW_USE_DEF_PATH);

        // Check if the initialization failed
        if( Status != ccScanw::SCANW_OK )
        {
            CCTRACEE(_T("CThreatsByVID::Initialize() - Error initializing the scanner. SCANSTATUS = 0x%X"), Status);
            return false;
        }

        //
        // Populate map with all non-viral VIDs
        //

        // Get the definitions information
        ccScanw::IScanwDefinitionsInfoPtr spDefs;
        Status = m_spScanner->GetDefinitionsInfoW(spDefs);
        if( Status != ccScanw::SCANW_OK || spDefs == NULL )
        {
            CCTRACEE(_T("CThreatsByVID::Initialize() - Error getting definitions info. SCANSTATUS = 0x%X"), Status);
            return false;
        }

        unsigned long ulCount;
		if((Status = spDefs->GetCount(ulCount)) != ccScanw::SCANW_OK)
		{
			CCTRCTXE1(_T("Error calling IScanwDefinitionsInfo::GetCount. SCANSTATUS = 0x%X"), Status);
			return false;
		}

        for(unsigned long ulIndex = 0; ulIndex<ulCount; ulIndex++)
        {
            ccScanw::IScanwVirusInfoPtr spVirInfo;
            if( ccScanw::SCANW_OK != (Status = spDefs->GetVirusInfo(ulIndex, spVirInfo)) || spVirInfo == NULL )
            {
                CCTRACEE(_T("CThreatsByVID::Initialize() - Error getting virus info for item %lu. SCANSTATUS = 0x%X"), ulIndex, Status);
                continue;
            }

            // Get the categories
            DWORD dwCatCount = 0;
            const unsigned long* pculCats = NULL;
            if( ccScanw::SCANW_OK != spVirInfo->GetCategories(pculCats, dwCatCount) || pculCats == NULL )
            {
                CCTRACEE(_T("CThreatsByVID::Initialize() - Error getting category count for item %lu"), ulIndex);
                continue;
            }

            CThreatItem item;

            // See if any of the categories are classified as non-viral
            bool bNonViral = false;
            for(DWORD i=0; i<dwCatCount; i++)
            {
                if( pculCats[i] >= ccScanw::SCANW_THREAT_TYPE_SECURITY_RISK &&
                    pculCats[i] <= ccScanw::SCANW_THREAT_TYPE_JOKE)
                {
                    bNonViral = true;
                    item.m_setCategories.insert(pculCats[i]);
                }
            }

            if( bNonViral )
            {
				ATL::CAtlStringW strName;
				DWORD dwBufSize = MAX_PATH;

				Status = spVirInfo->GetVirusID(item.m_dwVID);
				if(Status != ccScanw::SCANW_OK)
				{
					CCTRCTXE2(_T("Error getting VID from item %lu. Status = 0x%X"), ulIndex, Status);
				}

                Status = spVirInfo->GetName(strName.GetBuffer(dwBufSize), dwBufSize);
                strName.ReleaseBuffer();

                if( Status == ccScanw::SCANW_ERROR_BUFFER_TOO_SMALL )
                {
                    // Try again with a bigger buffer
                    dwBufSize++;
                    Status = spVirInfo->GetName(strName.GetBuffer(dwBufSize), dwBufSize);
                    strName.ReleaseBuffer();
                }

                if(Status != ccScanw::SCANW_OK)
                {
                    CCTRACEE(_T("CThreatsByVID::Initialize() - Error getting threat name for item %lu. Status = 0x%X"), ulIndex, Status);
                    continue;
                }

                // Make sure this is not one of the threats we shouldn't add
                if( -1 != strName.Find(_T("Test.SYMC.")) )
                {
                    CCTRACEI(_T("CThreatsByVID::Initialize() - Not adding threat %d because it's a test sample."), item.m_dwVID); // strName
                    continue;
                }

                item.m_bstrThreatName = strName;

                // Now add this item to the vector
                m_vItems.push_back(item);
            }
        }

        // Sort the list by the threat names
        sort(m_vItems.begin(), m_vItems.end(), Compare_ByThreatName);
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCH_STD(exceptionInfo);

    CCTRACEI(_T("CThreatsByVID::Initialize() - Vector contains %lu non-viral items"), m_vItems.size());

    return true;
}

HRESULT CThreatsByVID::get_Count(/*[out,retval]*/ unsigned long* pulCount)
{
    if( NULL == pulCount )
    {        
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        exceptionInfo.SetFunction(_T("CThreatsByVID::get_Count()"));

        *pulCount = 0;
        m_vCurFilterItems.clear();

        // Build a vector of only the items selected in the current filter
        std::vector<CThreatItem>::iterator Iter;
        for( Iter = m_vItems.begin(); Iter != m_vItems.end(); Iter++ )
        {
            if( Iter->InFilter(m_CurFilter) )
            {
                m_vCurFilterItems.push_back(&(*Iter));
            }
        }
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCH_STD(exceptionInfo);

    *pulCount = m_vCurFilterItems.size();

    return S_OK;
}

HRESULT CThreatsByVID::get_ThreatName(/*[in]*/ unsigned long ulIndex, /*[out,retval]*/ BSTR* bstrThreatName)
{
    if( NULL == bstrThreatName )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    // validate the index is good
    if( ulIndex > m_vCurFilterItems.size() )
    {
        MakeError (IDS_ERR_ANOMALYVID_EXCL_INVALID_INDEX, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        exceptionInfo.SetFunction(_T("CThreatsByVID::get_ThreatName()"));

        CThreatItem* pItem = m_vCurFilterItems.at(ulIndex);

        if( pItem )
        {
            *bstrThreatName = pItem->m_bstrThreatName.Copy();
        }
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCH_STD(exceptionInfo);

    return S_OK;
}

HRESULT CThreatsByVID::get_ThreatVID(/*[in]*/ unsigned long ulIndex, /*[out,retval]*/ unsigned long* pulVID)
{
    if( NULL == pulVID )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    // validate the index is good
    if( ulIndex > m_vCurFilterItems.size() )
    {
        MakeError (IDS_ERR_ANOMALYVID_EXCL_INVALID_INDEX, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        exceptionInfo.SetFunction(_T("CThreatsByVID::get_ThreatVID()"));

        *pulVID = 0;

        CThreatItem* pItem = m_vCurFilterItems.at(ulIndex);

        if( pItem )
        {
            *pulVID = pItem->m_dwVID;
        }
    }
    CCCATCHMEM(exceptionInfo)
    CCCATCH_STD(exceptionInfo);

    return S_OK;
}

HRESULT CThreatsByVID::get_Filter(/*[out,retval]*/ ENonViralCategories* peFilterCategory)
{
    if( NULL == peFilterCategory )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    *peFilterCategory = m_CurFilter;
    return S_OK;
}


HRESULT CThreatsByVID::put_Filter(/*[in]*/ long lFilterCategory)
{
    // Make sure this is an acceptable category filter
    if( lFilterCategory == AllNonVirals ||
        lFilterCategory >= SecurityRisk &&
        lFilterCategory <= JokePrograms)
    {
        m_CurFilter = static_cast<ENonViralCategories>(lFilterCategory);
        CCTRACEI(_T("CThreatsByVID::put_Filter() - %d is now the filter"), m_CurFilter);
    }
    else
    {
        CCTRACEE(_T("CThreatsByVID::put_Filter() - %d is an invalid filter"), m_CurFilter);
        MakeError(IDS_ERR_PUT_THREATBYVID_FILTER, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CThreatsByVID::put_NAVERROR(/*[in]*/  INAVCOMError* pNavError)
{
    if( NULL == pNavError )
    {
        return E_POINTER;
    }

    m_spError = pNavError;

    return S_OK;
}

HRESULT CThreatsByVID::get_NAVERROR(/*[out,retval]*/  INAVCOMError** ppNavError)
{
    if ( !m_spError )
        return E_FAIL;

    *ppNavError = m_spError;
    (*ppNavError)->AddRef (); // Ref count for the caller

    return S_OK;
}

void CThreatsByVID::MakeError(long lErrorID, long lHResult, long lNAVErrorResID)
{
    if ( !m_spError )
        return;

    m_spError->put_ModuleID ( AV_MODULE_ID_OPTIONS_UI );
    m_spError->put_ErrorID ( lErrorID );
    m_spError->put_ErrorResourceID ( lNAVErrorResID );
    m_spError->put_HResult ( lHResult );
    _Module.SetResourceInstance(_Module.GetModuleInstance());
}

HRESULT CThreatsByVID::DisplayDetails(unsigned long ulVID, long hwnd)
{
	HRESULT hr;

    // Load the scan UI
    if( !m_spScanUIMisc )
    {
        SYMRESULT symRes = m_ScanUILoader.CreateObject(m_spScanUIMisc);
        if( SYM_FAILED(symRes) || !m_spScanUIMisc )
        {
            // UI object creation failed, let's fail gracefully
            CCTRCTXE1(_T("Failed to create the scan UI object. Result = 0x%X"), symRes);
            return S_FALSE;
        }
    }

    // Get the enabled pop-up window to make this modal to
    HWND hwndParent = GetWindow(reinterpret_cast<HWND>(hwnd), GW_ENABLEDPOPUP);

    if( hwndParent == NULL )
    {
        CCTRCTXI0(_T("Did not find a popup enabled window"));
        hwndParent = reinterpret_cast<HWND>(hwnd);
    }

    // Display the dialog
	if(FAILED(hr = m_spScanUIMisc->Initialize()))
	{
		CCTRCTXE1(_T("Failed to initialize AV Scan UI. HR = 0x%08x"), hr);
		return S_FALSE;
	}

	if(FAILED(hr = m_spScanUIMisc->DisplayVIDDetails(hwndParent, ulVID)))
	{
		CCTRCTXE2(_T("Error trying to display VID details for VID %x. HR = 0x%08x"), ulVID, hr);
		return S_FALSE;
	}

    return S_OK;
}
