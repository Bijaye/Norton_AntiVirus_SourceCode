#if !defined(AFX_SERVERWIZARD_H__EB7D6FB0_DDC0_11D2_ADE8_830E8303CE4E__INCLUDED_)
#define AFX_SERVERWIZARD_H__EB7D6FB0_DDC0_11D2_ADE8_830E8303CE4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ServerWizard.h : header file
//


#include "resource.h"
#include "QPropPage.h"
#include "qsconsole.h"
/////////////////////////////////////////////////////////////////////////////
// CServerWizard dialog

class CServerWizard : public CQPropPage
{
	DECLARE_DYNCREATE(CServerWizard)

// Construction
public:
	CServerWizard();
	~CServerWizard();

// Dialog Data
	//{{AFX_DATA(CServerWizard)
	enum { IDD = IDD_SELECT_COMPUTER_PAGE };
	int		m_iComputer;
	CString	m_sDomain;
	CString	m_sPassword;
	CString	m_sUser;
	CString	m_sServerName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CServerWizard)
	public:
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

public:
    long            m_lNotifyHandle;
    CSnapInItem*    m_pRootNode;


// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CServerWizard)
	virtual BOOL OnInitDialog();
	afx_msg void OnChooserRadioLocalMachine();
	afx_msg void OnChangeChooserEditMachineName();
	afx_msg void OnChooserButtonBrowseMachinenames();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    BOOL AttemptConnection( LPCTSTR pszServer = NULL,
                            LPCTSTR pszUser = NULL,
                            LPCTSTR pszPassword = NULL,
                            LPCTSTR pszDomain = NULL );

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERWIZARD_H__EB7D6FB0_DDC0_11D2_ADE8_830E8303CE4E__INCLUDED_)
