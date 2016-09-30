// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef _NOTESEXT_H_
#define _NOTESEXT_H_

#pragma warning( disable: 4100 4115 4127 4201 4706 )	// Needed for /W4 and <windows.h>

//4100: unreferenced formal parameter
//4115:	named type definition in parentheses
//4127: conditional expression is constant
//4201: nameless unions
//4706	assignment within conditional expression

//4214: non 'int' bit fields
//4514: unreferenced inline functions

// OS header files

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <io.h>
#include <tchar.h>
#include <fcntl.h>

// reset since windows.h resets some of these...
#pragma warning( disable: 4100 4115 4127 4201 4706 )	// Needed for /W4 and <windows.h>

// NavCorp headers

#include "vpcommon.h"
#include "clientReg.h"

#include "debug.h"			// included so any file can debug

#include "resource.h"

/*---------------------------------------------------------------------------*/

#define LOTUS_NOTES				_T("LotusNotes")
#define NSE_SIGNATURE			_T('VESN')
#define MAX_REALTIME_THREADS	2

/*---------------------------------------------------------------------------*/
//
// Global data
//

extern HINSTANCE	g_hResource;
extern HANDLE		g_hShutdown;
extern HANDLE       g_hUserAccessToken;

/*---------------------------------------------------------------------------*/
//
// Global functions
//

// notesext.c
void GetExtension(LPTSTR szFileName, LPTSTR szExt);
void GetFakeSID(mSID * SID);
void HookNotesIfNeeded( HANDLE userAccessToken );

#endif	// _NOTESEXT_H_

/*--- end of header ---*/
