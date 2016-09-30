// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_CONFIGHISTORY_H__03F30102_F668_11D1_82F8_00A0C9749EEF__INCLUDED_)
#define AFX_CONFIGHISTORY_H__03F30102_F668_11D1_82F8_00A0C9749EEF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ConfigHistory.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfigHistory dialog
#include "vpcommon.h"

class CConfigHistory : public CDialog
{
private:
	BOOL m_bManualChange;
	bool m_bApplyToClients;
	bool m_bShowApplyAll;
	bool m_bReadOnly;

// Construction
public:
	CConfigHistory(CWnd* pParent = NULL, bool bShowApplyAll = false, bool bReadOnly = false);   // standard constructor
	IGenericConfig	*m_pGenConfig;
	IConfig			*m_pConfig;
	IGenericConfig	*m_pConfig2;	// used by the admin only
	DWORD			m_Flags;

// Dialog Data
	//{{AFX_DATA(CConfigHistory)
	enum { IDD = IDD_CONFIG_HISTORY };
	CButton	m_ctlApplyToClients;
	CSpinButtonCtrl	m_ctlSpin;
	CEdit	m_ctlEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigHistory)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigHistory)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnKillfocusValue();
	afx_msg void OnHistoryHelp();
	afx_msg void OnApplyToClients();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGHISTORY_H__03F30102_F668_11D1_82F8_00A0C9749EEF__INCLUDED_)
