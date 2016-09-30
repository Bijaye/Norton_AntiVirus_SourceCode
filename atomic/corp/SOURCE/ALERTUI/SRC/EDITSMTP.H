/////////////////////////////////////////////////////////////////////
// EditSMTP.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/EDITSMTP.H_v   1.1   25 Apr 1997 11:03:42   JBRENNA  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/EDITSMTP.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:03:42   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:22   RFULLER
// Initial revision
// 
//    Rev 1.5   15 Jan 1997 13:58:18   SEDWARD
// Added 'UpdateOKButton()'.
//
//    Rev 1.3   13 Jan 1997 20:41:50   SEDWARD
// Added support for help.
//
//    Rev 1.2   03 Dec 1996 19:50:40   SEDWARD
// No change.
//
//    Rev 1.1   25 Nov 1996 15:45:34   SEDWARD
// No change.
//
//    Rev 1.0   21 Nov 1996 17:26:00   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#ifndef EDIT_SMTP_HEADER
#define EDIT_SMTP_HEADER

#include "helpdlg.h"                // base class for default help support



/////////////////////////////////////////////////////////////////////////////
// CEmailEditSMTP dialog
/////////////////////////////////////////////////////////////////////////////

class CEmailEditSMTP : public CHelpDialog
{
// Construction
public:
    CEmailEditSMTP(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CEmailEditSMTP)
	enum { IDD = IDD_EMAIL_SMTP_EDIT };
	CButton	m_OKButton;
	CEdit	m_internetName;
	CEdit	m_internetAddress;
	CString	m_internetAddressStr;
	CString	m_internetNameStr;
	//}}AFX_DATA

    static DWORD    m_dwWhatsThisMap[];
    static DWORD    m_dwWhatsThisGrayMap[];
    static DWORD    m_dwHowToMap[];


// Overrides
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CEmailEditSMTP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
    //{{AFX_MSG(CEmailEditSMTP)
    afx_msg void OnChangeEmailName(void);
    virtual BOOL OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnWhatsThis(void);
    afx_msg void OnChangeEmailAddress(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    void    UpdateOKButton(void);
};

#endif  // EDIT_SMTP_HEADER
