// Copyright 1996 Symantec, Peter Norton Computing Group
// -------------------------------------------------------------------------
// STDAFX.H
// Standard includes for COM objects
//
// $Header:   S:/NAVABOUT/VCS/stdafx.h_v   1.1   08 Dec 1997 15:54:58   jtaylor  $
// -------------------------------------------------------------------------
// $Log:   S:/NAVABOUT/VCS/stdafx.h_v  $
// 
//    Rev 1.1   08 Dec 1997 15:54:58   jtaylor
// Made changes required to upgrade to vc 5.0 and coresdk.005
//
//    Rev 1.0   19 Aug 1997 14:09:44   KSACKIN
// Initial revision.
//
//    Rev 1.0.1.0   12 Aug 1997 22:44:50   DALVIDR
// Branch base for version QAKG
//
//    Rev 1.0   06 Jun 1997 20:02:08   BMCCORK
// Initial revision.
//
//    Rev 1.0   16 May 1997 18:28:30   JLAMB
// Initial revision.
//
//    Rev 1.1   28 Oct 1996 21:17:46   JLAMB
//
//    Rev 1.0   28 Oct 1996 20:15:16   JLAMB
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#ifndef __STDAFX_H__
#define __STDAFX_H__

#define STRICT

#define _INC_OLE
#define CONST_VTABLE

#if ( defined _WIN32 && !defined SYM_WIN32 )
	#define SYM_WIN32
#endif

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>
#include <shellapi.h>
#include <ole2.h>
#include <shlobj.h>
#include <commctrl.h>
#include <winnetwk.h>
#include <windowsx.h>

#pragma comment( lib, "advapi32.lib" )
#pragma comment( lib, "comctl32.lib" )

#define NULL_STR	_T( "" )

// Global Variables

extern LONG g_cLock;					// Used by the LockContainer
										// LockServer members

#endif // __STDAFX_H__
