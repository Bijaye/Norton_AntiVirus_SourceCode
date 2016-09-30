// Copyright 1996-1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/NavLuApp.h_v   1.6   10 Nov 1998 08:26:26   JBRENNA  $
//
// Description:
//   Main header file for the NAVLU application.
//
// Contains:
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLU/VCS/NavLuApp.h_v  $
// 
//    Rev 1.6   10 Nov 1998 08:26:26   JBRENNA
// Port PVENKAT from QAKN branch to trunk:
//   Added CNavLuApp::EnablePrivilege to enable 'Shutdown' privilege on NT.  This
//   will enable the application to call ExitWindowsEx successfully otherwise it
//   will fail.  On 95, Enableprivilege will do nothing.
// 
//    Rev 1.5   28 Aug 1998 16:21:14   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.4.2.2   10 Jul 1998 17:24:52   TCASHIN
//    Added ProcessSchedulerSettings().
// 2. Rev 1.4.2.1   01 Jul 1998 19:21:12   tcashin
//    Return an exit code of 1 if a reboot is needed.
// 
//    Rev 1.4   03 Jun 1997 14:00:58   JBRENNA
// Remove m_lpszRetryCountValue.
// 
//    Rev 1.3   01 Jun 1997 19:15:22   JBRENNA
// Give prototypes for new private functions.
// 
//    Rev 1.2   26 May 1997 14:57:24   JBRENNA
// Add CNavLuApp::DisplayFailMsgBox() such that it is displayed when LiveUpdate
// is not successful and we are in /scheduled mode.
// 
//    Rev 1.1   23 May 1997 21:35:14   JBRENNA
// m_CmdLineInfo member variable.
// 
//    Rev 1.0   16 May 1997 11:09:38   JBRENNA
// Initial revision.
// 
//    Rev 1.0   10 May 1997 01:21:08   JBRENNA
// Initial revision.
// 
//****************************************************************************

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "CmdLineInfo.h"

/////////////////////////////////////////////////////////////////////////////
// CNavLuApp:
// See NavLu.cpp for the implementation of this class
//

class CNavLuApp : public CWinApp
{
public:

    CNavLuCmdLineInfo m_CmdLineInfo;
    int               m_ExitCode;

    CNavLuApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNavLuApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNavLuApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Private Functions
private:
    BOOL  ProcessLuResult (WORD wLuResult);
    int   ForcedMsgBox (UINT uTitleId, UINT uMsgId, UINT uMsgBoxFlags) const;
    DWORD GetRetryCount () const;
    void  SetRetryCount (DWORD dwCount) const;
    void  RemoveRetryCount () const;
    void  ProcessSchedulerSettings();
    BOOL  EnablePrivilege (LPTSTR lpszPrivilege);

// Private Data
private:
};


/////////////////////////////////////////////////////////////////////////////
