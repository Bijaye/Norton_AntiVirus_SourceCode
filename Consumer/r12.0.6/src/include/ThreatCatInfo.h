// Threat Categorization info header file
//
#pragma once

#include <string>
#include <vector>

#include "ccIndexValueCollectionInterface.h"

// The resource strings for each known category are stored starting at
// this resource ID in the string table of NAVOpts
#define THREAT_RESOURCE_BASE_STRING 1
#define THREAT_RESOURCE_DESC_BASE_STRING 51

// Resource ID for unknown threat string
#define UNKNOWN_THREAT_RESOURCE_STRING 0

#define MAX_DESC 500

class CThreatCatInfo
{
public:
    CThreatCatInfo();
    ~CThreatCatInfo();

    // Get the text associated with a single category
    BOOL GetCategoryText(const unsigned long culCat, LPTSTR pszCategory, DWORD dwSize);

    // Get the text and description associated with a single category
    BOOL GetCategoryTextAndDesc(const unsigned long culCat,
		                        /*out*/LPTSTR pszCategoryList, 
						        DWORD dwDescriptionSize,
						        /*out*/LPSTR pszDescriptionList,
						        DWORD dsDescriptionSize);

    // Get the text associated with a string of space delimited categories
    // The list of categories returned will be separated by backslashes
    BOOL GetCategoryText(/*in*/LPCTSTR pcszCategoryNums, /*out*/LPTSTR pszCategoryList, DWORD dwSize);

    BOOL GetCategoryText (/*in*/LPCSTR pcszCategoryNums, /*out*/std::vector<std::string>& vecResults );

    BOOL GetCategoryText (/*in*/cc::IIndexValueCollection* pCategories, /*out*/LPTSTR pszCategoryList, DWORD dwSize );
    
    // Get the text and description associated with a string of space delimited 
	// categories.  The list of categories returned will be separated by backslashes
	BOOL GetCategoryTextAndDesc (/*in*/cc::IIndexValueCollection* pCategories, 
		                         /*out*/LPTSTR pszCategoryList, 
						         DWORD dwDescriptionSize,
						         /*out*/LPSTR pszDescriptionList,
						         DWORD dsDescriptionSize);

private:
    HMODULE m_hNavOptsDll;
};