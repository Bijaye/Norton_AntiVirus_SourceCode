// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__72DCA3EF_151B_4DB8_AE61_789C1703689F__INCLUDED_)
#define AFX_STDAFX_H__72DCA3EF_151B_4DB8_AE61_789C1703689F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_WINNT 0x0400

#define STRICT
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlctl.h> // For IObjectSafetyImpl

#include "OSInfo.h"     // Generic OS information, static
#include "NAVInfo.h"    // Contains useful about NAV, like install path
#include "DebugLog.h"

// Only need one of these objects, ever.
//
extern CNAVInfo g_NAVInfo;
extern COSInfo g_OSInfo;
extern bool g_bIsAdminProcess;

#import "ccEventManager.tlb"
#include "ccLib.h"
#include "ccCriticalSection.h"
#include "ccSingleLock.h"
#include "ccThread.h"
#include "ccMessageLock.h"
#include "ccEvent.h"

#include "StatusPropertyNames.h"
#include "EventData.h"
#include <memory>   // for auto_ptr

namespace AVStatus
{
    // These are dynamic properties used by the NAVStatus client
    //
    enum enumStatusClientProperties
    {
        propALUCanEnable = AVStatus::propLAST + 1000,
        propAPCanEnable,
        propEmailCanEnable,
		propSpywareCanEnable,
        propDefsAge,        // L - # days old - Dynamic, this value is not cached
        propFSSAge,         // L - # days old - Dynamic, this value is not cached

    };
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__72DCA3EF_151B_4DB8_AE61_789C1703689F__INCLUDED)
