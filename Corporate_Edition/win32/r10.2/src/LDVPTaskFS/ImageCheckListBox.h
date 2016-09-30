// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ImageCheckListBox.h : header file
//

#ifndef __IMAGECHECKLISTBOX_H
#define __IMAGECHECKLISTBOX_H

/////////////////////////////////////////////////////////////////////////////
// CImageCheckListBox window

class CImageCheckListBox : public CCheckListBox
{
// Construction
public:
	int SetImageList(CImageList *pImageList);
	CImageCheckListBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageCheckListBox)
	public:
	virtual void OnFinalRelease();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImageCheckListBox();

	// Generated message map functions
protected:
	CImageList * m_pImageList;
	//{{AFX_MSG(CImageCheckListBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CImageCheckListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

#endif /* ndef __IMAGECHECKLISTBOX_H */

/////////////////////////////////////////////////////////////////////////////
