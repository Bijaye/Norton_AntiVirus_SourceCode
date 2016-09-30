F//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// GenOptsPage.cpp : implementation file for general page of options dialog
//
//////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/GenOptsPage.cpv  $
// 
//    Rev 1.5   09 Jun 1998 23:10:22   DBuches
// Fixed problem with enabling browser button.
// 
//    Rev 1.4   21 May 1998 11:29:06   DBuches
// Added help topic IDs.
// 
//    Rev 1.3   20 May 1998 17:11:18   DBuches
// 1st pass at help.
// 
//    Rev 1.2   19 May 1998 13:40:58   DBuches
// Fixed problem with returning from browse dialog.  Added Call to UpdateData().
// 
//    Rev 1.1   08 May 1998 17:24:26   DBuches
// Added browse button.
// 
//    Rev 1.0   03 Apr 1998 13:30:06   DBuches
// Initial revision.
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "GenOptsPage.h"
#include "helpdlg.h"
#include "qconhlp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////////////
// CGenOptsPage property page

IMPLEMENT_DYNCREATE(CGenOptsPage, CHelperPage)

CGenOptsPage::CGenOptsPage() : CHelperPage(CGenOptsPage::IDD)
{
	//{{AFX_DATA_INIT(CGenOptsPage)
	m_bScanPrompt = FALSE;
	m_bEnableIntro = FALSE;
	//}}AFX_DATA_INIT

	m_dwHelpButtonHelpID = IDH_QUARAN_OPTIONS_GENERAL_HLPBTN;

	m_psp.dwFlags |= PSP_HASHELP;
}

CGenOptsPage::~CGenOptsPage()
{
}

void CGenOptsPage::DoDataExchange(CDataExchange* pDX)
{
	CHelperPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenOptsPage)
	DDX_Check(pDX, IDC_SCAN_PROMPT_CHECK, m_bScanPrompt);
	DDX_Check(pDX, IDC_INTRO_CHECK, m_bEnableIntro);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenOptsPage, CHelperPage)
	//{{AFX_MSG_MAP(CGenOptsPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////////
// CGenOptsPage message handlers

//////////////////////////////////////////////////////////////////////////////
// Handles WM_INITDIALOG message
// 3/31/98 DBUCHES - Function created / Header added.
//////////////////////////////////////////////////////////////////////////////
BOOL CGenOptsPage::OnInitDialog() 
{
	CHelperPage::OnInitDialog();

	// Move data to controls
	m_pOpts->GetScanAtStartup(&m_bScanPrompt);

	m_bEnableIntro = m_pDoc->GetIntroEnabled();
	
	if(m_pDoc->GetExpandedThreatShowSetting())
	{
		GetDlgItem(IDC_INTRO_CHECK)->EnableWindow(TRUE);
		GetDlgItem(IDC_INTRO_CHECK)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_INTRO_GROUP)->ShowWindow(SW_SHOW);
	}

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


//////////////////////////////////////////////////////////////////////////////
// Handles OK button click
// 4/2/98 - DBUCHES function created / header added
//////////////////////////////////////////////////////////////////////////////
void CGenOptsPage::OnOK() 
{
	// Get data from controls
	UpdateData();

	m_pOpts->SetScanAtStartup(m_bScanPrompt);

	bool bEnableIntro = (FALSE == m_bEnableIntro) ? false : true;
	m_pDoc->SetIntroEnabled(bEnableIntro);

	CHelperPage::OnOK();
}
