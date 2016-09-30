// NAVOptionsThreatExclusions.cpp

#include "stdafx.h"
#include "NAVOptionsObj.h"
#include "..\navoptionsres\resource.h"
#include "NAVErrorResource.h"
#include "ExclusionFileLoader.h"
#include "ccSymStringImpl.h"  // ccSym::CStringImpl
#include <algorithm>

//
// Threat Exclusion functions
//
// DEFAULT EXCLUSION FILE: defexcl.dat
//

// Comparison method used to sort items by display name in our vector of
// exclusions
bool CNAVOptions::Compare_ByExclusionName(NavExclusions::IExclusion* pExclusion1, NavExclusions::IExclusion* pExclusion2)
{
    if( pExclusion1 == NULL || pExclusion2 == NULL )
    {
        CCTRACEE(_T("CNavOptions::Compare_ByExclusionName - invalid item."));
        return false;
    }

    cc::IStringPtr pStrName1;
    cc::IStringPtr pStrName2;
    
    if( NavExclusions::Success != pExclusion1->getDisplayName(pStrName1) ||
        NavExclusions::Success != pExclusion2->getDisplayName(pStrName2) )
    {
        CCTRACEE(_T("CNavOptions::Compare_ByExclusionName - Failed to get display name."));
        return false;
    }

    ATL::CAtlString strName1 = pStrName1->GetStringA();
    ATL::CAtlString strName2 = pStrName2->GetStringA();

    if( strName1 < strName2 )
        return true;

    return false;
}

// Loads/Re-Loads the threat exclusions from the files
HRESULT CNAVOptions::LoadExclusions(bool bReload)
{
    CCTRACEI(_T("CNavOptions::LoadExclusions - Start."));

    if( !m_bExclusionsLoaded || bReload )
    {
        NavExclusions::ExResult exRes;
        // Load the threat exclusions list
        m_vFSExclusions.clear();
        m_vAnomalyExclusions.clear();

        CCTRACEI(_T("CNavOptions::LoadExclusions - (Re)Loading exclusions."));

        if(!m_spExclusionMgr)
        {
            CCTRACEI(_T("CNavOptions::LoadExclusions - No manager found, loading DLL."));

            // Load the factory/lifetime manager, if necessary (requires COM)
            if(forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) ||
                !SYM_SUCCEEDED(m_ExclusionFactory.Initialize()))
            {
                MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }

            CCTRACEI(_T("CNavOptions::LoadExclusions - DLL Loaded, creating."));

            // Create the exclusion manager object.
            if(forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) ||
                !SYM_SUCCEEDED(m_ExclusionFactory.CreateObject(m_spExclusionMgr)))
            {
                MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }

            if(forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) || !m_spExclusionMgr)
            {
                MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }

            CCTRACEI(_T("CNavOptions::LoadExclusions - Object created, initializing and loading."));

            // Load exclusions from CC Settings & set up
            exRes = m_spExclusionMgr->initialize(false);
            if(forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) ||
                exRes != NavExclusions::Success)
            {
                MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }
            CCTRACEI(_T("CNavOptions::LoadExclusions - Object initialized OK."));
            m_bExclusionsDirty = false;
        }
        // No else - if we've already loaded once, we don't want to reload
        // until the whole UI reloads.

        CCTRACEI(_T("CNavOptions::LoadExclusions - Resetting enumeration."));

        // Reset the exclusion enumeration
        exRes = m_spExclusionMgr->enumReset();
        if(forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) ||
            exRes != NavExclusions::Success)
        {
            MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }

        NavExclusions::IExclusionPtr excluItem;
        NavExclusions::IExclusion::ExclusionType eType;

        CCTRACEI(_T("CNavOptions::LoadExclusions - Populating exclusion list(s)."));

        while((exRes = m_spExclusionMgr->enumGetNextExclusionObj(excluItem)) == NavExclusions::Success)
        {
            // Filter out improper items
            if(excluItem->getStateFlag((m_bExclusionsAreViral?
                       NavExclusions::IExclusion::ExclusionState::EXCLUDE_VIRAL:
                       NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL))
                == NavExclusions::Success)
            {
                CCTRACEI(_T("CNavOptions::LoadExclusions - Found a valid item."));

                // Filter out Anomalies if we're filesystem-related
                eType = excluItem->getExclusionType();
                if(eType == NavExclusions::IExclusion::ExclusionType::FILESYSTEM)
                {
                    CCTRACEI(_T("CNavOptions::LoadExclusions - Added FS item."));
                    m_vFSExclusions.push_back(excluItem);
                }
                else if(eType == NavExclusions::IExclusion::ExclusionType::ANOMALY)
                {
                    CCTRACEI(_T("CNavOptions::LoadExclusions - Added Anomaly item."));
                    m_vAnomalyExclusions.push_back(excluItem);
                }
            }
        }

        // Sort the anomaly exclusions by name
        if( !m_vAnomalyExclusions.empty() )
        {
            sort(m_vAnomalyExclusions.begin(), m_vAnomalyExclusions.end(), Compare_ByExclusionName);
        }

        CCTRACEI(_T("CNavOptions::LoadExclusions - (Re)Load OK."));
        m_bExclusionsLoaded = true;
    }
    CCTRACEI(_T("CNavOptions::LoadExclusions - Finished."));
    return S_OK;
}

