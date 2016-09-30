/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/CorpRepairDlg.cpv   1.1   18 May 1998 13:34:08   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// CorpRepairDlg.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/CorpRepairDlg.cpv  $
// 
//    Rev 1.1   18 May 1998 13:34:08   DBuches
// Added restore text.
// 
//    Rev 1.0   11 May 1998 15:43:34   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "CorpRepairDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCorpRepairDlg dialog


CCorpRepairDlg::CCorpRepairDlg(CWnd* pParent /*=NULL*/, BOOL bRepair)
	: CDialog(CCorpRepairDlg::IDD, pParent),
      m_bRepair( bRepair )
{
	//{{AFX_DATA_INIT(CCorpRepairDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCorpRepairDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCorpRepairDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCorpRepairDlg, CDialog)
	//{{AFX_MSG_MAP(CCorpRepairDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCorpRepairDlg message handlers


////////////////////////////////////////////////////////////////////////////
// Function name	: CCorpRepairDlg::OnInitDialog
//
// Description	    : Handles WM_INITDIALOG message
//
// Return type		: BOOL 
//
//
////////////////////////////////////////////////////////////////////////////
// 5/11/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CCorpRepairDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    CString s;

    // Set static icon to question mark.
    HICON hIcon = LoadIcon( NULL, IDI_QUESTION );
    ((CStatic*)GetDlgItem(IDC_ICON_STATIC))->SetIcon( hIcon );

    // Load correct text
    if( m_bRepair )
        {
        s.LoadString( IDS_CORP_REPAIR_TEXT );        
        GetDlgItem( IDC_MAIN_STATIC )->SetWindowText( s );
        }
    else
        {
        s.LoadString( IDS_CORP_RESTORE_TEXT );        
        GetDlgItem( IDC_MAIN_STATIC )->SetWindowText( s );

        // Set dlg title text correctly.
        s.LoadString( IDS_RESTORE_ITEMS );
        SetWindowText( s );
        }

    // Beep!
    MessageBeep( MB_ICONQUESTION );


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
