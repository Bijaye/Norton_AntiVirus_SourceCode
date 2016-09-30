/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/qconsoleview.h_v   1.11   21 May 1998 11:27:06   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// qconsoleView.h : interface of the CQconsoleView class
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/qconsoleview.h_v  $
// 
//    Rev 1.11   21 May 1998 11:27:06   DBuches
// Added help topic IDs.
// 
//    Rev 1.10   18 May 1998 13:33:46   DBuches
// 1st pass at corporate user directory creation.
// 
//    Rev 1.9   12 May 1998 15:20:54   DBuches
// Added code for NT security support.
// 
//    Rev 1.8   07 May 1998 18:18:54   DBuches
// Added code to enforce operaion restrictions in All Items view mode.
// 
//    Rev 1.7   07 May 1998 15:08:12   DBuches
// Added support for status text.
// 
//    Rev 1.6   27 Apr 1998 16:11:14   DBuches
// Fixed a few minor bugs, and hooked up corporate mode stuff.
// 
//    Rev 1.5   07 Apr 1998 16:49:40   DBuches
// 1st pass at startup scanning.
// 
//    Rev 1.4   13 Mar 1998 15:22:46   DBuches
// Hooked up item repair.
// 
//    Rev 1.3   11 Mar 1998 15:16:40   DBuches
// 1st pass at Restore item.
// 
//    Rev 1.2   06 Mar 1998 11:21:52   DBuches
// Checked in more work in progress.
// 
//    Rev 1.1   03 Mar 1998 17:04:48   DBuches
// 1st pass at callback driven list.
// 
//    Rev 1.0   27 Feb 1998 15:10:26   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_QCONSOLEVIEW_H__396DC361_A5D8_11D1_A51E_0000C06F46D0__INCLUDED_)
#define AFX_QCONSOLEVIEW_H__396DC361_A5D8_11D1_A51E_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxcview.h>
#include "const.h"
#include "resultsdlg.h"

class CQconsoleView : public CListView
{
protected: // create from serialization only
	CQconsoleView();
	DECLARE_DYNCREATE(CQconsoleView)

// Attributes
public:
	CQconsoleDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQconsoleView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQconsoleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	
    
    //{{AFX_MSG(CQconsoleView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnActionRepair();
	afx_msg void OnActionDeleteitem();
	afx_msg void OnActionRestore();
	afx_msg void OnActionProperties();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnActionSubmittosarc();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);
    afx_msg void OnUpdateSelectedItem(CCmdUI* pCmdUI);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnWhatsThis();
	afx_msg void OnViewContents();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL IsExecutable(CString& sFileName);
	void HandleStyleChange();
    void HandleFilterChange();
	void SetupListCtrl();
	void HandleRefresh();
    void HandleNewDefsScan();
	BOOL IsItemSelected();
    void SaveColumnWidths();
    void GetColumnWidths(int *pColumns, int nCount );
    void PopulateListCtrl( BOOL bRebuild = FALSE );
    void DrawArrow( CDC* pDC, int x, int y, BOOL bUp);
    int RestoreItem( IQuarantineItem* pItem, CResultsListItem* pResult );
    int RepairItem( IQuarantineItem* pItem, CResultsListItem* pResult );
    void DoRepair( BOOL bRepair );
    void EnableForViewModeAll(CCmdUI* pCmdUI);
    BOOL GetSaveLocation( CString& sFilePath, CString& sOriginalFileName, CString& sFileName );
    BOOL CreateItemUserDirectory( IQuarantineItem* pItem, CString& sFilePath, CString& sDestFileName, CString& sFileName );

    static int PASCAL SortAscendCompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );
    static int PASCAL SortDescendCompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );

    HRESULT GetFileSecurityDesc( LPSTR lpszSourceFileName, LPBYTE *pDesc );
    HRESULT SetFileSecurityDesc( LPSTR lpszSourceFileName, LPBYTE pDesc );


private:
	void GetExeImages();
	void GetItemImage( LV_ITEM* pItem );
    void GetItemText( LV_ITEM* pItem );

    int     m_iLastViewMode;
    int     m_iLargeExeImage;
    int     m_iSmallExeImage;
    BOOL    m_bSortAscending;


};

#ifndef _DEBUG  // debug version in qconsoleView.cpp
inline CQconsoleDoc* CQconsoleView::GetDocument()
   { return (CQconsoleDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
// Popup menu indexes
#define VIEW_POPUP          0
#define VIEW_POPUP_ONITEM   1


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QCONSOLEVIEW_H__396DC361_A5D8_11D1_A51E_0000C06F46D0__INCLUDED_)
