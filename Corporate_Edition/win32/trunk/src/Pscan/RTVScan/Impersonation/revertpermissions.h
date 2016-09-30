// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005, Symantec Corporation, All rights reserved.


#pragma once

#ifndef __cplusplus
#error revertpermissions.h requires C++ to work. We recommend you covert your file to C++.
#endif

#if !defined WIN32 
#error revertpermissions.h is a win32 specific header
#endif

#include "impersonate.h"


//stack based revert logged on user and then re impersonate logged on user
// once class pops off the stack
class CRevertPermissions
{
public:
    CRevertPermissions():m_pImpersonate(NULL)
    {
    }

    void RevertImpersonator(CImpersonate* pImpersonate)
    {
        SAVASSERT( NULL != pImpersonate );
        if( NULL == pImpersonate )
            return;

        m_pImpersonate = pImpersonate;
        m_pImpersonate->RevertToSelf();
    }

    ~CRevertPermissions()
    {
        if( m_pImpersonate != NULL )
            m_pImpersonate->ImpersonateLoggedOnUser();
    }

private:
    CImpersonate* m_pImpersonate;
};