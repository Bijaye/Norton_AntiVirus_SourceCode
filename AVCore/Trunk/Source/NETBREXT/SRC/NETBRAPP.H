// NetBrApp.h : main header file for the NETBRAPP application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "NetAppRC.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNetBrAppApp:
// See NetBrApp.cpp for the implementation of this class
//

class CNetBrAppApp : public CWinApp
{
public:
	CNetBrAppApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetBrAppApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNetBrAppApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
