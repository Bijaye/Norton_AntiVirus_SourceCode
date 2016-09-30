// WizardPanelE.cpp : implementation file
//

#include "stdafx.h"
#include "DefAnnty.h"
#include "SubWizard.h"
//#include "WizardPanelE.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum BUTTONS { BUTTON_YES, BUTTON_NO };

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelE property page

IMPLEMENT_DYNCREATE(CWizardPanelE, CPropertyPage)

CWizardPanelE::CWizardPanelE() : CPropertyPage(CWizardPanelE::IDD)
{
	//{{AFX_DATA_INIT(CWizardPanelE)
	m_iButton = -1;
	//}}AFX_DATA_INIT
	m_pParent = NULL;
	m_psp.dwFlags &= (~PSP_HASHELP);
}

CWizardPanelE::~CWizardPanelE()
{
}

void CWizardPanelE::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardPanelE)
	DDX_Radio(pDX, IDC_RADIO1, m_iButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizardPanelE, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardPanelE)
	ON_BN_CLICKED(IDC_RADIO1, OnYes)
	ON_BN_CLICKED(IDC_RADIO2, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelE message handlers

BOOL CWizardPanelE::OnInitDialog() 
{	CWnd *pText;
	CSubWizard *pParent;
	CString csTemp;

	CPropertyPage::OnInitDialog();

	m_pParent->SetWizardButtons(PSWIZB_NEXT);
	m_pParent->SetTitle(m_csTitle);
	pText = GetDlgItem(IDC_TITLESTATIC);
    AdjustHeaderFont ( pText, &m_Font );
	pParent = (CSubWizard *) m_pParent;
	pText = GetDlgItem(IDC_WIZFORMAT);
    csTemp.Format(pParent->GetRemaining() == 1 ? IDS_WIZARDFMT2_ONE_DAY : IDS_WIZARDFMT2, pParent->GetRemaining());
	pText->SetWindowText(csTemp);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWizardPanelE::OnWizardNext() 
{	CSubWizard *pParent;

	pParent = (CSubWizard *) m_pParent;
	UpdateData();
	switch(m_iButton)
	{	case BUTTON_YES:
		{	DWORD dwInetEnable = 0;
			DefAnnuityGetInetEnable(&dwInetEnable);
			if(IsVeniceMode() && dwInetEnable)
				m_pParent->SetActivePage(PANEL_VENICE);
			else
				m_pParent->SetActivePage(PANEL_SUBINFO);
			break;
		}
		case BUTTON_NO:
			m_pParent->SetActivePage(PANEL_FINALALL);
			break;
	}
	pParent->SetLastPanel(this);
	return -1;
//	return CPropertyPage::OnWizardNext();
}

void CWizardPanelE::MySetWizardButtons()
{	UpdateData(TRUE);
	if(m_iButton == -1)
		m_pParent->SetWizardButtons(0);
	else
		m_pParent->SetWizardButtons(PSWIZB_NEXT);
}

BOOL CWizardPanelE::OnSetActive() 
{
	MySetWizardButtons();
	return CPropertyPage::OnSetActive();
}

void CWizardPanelE::OnYes() 
{
	MySetWizardButtons();
}

void CWizardPanelE::OnNo() 
{
	MySetWizardButtons();
}
