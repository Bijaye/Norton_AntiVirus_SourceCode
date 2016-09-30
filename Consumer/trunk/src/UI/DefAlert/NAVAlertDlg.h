////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVAlertDlg.h

#pragma once

#include "SymAlertInterface.h"
#include "CLTProductHelper.h"

class CAlertDlg:
     public SIMON::CSimonClass<CAlertDlg,&CLSID_CAlertDlg>  
    ,public SIMON::IInterfaceManagementImpl<false>
    ,public IAlertDlg
{
public:
	CAlertDlg(void) {}
	~CAlertDlg(void) {}

    SIMON_INTERFACE_MAP_BEGIN()
   	SIMON_INTERFACE_ENTRY(IID_IAlertDlg, IAlertDlg)
    SIMON_INTERFACE_MAP_END()

    SIMON_STDMETHOD(Show)(SYMALERT_TYPE Type, HWND hwndParent, BOOL bWait);

private:
	HRESULT DoTask(SYMALERT_RESULT Result, HWND hWndParent, BOOL bWait);
	HRESULT LaunchActivationWizard(HWND hWndParent, long nParam);
	HRESULT LaunchSubscriptionWizard(HWND hWndParent);
	HRESULT EnableALU();
	HRESULT LaunchLiveUpdate(BOOL bWait);
	HRESULT LaunchUninstall();
	bool LookupHTML(SYMALERT_TYPE hType, int& iHTMLIndex);
	HRESULT GetAlertHTMLInfo(SYMALERT_TYPE Type, LPTSTR lpszHTML, long& nWidth, long& nHeight);

    CCLTProductHelper m_CltHelper;
};