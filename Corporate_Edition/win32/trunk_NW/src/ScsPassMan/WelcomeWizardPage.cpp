// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// CWelcomeWizardPage.cpp : implementation file
//

#include "stdafx.h"
#include "ScsPassMan.h"
#include "WelcomeWizardPage.h"
#include ".\welcomewizardpage.h"


// CWelcomeWizardPage dialog

//REMOVED_FOR_VC6//IMPLEMENT_DYNAMIC(CWelcomeWizardPage, CNewWizPage)
CWelcomeWizardPage::CWelcomeWizardPage(CWnd* pParent /*=NULL*/)
	: CNewWizPage(CWelcomeWizardPage::IDD, pParent)
{
}

CWelcomeWizardPage::~CWelcomeWizardPage()
{
}


CStatic& CWelcomeWizardPage::getWelcomeText()
{
	return m_welcomeText;
}

void CWelcomeWizardPage::DoDataExchange(CDataExchange* pDX)
{
	CNewWizPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WELCOME_TEXT, m_welcomeText);
}


BEGIN_MESSAGE_MAP(CWelcomeWizardPage, CNewWizPage)
//	ON_WM_CREATE()
ON_WM_ENABLE()
END_MESSAGE_MAP()


// CWelcomeWizardPage message handlers

//int CWelcomeWizardPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CNewWizPage::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//	// TODO:  Add your specialized creation code here
//	m_welcomeText.SetFont( &m_LargeFont );
//
//	return 0;
//}

void CWelcomeWizardPage::OnEnable(BOOL bEnable)
{
	CNewWizPage::OnEnable(bEnable);

	// TODO: Add your message handler code here
	m_welcomeText.SetFont( &m_LargeFont );
}