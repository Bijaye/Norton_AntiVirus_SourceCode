// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006, Symantec Corporation, All rights reserved.
// HPPBasic.h
#pragma once
#include "BaseUI.h"
#include "afxwin.h"
#include "afxcmn.h"

/**
* @pkg CHPPBasic
* @author Terry Marles
* @version 1.0              
*
* The CHPPBasic class implements the basic panel for HPP.
*
**/

// CHPPBasic dialog

class CHPPBasic : public CBaseUI
{
	DECLARE_DYNAMIC(CHPPBasic)

public:
	CHPPBasic(DWORD dwMode, DWORD dwConfiguration,CWnd* pParent = NULL);   // standard constructor
	virtual ~CHPPBasic();

// Dialog Data
	enum { IDD = IDD_HPP_BASIC };


	/**
	* Load is called by various dialog routines to read the settings out of the 
	* configuration store.
	*
    * @return void
	* 
	*/
	void Load();

	/**
	* Store is called by various dialog routines to write modified settings out to the 
	* configuration store.
	*
    * @return void
	* 
	*/
	void Store();
	DWORD HelpId(){ return IDD; };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedEnableHPPRts();
	afx_msg void OnStnClickedHPPRtStatus();
	afx_msg void OnStnClickedHPPDetections();
	afx_msg void OnUpdateUI();
	CButton m_ctrlEnableHeurPP;
	CStatic m_ctrlHeurPPStatusText;
	CStatic m_ctrlHeurPPDetectionsText;
	CStatic m_ctrlHeurPPNewPSTText;
	CStatic m_ctrlHeurPPFPStimeText;
	CStatic m_ctrlHeurPPProcessNameText;
	CStatic m_ctrlHeurPPFilenameText;
	CStatic m_ctrlHeurPPActionText;
	CStatic m_ctrlHeurPPActionTimeText;
	BOOL m_bEnableHeurPP;

protected:
	virtual void OnCancel();
	virtual void OnOK();

private:
	DWORD m_dwNewProccessScanInterval;
	DWORD m_dwFullProcessScanInterval;
};
