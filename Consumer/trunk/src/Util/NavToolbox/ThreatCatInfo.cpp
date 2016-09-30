////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <atlbase.h>
#include <atlconv.h>

#include "NAVInfo.h"
#include "ThreatCatInfo.h"

#include <time.h>
#include "ccScanInterface.h"        // Threat category defines
#include <ccEraserInterface.h>      // More threat cat defines

CThreatCatInfo::CThreatCatInfo()
{
    // Load NAVOpts.dll which has the threat cat resource strings
    TCHAR szNavOptsDll[MAX_PATH] = {0};
    CNAVInfo navInfo;
    _tcscpy(szNavOptsDll, navInfo.GetNAVDir());
    _tcscat(szNavOptsDll, _T("\\NAVOpts.loc"));
    m_hNavOptsDll = LoadLibraryEx(szNavOptsDll, NULL, LOAD_LIBRARY_AS_DATAFILE);
}

CThreatCatInfo::~CThreatCatInfo()
{
    if( m_hNavOptsDll )
        FreeLibrary(m_hNavOptsDll);
}

BOOL CThreatCatInfo::GetCategoryText ( LPCSTR pcszCategoryNums, std::vector<tstring>& vecResults )
{
    // Create a copy of the category numbers to parse
    TCHAR* pszCatNums = new TCHAR[_tcslen(CA2T(pcszCategoryNums))+1];
    if( !pszCatNums )
        return FALSE;

    _tcscpy(pszCatNums, CA2T(pcszCategoryNums));

    // Parse the categories
    TCHAR *pszToken = _tcstok(pszCatNums, _T(" "));

    TCHAR szNextCat[MAX_PATH] = {0};
    long lCat = 0;
    long lIndex = 0;

    while( NULL != pszToken )
    {
        // convert to a number
        lCat = _ttol(pszToken);

        // Get the category
        if( GetCategoryText(lCat, szNextCat, MAX_PATH) )
        {
            vecResults[lIndex] = szNextCat;
        }
        
        pszToken = _tcstok(NULL, _T(" "));
    }

    // Free our copy of the category numbers
    delete [] pszCatNums;

    if( 0 == vecResults.size () )
        return FALSE;
    
    return TRUE;
}

// Get the text associated with a category
BOOL CThreatCatInfo::GetCategoryText(const unsigned long culCat, LPTSTR pszCategory, DWORD dwSize)
{
    if( !m_hNavOptsDll )
        return FALSE;

    // Don't show generic or client compliancy categories
    if( culCat == ccEraser::IAnomaly::ClientCompliancy || culCat == ccEraser::IAnomaly::GenericLoadPoint )
        return FALSE;

    // If the resource cannot be loaded then use "Unknown"
    if( 0 == LoadString(m_hNavOptsDll, THREAT_RESOURCE_BASE_STRING+culCat, pszCategory, dwSize) )
    {
        if( 0 == LoadString(m_hNavOptsDll, UNKNOWN_THREAT_RESOURCE_STRING, pszCategory, dwSize) )
            return FALSE;
    }

    return TRUE;
}

// Get the text and descriptionassociated with a category
BOOL CThreatCatInfo::GetCategoryTextAndDesc(const unsigned long culCat, 
											LPTSTR pszCategory, 
											DWORD dwCategorySize,
											LPTSTR pszDescription, 
											DWORD dwDescriptionSize)
{
	if (!GetCategoryText(culCat, pszCategory, dwCategorySize) || 
        0 == LoadString(m_hNavOptsDll, 
		                THREAT_RESOURCE_DESC_BASE_STRING + culCat, 
						pszDescription, dwDescriptionSize) )
    {
		return FALSE;
	}

	return TRUE;
}

