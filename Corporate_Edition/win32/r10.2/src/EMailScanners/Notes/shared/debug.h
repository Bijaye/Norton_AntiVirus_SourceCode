// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef _SMRTYDEBUG_H_
#define _SMRTYDEBUG_H_

#define SMRTY_DEBUG

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
#include <tchar.h>

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

#ifdef SMRTY_DEBUG
	#define NOTESMSG(error)															\
		{																			\
		OSLoadString(NULL, ERR(error), g_szTextBuffer, sizeof(g_szTextBuffer)-1);	\
		DebugOut(SM_ERROR, g_szTextBuffer);											\
		}
#else
	#define NOTESMSG(error)
#endif


/*---------------------------------------------------------------------------*/

#ifdef SMRTY_DEBUG
	#define ASSERT0(Exp, STRING) if(!(Exp)) { DebugOut(SM_ERROR, STRING); __asm int 3 }
#else
	#define ASSERT0(Exp, STRING) 
#endif


/*---------------------------------------------------------------------------*/
//
// Global data
//

#define MAX_SM_MESSAGE			1024
extern char	g_szTextBuffer[MAX_SM_MESSAGE];
extern DWORD g_debug;


/*---------------------------------------------------------------------------*/
//
// Global functions
//

#ifdef SMRTY_DEBUG
void DebugOut(DWORD level, LPSTR pszFormat, ...);
#else
#define DebugOut(DWORD /* level */, LPSTR /*pszFormat */)
#endif

#endif	// _SMRTYDEBUG_H_

/*--- end of header ---*/