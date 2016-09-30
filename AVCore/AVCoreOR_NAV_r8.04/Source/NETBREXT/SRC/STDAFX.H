/////////////////////////////////////////////////////////////////////
// STDAFX.H - Created - 11/20/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NETBREXT/VCS/stdafx.h_v   1.0   21 Nov 1997 11:52:18   KSACKIN  $
//
// Description:
//      Standard AFX Include file.  This file will be the same for
//    most Visual C++ applications, therefore, we are going to check
//    this file into the global include area.  If you need a specific
//    version of this file, or if you wish to modify one for your
//    particular project, you should change the name of yours to a
//    file that only your project can include.
//
//    Microsoft Description:
//
//    stdafx.h : include file for standard system include files,
//    or project specific include files that are used frequently, but
//    are changed infrequently.
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NETBREXT/VCS/stdafx.h_v  $
// 
//    Rev 1.0   21 Nov 1997 11:52:18   KSACKIN
// Initial revision.
// 
//    Rev 1.0   14 May 1997 15:19:40   KSACKIN
// Initial revision.
// 
//    Rev 1.0   24 Apr 1997 20:15:48   JBRENNA
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:10:44   RFULLER
// Initial revision
// 
//    Rev 1.0   20 Nov 1996 02:11:58   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__STDAFX_H)
#define __STDAFX_H


//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#endif // if !defined(__STDAFX_H)


