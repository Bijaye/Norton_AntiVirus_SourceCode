//************************************************************************
// Copyright 1997 Symantec Corporation
// _______________________________________________________________________
//
// $Header:   S:/NSPLUGIN/VCS/STDAFX.H_v   1.0   21 May 1997 16:38:50   MDUNN  $
//
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
//************************************************************************
//  $Log:   S:/NSPLUGIN/VCS/STDAFX.H_v  $
// 
//    Rev 1.0   21 May 1997 16:38:50   MDUNN
// Initial revision.
//************************************************************************

#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#include <afxpriv.h>        // Private help-related stuff
#include <winreg.h>         // Registry #defines and functions

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT




