////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TestSubscriberSub.h"
#include "NAVEventCommon.h"

class CTestSubscriberDlg : public CDialog
{
// Construction
public:
	CTestSubscriberDlg(CWnd* pParent = NULL);	// standard constructor

protected:
    CTestSubscriberSub m_Subscriber;
    CEventEx* m_pEvent;
    //CNAVEventCommon* m_pEvent;

    enum 
    { 
        EventTypeColumn = 0,
        EventIdColumn,
        BroadcastColumn,
        CancelColumn,
        UserNameColumn,
        ComputerNameColumn
    };

   void ResizeColumn(int nColumn, LPCTSTR szString);

// Dialog Data
	//{{AFX_DATA(CTestSubscriberDlg)
	enum { IDD = IDD_TESTSUBSCRIBER_DIALOG };
	CListCtrl	m_EventList;
	long	m_nSubscriberId;
	long	m_nPriority;
	int		m_nAction;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestSubscriberDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestSubscriberDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOk();
	virtual void OnCancel();
	afx_msg void OnConnectButton();
	afx_msg void OnRegisterButton();
	afx_msg void OnUnregisterButton();
	afx_msg void OnCommitButton();
	afx_msg void OnCancelButton();
	afx_msg void OnNoactionRadio();
	afx_msg void OnAutocancelRadio();
	afx_msg void OnAutocommitRadio();
	afx_msg void OnClearButton();
	//}}AFX_MSG
	afx_msg LRESULT OnEvent(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShutdown(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
