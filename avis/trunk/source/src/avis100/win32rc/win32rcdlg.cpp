// win32rcDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "defs.h"
#include "win32rcdlg.h"
#include "rcerror.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWin32rcDlg dialog

#define GENINFO_DLG_TIMER 1
#define TIMER_TICKS 5000

CWin32rcDlg::CWin32rcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWin32rcDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWin32rcDlg)
	m_CurrentTxt = _T("alla");
	//}}AFX_DATA_INIT
	Create(IDD_WIN32RC_DIALOG, pParent);
}

void CWin32rcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWin32rcDlg)
	DDX_Text(pDX, IDC_INFO_TXT, m_CurrentTxt);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWin32rcDlg, CDialog)
	//{{AFX_MSG_MAP(CWin32rcDlg)
     ON_MESSAGE(ON_SETTEXT, OnSetText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWin32rcDlg message handlers

BOOL CWin32rcDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CTime theTime;
	//theTime = CTime::GetCurrentTime();
    //CString s = theTime.Format( "%A, %B %d, %Y" );

	CString strTxt;
	strTxt.Format("RC for Win32 viruses - version %s", VERNUMBER);
	SetWindowText(strTxt);
	SetTimer(GENINFO_DLG_TIMER,TIMER_TICKS,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
}

LONG CWin32rcDlg::OnSetText(WPARAM p1, LPARAM p2)
{
   	CString *pstrText = (CString *)p1;
    SetDlgItemText(IDC_INFO_TXT, *pstrText);
//	m_CurrentTxt = *pstrText;
    return 0;
}


