////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVTestEventProviderDlg.h : header file
//

#pragma once

#include "ProviderManager.h"
#include "EventDescriptions.h"
#include "afxwin.h"

// CNAVTestEventProviderDlg dialog
class CNAVTestEventProviderDlg : public CDialog
{
// Construction
public:
	CNAVTestEventProviderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_NAVTESTEVENTPROVIDER_DIALOG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
public:
    CString m_strTestText;
    DWORD m_dwSendDelay;
    BOOL m_bLoopForever;
    BOOL m_bAllTypes;
    CButton m_btnStop;
    CButton m_btnGo;
    CEdit m_edtTestText;
    CEdit m_edtSendDelay;
    CButton m_chkLoop;
    CButton m_chkAllTypes;

    afx_msg void OnDestroy();
    afx_msg void OnTimer (UINT nIDEvent);
    afx_msg void OnBnClickedLaunchlogviewer();
    afx_msg void OnBnClickedGo();
    afx_msg void OnBnClickedStop();
    afx_msg void OnBtnClickedLaunchLogViewer();

protected:
	HICON m_hIcon;
    CProviderManager* m_pProvManager;
    CEventDescriptions m_EventDescriptions;
    CComboBox m_cboEventType;
    int m_iSendEvent; // Loop through the events to send

    void SendEvent(void);
    void stopUI (void);
    void runningUI (void);

    // Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
