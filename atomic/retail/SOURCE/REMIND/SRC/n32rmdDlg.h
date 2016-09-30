////////////////////////////////////////////////////////////////////////////////////
// $Header:   S:/REMIND/VCS/n32rmdDlg.h_v   1.3   17 Dec 1997 21:13:06   jtaylor  $
////////////////////////////////////////////////////////////////////////////////////
//
// n32rmdDlg.h : header file
//
////////////////////////////////////////////////////////////////////////////////////
// $Log:   S:/REMIND/VCS/n32rmdDlg.h_v  $
// 
//    Rev 1.3   17 Dec 1997 21:13:06   jtaylor
// Prevented the user from closing the window with Alt-f4 or the x button.
// 
//    Rev 1.2   15 Dec 1997 17:34:04   jtaylor
// fixed the logout problem, updated the timer logic.
//
////////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_N32RMDDLG_H__85687EC8_6DD3_11D1_8F2D_444553540000__INCLUDED_)
#define AFX_N32RMDDLG_H__85687EC8_6DD3_11D1_8F2D_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CN32rmdDlg dialog

class CN32rmdDlg : public CDialog
{
// Construction
public:
	BOOL m_bLaunchScanner;
	CN32rmdDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CN32rmdDlg)
	enum { IDD = IDD_N32RMD_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CN32rmdDlg)
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CN32rmdDlg)
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnNoscannow();
	afx_msg void OnScannow();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_N32RMDDLG_H__85687EC8_6DD3_11D1_8F2D_444553540000__INCLUDED_)
