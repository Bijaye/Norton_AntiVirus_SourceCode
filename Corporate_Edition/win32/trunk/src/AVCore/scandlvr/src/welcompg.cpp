// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/WelcomPg.cpv   1.5   26 May 1998 18:28:00   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// WelcomPg.cpp: implementation of the CWelcomePropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/WelcomPg.cpv  $
// 
//    Rev 1.5   26 May 1998 18:28:00   SEDWARD
// Put 'm_' in front of the bitmap member variables.
//
//    Rev 1.4   26 May 1998 16:18:58   SEDWARD
// Fixed bitmap ID error in constructor.
//
//    Rev 1.3   25 May 1998 13:50:12   SEDWARD
// Set bitmap IDs and caption in constructor.
//
//    Rev 1.2   26 Apr 1998 17:29:44   SEDWARD
// Added GetTemplateID(), now derive from CWizardPropertyPage.
//
//    Rev 1.1   20 Apr 1998 20:58:26   SEDWARD
// Added OnKillActive() and OnSetActive().
//
//    Rev 1.0   20 Apr 1998 18:10:40   SEDWARD
// Initial revision.
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "scandlvr.h"
#include "ScanWiz.h"
#include "WelcomPg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWelcomePropertyPage property page

IMPLEMENT_DYNCREATE(CWelcomePropertyPage, CWizardPropertyPage)

CWelcomePropertyPage::CWelcomePropertyPage()
    : CWizardPropertyPage(CWelcomePropertyPage::IDD, IDS_CAPTION_WELCOME)
{
    //{{AFX_DATA_INIT(CAcceptedFilesPropertyPage)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

    m_bitmap16 = IDB_SDBMP_WELCOME_16;
    m_bitmap256 = IDB_SDBMP_WELCOME_256;
}

CWelcomePropertyPage::~CWelcomePropertyPage()
{
}

void CWelcomePropertyPage::DoDataExchange(CDataExchange* pDX)
{
    CWizardPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CWelcomePropertyPage)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWelcomePropertyPage, CWizardPropertyPage)
    //{{AFX_MSG_MAP(CWelcomePropertyPage)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWelcomePropertyPage message handlers



// ==== OnKillActive ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL CWelcomePropertyPage::OnKillActive(void)
{
	// TODO: Add your specialized code here and/or call the base class

    auto    CScanDeliverWizard*         pWizSheet;
    auto    DWORD                       dwFlags = PSWIZB_NEXT | PSWIZB_BACK;

    // get a pointer to the parent window (the property sheet)

    pWizSheet = (CScanDeliverWizard*)this->GetParent();
    if (NULL != pWizSheet)
        {
        pWizSheet->SetWizardButtons(dwFlags);
        }

    return CWizardPropertyPage::OnKillActive();

}  // end of "CWelcomePropertyPage::OnKillActive"



// ==== OnSetActive =======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL CWelcomePropertyPage::OnSetActive(void)
{
	// TODO: Add your specialized code here and/or call the base class

    auto    CScanDeliverWizard*         pWizSheet;
    auto    DWORD                       dwFlags = PSWIZB_NEXT;

    // get a pointer to the parent window (the property sheet) and set the
    // wizard button flags
    pWizSheet = (CScanDeliverWizard*)this->GetParent();
    if (NULL != pWizSheet)
        {
        pWizSheet->SetWizardButtons(dwFlags);
        }

    // NOTE: the MS docs say that the "override of this member function should
    // call the default version before any other processing is done"; however,
    // the framework generates the parent call on the return statement...
    return CWizardPropertyPage::OnSetActive();

}  // end of "CWelcomePropertyPage::OnSetActive"