/////////////////////////////////////////////////////////////////////
// ATSmtpPg.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATSMTPPG.H_v   1.1   25 Apr 1997 11:04:52   JBRENNA  $
//
// Description:
//      This file contains the class definitions for both the SMTP alert target
//      property page and its corresponding "settings" property page.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATSMTPPG.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:04:52   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:08:58   sedward
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////


#ifndef ALERT_SMTP_PROPERTY_PAGE_HEADER
#define ALERT_SMTP_PROPERTY_PAGE_HEADER


#include "ATMailPg.h"

// define statements
#define NUM_SMTP_SETTINGS_GROUPBOX_CONTROLS     5


/////////////////////////////////////////////////////////////////////////////
//
// CAlertSmtpPropertyPage
//
/////////////////////////////////////////////////////////////////////////////

class CAlertSmtpPropertyPage : public CAlertEmailPropertyPage
{
    DECLARE_DYNCREATE(CAlertSmtpPropertyPage)

// Construction
public:
    CAlertSmtpPropertyPage();
    CAlertSmtpPropertyPage(UINT  dlgID);
    ~CAlertSmtpPropertyPage();

// Dialog Data
    //{{AFX_DATA(CAlertSmtpPropertyPage)
	enum { IDD = IDD_SMTP_PROP_PAGE };
	CListCtrl	m_listCtrl;
	CButton	m_removeButton;
	CButton	m_modifyButton;
	CButton	m_addButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CAlertSmtpPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
    //{{AFX_MSG(CAlertSmtpPropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickSmtpTargetList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkSmtpTargetList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Functions
protected:
    BOOL                    InitializeData(void);

public:
    virtual BOOL            CopyAlertTargetInfo(CAlertTarget*  source
                                                        , CAlertTarget*  dest);
    virtual BOOL            GetDisplayString(CAlertTarget*  pAlertTarget
                                            , CString&  displayStr, int  index);
    virtual CAlertTarget*   GetNewAlertTarget(void);
    virtual BOOL            TargetIsActive(CAlertTarget*  pAlertTarget);

// Data members
public:
};







/////////////////////////////////////////////////////////////////////////////
// CSmtpOptPpg dialog
/////////////////////////////////////////////////////////////////////////////

class CSmtpOptPpg : public CHelpPropertyPage
{
	DECLARE_DYNCREATE(CSmtpOptPpg)

// Construction
public:
	CSmtpOptPpg();
	~CSmtpOptPpg();

// Dialog Data
	//{{AFX_DATA(CSmtpOptPpg)
	enum { IDD = IDD_SMTP_MAIL_SETTINGS };
	CStatic	m_staticMessageFrom;
	CStatic	m_staticMessageSubject;
	CStatic	m_staticServerName;
	CString	m_szSmtpServerName;
	CString	m_szSmtpAuthorName;
	CString	m_szSmtpSubjectText;
	//}}AFX_DATA

    static DWORD    m_dwWhatsThisMap[];
    static DWORD    m_dwHowToMap[];


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSmtpOptPpg)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSmtpOptPpg)
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    CWnd*           m_groupboxControls[NUM_SMTP_SETTINGS_GROUPBOX_CONTROLS + 1];
};



#endif  // ALERT_SMTP_PROPERTY_PAGE_HEADER