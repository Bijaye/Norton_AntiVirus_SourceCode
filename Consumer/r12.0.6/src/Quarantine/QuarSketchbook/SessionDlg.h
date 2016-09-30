// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
//// SessionDlg.h : header file
#pragma once

#include "SketchbookDlg.h"

// CSessionDlg dialog

class CSessionDlg : public CDialog
{
	DECLARE_DYNAMIC(CSessionDlg)

public:
	CSessionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSessionDlg();

	//CList<HWND> listHWnd;

// Dialog Data
	enum { IDD = IDD_SESSION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnCancel();

	HICON m_hIcon;
	CList<CSketchbookDlg *> listWnd;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedNewdefinition();
	afx_msg void OnBnClickedNewinstance();
	afx_msg LRESULT OnModeLessClose(WPARAM wParam, LPARAM lParam);
};
