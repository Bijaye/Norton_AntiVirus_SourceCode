// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
#pragma once

#include "ccSettingsLoader.h"

const LPCTSTR KEY_PATH_FMT      = _T("CommonClient\\Passwords");
const LPCTSTR ENABLED_VAL		= _T("enabled");
const LPCTSTR PASSWORD_VAL		= _T("password");

/////////////////////////////////////////////////////////////////////////////
// Class for managing global objects
class CSettingsInterfaceMgr
{
public:
    CSettingsInterfaceMgr()
    {
        initialize();
    }

    virtual ~CSettingsInterfaceMgr(){}

    BOOL isValid();
	ccSettings::ISettings *getSettingsForUser(LPCTSTR szUserName);
    ccSettings::ISettings *getSettingsForPath(LPCTSTR szSettingsPath);
    HRESULT putSettingsForUser(LPCTSTR szUserName, const CSymPtr<ccSettings::ISettings> &pSet);
    HRESULT putSettingsNoUser(const CSymPtr<ccSettings::ISettings> &pSet);

private:
	cc::ccSet_ISettingsUserManager m_UserManagerLoader;
	ccSettings::ISettingsUserManagerPtr m_UserMgr;

    BOOL initialize();
};