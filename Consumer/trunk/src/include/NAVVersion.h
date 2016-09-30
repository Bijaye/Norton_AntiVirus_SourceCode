////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVVersion.h: interface for the CNAVVersion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVVERSION_H__2FA17356_5352_4C51_9FFA_4139C2E48C7B__INCLUDED_)
#define AFX_NAVVERSION_H__2FA17356_5352_4C51_9FFA_4139C2E48C7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "tchar.h"

#include "NAVInfo.h"
#include "optnames.h"     // Names of options

#include "navtstring.h"

// Class for getting version information for the Norton AntiVirus product
//
class CNAVVersion  
{
public:
	
    // Major.Minor.Inline.BuildNumber
    //
	ULONG GetMajorVersion();
    ULONG GetMinorVersion();
    ULONG GetInlineVersion();
	ULONG GetBuildNumber();

	// Partial build letter
    //
    LPCTSTR GetPartial();
    
    // Build type - "Retail", "TOnline", etc
    //
	LPCTSTR GetBuildType();

    // Version as string e.g. "8.1.2.3.c"
    //
    LPCTSTR GetPublicRevision();

    // Language two letter code - EN
    //
    LPCTSTR GetLanguageCode();

    // Always "NAV" - for CC Error Display
    //
	LPCTSTR GetProductAbbr();
	CNAVVersion();
	virtual ~CNAVVersion();

protected:
    tstring m_strProductAbbr;
    tstring m_strBuildType;
    tstring m_strPartial;
    tstring m_strPublicRevision;
    tstring m_strLanguageCode;

    ULONG m_ulMajorVersion;
    ULONG m_ulMinorVersion;
    ULONG m_ulInlineVersion;
    ULONG m_ulBuildNumber;

};

#endif // !defined(AFX_NAVVERSION_H__2FA17356_5352_4C51_9FFA_4139C2E48C7B__INCLUDED_)
