// WizardPanelD.cpp : implementation file
//

#include "stdafx.h"
#include "DefAnnty.h"
#include "SubWizard.h"
//#include "WizardPanelD.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum BUTTONS { BUTTON_YES, BUTTONS_NOPATCH, BUTTON_NO };

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelD property page

IMPLEMENT_DYNCREATE(CWizardPanelD, CPropertyPage)

CWizardPanelD::CWizardPanelD() : CPropertyPage(CWizardPanelD::IDD)
{
	//{{AFX_DATA_INIT(CWizardPanelD)
	m_iButton = -1;
	//}}AFX_DATA_INIT
	m_pParent = NULL;
	m_psp.dwFlags &= (~PSP_HASHELP);
}

CWizardPanelD::~CWizardPanelD()
{
}

void CWizardPanelD::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardPanelD)
	DDX_Radio(pDX, IDC_RADIO1, m_iButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizardPanelD, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardPanelD)
	ON_BN_CLICKED(IDC_RADIO1, OnYes)
	ON_BN_CLICKED(IDC_RADIO2, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelD message handlers

BOOL CWizardPanelD::OnInitDialog() 
{	CWnd *pText;

	CPropertyPage::OnInitDialog();

	m_pParent->SetWizardButtons(PSWIZB_NEXT);
	m_pParent->SetTitle(m_csTitle);
	pText = GetDlgItem(IDC_TITLESTATIC);
    AdjustHeaderFont ( pText, &m_Font );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWizardPanelD::OnWizardNext() 
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
		case BUTTONS_NOPATCH:
			m_pParent->SetActivePage(PANEL_FINALPATCH);
			break;
		case BUTTON_NO:
			m_pParent->SetActivePage(PANEL_FINALALL);
			break;
	}
	pParent->SetLastPanel(this);
	return -1;
//	return CPropertyPage::OnWizardNext();
}

void CWizardPanelD::MySetWizardButtons()
{	UpdateData(TRUE);
	if(m_iButton == -1)
		m_pParent->SetWizardButtons(0);
	else
		m_pParent->SetWizardButtons(PSWIZB_NEXT);
}

BOOL CWizardPanelD::OnSetActive() 
{
	MySetWizardButtons();
	return CPropertyPage::OnSetActive();
}

void CWizardPanelD::OnYes() 
{
	MySetWizardButtons();
}

void CWizardPanelD::OnNo() 
{
	MySetWizardButtons();
}
