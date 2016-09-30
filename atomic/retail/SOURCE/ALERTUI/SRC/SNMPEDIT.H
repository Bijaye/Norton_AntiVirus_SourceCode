/////////////////////////////////////////////////////////////////////
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/snmpedit.h_v   1.0   04 Jun 1997 21:19:02   DBUCHES  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/snmpedit.h_v  $
// 
//    Rev 1.0   04 Jun 1997 21:19:02   DBUCHES
// Initial revision.
// 


#if !defined(AFX_SNMPEDIT_H__51407107_DD03_11D0_8AE6_0060977341AC__INCLUDED_)
#define AFX_SNMPEDIT_H__51407107_DD03_11D0_8AE6_0060977341AC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SNMPEdit.h : header file
//

#include "winsock.h"


/////////////////////////////////////////////////////////////////////////////
// CAddressEdit window

class CAddressEdit : public CEdit
{
// Construction
public:
	CAddressEdit();

// Attributes
public:
    BOOL    m_bAllowHex;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddressEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAddressEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAddressEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CSNMPEditDlg dialog

class CSNMPEditDlg : public CDialog
{
// Construction
public:
	CSNMPEditDlg(CWnd* pParent = NULL);   // standard constructor

    SOCKADDR    m_SockAddr;               // Address
    

// Dialog Data
	//{{AFX_DATA(CSNMPEditDlg)
	enum { IDD = IDD_SNMP_EDIT };
	CAddressEdit	m_NodeEdit;
	CAddressEdit	m_NetEdit;
	int		m_iProtocol;
	CString	m_sTargetName;
	CString	m_sNetNumber;
	CString	m_sNodeNumber;
	CString	m_sIP1;
	CString	m_sIP2;
	CString	m_sIP3;
	CString	m_sIP4;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSNMPEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSNMPEditDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeSnmpProtocols();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SNMPEDIT_H__51407107_DD03_11D0_8AE6_0060977341AC__INCLUDED_)
