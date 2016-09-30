// win32rcDlg.h : header file
//
#if !defined(AFX_WIN32RCDLG_H__00C2D8E8_0A25_11D3_985D_000629167334__INCLUDED_)
#define AFX_WIN32RCDLG_H__00C2D8E8_0A25_11D3_985D_000629167334__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CWin32rcDlg dialog

class CWin32rcDlg : public CDialog
{
// Construction
public:
	CWin32rcDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CWin32rcDlg)
	enum { IDD = IDD_WIN32RC_DIALOG };
	CString	m_CurrentTxt;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWin32rcDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
     
	LONG OnSetText(WPARAM p1, LPARAM p2);
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWin32rcDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIN32RCDLG_H__00C2D8E8_0A25_11D3_985D_000629167334__INCLUDED_)
