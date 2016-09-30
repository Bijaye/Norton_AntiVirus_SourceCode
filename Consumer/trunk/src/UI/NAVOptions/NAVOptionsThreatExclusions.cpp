////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVOptionsPathExclusions.cpp

#include "stdafx.h"
#include "NAVOptionsObj.h"
#include "..\navoptionsres\resource.h"
#include "isErrorResource.h"
#include "isErrorIDs.h"
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
bool CNAVOptions::Compare_ByExclusionName(AVExclusions::IExclusion* pExclusion1, AVExclusions::IExclusion* pExclusion2)
{
    if( pExclusion1 == NULL || pExclusion2 == NULL )
    {
        CCTRACEE(_T("CNavOptions::Compare_ByExclusionName - invalid item."));
        return false;
    }

    cc::IStringPtr pStrName1;
    cc::IStringPtr pStrName2;
    
    if( AVExclusions::Success != pExclusion1->getDisplayName(pStrName1) ||
        AVExclusions::Success != pExclusion2->getDisplayName(pStrName2) )
    {
        CCTRACEE(_T("CNavOptions::Compare_ByExclusionName - Failed to get display name."));
        return false;
    }

    ATL::CAtlStringW strName1 = pStrName1->GetStringW();
    ATL::CAtlStringW strName2 = pStrName2->GetStringW();

    if( strName1 < strName2 )
        return true;

    return false;
}

