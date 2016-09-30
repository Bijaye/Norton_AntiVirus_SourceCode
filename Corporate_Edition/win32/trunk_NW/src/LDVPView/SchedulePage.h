// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_SCHEDULEPAGE_H__971A1321_70BB_11D1_81CA_00A0C95C0756__INCLUDED_)
#define AFX_SCHEDULEPAGE_H__971A1321_70BB_11D1_81CA_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WizardPage.h"
#include "ListCtrlEx.h"

// SchedulePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSchedulePage dialog

class CSchedulePage : public CWizardPage
{
	DECLARE_DYNCREATE(CSchedulePage)

private:
	CImageList	m_ilImages;

	void GetAdminScans();
	void GetClientScans();

	CString GetScheduleString( CConfigObj &config );
	void AddScan( CString strTaskname, CString strScheduleInfo, DWORD dwOriginID );
	CString GetMinutes( DWORD dwMinutes );

// Construction
public:
	CSchedulePage();
	~CSchedulePage();

// Dialog Data
	//{{AFX_DATA(CSchedulePage)
	enum { IDD = IDD_TASK_SCHEDULE };
	CListCtrlEx	m_List;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSchedulePage)
	public:
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSchedulePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	void SizePage( int cx, int cy );
	
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEDULEPAGE_H__971A1321_70BB_11D1_81CA_00A0C95C0756__INCLUDED_)
