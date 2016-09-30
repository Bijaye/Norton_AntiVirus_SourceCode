////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <activeds.h>

class CConfigUsers
{
public:
    virtual ~CConfigUsers();

    HRESULT Initialize(bool bDeleteUsersOnExit);
    HRESULT CreateUser(PCWSTR szUserName, PCWSTR szPassword);

private:
    bool                        m_bDeleteUsersOnExit;
    ATL::CComPtr<IADsContainer> m_pCont;
    std::vector<CStringW>       m_vCreatedUsers;
};