BOOL CThreatCatInfo::GetCategoryText(/*in*/LPCTSTR pcszCategoryNums, /*out*/LPTSTR pszCategoryList, DWORD dwSize)
{
    // Clear the output buffer
    ZeroMemory(pszCategoryList, dwSize);

    // Create a copy of the category numbers to parse
    TCHAR* pszCatNums = new TCHAR[_tcslen(pcszCategoryNums)+1];
    if( !pszCatNums )
        return FALSE;

    _tcscpy(pszCatNums, pcszCategoryNums);

    // Parse the categories
    TCHAR *pszToken = _tcstok(pszCatNums, _T(" "));

    TCHAR szNextCat[MAX_PATH] = {0};
    long lCat = 0;

    while( NULL != pszToken )
    {
        // convert to a number
        lCat = _ttol(pszToken);

        // Get the category
        if( GetCategoryText(lCat, szNextCat, MAX_PATH) )
        {
            if( 0 == _tcslen(pszCategoryList) )
                _tcsncpy(pszCategoryList, szNextCat, dwSize);
            else
            {
                _tcsncat(pszCategoryList, _T("\\"), dwSize - _tcslen(pszCategoryList) -1);
                _tcsncat(pszCategoryList, szNextCat, dwSize - _tcslen(pszCategoryList) -1);
            }
        }
        
        pszToken = _tcstok(NULL, _T(" "));
    }

    // Free our copy of the category numbers
    delete [] pszCatNums;

    if( 0 == _tcslen(pszCategoryList) )
        return FALSE;
    
    return TRUE;
}

BOOL CThreatCatInfo::GetCategoryText (/*in*/cc::IIndexValueCollection* pCategories, /*out*/LPTSTR pszCategoryList, DWORD dwSize )
{
    if( pCategories == NULL )
        return FALSE;

    TCHAR szNextCat[MAX_PATH] = {0};
    size_t nCatCount = pCategories->GetCount();
    for( size_t nCur = 0; nCur < nCatCount; nCur++ )
    {
        DWORD dwCategory = 0;
        if( !pCategories->GetValue(nCur, dwCategory) )
        {
            continue;
        }

        // Get the category
        if( GetCategoryText(dwCategory, szNextCat, MAX_PATH) )
        {
            if( 0 == _tcslen(pszCategoryList) )
                _tcsncpy(pszCategoryList, szNextCat, dwSize);
            else
            {
                _tcsncat(pszCategoryList, _T("\\"), dwSize - _tcslen(pszCategoryList) -1);
                _tcsncat(pszCategoryList, szNextCat, dwSize - _tcslen(pszCategoryList) -1);
            }
        }
    }

    return TRUE;
}

BOOL CThreatCatInfo::GetCategoryTextAndDesc (/*in*/cc::IIndexValueCollection* pCategories, 
											 /*out*/LPTSTR pszCategoryList, 
											 DWORD dwCategoryListSize ,
											 /*out*/LPTSTR pszDescriptionList, 
											 DWORD dwDescriptionListSize )
{
    if( pCategories == NULL )
        return FALSE;

    TCHAR szNextCat[MAX_PATH] = {0};
    TCHAR szNextDesc[MAX_DESC] = {0};
    size_t nCatCount = pCategories->GetCount();

    for( size_t nCur = 0; nCur < nCatCount; nCur++ )
    {
        DWORD dwCategory = 0;
        if( !pCategories->GetValue(nCur, dwCategory) )
        {
            continue;
        }

        // Get the category
        if( GetCategoryTextAndDesc(dwCategory, szNextCat, MAX_PATH, szNextDesc, MAX_DESC) )
        {
			if( 0 == _tcslen(pszCategoryList) ) {
                _tcsncpy(pszCategoryList, szNextCat, dwCategoryListSize);
                _tcsncpy(pszDescriptionList, szNextDesc, dwDescriptionListSize);
			}
            else {
                _tcsncat(pszCategoryList, _T("\\"), dwCategoryListSize - _tcslen(pszCategoryList) -1);
                _tcsncat(pszCategoryList, szNextCat, dwCategoryListSize - _tcslen(pszCategoryList) -1);
                _tcsncat(pszDescriptionList, _T("\\"), dwDescriptionListSize - _tcslen(pszDescriptionList) -1);
                _tcsncat(pszDescriptionList, szNextDesc, dwDescriptionListSize - _tcslen(pszDescriptionList) -1);
           }
        }
    }

    return TRUE;
}