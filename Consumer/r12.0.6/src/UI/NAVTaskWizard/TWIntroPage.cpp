// TWIntroPage.cpp : implementation file
//

#include "stdafx.h"
#include "TWIntroPage.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTWIntroPage property page

IMPLEMENT_DYNCREATE(CTWIntroPage, CBaseWizPage)

CTWIntroPage::CTWIntroPage() : CBaseWizPage(IDD_TW_INTRO)
//CTWIntroPage::CTWIntroPage() : CBaseWizPage(CTWIntroPage::IDD)
{
	//{{AFX_DATA_INIT(CTWIntroPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    m_bEditingTask = FALSE;
}

CTWIntroPage::~CTWIntroPage()
{
}

void CTWIntroPage::DoDataExchange(CDataExchange* pDX)
{
	CBaseWizPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTWIntroPage)
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTWIntroPage, CBaseWizPage)
	//{{AFX_MSG_MAP(CTWIntroPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTWIntroPage message handlers

BOOL CTWIntroPage::OnInitDialog() 
{
	CBaseWizPage::OnInitDialog();

    m_Picture.SetBitmap ( g_bUseHiColorBmps ? IDB_WIZ256_0 : IDB_WIZ16_0,
                          111, 236 );	

	// Format body text with product name
	CString csText;
	CString csFormat;
	GetDlgItemText(IDC_STATIC_TW_INTRO, csFormat);
	csText.Format(csFormat, m_csProductName);
	SetDlgItemText(IDC_STATIC_TW_INTRO, csText);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTWIntroPage::OnSetActive() 
{
    // This page shouldn't even be in the wizard if the user invoked
    // it to edit an existing task.
    ASSERT ( !m_bEditingTask );

    GetParentSheet()->SetWizardButtons ( PSWIZB_NEXT );

	return CBaseWizPage::OnSetActive();
}
