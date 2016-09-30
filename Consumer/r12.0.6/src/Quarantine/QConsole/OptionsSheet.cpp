//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// OptionsSheet.cpp : implementation file
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
// 3/31/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
COptionsSheet::COptionsSheet(UINT nIDCaption, IQuarantineOpts* pOpts, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
    m_pOpts = pOpts;
    m_psh.dwFlags |= PSH_NOAPPLYNOW;    
	AddPages();
}


COptionsSheet::COptionsSheet(LPCTSTR pszCaption, IQuarantineOpts* pOpts, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    m_pOpts = pOpts;
    m_psh.dwFlags |= PSH_NOAPPLYNOW;    
	AddPages();
}


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
// Description	    : Adds pages to this property sheet.
// 3/31/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
void COptionsSheet::AddPages()
{
    // Add general page
    AddPage( &m_GenPage );
    m_GenPage.m_pOpts = m_pOpts;
	m_GenPage.m_pDoc = (CQconsoleDoc*)((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();

    //m_ThreatBackup.Initialize( m_pOpts, FILE_TYPE_THREAT_BACKUP );
    //AddPage( &m_ThreatBackup );

    // Add file type pages
    //m_QuarantinePage.Initialize( m_pOpts, FILE_TYPE_QUARANTINE );
    //AddPage( &m_QuarantinePage );
    //    
    //m_BackupPage.Initialize( m_pOpts, FILE_TYPE_BACKUP );
    //AddPage( &m_BackupPage );

    //m_SubmittedPage.Initialize( m_pOpts, FILE_TYPE_SUBMITTED );
    //AddPage( &m_SubmittedPage );

    // bugbug - Add server config page
}


////////////////////////////////////////////////////////////////////////////
// Description	    : Handles WM_INITDIALOG
// 3/31/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL COptionsSheet::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
