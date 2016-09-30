////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TWSaveEditedTaskPage.h : header file
//

#if !defined(AFX_TWSAVEEDITEDTASKPAGE_H__0669E6A1_B5D9_11D2_B9D0_00C04FAC114C__INCLUDED_)
#define AFX_TWSAVEEDITEDTASKPAGE_H__0669E6A1_B5D9_11D2_B9D0_00C04FAC114C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "picture.h"
#include "wizardpg.h"

/////////////////////////////////////////////////////////////////////////////
// CTWSaveEditedTaskPage dialog -- Last page in the scan task wizard when the
// user is editing an existing task.

class CTWSaveEditedTaskPage : public CBaseWizPage
{
	DECLARE_DYNCREATE(CTWSaveEditedTaskPage)

// Construction
public:
	CTWSaveEditedTaskPage();
	~CTWSaveEditedTaskPage();

// Dialog Data
	//{{AFX_DATA(CTWSaveEditedTaskPage)
	//enum { IDD = IDD_TW_SAVEEDITEDTASK };
	CPicture	m_Picture;
	//}}AFX_DATA

    CString    m_sTaskName;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTWSaveEditedTaskPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTWSaveEditedTaskPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TWSAVEEDITEDTASKPAGE_H__0669E6A1_B5D9_11D2_B9D0_00C04FAC114C__INCLUDED_)
