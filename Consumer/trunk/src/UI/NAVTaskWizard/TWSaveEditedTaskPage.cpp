////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TWSaveEditedTaskPage.cpp : implementation file
//

#include "stdafx.h"
#include "Globals.h"

#include "ScanTaskInterface.h"

#include "TWIntroPage.h"
#include "TWSelectItemsPage.h"
#include "TWSaveTaskPage.h"
#include "TWSaveEditedTaskPage.h"
#include "wizard.h"

#include "resource.h"
#include "..\\NavTaskWizardRes\\resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace avScanTask;

/////////////////////////////////////////////////////////////////////////////
// CTWSaveEditedTaskPage property page

IMPLEMENT_DYNCREATE(CTWSaveEditedTaskPage, CBaseWizPage)

CTWSaveEditedTaskPage::CTWSaveEditedTaskPage() : CBaseWizPage(IDD_TW_SAVEEDITEDTASK)
//CTWSaveEditedTaskPage::CTWSaveEditedTaskPage() : CBaseWizPage(CTWSaveEditedTaskPage::IDD)
{
	//{{AFX_DATA_INIT(CTWSaveEditedTaskPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CTWSaveEditedTaskPage::~CTWSaveEditedTaskPage()
{
}

void CTWSaveEditedTaskPage::DoDataExchange(CDataExchange* pDX)
{
	CBaseWizPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTWSaveEditedTaskPage)
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTWSaveEditedTaskPage, CBaseWizPage)
	//{{AFX_MSG_MAP(CTWSaveEditedTaskPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTWSaveEditedTaskPage message handlers

BOOL CTWSaveEditedTaskPage::OnInitDialog() 
{
    //ASSERT ( m_pTask != NULL );

	CBaseWizPage::OnInitDialog();
	
    m_Picture.SetBitmap ( g_bUseHiColorBmps ? IDB_WIZ256_3 : IDB_WIZ16_3,
                          111, 236 );	

	CString csText;

	csText.LoadString(GetResInst(), IDS_STATIC_CLICK_FINISH);
	SetDlgItemText(IDC_STATIC_CLICK_FINISH, csText);
	
	csText.LoadString(GetResInst(), IDS_STATIC_CLICK_BACK);
	SetDlgItemText(IDC_STATIC_CLICK_BACK, csText);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTWSaveEditedTaskPage::OnSetActive() 
{
    GetParentSheet()->SetWizardButtons ( PSWIZB_BACK | PSWIZB_FINISH );
	
	return CBaseWizPage::OnSetActive();
}

BOOL CTWSaveEditedTaskPage::OnWizardFinish() 
{
    CString      sTaskFile;
    CTaskWizard* pParent = static_cast<CTaskWizard*>(GetParent());
    IScanTask* pTask = pParent->GetScanTask ();
    
    if ( !pTask )
        return FALSE;

    // Use the existing path
    //
    if ( !pTask->Save ())
    {
		CString sMsg;
		sMsg.LoadString( GetResInst(), IDP_TASKWIZ_ERROR_SAVING_TASK );
        MessageBox ( sMsg, m_csProductName, MB_ICONSTOP );

        return FALSE;
    }

	return TRUE;                        // ok to close the wizard
}
