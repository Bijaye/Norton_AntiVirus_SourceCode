// WizardPanelG.cpp : implementation file
//

#include "stdafx.h"
#include "DefAnnty.h"
#include "SubWizard.h"
//#include "WizardPanelG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// WizardPanelG property page

IMPLEMENT_DYNCREATE(WizardPanelG, CPropertyPage)

WizardPanelG::WizardPanelG() : CPropertyPage(WizardPanelG::IDD)
{
	//{{AFX_DATA_INIT(WizardPanelG)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pParent = NULL;
	m_psp.dwFlags &= (~PSP_HASHELP);
}

WizardPanelG::~WizardPanelG()
{
}

void WizardPanelG::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(WizardPanelG)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(WizardPanelG, CPropertyPage)
	//{{AFX_MSG_MAP(WizardPanelG)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// WizardPanelG message handlers

BOOL WizardPanelG::OnInitDialog() 
{	CWnd *pText;

	CPropertyPage::OnInitDialog();

	m_pParent->SetTitle(m_csTitle);
	pText = GetDlgItem(IDC_TITLESTATIC);
    AdjustHeaderFont ( pText, &m_Font );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL WizardPanelG::OnSetActive() 
{
	m_pParent->SetWizardButtons(PSWIZB_FINISH | PSWIZB_BACK);
	
	return CPropertyPage::OnSetActive();
}

LRESULT WizardPanelG::OnWizardBack() 
{	CSubWizard *pParent;

	pParent = (CSubWizard *) m_pParent;
	pParent->GotoLastPanel();
	return -1;
//	return CPropertyPage::OnWizardBack();
}

BOOL WizardPanelG::OnWizardFinish() 
{	CSubWizard *pParent;

	pParent = (CSubWizard *) m_pParent;
	pParent->SetResult(RESULT_PATCHES);
	
	return CPropertyPage::OnWizardFinish();
}
