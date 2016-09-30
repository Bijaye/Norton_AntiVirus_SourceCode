// TWSaveTaskPage.cpp : implementation file
//

#include "stdafx.h"
#include "globals.h"

#include "ScanTask.h"
#include "TWIntroPage.h"
#include "TWSelectItemsPage.h"
#include "TWSaveTaskPage.h"
#include "TWSaveEditedTaskPage.h"
#include "wizard.h"
#include "AVRESBranding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
		CBrandingRes BrandRes;
        ::MessageBox ( pDX->m_pDlgWnd->GetSafeHwnd(),
			_S(IDS_ENTER_TASK_NAME), BrandRes.ProductName(),
                       MB_ICONEXCLAMATION );

        pDX->Fail();    // throws an exception and does not return
    }
    else if ( -1 != sName.FindOneOf ( _T("\'\"\\/<>:*?|.") ) )
    {
		CBrandingRes BrandRes;
        ::MessageBox ( pDX->m_pDlgWnd->GetSafeHwnd(),
                       _S(IDS_ILLEGAL_CHAR_IN_NAME), BrandRes.ProductName(),
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
        m_sTaskName = pTask->GetName ();

    UpdateData (FALSE);

    // OnInitDialog() called UpdateData() which set the character limit on
    // the task name edit box.

    m_Picture.SetBitmap ( g_bUseHiColorBmps ? IDB_WIZ256_3 : IDB_WIZ16_3, 
                          111, 236 );	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CTWSaveTaskPage::OnSetActive() 
{
    GetParentSheet()->SetWizardButtons ( PSWIZB_BACK | PSWIZB_FINISH );

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
        return FALSE;   // Stay on this page
    }

    // BUG: The MFC DDV code normally sets the focus to the control that
    //      failed validation (in this case, the edit box), but it isn't
    //      working here for some reason.  I saw no reason for this [MFC
    //      just calls ::SetFocus()], and if I call it here myself, it
    //      still doesn't work. :(
    
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
