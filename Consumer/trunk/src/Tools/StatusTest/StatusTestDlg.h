////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// StatusTestDlg.h : header file
//

#pragma once
#include "EMSubscriber.h"

// CStatusTestDlg dialog
class CStatusTestDlg : public CDialog, public CEMSubscriberSink
{
// Construction
public:
	CStatusTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_STATUSTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    // EMSubscriberSink
    void EMSubscriberOnEvent (const ccEvtMgr::CEventEx& Event,
                                    ccEvtMgr::CSubscriberEx::EventAction& eAction);
    void EMSubscriberOnShutdown ();

    void EndDialog(int nResult);

// Implementation
protected:
    CEMSubscriber* m_pEMSubscriber;
    std::vector <long> m_vecEventIDs;
    AV::IAvEventFactoryPtr m_pProvider;

	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CString m_strLog;
    afx_msg void OnBnClickedGetstatus();
    BOOL m_bALU;
    BOOL m_bAP;
    BOOL m_bFSS;
protected:
    void getProperties(CEventData& edStatus);
    const char* getStatusString (DWORD dwStatus);
    const char* CStatusTestDlg::getEventString (long lEventID );
public:
    BOOL m_bDefs;
    BOOL m_bEmail;
    BOOL m_bIWP;
    BOOL m_bLicensing;
    BOOL m_bIM;
    afx_msg void OnRefreshAP();
    afx_msg void OnBnClickedClear();
    BOOL m_bSpyware;
};
