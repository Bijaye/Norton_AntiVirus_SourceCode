/////////////////////////////////////////////////////////////////////////////////
// $Header:
//
// Description: Header for HelpDlg.cpp - defines the CHelpDialog class
//
/////////////////////////////////////////////////////////////////////////////////
// $Log:


// HLPropPg.h : header file
//
#ifndef _CHelpDialog_CLASS
#define _CHelpDialog_CLASS
#include "alrtres.h"


/////////////////////////////////////////////////////////////////////////////
// CHelpDialog dialog

class AFX_EXT_CLASS CHelpDialog : public CDialog
{

// Construction
public:
	CHelpDialog(UINT nIDTemplate, CWnd * pParent = NULL);
	~CHelpDialog();
	void Initialize();
	void SetHowToMenuIndex(UINT uiHTIndex)
		{m_uiMenuIndex = uiHTIndex; };
    virtual LRESULT OnHelpMessage(WPARAM wParam, LPARAM lParam);

// Dialog Data
	//{{AFX_DATA(CHelpDialog)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHelpDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CHelpDialog)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG

    virtual afx_msg BOOL MainDlgWhatsThisHelp(int ID);
	afx_msg void OnHelpInfoDesk();
	afx_msg void OnWhatsThis();
    afx_msg void ShutdownHelp(void);

	DECLARE_MESSAGE_MAP()

protected:
	CWnd*	m_pCurrentContextWnd;
	DWORD * m_pHelpHowToMap;
	DWORD * m_pHelpWhatsThisMap;
    DWORD * m_pHelpWhatsThisGrayMap;
	UINT	m_uiMenuIndex;

};
/////////////////////////////////////////////////////////////////////////////
// CHelpPropertyPage dialog

class AFX_EXT_CLASS CHelpPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CHelpPropertyPage)

// Construction
public:
	virtual BOOL HelpTopicExists(int ID);
	CHelpPropertyPage(UINT nIDTemplate);
	CHelpPropertyPage();
	~CHelpPropertyPage();
	void Initialize();
	inline void SetHowToMenuIndex(UINT uiHTIndex)
		{m_uiMenuIndex = uiHTIndex; };
    virtual LRESULT OnHelpMessage(WPARAM wParam, LPARAM lParam);

// Dialog Data
	//{{AFX_DATA(CHelpPropertyPage)
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CHelpPropertyPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CHelpPropertyPage)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	afx_msg void OnHelpInfoDesk();
	afx_msg void OnWhatsThis();

	DECLARE_MESSAGE_MAP()

protected:
	CWnd*	m_pCurrentContextWnd;
	DWORD * m_pHelpHowToMap;
	DWORD * m_pHelpWhatsThisMap;
	UINT	m_uiMenuIndex;

};


/////////////////////////////////////////////////////////////////////////////
// CStdHelpDlg dialog

class CStdHelpDlg : public CDialog
{
// Construction
public:
	CStdHelpDlg(UINT nIDTemplate, CWnd* pParent /*=NULL*/);
	BOOL HelpTopicExists(int ID);
// Dialog Data
	//{{AFX_DATA(CStdHelpDlg)

		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStdHelpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CStdHelpDlg)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LRESULT OnHelpMessage(WPARAM wParam, LPARAM lParam);
	void OnHelpInfoDesk();
	void OnWhatsThis();
	DECLARE_MESSAGE_MAP()

public:
	DWORD* m_pHelpHowToMap;
	DWORD* m_pHelpWhatsThisMap;
	CWnd* pCurrentContextWnd;


};
#endif
