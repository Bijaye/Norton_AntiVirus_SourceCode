/////////////////////////////////////////////////////////////////////
// AlertMes.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ALERTMES.H_v   1.1   25 Apr 1997 11:03:44   JBRENNA  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ALERTMES.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:03:44   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:24   RFULLER
// Initial revision
// 
//    Rev 1.4   13 Jan 1997 20:41:36   SEDWARD
// Added support for help.
//
//    Rev 1.3   03 Dec 1996 19:50:56   SEDWARD
// Substantial changes (not frozen yet...)
//
//    Rev 1.2   27 Nov 1996 18:48:20   SEDWARD
// Hooked up NAVOPTS to local alert message configuration.
//
//    Rev 1.1   25 Nov 1996 15:45:36   SEDWARD
// No change.
//
//    Rev 1.0   21 Nov 1996 17:26:02   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#ifndef ALERT_MESSAGE_DLG_HEADER
#define ALERT_MESSAGE_DLG_HEADER

#include "helpdlg.h"                // base class for default help support


/////////////////////////////////////////////////////////////////////////////
// CAlertMessageDlg dialog
/////////////////////////////////////////////////////////////////////////////

class CAlertMessageDlg : public CHelpDialog
{
// Construction
public:
	CAlertMessageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAlertMessageDlg)
	enum { IDD = IDD_ALERT_MESSAGE };
	CButton	m_displayMessage;
	CButton	m_alertSound;
	CEdit	m_alertMessage;
	//}}AFX_DATA

    static DWORD    m_dwWhatsThisMap[];
    static DWORD    m_dwWhatsThisGrayMap[];
    static DWORD    m_dwHowToMap[];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlertMessageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAlertMessageDlg)
    virtual BOOL OnInitDialog(void);
    virtual void OnOK(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif  // ALERT_MESSAGE_DLG_HEADER
