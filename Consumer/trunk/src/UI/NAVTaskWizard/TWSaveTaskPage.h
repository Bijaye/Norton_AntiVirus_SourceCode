////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TWSaveTaskPage.h : header file
//

#if !defined(AFX_TWSAVETASKPAGE_H__8AD9EAA6_5854_11D2_96F6_00C04FAC114C__INCLUDED_)
#define AFX_TWSAVETASKPAGE_H__8AD9EAA6_5854_11D2_96F6_00C04FAC114C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "picture.h"
#include "wizardpg.h"

/////////////////////////////////////////////////////////////////////////////
// CTWSaveTaskPage dialog

class CTWSaveTaskPage : public CBaseWizPage
{
	DECLARE_DYNCREATE(CTWSaveTaskPage)

// Construction
public:
	CTWSaveTaskPage();
	~CTWSaveTaskPage();

// Dialog Data
	//{{AFX_DATA(CTWSaveTaskPage)
	//enum { IDD = IDD_TW_SAVETASK };
	CPicture	m_Picture;
	CString	m_sTaskName;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTWSaveTaskPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual BOOL OnKillActive();
	LRESULT OnResetFocus(WPARAM wParam, LPARAM lParam);

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTWSaveTaskPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TWSAVETASKPAGE_H__8AD9EAA6_5854_11D2_96F6_00C04FAC114C__INCLUDED_)
