// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// \SourceDepot\Norton_AntiVirus\Corporate_Edition\Win32\src\AVCore\scandlvr\src\Review1Pg.cpp : implementation file
//

#include "stdafx.h"
#include "Global.h"
#include "ScanDeliverDLL.h"
#include "scandlvr.h"
#include "ScanWiz.h"
#include "Review1Pg.h"

extern BOOL g_bSubmitAdditionalInfo;

// CReview1PropertyPage dialog

IMPLEMENT_DYNAMIC(CReview1PropertyPage, CWizardPropertyPage)
CReview1PropertyPage::CReview1PropertyPage(CScanDeliverDLL*  pDLL)
	: CWizardPropertyPage(CReview1PropertyPage::IDD, IDS_CAPTION_REVIEW)
{
	m_bSubmitAddInfo = TRUE;
}

CReview1PropertyPage::~CReview1PropertyPage()
{
}

void CReview1PropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPropertyPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_SUB_ADD_INFO, m_bSubmitAddInfo);	
}


BEGIN_MESSAGE_MAP(CReview1PropertyPage, CDialog)
END_MESSAGE_MAP()


// CReview1PropertyPage message handlers

// ==== OnInitDialog ======================================================
BOOL CReview1PropertyPage::OnInitDialog(void)
{
    CWizardPropertyPage::OnInitDialog();

    // TODO: Add extra initialization here

    // initialize this dialog's member variables
	auto CEdit *pEdCtrl = NULL;

	pEdCtrl = (CEdit*)GetDlgItem(IDC_REVIEW1_EDIT_DISC);
	if(pEdCtrl)
	{
		auto    CString         tempStr;
		if( tempStr.LoadString(IDS_REVIEW1_DISC) != 0)
		{
			pEdCtrl->SetWindowText(tempStr);
		}
	}

    // update field contents with the member variables
    this->UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE

}  // end of "CReview1PropertyPage::OnInitDialog"

// ==== OnWizardNext ======================================================
//
//
LRESULT CReview1PropertyPage::OnWizardNext(void)
{
    // TODO: Add your specialized code here and/or call the base class

    this->UpdateData(TRUE);
    g_bSubmitAdditionalInfo = m_bSubmitAddInfo;
    return CWizardPropertyPage::OnWizardNext();

}  // end of "CReview1PropertyPage::OnWizardNext"



// ==== OnWizardBack ======================================================
//
//
LRESULT CReview1PropertyPage::OnWizardBack(void)
{
    // TODO: Add your specialized code here and/or call the base class

    this->UpdateData(TRUE);
	g_bSubmitAdditionalInfo = m_bSubmitAddInfo;

    return CWizardPropertyPage::OnWizardBack();

}  // end of "CReview1PropertyPage::OnWizardBack"

