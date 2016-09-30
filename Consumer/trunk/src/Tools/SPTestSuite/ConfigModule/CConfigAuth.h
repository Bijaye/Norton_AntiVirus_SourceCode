////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CEventProvider.h"

class CConfigAuth
{
public:
    CConfigAuth(void);
    ~CConfigAuth(void);

    bool Initialize();
    bool AddUser(const wchar_t* szAccountName);
    bool AddMD5Hash(const wchar_t* szHashName, const void* pHash, unsigned long ulFileLength);
    bool AddSHA1Hash(const wchar_t* szHashName, const void* pHash, unsigned long ulFileLength);
    bool AddLocation(const wchar_t* szLocation, bool bFileOnly);
    bool AddInstallLocation(const wchar_t* szInstallLocation);
    bool AddDevice(unsigned long ulDeviceType);
    bool AddInstallGuid(const GUID& iid);

private:
    CEventProvider m_EventProvider;
};