STDMETHODIMP CNAVOptions::get_ExclusionType(/*[out]*/BOOL *pIsViral)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pIsViral)
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    *pIsViral = (this->m_bExclusionsAreViral)?VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
}

STDMETHODIMP CNAVOptions::put_ExclusionType(/*[out]*/BOOL bIsViral)
{
    CCTRACEI(_T("CNAVOptions::put_ExclusionType - Being called."));
    bool bViral = (bIsViral == VARIANT_TRUE)?true:false;
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    if(this->m_bExclusionsAreViral != bViral)
    {
        CCTRACEI(_T("Exclusion Type changing - forcing reload of exclusions."));
        this->m_bExclusionsAreViral = bViral;
        return this->LoadExclusions(true);
    }
    else
    {
        return S_OK;
    }
}

STDMETHODIMP CNAVOptions::ValidateExclusionPath(BSTR bstrExclusion, EExclusionValidateResult *pResult)
{
    USES_CONVERSION;

    if ( ::SysStringLen ( bstrExclusion ) > MAX_PATH )
        return E_POINTER;

    PWCHAR pszExclusion = OLE2W(bstrExclusion);
//    PTCHAR pszExclusion = OLE2T(bstrExclusion);
    WCHAR szExtension [_MAX_EXT] = {0};
    WCHAR* pszNext = NULL;

    if (forceError (ERR_INVALID_POINTER) ||
        !pResult)
    {
        MakeError(ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // Get the extension
    ::_wsplitpath(pszExclusion, NULL, NULL, NULL, szExtension);

    // Don't allow UNC Path exclusions
    if( NULL != wcsstr(pszExclusion, L"\\\\") )
    {
        *pResult = EExclusionValidateResult_Fail;
        return S_OK;
    }

    // Check if it's an extension exclusion (i.e: *.xxx or .xxx).
    WCHAR* pszWildCard = wcschr(pszExclusion, WCHAR('*'));
    if( (NULL != pszWildCard && pszWildCard == pszExclusion && (*CharNextW(pszWildCard)) == WCHAR('.'))
        || (*pszExclusion) == WCHAR('.') )
    {
        //
        // Now we have an extension exclusion so let's make sure it's a valid extension
        //
        WCHAR* pszAfterDot = CharNextW(wcschr(pszExclusion, WCHAR('.')));

        // Make sure there are characters after the *. and it's not *.*

        if( 0 == wcscmp(pszExclusion, L"*.*") )
        {
            *pResult = EExclusionValidateResult_Fail_AllPaths;
            return S_OK;
        }

        if( !pszAfterDot || WCHAR('\0') == *pszAfterDot )
        {
            *pResult = EExclusionValidateResult_Fail;
            return S_OK;
        }

        // Now make sure the characters are not \/:"<>| which are invalid extension characters
        // Also exclude spaces since our script processing does not support it.
        int nFound = wcscspn(pszAfterDot, L"\\/:\"<>| .");

        if( nFound != wcslen(pszAfterDot) )
        {
            *pResult = EExclusionValidateResult_Fail;
            return S_OK;
        }

        // This is a valid extension exclusion
        *pResult = EExclusionValidateResult_Success;
        return S_OK;
    }

    // Do not allow wildcard characters or invalid characters in the path
    if( NULL != wcspbrk(pszExclusion, L"/*?\"<>|") )
    {
        // Invalid characters found
        *pResult = EExclusionValidateResult_WildcardInPath;
        return S_OK;
    }

    // Special invalid character case: the colon
    WCHAR* colPos = wcsrchr(pszExclusion, WCHAR(':'));
    if( colPos != NULL )
    {
        // There's a : here somewhere; there's only one valid location.
        // Is it there?
        if( colPos == CharNextW(pszExclusion) ) // Array position 1
        {

            if(iswalpha(pszExclusion[0]) == 0)
            {
                // Nonalphanumeric character before :, invalid
                *pResult = EExclusionValidateResult_WildcardInPath;
                return S_OK;
            }
            // Else, (alpha): marks the legitimate use of a :

            // Two valid cases: Next char is either null (meaning X:) or
            // a backslash (X:\[...]).
            pszNext = CharNextW(colPos);
            if(pszNext && *pszNext != WCHAR(0) && *pszNext != WCHAR('\\'))
            {
                *pResult = EExclusionValidateResult_Fail;
                return S_OK;
            }
        }
        else
        {   // : elsewhere in path, invalid
            *pResult = EExclusionValidateResult_WildcardInPath;
            return S_OK;
        }
    }
    // Null, not found.

    // Block filename sections made up entirely of dots (regexp: \[.]+(?:\\|$))
    WCHAR* pszSlashDot = wcsstr(pszExclusion, L"\\.");
    if(pszSlashDot)
    {   // We found \\. somewhere; Windows Explorer won't like it, but it can be done..
        pszNext = CharNextW(pszSlashDot);

        // Step through the dots.
        while(pszNext && *pszNext == WCHAR('.'))
        { pszNext = CharNextW(pszNext); }

        // First non-dot char; must NOT be null or backslash. Null str means error.
        if(!pszNext || *pszNext == WCHAR('\0') || *pszNext == WCHAR('\\'))
        {
            *pResult = EExclusionValidateResult_Fail;
            return S_OK;
        }
    }
    // No \\., so none of those four can be found

    // If we get here the path is valid
    *pResult = EExclusionValidateResult_Success;
    return S_OK;
}

/*** ANOMALY EXCLUSION STUFF ***/
STDMETHODIMP CNAVOptions::SetAnomalyExclusionDefaults()
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    NavExclusions::IExclusion::ExclusionState eState;
    eState = (m_bExclusionsAreViral? 
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_VIRAL :
    NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL);

    if( forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) ||
        m_spExclusionMgr->resetToDefaults(eState, 
        NavExclusions::IExclusion::ExclusionType::ANOMALY) != NavExclusions::Success)
    {
        MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    m_bExclusionsDirty = true;

    // Load the threat exclusions
    if( FAILED(hr = LoadExclusions(true)) )
        return hr;

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_AnomalyExclusionCount(long *pVal)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    *pVal = 0;

    *pVal = m_vAnomalyExclusions.size();

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_AnomalyExclusionItemPath(long index, BSTR *pVal)
{
    cc::IStringPtr spStr = NULL;

    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // Make sure the index is valid
    if( forceError (IDS_ERR_THREAT_EXCL_INVALID_INDEX) || index < 0 || index > m_vAnomalyExclusions.size() )
    {
        MakeError (IDS_ERR_THREAT_EXCL_INVALID_INDEX, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    if(forceError ( ERR_INVALID_POINTER ) ||
        m_vAnomalyExclusions[index]->getDisplayName(spStr) != NavExclusions::Success ||
        !spStr)
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    *pVal = ::SysAllocString(spStr->GetStringW());

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_AnomalyExclusionItemVID(long index, unsigned long *pulVID)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pulVID )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    *pulVID = 0;

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    // Make sure the index is valid
    if( forceError (IDS_ERR_THREAT_EXCL_INVALID_INDEX) || index < 0 || index > m_vAnomalyExclusions.size() )
    {
        MakeError (IDS_ERR_THREAT_EXCL_INVALID_INDEX, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    // Get an IAnoamlyExclusion so we can get the VID
    NavExclusions::IAnomalyExclusionQIPtr spQIAnomExclusion = m_vAnomalyExclusions[index];

    if( !spQIAnomExclusion )
    {
        CCTRACEE(_T("CNAVOptions::get_AnomalyExclusionItemVID - Failed to QI for the anomaly exclusion interface."));
        MakeError (ERR_INVALID_ARG, E_NOINTERFACE, IDS_Err_Internal);
        return E_NOINTERFACE;
    }

    *pulVID = spQIAnomExclusion->getVid();

    return S_OK;
}

STDMETHODIMP CNAVOptions::SetAnomalyExclusionItem(unsigned long ulVid, BSTR bstrThreatName, BOOL bSubFolders, EExclusionAddResult *peResult)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    if( SysStringLen(bstrThreatName) == 0 )
    {
        CCTRACEE(_T("CNAVOptions::SetAnomalyExclusionItem() - You must specify a threat name."));
        MakeError (IDS_ERR_ADD_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == peResult )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // Initialize return value
    *peResult = ExclusionAddResult_Failed;

    // Get an IString for the threat name
    cc::IStringPtr spStrThreat = ccSym::CStringImpl::CreateStringImpl(bstrThreatName);
    if(forceError (ERR_INVALID_POINTER) || !spStrThreat)
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    NavExclusions::IExclusionPtr pTemp;

    UINT uResult = 0;
    ULONGLONG ullNewItem = 0;

    // Check for dupes
    ullNewItem = m_spExclusionMgr->findExclusionAnomaly(ulVid, NULL);
    if(ullNewItem != _INVALID_ID)
    {
        // Duplicate
        *peResult = ExclusionAddResult_Duplicate;
    }
    else
    {
        ULONG eState = NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL;

        eState |= (bSubFolders==TRUE ? 
            NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_NONVIRAL:
        NavExclusions::IExclusion::ExclusionState::NOT_EXCLUDED);

        // Add the item to the exclusion system and retrieve its EID
        ullNewItem = m_spExclusionMgr->addExclusionAnomaly(ulVid, NULL, 0, spStrThreat,
            static_cast<NavExclusions::IExclusion::ExclusionState>(eState));

        if( forceError (IDS_ERR_ADD_THREAT_EXCL_ITEM) || _INVALID_ID == ullNewItem)
        {
            CCTRACEE(_T("CNAVOptions::SetAnomalyExclusionItem() - Anomaly excluion add failed."));
            MakeError (IDS_ERR_ADD_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }

        // Add the new exclusion to the internal list
        if( m_spExclusionMgr->getExclusion(ullNewItem, pTemp) != NavExclusions::Success
            || pTemp == NULL)
        {
            // Error
            MakeError (IDS_ERR_ADD_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }

        m_vAnomalyExclusions.push_back(pTemp);

        // Re-sort the list
         sort(m_vAnomalyExclusions.begin(), m_vAnomalyExclusions.end(), Compare_ByExclusionName);
        *peResult = ExclusionAddResult_New;
        m_bExclusionsDirty = true;
    }

    return S_OK;
}

STDMETHODIMP CNAVOptions::RemoveAnomalyExclusionItem(long index)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    // Validate the index to remove
    if( forceError (ERR_INVALID_ARG) ||
        index < 0 || index > m_vAnomalyExclusions.size() )
    {
        MakeError (ERR_INVALID_ARG, E_INVALIDARG, IDS_NAVERROR_INTERNAL);
        return E_INVALIDARG;
    }

    // Get the item to remove
    NavExclusions::ExclusionId exId = m_vAnomalyExclusions[index]->getID();
    if( forceError(IDS_ERR_REMOVE_THREAT_EXCL_ITEM) || exId <= _INVALID_ID )
    {
        MakeError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    // OR out the appropriate V/NV flag.
    if( m_vAnomalyExclusions[index]->setStateFlag((NavExclusions::IExclusion::ExclusionState)
        (m_bExclusionsAreViral? 
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_VIRAL |
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_VIRAL :
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL |
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_NONVIRAL),false)
            != NavExclusions::Success )
    {
        MakeError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    // Do we still have the other flag set?
    if(m_vAnomalyExclusions[index]->getState() & 
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_BOTH)
    {
        // Yes - Replace the exclusion to update the state
        if( forceError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM) || 
            m_spExclusionMgr->replaceExclusion(exId, m_vAnomalyExclusions[index]) 
            != NavExclusions::Success )
        {
            MakeError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
    }
    else
    {
        // No - Remove it from the exclusions library
        if( forceError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM) || 
            !m_spExclusionMgr->removeExclusion(exId) )
        {
            MakeError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
    }


    // Remove it from the internal collection
    m_vAnomalyExclusions.erase(m_vAnomalyExclusions.begin()+index);
    m_bExclusionsDirty = true;

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_AnomalyExclusionItemSubFolder(long index, BOOL *pSubfolders)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pSubfolders )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    // Make sure the index is valid
    if( forceError (IDS_ERR_THREAT_EXCL_INVALID_INDEX) || index < 0 || index > m_vAnomalyExclusions.size() )
    {
        MakeError (IDS_ERR_THREAT_EXCL_INVALID_INDEX, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    *pSubfolders = (m_vAnomalyExclusions[index]->getStateFlag(
        (m_bExclusionsAreViral ?
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_VIRAL:
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_NONVIRAL))
        == NavExclusions::Success) ? TRUE : VARIANT_FALSE;

    return S_OK;
}
/*** FS EXCLUSION STUFF ***/

// Restores the default files
STDMETHODIMP CNAVOptions::SetThreatExclusionDefaults()
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    NavExclusions::IExclusion::ExclusionState eState;
    eState = (m_bExclusionsAreViral? 
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_VIRAL :
    NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL);

    if( forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) ||
        m_spExclusionMgr->resetToDefaults(eState, 
        NavExclusions::IExclusion::ExclusionType::FILESYSTEM) != NavExclusions::Success)
    {
        MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    m_bExclusionsDirty = true;

    // Load the threat exclusions
    if( FAILED(hr = LoadExclusions(true)) )
        return hr;

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_ThreatExclusionCount(long *pVal)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    *pVal = 0;

    *pVal = m_vFSExclusions.size();

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_ThreatExclusionItemPath(long index, BSTR *pVal)
{
    cc::IStringPtr spStr = NULL;

    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

	if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // Make sure the index is valid
    if( forceError (IDS_ERR_THREAT_EXCL_INVALID_INDEX) || index < 0 || index > m_vFSExclusions.size() )
    {
        MakeError (IDS_ERR_THREAT_EXCL_INVALID_INDEX, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    if(forceError ( ERR_INVALID_POINTER ) ||
        m_vFSExclusions[index]->getDisplayName(spStr) != NavExclusions::Success ||
        !spStr)
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    *pVal = ::SysAllocString(spStr->GetStringW());

	return S_OK;
}

// This function has been superceded by the version that includes subfolders
STDMETHODIMP CNAVOptions::SetThreatExclusionItem(long index, BSTR bstrPath, EExclusionAddResult *peResult)
{
    return E_NOTIMPL;
}

STDMETHODIMP CNAVOptions::SetThreatExclusionItemWithSubFolder(long index, BSTR bstrPath, BOOL bSubFolders, EExclusionAddResult *peResult)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

	if( forceError ( ERR_INVALID_POINTER ) || NULL == peResult )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // Make sure an Exclusion item exists at the index requested to commit
    if( forceError (ERR_INVALID_ARG) ||
        index < 0 || index > m_vFSExclusions.size()+1 )
    {
        MakeError (ERR_INVALID_ARG, E_INVALIDARG, IDS_NAVERROR_INTERNAL);
        return E_INVALIDARG;
    }

    // Hackfix for defect 1-4A5NT6: Excusions dialog within Nav Options takes 
    // too long to open (when there are excessive FS exclusions). Fixing by
    // imposing a UI-level cap of 200 on FS exclusions.
    if(index > 200 || m_vFSExclusions.size() >= 200)
    {
        // Piggybacking on the AP exclusion max
        *peResult = ExclusionAddResult_TooMany;
        return S_OK;
    }

    // Initialize return value
    *peResult = ExclusionAddResult_Failed;

    // Get ANSI and OEM strings for the exclusion item
    // The exclusions library stores the path in OEM and the
    // path is displayed in ANSI
    cc::IStringPtr spStr = ccSym::CStringImpl::CreateStringImpl(bstrPath);
    if(forceError (ERR_INVALID_POINTER) || !spStr)
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    NavExclusions::IExclusionPtr pTemp = NULL;
    ULONG eState = (m_bExclusionsAreViral?
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_VIRAL:
    NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL);

    // Check if the exclusion is a replacement
    if( m_vFSExclusions.size()+1 != index )
    {
        //
        // Replace an existing item
        //

        pTemp = m_vFSExclusions[index];

        // QI so we can access business data.
        NavExclusions::IFileSystemExclusionQIPtr pQI = pTemp;
        if(forceError (ERR_INVALID_POINTER) || !pQI)
        {
            MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
            return E_POINTER;
        }

        // Lop off backslash for match testing.
        CStringW cswBuffer(bstrPath);
        cc::IStringPtr spBuffer;
        if(cswBuffer[cswBuffer.GetLength()-1] == '\\')
        {
            cswBuffer.Delete(cswBuffer.GetLength()-1,1);
            spBuffer = ccSym::CStringImpl::CreateStringImpl(cswBuffer.GetString());
        }
        else
        {
            spBuffer = spStr;
        }

        // See if we're changing the path.
        if(pQI->isExactMatch(spBuffer) == NavExclusions::Success)
        {
            // No semantical path change. Edit the flags.
            if(forceError (IDS_ERR_EDIT_THREAT_EXCL_ITEM) ||
                pTemp->setStateFlag(
                (m_bExclusionsAreViral?
                NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_VIRAL:
                NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_NONVIRAL),
                (bSubFolders == TRUE?true:false)) != NavExclusions::Success)
            {
                MakeError (IDS_ERR_EDIT_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }

            // Replace it in the original list - this will handle
            // merging if need be
            if( forceError (IDS_ERR_EDIT_THREAT_EXCL_ITEM) ||
                m_spExclusionMgr->replaceExclusion(pTemp->getID(), pTemp)
                != NavExclusions::Success )
            {
                MakeError (IDS_ERR_EDIT_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }

            // Return success
            *peResult = ExclusionAddResult_New;
            m_bExclusionsDirty = true;
            return S_OK;
        }
        
        if(pTemp->getStateFlag((m_bExclusionsAreViral? 
            NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL :
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_VIRAL)) == NavExclusions::Success)
        {   // If this is a dual-flag exclusion, we'll unset the flag from the
            // current item.
            CCTRACEI(_T("CNAVOptions::SetThreatExclusionWithSubfolder - Found "\
                "a dual-state exclusion for edit; splitting exclusion."));

            if( forceError(IDS_ERR_EDIT_THREAT_EXCL_ITEM) ||
                pTemp->setStateFlag((NavExclusions::IExclusion::ExclusionState)
                (m_bExclusionsAreViral? 
                NavExclusions::IExclusion::ExclusionState::EXCLUDE_VIRAL |
                NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_VIRAL :
                NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL |
                NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_NONVIRAL), 
                false) != NavExclusions::Success )
            {
                MakeError (IDS_ERR_EDIT_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }

            // Replace it in the original list
            if( forceError (IDS_ERR_EDIT_THREAT_EXCL_ITEM) ||
                m_spExclusionMgr->replaceExclusion(pTemp->getID(), pTemp)
                != NavExclusions::Success )
            {
                MakeError (IDS_ERR_EDIT_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }
            m_bExclusionsDirty = true;
        }
        else
        {   // Else, proceed normally with edit
            CCTRACEI(_T("CNAVOptions::SetThreatExclusionWithSubfolder - Found "\
                        "a single-state exclusion for edit; proceeding normally."));

            // Edit the item
            if(forceError (IDS_ERR_EDIT_THREAT_EXCL_ITEM) ||
                pQI->setStateFlag( (m_bExclusionsAreViral ?
                NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_VIRAL:
                NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_NONVIRAL),
                (bSubFolders == TRUE?true:false)) != NavExclusions::Success)
            {
                MakeError (IDS_ERR_EDIT_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }

            if(forceError (IDS_ERR_EDIT_THREAT_EXCL_ITEM) ||
                pQI->setFSPath(spStr) != NavExclusions::Success)
            {
                MakeError (IDS_ERR_EDIT_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }

            // Replace it in the original list - this will handle
            // merging if need be
            if( forceError (IDS_ERR_EDIT_THREAT_EXCL_ITEM) ||
                m_spExclusionMgr->replaceExclusion(pQI->getID(), pTemp)
                != NavExclusions::Success )
            {
                MakeError (IDS_ERR_EDIT_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }

            // Return success
            *peResult = ExclusionAddResult_New;
            m_bExclusionsDirty = true;
            return S_OK;
        }
    }

    CCTRACEI(_T("CNAVOptions::SetThreatExclusionWithSubfolder - Creating new exclusion."));

     // else: new exclusion/split exclusion
    UINT uResult = 0;
    ULONGLONG ullNewItem = 0;

    // Ensure we're not adding a dupe if this isn't a split
    if(pTemp == NULL)
    {
        NavExclusions::ExclusionId dupeId = m_spExclusionMgr->findExclusionFS(spStr);
        // See if there's already an exclusion with the current path
        if(dupeId != _INVALID_ID)
        {
            NavExclusions::IExclusionPtr pExTmp;
            if(m_spExclusionMgr->getExclusion(dupeId, pExTmp) == NavExclusions::Success)
            {
                // If we have the same flag, it's a dupe.
                if(pExTmp->getStateFlag((NavExclusions::IExclusion::ExclusionState)eState) 
                    == NavExclusions::Success)
                {
                    *peResult = ExclusionAddResult_Duplicate;
                    return S_OK;
                }
                // Else, internal merge logic will handle this case, continue.
            }
            else if( forceError(IDS_ERR_ADD_THREAT_EXCL_ITEM) )
            {
                // Can't retrieve the exclusion - some weird internal error.
                MakeError (IDS_ERR_ADD_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
                return E_FAIL;
            }
        }
    }

    eState |= (bSubFolders==TRUE ? (m_bExclusionsAreViral ? 
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_VIRAL:
    NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_NONVIRAL):
    NavExclusions::IExclusion::ExclusionState::NOT_EXCLUDED);

    // Add the item to the exclusion system and retrieve its EID
    ullNewItem = m_spExclusionMgr->addExclusionFS(spStr, 0, 
                (NavExclusions::IExclusion::ExclusionState)eState);

    if( forceError (IDS_ERR_ADD_THREAT_EXCL_ITEM) || _INVALID_ID == ullNewItem)
    {
        MakeError (IDS_ERR_ADD_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    // Add the new exclusion to the internal list
    if( forceError (IDS_ERR_ADD_THREAT_EXCL_ITEM) ||
        m_spExclusionMgr->getExclusion(ullNewItem, pTemp) != NavExclusions::Success
            || pTemp == NULL)
    {
        // Error
        MakeError (IDS_ERR_ADD_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    m_vFSExclusions.push_back(pTemp);
    *peResult = ExclusionAddResult_New;
    m_bExclusionsDirty = true;

	return S_OK;
}

STDMETHODIMP CNAVOptions::RemoveThreatExclusionItem(long index)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    // Validate the index to remove
    if( forceError (ERR_INVALID_ARG) ||
        index < 0 || index > m_vFSExclusions.size() )
    {
        MakeError (ERR_INVALID_ARG, E_INVALIDARG, IDS_NAVERROR_INTERNAL);
        return E_INVALIDARG;
    }

    // Get the item to remove
    NavExclusions::ExclusionId exId = m_vFSExclusions[index]->getID();
    if( forceError(IDS_ERR_REMOVE_THREAT_EXCL_ITEM) || exId <= _INVALID_ID )
    {
        MakeError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    // OR out the appropriate V/NV flag.
    if( forceError(IDS_ERR_REMOVE_THREAT_EXCL_ITEM) ||
        m_vFSExclusions[index]->setStateFlag((NavExclusions::IExclusion::ExclusionState)
        (m_bExclusionsAreViral? 
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_VIRAL |
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_VIRAL :
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_NONVIRAL |
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_NONVIRAL),false)
        != NavExclusions::Success)
    {
        MakeError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    // Do we still have the other flag set?
    if(m_vFSExclusions[index]->getState() & 
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_BOTH)
    {
        // Yes - Replace the exclusion to update the state
        if( forceError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM) || 
            m_spExclusionMgr->replaceExclusion(exId, m_vFSExclusions[index])
            != NavExclusions::Success )
        {
            MakeError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
    }
    else
    {
        // No - Remove it from the exclusions library
        if( forceError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM) || 
            !m_spExclusionMgr->removeExclusion(exId) )
        {
            MakeError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM, E_FAIL, IDS_NAVERROR_INTERNAL);
            return E_FAIL;
        }
    }

    // Remove it from the internal collection
    m_vFSExclusions.erase(m_vFSExclusions.begin()+index);
    m_bExclusionsDirty = true;

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_ThreatExclusionItemSubFolder(long index, BOOL *pSubfolders)
{
	// Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE) ||
        !IsItSafe())
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_NAVERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pSubfolders )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_NAVERROR_INTERNAL);
        return E_POINTER;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    // Make sure the index is valid
    if( forceError (IDS_ERR_THREAT_EXCL_INVALID_INDEX) || index < 0 || index > m_vFSExclusions.size() )
    {
        MakeError (IDS_ERR_THREAT_EXCL_INVALID_INDEX, E_FAIL, IDS_NAVERROR_INTERNAL);
        return E_FAIL;
    }

    *pSubfolders = (m_vFSExclusions[index]->getStateFlag(
        (m_bExclusionsAreViral ? 
        NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_VIRAL:
    NavExclusions::IExclusion::ExclusionState::EXCLUDE_SUBDIRS_NONVIRAL))
             == NavExclusions::Success) ? TRUE : FALSE;

	return S_OK;
}

