// NAVVersion.cpp: implementation of the CNAVVersion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NAVVersion.h"
#include <crtdbg.h>
#include "StahlSoft.h"
#include "SSOsinfo.h"
#include "NAVSettingsHelperEx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNAVVersion::CNAVVersion()
{
    m_strProductAbbr = _T("NAV");
    m_ulMajorVersion = 0;
    m_ulMinorVersion = 0;
    m_ulInlineVersion = 0;
    m_ulBuildNumber = 0;
    
    // Read our version number from VERSION.DAT.
    //
    TCHAR szVersion[80] = {0};
    TCHAR szVersionWord[80] = {0};
    TCHAR szPublicRevision [80] = {0};
    TCHAR szIsOEM[80] = {0};                // OEM = "1"
    TCHAR szVendorName[MAX_PATH] = {0};

    CNAVInfo NAVInfo;
    tstring strVersionDAT = NAVInfo.GetNAVDir ();

    strVersionDAT += _T("\\version.dat");

    // Read the public revision number.

    GetPrivateProfileString ( _T("Versions"),
                              _T("PublicRevision"),
                              _T("*"), 
                              szPublicRevision, 
                              80, 
                              strVersionDAT.c_str() );

    // Did we get the version?
    if ( 0 != lstrcmp ( szVersion, _T("*") ))
    {
        m_strPublicRevision = szPublicRevision;

        TCHAR* pszTemp;
        pszTemp = _tcstok ( szPublicRevision, _T(".") );
        
        if ( pszTemp )
        {
            m_ulMajorVersion = _ttol ( pszTemp );
            pszTemp = _tcstok ( NULL, _T(".") );
        }
    
        if ( pszTemp )
        {
            m_ulMinorVersion = _ttol ( pszTemp );
            pszTemp = _tcstok ( NULL, _T(".") );
        }

        if ( pszTemp )
        {
            m_ulInlineVersion = _ttol ( pszTemp );
            pszTemp = _tcstok ( NULL, _T(".") );
        }
    
        if ( pszTemp )
        {
            m_ulBuildNumber = _ttol ( pszTemp );
            pszTemp = _tcstok ( NULL, _T(".") );

            if ( pszTemp)
                m_strPartial = pszTemp;
        }

        // OEM information
        //
        if (GetPrivateProfileString(_T("Versions"),
                                    _T("NAVOEM"), 
                                    _T("0"), 
								    szIsOEM, 
                                    sizeof(szIsOEM), 
                                    strVersionDAT.c_str()) > 0)
	    {
		    if (_tcscmp(szIsOEM, _T("1")) == 0)
		    {
			    GetPrivateProfileString(_T("Versions"),
                                        _T("OEMVendor"),
                                        _T(""),
										szVendorName, 
                                        sizeof(szVendorName), 
                                        strVersionDAT.c_str());
		    }
	    }

        // Get the language
        //
        TCHAR szLanguage[10] = {0};

        if ( GetPrivateProfileString ( _T("Versions"),
                                       _T("Language"),
                                       _T("EN"), 
                                       szLanguage, 
                                       sizeof (szLanguage), 
                                       strVersionDAT.c_str()) > 0 )
        {
            m_strLanguageCode = szLanguage;
        }
    }
}

CNAVVersion::~CNAVVersion()
{

}

LPCTSTR CNAVVersion::GetProductAbbr()
{
    return m_strProductAbbr.c_str();
}

ULONG CNAVVersion::GetMajorVersion()
{
    return m_ulMajorVersion;
}

ULONG CNAVVersion::GetMinorVersion()
{
    return m_ulMinorVersion;
}

ULONG CNAVVersion::GetInlineVersion()
{
    return m_ulInlineVersion;
}

ULONG CNAVVersion::GetBuildNumber()
{
    return m_ulBuildNumber;
}

LPCTSTR CNAVVersion::GetBuildType()
{
    // Get the build target (Retail, OEM, etc.)
    //
    if ( m_strBuildType.empty ())
    {
        CNAVOptSettingsEx NavOpts;

	    if ( !NavOpts.Init() )
		    return NULL;

        // Enabled - Default to Enabled
        //
        TCHAR szBuildType[MAX_PATH] = {0};

	    if ( FAILED(NavOpts.GetValue(CC_ERR_BUILD_TYPE, szBuildType, MAX_PATH, _T("Retail") )))
		    return NULL;

        m_strBuildType = szBuildType;
    }

    return m_strBuildType.c_str();
}

LPCTSTR CNAVVersion::GetPartial()
{
    return m_strPartial.c_str();
}

LPCTSTR CNAVVersion::GetPublicRevision()
{
    return m_strPublicRevision.c_str();
}

LPCTSTR CNAVVersion::GetLanguageCode()
{
    return m_strLanguageCode.c_str();
}