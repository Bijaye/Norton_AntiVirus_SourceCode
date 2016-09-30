////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "SessionAppWindow.h"
#include "SessionAppWindowThread.h"
#include "SessionAppServerImpl.h"
#include "SessionAppComm.h"

using namespace SessionApp;

CAppComm::CAppComm(void)
{
}

CAppComm::~CAppComm(void)
{
}

HRESULT CAppComm::Command( const SYMGUID& gCommandGuid, LPCWSTR szCallbackChannel,
                 ISymBase* pBase, ISymBase** ppOutBase )
{
    if(!m_pSessionAppServer)
        return S_OK;

    if(SymIsEqualIID(gCommandGuid, SessionApp::CMDID_NotifyMessage))
    {
        m_pSessionAppServer->OnNotifyMessage(gCommandGuid, szCallbackChannel, pBase, ppOutBase);
    }
    else if(SymIsEqualIID(gCommandGuid, SessionApp::CMDID_AlertMessage))
    {
        m_pSessionAppServer->OnAlertMessage(gCommandGuid, szCallbackChannel, pBase, ppOutBase);
    }
    else if(SymIsEqualIID(gCommandGuid, SessionApp::CMDID_SetIconState))
    {
        m_pSessionAppServer->OnSetIconState(gCommandGuid, szCallbackChannel, pBase, ppOutBase);
    }
    else if(SymIsEqualIID(gCommandGuid, SessionApp::CMDID_GetIconState))
    {
        m_pSessionAppServer->OnGetIconState(gCommandGuid, szCallbackChannel, pBase, ppOutBase);
    }
    else if(SymIsEqualIID(gCommandGuid, SessionApp::CMDID_CustomCommand))
    {
        m_pSessionAppServer->OnCommand(gCommandGuid, szCallbackChannel, pBase, ppOutBase);
    }
    else
    {
        CCTRCTXW0(_T("Unknown CommandId received."));
        m_pSessionAppServer->OnCommand(gCommandGuid, szCallbackChannel, pBase, ppOutBase);
    }

    return S_OK;
}
