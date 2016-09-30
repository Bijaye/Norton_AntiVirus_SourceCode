//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// SEPage.h : header file
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEPage_H__47B210C4_A968_11D1_A523_0000C06F46D0__INCLUDED_)
#define AFX_SEPage_H__47B210C4_A968_11D1_A523_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "proppage.h"


/////////////////////////////////////////////////////////////////////////////
// CSEPage dialog

class CSEPage : public CItemPage
{
	DECLARE_DYNCREATE(CSEPage)

// Construction
public:
	CSEPage();
	~CSEPage();

// Dialog Data
	//{{AFX_DATA(CSEPage)
	enum { IDD = IDD_SE_PAGE };
	CString	m_sFiles;
	CString	m_sRegKeys;
	CString	m_sVirusName;
	//}}AFX_DATA

    void SetVirusIndex( unsigned long ulVID ) { m_ulVID = ulVID; }

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSEPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSEPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    CSymPtr<IScanVirusInfo> m_pScanVirusInfo;
    unsigned long m_ulVID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEPage_H__47B210C4_A968_11D1_A523_0000C06F46D0__INCLUDED_)
