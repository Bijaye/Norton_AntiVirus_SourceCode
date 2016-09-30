////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include "ccLogFactoryEx.h"

using namespace ccEvtMgr;

class CTestLogDlg : public CDialog
{
// Construction
public:
	CTestLogDlg(CWnd* pParent = NULL);	// standard constructor

protected:
    EVENTMANAGERLib::ILogManagerPtr m_piLogManager;
    typedef std::vector<CLogFactoryEx::EventInfo> EventInfoVector;
    typedef std::vector<EVENTMANAGERLib::IEventExPtr> EventVector;
    EventInfoVector m_EventInfoVector;

    enum 
    { 
        EventTypeColumn = 0,
        EventIndexColumn = 1, 
        EventIndexCheckColumn = 2, 
        EventTimeStampColumn = 3, 
        EventSequenceColumn = 4, 
        EventValueColumn = 5 
    };

    void ResizeColumn(int nColumn, LPCTSTR szString);
    BOOL Convert(const VARIANT& vIndex,
                 const VARIANT& vIndexCheck,
                 const VARIANT& vTimeStamp,
                 const VARIANT& vSequenceNumber,
                 EventInfoVector& EventInfos);
    BOOL InsertEvent(int& nItem, 
                     CLogFactoryEx::EventInfo& EventInfo,
                     EVENTMANAGERLib::IEventExPtr& piEvent);


// Dialog Data
	//{{AFX_DATA(CTestLogDlg)
	enum { IDD = IDD_TESTLOG_DIALOG };
	CListCtrl	m_EventList;
	long	m_nEventCount;
	long	m_nEventType;
	long	m_nMaxSize;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestLogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTestLogDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOk();
	virtual void OnCancel();
	afx_msg void OnConnectButton();
	afx_msg void OnSearchButton();
	afx_msg void OnLoadButton();
	afx_msg void OnClearButton();
	afx_msg void OnSearchloadButton();
	afx_msg void OnGetSizeButton();
	afx_msg void OnSetSizeButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

