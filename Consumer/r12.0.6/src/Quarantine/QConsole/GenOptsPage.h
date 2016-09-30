//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// GenOptsPage.h : header file for general page of options dialog
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/genoptspage.h_v  $
// 
//    Rev 1.2   20 May 1998 17:11:18   DBuches
// 1st pass at help.
// 
//    Rev 1.1   08 May 1998 17:24:28   DBuches
// Added browse button.
// 
//    Rev 1.0   03 Apr 1998 13:30:08   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENOPTSPAGE_H__0B07EEA5_C8ED_11D1_9114_00C04FAC114A__INCLUDED_)
#define AFX_GENOPTSPAGE_H__0B07EEA5_C8ED_11D1_9114_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "iquaran.h"
#include "helpdlg.h"
#include "qconsoledoc.h"


/////////////////////////////////////////////////////////////////////////////
// CGenOptsPage dialog

class CGenOptsPage : public CHelperPage
{
	DECLARE_DYNCREATE(CGenOptsPage)

// Construction
public:
	CGenOptsPage();
	~CGenOptsPage();

// Help maps.
    //static DWORD m_dwWhatsThisMap[];
    //static DWORD m_dwHowToMap[];


// Dialog Data
	//{{AFX_DATA(CGenOptsPage)
	enum { IDD = IDD_OPTIONS_GENERAL_PAGE };
	BOOL m_bScanPrompt;
	BOOL m_bEnableIntro;
	//}}AFX_DATA

    // Pointer to options data
    IQuarantineOpts* m_pOpts;
	CQconsoleDoc* m_pDoc;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGenOptsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGenOptsPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENOPTSPAGE_H__0B07EEA5_C8ED_11D1_9114_00C04FAC114A__INCLUDED_)
