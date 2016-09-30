////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__033717B5_914F_4B57_95F3_286A9C3C4E81__INCLUDED_)
#define AFX_STDAFX_H__033717B5_914F_4B57_95F3_286A9C3C4E81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0400
#include <atlbase.h>
#include <atlstr.h>
#include <atlpath.h>
#include <atlconv.h>
#include <comdef.h>
#include <time.h>

#include "navtstring.h"

#include "stahlsoft.h"
#include "CommandLine.h"

#include "WaitForShutdown.h"
extern CWaitForShutdown g_Shutdown;

// Include ccLib
#include "cclib.h"
extern CString _g_csHandlerPath;


#define REG_SUCCEEDED(rc) (rc == ERROR_SUCCESS)
#define REG_FAILED(rc) (rc != ERROR_SUCCESS)
// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__033717B5_914F_4B57_95F3_286A9C3C4E81__INCLUDED_)
