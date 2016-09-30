// Copyright 1996 Symantec, Peter Norton Computing Group
// -------------------------------------------------------------------------
// STDAFX.H
// Standard includes for COM objects
//
// $Header:   S:/ABOUTSW/VCS/stdafx.h_v   1.1   13 Oct 1998 18:37:52   rchinta  $
// -------------------------------------------------------------------------
// $Log:   S:/ABOUTSW/VCS/stdafx.h_v  $
// 
//    Rev 1.1   13 Oct 1998 18:37:52   rchinta
// No Change.
// 
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
