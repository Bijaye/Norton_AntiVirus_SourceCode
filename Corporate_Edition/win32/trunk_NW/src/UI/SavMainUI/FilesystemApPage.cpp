// FilesystemApPage.cpp : implementation file
//

#include "stdafx.h"
#include "SavMainUI.h"
#include "FilesystemApPage.h"


// CFilesystemApPage dialog
IMPLEMENT_DYNAMIC(CFilesystemApPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CFilesystemApPage, CPropertyPage)
END_MESSAGE_MAP()

CFilesystemApPage::CFilesystemApPage()
	: CPropertyPage(CFilesystemApPage::IDD)
{
    ldvpCtrl.SetSavSnapinID(CLdvpSnapinCtrl::SavSnapin_Filesystem);
}

CFilesystemApPage::~CFilesystemApPage()
{
    // Nothing for now
}

void CFilesystemApPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LDVPUICTRL, ldvpCtrl);
}

// CFilesystemApPage message handlers
void CFilesystemApPage::OnOK()
{
    ldvpCtrl.Store();
}

BOOL CFilesystemApPage::OnInitDialog()
{
    CDialog::OnInitDialog();
    // Required or page will freeze
    ldvpCtrl.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
    return TRUE;
}
