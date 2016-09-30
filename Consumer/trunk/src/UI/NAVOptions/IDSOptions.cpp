////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IDSOptions.h"
#include "IDSUILoader.h"
#include "ProductType.h"
#include "ISNames.h"
#include "resource.h"
#include "../navoptionsres/resource.h"
#include "SymHelpLauncher.h"
#include "SymHelp.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include "FWConstants.h"

CIDSOptions::CIDSOptions()
	: m_bInitialized(false)
	, m_hWndParent(NULL)
	, m_strTitle (_T(""))
	, m_hIcon (NULL)
	, m_nWidth (550)
	, m_nHeight (550)
	, m_pIDSDoc(NULL)
{
}

CIDSOptions::~CIDSOptions()
{
}

HRESULT CIDSOptions::Initialize(HWND hWndParent, LPCWSTR szTitle, HICON hIcon)
{
	m_hWndParent = hWndParent;
	m_strTitle = szTitle;
	m_hIcon = hIcon;

	if ((m_spIDS.m_p == NULL) && SYM_FAILED(m_loader_IDSUI.CreateObject(m_spIDS)))
	{
		CCTRACEE(_T("CIDSOptions:: failed to create IDS UI object"));
		return E_FAIL;
	}

	if (FAILED(m_spIDS->Initialize(false)))
	{
		CCTRACEE(_T("CIDSOptions:: failed to init IDS UI object"));
		return E_FAIL;
	}

	m_bInitialized = true;
	m_pIDSDoc = m_spIDS;

	return S_OK;
}

HRESULT CIDSOptions::OnDocumentComplete() throw()
{
    __super::OnDocumentComplete(); // ALWAYS MAKE SURE YOU CALL THE BASE MEDTHOD!

	//Set the title string
	SetWindowText(m_strTitle);
	//Set the windows position
	SetWindowPos(0, 0, 0, m_nWidth, m_nHeight, SWP_NOZORDER|SWP_NOMOVE);

	//Set the icon
	if (m_hIcon)
		CSymHTMLDialogDocumentImpl::SetIcon(m_hIcon, TRUE);

	// render the PC control
	if (m_spIFrameWnd.m_p && m_pIDSDoc)
		m_spIFrameWnd->Render(m_pIDSDoc);

	CenterWindow();

	return S_OK;
}

HRESULT CIDSOptions::OnNewIFrame( symhtml::ISymHTMLElement* lpElement, symhtml::ISymHTMLWindow* lpNewFrame )
{
	m_spIFrameWnd = lpNewFrame;
	return S_OK;
}

HRESULT CIDSOptions::OnSave(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
	HRESULT hr = S_FALSE;

	if (m_bInitialized)
	{
		if (m_spIDS.m_p)
		{
			hr = m_spIDS->Save();
		}
	}

	EndDialog(IDOK);
	return hr;
}

HRESULT CIDSOptions::OnCancel(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
	if (m_bInitialized)
	{
		if (m_spIDS.m_p)
		{
			m_spIDS->Cancel();
		}
	}

	EndDialog(IDCANCEL);
	return S_OK;
}

HRESULT CIDSOptions::OnDefault(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
	if (m_bInitialized)
	{
		if (m_spIDS.m_p)
		{
			m_spIDS->Default();
		}
	}

	return S_OK;
}

HRESULT CIDSOptions::ShowModalDialog() throw()
{
	//The dialog object
	symhtml::ISymHTMLDialogPtr spDialog;

	//Create the dialog object
	SYMRESULT sym_result = symhtml::loader::ISymHTMLDialog::CreateObject(GETMODULEMGR(), &spDialog );
	if( SYM_FAILED(sym_result) )
	{
		return E_FAIL;
	}

	//Display the main UI and render the page specified by the page ID
	int nResult = 0;
	HRESULT hr = spDialog->DoModal(m_hWndParent, this, &nResult);

	return hr;
}

HRESULT CIDSOptions::OnHelp(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
	isshared::CHelpLauncher helpLauncher;
	DWORD dwHelpID = IWP_AUTOBLOCK;
	helpLauncher.LaunchHelp(dwHelpID, ::GetDesktopWindow());
	return S_OK;
}
