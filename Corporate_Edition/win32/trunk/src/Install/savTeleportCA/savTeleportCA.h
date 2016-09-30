// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SavTeleportCA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SavTeleportCA_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
/*#ifdef SavTeleportCA_EXPORTS
#define SavTeleportCA_API __declspec(dllexport)
#else
#define SavTeleportCA_API __declspec(dllimport)
#endif*/

///////////////////////////////////////////////////////////////////////////////
//
//  Preprocessor
//
//  NOTE: the dllexport attribute eliminates the need for a .DEF file
//
///////////////////////////////////////////////////////////////////////////////
#include <msi.h>
#include <msiquery.h>

#define array_sizeof(x) (sizeof(x) / sizeof(x[0]))
#define SESSIONID "SESSIONID"

UINT _stdcall GetCurrentState ( MSIHANDLE hInstall );
UINT _stdcall LaunchasUser ( MSIHANDLE hInstall );

UINT MsiLogMessage( MSIHANDLE hInstall, LPCTSTR szString );
