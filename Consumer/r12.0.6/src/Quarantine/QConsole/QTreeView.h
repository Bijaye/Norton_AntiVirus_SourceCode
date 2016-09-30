//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// QTreeView.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/QTreeView.h_v  $
// 
//    Rev 1.4   04 Jun 1998 15:37:44   DBuches
// Now, we emulate explorer in terms of right-clicking.
// 
//    Rev 1.3   21 May 1998 11:27:06   DBuches
// Added help topic IDs.
// 
//    Rev 1.2   06 Mar 1998 11:21:52   DBuches
// Checked in more work in progress.
// 
//    Rev 1.1   03 Mar 1998 17:05:02   DBuches
// Added quarantine items node.
// 
//    Rev 1.0   27 Feb 1998 15:10:26   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_QTREEVIEW_H__19CD21A4_A9CD_11D1_A526_0000C06F46D0__INCLUDED_)
#define AFX_QTREEVIEW_H__19CD21A4_A9CD_11D1_A526_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "qconsoleDoc.h"
#include <afxcview.h>


/////////////////////////////////////////////////////////////////////////////
// CQTreeView view

class CQTreeView : public CTreeView
{
protected:
	CQTreeView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CQTreeView)

// Attributes
public:
	CQconsoleDoc* GetDocument();

// Operations
public:
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQTreeView)
	public:
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CQTreeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CQTreeView)
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnViewContents();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void HandleFilterChange();
    void HandleRefresh();
    void SetupTree();
    void ShowPopupMenu( CPoint pt );

private:    
    // Tree nodes
    //HTREEITEM m_hRoot;
	//HTREEITEM m_hQuarantine;
	//HTREEITEM m_hBackup;
	//HTREEITEM m_hSubmitted;
	//HTREEITEM m_hThreatBackup;
	HTREEITEM m_hRootExpandedThreats;
	HTREEITEM m_hRootViralThreats;
    
    // Image list for tree.
    CImageList  m_ImageList;

    HTREEITEM   m_pOldSel;
};

#ifndef _DEBUG  // debug version in CQTreeView.cpp
inline CQconsoleDoc* CQTreeView::GetDocument()
   { return (CQconsoleDoc*)m_pDocument; }
#endif


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QTREEVIEW_H__19CD21A4_A9CD_11D1_A526_0000C06F46D0__INCLUDED_)
