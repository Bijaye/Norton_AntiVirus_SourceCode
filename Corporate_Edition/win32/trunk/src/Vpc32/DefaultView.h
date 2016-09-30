// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: DefaultView.h
//  Purpose: CDefaultView Definition file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#define WM_UPDATE_QUARANTINE_STATUS  WM_USER+105

#include "LockCtrl.h"
#include "Bitmap256.h"

#define IDC_UPDATE_PATTERNFILE_LOCK 8001

//----------------------------------------------------------------
//
// CDefaultView class
//
//----------------------------------------------------------------
class CDefaultView : public CFormView
{
private:
	CLockCtrl	m_updateLock;

    BOOL GetDefsDate(LPWORD lpwYear, LPWORD lpwMonth, LPWORD lpwDay);

protected:
	CDefaultView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDefaultView)
	CFont m_welcomeFont;


	void GetRegInfo();
	LPARAM OnUpdateDisplayedDefVersion( WPARAM wparam, LPARAM lparam );

// Form Data
public:
	//{{AFX_DATA(CDefaultView)
	enum { IDD = IDD_DEFAULT };
	CBitmap256 m_image;
	//}}AFX_DATA

// Attributes
public:

protected:
	//Tooltip use: See MSDN's Q141758 for details
	CToolTipCtrl* m_pParentServerToolTip;
	BOOL m_bParentServerToolTipInitialized;

// Operations
public:
    DWORD UpdateQuarantineCount(DWORD dwQuarantineFlag);
	void UpdateQuarantineStatus();
	LPARAM OnUpdateQuarantineStatus( WPARAM wparam, LPARAM lparam );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefaultView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDefaultView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CDefaultView)
	afx_msg void OnUpdatePatternfile();
	afx_msg void OnScheduleUpdates();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnExit();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

