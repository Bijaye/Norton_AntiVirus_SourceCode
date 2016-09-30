// Copyright 1997, Symantec Corporation, Peter Norton Product Group
//********************************************************************
// $Header:   S:/PRIVMAN/VCS/STATDLG.H_v   1.0   25 Jun 1997 16:07:18   DSACKIN  $
//
// Description:      Status Dialog
//
//********************************************************************
// $Log:   S:/PRIVMAN/VCS/STATDLG.H_v  $
// 
//    Rev 1.0   25 Jun 1997 16:07:18   DSACKIN
// Initial revision.
// 
//    Rev 1.0   08 May 1997 10:13:10   pvenkat
// Initial revision.
//********************************************************************
//

// StatDlg.h : header file
//

#ifndef		__STATDLG_H__
#define		__STATDLG_H__

/////////////////////////////////////////////////////////////////////////////
// CStatusDlg dialog

class CStatusDlg : public CDialog
{
// Construction
public:
	VOID SetMsg (LPTSTR lpszOp, LPTSTR lpszUser, LPTSTR lpszMachine);
	CStatusDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStatusDlg)
	enum { IDD = IDD_STATUS };
	CString	m_strMsg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStatusDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
