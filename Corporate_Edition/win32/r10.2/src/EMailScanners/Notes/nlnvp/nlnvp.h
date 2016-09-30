// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/
//	Lotus Notes extension library definitions

#ifndef _NLNVP_H_
#define _NLNVP_H_

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

// Notes API header files

#include <global.h>
#include <globerr.h>
#include <names.h>
#include <stdnames.h>                   /* FIELD_FORM, etc */
#include <osfile.h>
#include <nsf.h>
#include <kfm.h>                        /* SECKFMUserInfo */
#include <nsfdb.h>
#include <nsfnote.h>
#include <nsfsearc.h>
#include <nsferr.h>
#include <ostime.h>
#include <osmem.h>
#include <ods.h>
#include <editods.h>
#include <osenv.h>                      /* OSGetEnvironmentString */
#include <fontid.h>
#include <osmisc.h>
#include <misc.h>
#include <mailserv.h>
#include <mail.h>
#include <textlist.h>
#include <nif.h>
#include <idtable.h>

// NavCorp headers

// TODO: Defining these here is an ugly hack to work-around a build problem with "vpcommon.h" and <xlocale>
#define AC_MOVE				1	// Corresponds to "Quarantine infected file"
#define AC_RENAME			2	// apparently no longer used but still referenced in the code since it's difficult to be sure it's not being used in some obscure way
#define AC_DEL				3	// Corresponds to a "Delete infected file" action
#define AC_NOTHING			4	// Corresponds to a "Leave alone (log only)" action
#define AC_CLEAN			5	// Corresponds to a "Clean virus from file" action
#define ERROR_FUNCTION_NOT_SUPPORTED   0x2000002b

//#include "vpcommon.h"
#include "clientReg.h"

#include "debug.h"			// included so any file can debug
#include "fileinfo.h"		// included for interface below

/*---------------------------------------------------------------------------*/

#define SCAN_TIMEOUT		30000
#define TEMP_BODY_ITEM		_T("TempBody")

/*---------------------------------------------------------------------------*/
//
// Global data
//

extern BOOL g_bMailContextRegistered;

/*---------------------------------------------------------------------------*/
//
// Global functions
//

// nlnvp.c
void MakeTempFileName(LPTSTR pszPath, LPTSTR pszPrefix, DWORD index, LPTSTR pszExt, LPTSTR pszFilePath, DWORD dwFilePathSize);
void TranslateToLMBCS(char *source, char *dest, WORD maxDest);

// notifications.c
void StripDirInfo(LPTSTR pszDest, LPTSTR pszSrc, int maxLen);
DWORD DoNotifications(PFILE_INFO pInfectionList);

// mailcontext.c
BOOL ProcessHookedNote(DBHANDLE hDB, NOTEHANDLE hNote, char *szUserName);

#endif	// _NLNVP_H_

/*--- end of header ---*/
