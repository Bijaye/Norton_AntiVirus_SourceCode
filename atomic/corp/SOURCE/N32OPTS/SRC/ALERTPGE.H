/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/alertpge.h_v   1.4   25 Jun 1997 15:16:04   DBUCHES  $
/////////////////////////////////////////////////////////////////////////////
//
// AlertPge.h : header file
//
//
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/alertpge.h_v  $
// 
//    Rev 1.4   25 Jun 1997 15:16:04   DBUCHES
// Converted buttons to owner draw buttons.
// 
//    Rev 1.3   13 Jun 1997 21:57:38   JBRENNA
// Reworked NLM alerting and added Forward alerting to NAV NT alert services.
// 
//    Rev 1.2   27 May 1997 20:55:12   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.1   18 May 1997 23:26:34   DBUCHES
// Fixed browsing for NAV-NLM servers.
//
//    Rev 1.0   14 May 1997 15:01:20   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_ALERTPGE_H__2BCEBDF5_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
#define AFX_ALERTPGE_H__2BCEBDF5_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "helpdlg.h"
#include "iconbtn.h"

/////////////////////////////////////////////////////////////////////////////
// CAlertsPage dialog

class CAlertsPage : public CHelperPage
{
	DECLARE_DYNCREATE(CAlertsPage)

// Construction
public:
	BOOL GetData(LPNAVOPTIONS pOptions);
	BOOL SetData(LPNAVOPTIONS pOptions);
	CAlertsPage();
	~CAlertsPage();

// Dialog Data
	//{{AFX_DATA(CAlertsPage)
	enum { IDD = IDD_ALERTS_PAGE };
	CSpinButtonCtrl	m_Spin;
	BOOL	m_bDisplayAlert;
	CString	m_sAlertText;
	CString	m_sSecondsText;
	BOOL	m_bRemoveAfter;
	BOOL	m_bAlertNLM;
	BOOL	m_bBeep;
	BOOL	m_bNtAlert;
	CString	m_sNtAlertServer;
	CString	m_sNLMCombo;
    CIconButton m_BrowseBtn;
	//}}AFX_DATA
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];



// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAlertsPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAlertsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDisplayCheck();
	afx_msg void OnRemoveCheck();
	afx_msg void OnNlmCheck();
	afx_msg void OnDropdownNlmCombo();
	afx_msg void OnNtAlertCheck();
	afx_msg void OnNtAlertBrowse();
	virtual BOOL OnKillActive();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void InitNLMCombo();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALERTPGE_H__2BCEBDF5_C26D_11D0_A3F2_0000C06F46D0__INCLUDED_)
