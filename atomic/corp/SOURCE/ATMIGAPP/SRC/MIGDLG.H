/////////////////////////////////////////////////////////////////////
// File: MigDlg.cpp
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ATMIGAPP/VCS/MIGDLG.H_v   1.0   27 May 1998 17:27:46   SEDWARD  $
//
// Description:
//  This is the header file for the "migapp.exe" program, which is
//  used to copy over the main NAV executables in a Win95->NT5
//  migration.  It just displays a dialog that informs the user of
//  the need to upgrade to a NT5-specific version.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGAPP/VCS/MIGDLG.H_v  $
// 
//    Rev 1.0   27 May 1998 17:27:46   SEDWARD
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////


#if !defined(AFX_MIGDLG_H__EB9F509B_659D_11D1_9D4A_00C04FB906C0__INCLUDED_)
#define AFX_MIGDLG_H__EB9F509B_659D_11D1_9D4A_00C04FB906C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CMigrateDlg dialog

class CMigrateDlg : public CDialog
{
// Construction
public:
	CMigrateDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMigrateDlg)
	enum { IDD = IDD_MIGAPP_DIALOG };
	CStatic	m_migrateText1;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMigrateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMigrateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MIGDLG_H__EB9F509B_659D_11D1_9D4A_00C04FB906C0__INCLUDED_)