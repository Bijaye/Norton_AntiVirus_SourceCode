// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.

// VirusFoundCOMCallback.cpp : Implementation of CVirusFoundCOMCallback

#include "stdafx.h"
#include "VirusFoundCOMCallback.h"
#include "uimanager.h"

// CVirusFoundCOMCallback
STDMETHODIMP CVirusFoundCOMCallback::DlgClosed(ULONG ulSessionId)
{

    dprintf("Closing VirusFound Dlg for session(%d)\n", ulSessionId);
    if( false == m_bTrusted )
        return E_NOTIMPL;

    CUIManager& objUIManager = CUIManager::GetInstance();
    objUIManager.RemoveNotificationDlg(ulSessionId);

    return S_OK;
}
