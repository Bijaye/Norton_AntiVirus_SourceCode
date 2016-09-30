// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__6C4AD2C1_D398_448A_B13E_E717F2E3372E__INCLUDED_)
#define AFX_STDAFX_H__6C4AD2C1_D398_448A_B13E_E717F2E3372E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

/* Uncomment this block if you want to debug ref counts. It severely impacts
   performance so I left it commented out.

#ifdef _DEBUG   // For debugging ref counts!
    #define _ATL_DEBUG_INTERFACES
#endif
*/

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlstr.h>
#undef SubclassWindow 
#include <atlwin.h>

// For CComBSTR and OLE2T conversions
#include <atlconv.h>
#include <atlctl.h> // For IObjectSafetyImpl

#define PLUG_ORDER	200		// This indicates our plugin's position in the NSW
                            // integrator's list of products.

// We don't have/use SymExcept, so redefine this macro to propogate exceptions
// up the call stack.

// Toolbox headers
//
#include "NAVInfo.h"
#include "OSInfo.h"
#include "StahlSoft.h"
#include "DebugLog.h"
#include "ccModuleID.h" // Defines the module IDs for Common Error Handling
#include <map>
#include "AVRES.h"

// Global variables
#include "cltLicenseConstants.h"
#include "Globals.h"
#include "OptNames.h"
#include "cclibStd.h"
#include "ccCollection.h"

// Only need one of these objects, ever.
//
static CNAVInfo g_NAVInfo;
static COSInfo g_OSInfo;
extern CDebugLog g_Log;
extern bool g_bLicensingAlertRequestSent;
extern bool    g_bStandAlone;
extern CString g_csProductName;

// Misc defines.
//#define REFRESH_AP_STATUS_EVENT_NAME    _T("SYM_REFRESHAPSTATUS")
#define UWM_DETAILS_DLG_HELP            (WM_APP + 38)
#define UWM_MORE_DETAILS_CLICKED        (WM_APP + 39)

HINSTANCE _getPrimaryResourceInstance();

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6C4AD2C1_D398_448A_B13E_E717F2E3372E__INCLUDED)
