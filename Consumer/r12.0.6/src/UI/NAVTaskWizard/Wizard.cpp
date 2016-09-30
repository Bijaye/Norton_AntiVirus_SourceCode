// Wizard.cpp : implementation file
//

#include "stdafx.h"

#include "ScanTask.h"
#include "TWIntroPage.h"
#include "TWSelectItemsPage.h"
#include "TWSaveTaskPage.h"
#include "TWSaveEditedTaskPage.h"
#include "Wizard.h"
#include "Resource.h"
#include "AVRESBranding.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskWizard

IMPLEMENT_DYNAMIC(CTaskWizard, CPropertySheet)

CTaskWizard::CTaskWizard(IScanTask* pScanTask,
                         int iMode,
                         UINT nIDCaption,
                         CWnd* pParentWnd,
                         UINT iSelectPage) 
           : m_pScanTask(pScanTask)
           , CPropertySheet(IDS_NAV, pParentWnd, iSelectPage)
{
    m_iMode = iMode;


	// Format caption text with product name
	CBrandingRes BrandRes;
	m_csTitle.Format(IDS_TITLE, BrandRes.ProductName());

    // Set up the wizard pages to be used.  The pages are different when
    // creating a new task vs. when editing an existing task vs. a duplicate
    // named task.

	if ( m_iMode == modeCreate)
    {
		m_IntroPg.m_psp.dwFlags |= PSP_USETITLE;
		m_IntroPg.m_psp.pszTitle = m_csTitle;
        AddPage(&m_IntroPg);

		m_SelectPg.m_psp.dwFlags |= PSP_USETITLE;
		m_SelectPg.m_psp.pszTitle = m_csTitle;
        AddPage(&m_SelectPg);
    }

    if ( m_iMode == modeEdit )
	{
		m_SelectPg.m_psp.dwFlags |= PSP_USETITLE;
		m_SelectPg.m_psp.pszTitle = m_csTitle;
    	AddPage(&m_SelectPg);
	}

	m_SavePg.m_psp.dwFlags |= PSP_USETITLE;
	m_SavePg.m_psp.pszTitle = m_csTitle;
	AddPage(&m_SavePg);
}

CTaskWizard::~CTaskWizard()
{
}


BEGIN_MESSAGE_MAP(CTaskWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CTaskWizard)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskWizard message handlers

int CTaskWizard::DoModal() 
{
	m_psh.dwFlags &= ~PSH_HASHELP;
    m_psh.dwFlags |= PSH_WIZARD;

	return CPropertySheet::DoModal();
}

IScanTask* CTaskWizard::GetScanTask(void)
{
	return m_pScanTask;
}

bool CTaskWizard::IsEditing()
{
    return ( m_iMode == modeEdit );
}
