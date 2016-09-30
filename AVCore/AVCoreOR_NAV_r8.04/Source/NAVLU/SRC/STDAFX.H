// Copyright 1996-1997 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/StdAfx.h_v   1.0   16 May 1997 11:13:04   JBRENNA  $
//
// Description:
//  Include file for standard system include files, or project specific
//  include files that are used frequently, but are changed infrequently.
//
//****************************************************************************
// $Log:   S:/NAVLU/VCS/StdAfx.h_v  $
// 
//    Rev 1.0   16 May 1997 11:13:04   JBRENNA
// Initial revision.
// 
//****************************************************************************

#define VC_EXTRALEAN                    // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>                     // MFC core and standard components
#include <afxext.h>                     // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
