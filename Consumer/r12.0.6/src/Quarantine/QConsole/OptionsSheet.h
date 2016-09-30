//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// OptionsSheet.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/optionssheet.h_v  $
// 
//    Rev 1.1   20 May 1998 17:15:14   DBuches
// 1st pass at help
// 
//    Rev 1.0   03 Apr 1998 13:30:12   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_OPTIONSSHEET_H__0B07EEA4_C8ED_11D1_9114_00C04FAC114A__INCLUDED_)
#define AFX_OPTIONSSHEET_H__0B07EEA4_C8ED_11D1_9114_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "genoptspage.h"


/////////////////////////////////////////////////////////////////////////////
// COptionsSheet

class COptionsSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COptionsSheet)

// Construction
public:
	COptionsSheet(UINT nIDCaption, IQuarantineOpts* pOpts, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptionsSheet(LPCTSTR pszCaption, IQuarantineOpts* pOpts, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COptionsSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionsSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COptionsSheet();

public:
    // Pointer to options data
    IQuarantineOpts *m_pOpts;

	// Generated message map functions
protected:
	//{{AFX_MSG(COptionsSheet)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void AddPages();


private:
    CGenOptsPage m_GenPage;
    //CFileTypePage m_QuarantinePage;
    //CFileTypePage m_BackupPage;
    //CFileTypePage m_SubmittedPage;
    //CFileTypePage m_ThreatBackup;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSSHEET_H__0B07EEA4_C8ED_11D1_9114_00C04FAC114A__INCLUDED_)
