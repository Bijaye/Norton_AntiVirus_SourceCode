/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__6A0B66FE_FE02_11D2_9798_00C04F688464__INCLUDED_)
#define AFX_STDAFX_H__6A0B66FE_FE02_11D2_9798_00C04F688464__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#ifndef  _WIN32_DCOM
   #define _WIN32_DCOM
#endif

#include <afxwin.h>
#include <afxdisp.h>
#include <afxcmn.h>

#include <atlbase.h>
#include <afxtempl.h>
#include <afxole.h>
#include <afxdlgs.h>	//jhill

//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlwin.h>
#include <atlctl.h>

#include <atlsnap.h>    //jhill
#include "mmc.h"


#include "UtilPlus.h"	 
#include "util.h"
#include "localize.h"
#include "AvisHelp.h"
#include "InputRange.h"





//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6A0B66FE_FE02_11D2_9798_00C04F688464__INCLUDED)
