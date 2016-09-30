// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_CONFIGURESNAPIN_H__D6AF2207_C3F8_11D1_8266_00A0C95C0756__INCLUDED_)
#define AFX_CONFIGURESNAPIN_H__D6AF2207_C3F8_11D1_8266_00A0C95C0756__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ConfigureSnapIn.h : header file
//
#include "ldvpui.h"

/////////////////////////////////////////////////////////////////////////////
// CConfigureSnapIn form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#define IDC_CONFIG_OCX	6000

class CConfigureSnapIn : public CFormView
{
private:
	CLDVPUI	m_configOCX;
	CFont	m_titleFont;

	void InitSnapIn( CRect rcClientRect );

protected:
	CConfigureSnapIn();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CConfigureSnapIn)

// Form Data
public:
	//{{AFX_DATA(CConfigureSnapIn)
	enum { IDD = IDD_CONFIGURE_SNAPIN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigureSnapIn)
	public:
	virtual void OnInitialUpdate();
	virtual void OnHelp();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CConfigureSnapIn();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CConfigureSnapIn)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGURESNAPIN_H__D6AF2207_C3F8_11D1_8266_00A0C95C0756__INCLUDED_)
