/////////////////////////////////////////////////////////////////////
// AddAlert.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ADDALERT.H_v   1.1   25 Apr 1997 11:03:42   JBRENNA  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ADDALERT.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:03:42   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:22   RFULLER
// Initial revision
// 
//    Rev 1.1   25 Nov 1996 15:45:32   SEDWARD
// No change.
// 
//    Rev 1.0   21 Nov 1996 17:25:58   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#ifndef ADD_ALERT_DLG_HEADER
#define ADD_ALERT_DLG_HEADER

/////////////////////////////////////////////////////////////////////////////
// CAlertTypesDlg dialog

class CAlertTypesDlg : public CDialog
{
// Construction
public:
    CAlertTypesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CAlertTypesDlg)
	enum { IDD = IDD_ADD_ALERT_TARGET };
	CListBox	m_alertTargetsListbox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAlertTypesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
    // data members
    DWORD*      m_alertTypePtr;

    // functions
    void        AlertSelected(void);


// Implementation
protected:

	// Generated message map functions
    //{{AFX_MSG(CAlertTypesDlg)
    virtual BOOL OnInitDialog(void);
    virtual void OnOK(void);
    afx_msg void OnDblclkAlertTargetListbox(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif  // ADD_ALERT_DLG_HEADER
