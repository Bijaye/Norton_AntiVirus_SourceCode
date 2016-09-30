/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_PURGELOG_H__FC98D328_55BE_439C_809E_BB493AD2327E__INCLUDED_)
#define AFX_PURGELOG_H__FC98D328_55BE_439C_809E_BB493AD2327E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PurgeLog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPurgeLog dialog

class CPurgeLog : public CDialog
{
// Construction
public:
	CPurgeLog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPurgeLog)
	enum { IDD = IDD_DIALOG_PURGE_LOG };
	CRichEditCtrl	m_PurgeEditCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPurgeLog)
	public:
	virtual void OnSetFont(CFont* pFont);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPurgeLog)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonClear();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL OpenPurgeFile(void);


	HANDLE hPurgeFile;
	CString m_sPurgeFileName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PURGELOG_H__FC98D328_55BE_439C_809E_BB493AD2327E__INCLUDED_)
