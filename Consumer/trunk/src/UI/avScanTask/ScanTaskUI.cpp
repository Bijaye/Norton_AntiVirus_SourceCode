////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ScanTaskUI.cpp : implementation file
//
#include "StdAfx.h"
#include "ScanTaskUI.h"
#include "ScanTaskDialogDocument.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include "isSymTheme.h"
#include "SymHTMLAsyncTask.h" 
#include "uiElementInterface.h"

//-------------------------------------------------------------------------
CScanTaskUI::CScanTaskUI(void)
{
}

//-------------------------------------------------------------------------
CScanTaskUI::~CScanTaskUI(void)
{
}

//-------------------------------------------------------------------------
HRESULT CScanTaskUI::DisplayUI(HWND hWndParent, cc::IKeyValueCollection* pKeyValueCol)
{
	CCTRCTXI0(L"Enter");
	ccLib::CExceptionInfo exceptionInfo;

	STAHLSOFT_HRX_TRY(hr)
	{
		// Create SymTheme Skin window.
		// Do not bail out if failed.
		// Should continue to display our Scan window
		HRESULT hr1 = S_OK;
		CISSymTheme isSymTheme;
		hr1 = isSymTheme.Initialize(GetModuleHandle(NULL));
		if(FAILED(hr1))
		{
			CCTRCTXE1(L"CISSymTheme::Initialize() failed. Error: 0x%08X", hr1);
		}

		// Create a SYMHTML dialog object
		symhtml::ISymHTMLDialogPtr spDialog;
		SYMRESULT sr = symhtml::loader::ISymHTMLDialog::CreateObject(GETMODULEMGR(), spDialog);
		if(SYM_FAILED(sr))
		{
			CCTRCTXE1(L"Unable to create SymHTMLDialog object. SR: 0x%08X", sr);
			hrx << E_FAIL;
		}

		// Create Scan Task UI document
		symhtml::ISymHTMLDocumentPtr spDialogDocument = new CScanTaskDialogDocument();

		if(spDialogDocument)
		{
			// Show the Scan Task UI document
			int nResult = 0;
			hr = spDialog->DoModal(hWndParent, spDialogDocument, &nResult);
			if(FAILED(hr))
			{
				CCTRCTXE1(L"Failed to show custom scan UI. HR: 0x%08X", hr);
			}
		}
	}	
	CCCATCHMEM(exceptionInfo)
	CCCATCHCOM(exceptionInfo)

	if(exceptionInfo.IsException())
	{
		hr = E_FAIL;
	}

	return hr;
}