// Loads/Re-Loads the threat exclusions from the files
HRESULT CNAVOptions::LoadExclusions(bool bReload)
{
    CCTRCTXI0(_T("Start."));

    if( !m_bExclusionsLoaded || bReload )
    {
        AVExclusions::ExResult exRes;
		SYMRESULT sr;
        // Load the threat exclusions list
        m_vFSExclusions.clear();
        m_vAnomalyExclusions.clear();

        CCTRCTXI0(_T("(Re)Loading exclusions."));

        if(!m_spExclusionMgr)
        {
            CCTRCTXI0(_T("No manager found, loading DLL."));

            // Load the factory/lifetime manager, if necessary (requires COM)
            if(forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) ||
                !SYM_SUCCEEDED(sr = m_ExclusionFactory.Initialize()))
            {
				CCTRCTXE1(_T("Error initializing exclusion factory object. SR=0x%08x"),sr);
                MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
                return E_FAIL;
            }

            // Create the exclusion manager object.
            if(forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) ||
                !SYM_SUCCEEDED(m_ExclusionFactory.CreateObject(m_spExclusionMgr)))
            {
				CCTRCTXE1(_T("Error creating Exclusion Manager. SR=0x%08x"),sr);
                MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
                return E_FAIL;
            }

            if(forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) || !m_spExclusionMgr)
            {
				CCTRCTXE0(_T("Exclusion Manager is null after creation!"));
                MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
                return E_FAIL;
            }

            // Load exclusions from CC Settings & set up
            exRes = m_spExclusionMgr->initialize(false);
            if(forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) ||
                exRes != AVExclusions::Success)
            {
				CCTRCTXE1(_T("Exclusion Manager failed to init. ExRes = %d"),(UINT)exRes);
                MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
                return E_FAIL;
            }

            m_bExclusionsDirty = false;
        }
        // No else - if we've already loaded once, we don't want to reload
        // until the whole UI reloads.

        CCTRCTXI0(_T("Enumerating exclusions."));

        // Reset the exclusion enumeration
        exRes = m_spExclusionMgr->enumReset();
        if(forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) ||
            exRes != AVExclusions::Success)
        {
			CCTRCTXE1(_T("Error resetting exclusion enumeration. ExRes = %d"),(UINT)exRes);
            MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
            return E_FAIL;
        }

        AVExclusions::IExclusionPtr excluItem;
        AVExclusions::IExclusion::ExclusionType eType;

        while((exRes = m_spExclusionMgr->enumGetNextExclusionObj(excluItem)) == AVExclusions::Success)
        {
            // Triage: Anomalies into Anomaly list, FS into FS list.
            eType = excluItem->getExclusionType();
            if(eType == AVExclusions::IExclusion::ExclusionType::FILESYSTEM)
            {
                m_vFSExclusions.push_back(excluItem);
            }
            else if(eType == AVExclusions::IExclusion::ExclusionType::ANOMALY)
            {
                m_vAnomalyExclusions.push_back(excluItem);
            }
        }

        // Sort the anomaly exclusions by name
        if( !m_vAnomalyExclusions.empty() )
        {
            sort(m_vAnomalyExclusions.begin(), m_vAnomalyExclusions.end(), Compare_ByExclusionName);
        }

        CCTRCTXI0(_T("(Re)Load OK."));
        m_bExclusionsLoaded = true;
    }

    return S_OK;
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
        MakeError(ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
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
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    // Get path of default exclusions file
	ccLib::CString defaultExcluFile;
	if( !ccSym::CInstalledApps::GetInstAppsDirectory(_T("AntiVirus"), defaultExcluFile) )
	{
		CCTRACEE(_T("CNAVOptions::Default() - Error, could not find CAV InstalledApps Director"));
		MakeError(IDS_ERR_SET_THREAT_DEFAULTS, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        m_spError->LogAndDisplay(0);
	}
	ccLib::CStringConvert::AppendPath(defaultExcluFile, EXCLUSIONS_DEFAULTS_FILE);
	
	if(  forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) || m_spExclusionMgr->loadFromFile(&defaultExcluFile,
								(AVExclusions::IExclusion::ExclusionState)0,
								AVExclusions::IExclusion::ExclusionType::ANOMALY) 
					!= AVExclusions::Success)
    {
        MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    m_bExclusionsDirty = true;

    // Force a reload of the threat exclusions
    if( FAILED(hr = LoadExclusions(true)) )
        return hr;

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_AnomalyExclusionCount(long *pVal)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

	if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
	{
		MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
		return E_POINTER;
	}

	*pVal = 0;

	// Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    *pVal = m_vAnomalyExclusions.size();

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_AnomalyExclusionItemName(long index, BSTR *pVal)
{
    cc::IStringPtr spStr = NULL;

    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

	if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
	{
		MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
		return E_POINTER;
	}

	// Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    // Make sure the index is valid
    if( forceError (IDS_ERR_THREAT_EXCL_INVALID_INDEX) || index < 0 || index > (long)m_vAnomalyExclusions.size() )
    {
        MakeError (IDS_ERR_THREAT_EXCL_INVALID_INDEX, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    if(forceError ( ERR_INVALID_POINTER ) ||
        m_vAnomalyExclusions[index]->getDisplayName(spStr) != AVExclusions::Success ||
        !spStr)
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    *pVal = ::SysAllocString(spStr->GetStringW());

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_AnomalyExclusionItemVid(long index, unsigned long* pVal)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    // Make sure the index is valid
    if( forceError (IDS_ERR_THREAT_EXCL_INVALID_INDEX) || index < 0 || index > (long)m_vAnomalyExclusions.size() )
    {
        MakeError (IDS_ERR_THREAT_EXCL_INVALID_INDEX, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    AVExclusions::IAnomalyExclusionQIPtr anom;

    if(forceError ( ERR_INVALID_POINTER ) ||
        (anom = m_vAnomalyExclusions[index]) == NULL)
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    *pVal = anom->getVid();

    return S_OK;
}

STDMETHODIMP CNAVOptions::SetAnomalyExclusionItem(unsigned long ulVid, BSTR bstrThreatName, EExclusionAddResult *peResult)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    if( SysStringLen(bstrThreatName) == 0 )
    {
        CCTRCTXE0(_T("You must specify a threat name."));
        MakeError (IDS_ERR_ADD_THREAT_EXCL_ITEM, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == peResult )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

	// Load the threat exclusions
	HRESULT hr;
	if( FAILED(hr = LoadExclusions()) )
		return hr;

    // Initialize return value
    *peResult = ExclusionAddResult_Failed;

    // Get an IString for the threat name
    cc::IStringPtr spStrThreat;
    spStrThreat.Attach(ccSym::CStringImpl::CreateStringImpl(bstrThreatName));
    if(forceError (ERR_INVALID_POINTER) || !spStrThreat)
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    AVExclusions::IExclusionPtr pTemp;

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
        ULONG eState = AVExclusions::IExclusion::ExclusionState::EXCLUDE_BOTH;

        // Add the item to the exclusion system and retrieve its EID
        ullNewItem = m_spExclusionMgr->addExclusionAnomaly(ulVid, NULL, 0, spStrThreat,
            static_cast<AVExclusions::IExclusion::ExclusionState>(eState));

        if( forceError (IDS_ERR_ADD_THREAT_EXCL_ITEM) || _INVALID_ID == ullNewItem)
        {
            CCTRCTXE0(_T("Anomaly exclusion add failed."));
            MakeError (IDS_ERR_ADD_THREAT_EXCL_ITEM, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
            return E_FAIL;
        }

        // Add the new exclusion to the internal list
        if( m_spExclusionMgr->getExclusion(ullNewItem, pTemp) != AVExclusions::Success
            || pTemp == NULL)
        {
            // Error
            MakeError (IDS_ERR_ADD_THREAT_EXCL_ITEM, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
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
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    // Validate the index to remove
    if( forceError (ERR_INVALID_ARG) ||
        index < 0 || index > (long) m_vAnomalyExclusions.size() )
    {
        MakeError (ERR_INVALID_ARG, E_INVALIDARG, IDS_ISSHARED_ERROR_INTERNAL);
        return E_INVALIDARG;
    }

    // Get the item to remove
    AVExclusions::ExclusionId exId = m_vAnomalyExclusions[index]->getID();
    if( forceError(IDS_ERR_REMOVE_THREAT_EXCL_ITEM) || exId <= _INVALID_ID )
    {
        MakeError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    // No - Remove it from the exclusions library
    if( forceError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM) || 
        !m_spExclusionMgr->removeExclusion(exId) )
    {
        MakeError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    // Remove it from the internal collection
    m_vAnomalyExclusions.erase(m_vAnomalyExclusions.begin()+index);
    m_bExclusionsDirty = true;

    return S_OK;
}

/*** FS EXCLUSION STUFF ***/

// Restores the default filesystem exclusion set
STDMETHODIMP CNAVOptions::SetPathExclusionDefaults()
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
    {
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
        return E_FAIL;
    }

    // Load the path exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

	// Get path of default exclusions file
	ccLib::CString defaultExcluFile;
	if( !ccSym::CInstalledApps::GetInstAppsDirectory(_T("AntiVirus"), defaultExcluFile) )
	{
		CCTRACEE(_T("CNAVOptions::Default() - Error, could not find CAV InstalledApps Director"));
		MakeError(IDS_ERR_SET_THREAT_DEFAULTS, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        m_spError->LogAndDisplay(0);
	}
	ccLib::CStringConvert::AppendPath(defaultExcluFile, EXCLUSIONS_DEFAULTS_FILE);

	// Unfiltered filesystem reset
    if( forceError(IDS_ERR_THREAT_EXCL_FAILED_LOAD) ||
		m_spExclusionMgr->loadFromFile(&defaultExcluFile,
								(AVExclusions::IExclusion::ExclusionState)0, 
								AVExclusions::IExclusion::ExclusionType::FILESYSTEM) 
				!= AVExclusions::Success)
    {
        MakeError(IDS_ERR_THREAT_EXCL_FAILED_LOAD, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    m_bExclusionsDirty = true;

    // Reload the path exclusions
    if( FAILED(hr = LoadExclusions(true)) )
        return hr;

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_PathExclusionCount(long *pVal)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

	if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
	{
		MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
		return E_POINTER;
	}

    *pVal = 0;

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    *pVal = m_vFSExclusions.size();

	return S_OK;
}

STDMETHODIMP CNAVOptions::get_PathExclusionItemPath(long index, BSTR *pVal)
{
    cc::IStringPtr spStr = NULL;

    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

	if( forceError ( ERR_INVALID_POINTER ) || NULL == pVal )
	{
		MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
		return E_POINTER;
	}

	// Make sure the index is valid
	if( forceError (IDS_ERR_THREAT_EXCL_INVALID_INDEX) || index < 0 || index > (long) m_vFSExclusions.size() )
	{
		MakeError (IDS_ERR_THREAT_EXCL_INVALID_INDEX, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
		return E_FAIL;
	}
	
	// Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    if(forceError ( ERR_INVALID_POINTER ) ||
        m_vFSExclusions[index]->getDisplayName(spStr) != AVExclusions::Success ||
        !spStr)
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    *pVal = ::SysAllocString(spStr->GetStringW());

	return S_OK;
}

STDMETHODIMP CNAVOptions::SetPathExclusionItem(long index, BSTR bstrPath, BOOL bSubFolders, EExclusionAddResult *peResult)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

	if( forceError ( ERR_INVALID_POINTER ) || NULL == peResult )
	{
		MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
		return E_POINTER;
	}
	
	// Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

	// Initialize return value
	*peResult = ExclusionAddResult_Failed;

    // Make sure an Exclusion item exists at the index requested to commit
    if( forceError (ERR_INVALID_ARG) ||
        index < 0 || index > (long) m_vFSExclusions.size()+1 )
    {
        MakeError (ERR_INVALID_ARG, E_INVALIDARG, IDS_ISSHARED_ERROR_INTERNAL);
        return E_INVALIDARG;
    }

    // Hackfix for defect 1-4A5NT6: Exclusions dialog within Nav Options takes 
    // too long to open (when there are excessive FS exclusions). Fixing by
    // imposing a UI-level cap of 200 on FS exclusions.
    if(index > 200 || m_vFSExclusions.size() >= 200)
    {
        // Piggybacking on the AP exclusion max
        *peResult = ExclusionAddResult_TooMany;
        return S_OK;
    }

    cc::IStringPtr spStr;
    spStr.Attach(ccSym::CStringImpl::CreateStringImpl(bstrPath));
    if(forceError (ERR_INVALID_POINTER) || !spStr)
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    // Determine whether an exclusion with the same path currently exists.
    AVExclusions::ExclusionId dupeId = m_spExclusionMgr->findExclusionFS(spStr);

    AVExclusions::IExclusionPtr pTemp = NULL;

    // Check if the exclusion is a replacement
    if( m_vFSExclusions.size()+1 != index )
    {   // Replace an existing item
        pTemp = m_vFSExclusions[index];

        // QI so we can access business data.
        AVExclusions::IFileSystemExclusionQIPtr pQI = pTemp;
        if(forceError (ERR_INVALID_POINTER) || !pQI)
        {
            MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
            return E_POINTER;
        }

        // Do not allow the replacement of the exclusion if another exclusion with the
        // same path exists.  This code will not behave very well if the replacement
        // path is not being changed and another exclusion with the same path currently
        // exists (pre-existing dupe).  Unfortunately this limitation comes from the
        // current AVExclusions functionality (as much as I understand it).
        if( dupeId != _INVALID_ID &&
            dupeId != pQI->getID() )
        {
            *peResult = ExclusionAddResult_Duplicate;
            return S_OK;
        }

        // Lop off backslash for match testing.
        CStringW cswBuffer(bstrPath);
        cc::IStringPtr spBuffer;
        if(cswBuffer[cswBuffer.GetLength()-1] == '\\')
        {
            cswBuffer.Delete(cswBuffer.GetLength()-1,1);
            spBuffer.Attach(ccSym::CStringImpl::CreateStringImpl(cswBuffer.GetString()));
        }
        else
        {
            spBuffer = spStr;
        }

        // See if we're changing the path.
        if(pQI->isExactMatch(spBuffer) != AVExclusions::Success)
        {
			// We're changing the path!
			CCTRCTXI0(_T("Editing exclusion path."));

			if(forceError (IDS_ERR_EDIT_THREAT_EXCL_ITEM) ||
				pQI->setFSPath(spStr) != AVExclusions::Success)
			{
				MakeError (IDS_ERR_EDIT_THREAT_EXCL_ITEM, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
				return E_FAIL;
			}
		}
		// Else: Not changing the path, just edit the flags.

		// Edit the subdirs flag.
		if(forceError (IDS_ERR_EDIT_THREAT_EXCL_ITEM) ||
			pQI->setStateFlag(AVExclusions::IExclusion::ExclusionState::EXCLUDE_INCLUDE_SUBDIRS,
			(bSubFolders == TRUE?true:false)) != AVExclusions::Success)
		{
			MakeError (IDS_ERR_EDIT_THREAT_EXCL_ITEM, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
			return E_FAIL;
		}

		// Replace it in the original list - this will handle
		// merging if need be
		if( forceError (IDS_ERR_EDIT_THREAT_EXCL_ITEM) ||
			m_spExclusionMgr->replaceExclusion(pQI->getID(), pQI)
			!= AVExclusions::Success )
		{
			MakeError (IDS_ERR_EDIT_THREAT_EXCL_ITEM, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
			return E_FAIL;
		}

		// Edit/Update - Return success
		*peResult = ExclusionAddResult_New;
		m_bExclusionsDirty = true;
		return S_OK;
	}

    CCTRCTXI0(_T("Creating new exclusion."));

    // New exclusion
    UINT uResult = 0;
    ULONGLONG ullNewItem = 0;
	ULONG eState = AVExclusions::IExclusion::ExclusionState::EXCLUDE_BOTH | 
		(bSubFolders==TRUE ? AVExclusions::IExclusion::ExclusionState::EXCLUDE_INCLUDE_SUBDIRS :
	AVExclusions::IExclusion::ExclusionState::NOT_EXCLUDED);

    // Do not allow the new exclusion if an exclusion with the same path currently
    // exists.
    if( dupeId != _INVALID_ID )
    {
        *peResult = ExclusionAddResult_Duplicate;
        return S_OK;
    }

	// Add the item to the exclusion system and retrieve its EID
    ullNewItem = m_spExclusionMgr->addExclusionFS(spStr, 0, 
                (AVExclusions::IExclusion::ExclusionState)eState);

    if( forceError (IDS_ERR_ADD_THREAT_EXCL_ITEM) || _INVALID_ID == ullNewItem)
    {
        MakeError (IDS_ERR_ADD_THREAT_EXCL_ITEM, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    // Add the new exclusion to the internal list
    if( forceError (IDS_ERR_ADD_THREAT_EXCL_ITEM) ||
        m_spExclusionMgr->getExclusion(ullNewItem, pTemp) != AVExclusions::Success
            || pTemp == NULL)
    {
        // Error
        MakeError (IDS_ERR_ADD_THREAT_EXCL_ITEM, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    m_vFSExclusions.push_back(pTemp);
    *peResult = ExclusionAddResult_New;
    m_bExclusionsDirty = true;

	return S_OK;
}

STDMETHODIMP CNAVOptions::RemovePathExclusionItem(long index)
{
    // Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    // Validate the index to remove
    if( forceError (ERR_INVALID_ARG) ||
        index < 0 || index > (long) m_vFSExclusions.size() )
    {
        MakeError (ERR_INVALID_ARG, E_INVALIDARG, IDS_ISSHARED_ERROR_INTERNAL);
        return E_INVALIDARG;
    }

    // Get the item to remove
    AVExclusions::ExclusionId exId = m_vFSExclusions[index]->getID();
    if( forceError(IDS_ERR_REMOVE_THREAT_EXCL_ITEM) || exId <= _INVALID_ID )
    {
        MakeError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

        // Remove it from the exclusions library
    if( forceError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM) || 
        !m_spExclusionMgr->removeExclusion(exId) )
    {
        MakeError (IDS_ERR_REMOVE_THREAT_EXCL_ITEM, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    // Remove it from the internal collection
    m_vFSExclusions.erase(m_vFSExclusions.begin()+index);
    m_bExclusionsDirty = true;

    return S_OK;
}

STDMETHODIMP CNAVOptions::get_PathExclusionItemSubFolder(long index, BOOL *pSubfolders)
{
	// Make sure the caller is Symantec signed
    if (forceError (IDS_ERR_IT_IS_NOT_SAFE))
	{
        MakeError(IDS_ERR_IT_IS_NOT_SAFE, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL_REINSTALL);
	    return E_FAIL;
    }

    if( forceError ( ERR_INVALID_POINTER ) || NULL == pSubfolders )
    {
        MakeError (ERR_INVALID_POINTER, E_POINTER, IDS_ISSHARED_ERROR_INTERNAL);
        return E_POINTER;
    }

    // Load the threat exclusions
    HRESULT hr;
    if( FAILED(hr = LoadExclusions()) )
        return hr;

    // Make sure the index is valid
    if( forceError (IDS_ERR_THREAT_EXCL_INVALID_INDEX) || index < 0 || index > (long) m_vFSExclusions.size() )
    {
        MakeError (IDS_ERR_THREAT_EXCL_INVALID_INDEX, E_FAIL, IDS_ISSHARED_ERROR_INTERNAL);
        return E_FAIL;
    }

    *pSubfolders = (m_vFSExclusions[index]->getStateFlag(AVExclusions::IExclusion::ExclusionState::EXCLUDE_INCLUDE_SUBDIRS)
		== AVExclusions::Success) ? TRUE : FALSE;

	return S_OK;
}
