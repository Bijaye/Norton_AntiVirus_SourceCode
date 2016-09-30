// InternetMailApPage.cpp : implementation file
//

#include "stdafx.h"
#include "SavMainUI.h"
#include "InternetMailApPage.h"


// CInternetMailApPage dialog
IMPLEMENT_DYNAMIC(CInternetMailApPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CInternetMailApPage, CPropertyPage)
END_MESSAGE_MAP()

CInternetMailApPage::CInternetMailApPage()
    : CPropertyPage(CInternetMailApPage::IDD)
{
    ldvpCtrl.SetSavSnapinID(CLdvpSnapinCtrl::SavSnapin_InternetEmail);
}

CInternetMailApPage::~CInternetMailApPage()
{
    // Nothing for now
}

void CInternetMailApPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_INETMAILUICTRL, ldvpCtrl);
}

// CInternetMailApPage message handlers
void CInternetMailApPage::OnOK()
{
    ldvpCtrl.Store();
}

BOOL CInternetMailApPage::OnInitDialog()
{
    CDialog::OnInitDialog();
    // Required or page will freeze
    ldvpCtrl.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
    return TRUE;
}
