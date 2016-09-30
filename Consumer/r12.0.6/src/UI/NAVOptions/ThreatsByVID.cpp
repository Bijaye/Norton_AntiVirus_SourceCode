#include "stdafx.h"
#include "AVccModuleID.h"
#include "threatsbyvid.h"
#include "ccCatch.h"
#include "NAVErrorResource.h"
#include "..\navoptionsres\resource.h"
#include "ccModuleNames.h"
#include <algorithm>
#include "ccSymStringImpl.h"
#include "ccSymIndexValueCollectionImpl.h"

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

CThreatsByVID::CThreatsByVID(void) : m_pScanProps(NULL),
                                     m_pEraserScanSink(NULL)
{
    m_CurFilter = AllNonVirals;
}

CThreatsByVID::~CThreatsByVID(void)
{
}

HRESULT CThreatsByVID::FinalConstruct()
{
    if( !Initialize() )
        CCTRACEE("CThreatsByVID::FinalConstruct() - Failed to initialize.");

    return S_OK;
}

void CThreatsByVID::FinalRelease()
{
    if( m_spEraser )
    {
        unsigned long ulRef = m_spEraser.Release();
        CCTRACEI("CThreatsByVID::FinalRelease() - After releasing IEraser ref count = %d.", ulRef);
    }

    if( m_spScanner4 )
    {
        unsigned long ulRef = m_spScanner4.Release();
        CCTRACEI("CThreatsByVID::FinalRelease() - After releasing IScanner4 ref count = %d.", ulRef);
    }

    if( m_pScanProps != NULL )
    {
        unsigned long ulRef = m_pScanProps->Release();
        m_pScanProps = NULL;
        CCTRACEI("CThreatsByVID::FinalRelease() - After releasing properties sink ref count = %d.", ulRef);
    }

    if( m_pEraserScanSink != NULL )
    {
        unsigned long ulRef = m_pEraserScanSink->Release();
        m_pEraserScanSink = NULL;
        CCTRACEI("CThreatsByVID::FinalRelease() - After releasing scan sink ref count = %d.", ulRef);
    }
}

