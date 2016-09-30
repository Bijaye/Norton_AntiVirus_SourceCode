// FutzerificDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SavMainUI.h"
#include "FutzerificDlg.h"
#include <afxpriv.h>


// CFutzerificDlg dialog

IMPLEMENT_DYNAMIC(CFutzerificDlg, CDialog)

CFutzerificDlg::CFutzerificDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFutzerificDlg::IDD, pParent)
{

}

CFutzerificDlg::~CFutzerificDlg()
{
}

void CFutzerificDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LDVPUICTRL1, uiCtrl);
}


BEGIN_MESSAGE_MAP(CFutzerificDlg, CDialog)
END_MESSAGE_MAP()


// CFutzerificDlg message handlers
BOOL CFutzerificDlg::OnInitDialog()
{
    BOOL rvb = FALSE;
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    rvb = CDialog::OnInitDialog();
    // Note that the control already has this style in it's RC, but have to do this anyway
    // for the LDVP OCX controls.  Microsoft controls (e.g., datagrid, datepicker) don't
    // require this.
    // This must be done all the way up the chain, control->sheet
    uiCtrl.ModifyStyleEx(NULL, WS_EX_CONTROLPARENT);
    return TRUE;
}