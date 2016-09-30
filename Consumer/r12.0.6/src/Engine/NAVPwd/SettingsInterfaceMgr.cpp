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
#include "stdafx.h"
#include "SettingsInterfaceMgr.h"
#include <stdio.h>

using namespace ccSettings;

BOOL CSettingsInterfaceMgr::isValid()
{
    return (NULL != m_UserMgr.m_p);
}

ISettings *CSettingsInterfaceMgr::getSettingsForUser(LPCTSTR szUserName)
{
    ISettings *pSettings = NULL;

    if(!isValid())
        return NULL;

    m_UserMgr->GetSettings(szUserName, KEY_PATH_FMT, &pSettings);

    if(NULL == pSettings)
        m_UserMgr->CreateSettings(szUserName, KEY_PATH_FMT, &pSettings);
    
    return pSettings;
}

ISettings *CSettingsInterfaceMgr::getSettingsForPath(LPCTSTR szSettingsPath)
{
    ISettings *pSettings = NULL;

    if(!isValid())
        return NULL;

    if((NULL == szSettingsPath) || (_T('\0') == szSettingsPath[0]))
		return NULL;

    CSymPtr<ISettingsManager> settMgr;

    m_UserMgr->QueryInterface(IID_SettingsManager, (void**)&settMgr);
		
    settMgr->GetSettings(szSettingsPath, &pSettings);

    if(NULL == pSettings)
        settMgr->CreateSettings(szSettingsPath, &pSettings);
    
    return pSettings;
}

HRESULT CSettingsInterfaceMgr::putSettingsForUser(LPCTSTR szUserName, const CSymPtr<ISettings> &pSet)
{
    HRESULT hr = S_OK;

    if(!isValid())
        return E_FAIL;;

    // If no username provided, set global setting
    if((NULL == szUserName) || (_T('\0') == szUserName[0]))
    {
		putSettingsNoUser(pSet);
	}
    // Else set user setting
    else
    {
        SYMRESULT sr = m_UserMgr->PutSettings(pSet);
        hr = ((SYM_OK == sr) ? S_OK : E_FAIL);
    }

    return hr;
}

HRESULT CSettingsInterfaceMgr::putSettingsNoUser(const CSymPtr<ISettings> &pSet)
{
    HRESULT hr = S_OK;

    if(!isValid())
        return E_FAIL;;

    CSymPtr<ISettingsManager> settMgr;

    m_UserMgr->QueryInterface(IID_SettingsManager, (void**)&settMgr);
    SYMRESULT sr = settMgr->PutSettings(pSet);

    hr = ((SYM_OK == sr) ? S_OK : E_FAIL);

    return hr;
}

BOOL CSettingsInterfaceMgr::initialize()
{
    if(NULL == m_UserMgr.m_p)
    {            
        m_UserManagerLoader.CreateObject(&m_UserMgr);
    }

    if(NULL == m_UserMgr.m_p)
        return FALSE;

    return TRUE;
}