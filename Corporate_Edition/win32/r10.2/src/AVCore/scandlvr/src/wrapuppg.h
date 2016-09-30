// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/WrapUpPg.h_v   1.9   28 Jul 1998 13:37:28   jtaylor  $
/////////////////////////////////////////////////////////////////////////////
//
// WrapUpPg.h: interface for the CWrapUpPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/WrapUpPg.h_v  $
// 
//    Rev 1.9   28 Jul 1998 13:37:28   jtaylor
// Added a final panel for all false positives.
// 
//    Rev 1.8   27 May 1998 21:37:10   SEDWARD
// Added support for more SARC errors.
//
//    Rev 1.7   19 May 1998 16:04:32   SEDWARD
// Added some more WRAPUP types.
//
//    Rev 1.6   27 Apr 1998 20:23:56   SEDWARD
// Added support for new messages: virus defs not paid, file of the day
// cancelled.
//
//    Rev 1.5   27 Apr 1998 02:08:34   SEDWARD
// Added OnInitDialog().
//
//    Rev 1.4   26 Apr 1998 17:35:38   SEDWARD
// Added member variables for static text controls, SetTextContent().
//
//    Rev 1.2   24 Apr 1998 18:23:30   SEDWARD
// Added logfile headers.
/////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_WRAP_UP_H__5D02B181_CCC9_11D1_A7DC_0000E8D34392__INCLUDED_)
#define AFX_WRAP_UP_H__5D02B181_CCC9_11D1_A7DC_0000E8D34392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WizPage.h"


// these are used to identify what string resources to use when calling "SetTextContent"
#define WRAPUP_TYPE_USER_CANCELLED                      1
#define WRAPUP_TYPE_GENERAL_FAILURE                     2
#define WRAPUP_TYPE_NO_FILES_ACCEPTED                   3
#define WRAPUP_TYPE_FILE_OF_THE_DAY_CANCEL              4
#define WRAPUP_TYPE_VIRUS_SUBSCRIPTION_NOT_PAID         5
#define WRAPUP_TYPE_SINGLE_REJECTED_FILE                6
#define WRAPUP_TYPE_INVALID_TIME_LAPSE                  7
#define WRAPUP_TYPE_SINGLE_COMPRESSED_FILE              8
#define WRAPUP_TYPE_SARC_ERROR_CREATE_PACKAGE           9
#define WRAPUP_TYPE_SARC_ERROR_CHECK_BACKEND_STATUS     10
#define WRAPUP_TYPE_SARC_ERROR_SEND_TO_SARC             11
#define WRAPUP_TYPE_ALL_FALSE_POSITIVES                 12




/////////////////////////////////////////////////////////////////////////////
// CWrapUpPropertyPage dialog

class CWrapUpPropertyPage : public CWizardPropertyPage
{
    DECLARE_DYNCREATE(CWrapUpPropertyPage)

// Construction
public:
    CWrapUpPropertyPage();
    ~CWrapUpPropertyPage();

// Dialog Data
    //{{AFX_DATA(CWrapUpPropertyPage)
	enum { IDD = IDD_WRAP_UP };
	CString	m_szText1;
	CString	m_szText2;
	//}}AFX_DATA

    virtual UINT  GetTemplateID(void) { return  IDD; }

// Overrides
	// ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CWrapUpPropertyPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


public:
    BOOL    SetTextContent(DWORD  nWrapUpType);


// Implementation
protected:
	// Generated message map functions
    //{{AFX_MSG(CWrapUpPropertyPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WRAP_UP_H__5D02B181_CCC9_11D1_A7DC_0000E8D34392__INCLUDED_)