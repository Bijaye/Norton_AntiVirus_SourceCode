////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* NAV Instant Messaging Options Object                                 */
/************************************************************************/

#include "stdafx.h"
#include "IMOptions.h"
#include "IMConfigLoader.h"
#include "ccSymModuleLifetimeMgrHelper.h"

CIMOptions::CIMOptions(): m_bInitialized(false), m_bDirty(false), m_spConfig(NULL) {}

CIMOptions::~CIMOptions()
{
    m_ImMap.clear();
}

HRESULT CIMOptions::Initialize()
{
    SYMRESULT sr;

    if(m_bInitialized)
        return S_FALSE;

    if(!m_spConfig)
    {
        ISShared::ISShared_IMLoader imLoader;
        if(SYM_FAILED(sr = imLoader.CreateObject(GETMODULEMGR(), m_spConfig)))
        {
            CCTRCTXE1(_T("Error creating IMConfigInterface object. Symresult = 0x%08x"), sr);
            return E_FAIL;
        }
    }

    // Get Current State of all IM clients

    m_bInitialized = true;
    return S_OK;
}

HRESULT CIMOptions::Save()
{
    ISShared::IIMConfig::IMSTATUS imStatus;

    if(!m_bInitialized || !m_bDirty)
        return S_FALSE;

    if(!m_spConfig)
        return E_POINTER;

    for(ISShared::IIMConfig::IMTYPE i = ISShared::IIMConfig::IMTYPE_FIRST;
        i < ISShared::IIMConfig::IMTYPE_LAST; i=(ISShared::IIMConfig::IMTYPE)(i+1))
    {
        if(m_ImMap[i])
        {
            if((imStatus = m_spConfig->ConfigureIM(i)) == ISShared::IIMConfig::IMSTATUS_ERROR)
                return E_FAIL;
        }
        else
        {
            if((imStatus = m_spConfig->UnconfigureIM(i)) == ISShared::IIMConfig::IMSTATUS_ERROR)
                return E_FAIL;
        }
    }

    m_bDirty = false;
    return S_OK;
}

HRESULT CIMOptions::Notify() {return S_FALSE;}

HRESULT CIMOptions::Default()
{
    HRESULT hr;
    ISShared::IIMConfig::IMSTATUS imStatus;

    if(FAILED(hr = Initialize()))
        return hr;

    if((imStatus = m_spConfig->SetDefaults()) == ISShared::IIMConfig::IMSTATUS_ERROR)
        return E_FAIL;

    m_bDirty = false;
    return S_OK;
}

bool CIMOptions::IsDirty(){return m_bDirty;}

HRESULT CIMOptions::GetState(const ISShared::IIMConfig::IMTYPE client, DWORD& dwState)
{
    HRESULT hr;

    if(FAILED(hr = Initialize()))
        return hr;

    m_ImMap[client] = m_spConfig->GetConfiguredIM(client);
    dwState = m_ImMap[client] ? VARIANT_TRUE:VARIANT_FALSE;
    return S_OK;
}

HRESULT CIMOptions::SetState(const ISShared::IIMConfig::IMTYPE client, const DWORD& dwState)
{
    HRESULT hr;

    if(FAILED(hr = Initialize()))
        return hr;

	if(m_ImMap[client] != (dwState?true:false))
    {
        m_bDirty = true;
        m_ImMap[client] = (dwState?true:false);
    }

    return S_OK;
}

HRESULT CIMOptions::IsInstalled(const ISShared::IIMConfig::IMTYPE client, bool& bInstalled)
{
    HRESULT hr;
    if(FAILED(hr = Initialize()))
        return hr;

    bInstalled = m_spConfig->IsIMClientInstalled(client);
    return S_OK;
}

HRESULT CIMOptions::Configure(const ISShared::IIMConfig::IMTYPE client)
{
    HRESULT hr;
    if(FAILED(hr = Initialize()))
        return hr;

    if(m_spConfig->ConfigureIM(client) == ISShared::IIMConfig::IMSTATUS_ERROR)
        return E_FAIL;
    else
        return S_OK;
}