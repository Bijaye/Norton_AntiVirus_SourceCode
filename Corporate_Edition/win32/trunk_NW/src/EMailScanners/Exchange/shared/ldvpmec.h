// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef __LDVPMEC_H__
#define __LDVPMEC_H__

#pragma warning(disable: 4100 4115 4127 4201 4706)	// Needed for /W4 and <windows.h>

//4100: unreferenced formal parameter
//4115:	named type definition in parentheses
//4127: conditional expression is constant
//4201: nameless unions
//4706	assignment within conditional expression

//4512: assignment operator could not be generated

//4214: non 'int' bit fields
//4514: unreferenced inline functions

#include <windows.h>

// need to set again, since windows.h resets some of these
#pragma warning(disable: 4100 4115 4127 4201 4706)	// Needed for /W4 and <windows.h>

#include <tchar.h>

#include "vpcommon.h"
#include "ClientReg.h"

#include "smrtydebug.h"

/*---------------------------------------------------------------------------*/

// General macros

#ifndef DllExport
	#define DllExport __declspec(dllexport)
#endif

#define MEC_SIGNATURE		'VCEM'
#define MEC_VERSION				1
#define MAX_REALTIME_THREADS	2

/*---------------------------------------------------------------------------*/

#include "helpers.h"
#include "memexcp.h"
#include "storage.h"

/*---------------------------------------------------------------------------*/

// .DLL Wide Globals

extern TCHAR
	g_szComputerName[MAX_COMPUTERNAME_LENGTH + 1];	// Computer name

extern HINSTANCE
	g_hResInstance;						// Resource Instance

extern OSVERSIONINFO
	g_osVersionInfo;					// OS Version Info (from GetOSVersionInfoEx())

enum PERSONALITES { Unknown_Personality = 0, SE_Personality, MEC_Personality };

extern PERSONALITES
	g_personality;

#endif //__LDVPMEC_H__