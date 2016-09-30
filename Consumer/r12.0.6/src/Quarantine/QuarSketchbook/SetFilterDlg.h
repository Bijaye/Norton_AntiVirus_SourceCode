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

#include "ccSymInterfaceLoader.h"
#include "ccSymPathProvider.h"
#include "ccEraserInterface.h"
#include "ccModuleNames.h"

// SetFilterDlg dialog

class SetFilterDlg : public CDialog
{
	DECLARE_DYNAMIC(SetFilterDlg)

public:
	SetFilterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SetFilterDlg();

// Dialog Data
	enum { IDD = IDD_SETFILTERDLG };
	bool m_chkFile;
	bool m_chkRegistry;
	bool m_chkProcess;
	bool m_chkService;
	bool m_chkBatch;
	bool m_chkINI;
	bool m_chkStartup;
	bool m_chkCOM;

	bool m_chkViral;
	bool m_chkMalicious;
	bool m_chkReservedMalicious;
	bool m_chkHeuristic;
	bool m_chkSecurityRisk;
	bool m_chkHacktool;
	bool m_chkSpyWare;
	bool m_chkTrackware;
	bool m_chkDialer;
	bool m_chkRemoteAccess;
	bool m_chkAdware;
	bool m_chkJoke;
	bool m_chkGenericLoadpoint;
	bool m_chkClientCompliancy;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:

	BOOL m_bOutFilter;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
