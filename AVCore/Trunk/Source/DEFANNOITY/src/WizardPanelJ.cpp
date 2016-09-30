// WizardPanelJ.cpp : implementation file
//

#include "stdafx.h"
#include "DefAnnty.h"
#include "SubWizard.h"
//#include "WizardPanelJ.h"
#include "SymWorld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void LaunchUrl();

enum BUTTONS { BUTTON_INTERNET, BUTTON_SYMANTEC };

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelJ property page

IMPLEMENT_DYNCREATE(CWizardPanelJ, CPropertyPage)

CWizardPanelJ::CWizardPanelJ() : CPropertyPage(CWizardPanelJ::IDD)
{
	//{{AFX_DATA_INIT(CWizardPanelJ)
	m_iButton = -1;
//	m_strConfirmation = _T("");
	//}}AFX_DATA_INIT
	m_pParent = NULL;
	m_psp.dwFlags &= (~PSP_HASHELP);
}

CWizardPanelJ::~CWizardPanelJ()
{
}

void CWizardPanelJ::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardPanelJ)
	DDX_Radio(pDX, IDC_RADIO1, m_iButton);
//	DDX_Text(pDX, IDC_EDIT2, m_strConfirmation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizardPanelJ, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardPanelJ)
	ON_BN_CLICKED(IDC_RADIO1, OnInternet)
	ON_BN_CLICKED(IDC_RADIO2, OnSymantec)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelJ message handlers

BOOL CWizardPanelJ::OnInitDialog() 
{	CWnd *pText;

	CPropertyPage::OnInitDialog();

	m_pParent->SetTitle(m_csTitle);
	pText = GetDlgItem(IDC_TITLESTATIC);
    AdjustHeaderFont ( pText, &m_Font );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWizardPanelJ::OnWizardNext() 
{	CSubWizard *pParent;
    CString csNumber;

	pParent = (CSubWizard *) m_pParent;
	UpdateData();
	switch(m_iButton)
	{	case BUTTON_INTERNET:
			m_pParent->SetActivePage(PANEL_FINALSUB);
			break;
		case BUTTON_SYMANTEC:
			m_pParent->SetActivePage(PANEL_SUBINFO);
			break;
	}
	pParent->SetLastPanel(this);
	return -1;
//	return CPropertyPage::OnWizardNext();
}

BOOL CWizardPanelJ::OnWizardFinish() 
{	CSubWizard *pParent;

	pParent = (CSubWizard *) m_pParent;
	pParent->SetResult(RESULT_CANCEL);
	LaunchUrl();
	
	return CPropertyPage::OnWizardFinish();
}

LRESULT CWizardPanelJ::OnWizardBack() 
{	CSubWizard *pParent;

	pParent = (CSubWizard *) m_pParent;
	pParent->SetState(pParent->GetState());
	return -1;
//	return CPropertyPage::OnWizardBack();
}

void CWizardPanelJ::MySetWizardButtons()
{	UpdateData(TRUE);
	switch(m_iButton)
	{	case -1:
			m_pParent->SetWizardButtons(PSWIZB_BACK);
			break;
		case 0:
			m_pParent->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
			break;
		case 1:
			m_pParent->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
			break;
	}
}

BOOL CWizardPanelJ::OnSetActive() 
{
	MySetWizardButtons();
	return CPropertyPage::OnSetActive();
}

void CWizardPanelJ::OnInternet() 
{
	MySetWizardButtons();
}

void CWizardPanelJ::OnSymantec() 
{
	MySetWizardButtons();
}
