// WizardPanelH.cpp : implementation file
//

#include "stdafx.h"
#include "DefAnnty.h"
#include "SubWizard.h"
//#include "WizardPanelH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// WizardPanelH property page

IMPLEMENT_DYNCREATE(WizardPanelH, CPropertyPage)

WizardPanelH::WizardPanelH() : CPropertyPage(WizardPanelH::IDD)
{
	//{{AFX_DATA_INIT(WizardPanelH)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pParent = NULL;
	m_psp.dwFlags &= (~PSP_HASHELP);
}

WizardPanelH::~WizardPanelH()
{
}

void WizardPanelH::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(WizardPanelH)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(WizardPanelH, CPropertyPage)
	//{{AFX_MSG_MAP(WizardPanelH)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// WizardPanelH message handlers

BOOL WizardPanelH::OnInitDialog() 
{	CWnd *pText;

	CPropertyPage::OnInitDialog();

	m_pParent->SetTitle(m_csTitle);
	pText = GetDlgItem(IDC_TITLESTATIC);
    AdjustHeaderFont ( pText, &m_Font );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL WizardPanelH::OnSetActive() 
{
	m_pParent->SetWizardButtons(PSWIZB_FINISH | PSWIZB_BACK);
	
	return CPropertyPage::OnSetActive();
}

LRESULT WizardPanelH::OnWizardBack() 
{	CSubWizard *pParent;

	pParent = (CSubWizard *) m_pParent;
	pParent->GotoLastPanel();
	return -1;
//	return CPropertyPage::OnWizardBack();
}

BOOL WizardPanelH::OnWizardFinish() 
{	CSubWizard *pParent;

	pParent = (CSubWizard *) m_pParent;
	pParent->SetResult(RESULT_DOWNLOAD);
	
	return CPropertyPage::OnWizardFinish();
}
