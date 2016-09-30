/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_MYLISTCTRL_H__102878A0_E86E_11D2_9597_00A024265061__INCLUDED_)
#define AFX_MYLISTCTRL_H__102878A0_E86E_11D2_9597_00A024265061__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyListCtrl.h : header file   ListCtrl
//

// FOR   ::Initialize(DWORD dwAllowSelection,DWORD dwUseReadOnlyLook)
#define  LC_ENABLE_TEXTSELECTION     0x0001 
#define  LC_DISABLE_TEXTSELECTION    0
#define  LC_ENABLE_READONLY_LOOK     0x0002
#define  LC_DISABLE_READONLY_LOOK    0
#define  LC_SELECT_ALL               0x0004
#define  LC_FULL_ROW_SELECTION       0x0008
#define  LC_PARTIAL_READONLY_LOOK    0x0010
#define	 LC_ENABLE_CHECKBOX			 0x0020


// FOR WIDTH SELECTION OF  COLUMS
//#define  ATTRIBUTE_COL_WIDTH         1
//#define  CLIENT_NAME_COL_WIDTH       2


#include "afxcmn.h"

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl window     CWnd

class CMyListCtrl : public CListCtrl
{

   	DECLARE_DYNCREATE(CMyListCtrl)
 
// Construction
public:
  	CMyListCtrl();

// Attributes
public:
    DWORD   m_dwAllowTextSelection;   // if TRUE, allow items to be selected
                                    // if FALSE,  and prevent selection
    DWORD   m_dwUseReadOnlyLook;      // if TRUE, set bgnd color to LT_GREY to appear read only.
    DWORD   m_dwInitialized;          // Was Initialize() called
    DWORD   m_dwSelectAllOnStart;     // Set all items to selected when inserted
            
    int     m_iLastItemInsertedPos;   // Last inserted item
    int     m_iNextItemInsertPos;     // Next pos to insert to end of list
            
    BOOL    m_bFullRowSel;
    BOOL    m_bAllowMultiSelect;
    CString sAttributeNameFromListbox;


// Attributes
protected:
    int   m_dwCol1WidthPcnt;
    //DWORD m_dwCol2WidthType;


// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyListCtrl();
  
    // Call from Derived class in OnInitDialog.   Set the operating mode.
    void Initialize( DWORD dwFlags, DWORD dwStyle = 0 );
    void InsertTwoColumns(LPCTSTR lpszColumn1Name,LPCTSTR lpszColumn2Name);
    int  InsertItemAndSubItem(LPTSTR lpszItem, LPTSTR lpszSubItem, LPARAM lParam = 0);
  
    // Called from CPlatfromAttributes
    BOOL GetSelectedAttributeName( TCHAR *lpszAttributeName );



// Implementation
protected:
//  Implementation - client area width
	int m_cxClient;
	CImageList m_StateImageList;

    // Implementation - list view colors
   	COLORREF m_clrText;
   	COLORREF m_clrTextBk;
   	COLORREF m_clrBkgnd;
   	afx_msg LRESULT OnSetTextColor(WPARAM wParam, LPARAM lParam);
   	afx_msg LRESULT OnSetTextBkColor(WPARAM wParam, LPARAM lParam);
   	afx_msg LRESULT OnSetBkColor(WPARAM wParam, LPARAM lParam);


    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    LPCTSTR MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset);
    void RepaintSelectedItems();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyListCtrl)
	afx_msg void OnItemchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnRightClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTCTRL_H__102878A0_E86E_11D2_9597_00A024265061__INCLUDED_)
