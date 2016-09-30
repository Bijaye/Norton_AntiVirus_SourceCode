//************************************************************************
// Copyright 1997 Symantec Corporation
// _______________________________________________________________________
//
// $Header:   S:/NSPLUGIN/VCS/NPLUGIN.H_v   1.1   09 Jun 1997 13:59:22   MDUNN  $
//
// Description: Main header file for the NPLUGIN app.
//
//************************************************************************
//  $Log:   S:/NSPLUGIN/VCS/NPLUGIN.H_v  $
// 
//    Rev 1.1   09 Jun 1997 13:59:22   MDUNN
// Made the various state variables global, instead of having them be member
// variables of the app class.  This makes the code way more readable and
// intuitive.
// Added the /r and /n switches, with supporting code.
// Added in logic to handle the case where NAV isn't installed (the only
// thing we can do is uninstall NAV as a plug-in).
// 
//    Rev 1.0   21 May 1997 16:35:02   MDUNN
// Initial revision.
//************************************************************************

#ifndef __NPLUGIN_H__
#define __NPLUGIN_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// Constants used to make a return value from the dialog.

const int  NSP_CANCEL              = 0;
const int  NSP_SAVE                = 1;
const int  NSP_NONE_CHECKED        = 0;
const int  NSP_SOME_OR_ALL_CHECKED = 2;
const int  NSP_SOME_CHECKED        = 0;
const int  NSP_ALL_CHECKED         = 4;
const int  NSP_ERROR_SAVING        = 8;


/////////////////////////////////////////////////////////////////////////////
// CNPICommandLineInfo:  Derived from CCommandLineInfo to parse command
// line flags and set up the app's flags.

class CNPICommandLineInfo : public CCommandLineInfo
{
public:
    CNPICommandLineInfo();
    
    void ParseParam ( LPCSTR lpszParam, BOOL bFlag, BOOL bLast );
};

/////////////////////////////////////////////////////////////////////////////
// CNPlugInApp:
// See NPlugIn.cpp for the implementation of this class
//

class CNPlugInApp : public CWinApp
{
public:
	CNPlugInApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNPlugInApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNPlugInApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif  // ndef __NPLUGIN_H__
