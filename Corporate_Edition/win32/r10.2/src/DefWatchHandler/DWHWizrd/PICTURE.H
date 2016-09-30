// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998 - 2003, 2005 Symantec Corporation. All rights reserved. 
//*************************************************************************
// picture.h - created 03/05/1998 6:14:04 PM
//
// $Header:   S:/NAVWORK/VCS/picture.h_v   1.0   09 Mar 1998 23:47:48   DALLEE  $
//
// Description:
//      CPicture declaration. Repair Wizard picture display control.
//
// Contains:
//      CPicture
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/picture.h_v  $
// 
//    Rev 1.0   09 Mar 1998 23:47:48   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __PICTURE_H
#define __PICTURE_H

/////////////////////////////////////////////////////////////////////////////
// CPicture window

class CPicture : public CButton
{
// Construction
public:
	CPicture();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPicture)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SetBitmap(int iID, int iWidth, int iHeight, BOOL bDrawFrame = TRUE );
	virtual ~CPicture();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPicture)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
    int m_iBitmapID;
    int m_iBitmapWidth;
    int m_iBitmapHeight;
    BOOL m_bDrawFrame;
    CBitmap m_bitmap;

};


#endif // !__PICTURE_H
