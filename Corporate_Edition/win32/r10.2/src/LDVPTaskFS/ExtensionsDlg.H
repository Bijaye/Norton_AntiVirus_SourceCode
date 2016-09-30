// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef __EXTENSIONDLG
#define __EXTENSIONDLG

// ExtensionsDlg.h : header file
//

#if !defined( __SRVCON ) && !defined( __CLNTCON )
#define CVPString	CString
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtensionsDlg dialog

class CExtensionsDlg : public CDialog
{

// Construction
public:
	CExtensionsDlg(CWnd* pParent = NULL);   // standard constructor
	~CExtensionsDlg();

// Dialog Data
	//{{AFX_DATA(CExtensionsDlg)
	enum { IDD = IDD_EXTENSIONS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CStringList	m_extensions;
	CVPString m_description;

	void GetExtensions( CStringList *ptrList );
	void SetExtensions( CStringList *ptrList );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtensionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExtensionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeExtension();
	afx_msg void OnSelchangeExtensionList();
	afx_msg void OnExtensionExecutable();
	afx_msg void OnExtensionRemove();
	afx_msg void OnExtensionDocuments();
	afx_msg void OnExtensionAdd();
	virtual void OnOK();
	afx_msg void OnDefaults();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
