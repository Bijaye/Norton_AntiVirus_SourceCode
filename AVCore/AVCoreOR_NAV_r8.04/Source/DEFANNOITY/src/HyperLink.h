//***************************************************************************
// HyperLink.h : header file
//***************************************************************************
// CBROWN 07/10/1999
//***************************************************************************
#if !defined(AFX_HYPERLINK_H__C522E120_372A_11D3_B367_00104B9DFF64__INCLUDED_)
#define AFX_HYPERLINK_H__C522E120_372A_11D3_B367_00104B9DFF64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//***************************************************************************
// CHyperLink window
//***************************************************************************
class CHyperLink : public CStatic
{
// Construction
public:
	CHyperLink();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHyperLink)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CHyperLink();

	COLORREF	m_crHyperLinkColor;
	HCURSOR		m_hHandCursor;
	CFont		m_Font;

	// Generated message map functions
protected:

	HINSTANCE CHyperLink::GotoURL( LPCTSTR url, int showcmd );

	//{{AFX_MSG(CHyperLink)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//***************************************************************************
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
//***************************************************************************

#endif // !defined(AFX_HYPERLINK_H__C522E120_372A_11D3_B367_00104B9DFF64__INCLUDED_)
