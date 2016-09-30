#if !defined(AFX_SYMWORLD_H__501500E1_C977_11D1_AE4A_006097959B77__INCLUDED_)
#define AFX_SYMWORLD_H__501500E1_C977_11D1_AE4A_006097959B77__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SymWorld.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSymWorld dialog

class CSymWorld : public CDialog
{
// Construction
public:
	CFont m_Font;
	CSymWorld(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSymWorld)
	enum { IDD = IDD_SYMANTEC_WORLDWIDE };
	CEdit	m_Worldwide;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSymWorld)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSymWorld)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYMWORLD_H__501500E1_C977_11D1_AE4A_006097959B77__INCLUDED_)
