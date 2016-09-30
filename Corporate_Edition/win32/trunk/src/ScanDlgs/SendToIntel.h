// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_SENDTOINTEL_H__B6498E8F_AC63_11D1_AB23_00A0C90F8F6F__INCLUDED_)
#define AFX_SENDTOINTEL_H__B6498E8F_AC63_11D1_AB23_00A0C90F8F6F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SendToIntel.h : header file
//

#include "time.h"
#include "fcntl.h"
#include "io.h"
#include "Resource.h"


#define STI_EMAIL 1
#define STI_SMAIL 2
#define STI_INFOOK 4

typedef struct {
	DWORD Flags;
	DWORD Time;
	char Name[128];
	char Email[128];
	char Address[64];
	char City[64];
	char State[16];
	char Zip[16];
	char Description[1024];
	} USERDATAUPLOADBLOCK;


/////////////////////////////////////////////////////////////////////////////
// CSendToIntel dialog

class CSendToIntel : public CDialog
{
// Construction
public:
	DWORD AttachData(char *FileName);
	IMessages * Messages;
	DWORD Debug(IMessages *mess);
	~CSendToIntel();
	USERDATAUPLOADBLOCK UserData;
	IFTP * Ftp;
	CSendToIntel(CWnd* pParent = NULL);   // standard constructor
	DWORD m_Flags;

// Dialog Data
	//{{AFX_DATA(CSendToIntel)
	enum { IDD = IDD_SENDTOINTEL };
	CString	m_Name;
	CString	m_City;
	CString	m_Description;
	CString	m_Email;
	CString	m_State;
	CString	m_Zip;
	CString	m_Address;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendToIntel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSendToIntel)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDTOINTEL_H__B6498E8F_AC63_11D1_AB23_00A0C90F8F6F__INCLUDED_)
