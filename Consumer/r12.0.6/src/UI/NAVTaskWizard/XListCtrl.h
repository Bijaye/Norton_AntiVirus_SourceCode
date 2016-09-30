// XListCtrl.h : header file
//

#if !defined(AFX_XLISTCTRL_H__F0732CC3_CC22_11D2_8D3B_95401FE6963E__INCLUDED_)
#define AFX_XLISTCTRL_H__F0732CC3_CC22_11D2_8D3B_95401FE6963E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CXListCtrl window -- List control that displays a "no items to show" 
// message when the control is empty.

class CXListCtrl : public CListCtrl
{
// Construction
public:
	CXListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CXListCtrl)
	//}}AFX_MSG
	afx_msg void OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XLISTCTRL_H__F0732CC3_CC22_11D2_8D3B_95401FE6963E__INCLUDED_)
