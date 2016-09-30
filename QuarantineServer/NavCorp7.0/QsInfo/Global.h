// Copyright 1999 Symantec Corporation
////////////////////////////////////////////////////////////////////////////
// Global.h created 2/9/1999
//
// $Header$
// 
// Description: Misc. global functions for the qsinfo32.dll. This code
//              was ported/borrowed from Atomic/Quar32.
//
// Contains:    
//
////////////////////////////////////////////////////////////////////////////

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

//
// Disable macro redfinition warning.  
// The LanMan header defines PATHLEN again for us.
//

#pragma warning ( disable : 4005 )
#include "lm.h"
#pragma warning ( default : 4005 )

BOOL    GetDomainName( LPSTR lpszDomainName, DWORD nBuffSize );
BOOL    IsWinNT( void );

//
// Function pointer for NT specific routine.
//
typedef NET_API_STATUS ( NET_API_FUNCTION *PFNNetWkstaGetInfo)(LPTSTR szServerName, DWORD level, LPBYTE* pBuffer);

#endif
