/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#ifndef __CLISTCTRLEX_H__
#define __CLISTCTRLEX_H__

// ListCtrlEx.h : header file
//
#include "afxcmn.h"

/////////////////////////////////////////////////////////////////////////////
// CStaticTextWnd window

class CStaticTextWnd : public CWnd
{
// Construction
public:
        CStaticTextWnd();

// Attributes
public:

// Operations
public:

// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CStaticTextWnd)
        //}}AFX_VIRTUAL

// Implementation
public:
        virtual ~CStaticTextWnd();

        // Generated message map functions
protected:
        //{{AFX_MSG(CStaticTextWnd)
        afx_msg void OnPaint();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx window

#define SORT_NONE               0
#define SORT_ASCENDING  1
#define SORT_DESCENDING 2

// offsets for first and other columns
#define OFFSET_FIRST    2
#define OFFSET_OTHER    6

// Right mouse click notification
#define LVN_RCOLUMNCLICK        WM_USER+10
#define WM_POSTCREATE           WM_USER+11

#define LVS_PRESELECTION        0x00000001


class CListCtrlEx : public CListCtrl
{
// Construction
public:
        CListCtrlEx();
        void ForwardSysColorChange();
        int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
        int InsertColumn(int nCol, const LV_COLUMN* pColumn);
        void ModifyUserStyle(DWORD dwClear, DWORD dwSet);
        DWORD GetUserStyle();
        BOOL SortItems( PFNLVCOMPARE pfnCompare, DWORD dwData, int iSortColumn );
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

// Attributes
public:
        int m_iSortOrder;
        int m_iSortColumn;
        int m_iLastSortColumn;
        CStaticTextWnd *m_phStaticText;

protected:
        DWORD m_dwUserStyle;

// Operations
public:
// Overrides

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CListCtrlEx)
        //}}AFX_VIRTUAL


// Implementation
public:
        int ClearStaticText();
        int SetStaticText(CString str);
        int SetStaticText(DWORD idStr);
        int m_cxClient;
        int m_cyHeader;
        BOOL m_bClientWidthSel;
        int m_fBmpsCreated;
        virtual ~CListCtrlEx();

        // Generated message map functions
protected:
        int m_nSelItem;
        CFont m_underlineFont;
        CFont m_font;
        int SetHeaderBmp(int nCol, HBITMAP hBmp);
        CHeaderCtrl * GetHeaderCtrl();
        COLORREF m_colorButtonFace;
        HBITMAP m_hbmpDescending;
        HBITMAP m_hbmpAscending;
        HBITMAP m_hbmpNone;
        PFNLVCOMPARE m_pCompareFunc;

        BOOL    m_bCaptured;
        CWnd*   m_pPreCaptureWnd;

        //{{AFX_MSG(CListCtrlEx)
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnPaint();
        afx_msg void OnKillFocus(CWnd* pNewWnd);
        afx_msg BOOL OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnCaptureChanged(CWnd *pWnd);
        //}}AFX_MSG
        afx_msg long OnPostCreate(WPARAM wParam, LPARAM lParam);
        void DoGenericColumnDraw(LPDRAWITEMSTRUCT lpDrawItemStruct);
        LPCTSTR MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset);
        afx_msg LRESULT OnSetImageList(WPARAM wParam, LPARAM lParam);
        void RepaintSelectedItems();
        virtual void PostSort();
        virtual void PreSort(int iSortColumn);
        virtual BOOL HiliteItem(CPoint &point, BOOL force = FALSE, BOOL bUrgent = FALSE);
        virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif
