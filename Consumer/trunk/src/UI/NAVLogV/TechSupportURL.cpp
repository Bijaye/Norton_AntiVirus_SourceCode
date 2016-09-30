////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TechSupportURL.cpp: implementation of the CTechSupportURL class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StahlSoft.h"
#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"
#include "NAVVersion.h"

#include <string>
using namespace std;
#include "TechSupportURL.h"
#include "..\navlogvres\resource.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTechSupportURL::CTechSupportURL()
{
    CNAVVersion Version;
    m_strProduct = Version.GetProductAbbr ();
    
    TCHAR szVersion [100] = {0};
    wsprintf ( szVersion, _T("%u.%u"), Version.GetMajorVersion (), Version.GetMinorVersion ());

    m_strVersion = szVersion;
    CResourceHelper::LoadResourceString ( IDS_LANGUAGE, m_strLanguage, _Module.GetResourceInstance() );
    
    LPCTSTR szBuildType = Version.GetBuildType();
    m_strBuildType = (NULL != szBuildType) ? szBuildType : _T("");
    
    m_strURL = _T("");
    CResourceHelper::LoadResourceString (IDS_TECHSUPPORT_URL, m_strURLPattern, _Module.GetResourceInstance());
}

CTechSupportURL::~CTechSupportURL()
{

}

LPCTSTR CTechSupportURL::GetURL ( long lModuleID,
                                  long lErrorID )
{
	TCHAR szEncoded[1024] = {0};
    tstring szParameters;

	// Module ID.

	wsprintf(szEncoded, _T("module=%u"), lModuleID);
	szParameters.append(szEncoded);

	// Error ID.
	szParameters.append(_T("&"));
	wsprintf(szEncoded, _T("error=%u"), lErrorID);
	szParameters.append(szEncoded);
	
	// Language.
	szParameters.append(_T("&"));
	szParameters.append(_T("language="));
	szParameters.append(m_strLanguage.c_str());

	// Is there a Product ID?
	szParameters.append(_T("&"));
	szParameters.append(_T("product="));
	szParameters.append(m_strProduct.c_str());

	// Is there a version number?
	szParameters.append(_T("&"));
	szParameters.append(_T("version="));
	szParameters.append(m_strVersion.c_str());

	// Build type
    szParameters.append(_T("&"));
	szParameters.append(_T("build="));
	szParameters.append(m_strBuildType.c_str());
    
    // URL + search parameters
    //
    m_strURL = m_strURLPattern;
    m_strURL += szParameters;

    return m_strURL.c_str();
}
