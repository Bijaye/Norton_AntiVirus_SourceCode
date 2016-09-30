/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/ImageButton.h_v   1.0   27 Feb 1998 15:10:24   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// ImageButton.h : header file
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/ImageButton.h_v  $
// 
//    Rev 1.0   27 Feb 1998 15:10:24   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEBUTTON_H__3E560268_A6FB_11D1_A521_0000C06F46D0__INCLUDED_)
#define AFX_IMAGEBUTTON_H__3E560268_A6FB_11D1_A521_0000C06F46D0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CImageButton window

class CImageButton : public CButton
{
// Construction
public:
	CImageButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetImageIndex(int iIndex);
	void SetImageList(CImageList* pImageList);
	CImageList * m_pImageList;
	virtual ~CImageButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CImageButton)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_iIndex;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEBUTTON_H__3E560268_A6FB_11D1_A521_0000C06F46D0__INCLUDED_)
