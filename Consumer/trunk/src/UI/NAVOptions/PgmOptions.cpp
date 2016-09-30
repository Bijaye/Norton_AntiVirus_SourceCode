////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PgmOptions.h"
#include "ProgCtrlUILoader.h"
#include "ProductType.h"
#include "ISNames.h"
#include "resource.h"
#include "../navoptionsres/resource.h"
#include "SymHelpLauncher.h"
#include "SymHelp.h"
#include "ccSymModuleLifetimeMgrHelper.h"

static fwui::PgmCtlLoader s_pgm_loader;

CPgmOptions::CPgmOptions()
	: m_bInitialized(false)
	, m_hWndParent(NULL)
	, m_strTitle (_T(""))
	, m_hIcon (NULL)
	, m_nWidth (550)
	, m_nHeight (500)
	, m_pDoc(NULL)
{
}

CPgmOptions::~CPgmOptions()
{
}

HRESULT CPgmOptions::Initialize(HWND hWndParent, LPCWSTR szTitle, HICON hIcon, CSymPtr<CSNDHelper> & sndHelper)
{
	m_hWndParent = hWndParent;
	m_strTitle = szTitle;
	m_hIcon = hIcon;

	if (sndHelper.m_p == NULL)
	{
		CCTRACEE(_T("CPgmOptions:: failed to create CSNDHelper object"));
		return E_FAIL;
	}

	m_sndHelper = sndHelper;

	return S_OK;
}

HRESULT CPgmOptions::OnDocumentComplete() throw()
{
    __super::OnDocumentComplete(); // ALWAYS MAKE SURE YOU CALL THE BASE MEDTHOD!

	//Set the title string
	SetWindowText(m_strTitle);
	//Set the windows position
	SetWindowPos(0, 0, 0, m_nWidth, m_nHeight, SWP_NOZORDER|SWP_NOMOVE);

	//Set the icon
	if (m_hIcon)
		CSymHTMLDialogDocumentImpl::SetIcon(m_hIcon, TRUE);

	if ((m_spPgmUI.m_p == NULL) && SYM_FAILED(s_pgm_loader.CreateObject(m_spPgmUI)))
	{
		CCTRACEE(_T("CPgmOptions:: failed to create Pgm UI object"));
		return E_FAIL;
	}

	if (FAILED(LoadAppRules()))
	{
		CCTRACEE(_T("CPgmOptions:: failed to load application list"));
		return E_FAIL;
	}

	if (FAILED(m_spPgmUI->Initialize(m_sndHelper, NULL, m_spCopySNDApps, m_strTitle, m_hIcon)))
	{
		CCTRACEE(_T("CPgmOptions:: failed to init Pgm UI object"));
		return E_FAIL;
	}

	m_spPgmUI->SetProduct(FWUI_PRODUCT_NAV);

	m_bInitialized = true;
	m_pDoc = m_spPgmUI;

	// render the PC control
	if (m_spIFrameWnd.m_p && m_pDoc)
		m_spIFrameWnd->Render(m_pDoc);

	CenterWindow();

	return S_OK;
}

HRESULT CPgmOptions::OnNewIFrame( symhtml::ISymHTMLElement* lpElement, symhtml::ISymHTMLWindow* lpNewFrame )
{
	m_spIFrameWnd = lpNewFrame;
	return S_OK;
}

HRESULT CPgmOptions::OnSave(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
	if (m_bInitialized)
	{
		bool bDirty = false;
		if (m_spPgmUI.m_p && SUCCEEDED(m_spPgmUI->GetIsDirty(bDirty)) && bDirty)
		{
			// copy the temp list to the real list
			if (SN_SUCCEEDED(m_spSNDApps->CopyFrom(m_spCopySNDApps)))
			{
				m_sndHelper->CommitSharedRuleDB();
			}
		}
	}

	EndDialog(IDOK);
	return S_OK;
}

HRESULT CPgmOptions::OnCancel(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
	EndDialog(IDCANCEL);
	return S_OK;
}

HRESULT CPgmOptions::ShowModalDialog() throw()
{
	//The dialog object
	symhtml::ISymHTMLDialogPtr spDialog;

	//Create the dialog object
	SYMRESULT sym_result = symhtml::loader::ISymHTMLDialog::CreateObject(GETMODULEMGR(), &spDialog);
	if( SYM_FAILED(sym_result) )
	{
		return E_FAIL;
	}

	//Display the main UI and render the page specified by the page ID
	int nResult = 0;
	HRESULT hr = spDialog->DoModal(m_hWndParent, this, &nResult);

	return hr;
}

HRESULT CPgmOptions::OnHelp(symhtml::ISymHTMLElement* lpElement, ISymBase* pExtra, bool& bHandled)
{
	isshared::CHelpLauncher helpLauncher;
	DWORD dwHelpID = NAV_IWP_create_app_rules;
	helpLauncher.LaunchHelp(dwHelpID, ::GetDesktopWindow());
	return S_OK;
}

HRESULT CPgmOptions::LoadAppRules()
{
	HRESULT hr = E_FAIL;

	m_spSNDApps.Release();
	m_spCopySNDApps.Release();

	if (m_sndHelper.m_p)
	{
		// get the root db...

		SymNeti::IFWRecordPtr pRootDB;
		pRootDB = m_sndHelper->GetSharedRootDB();
		if (pRootDB == NULL)
		{
			CCTRACEE(_T("LoadAppRules: unable to get the Root DB"));
			return E_FAIL;
		}

		// get the app rules
		if (SN_SUCCEEDED(pRootDB->GetItem(SymNeti::xApplications, &m_spSNDApps)) && m_spSNDApps.m_p)
		{
			// and copy them to our temp list
			if (SN_SUCCEEDED(m_spSNDApps->Create(&m_spCopySNDApps)) && m_spCopySNDApps.m_p &&
				SN_SUCCEEDED(m_spCopySNDApps->CopyFrom(m_spSNDApps)))
			{
				hr = S_OK;
			}
		}
	}

	return hr;
}
