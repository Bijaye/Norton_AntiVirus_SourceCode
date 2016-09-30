/////////////////////////////////////////////////////////////////////
// EmailPSh.h : header file
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:
//
// Description:
//
//  This file contains the property sheet for the SMTP email pages.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:
//
//
/////////////////////////////////////////////////////////////////////



#ifndef EMAIL_PROPERTY_SHEET_HEADER
#define EMAIL_PROPERTY_SHEET_HEADER

#include "NoCtxtPS.h"


#define SMTP_LIST_PAGE_INDEX        0
#define SMTP_SETTINGS_PAGE_INDEX    1



/////////////////////////////////////////////////////////////////////////////
// CEmailPropertySheet

class CEmailPropertySheet : public CNoContextPropertySheet
{
	DECLARE_DYNAMIC(CEmailPropertySheet)

// Construction
public:
	CEmailPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CEmailPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

private:
    BOOL    m_settingsDialogInitialized;

// Operations
public:
    void    SetSettingsDialogFlag(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEmailPropertySheet)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEmailPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEmailPropertySheet)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


    BOOL    MinimalSmtpFieldsDefined(void);
    BOOL    SmtpSenderDefined(void);
    BOOL    SmtpServerDefined(void);
};

/////////////////////////////////////////////////////////////////////////////


#endif  // EMAIL_PROPERTY_SHEET_HEADER