////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// CSymProtectControlHelperNAV.h : Hijacked NAV version of CSymProtectControlHelper
//
//  Used for additional NAV specific work.
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "SymProtectControlHelper.h"

class CSymProtectControlHelperNAV
	: public CSymProtectControlHelper
{
public:
    CSymProtectControlHelperNAV();
    virtual ~CSymProtectControlHelperNAV();

    // Add a notification filter to SymProtect
    //
    bool AddNotificationFilter(const wchar_t* pszPath);

    // Don't allow shallow copies
private:
    CSymProtectControlHelperNAV(const CSymProtectControlHelperNAV&);
    CSymProtectControlHelperNAV& operator=(const CSymProtectControlHelperNAV&);
};
