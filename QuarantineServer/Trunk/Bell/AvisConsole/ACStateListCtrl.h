/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_ACSTATELISTCTRL_H__273B1458_BC44_4A50_8F72_6D31A93ED129__INCLUDED_)
#define AFX_ACSTATELISTCTRL_H__273B1458_BC44_4A50_8F72_6D31A93ED129__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ACStateListCtrl.h : header file
//
#include "ListCtrlEx.h"
#include "resource.h"


#define WM_USER_UPDATE_VALUE	WM_USER+123
#define WM_USER_EDIT_CLOSING	WM_USER+124
#define WM_USER_INFO_DIRTY		WM_USER+125

/////////////////////////////////////////////////////////////////////////////
// CACStateListCtrl window
class CEventObject :public CObject
{
public:
	CEventObject()
	{
//		m_dwTime = 0;
		m_bEnabled = FALSE;
        dwEventRulesTableID = 0;
        dwEventID = 0;
	};
	
	~CEventObject (){};

	CString m_sEventName;
	CString m_sTime;
	BOOL    m_bEnabled;
    DWORD   dwEventRulesTableID;
    DWORD   dwEventID;
	int m_iIndex;
//	DWORD m_dwTime;
};


class CACStateListCtrl : public CListCtrlEx
{
// Construction
public:
	CACStateListCtrl();
		

// Attributes
public:
	CEdit *m_pEdit;
// Operations
public:
    virtual void Initialize(void);
    int  InsertEventItem(CEventObject* pEvent);
	afx_msg LRESULT OnUpdateValue(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEditClose(WPARAM wParam, LPARAM lParam);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CACStateListCtrl)
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CACStateListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CACStateListCtrl)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	CImageList m_images;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACSTATELISTCTRL_H__273B1458_BC44_4A50_8F72_6D31A93ED129__INCLUDED_)
