// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_LICENSINGVIEW_H__4F8CD93F_8D82_4A8E_9CEE_623E5F3EF630__INCLUDED_)
#define AFX_LICENSINGVIEW_H__4F8CD93F_8D82_4A8E_9CEE_623E5F3EF630__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LicensingView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLicensingView form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "WizardPage.h"
#include "StaticLink.h"
#include "Resource.h"

class CLicensingView : public CWizardPage
{
public:
	CLicensingView();
	virtual ~CLicensingView();

	DECLARE_DYNCREATE(CLicensingView)

// Form Data
public:
	//{{AFX_DATA(CLicensingView)
	enum { IDD = IDD_LICENSING };
	CStaticLink	m_InstructionLinkCtl;
	CString	m_InstructionText;
	CString	m_LicExpiryDate;
	CString	m_LicSerialNumber;
	CString	m_LicStatus;
	CString	m_LicStartDate;
	CString	m_InstructionLink;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

	BOOL OnSetActive();
	BOOL OnWizardFinish();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLicensingView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLicensingView)
	afx_msg void OnInstallLicense();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void UpdateLicenseData();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LICENSINGVIEW_H__4F8CD93F_8D82_4A8E_9CEE_623E5F3EF630__INCLUDED_)
