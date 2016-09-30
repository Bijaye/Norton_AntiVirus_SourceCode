// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ScanOptionsPage.h : header file
//
#if !defined (SCAN_OPTIONS_INCLUDED)
#define SCAN_OPTIONS_INCLUDED

#include "WizardPage.h"


/////////////////////////////////////////////////////////////////////////////
// CScanOptionsPage dialog

class CScanOptionsPage : public CWizardPage
{
	DECLARE_DYNCREATE(CScanOptionsPage)
private:
	BOOL m_bInitialized;

	void GetDialogOptions();
	void SetDialogOptions();
	void OnDestroying();

// Construction
public:
	CScanOptionsPage();
	~CScanOptionsPage();

// Dialog Data
	//{{AFX_DATA(CScanOptionsPage)
	enum { IDD = 0 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CScanOptionsPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CScanOptionsPage)
	afx_msg void OnPaint();
	afx_msg void OnBrowseFileTypes();
	afx_msg void OnMessage();
	afx_msg void OnBrowseExcludeFiles();
	afx_msg void OnTypesAll();
	afx_msg void OnTypesSelected();
	afx_msg void OnExcludeFiles();
	afx_msg void OnMessageBox();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
