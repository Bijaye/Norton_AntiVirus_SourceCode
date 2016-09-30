// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// IdleOptionsPage.h : header file
//

#if !defined (IDLE_OPTIONS_INCLUDED)
#define IDLE_OPTIONS_INCLUDED

#include "WizardPage.h"

/////////////////////////////////////////////////////////////////////////////
// CIdleOptionsPage dialog

class CIdleOptionsPage : public CWizardPage
{
	DECLARE_DYNCREATE(CIdleOptionsPage)

private:
	BOOL m_bInitialized;

	void SetDialogOptions();
	void GetDialogOptions();
	void OnDestroying();

// Construction
public:
	CIdleOptionsPage();
	~CIdleOptionsPage();

// Dialog Data
	//{{AFX_DATA(CIdleOptionsPage)
	enum { IDD = IDD_IDLE_OPTIONS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CIdleOptionsPage)
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
	//{{AFX_MSG(CIdleOptionsPage)
	afx_msg void OnPaint();
	afx_msg void OnBrowseFileTypes();
	afx_msg void OnMessage();
	afx_msg void OnBrowseSelectedDrives();
	afx_msg void OnBrowseExcludeFiles();
	afx_msg void OnOnoff();
	afx_msg void OnTypesAll();
	afx_msg void OnTypesSelected();
	afx_msg void OnSelectedDrives();
	afx_msg void OnExcludeFiles();
	afx_msg void OnMessageBox();
	afx_msg void OnAllDrives();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
