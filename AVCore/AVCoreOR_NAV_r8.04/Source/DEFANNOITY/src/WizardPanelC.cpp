// WizardPanelC.cpp : implementation file
//

#include "stdafx.h"
#include "DefAnnty.h"
#include "SubWizard.h"
//#include "WizardPanelC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelC property page

IMPLEMENT_DYNCREATE(CWizardPanelC, CPropertyPage)

CWizardPanelC::CWizardPanelC() : CPropertyPage(CWizardPanelC::IDD)
{
	//{{AFX_DATA_INIT(CWizardPanelC)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pParent = NULL;
	m_psp.dwFlags &= (~PSP_HASHELP);
}

CWizardPanelC::~CWizardPanelC()
{
}

void CWizardPanelC::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardPanelC)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizardPanelC, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardPanelC)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelC message handlers

BOOL CWizardPanelC::OnInitDialog() 
{	CWnd *pText;

	CPropertyPage::OnInitDialog();

	m_pParent->SetTitle(m_csTitle);
	pText = GetDlgItem(IDC_TITLESTATIC);
    AdjustHeaderFont ( pText, &m_Font );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWizardPanelC::OnSetActive() 
{
	m_pParent->SetWizardButtons(PSWIZB_FINISH | PSWIZB_BACK);
	
	return CPropertyPage::OnSetActive();
}

LRESULT CWizardPanelC::OnWizardBack() 
{	CSubWizard *pParent;

	pParent = (CSubWizard *) m_pParent;
	pParent->GotoLastPanel();
	return -1;
//	return CPropertyPage::OnWizardBack();
}

BOOL CWizardPanelC::OnWizardFinish() 
{	CSubWizard *pParent;

	pParent = (CSubWizard *) m_pParent;
	pParent->SetResult(RESULT_SUBSCRIBED);
	
	return CPropertyPage::OnWizardFinish();
}
