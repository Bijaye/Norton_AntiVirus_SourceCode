////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WizardPg.h"
#include "ISVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString CBaseWizPage::m_csProductName;

/////////////////////////////////////////////////////////////////////////////
// CBaseWizPage property page

IMPLEMENT_DYNCREATE(CBaseWizPage, CPropertyPage)

CBaseWizPage::CBaseWizPage()
{
    //{{AFX_DATA_INIT(CBaseWizPage)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

    // Remove help button.
    m_psp.dwFlags &= ~PSP_HASHELP;

	if(m_csProductName.IsEmpty())
	{
		m_csProductName = CISVersion::GetProductName();
	}
} // CBaseWizPage::CBaseWizPage()

CBaseWizPage::CBaseWizPage( int iDlgID ) : CPropertyPage(iDlgID)
{
    // Remove help button.
    m_psp.dwFlags &= ~PSP_HASHELP;
    //CBaseWizPage();

	if(m_csProductName.IsEmpty())
	{
		m_csProductName = CISVersion::GetProductName();
	}
} // CBaseWizPage::CBaseWizPage()

CBaseWizPage::~CBaseWizPage()
{
} // CBaseWizPage::~CBaseWizPage()

/////////////////////////////////////////////////////////////////////////////

void CBaseWizPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CBaseWizPage)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
} // CBaseWizPage::DoDataExchange()


BEGIN_MESSAGE_MAP(CBaseWizPage, CPropertyPage)
    //{{AFX_MSG_MAP(CBaseWizPage)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseWizPage message handlers
