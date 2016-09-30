/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_MANAGEWND_H__FB2E3B2C_69EA_4044_A809_900A04700707__INCLUDED_)
#define AFX_MANAGEWND_H__FB2E3B2C_69EA_4044_A809_900A04700707__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ManageWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CManageWnd window
class CManageWnd : public CWnd
{
// Construction
public:
	CManageWnd();

// Attributes
public:
	UINT m_hTimer;

	DWORD m_dwTimerIntervel;  // how may minutes loaded from registery
	DWORD m_dwPasses;
	DWORD m_dwIconIndex;
	BOOL  m_bSetImage;
    CQSConsoleData    *m_pRootNode;    
	CQSConsoleComponent* m_pCQSConsoleComponent;
// Operations
public:
	void ReadTimerIntervalFromReg(void);
	BOOL NeedChangeQSIcon(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManageWnd)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CManageWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CManageWnd)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
    HRESULT OnDisplayProgressText( WPARAM wParam, LPARAM lParam);
    HRESULT OnDisplayRecordCount( WPARAM wParam, LPARAM lParam);
    HRESULT OnNotifyRefresh( WPARAM wParam, LPARAM lParam);
    HRESULT OnNotifyReadFinished( WPARAM wParam, LPARAM lParam);

	void CleanupDataLists( void);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANAGEWND_H__FB2E3B2C_69EA_4044_A809_900A04700707__INCLUDED_)
