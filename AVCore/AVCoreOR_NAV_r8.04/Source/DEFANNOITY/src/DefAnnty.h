// Copyright 1998 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/DEFANNOITY/VCS/DefAnnty.h_v   1.1   08 Apr 1998 19:45:28   RStanev  $
//
// Description:
//   Local header file for the DEFANNTY DLL.
//
//****************************************************************************
// $Log:   S:/DEFANNOITY/VCS/DefAnnty.h_v  $
// 
//    Rev 1.1   08 Apr 1998 19:45:28   RStanev
// Merged branch changes.
// 
//    Rev 1.0.1.1   08 Apr 1998 00:51:56   RStanev
// lotsa things.
// 
//    Rev 1.0.1.0   29 Mar 1998 01:15:06   RStanev
// Branch base for version QAKG
// 
//    Rev 1.0   29 Mar 1998 01:14:32   RStanev
// Initial revision.
// 
//****************************************************************************

#if !defined(AFX_DEFANNTY_H__442F07A6_C50B_11D1_AE4A_006097959B77__INCLUDED_)
#define AFX_DEFANNTY_H__442F07A6_C50B_11D1_AE4A_006097959B77__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

// Enum that describes the panels
enum SUB_PANEL { PANEL_FREEWARN = 0, PANEL_FREEEXP, PANEL_PAIDWARN, PANEL_PAIDEXP,
					PANEL_SUBINFO, PANEL_SUB, PANEL_FINALSUB, PANEL_FINALPATCH,
                    PANEL_FINALALL, PANEL_FINALTRY, PANEL_VENICE };

enum SUB_RESULT { RESULT_ERROR = 0, RESULT_PATCHES,
                  RESULT_DOWNLOAD, RESULT_SUBSCRIBED, RESULT_CANCEL };

BOOL IsVeniceMode();
LONG DefAnnuitySetSubsID ( LPCTSTR szValue );
LONG DefAnnuityGetSubsID ( LPTSTR szValue,  int iLength);
LONG DefAnnuitySetSubsXSKU ( LPCTSTR szValue );
LONG DefAnnuityGetSubsXSKU ( LPTSTR szValue,  int iLength);
LONG DefAnnuitySetSubsXPRO ( LPCTSTR szValue );
LONG DefAnnuityGetSubsXPRO ( LPTSTR szValue,  int iLength);
LONG DefAnnuitySetInetEnable ( DWORD dwValue );
LONG DefAnnuityGetInetEnable ( DWORD *pdwValue );

/////////////////////////////////////////////////////////////////////////////
// CDefAnntyApp
// See DefAnnty.cpp for the implementation of this class
//

class CDefAnntyApp : public CWinApp
{
public:
	CDefAnntyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefAnntyApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDefAnntyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFANNTY_H__442F07A6_C50B_11D1_AE4A_006097959B77__INCLUDED_)
