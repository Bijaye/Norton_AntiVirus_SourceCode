// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPSheet.cpp : implementation file
//

#include "stdafx.h"
#include "HPPSheet.h"


// CHPPSheet

IMPLEMENT_DYNAMIC(CHPPSheet, CPropertySheet)

CHPPSheet::CHPPSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CBaseSheetUI(nIDCaption, pParentWnd, iSelectPage)
{
}

CHPPSheet::CHPPSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CBaseSheetUI(pszCaption, pParentWnd, iSelectPage)
{
}

CHPPSheet::~CHPPSheet()
{
}


BEGIN_MESSAGE_MAP(CHPPSheet, CBaseSheetUI)
END_MESSAGE_MAP()


// CHPPSheet message handlers

void CHPPSheet::Load()
{
	for ( int nPage = 0L; nPage < GetPageCount(); nPage++ )
	{
		CPropertyPage* pPage = GetPage(nPage);
		if ( pPage && pPage->IsKindOf(RUNTIME_CLASS(CBasePageUI)) )
		{
			CBasePageUI* pBasePage = reinterpret_cast<CBasePageUI*>(pPage);
			pBasePage->Load();
		}
	}
}

void CHPPSheet::Store()
{
	for ( int nPage = 0L; nPage < GetPageCount(); nPage++ )
	{
		CPropertyPage* pPage = GetPage(nPage);
		if ( pPage && pPage->IsKindOf(RUNTIME_CLASS(CBasePageUI)) )
		{
			CBasePageUI* pBasePage = reinterpret_cast<CBasePageUI*>(pPage);
			pBasePage->Store();
		}
	}
}

DWORD CHPPSheet::HelpId()
{
	return 0L;
}

