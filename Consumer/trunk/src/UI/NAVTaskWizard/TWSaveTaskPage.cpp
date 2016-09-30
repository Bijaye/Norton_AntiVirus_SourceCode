////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TWSaveTaskPage.cpp : implementation file
//

#include "stdafx.h"
#include "globals.h"

#include "ScanTaskInterface.h"
#include "TWIntroPage.h"
#include "TWSelectItemsPage.h"
#include "TWSaveTaskPage.h"
#include "TWSaveEditedTaskPage.h"
#include "wizard.h"
#include "ISVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace avScanTask;

// User defined message to set focus to the task name edit box
#define UDM_RESET_FOCUS WM_USER + 1


/////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//
// Function:    DDV_ValidFilename
//
// Description:
//  DDV function that validates a string contains no characters that are not
//  valid in a file name.
//
// Input:
//  pDX: Pointer to a CDataExchange object provided by MFC.
//  sName: [in] The string to validate.
//
// Returns:
//  Nothing.  If validation fails, this fn throws a CUserException that 
//  MFC catches.
//
//////////////////////////////////////////////////////////////////////////

void AFXAPI DDV_ValidFilename ( CDataExchange* pDX, const CString& sName )
{
    // If MFC is transferring data _to_ the dialog, we don't have anything
    // to do.
    if ( !pDX->m_bSaveAndValidate )
        return;

    if ( sName.GetLength() < 1 )
    {
		CString sMsg;
		sMsg.LoadString( GetResInst(), IDS_ENTER_TASK_NAME );
        ::MessageBox ( pDX->m_pDlgWnd->GetSafeHwnd(),
			sMsg, CISVersion::GetProductName(),
                       MB_ICONEXCLAMATION );

        pDX->Fail();    // throws an exception and does not return
    }
    else if ( -1 != sName.FindOneOf ( _T("\'\"\\/<>:*?|.") ) )
    {
		CString sMsg;
		sMsg.LoadString( GetResInst(), IDS_ILLEGAL_CHAR_IN_NAME );
        ::MessageBox ( pDX->m_pDlgWnd->GetSafeHwnd(),
			sMsg, CISVersion::GetProductName(),
                       MB_ICONEXCLAMATION );

        pDX->Fail();    // throws an exception and does not return
    }
}


/////////////////////////////////////////////////////////////////////////////
// CTWSaveTaskPage property page

IMPLEMENT_DYNCREATE(CTWSaveTaskPage, CBaseWizPage)

CTWSaveTaskPage::CTWSaveTaskPage() : CBaseWizPage(IDD_TW_SAVETASK)
//CTWSaveTaskPage::CTWSaveTaskPage() : CBaseWizPage(CTWSaveTaskPage::IDD)
{
    //{{AFX_DATA_INIT(CTWSaveTaskPage)
    m_sTaskName = _T("");
	//}}AFX_DATA_INIT
}

CTWSaveTaskPage::~CTWSaveTaskPage()
{
}

void CTWSaveTaskPage::DoDataExchange(CDataExchange* pDX)
{
	CBaseWizPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTWSaveTaskPage)
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
	DDX_Text(pDX, IDC_TASK_NAME, m_sTaskName);
	//}}AFX_DATA_MAP
    
    // Custom filename validation: check the length of the task name, making
    // sure that the full filename it will eventually have has _less than_
    // TASK_MAX_NAME_LENGTH characters including the terminating null.

    pDX->PrepareEditCtrl (IDC_TASK_NAME);
    DDV_MaxChars ( pDX, m_sTaskName, 64 );
    DDV_ValidFilename ( pDX, m_sTaskName );
}               


BEGIN_MESSAGE_MAP(CTWSaveTaskPage, CBaseWizPage)
	//{{AFX_MSG_MAP(CTWSaveTaskPage)
	ON_MESSAGE(UDM_RESET_FOCUS, OnResetFocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTWSaveTaskPage message handlers

BOOL CTWSaveTaskPage::OnInitDialog() 
{
	CBaseWizPage::OnInitDialog();

    CTaskWizard* pParent = static_cast<CTaskWizard*>(GetParent());
    IScanTask* pTask = pParent->GetScanTask ();

    if ( pTask )
	{
		cc::IStringPtr pName;
        if(SUCCEEDED(pTask->GetName (&pName)))
		{
			m_sTaskName = pName->GetStringW();
		}
	}

    UpdateData (FALSE);

    // OnInitDialog() called UpdateData() which set the character limit on
    // the task name edit box.

    m_Picture.SetBitmap ( g_bUseHiColorBmps ? IDB_WIZ256_3 : IDB_WIZ16_3, 
                          111, 236 );	

	// Set body texts	
	CString csText;

	csText.LoadString(GetResInst(), IDS_STATIC_SCAN_NAME);
	SetDlgItemText(IDC_STATIC_SCAN_NAME, csText.GetString());

	csText.LoadString(GetResInst(), IDS_STATIC_ENTER_NAME);
	SetDlgItemText(IDC_STATIC_ENTER_NAME, csText.GetString());

	csText.LoadString(GetResInst(), IDS_STATIC_PROMPT_SAVE);
	SetDlgItemText(IDC_STATIC_PROMPT_SAVE, csText.GetString());

	csText.LoadString(GetResInst(), IDS_CANCEL);
	GetParentSheet()->SetDlgItemText(IDCANCEL, csText);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CTWSaveTaskPage::OnSetActive() 
{
    GetParentSheet()->SetWizardButtons ( PSWIZB_BACK | PSWIZB_FINISH );

	CString csText;
	csText.LoadString(GetResInst(), IDS_FINISH);
	GetParentSheet()->SetFinishText(csText.GetString());
	
	return CBaseWizPage::OnSetActive();
}


BOOL CTWSaveTaskPage::OnKillActive() 
{
    // I don't call the base class OnKillActive() because it calls UpdateData().
    // UpdateData() causes the DDV to execute when the user clicks the Back
    // button, and I didn't like how that felt.  The DDV error should only
    // appear after clicking Finish.
    // Note that a side effect is that if you get to this page,
    // type in a name, and then go back to the item selection page, the task
    // name you enter is lost.

    return TRUE;                        // allow the page to be deactivated
}


BOOL CTWSaveTaskPage::OnWizardFinish() 
{
    if ( !UpdateData ( TRUE ))
    {
		// Calling SetFocus() to set focus on the task name edit box works should generally work. 
		// However, for the case in which the page is a finish page, 
		// and the validation in this page failed, and we return FALSE in OnWizardFinish, 
		// we must call PostMessage() to post a user defined message 
		// in which will SetFocus() on the edit box.
		PostMessage(UDM_RESET_FOCUS, 0, 0);

        return FALSE;   // Stay on this page
    }
  
    CTaskWizard* pParent = static_cast<CTaskWizard*>(GetParent());
    IScanTask* pTask = pParent->GetScanTask ();

    if ( !pTask )
        return FALSE;
    
    // Set the name of this task
    //
    pTask->SetName ( m_sTaskName.GetBuffer(1) );

    m_sTaskName.ReleaseBuffer ();

	return TRUE;                        // ok to close the wizard
}

LRESULT CTWSaveTaskPage::OnResetFocus(WPARAM wParam, LPARAM lParam)
{
	// Set focus to the task name edit box
	GetDlgItem(IDC_TASK_NAME)->SetFocus();
	return 0;
}
