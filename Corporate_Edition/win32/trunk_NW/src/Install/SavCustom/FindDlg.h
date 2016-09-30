// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_FINDDLG_H__9CBFE994_5F8C_11D3_A2B1_00C04F91B157__INCLUDED_)
#define AFX_FINDDLG_H__9CBFE994_5F8C_11D3_A2B1_00C04F91B157__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindDlg.h : header file
//

// These are the indexes of the search type within the find dialog if
// the user is searching by name.
#define	I_SEARCH_NTORNETWARESERVER	0
#define	I_SEARCH_NTSERVER			1
#define I_SEARCH_NETWARESERVER		2
#define I_SEARCH_CLIENT				3

#define I_ADDRESSOFFSET		4

// These are the indexes of the search type within the find dialog if
// the user is searching by address. I determine these by adding
// I_ADDRESSOFFSET to whatever the combobox index is if the address
// radio button is set.
#define	I_SEARCH_IPADDRESS	4
#define	I_SEARCH_IPXADDRESS	5

#define NAVCORPREG_ROOT			"SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion"
#define SERVEREXIST_VALUE		"ServerExists"
#define SERVERNAME_VALUE		"ServerName"


/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog

class CFindDlg : public CDialog
{
// Construction
public:
	CString GetReportedName();
	void FillComboList(int nType);
	void AddStringOnce(CString sNewString);
	CString m_sFoundComputer;
	CFindDlg(CWnd* pParent = NULL);   // standard constructor

private:
	void SaveServerInfo();
	BOOL m_bIsValidServer;

// Dialog Data
	//{{AFX_DATA(CFindDlg)
	enum { IDD = IDD_FINDCOMPUTER_DIALOG };
	CComboBox	m_ctlComputerType;
	CComboBox	m_ctlComputer;
	CString	m_strComputer;
	int		m_iRadioName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bFound;

	// Generated message map functions
	//{{AFX_MSG(CFindDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRadioaddress();
	afx_msg void OnRadioname();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDDLG_H__9CBFE994_5F8C_11D3_A2B1_00C04F91B157__INCLUDED_)
