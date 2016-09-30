// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved.
//*************************************************************************
//
// $Header:   S:/NAVWORK/VCS/rwizard.cpv   1.1   03 Apr 1998 11:19:54   tcashin  $
//
// Description:
//      Implementation of CRepairWizard. Wizard for dealing with problems
//      found during scanning.
//
// Contains:
//
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/rwizard.cpv  $
// 
//    Rev 1.1   03 Apr 1998 11:19:54   tcashin
// Added the Quarantine panel.
// 
//    Rev 1.0   09 Mar 1998 23:45:12   DALLEE
// Initial revision.
//*************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "nunavstb.h"
#include "RWizard.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRepairWizard

IMPLEMENT_DYNAMIC(CRepairWizard, CPropertySheet)


//*************************************************************************
// CRepairWizard::CRepairWizard()
// CRepairWizard::~CRepairWizard()
//
// CRepairWizard::CRepairWizard(
//      CWnd *pWndParent )              [in]
//
// Returns:
//      Nothing
//*************************************************************************
// 2/12/98 DBUCHES - Function created / Header added.
//*************************************************************************

CRepairWizard::CRepairWizard(CWnd* pWndParent)
     : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
    // Set defaults
    m_bManual = FALSE;

    // Remove help button.
    m_psh.dwFlags &= ~PSH_HASHELP;

    // Add pages here.
    AddPage(&m_pageIntro);
    AddPage(&m_pageRepair);
    AddPage(&m_pageQuarantine);
    AddPage(&m_pageDelete);
    AddPage(&m_pageStillInfected);
    AddPage(&m_pageFinishInfected);
    AddPage(&m_pageFinishClean);

    // Set wizard mode
    SetWizardMode();
} // CRepairWizard::CRepairWizard()


CRepairWizard::~CRepairWizard()
{
} // CRepairWizard::~CRepairWizard()


//*************************************************************************
// CRepairWizard::SetNoNav()
//
// void CRepairWizard::SetNoNav(
//      LPNONAV lpNoNav )               [in]
//
// Stores pointer to options and information on the current scan.
// Pointer must remain valid for duration of repair wizard.
// This function must be called before displaying the Repair Wizard.
//
// Returns:
//      Nothing
//*************************************************************************
// 02/18/1998 DALLEE - Function created / Header added.
//*************************************************************************

void CRepairWizard::SetNoNav(LPVOID lpNoNav)
{
    auto    CPropertyPage   *pPage;
    auto    int             i;

    // Save NONAV, also save in each page.

    m_lpNoNav = lpNoNav;

    for ( i = 0; i < GetPageCount(); i++ )
    {
        pPage = GetPage( i );

        if ( pPage->IsKindOf( RUNTIME_CLASS( CRepWizPage ) ) )
        {
            (( CRepWizPage * )pPage)->SetNoNav( lpNoNav );
        }
#ifdef SYM_DEBUG
        else
        {
            SYM_ASSERT( FALSE );
        }
#endif
    }
} // CRepairWizard::SetNoNav()


//*************************************************************************
// CRepairWizard::GetNoNav()
//
// LPNONAV CRepairWizard::GetNoNav( )
//
// Retrieves stored pointer to options and information on the current scan.
//
// Returns:
//      LPNONAV
//*************************************************************************
// 02/18/1998 DALLEE - Function created / Header added.
//*************************************************************************

LPVOID CRepairWizard::GetNoNav()
{
    return ( m_lpNoNav );
} // CRepairWizard::GetNoNav()


BEGIN_MESSAGE_MAP(CRepairWizard, CPropertySheet)
    //{{AFX_MSG_MAP(CRepairWizard)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



