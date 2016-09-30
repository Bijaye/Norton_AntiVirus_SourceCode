////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// FixNAVDlg.h : header file
//

#if !defined(AFX_FIXNAVDLG_H__2ACFED81_7A62_453F_9EEC_F9B8F837D6BB__INCLUDED_)
#define AFX_FIXNAVDLG_H__2ACFED81_7A62_453F_9EEC_F9B8F837D6BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////// 
// disable warning C4786: symbol greater than 255 character,
// okay to ignore, MSVC chokes on STL templates!
#pragma warning(disable: 4786)

#include <list>
#include <string>
#include <iterator>

typedef std::list <std::string> stringlist;

/////////////////////////////////////////////////////////////////////////////
// CFixNAVDlg dialog

class CFixNAVDlg : public CDialog
{
// Construction
public:
	CFixNAVDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CFixNAVDlg)
	enum { IDD = IDD_FIXNAV_DIALOG };
	CButton	m_btnFix;
	CButton	m_btnExit;
	CButton	m_btnSave;
	CEdit	m_edtLog;
	CString	m_strLog;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixNAVDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	DWORD RegisterModule ( LPCTSTR lpszModulePath, HRESULT* phrExtendedError );
	HICON m_hIcon;

    stringlist m_listModules;

    enum RegisterError
    {
        SUCCESS = 0,            // Registered!
        FAIL_FILE_NOT_FOUND,    // Module isn't there
        FAIL_LOAD,              // Failed to load the file
        FAIL_ENTRY,             // No exported function
        FAIL_REG,               // Failed during DLLRegisterServer
    };

	// Generated message map functions
	//{{AFX_MSG(CFixNAVDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnFix();
	afx_msg void OnExit();
	afx_msg void OnSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIXNAVDLG_H__2ACFED81_7A62_453F_9EEC_F9B8F837D6BB__INCLUDED_)
