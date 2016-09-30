// Copyright 1998 Symantec Corporation. 
//*************************************************************************
// n_app.h - created 03/09/1998 6:56:07 PM
//
// $Header:   S:/NAVWORK/VCS/n_app.h_v   1.0   09 Mar 1998 23:47:48   DALLEE  $
//
// Description:
//      main header file for the NAVWORK application
//
// Contains:
//      CNavworkApp
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/n_app.h_v  $
// 
//    Rev 1.0   09 Mar 1998 23:47:48   DALLEE
// Initial revision.
//*************************************************************************

#if !defined(AFX_NAVWORK_H__2FA3D9C7_A321_11D1_9103_00C04FAC114A__INCLUDED_)
#define AFX_NAVWORK_H__2FA3D9C7_A321_11D1_9103_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

/////////////////////////////////////////////////////////////////////////////
// CNavworkApp:
// See Navwork.cpp for the implementation of this class
//

class CNavworkApp : public CWinApp
{
public:
	CNavworkApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNavworkApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNavworkApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAVWORK_H__2FA3D9C7_A321_11D1_9103_00C04FAC114A__INCLUDED_)
