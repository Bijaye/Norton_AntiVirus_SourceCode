#if !defined(AFX_DFLISTCTRL_H__529902E0_3CEC_11D2_A581_0004ACECC1E1__INCLUDED_)
#define AFX_DFLISTCTRL_H__529902E0_3CEC_11D2_A581_0004ACECC1E1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DFListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDFListCtrl window

class CDFListCtrl : public CListCtrlEx
{
// Construction
public:
	CDFListCtrl();

// Attributes
public:
  
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDFListCtrl)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDFListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDFListCtrl)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DFLISTCTRL_H__529902E0_3CEC_11D2_A581_0004ACECC1E1__INCLUDED_)
