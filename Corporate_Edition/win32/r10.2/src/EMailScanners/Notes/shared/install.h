// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef _NOTESINSTALL_H_
#define _NOTESINSTALL_H_

#pragma warning( disable: 4100 4115 4127 4201 4706 )	// Needed for /W4 and <windows.h>

//4100: unreferenced formal parameter
//4115:	named type definition in parentheses
//4127: conditional expression is constant
//4201: nameless unions
//4706	assignment within conditional expression

//4214: non 'int' bit fields
//4514: unreferenced inline functions

#include <windows.h>
#include <tchar.h>

/*---------------------------------------------------------------------------*/

#define NOTES_NOT_INSTALLED     0
#define NOTES4_INSTALLED        4
#define NOTES5_INSTALLED        5

#define SERVERSETUP			_T("ServerSetup")
#define CLIENTSETUP			_T("Setup")
#define NOTESSECTION		_T("Notes")
#define KITTYPE             _T("KitType")

#define KITTYPE_WORKSTATION 1       // Workstation only install
#define KITTYPE_SERVER      2       // Server only, or server plus workstation install


#define S_CLIENT			0x1
#define S_SERVER			0x2

/*---------------------------------------------------------------------------*/
//
// Global functions
//

DWORD IsNotesClient4Installed();
DWORD IsNotesClient5Installed(LPTSTR lpsNotesPath);
DWORD GetNotesInstallPath(LPTSTR installPath, DWORD maxLen);

#endif	// _NOTESINSTALL_H_

/*--- end of header ---*/
