// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_LSLICENSINGVIEW_H__D52129FE_A3FC_427C_90BB_8B72F88B7067__INCLUDED_)
#define AFX_LSLICENSINGVIEW_H__D52129FE_A3FC_427C_90BB_8B72F88B7067__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LSLicensingView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLSLicensingView dialog
#include "WizardPage.h"
#include "StaticLink.h"
#include "afxwin.h"

class CLSLicensingView : public CWizardPage
{
	DECLARE_DYNCREATE(CLSLicensingView)

// Construction
public:
	CLSLicensingView();
	~CLSLicensingView();
	void UpdateLicenseData();

// Dialog Data
	//{{AFX_DATA(CLSLicensingView)
	enum { IDD = IDD_LS_LICENSING };
	CStaticLink	m_InstructionLinkCtl;
	CString	m_InstructionLink;
	CString	m_InstructionText;
	CString	m_LicExpiryDate;
	CString	m_LicStartDate;
	CString	m_LicStatus;
	CString	m_LicVeniceId;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLSLicensingView)
	public:
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLSLicensingView)
	virtual BOOL OnInitDialog();
	afx_msg void OnInstallLicense();
	afx_msg void OnSync();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CLSLicensingViewDlg : public CDialog
{
// Construction
public:
	CLSLicensingViewDlg(CWnd* pParent = NULL);	// standard constructor

	void SetTheParentDlg(CLSLicensingView *pLSLicensingView) {m_pDlgLSLicensingView = pLSLicensingView;};

// Dialog Data
	//{{AFX_DATA(CLSLicensingViewDlg)
	enum { IDD = IDD_EVENT_LOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLSLicensingViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CLSLicensingViewDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CLSLicensingView *m_pDlgLSLicensingView;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LSLICENSINGVIEW_H__D52129FE_A3FC_427C_90BB_8B72F88B7067__INCLUDED_)
