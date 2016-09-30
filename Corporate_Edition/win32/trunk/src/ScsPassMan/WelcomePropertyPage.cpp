// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// WelcomePropertyPage.cpp : implementation file
//

#include "stdafx.h"
#include "ScsPassMan.h"
#include "WelcomePropertyPage.h"


// CWelcomePropertyPage dialog

IMPLEMENT_DYNAMIC(CWelcomePropertyPage, CPropertyPage)
CWelcomePropertyPage::CWelcomePropertyPage()
	: CPropertyPage(CWelcomePropertyPage::IDD)
{
}

CWelcomePropertyPage::~CWelcomePropertyPage()
{
}

void CWelcomePropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CWelcomePropertyPage, CPropertyPage)
END_MESSAGE_MAP()


// CWelcomePropertyPage message handlers
