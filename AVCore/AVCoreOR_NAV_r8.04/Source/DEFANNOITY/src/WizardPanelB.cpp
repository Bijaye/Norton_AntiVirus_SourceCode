// WizardPanelB.cpp : implementation file
//

#include "stdafx.h"
#include "DefAnnty.h"
#include "SubWizard.h"
//#include "WizardPanelB.h"
#include "SymWorld.h"
#include "SCValid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum BUTTONS { BUTTON_CONFIRM, BUTTON_NO };

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelB property page

IMPLEMENT_DYNCREATE(CWizardPanelB, CPropertyPage)

CWizardPanelB::CWizardPanelB() : CPropertyPage(CWizardPanelB::IDD)
{
	//{{AFX_DATA_INIT(CWizardPanelB)
	m_iButton = -1;
	m_strConfirmation = _T("");
	//}}AFX_DATA_INIT
	m_pParent = NULL;
	m_psp.dwFlags &= (~PSP_HASHELP);
}

CWizardPanelB::~CWizardPanelB()
{
}

void CWizardPanelB::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardPanelB)
	DDX_Radio(pDX, IDC_RADIO1, m_iButton);
	DDX_Text(pDX, IDC_EDIT2, m_strConfirmation);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizardPanelB, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardPanelB)
	ON_BN_CLICKED(IDC_RADIO1, OnNumberIs)
	ON_BN_CLICKED(IDC_RADIO2, OnNoNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelB message handlers

BOOL CWizardPanelB::OnInitDialog() 
{	CWnd *pText;

	CPropertyPage::OnInitDialog();

	m_pParent->SetTitle(m_csTitle);
	pText = GetDlgItem(IDC_TITLESTATIC);
    AdjustHeaderFont ( pText, &m_Font );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWizardPanelB::OnWizardNext() 
{	CSubWizard *pParent;
    CString csNumber;

	pParent = (CSubWizard *) m_pParent;
	UpdateData();
	switch(m_iButton)
	{	case BUTTON_CONFIRM:
            csNumber = m_strConfirmation;
            csNumber.TrimLeft();
            csNumber.TrimRight();
			if(!SCShouldExtend(csNumber))
			{	CString csTemp, csTitle;
				csTitle.LoadString(IDS_WIZTITLE);
				csTemp.LoadString(IDS_BADCONFIRMATION);
				MessageBox(csTemp, csTitle, MB_ICONHAND | MB_OK);
				return -1;
			}
			m_pParent->SetActivePage(PANEL_FINALSUB);
			break;
		case BUTTON_NO:
		{	SUB_STATE state;

			state = pParent->GetState();
			switch(state)
			{	case SUB_FREEWARN:
				case SUB_PAIDWARN:
					m_pParent->SetActivePage(PANEL_FINALALL);
					break;
				default:
					m_pParent->SetActivePage(PANEL_FINALPATCH);
					break;
			}
			break;
		}
	}
	pParent->SetLastPanel(this);
	return -1;
//	return CPropertyPage::OnWizardNext();
}

LRESULT CWizardPanelB::OnWizardBack() 
{	CSubWizard *pParent;
	DWORD dwInetEnable = 0;

	pParent = (CSubWizard *) m_pParent;
	DefAnnuityGetInetEnable(&dwInetEnable);
	if(IsVeniceMode() && dwInetEnable)
		m_pParent->SetActivePage(PANEL_VENICE);
	else
		m_pParent->SetActivePage(PANEL_SUBINFO);
	return -1;
}

void CWizardPanelB::MySetWizardButtons()
{	UpdateData(TRUE);
	if(m_iButton == -1)
		m_pParent->SetWizardButtons(PSWIZB_BACK);
	else
		m_pParent->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
}

BOOL CWizardPanelB::OnSetActive() 
{
	MySetWizardButtons();
	return CPropertyPage::OnSetActive();
}

void CWizardPanelB::OnNumberIs() 
{
	MySetWizardButtons();
}

void CWizardPanelB::OnNoNumber() 
{
	MySetWizardButtons();
}
