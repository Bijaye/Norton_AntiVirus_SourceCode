// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_SCANWIZ_H__8D28F8F9_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_)
#define AFX_SCANWIZ_H__8D28F8F9_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ScanWiz.h : header file
//


// this value for the cancel button ID was fetched via Spy++
#define ID_WIZCANCEL    0x00000002



/////////////////////////////////////////////////////////////////////////////
// CScanDeliverWizard

class CScanDeliverWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CScanDeliverWizard)

// Construction
public:
	CScanDeliverWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CScanDeliverWizard(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:
    int     GetIndexFromResID(UINT  resID);
    BOOL    GoToWrapUpPage(DWORD  dwWrapUpType);
    BOOL    SetWrapUpPageText(DWORD  dwWrapUpType);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScanDeliverWizard)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CScanDeliverWizard();

	// Generated message map functions
protected:
	//{{AFX_MSG(CScanDeliverWizard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    BOOL OnCommand( WPARAM wParam, LPARAM lParam );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCANWIZ_H__8D28F8F9_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_)