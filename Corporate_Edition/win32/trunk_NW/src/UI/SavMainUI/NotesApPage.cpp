// NotesApPage.cpp : implementation file
//

#include "stdafx.h"
#include "SavMainUI.h"
#include "NotesApPage.h"


// CNotesApPage dialog
IMPLEMENT_DYNAMIC(CNotesApPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CNotesApPage, CPropertyPage)
END_MESSAGE_MAP()

CNotesApPage::CNotesApPage()
    : CPropertyPage(CNotesApPage::IDD)
{
    ldvpCtrl.SetSavSnapinID(CLdvpSnapinCtrl::SavSnapin_Notes);
}

CNotesApPage::~CNotesApPage()
{
    // Nothing for now
}

void CNotesApPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LOTNOTESUICTRL, ldvpCtrl);
}

// CNotesApPage message handlers
void CNotesApPage::OnOK()
{
    ldvpCtrl.Store();
}

BOOL CNotesApPage::OnInitDialog()
{
    CDialog::OnInitDialog();
    // Required or page will freeze
    ldvpCtrl.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
    return TRUE;
}
