// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CERTLIBN/VCS/stricomp.cpv   1.5   02 Feb 1996 20:56:36   RSTANEV  $
//
// Description:
//      Same as STRICMP(), but CERTLIBN version.
//
// See Also:
//************************************************************************
// $Log:   S:/CERTLIBN/VCS/stricomp.cpv  $
// 
//    Rev 1.5   02 Feb 1996 20:56:36   RSTANEV
// Added Unicode support.
//
//    Rev 1.4   29 Apr 1995 17:09:14   DALLEE
// Added LOADDS for 16-bit "C" functions since CERTLIBN is naughty with DS.
//
//    Rev 1.3   21 Dec 1994 19:11:32   DALLEE
// Compile as "C" for all platforms.
//
//    Rev 1.2   21 Dec 1994 13:33:50   JMILLARD
// ifdef c++ around extern (NLM builds as c even with cpp extension)
//
//    Rev 1.1   20 Dec 1994 17:46:16   BRAD
// Made extern 'C'
//
//    Rev 1.0   20 Dec 1994 17:12:24   BRAD
// Added to support CRC of long filename in FPROT.ASM
//************************************************************************

#include "platform.h"
#include "ctsn.h"

/*@API:**********************************************************************
@Declaration:
BOOL WINAPI CTSN_LOADDS STR4_ICOMPARE(LPTSTR lpszString1, LPTSTR lpszString2);

@Description:
This function is the same as STRICMP(), but used by CERTLIBN.

@Parameters:
$lpszString1$          String1 to compare.
$lpszString2$          String2 to compare.

@Returns: If equal, returns TRUE.  Otherwise FALSE.
@See:
@Include:   ctsn.h
@Compatibility: DOS DX WIN W32 VXD NLM
****************************************************************************/

#if defined(__cplusplus)
    extern "C"
#endif

BOOL WINAPI CTSN_LOADDS STR4_ICOMPARE(LPTSTR lpszString1, LPTSTR lpszString2)
{
    return((STRICMP(lpszString1, lpszString2) == 0) ? TRUE : FALSE);
}

