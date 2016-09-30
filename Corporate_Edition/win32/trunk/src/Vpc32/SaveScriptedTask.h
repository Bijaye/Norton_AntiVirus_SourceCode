// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: SaveScriptedTask.h
//  Purpose: CSaveScriptedTask Definition file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------
#if !defined (SAVE_SCRIPTED_TASK_INCLUDED)
#define SAVE_SCRIPTED_TASK_INCLUDED

//----------------------------------------------------------------
//
// CSaveScriptedTask class
//
//----------------------------------------------------------------
class CSaveScriptedTask : public CDialog
{
// Construction
public:
	CSaveScriptedTask(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSaveScriptedTask)
	enum { IDD = IDD_SCRIPTED_TASK };
	CString	m_scriptedTaskName;
	CString	m_taskDescription;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveScriptedTask)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSaveScriptedTask)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
