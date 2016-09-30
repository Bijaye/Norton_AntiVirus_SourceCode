/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/password.cpv   1.2   21 May 1998 11:28:24   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// password.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/password.cpv  $
// 
//    Rev 1.2   21 May 1998 11:28:24   DBuches
// Added help topics.
// 
//    Rev 1.1   20 May 1998 17:15:14   DBuches
// 1st pass at help
// 
//    Rev 1.0   03 Apr 1998 13:30:14   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "password.h"
#include "qconhlp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordChangeDlg dialog

// Help maps
DWORD CPasswordChangeDlg::m_dwWhatsThisMap[] = {
    IDC_NEW_STATIC,     IDH_QUARAN_OPTIONS_GENERAL_NEW_PASSWORD_BTN, 
    IDC_CONFIRM_STATIC, IDH_QUARAN_OPTIONS_GENERAL_CONFIRM_PASSWORD_BTN,
    IDC_OLD_STATIC,     IDH_QUARAN_OPTIONS_GENERAL_OLD_PASSWORD_BTN,    
    0,0};

DWORD CPasswordChangeDlg::m_dwHowToMap[] = {0,0};



CPasswordChangeDlg::CPasswordChangeDlg(CWnd* pParent /*=NULL*/)
	: CHelperDialog(CPasswordChangeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordChangeDlg)
	m_sConfirm = _T("");
	m_sNew = _T("");
	m_sOld = _T("");
	//}}AFX_DATA_INIT

    m_pHelpWhatsThisMap = m_dwWhatsThisMap;
    m_pHelpHowToMap = m_dwHowToMap;
    m_dwHelpButtonHelpID = IDH_QUARAN_OPTIONS_GENERAL_PASSWORD_HLPBTN;
}


void CPasswordChangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CHelperDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordChangeDlg)
	DDX_Control(pDX, IDC_NEW_PW_EDIT, m_CtrlNew);
	DDX_Text(pDX, IDC_CONFIRM_PW_EDIT, m_sConfirm);
	DDV_MaxChars(pDX, m_sConfirm, 16);
	DDX_Text(pDX, IDC_NEW_PW_EDIT, m_sNew);
	DDV_MaxChars(pDX, m_sNew, 16);
	DDX_Text(pDX, IDC_OLD_PW_EDIT, m_sOld);
	DDV_MaxChars(pDX, m_sOld, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordChangeDlg, CHelperDialog)
	//{{AFX_MSG_MAP(CPasswordChangeDlg)
	ON_EN_CHANGE(IDC_NEW_PW_EDIT, OnChangeNewPwEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordChangeDlg message handlers



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CPasswordChangeDlg::OnInitDialog
// Description	    : Handles WM_INITDIALOG message
// Return type		: BOOL 
//
////////////////////////////////////////////////////////////////////////////////
// 4/2/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
BOOL CPasswordChangeDlg::OnInitDialog() 
{
	CHelperDialog::OnInitDialog();

    int iLength = m_sPassword.GetLength();
    
	// If we have no password now, skip to new password edit area.
    if( iLength == 0 )
        {
        GetDlgItem( IDC_NEW_PW_EDIT )->EnableWindow( TRUE );
        GetDlgItem( IDC_NEW_PW_EDIT )->SetFocus();
        return FALSE;
        }
    else
        {
        // Enable old password edit field
        GetDlgItem( IDC_OLD_PW_EDIT )->EnableWindow( TRUE );
        GetDlgItem( IDC_OLD_PW_EDIT )->SetFocus();
        

        // Build actual password text from hex string
        CString sTemp;
        TCHAR   ch, *chStop;

        // Make sure we have an even number of characters
        iLength -= iLength % 2 ? 1 : 0;

        for( int i = 0; i < iLength; i += 2 )
            {
            // Convert each hex byte to char
            sTemp = _T("0x");
            sTemp += m_sPassword.Mid( i, 2 );
            ch = (TCHAR) strtoul( sTemp, &chStop, 16);

            // Add to password text
            m_sPasswordText += ch;
            }
        }
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CPasswordChangeDlg::OnChangeNewPwEdit
// Description	    : Handle notificaiton of change of new password edit control
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 4/2/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CPasswordChangeDlg::OnChangeNewPwEdit() 
{
    // Make sure there is text in this control.  If empty,
    // disable the confirmation edit control.
    CString s;
    m_CtrlNew.GetWindowText( s );
    if( s.GetLength() == 0 )
        {
        GetDlgItem( IDC_CONFIRM_PW_EDIT )->EnableWindow( FALSE );
        }
    else
        {
        GetDlgItem( IDC_CONFIRM_PW_EDIT )->EnableWindow( TRUE );
        }
	
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CPasswordChangeDlg::OnOK
// Description	    : Handles OK button click
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 4/2/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CPasswordChangeDlg::OnOK( )
{
    // Get data from controls.
    UpdateData();    

    if( m_sPassword.GetLength() )
        {
        if( m_sOld != m_sPasswordText )
            {
            DisplayError();
            GetDlgItem( IDC_OLD_PW_EDIT )->SetFocus();
            ((CEdit*)GetDlgItem( IDC_OLD_PW_EDIT ))->SetSel( 0, -1 );
            return;
            }
        
        if( m_sNew.GetLength() == 0 )
            {
            GetDlgItem( IDC_OLD_PW_EDIT )->EnableWindow( FALSE );
            GetDlgItem( IDC_CONFIRM_PW_EDIT )->EnableWindow( TRUE );
            m_CtrlNew.EnableWindow( TRUE );
            m_CtrlNew.SetFocus();
            return;
            }
        
        if( m_sConfirm.GetLength() == 0 )
            {
            GetDlgItem( IDC_CONFIRM_PW_EDIT )->SetFocus();
            return;
            }
        }

    // Make sure passwords match
    if( m_sNew != m_sConfirm )
        {
        // Tell the user what happened.
        DisplayError();

        // Clear entered passwords
        m_sNew = _T("");
        m_sConfirm = _T("");
        UpdateData( FALSE );

        // Set focus to new password field.
        m_CtrlNew.SetFocus();
        return;
        }

    // Get rid of old password
    m_sPassword.Empty();
    
    // Convert password to hex string.
    CString s;
    int iLength = m_sNew.GetLength();
    for( int i =0; i < iLength; i++ )
        {
        s.Format( "%02X", m_sNew[i] );
        m_sPassword += s;
        }

    // Call base class
    CHelperDialog::OnOK();
}



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CPasswordChangeDlg::DisplayError
// Description	    : 
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 4/2/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CPasswordChangeDlg::DisplayError()
{
    CString sTitle;
    GetWindowText( sTitle );
    CString sText((LPCTSTR) IDS_ERROR_PW_CONFIRM );
    MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
}
/////////////////////////////////////////////////////////////////////////////
// CPasswordCheckDlg dialog


CPasswordCheckDlg::CPasswordCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordCheckDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordCheckDlg)
	m_sPasswordText = _T("");
	//}}AFX_DATA_INIT
}


void CPasswordCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordCheckDlg)
	DDX_Text(pDX, IDC_EDIT1, m_sPasswordText);
	DDV_MaxChars(pDX, m_sPasswordText, 16);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordCheckDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordCheckDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordCheckDlg message handlers



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CPasswordCheckDlg::OnOK
// Description	    : Handles OK button press.
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 4/2/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CPasswordCheckDlg::OnOK() 
{
	// TODO: Add extra validation here
    UpdateData();
    
    // Convert password to hex string.
    CString s;
    CString sTest;
    int iLength = m_sPasswordText.GetLength();
    for( int i =0; i < iLength; i++ )
        {
        s.Format( "%02X", m_sPasswordText[i] );
        sTest += s;
        }

    // If password matches, return OK.
    if( sTest == m_sPassword )
        {
        CDialog::OnOK();
        return;
        }

    // Give the user another chance.
    CString sTitle;
    GetWindowText( sTitle );
    CString sText((LPCTSTR) IDS_ERROR_PW_CONFIRM );
    MessageBox( sText, sTitle, MB_ICONEXCLAMATION | MB_OK );
    GetDlgItem( IDC_EDIT1 )->SetFocus();
    ((CEdit*)GetDlgItem( IDC_EDIT1 ))->SetSel( 0, -1 );
    
}
