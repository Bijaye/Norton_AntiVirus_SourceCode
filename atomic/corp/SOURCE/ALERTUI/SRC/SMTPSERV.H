/////////////////////////////////////////////////////////////////////
// SMTPServ.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/SMTPSERV.H_v   1.1   25 Apr 1997 11:03:40   JBRENNA  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/SMTPSERV.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:03:40   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:22   RFULLER
// Initial revision
// 
//    Rev 1.1   25 Nov 1996 15:45:44   SEDWARD
// Major overhaul (more to come).
// 
//    Rev 1.0   21 Nov 1996 17:25:56   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#ifndef SMTP_SERV_HEADER
#define SMTP_SERV_HEADER


/////////////////////////////////////////////////////////////////////////////
// CSMTPServerDlg dialog

class CSMTPServerDlg : public CDialog
{
// Construction
public:
	CSMTPServerDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSMTPServerDlg)
	enum { IDD = IDD_SMTP_SERVER_CONFIG };
	CString	m_smtpServerStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSMTPServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSMTPServerDlg)
	virtual BOOL OnInitDialog(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif  // SMTP_SERV_HEADER
