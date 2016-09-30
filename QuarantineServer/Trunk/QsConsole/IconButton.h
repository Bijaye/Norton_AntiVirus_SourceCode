/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

#if !defined(AFX_ICONBUTTON_H__BFCF8434_C10E_11D2_8F46_3078302C2030__INCLUDED_)
#define AFX_ICONBUTTON_H__BFCF8434_C10E_11D2_8F46_3078302C2030__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IconButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIconButton window

class CIconButton : public CButton
{
// Construction
public:
	CIconButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIconButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetIcon( HICON hIcon );
	virtual ~CIconButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIconButton)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
    HICON m_hIcon;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICONBUTTON_H__BFCF8434_C10E_11D2_8F46_3078302C2030__INCLUDED_)
