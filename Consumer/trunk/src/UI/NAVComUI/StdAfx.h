////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x501
#endif
#define _ATL_APARTMENT_THREADED

#define _WIN32_IE	0x0500
#define WINVER		0x0500

//#define _ATL_DEBUG_INTERFACES

#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlcom.h>
#include <atlwin.h>
#include <atlhost.h>
#include <atlddx.h>
#include <atltypes.h>
#include <time.h>
#include <atlctl.h>
#include <atlctrls.h>
#include <atlframe.h>
#include <commctrl.h>

#include <string>
#include <vector>
#include <algorithm>

#include "ccLib.h"
#include "ccSymDebugOutput.h"

#include "StahlSoft.h"
#include "ComSysThreadBase.h"
#include "RunnableHostThread.h"

#include "Scaling.h"
#include "SymHelp.h"
#include "NAVLnch.h"
#include "NAVtstring.h"

/*
** Name of the Modules Resource DLL
*/ 

#include "ResResource.h"
#define SYMC_RESOURCE_DLL		_T("NAVComUI.loc")


extern CString _g_csHandlerPath;
extern ccSym::CDebugOutput g_DebugOutput;

//Used for pragma message outputs.
#undef STRIZE
#define STRIZE(x) #x
#undef PPSTRIZE
#define PPSTRIZE(x) STRIZE(x)
#define AUTO_FUNCNAME __FILE__ "(" PPSTRIZE(__LINE__) ") : " __FUNCTION__ ":  "

#ifndef _countof
#define _countof(x)(sizeof(x)/sizeof(*x))
#endif

#ifndef _S
#define _S(x)((CString)(LPCTSTR)x)
#endif
