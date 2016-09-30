// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef __CLDVPPROPSHEET
#define __CLDVPPROPSHEET

// LDVPPropSheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLDVPPropSheet

class CLDVPPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CLDVPPropSheet)

// Construction
public:
	CLDVPPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CLDVPPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLDVPPropSheet)
	public:
	virtual int DoModal();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLDVPPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLDVPPropSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif