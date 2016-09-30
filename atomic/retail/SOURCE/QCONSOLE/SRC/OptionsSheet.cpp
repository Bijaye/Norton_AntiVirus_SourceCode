/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/optionssheet.cpv   1.2   21 May 1998 11:28:24   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// OptionsSheet.cpp : implementation file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/optionssheet.cpv  $
// 
//    Rev 1.2   21 May 1998 11:28:24   DBuches
// Added help topics.
// 
//    Rev 1.1   20 May 1998 17:15:14   DBuches
// 1st pass at help
// 
//    Rev 1.0   03 Apr 1998 13:30:12   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "OptionsSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsSheet

IMPLEMENT_DYNAMIC(COptionsSheet, CPropertySheet)


////////////////////////////////////////////////////////////////////////////
// Function name	: COptionsSheet::COptionsSheet
//
// Description	    : Constructor
//
// Argument         : UINT nIDCaption
// Argument         : CWnd* pParentWnd
// Argument         : UINT iSelectPage
//
////////////////////////////////////////////////////////////////////////////
// 3/31/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
COptionsSheet::COptionsSheet(UINT nIDCaption, IQuarantineOpts* pOpts, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
    m_pOpts = pOpts;
    m_psh.dwFlags |= PSH_NOAPPLYNOW;    
    AddPages();
}


////////////////////////////////////////////////////////////////////////////
// Function name	: COptionsSheet::COptionsSheet
//
// Description	    : Constructor
//
// Argument         : LPCTSTR pszCaption
// Argument         : CWnd* pParentWnd
// Argument         : UINT iSelectPage
//
////////////////////////////////////////////////////////////////////////////
// 3/31/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
COptionsSheet::COptionsSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    m_psh.dwFlags |= PSH_NOAPPLYNOW;
    AddPages();
}


////////////////////////////////////////////////////////////////////////////
// Function name	: COptionsSheet::~COptionsSheet
//
// Description	    : Destructor
//
////////////////////////////////////////////////////////////////////////////
// 3/31/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
COptionsSheet::~COptionsSheet()
{
}


BEGIN_MESSAGE_MAP(COptionsSheet, CPropertySheet)
	//{{AFX_MSG_MAP(COptionsSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsSheet message handlers


////////////////////////////////////////////////////////////////////////////
// Function name	: COptionsSheet::AddPages
//
// Description	    : Adds pages to this property sheet.
//
// Return type		: void 
//
//
////////////////////////////////////////////////////////////////////////////
// 3/31/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void COptionsSheet::AddPages()
{
    // Add general page
    AddPage( &m_GenPage );
    m_GenPage.m_pOpts = m_pOpts;

    // Add file type pages
    m_QuarantinePage.Initialize( m_pOpts, FILE_TYPE_QUARANTINE );
    AddPage( &m_QuarantinePage );
        
    m_BackupPage.Initialize( m_pOpts, FILE_TYPE_BACKUP );
    AddPage( &m_BackupPage );

    m_SubmittedPage.Initialize( m_pOpts, FILE_TYPE_SUBMITTED );
    AddPage( &m_SubmittedPage );

    // bugbug - Add server config page
}


////////////////////////////////////////////////////////////////////////////
// Function name	: COptionsSheet::OnInitDialog
//
// Description	    : Handles WM_INITDIALOG
//
// Return type		: BOOL 
//
//
////////////////////////////////////////////////////////////////////////////
// 3/31/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL COptionsSheet::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();
	
	// Add context help button
    ModifyStyleEx( 0, WS_EX_CONTEXTHELP );
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
