// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/FOTDPg.h_v   1.1   26 Apr 1998 17:26:30   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// FOTDPg.h: interface for the CFileOfTheDayPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/FOTDPg.h_v  $
// 
//    Rev 1.1   26 Apr 1998 17:26:30   SEDWARD
// Added GetTemplateID(); if user declines file of the day submission, 
// we jump to the "finish" page.
//
//    Rev 1.0   24 Apr 1998 18:10:20   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_FOTDPG_H__56F10F50_DA95_11D1_A7FB_0000E8D34392__INCLUDED_)
#define AFX_FOTDPG_H__56F10F50_DA95_11D1_A7FB_0000E8D34392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "ScanDeliverDLL.h"
#include "WizPage.h"



/////////////////////////////////////////////////////////////////////////////
// CFileOfTheDayPropertyPage dialog

class CFileOfTheDayPropertyPage : public CWizardPropertyPage
{
	DECLARE_DYNCREATE(CFileOfTheDayPropertyPage)

// Construction
public:
	CFileOfTheDayPropertyPage();
    CFileOfTheDayPropertyPage(CScanDeliverDLL*  pDLL);
	~CFileOfTheDayPropertyPage();

// Dialog Data
	//{{AFX_DATA(CFileOfTheDayPropertyPage)
	enum { IDD = IDD_FILE_OF_THE_DAY };
	CStatic	m_fileOfTheDayDescription;
	CListCtrl	m_fileOfTheDayListCtrl;
	//}}AFX_DATA

    virtual UINT  GetTemplateID(void) { return  IDD; }

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CFileOfTheDayPropertyPage)
	public:
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CFileOfTheDayPropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemchangedListFileOfTheDay(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    void InitializeColumnHeaders(void);
    void PopulateListCtrl(void);

    BOOL    GetFileOfTheDayRadioButtonValue(void);
    void    InitializeFileOfTheDayRadioButtons(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOTDPG_H__56F10F50_DA95_11D1_A7FB_0000E8D34392__INCLUDED_)