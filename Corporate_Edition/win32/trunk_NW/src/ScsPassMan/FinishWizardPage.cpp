// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// FinishWizardPage.cpp : implementation file
//

#include "stdafx.h"
#include "ScsPassMan.h"
#include "FinishWizardPage.h"


// CFinishWizardPage dialog

//REMOVED_FOR_VC6//IMPLEMENT_DYNAMIC(CFinishWizardPage, CNewWizPage)
CFinishWizardPage::CFinishWizardPage(CWnd* pParent /*=NULL*/)
	: CNewWizPage(CFinishWizardPage::IDD, pParent)
{
}

CFinishWizardPage::~CFinishWizardPage()
{
}

void CFinishWizardPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFinishWizardPage, CNewWizPage)
END_MESSAGE_MAP()


// CFinishWizardPage message handlers

/*
 * When this wizard page is activated, hide the "Cancel" button and show the "Finish" button
 */
void CFinishWizardPage::OnSetActive()
{
	GetParent()->GetDescendantWindow( ID_WIZFINISH )->ShowWindow( SW_SHOWNA );
	GetParent()->GetDescendantWindow( IDCANCEL )->ShowWindow( SW_HIDE );
}

/*
 * When "Back" is pressed, hide the "Finish" button and show the "Cancel" button
 */
LRESULT CFinishWizardPage::OnWizardBack()
{
	GetParent()->GetDescendantWindow( IDCANCEL )->ShowWindow( SW_SHOWNA );
	GetParent()->GetDescendantWindow( ID_WIZFINISH )->ShowWindow( SW_HIDE );

	return 0;
}