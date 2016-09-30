// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ExcludePatternsDlg.h : header file
//
//#include "ImageCheckListBox.h"

/////////////////////////////////////////////////////////////////////////////
// CExcludePatternsDlg dialog

class CExcludePatternsDlg : public CDialog
{

private:
	
	CStringList			*m_PatternsList;
	CStringList			*m_SelPatternsList;
	//CImageList			*m_ImageList;

// Construction
public:
	CExcludePatternsDlg(CWnd* pParent = NULL);   // standard constructor

	void SetPatterns( CStringList *lstAll, CStringList *ptrList );

// Dialog Data
	//{{AFX_DATA(CExcludePatternsDlg)
	enum { IDD = IDD_EXCLUDE_PATTERNS };
	CCheckListBox	m_ctrlPatterns;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExcludePatternsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExcludePatternsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
