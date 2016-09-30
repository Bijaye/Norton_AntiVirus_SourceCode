////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// FixNAV.h : main header file for the FIXNAV application
//

#if !defined(AFX_FIXNAV_H__E30D7197_9247_4A3D_B4F6_05CB85FA82DA__INCLUDED_)
#define AFX_FIXNAV_H__E30D7197_9247_4A3D_B4F6_05CB85FA82DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFixNAVApp:
// See FixNAV.cpp for the implementation of this class
//

class CFixNAVApp : public CWinApp
{
public:
	CFixNAVApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixNAVApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFixNAVApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIXNAV_H__E30D7197_9247_4A3D_B4F6_05CB85FA82DA__INCLUDED_)
