// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__762CDD8F_FDB6_4DFF_B204_D09F9C565BB7__INCLUDED_)
#define AFX_STDAFX_H__762CDD8F_FDB6_4DFF_B204_D09F9C565BB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <atlbase.h>
#include <atlstr.h>
extern CComModule _Module;
#include <atlstr.h>
#include <comdef.h>
#include "StahlSoft.h"
extern HMODULE g_hModule;
extern CString g_sModuleLocation;
extern CString g_sModuleFileName;

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__762CDD8F_FDB6_4DFF_B204_D09F9C565BB7__INCLUDED_)
