// /////////////////////////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
// /////////////////////////////////////////////////////////////////////////////////////////////////
//
// SketchbookDlg.h : header file
//

#pragma once

#include "cceraserinterface.h"
#include "TestScanner.h"
#include "QuarantineDllLoader.h"
#include "AutoProtectWrapper.h"


const char g_szEicar[] = "X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";


// CSketchbookDlg dialog
class CSketchbookDlg : public CDialog
{
// Construction
public:
	CSketchbookDlg(CWnd* pParent = NULL);	// standard constructor
	~CSketchbookDlg();

	CString eResult2String(ccEraser::eResult eRes);
	void setResultWindow(ccEraser::eResult, CString strError);
	void setQuarantineWindow();
	void SetAnomalyCtrl();
	void RefreshRemediationCtrl();
	void RefreshDetectionCtrl();
	static CString GetTimeStampFile(CString strPath, CString strExtension);

// Dialog Data
	enum { IDD = IDD_SKETCHBOOK_DIALOG };
	CListCtrl m_listCtrlAnomaly;
	CListCtrl m_listCtrlDetections;
	CListCtrl m_listCtrlRemediations;
	CEdit m_editCtrlQuarItem;
	CEdit m_editCtrlResult;
	CEdit m_editCtrlScan;
	CButton m_buttonCtrlAbort;
	BOOL m_bLocalized;
	BOOL m_bCache;
	BOOL m_bUndo;
	BOOL m_bGetAnomaly;
	BOOL m_bScanAbort;

	CTestScanner m_Scanner;

	IScannerPtr m_pScanner;
	IScanSinkPtr m_pSink;
	IScanPropertiesPtr m_pProperties;
	ccEraser::IEraserPtr m_pEraser;

	ccEraser::IRemediationAction::Handle_Threat_Ops m_RepairOpt;

	ccEraser::IAnomalyListPtr m_pAnomalyList;
	ccEraser::IDetectionActionListPtr m_pDetectionActionList;
	ccEraser::IRemediationActionListPtr m_pRemediationActionList;

	ccEraser::IRemediationActionListPtr m_pDynamicRemediationActionList;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// The one quarantineDLL object
    QuarDLL_Loader m_QuarDllLoader;
    IQuarantineDLL* m_pQuarantineDLL;
	IQuarantineItem* m_pQuarItem;
	bool m_bAPToggled;
	CAutoProtectWrapper m_APWrapper;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnCancel();
	virtual void PostNcDestroy();

	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLvnItemchangedListAnomaly(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawListRemediation(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSetSettings();
	afx_msg void OnBnClickedUndo();
	afx_msg void OnBnClickedGetanomaly();
	afx_msg void OnBnClickedDetAnomaly();
	afx_msg void OnBnClickedRemediate();
	afx_msg void OnBnClickedScan();
	afx_msg void OnBnClickedDetect();
	afx_msg void OnBnClickedSaveanomaly();
	afx_msg void OnBnClickedSaveremediation();
	afx_msg void OnBnClickedSavedetection();
	afx_msg void OnBnClickedLoadobject();
	afx_msg void OnBnClickedReload();
	afx_msg void OnBnClickedRemoveanomaly();
	afx_msg void OnBnClickedRemovedetect();
	afx_msg void OnBnClickedRemoveremediate();
	afx_msg void OnBnClickedLocalize();
	afx_msg void OnBnClickedCache();
	afx_msg void OnBnClickedFullscan();
	afx_msg void OnBnClickedSaveundo();
    afx_msg void OnWinIniChange(LPCTSTR lpszSection);
	afx_msg void OnBnClickedAbort();
	afx_msg void OnBnClickedDetAnomalyList();
	afx_msg void OnBnClickedButtonQuarItemCreateWfile();
	afx_msg void OnBnClickedButtonQuarItemCreateWithoutFile();
	afx_msg void OnBnClickedButtonVirusFileCreate();
	afx_msg void OnBnClickedButtonRemediateAddData();
	afx_msg void OnBnClickedButtonSNDGetRemediationData();
	afx_msg void OnBnClickedButtonLoadQuarItem();
	afx_msg void OnBnClickedButtonScanQuarItem();
	afx_msg void OnBnClickedButtonRepairQuarItem();
	afx_msg void OnBnClickedButtonQuarDLLTakeSnapshot();
	afx_msg void OnBnClickedButtonQuarDLLAddSnapshotToQuarItem();
};

UINT ThreadGetAnomaly(LPVOID lpParameter);
UINT ThreadScan(LPVOID lpParameter);
UINT ThreadFullScan(LPVOID lpParameter);
