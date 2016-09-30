////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// IWPTestDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "IWPPrivateSettingsLoader.h"
#include <memory>               // for auto_ptr
#include "ccCoInitialize.h"          // Com init
#include "SettingsWaitThread.h"

// CIWPTestDlg dialog
class CIWPTestDlg : 
    public CDialog,
    public CSettingsWaitSink
{
// Construction
public:
	CIWPTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IWPTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    LRESULT ReloadHandler (WPARAM wParam, LPARAM lParam);

    void OnWorkDone( LPCWSTR lpszKey );

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

    void getSettings (void);
public:
    CListCtrl m_listResults;

    IWP_IWPPrivateSettings m_IWPLoader;
    IWP::IIWPPrivateSettingsPtr m_pIWPSettings;
    std::auto_ptr <ccLib::CCoInitialize> m_COMInit;
    std::auto_ptr <CSettingsWaitThread> m_pIWPUpdate;

    DWORD m_dwState;
    BOOL m_bIsInstalled;
    BOOL m_bCanEnable;
    BOOL m_bUserWantsOn;
    BOOL m_bFirewallOn;
    CString m_strStateText;
    CString m_strSettingsChanged;   // Has the user change a setting?
    afx_msg void OnBnClickedRefresh();
    afx_msg void OnBnClickedSave();
    CString m_strStateOKText;
    afx_msg void OnBnClickedResetAles();
    afx_msg void OnBnClickedResetFwRules();
	afx_msg void OnBnClickedLegacyYield();
	afx_msg void OnBnClickedLegacyUnYield();
	afx_msg void OnBnClickedYield();
	afx_msg void OnBnClickedUnYield();

    afx_msg void OnBnClickedFirewallOn();
    afx_msg void OnBnClickedUserWantsOn();
};
