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
#pragma once
#include "afxwin.h"


// CDetectDialog dialog

class CDetectDialog : public CDialog
{
	DECLARE_DYNAMIC(CDetectDialog)

public:
	CDetectDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDetectDialog();

// Dialog Data
	enum { IDD = IDD_DETECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	DWORD m_dwVID;
	CEdit m_editCtrlFile;
	CEdit m_editCtrlShortFile;
	TCHAR m_szLongPath[MAX_PATH];
	TCHAR m_szShortPath[MAX_PATH];
	afx_msg void OnBnClickedOk();
};
