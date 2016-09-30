////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/global.h_v   1.5   22 Jun 1998 20:49:40   jtaylor  $
////////////////////////////////////////////////////////////////////////////
//
// QuarantineDLL.h - Contains interface definitions for Quarantine DLL
//
//
//
//
////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/global.h_v  $
// 
//    Rev 1.5   22 Jun 1998 20:49:40   jtaylor
// Added support for setting the Quarantine bloodhound level to 3.
// 
//    Rev 1.4   11 Jun 1998 20:41:22   DBuches
// Fixed problems with getting domain name.  We now run-time link with 
// the NT LanMan routines for getting the domain name.
// 
//    Rev 1.3   11 Jun 1998 13:45:32   DBuches
// Added code to save alternate data streams in TransferFile().
// 
//    Rev 1.2   08 Jun 1998 19:48:04   SEDWARD
// Added NumFilesInQuarantine().
//
//    Rev 1.1   18 May 1998 21:22:26   jtaylor
// Removed duplicate code, improved modularity and readability.
//
//    Rev 1.0   15 May 1998 18:47:16   jtaylor
// Initial revision.
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

HRESULT GetFileSecurityDesc( LPSTR lpszFileName, LPBYTE *pDesc );
BOOL    GetDomainName( LPSTR lpszDomainName, DWORD nBuffSize );
BOOL    IsFileCompressed( LPSTR lpszFileExtension );
BOOL    IsWinNT( void );
HRESULT SetFileSecurityDesc( LPSTR lpszFileName, LPBYTE pDesc );
HRESULT TransferFile( LPCTSTR lpszSourceName, HANDLE hSource, LPCTSTR lpszDestName, HANDLE hDest );
int     NumFilesInQuarantine(void);
VOID    WINAPI QuarantineGetConfigInfo( LPTSTR lpszID, LPVOID lpvResult, UINT uMaxResultLen, LPBOOL lpbImplemented, LPVOID lpvCookie );

//
// Function pointer for NT specific routine.
//
typedef NET_API_STATUS ( NET_API_FUNCTION *PFNNetWkstaGetInfo)(LPTSTR szServerName, DWORD level, LPBYTE* pBuffer);


#endif
