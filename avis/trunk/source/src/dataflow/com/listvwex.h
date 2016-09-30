// ListVwEx.h : interface of the CListViewEx class
// SCC: @(#) 67 10/11/99 11:30:35 Avnadmin32 1.2.1.1 @(#)
//

#include "TitleTip.h"
#define MINCOLWIDTH 20

class CListViewEx : public CListView
{
    DECLARE_DYNCREATE(CListViewEx)

// Construction
public:
    CListViewEx();

// Attributes
protected:
	void PrintHeader(CDC *pDC, CPrintInfo *pInfo);
	int DrawRow(CDC *pDC, int nItem, CPrintInfo *pInfo);
	void PrintFooter (CDC *pDC, CPrintInfo *pInfo);
    CToolTipCtrl    m_tooltip;

public:

// Overrides
protected:
    //virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CListViewEx)
	public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CListViewEx();
#ifdef _DEBUG
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	int CListViewEx::CellRectFromPoint(CPoint & point, RECT * cellrect, int * col);
    int GetColumnCount() const;
    void CListViewEx::AutoSizeColumns(int col =-1);
    BOOL CListViewEx::AddHeaderToolTip(int nCol, LPCTSTR sTip = NULL);
    void CListViewEx::RecalcHeaderTips();

// Implementation - TitleTip
	CTitleTip m_titleTip;

// Printing
	int m_nCharWidth;
	int m_nRowHeight;
	int m_nRowsPerPage;
	int m_nMaxPage;
// Generated message map functions
protected:
    //{{AFX_MSG(CListViewEx)
    afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
