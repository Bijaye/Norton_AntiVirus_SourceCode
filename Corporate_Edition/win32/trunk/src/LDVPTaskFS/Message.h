// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// Message.h : header file
//
#include "LockCtrl.h"

#define IDC_MESSAGE_LOCK		6010
#define IDC_MESSAGE_BEEP_LOCK	6011

/////////////////////////////////////////////////////////////////////////////
// CMessage dialog

class CMessage : public CDialog
{

private:
	CLockCtrl	m_MessageLock;
	CLockCtrl	m_MessageBeepLock;

// Construction
public:
	CMessage(CWnd* pParent = NULL);   // standard constructor

	BOOL	m_bMessageLocked;
	BOOL	m_bMessageBeepLocked;
	
// Dialog Data
	//{{AFX_DATA(CMessage)
	enum { IDD = IDD_MESSAGE };
	BOOL	m_bMessageBeep;
	CString	m_strMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMessage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
