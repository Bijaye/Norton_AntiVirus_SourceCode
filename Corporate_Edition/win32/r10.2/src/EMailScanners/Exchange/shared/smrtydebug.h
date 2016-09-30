// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef _SMRTYDEBUG_H_
#define _SMRTYDEBUG_H_

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#define SMRTY_DEBUG

extern DWORD g_debug;

/*---------------------------------------------------------------------------*/

// these values were lifted from smrtycodes.h in the QA project MsgViewer
// we need to find a common folder to share header and source files

// these are the actual major/minor codes
#define SM_MSG_GENERAL          0       // major func code

#define SM_MSG_WARNING			1       // major func code

#define SM_MSG_ERROR			2       // major func code

// these are special codes that encode the minor/major into a DWORD
#define SM_GENERAL					MAKELONG(0, SM_MSG_GENERAL)
#define SM_WARNING					MAKELONG(0, SM_MSG_WARNING)
#define SM_ERROR					MAKELONG(0, SM_MSG_ERROR)

// end of lifted code

/*---------------------------------------------------------------------------*/
//
// Global functions
//

#ifdef SMRTY_DEBUG
void DebugOut(DWORD level, LPSTR pszFormat, ...);
#else
inline DebugOut(DWORD /* level */, LPSTR /*pszFormat */, ...) { }
#endif

#endif	// _SMRTYDEBUG_H_

/*--- end of header ---*/
