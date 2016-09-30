// ExchangeApPage.cpp : implementation file
//

#include "stdafx.h"
#include "SavMainUI.h"
#include "ExchangeApPage.h"


// ExchangeApPage page
IMPLEMENT_DYNAMIC(CExchangeApPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CExchangeApPage, CPropertyPage)
END_MESSAGE_MAP()

CExchangeApPage::CExchangeApPage()
	: CPropertyPage(CExchangeApPage::IDD)
{
    ldvpCtrl.SetSavSnapinID(CLdvpSnapinCtrl::SavSnapin_Exchange);
}

CExchangeApPage::~CExchangeApPage()
{
    // Nothing for now
}

void CExchangeApPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EXCHNGUICTRL, ldvpCtrl);
}

// ExchangeApPage message handlers
void CExchangeApPage::OnOK()
{
    ldvpCtrl.Store();
}

BOOL CExchangeApPage::OnInitDialog()
{
    CDialog::OnInitDialog();
    // Required or page will freeze
    ldvpCtrl.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
    return TRUE;
}
