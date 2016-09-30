// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the NAVLOGON_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// NAVLOGON_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef NAVLOGON_EXPORTS
#define NAVLOGON_API __declspec(dllexport)
#else
#define NAVLOGON_API __declspec(dllimport)
#endif

#ifndef _NAVLOGON_INCLUDED
#define _NAVLOGON_INCLUDED

#include <WinWlx.h>

#if defined __cplusplus
extern "C"
{
#endif

NAVLOGON_API VOID NavStartShellEvent (PWLX_NOTIFICATION_INFO pInfo);
NAVLOGON_API VOID NavLogoffEvent (PWLX_NOTIFICATION_INFO pInfo);



#if defined __cplusplus
}
#endif

#endif