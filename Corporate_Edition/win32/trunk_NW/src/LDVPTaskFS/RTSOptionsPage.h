// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// RTSOptionsPage.h : header file
//

#if !defined (RTS_OPTIONS_INCLUDED)
#define RTS_OPTIONS_INCLUDED

#include "WizardPage.h"
#include "LockCtrl.h"

#define IDC_ENABLED_LOCK		6000
#define IDC_WHEN_TO_SCAN_LOCK	6001
#define IDC_SCANCD_LOCK			6003
#define IDC_SCANFLOPPY_LOCK		6004
#define IDC_DENY_ACCESS_LOCK	6005

//Index values for Scan When control
#define INDEX_BOTH		0
#define INDEX_ACCESSED	1
#define INDEX_MODIFIED	2

/////////////////////////////////////////////////////////////////////////////
// CRTSOptionsPage dialog

class CRTSOptionsPage : public CWizardPage
{
	DECLARE_DYNCREATE(CRTSOptionsPage)

private:
	//These are all of my locks
	CLockCtrl	m_EnabledLock;
    CLockCtrl	m_WhenToScanLock;
    CLockCtrl	m_ScanCDLock;
	CLockCtrl	m_ScanFloppyLock;
    CLockCtrl	m_DenyAccessLock;
	BOOL		m_bInitialized;

	void GetDialogOptions();
	void SetDialogOptions();
	void SetLocks();
	void OnDestroying();

// Construction
public:
	CRTSOptionsPage();
	~CRTSOptionsPage();

// Dialog Data
	//{{AFX_DATA(CRTSOptionsPage)
	enum { IDD = IDD_RTS_OPTIONS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRTSOptionsPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CRTSOptionsPage)
	afx_msg void OnPaint();
	afx_msg void OnBrowseFileTypes();
	afx_msg void OnEnableRts();
	afx_msg void OnMessage();
	afx_msg void OnBrowseExcludeFiles();
	afx_msg void OnTypesAll();
	afx_msg void OnTypesSelected();
	afx_msg void OnExcludeFiles();
	afx_msg void OnMessageBox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
