// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// S32UI.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "S32UI.h"
#include "rfile.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CS32UIApp

BEGIN_MESSAGE_MAP(CS32UIApp, CWinApp)
	//{{AFX_MSG_MAP(CS32UIApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CS32UIApp construction

CS32UIApp::CS32UIApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CS32UIApp object

CS32UIApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CS32UIApp initialization

BOOL CS32UIApp::InitInstance()
{
  CProcessor processor;

  processor.DoWork();
  
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

