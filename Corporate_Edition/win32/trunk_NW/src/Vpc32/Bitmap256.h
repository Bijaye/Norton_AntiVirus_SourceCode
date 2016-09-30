// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_BITMAP256_H__F8941371_684A_11D2_89AE_00A0C9749EEF__INCLUDED_)
#define AFX_BITMAP256_H__F8941371_684A_11D2_89AE_00A0C9749EEF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Bitmap256.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBitmap256 window

class CBitmap256 : public CStatic
{
// Construction
public:
	CBitmap256();

	void Draw(int x,int y,CDC* pDC);
	BOOL LoadBitmap(LPCSTR resID = NULL);
	BOOL LoadBitmap(UINT nID) { return LoadBitmap(MAKEINTRESOURCE(nID)); }

// Attributes
public:
	CPalette m_Palette;
	HBITMAP m_hBitmap;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmap256)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBitmap256();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBitmap256)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	BOOL	m_bLoaded;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAP256_H__F8941371_684A_11D2_89AE_00A0C9749EEF__INCLUDED_)