bool CThreatsByVID::Initialize()
{
    // See if we are already initialized
    if( !m_vItems.empty() )
        return true;

    SCANSTATUS Status = SCAN_OK;
    SYMRESULT symRes = SYM_OK;

    ccLib::CExceptionInfo exceptionInfo;
    try
    {
        exceptionInfo.SetFunction(_T("CThreatsByVID::Initialize()"));

        // Load ccScan
        if( SYM_FAILED(symRes = m_ccScanLoader.CreateObject(&m_spScanner4)) || m_spScanner4 == NULL )
        {
            CCTRACEE("CThreatsByVID::Initialize() - Error creating the scanner. SYMRESULT = 0x%X", symRes);
            return false;
        }

        // Always use the 32bit persistable VIDs
        long lFlags = 0;
        lFlags |= IScanner::ISCANNER_USE_PERSISTABLE_VIDS;

        // Initialize the scanning engine.
        TCHAR szTempDir[MAX_PATH];
        GetTempPath(MAX_PATH, szTempDir);
        GetShortPathName(szTempDir, szTempDir, MAX_PATH);

        Status = m_spScanner4->Initialize("NAV_OPTS", "", szTempDir, 0, lFlags);

        // Check if the initialization failed
        if( Status != SCAN_OK )
        {
            CCTRACEE("CThreatsByVID::Initialize() - Error initializing the scanner. SCANSTATUS = 0x%X", Status);
            return false;
        }

        //
        // Populate map with all non-viral VIDs
        //

        // Get the definitions information
        IScanDefinitionsInfoPtr spDefs;
        Status = m_spScanner4->GetDefinitionsInfo(&spDefs);
        if( Status != SCAN_OK || spDefs == NULL )
        {
            CCTRACEE("CThreatsByVID::Initialize() - Error getting definitions info. SCANSTATUS = 0x%X", Status);
            return false;
        }

        unsigned long ulCount = spDefs->GetCount();
        for(unsigned long ulIndex = 0; ulIndex<ulCount; ulIndex++)
        {
            IScanVirusInfoPtr spVirInfo;
            IScanVirusInfo3QIPtr spVirInfo3;
            if( SCAN_OK != (Status = spDefs->GetVirusInfo(ulIndex, &spVirInfo)) || spVirInfo == NULL )
            {
                CCTRACEE("CThreatsByVID::Initialize() - Error getting virus info for item %lu. SCANSTATUS = 0x%X", ulIndex, Status);
                continue;
            }

            spVirInfo3 = spVirInfo;
            if( spVirInfo3 == NULL )
            {
                CCTRACEE("CThreatsByVID::Initialize() - Error QI'ing for IScanVirusInfo3 interface for item %lu", ulIndex);
                continue;
            }

            // Get the categories
            DWORD dwCatCount = 0;
            const unsigned long* pculCats = NULL;
            if( SCAN_OK != spVirInfo3->GetCategories(&pculCats, dwCatCount) || pculCats == NULL )
            {
                CCTRACEE("CThreatsByVID::Initialize() - Error getting category count for item %lu", ulIndex);
                continue;
            }

            CThreatItem item;

            // See if any of the categories are classified as non-viral
            bool bNonViral = false;
            for(DWORD i=0; i<dwCatCount; i++)
            {
                if( pculCats[i] >= SCAN_THREAT_TYPE_SECURITY_RISK &&
                    pculCats[i] <= SCAN_THREAT_TYPE_JOKE)
                {
                    bNonViral = true;
                    item.m_setCategories.insert(pculCats[i]);
                }
            }

            if( bNonViral )
            {
                item.m_dwVID = spVirInfo3->GetVirusID();
                ATL::CAtlString strName;
                DWORD dwBufSize = MAX_PATH;
                Status = spVirInfo3->GetName(strName.GetBuffer(dwBufSize), &dwBufSize);
                strName.ReleaseBuffer();

                if( Status == SCAN_ERROR_BUFFER_TOO_SMALL )
                {
                    // Try again with a bigger buffer
                    dwBufSize++;
                    Status = spVirInfo3->GetName(strName.GetBuffer(dwBufSize), &dwBufSize);
                    strName.ReleaseBuffer();
                }

                if(Status != SCAN_OK)
                {
                    CCTRACEE("CThreatsByVID::Initialize() - Error getting threat name for item %lu. Status = 0x%X", ulIndex, Status);
                    continue;
                }

                // Make sure this is not one of the threats we shouldn't add
                if( -1 != strName.Find(_T("Test.SYMC.")) )
                {
                    CCTRACEI("CThreatsByVID::Initialize() - Not adding threat %s because it's a test sample.", (LPCTSTR)strName);
                    continue;
                }

                // Strip off anything before the first dot that matches our generic category names
                if( strName.Left(7) == _T("Adware.") && strName.GetLength() > 7 )
                {
                    strName = strName.Right(strName.GetLength() - 7);
                }
                else if(strName.Left(13) == _T("SecurityRisk.") && strName.GetLength() > 13)
                {
                    strName = strName.Right(strName.GetLength() - 13);
                }
                else if(strName.Left(8) == _T("Spyware.") && strName.GetLength() > 8)
                {
                    strName = strName.Right(strName.GetLength() - 8);
                }
                else if(strName.Left(7) == _T("Dialer.") && strName.GetLength() > 7)
                {
                    strName = strName.Right(strName.GetLength() - 7);
                }
                else if(strName.Left(5) == _T("Joke.") && strName.GetLength() > 5)
                {
                    strName = strName.Right(strName.GetLength() - 5);
                }
                else if(strName.Left(7) == _T("Remacc.") && strName.GetLength() > 7)
                {
                    strName = strName.Right(strName.GetLength() - 7);
                }
                else if(strName.Left(9) == _T("Hacktool.") && strName.GetLength() > 9)
                {
                    strName = strName.Right(strName.GetLength() - 9);
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

    CCTRACEI("CThreatsByVID::Initialize() - Vector contains %lu non-viral items", m_vItems.size());

    return true;
}

HRESULT CThreatsByVID::get_Count(/*[out,retval]*/ unsigned long* pulCount)
{
    // Make sure the caller is Symantec signed
    if (!IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    if( NULL == pulCount )
    {        
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
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
    // Make sure the caller is Symantec signed
    if (!IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    if( NULL == bstrThreatName )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // validate the index is good
    if( ulIndex > m_vCurFilterItems.size() )
    {
        MakeError (IDS_ERR_ANOMALYVID_EXCL_INVALID_INDEX, E_FAIL, IDS_NAVERROR_INTERNAL);
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
    // Make sure the caller is Symantec signed
    if (!IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    if( NULL == pulVID )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // validate the index is good
    if( ulIndex > m_vCurFilterItems.size() )
    {
        MakeError (IDS_ERR_ANOMALYVID_EXCL_INVALID_INDEX, E_FAIL, IDS_NAVERROR_INTERNAL);
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
    // Make sure the caller is Symantec signed
    if (!IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    if( NULL == peFilterCategory )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    *peFilterCategory = m_CurFilter;
    return S_OK;
}


HRESULT CThreatsByVID::put_Filter(/*[in]*/ long lFilterCategory)
{
    // Make sure the caller is Symantec signed
    if (!IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Make sure this is an acceptable category filter
    if( lFilterCategory == AllNonVirals ||
        lFilterCategory >= SecurityRisk &&
        lFilterCategory <= JokePrograms)
    {
        m_CurFilter = static_cast<ENonViralCategories>(lFilterCategory);
        CCTRACEI("CThreatsByVID::put_Filter() - %d is now the filter", m_CurFilter);
    }
    else
    {
        CCTRACEE("CThreatsByVID::put_Filter() - %d is an invalid filter", m_CurFilter);
        MakeError(IDS_ERR_PUT_THREATBYVID_FILTER, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CThreatsByVID::put_NAVERROR(/*[in]*/  INAVCOMError* pNavError)
{
    // Make sure the caller is Symantec signed
    if (!IsItSafe())
    {
        return E_FAIL;
    }

    if( NULL == pNavError )
    {
        return E_POINTER;
    }

    m_spError = pNavError;

    return S_OK;
}

HRESULT CThreatsByVID::get_NAVERROR(/*[out,retval]*/  INAVCOMError** ppNavError)
{
    // Make sure the caller is Symantec signed
    if (!IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

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
    // Make sure the caller is Symantec signed
    if (!IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Load ccEraser
    if( !LoadccEraser() )
        return S_FALSE;

    // Get an anomaly for this VID
    ccEraser::IAnomalyPtr spAnomaly;
    ccEraser::eResult eRes = ccEraser::Success;
    ccEraser::IEraser2QIPtr spQIEraser2 = m_spEraser;

    // If we have IEraser2 we can just get the anomaly using the VID, otherwise
    // we have to call DetectAnomalies
    if( spQIEraser2 )
    {
        CCTRACEI(_T("CThreatsByVID::DisplayDetails() - Calling IEraser2::GetAnomalyByVID() to attempt to get an anomaly for VID = %lu."), ulVID);
        eRes = spQIEraser2->GetAnomalyByVID(ulVID, spAnomaly);
        CCTRACEI(_T("CThreatsByVID::DisplayDetails() - Finished calling IEraser2::GetAnomalyByVID() eResult = %d"), eRes);

        if( ccEraser::Failed(eRes) )
        {
            CCTRACEI(_T("CThreatsByVID::DisplayDetails() - GetAnomalyByVID() did not detect an anomaly."));
        }
    }
    else
    {
        ccEraser::IAnomalyListPtr spAnomList;

        CCTRACEI(_T("CThreatsByVID::DisplayDetails() - Calling IEraser::DetectAnomalies() to attempt to get an anomaly for VID = %lu."), ulVID);
        eRes = m_spEraser->DetectAnomalies(NULL, ulVID, spAnomList);
        CCTRACEI(_T("CThreatsByVID::DisplayDetails() - Finished calling IEraser::DetectAnomalies() eResult = %d"), eRes);

        if( ccEraser::Failed(eRes) || !spAnomList )
        {
            CCTRACEW(_T("CThreatsByVID::DisplayDetails() - No anomaly list or failure from detect anomalies."));
        }
        else
        {
            // Get the anomaly
            size_t nCount = 0;
            eRes = spAnomList->GetCount(nCount);
            if( ccEraser::Failed(eRes) || nCount == 0 )
            {
                CCTRACEI(_T("CThreatsByVID::DisplayDetails() - No specific anomaly found by DetectAnomalies(). GetCount() eRes = %d. Count = %d."), eRes, nCount);
            }
            else
            {
                eRes = spAnomList->GetItem(0, spAnomaly);
                if( ccEraser::Failed(eRes) || spAnomaly == NULL )
                {
                    CCTRACEE(_T("CThreatsByVID::DisplayDetails() - Failed to get anomaly from the list. eResult = %d"), eRes);
                }
            }
        }
    }

    // Do we need to manually generate the anomaly?
    if( !spAnomaly )
    {
        CCTRACEI(_T("CThreatsByVID::DisplayDetails() - No specific anomaly found by eraser. Generating an anomaly."));
        if( !GenerateAnomaly(ulVID, spAnomaly) || spAnomaly == NULL )
        {
            CCTRACEE(_T("CThreatsByVID::DisplayDetails() - Failed to Generate an anomaly."));
            return S_FALSE;
        }
    }

    // Load the common UI
    if( !m_spCommonUI )
    {
        SYMRESULT symRes = m_CommonUILoader.CreateObject(&m_spCommonUI);
        if( SYM_FAILED(symRes) || !m_spCommonUI )
        {
            // CommonUI creation failed, let's fail gracefully
            CCTRACEE(_T("CThreatsByVID::DisplayDetails() - Failed to create the common UI. Result = 0x%X"), symRes);
            return S_FALSE;
        }
    }

    // Get the enabled pop-up window to make this modal to
    HWND hwndParent = GetWindow(reinterpret_cast<HWND>(hwnd), GW_ENABLEDPOPUP);

    if( hwndParent == NULL )
    {
        CCTRACEI(_T("CThreatsByVID::DisplayDetails() - Did not find a popup enabled window"));
        hwndParent = reinterpret_cast<HWND>(hwnd);
    }

    // Display the dialog
    m_spCommonUI->ShowDetailsDlg(spAnomaly, hwndParent);

    return S_OK;
}

bool CThreatsByVID::GenerateAnomaly(unsigned long ulVid, ccEraser::IAnomaly*& pAnomaly)
{
    if( pAnomaly != NULL )
    {
        pAnomaly->Release();
        pAnomaly = NULL;
    }

    ccEraser::eResult eRes = m_spEraser->CreateObject(ccEraser::AnomalyType, ccEraser::IID_Anomaly, reinterpret_cast<void**>(&pAnomaly));
    if( ccEraser::Failed(eRes) || pAnomaly == NULL )
    {
        CCTRACEE(_T("CThreatsByVID::GenerateAnomaly() - Failed to create a new anomaly object. eResult = %d"), eRes);
        return false;
    }

    // Add the infection data to the anomaly
    IScanVirusInfoPtr spVirInfo;
    IScanVirusInfo3QIPtr spVirInfo3;
    SCANSTATUS Status = m_spScanner4->GetVirusInfo(ulVid, &spVirInfo);
    if( Status != SCAN_OK || spVirInfo == NULL )
    {
        CCTRACEE("CThreatsByVID::GenerateAnomaly() - Error getting info for VID = %lu. SCANSTATUS = 0x%X", ulVid, Status);
        return false;
    }

    spVirInfo3 = spVirInfo;
    if( spVirInfo3 == NULL )
    {
        CCTRACEE("CThreatsByVID::GenerateAnomaly() - Error QI'ing for IScanVirusInfo3 interface for VID %lu", ulVid);
        return false;
    }

    // Get the anomaly's properties
    cc::IKeyValueCollectionPtr pProps;
    eRes = pAnomaly->GetProperties(pProps);
    if (ccEraser::Failed(eRes) || pProps == NULL )
    {
        CCTRACEE(_T("CThreatsByVID::GenerateAnomaly() - Failed to get the anomaly properties."));
        return false;
    }

    // Get the name
    ATL::CAtlString strName;
    unsigned long ulNameSize = MAX_PATH;
    Status = spVirInfo3->GetName(strName.GetBuffer(ulNameSize), &ulNameSize);
    strName.ReleaseBuffer();
    if( Status != SCAN_OK || strName.IsEmpty() )
    {
        CCTRACEE("CThreatsByVID::GenerateAnomaly() - Error getting name for VID = %lu. SCANSTATUS = 0x%X", ulVid, Status);
        return false;
    }

    // Set the name
    cc::IStringPtr pStrThreatName;
    pStrThreatName.Attach(ccSym::CStringImpl::CreateStringImpl(strName.GetString()));
    if (!pStrThreatName)
    {
        CCTRACEE(_T("CThreatsByVID::GenerateAnomaly() - Failed to allocate the anomaly name string."));
        return false;
    }

    if( !pProps->SetValue(ccEraser::IAnomaly::Name, pStrThreatName) )
    {
        CCTRACEE(_T("CThreatsByVID::GenerateAnomaly() - Failed to set the anomaly name to %s."), pStrThreatName->GetStringA());
        return false;
    }

    // Set the VID
    if( !pProps->SetValue(ccEraser::IAnomaly::VID, (DWORD)spVirInfo3->GetVirusID()) )
    {
        CCTRACEE(_T("CThreatsByVID::GenerateAnomaly() - Failed to set the anomaly VID."));
        return false;
    }

    // Create a collection to house the categories
    cc::IIndexValueCollectionPtr pCategories;
    pCategories.Attach(ccSym::CIndexValueCollectionImpl::CreateIndexValueCollectionImpl());
    if (!pCategories)
    {
        CCTRACEE(_T("CThreatsByVID::GenerateAnomaly() - Failed to allocate the anomaly category index value collection."));
        return false;
    }

    // Set anomaly categories collection in the anomaly
    if (!pProps->SetValue(ccEraser::IAnomaly::Categories, pCategories))
    {
        CCTRACEE(_T("CThreatsByVID::GenerateAnomaly() - Failed to set the anomaly categories."));
        return false;
    }

    // Get the categories count from the Virus Info
    DWORD dwCatCount = 0;
    Status = spVirInfo3->GetCategoryCount(&dwCatCount);
    if( Status != SCAN_OK || dwCatCount == 0 )
    {
        CCTRACEE("CThreatsByVID::GenerateAnomaly() - Error getting category count for VID = %lu. SCANSTATUS = 0x%X", ulVid, Status);
        return false;
    }

    // Get the categories from the Virus Info
    const unsigned long *pUlCategories = new unsigned long[dwCatCount];
    if( !pUlCategories )
    {
        CCTRACEE("CThreatsByVID::GenerateAnomaly() - Failed to allocate memory for categories.");
        return false;
    }
    Status = spVirInfo3->GetCategories(&pUlCategories, dwCatCount);
    if (Status != SCAN_OK)
    {
        CCTRACEE(_T("CThreatsByVID::GenerateAnomaly() - Failed to get the categories for VID = %lu. SCANSTATUS = 0x%X"), ulVid, Status);
        delete [] pUlCategories;
        pUlCategories = NULL;
        return false;
    }

    // Add categories
    for(DWORD i = 0; i < dwCatCount; i++ )
    {
        ccEraser::IAnomaly::Anomaly_Category iCategory = ccEraser::IAnomaly::Viral;
        switch( pUlCategories[i] )
        {
        case SCAN_THREAT_TYPE_VIRUS:                iCategory = ccEraser::IAnomaly::Viral; break;
        case SCAN_THREAT_TYPE_MALICIOUS:            iCategory = ccEraser::IAnomaly::Malicious; break;
        case SCAN_THREAT_TYPE_RESERVED_MALICIOUS:   iCategory = ccEraser::IAnomaly::ReservedMalicious; break;
        case SCAN_THREAT_TYPE_HEURISTIC:            iCategory = ccEraser::IAnomaly::Heuristic; break;
        case SCAN_THREAT_TYPE_SECURITY_RISK:        iCategory = ccEraser::IAnomaly::SecurityRisk; break;
        case SCAN_THREAT_TYPE_HACKTOOL:             iCategory = ccEraser::IAnomaly::Hacktool; break;
        case SCAN_THREAT_TYPE_SPYWARE:              iCategory = ccEraser::IAnomaly::SpyWare; break;
        case SCAN_THREAT_TYPE_TRACKWARE:            iCategory = ccEraser::IAnomaly::Trackware; break;
        case SCAN_THREAT_TYPE_DIALER:               iCategory = ccEraser::IAnomaly::Dialer; break;
        case SCAN_THREAT_TYPE_REMOTE_ACCESS:        iCategory = ccEraser::IAnomaly::RemoteAccess; break;
        case SCAN_THREAT_TYPE_ADWARE:               iCategory = ccEraser::IAnomaly::Adware; break;
        case SCAN_THREAT_TYPE_JOKE:                 iCategory = ccEraser::IAnomaly::Joke; break;
        case SCAN_THREAT_TYPE_UNKNOWN:
        default:
            continue;
        }
        if (!pCategories->AddValue(DWORD(iCategory)))
        {
            CCTRACEE(_T("CThreatsByVID::GenerateAnomaly() - Failed to add the %d -th category."), i);
            delete [] pUlCategories;
            pUlCategories = NULL;
            return false;
        }
    }

    // We are done with the categories we allocated
    delete [] pUlCategories;
    pUlCategories = NULL;

    // Set the auto-created flag since this is not specific
    if( !pProps->SetValue(ccEraser::IAnomaly::AutoCreated, true) )
    {
        CCTRACEE(_T("CThreatsByVID::GenerateAnomaly() - Failed to set the auto created flag."));
    }

    // Create a new remediation action list for the anomaly
    ccEraser::IRemediationActionListPtr spRemediationList;
    eRes = m_spEraser->CreateObject(ccEraser::RemediationActionListType, ccEraser::IID_RemediationActionList, reinterpret_cast<void**>(&spRemediationList));
    if( ccEraser::Failed(eRes) || spRemediationList == NULL )
    {
        CCTRACEE(_T("CThreatsByVID::GenerateAnomaly() - Failed to create a new remediaton action list. eResult = %d"), eRes);
        return false;
    }

    // Add the remediation action list to the anomaly
    eRes = pAnomaly->SetRemediationActions(spRemediationList);
    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CThreatsByVID::GenerateAnomaly() - Failed to add the new remediaton action list to the anomaly. eResult = %d"), eRes);
        return false;
    }

    return true;
}

bool CThreatsByVID::LoadccEraser()
{
    if( !m_spEraser )
    {
        // If we don't have a ccScan there will be problems, it should
        // have been created in the initialization of this class
        if( !m_spScanner4 )
        {
            CCTRACEE("CThreatsByVID::LoadccEraser() - No ccScan object.");
            return false;
        }

        ATL::CAtlString strEraserPath;
        CScanPtr <IScanDefinitionsInfo> spDefInfo;
        SCANSTATUS scanRes = m_spScanner4->GetDefinitionsInfo ( &spDefInfo );
        if ( SCAN_OK != scanRes || !spDefInfo )
        {
            CCTRACEE("CThreatsByVID::LoadccEraser() - Failed to get definitions info. SCANRESULT = 0x%X.", scanRes);
            return false;
        }

        strEraserPath = spDefInfo->GetDefinitionsDirectory();

        // Append the dll name
        strEraserPath += _T("\\");
        strEraserPath += cc::sz_ccEraser_dll;

        CCTRACEI(_T("CThreatsByVID::LoadccEraser() - Loading IEraser from %s"), (LPCTSTR)strEraserPath);
        SYMRESULT symRes = m_ccEraserLoader.Initialize(strEraserPath);
        if( SYM_FAILED(symRes) )
        {
            CCTRACEE("CThreatsByVID::LoadccEraser() - Failed to initialize eraser loader. SYMRESULT = 0x%X.", symRes);
            return false;
        }

        symRes = m_ccEraserLoader.CreateObject(ccEraser::IID_Eraser, ccEraser::IID_Eraser, reinterpret_cast<void**>(&m_spEraser));
        if( SYM_FAILED(symRes) || !m_spEraser )
        {
            CCTRACEE("CThreatsByVID::LoadccEraser() - Failed to create eraser object. SYMRESULT = 0x%X.", symRes);
            return false;
        }

        // Create the sinks
        m_pScanProps = new CScanProperties;
        if( !m_pScanProps )
        {
            CCTRACEE("CThreatsByVID::LoadccEraser() - Failed to create the properties sink.");
            return false;
        }

        // Ref count this for ourselves
        m_pScanProps->AddRef();

        m_pEraserScanSink = new CEraserScanSink;
        if( !m_pEraserScanSink )
        {
            CCTRACEE("CThreatsByVID::LoadccEraser() - Failed to create the scan sink.");
            return false;
        }

        // Ref count this for ourselves
        m_pEraserScanSink->AddRef();

        // Now initialize eraser
        ccEraser::eResult resEraser = m_spEraser->Initialize(m_spScanner4, m_pEraserScanSink, m_pScanProps);

        if( ccEraser::Failed(resEraser) )
        {
            CCTRACEE(_T("CThreatsByVID::LoadccEraser() - Error initializing IEraser. ccEraser::eResult = %d"), resEraser);
            return false;
        }
        CCTRACEI(_T("CThreatsByVID::LoadccEraser() - IEraser is loaded"));
    }

    return true;
}