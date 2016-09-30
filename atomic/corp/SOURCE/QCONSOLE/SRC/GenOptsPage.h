/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/genoptspage.h_v   1.2   20 May 1998 17:11:18   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// GenOptsPage.h : header file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/genoptspage.h_v  $
// 
//    Rev 1.2   20 May 1998 17:11:18   DBuches
// 1st pass at help.
// 
//    Rev 1.1   08 May 1998 17:24:28   DBuches
// Added browse button.
// 
//    Rev 1.0   03 Apr 1998 13:30:08   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENOPTSPAGE_H__0B07EEA5_C8ED_11D1_9114_00C04FAC114A__INCLUDED_)
#define AFX_GENOPTSPAGE_H__0B07EEA5_C8ED_11D1_9114_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "iquaran.h"
#include "helpdlg.h"

/////////////////////////////////////////////////////////////////////////////
// CGenOptsPage dialog

class CGenOptsPage : public CHelperPage
{
	DECLARE_DYNCREATE(CGenOptsPage)

// Construction
public:
	CGenOptsPage();
	~CGenOptsPage();

// Help maps.
    static DWORD m_dwWhatsThisMap[];
    static DWORD m_dwHowToMap[];


// Dialog Data
	//{{AFX_DATA(CGenOptsPage)
	enum { IDD = IDD_OPTIONS_GENERAL_PAGE };
	CString	m_sServerName;
	BOOL	m_bEnableForward;
	BOOL	m_bEnblePassword;
	BOOL	m_bScanPrompt;
	//}}AFX_DATA

    // Pointer to options data
    IQuarantineOpts *m_pOpts;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGenOptsPage)
	public:
	virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGenOptsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnForwardingCheck();
	afx_msg void OnPasswordCheck();
	afx_msg void OnPasswordButton();
    afx_msg void OnBrowseButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    CString m_sPassword;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENOPTSPAGE_H__0B07EEA5_C8ED_11D1_9114_00C04FAC114A__INCLUDED_)
