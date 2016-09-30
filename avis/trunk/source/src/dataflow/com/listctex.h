// ListCtEx.h : interface of the CListCtrlEx class
// SCC: @(#) 66 10/11/99 11:30:22 Avnadmin32 1.3.1.1 @(#)
//

#ifndef DF_INCLUDE_LISTCTEX_H
#define DF_INCLUDE_LISTCTEX_H

#include "TitleTip.h"
#define MINCOLWIDTH 20

class CListCtrlEx : public CListCtrl
{
    DECLARE_DYNCREATE(CListCtrlEx)

// Construction
public:
    CListCtrlEx();

// Attributes
protected:
	void PrintHeader(CDC *pDC, CPrintInfo *pInfo);
	int DrawRow(CDC *pDC, int nItem, CPrintInfo *pInfo);
	void PrintFooter (CDC *pDC, CPrintInfo *pInfo);
    CToolTipCtrl    m_tooltip;

public:

// Overrides
protected:
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CListCtrlEx)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CListCtrlEx();
#ifdef _DEBUG
    virtual void Dump(CDumpContext& dc) const;
#endif
    void CListCtrlEx::AutoSizeColumns(int col =-1);
    BOOL CListCtrlEx::AddHeaderToolTip(int nCol, LPCTSTR sTip = NULL);

protected:
	int CListCtrlEx::CellRectFromPoint(CPoint & point, RECT * cellrect, int * col);
    int GetColumnCount() const;
    void CListCtrlEx::RecalcHeaderTips();

// Implementation - TitleTip
	CTitleTip m_titleTip;

// Printing
	int m_nCharWidth;
	int m_nRowHeight;
	int m_nRowsPerPage;
	int m_nMaxPage;
// Generated message map functions
protected:
    //{{AFX_MSG(CListCtrlEx)
    afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
	BOOL m_tooltipCreated;
	BOOL m_titleTipCreated;
};

/////////////////////////////////////////////////////////////////////////////
#endif
