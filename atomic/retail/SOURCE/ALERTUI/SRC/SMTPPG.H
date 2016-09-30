/////////////////////////////////////////////////////////////////////
// SMTPPg.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/SMTPPG.H_v   1.1   25 Apr 1997 11:03:54   JBRENNA  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/SMTPPG.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:03:54   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:26   RFULLER
// Initial revision
// 
//    Rev 1.10   02 Feb 1997 16:36:54   SEDWARD
// Added support for context menus that are grayed out AND in groupbox controls.
//
//    Rev 1.9   19 Jan 1997 23:50:02   SEDWARD
// Added OnCommand prototype.
//
//    Rev 1.8   13 Jan 1997 20:41:58   SEDWARD
// Added support for help.
//
//    Rev 1.7   07 Jan 1997 18:20:04   TSmith
// Added class 'CSmtpOptPpg' for the SMTP mail settings property page.
//
//    Rev 1.6   06 Jan 1997 00:42:08   SEDWARD
// Now uses temporary CAlertOptions list for edits.
//
//    Rev 1.4   03 Dec 1996 19:50:46   SEDWARD
// Substantial changes (not frozen yet...)
//
//    Rev 1.3   02 Dec 1996 16:33:52   SEDWARD
// Big changes, nearing UI freeze.
//
//    Rev 1.2   26 Nov 1996 00:54:48   SEDWARD
// More sweeping changes.
//
//    Rev 1.1   25 Nov 1996 15:45:42   SEDWARD
// Major overhaul (more to come).
//
//    Rev 1.0   21 Nov 1996 17:26:12   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#ifndef SMTP_PAGE_HEADER
#define SMTP_PAGE_HEADER

#include "netbrwsd.h"                   // MFC net browser
#include "alrtres.h"                    // Resource symbols


// define statements
#define NUM_SMTP_SETTINGS_GROUPBOX_CONTROLS     5



/////////////////////////////////////////////////////////////////////////////
// CEmailSMTPPg dialog
/////////////////////////////////////////////////////////////////////////////

class CEmailSMTPPg : public CHelpPropertyPage
{
    DECLARE_DYNCREATE(CEmailSMTPPg)

// Construction
public:
    CEmailSMTPPg();
    ~CEmailSMTPPg();

// Dialog Data
    //{{AFX_DATA(CEmailSMTPPg)
	enum { IDD = IDD_EMAIL_PROP_PAGE };
	CButton	m_emailModifyButton;
	CButton	m_emailRemoveButton;
	CButton	m_emailAddButton;
	CListCtrl	m_emailList;
	//}}AFX_DATA

    static DWORD    m_dwWhatsThisMap[];
    static DWORD    m_dwHowToMap[];


// Overrides
	// ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CEmailSMTPPg)
	public:
    virtual void OnOK(void);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
    //{{AFX_MSG(CEmailSMTPPg)
    afx_msg void OnClickEmailList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEmailAddButton(void);
    afx_msg void OnEmailRemoveButton(void);
    virtual BOOL OnInitDialog(void);
    afx_msg void OnEmailModifyButton(void);
    afx_msg void OnHandleSMTPAdd(void);
    afx_msg void OnHandleSMTPModify(void);
    afx_msg void OnHandleSMTPRemove(void);
	afx_msg void OnDblclkEmailListview(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnWhatsThisSMTPListCtrl(void);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



// data members
    CAlertOptions*  m_targetsListPtr;


// data members
public:
    CImageList      m_imageList;
    int             m_clickedItemIndex;

// functions
    void            AddListboxItem(CEMailTarget*  emailRecPtr);
    BOOL            CreateTempAlertTargetsList(void);
    CEMailTarget*   GetListItemDataPtr(int  nIndex);
    void            HandleRclickEmailList(void);
    int             InitListbox(void);
    BOOL            UpdateListboxItem(int  itemIndex, CEMailTarget*  recPtr);
    void            UpdateMasterAlertTargetsList(void);
};



/////////////////////////////////////////////////////////////////////////////
// CSmtpOptPpg dialog

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

#endif  // SMTP_PAGE_HEADER