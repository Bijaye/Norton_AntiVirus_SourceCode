// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef _NOTESINITIPC_H_
#define _NOTESINITIPC_H_

#pragma warning( disable: 4100 4115 4127 4201 4706 )	// Needed for /W4 and <windows.h>

//4100: unreferenced formal parameter
//4115:	named type definition in parentheses
//4127: conditional expression is constant
//4201: nameless unions
//4706	assignment within conditional expression

//4214: non 'int' bit fields
//4514: unreferenced inline functions

#include <windows.h>

/*---------------------------------------------------------------------------*/
//
// Global functions
//

DWORD DeInitIPC();
DWORD InitIPC();

#endif	// _NOTESINITIPC_H_

/*--- end of header ---*/
