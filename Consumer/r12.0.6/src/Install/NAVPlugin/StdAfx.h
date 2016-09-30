// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A7C2915D_1F34_4C27_943E_242432CE9082__INCLUDED_)
#define AFX_STDAFX_H__A7C2915D_1F34_4C27_943E_242432CE9082__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include "ShellApi.h"
#include <tchar.h>
#include <stdexcept>
#include <crtdbg.h>

using std::exception;
using std::runtime_error;

// TODO: reference additional headers your program requires here
#include "NAVInfo.h"
#include "DebugLog.h"

//Globals
extern HINSTANCE ghInstance;
extern CDebugLog g_Log;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A7C2915D_1F34_4C27_943E_242432CE9082__INCLUDED_)
