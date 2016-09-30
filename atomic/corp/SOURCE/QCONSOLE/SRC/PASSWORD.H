/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/password.h_v   1.1   20 May 1998 17:15:14   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// password.h : header file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/password.h_v  $
// 
//    Rev 1.1   20 May 1998 17:15:14   DBuches
// 1st pass at help
// 
//    Rev 1.0   03 Apr 1998 13:30:14   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PASSWORD_H__F19020B4_CA64_11D1_A57B_0000C06F46D0__INCLUDED_)
#define AFX_PASSWORD_H__F19020B4_CA64_11D1_A57B_0000C06F46D0__INCLUDED_

#include "helpdlg.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CPasswordChangeDlg dialog

class CPasswordChangeDlg : public CHelperDialog
{
// Construction
public:
	CPasswordChangeDlg(CWnd* pParent = NULL);   // standard constructor

// Help maps.
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];


// Dialog Data
	//{{AFX_DATA(CPasswordChangeDlg)
	enum { IDD = IDD_CHANGE_PW_DIALOG };
	CEdit	m_CtrlNew;
	CString	m_sConfirm;
	CString	m_sNew;
	CString	m_sOld;
	//}}AFX_DATA

public:
    CString m_sPassword;
    
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordChangeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK( );
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasswordChangeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeNewPwEdit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void DisplayError();
    CString m_sPasswordText;
};

/////////////////////////////////////////////////////////////////////////////
// CPasswordCheckDlg dialog

class CPasswordCheckDlg : public CDialog
{
// Construction
public:
	CPasswordCheckDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPasswordCheckDlg)
	enum { IDD = IDD_PASSWORD_CHECK_DIALOG };
	CString	m_sPasswordText;
	//}}AFX_DATA

public:
    CString m_sPassword;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordCheckDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasswordCheckDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORD_H__F19020B4_CA64_11D1_A57B_0000C06F46D0__INCLUDED_)
