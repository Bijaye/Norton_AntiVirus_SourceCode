// WizardPanelBPre.cpp : implementation file
//

#include "stdafx.h"
//#include "resource.h"
#include "DefAnnty.h"
#include "SubWizard.h"
#include "SymWorld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_FIXUPWIZARDBUTTONS (WM_USER+800)

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelBPre property page

IMPLEMENT_DYNCREATE(CWizardPanelBPre, CPropertyPage)

CWizardPanelBPre::CWizardPanelBPre() : CPropertyPage(CWizardPanelBPre::IDD)
{
	//{{AFX_DATA_INIT(CWizardPanelBPre)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pParent = NULL;
	m_psp.dwFlags &= (~PSP_HASHELP);
}

CWizardPanelBPre::~CWizardPanelBPre()
{
}

void CWizardPanelBPre::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(WizardPanelBPre)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_INTERLINK, m_ddxInterLink);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWizardPanelBPre, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardPanelBPre)
	ON_BN_CLICKED(IDC_WORLDWIDE, OnWorldwide)
    ON_MESSAGE(WM_FIXUPWIZARDBUTTONS,OnFixUpWizardButtons)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWizardPanelBPre message handlers

BOOL CWizardPanelBPre::OnInitDialog() 
{	CWnd *pText;

	CPropertyPage::OnInitDialog();

	m_pParent->SetTitle(m_csTitle);
	pText = GetDlgItem(IDC_TITLESTATIC);
    AdjustHeaderFont ( pText, &m_Font );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWizardPanelBPre::OnWizardNext() 
{	CSubWizard *pParent;
	pParent = (CSubWizard *) m_pParent;
	UpdateData();
    m_pParent->SetActivePage(PANEL_SUB);
	return -1;
//	return CPropertyPage::OnWizardNext();
}

LRESULT CWizardPanelBPre::OnWizardBack() 
{	CSubWizard *pParent;
	DWORD dwInetEnable = 0;

	pParent = (CSubWizard *) m_pParent;
	DefAnnuityGetInetEnable(&dwInetEnable);
	if(IsVeniceMode() && dwInetEnable)
		m_pParent->SetActivePage(PANEL_VENICE);
	else
		pParent->SetState(pParent->GetState());
	return -1;
//	return CPropertyPage::OnWizardBack();
}

BOOL CWizardPanelBPre::OnSetActive() 
{
    GetDlgItem(IDC_WORLDWIDE)->ModifyStyle(WS_TABSTOP,0,0);

    PostMessage ( WM_FIXUPWIZARDBUTTONS, 0, 0);

	m_pParent->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	
	return CPropertyPage::OnSetActive();
}

void CWizardPanelBPre::OnWorldwide() 
{
	// TODO: Add your control notification handler code here

    auto CSymWorld *Contacts;

    if ( Contacts = new CSymWorld(this) )
    {
        Contacts->DoModal();
        delete Contacts;
    }
}

LONG CWizardPanelBPre::OnFixUpWizardButtons(UINT, LONG)
{
    GetDlgItem(IDC_WORLDWIDE)->ModifyStyle(0,WS_TABSTOP,0);

    return 0;
}

