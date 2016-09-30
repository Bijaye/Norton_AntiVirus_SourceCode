// Copyright 1998 Symantec Corporation. 
//*************************************************************************
// NAVWORK.cpp - created 03/09/1998 6:56:54 PM
//
// $Header:   S:/NAVWORK/VCS/NAVWORK.cpv   1.0   09 Mar 1998 23:45:38   DALLEE  $
//
// Description:
//      Implementaion of CNavworkApp. CWinApp based class for NAVWORK.DLL.
//
// Contains:
//      CNavworkApp
//*************************************************************************
// $Log:   S:/NAVWORK/VCS/NAVWORK.cpv  $
// 
//    Rev 1.0   09 Mar 1998 23:45:38   DALLEE
// Initial revision.
//*************************************************************************

#include "stdafx.h"
#include "n_app.h"
#include "navoptx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HINSTANCE hInstance;

//*****************************************************************************
// Global value to hold the extension list removed from the NAVOPTS structure
//*****************************************************************************
char gszExtensions[MAX_EXTENSION_LIST_SIZE+1];


/////////////////////////////////////////////////////////////////////////////
// CNavworkApp

BEGIN_MESSAGE_MAP(CNavworkApp, CWinApp)
	//{{AFX_MSG_MAP(CNavworkApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNavworkApp construction

CNavworkApp::CNavworkApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNavworkApp object

CNavworkApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CNavworkApp initialization

BOOL CNavworkApp::InitInstance()
{
    // TODO: Add your specialized code here and/or call the base class
    hInstance = m_hInstance;

    return ( CWinApp::InitInstance() );
}
