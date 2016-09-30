// TechSupportURL.h: interface for the CTechSupportURL class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TECHSUPPORTURL_H__47E228E6_4346_4852_BD29_951140B811AF__INCLUDED_)
#define AFX_TECHSUPPORTURL_H__47E228E6_4346_4852_BD29_951140B811AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resourcehelper.h"
#include "resource.h"
#include <string>

class CTechSupportURL  
{
public:
	LPCTSTR GetURL( long lModuleID,
                    long lErrorID );

	CTechSupportURL();
	virtual ~CTechSupportURL();

protected:
    ::std::string m_strLanguage;
    ::std::string m_strProduct;
    ::std::string m_strURL;
    ::std::string m_strVersion;
    ::std::string m_strURLPattern;
    ::std::string m_strBuildType;
};

#endif // !defined(AFX_TECHSUPPORTURL_H__47E228E6_4346_4852_BD29_951140B811AF__INCLUDED_)
