// WizardPanelF.cpp : implementation file
//

#include "stdafx.h"
#include "DefAnnty.h"
#include "SubWizard.h"
//#include "WizardPanelF.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum BUTTONS { BUTTON_YES, BUTTONS_NOPATCH, BUTTON_NO };

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelF property page

IMPLEMENT_DYNCREATE(CWizardPanelF, CPropertyPage)

CWizardPanelF::CWizardPanelF() : CPropertyPage(CWizardPanelF::IDD)
{
	//{{AFX_DATA_INIT(CWizardPanelF)
	m_iButton = -1;
	//}}AFX_DATA_INIT
	m_pParent = NULL;
	m_psp.dwFlags &= (~PSP_HASHELP);
}

CWizardPanelF::~CWizardPanelF()
{
}

void CWizardPanelF::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardPanelF)
	DDX_Radio(pDX, IDC_RADIO1, m_iButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizardPanelF, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardPanelF)
	ON_BN_CLICKED(IDC_RADIO1, OnYes)
	ON_BN_CLICKED(IDC_RADIO2, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelF message handlers

BOOL CWizardPanelF::OnInitDialog() 
{	CWnd *pText;

	CPropertyPage::OnInitDialog();

	m_pParent->SetTitle(m_csTitle);
	pText = GetDlgItem(IDC_TITLESTATIC);
    AdjustHeaderFont ( pText, &m_Font );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWizardPanelF::OnWizardNext() 
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

void CWizardPanelF::MySetWizardButtons()
{	UpdateData(TRUE);
	if(m_iButton == -1)
		m_pParent->SetWizardButtons(0);
	else
		m_pParent->SetWizardButtons(PSWIZB_NEXT);
}

BOOL CWizardPanelF::OnSetActive() 
{
	MySetWizardButtons();
	return CPropertyPage::OnSetActive();
}

void CWizardPanelF::OnYes() 
{
	MySetWizardButtons();
}

void CWizardPanelF::OnNo() 
{
	MySetWizardButtons();
}
