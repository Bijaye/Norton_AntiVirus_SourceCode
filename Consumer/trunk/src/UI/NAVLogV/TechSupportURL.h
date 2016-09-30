////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

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
    tstring m_strLanguage;
    tstring m_strProduct;
    tstring m_strURL;
    tstring m_strVersion;
    tstring m_strURLPattern;
    tstring m_strBuildType;
};

#endif // !defined(AFX_TECHSUPPORTURL_H__47E228E6_4346_4852_BD29_951140B811AF__INCLUDED_)
