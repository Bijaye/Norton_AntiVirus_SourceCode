// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// LDVPPropSheet.cpp : implementation file
//

#include "stdafx.h"
#include "LDVPPropSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Required declarations for DoModal overridden function
extern BOOL AFXAPI AfxUnhookWindowCreate();
extern void AFXAPI AfxHookWindowCreate(CWnd* pWnd);
extern BOOL AFXAPI AfxEndDeferRegisterClass(long fClass);
#define AfxDeferRegisterClass(fClass) \
	((afxRegisteredClasses & fClass) ? TRUE : AfxEndDeferRegisterClass(fClass))
#define AFX_WNDCOMMCTLS_REG     (0x0010)
// End of DoModal declarations


/////////////////////////////////////////////////////////////////////////////
// CLDVPPropSheet

IMPLEMENT_DYNAMIC(CLDVPPropSheet, CPropertySheet)

CLDVPPropSheet::CLDVPPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CLDVPPropSheet::CLDVPPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CLDVPPropSheet::~CLDVPPropSheet()
{
}


BEGIN_MESSAGE_MAP(CLDVPPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CLDVPPropSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLDVPPropSheet message handlers


// Override DoModal to fix bug with the parent window handle
// being changed in the pParentWnd class while I'm in the
// RunModalLoop(). Calling AfxLockTempMaps / AfxUnlockTempMaps
// keeps this from happening.
int CLDVPPropSheet::DoModal() 
{
/*
//*************************************************************
	AfxLockTempMaps();
//*************************************************************
	int nResult = CPropertySheet::DoModal();

//*/
//*
	ASSERT_VALID(this);
	ASSERT(m_hWnd == NULL);

	// register common controls
	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTLS_REG));

	// finish building PROPSHEETHEADER structure
	BuildPropPageArray();

	// allow OLE servers to disable themselves
	AfxGetApp()->EnableModeless(FALSE);

	// find parent HWND
	HWND hWndTop;
	CWnd* pParentWnd = CWnd::GetSafeOwner(m_pParentWnd, &hWndTop);
	m_psh.hwndParent = pParentWnd->GetSafeHwnd();

//*************************************************************
	AfxLockTempMaps();
//*************************************************************

	BOOL bEnableParent = FALSE;
	if (pParentWnd != NULL && pParentWnd->IsWindowEnabled())
	{
		pParentWnd->EnableWindow(FALSE);
		bEnableParent = TRUE;
	}
	HWND hWndCapture = ::GetCapture();
	if (hWndCapture != NULL)
		::SendMessage(hWndCapture, WM_CANCELMODE, 0, 0);

	// setup for modal loop and creation
	m_nModalResult = 0;
	m_nFlags |= WF_CONTINUEMODAL;

	// hook for creation of window
	AfxHookWindowCreate(this);
	m_psh.dwFlags |= PSH_MODELESS;
	m_nFlags |= WF_CONTINUEMODAL;
	HWND hWnd = (HWND)::PropertySheet((LPCPROPSHEETHEADER)&m_psh);
	m_psh.dwFlags &= ~PSH_MODELESS;
	AfxUnhookWindowCreate();

	// handle error
	if (hWnd == NULL || hWnd == (HWND)-1)
	{
		m_nFlags &= ~WF_CONTINUEMODAL;
		m_nModalResult = -1;
	}
	int nResult = m_nModalResult;
	if (m_nFlags & WF_CONTINUEMODAL)
	{
		// enter modal loop
		DWORD dwFlags = MLF_SHOWONIDLE;
		if (GetStyle() & DS_NOIDLEMSG)
			dwFlags |= MLF_NOIDLEMSG;
		nResult = RunModalLoop(dwFlags);
	}

	// hide the window before enabling parent window, etc.
	if (m_hWnd != NULL)
	{
		SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
	}
	if (bEnableParent)
		pParentWnd->EnableWindow(TRUE);
	if (pParentWnd != NULL && ::GetActiveWindow() == m_hWnd)
		pParentWnd->SetActiveWindow();

	// cleanup
	DestroyWindow();

	// allow OLE servers to enable themselves
	AfxGetApp()->EnableModeless(TRUE);
	if (hWndTop != NULL)
		::EnableWindow(hWndTop, TRUE);
//*/
//*************************************************************
	AfxUnlockTempMaps();
//*************************************************************

	return nResult;
}
