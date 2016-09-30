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

#include "sketchbookdlg.h"
#include "SetFilterDlg.h"

// SetSettings dialog

class SetSettings : public CDialog
{
	DECLARE_DYNAMIC(SetSettings)

public:
	SetSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~SetSettings();

// Dialog Data
	enum { IDD = IDD_SETSETTINGS };
	DWORD m_dwSettingsDepth;

	bool m_bScanDll;
	int m_dwDomain;
	int m_dwRepair;

	ISymBasePtr m_pInputFilter;
	ISymBasePtr m_pOutputFilter;

	CSketchbookDlg * m_pDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedFilter();
	afx_msg void OnBnClickedResetfilter();
	afx_msg void OnBnClickedOutfilter();
	afx_msg void OnBnClickedResetoutfilter();
};
