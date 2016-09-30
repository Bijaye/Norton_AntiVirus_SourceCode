// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPSheet.h : implementation file
//
#pragma once
#include "BaseSheetUI.h"
#include "afxwin.h"
#include "afxcmn.h"

// CHPPSheet

class CHPPSheet : public CBaseSheetUI
{
	DECLARE_DYNAMIC(CHPPSheet)

public:
	CHPPSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CHPPSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CHPPSheet();

	virtual void Load();
	virtual void Store();
	virtual DWORD HelpId();

protected:
	DECLARE_MESSAGE_MAP()
};


