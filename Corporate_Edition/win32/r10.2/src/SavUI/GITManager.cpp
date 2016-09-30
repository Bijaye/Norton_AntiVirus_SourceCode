// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

#include "StdAfx.h"
#include "gitmanager.h"

CGITManager CGITManager::s_objGITManager;

CGITManager::CGITManager(void)
{
}

CGITManager::~CGITManager(void)
{
}


/**
 * static CGITManager& GetInstance()const;
 *
 * Gets the singleton object
 *
**/
CGITManager& CGITManager::GetInstance()
{
    return s_objGITManager;
}


/**
 * DWORD RegisterInterface(IUknown* pUnk, const REFIID riid, DWORD& dwCookie);
 *
 * Registers an interface with the global interface table to allow for the 
 * use of the object in other apartments. The cookie returned is used to
 * retrieve the GIT later
 *
**/
DWORD CGITManager::RegisterInterface(IUnknown* pUnk, REFIID riid, DWORD& dwCookie)
{
    HRESULT hr = S_OK;
    if( m_pGITptr == NULL )
    {
        //if the GIT hasn't been initialized do so
        hr = CoCreateInstance(CLSID_StdGlobalInterfaceTable,
                                NULL,
                                CLSCTX_INPROC_SERVER,
                                IID_IGlobalInterfaceTable,
                                (void **)&m_pGITptr);
    }

    if( S_OK == hr && m_pGITptr != NULL )
    {
        hr = m_pGITptr->RegisterInterfaceInGlobal(pUnk, riid, &dwCookie);
    }

    return hr;
}

/**
 * DWORD RevokeInterface(const DWORD dwCookie);
 *
 * RevokeInterface removes an interface from the GIT
 *
**/
DWORD CGITManager::RevokeInterface(const DWORD dwCookie)
{
    if( m_pGITptr == NULL )
        return (DWORD)E_NOINTERFACE;

    return m_pGITptr->RevokeInterfaceFromGlobal(dwCookie);
}

/**
 * DWORD GetInterface(const DWORD dwCookie, const REFIID riid,void** pInterface);
 *
 * Get an interface stored in the GIT. 
 *
**/
DWORD CGITManager::GetInterface(const DWORD dwCookie, REFIID riid,void** pInterface)
{
    if( m_pGITptr == NULL )
        return (DWORD)E_NOINTERFACE;

    return m_pGITptr->GetInterfaceFromGlobal(dwCookie, riid, pInterface);
}
