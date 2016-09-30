//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
// 
// RestoreDlg.h - Header for class to handle simple custom dialog for 
// confirming restores.  If there are expanded threat items, adds a checkbox
// to ask the user if they want to exclude the selected items from future 
// scans.
// Fetches appropriate strings for: 
//		multiple vs single items selected
//		viral vs. non-viral view
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "qconsoleDoc.h"


// CRestoreDlg dialog

class CRestoreDlg : public CDialog
{
	DECLARE_DYNAMIC(CRestoreDlg)

public:
	CRestoreDlg(bool bKnownExpandedThreats,
		int iSelectedItems,
		CWnd* pParent = NULL);
	
	virtual ~CRestoreDlg();

private:
	CRestoreDlg();  // Prevent canonical behavior

public:
// Dialog Data
	enum { IDD = IDD_RESTORE_DIALOG };

	bool ShouldExcludeSelected();
    void SetDocument(CQconsoleDoc* pDoc) { m_pDoc = pDoc; }

	static CString m_sAppName;

public:
	afx_msg void OnBnClickedYes();
	afx_msg void OnBnClickedNo();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	bool IsExpandedThreatView();

	// Generated message map functions
	//{{AFX_MSG(CRestoreDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool m_bKnownExpandedThreats;
	int m_iSelectedItems;

	CQconsoleDoc* m_pDoc;

	HICON m_hIcon;
	CString m_sRestoreDlgText;
	BOOL m_bExclusionChecked;
};
