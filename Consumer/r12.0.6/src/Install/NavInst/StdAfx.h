// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A7C2915D_1F34_4C27_943E_242432CE9082__INCLUDED_)
#define AFX_STDAFX_H__A7C2915D_1F34_4C27_943E_242432CE9082__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_MSI 110

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <atlbase.h>
extern CComModule _Module;
#include <atlapp.h>
#include <atlmisc.h>
#include <atlstr.h>
#include <list>
#include <tchar.h>
#include <stdexcept>
#include <crtdbg.h>
#include <regstr.h>	
#include <Msiquery.h>
#include <msi.h>
#include "ShellApi.h"
#include "shlobj.h"
#include "shfolder.h"

#include "resource.h"

using std::exception;
using std::runtime_error;

#include "OptNames.h"
#include "NavOpt32.h"
#include "NAVInfo.h"
#include "DebugLog.h"
#include "SymScriptSafe.h"						// ISymScriptSafe
#include "ccLib.h"


//Globals
extern HINSTANCE ghInstance;
extern CDebugLog g_Log;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A7C2915D_1F34_4C27_943E_242432CE9082__INCLUDED_)
