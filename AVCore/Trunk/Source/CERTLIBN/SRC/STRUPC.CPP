// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CERTLIBN/VCS/strupc.cpv   1.7   03 Dec 1996 14:01:52   MKEATIN  $
//
// Description:
//      Same as STRUPR(), but CERTLIBN version.
//
// See Also:
//************************************************************************
// $Log:   S:/CERTLIBN/VCS/strupc.cpv  $
// 
//    Rev 1.7   03 Dec 1996 14:01:52   MKEATIN
// Ported AVCORE F
// 
//    Rev 1.6   02 Feb 1996 21:47:08   RSTANEV
// Added Unicode support.
//
//    Rev 1.5   29 Apr 1995 17:09:16   DALLEE
// Added LOADDS for 16-bit "C" functions since CERTLIBN is naughty with DS.
//
//    Rev 1.4   21 Dec 1994 19:12:54   DALLEE
// Compile as "C" for all platforms.
//
//    Rev 1.3   21 Dec 1994 13:34:28   JMILLARD
//
// ifdef c++ around extern (NLM builds as c even with cpp extension)
//
//    Rev 1.2   20 Dec 1994 17:57:04   BRAD
// Had wrong name for function
//
//    Rev 1.1   20 Dec 1994 17:46:18   BRAD
// Made extern 'C'
//
//    Rev 1.0   20 Dec 1994 17:12:20   BRAD
// Added to support CRC of long filename in FPROT.ASM
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "dbcs.h"
#include "ctsn.h"

/*@API:**********************************************************************
@Declaration:
VOID WINAPI STR4_UPPER(LPTSTR lpszString);

@Description:
This function uppercases a string according to file system behavior.
Double byte characters are not uppercased. For filenames, these are not
equivalent. This function should only be used to standardize the appearance
of filenames.

@Parameters:
$lpszString$            String to upper.

@Returns: Nothing
@See:
@Include:   ctsn.h
@Compatibility: DOS DX WIN W32 VXD NLM
****************************************************************************/

#if defined(__cplusplus)
    extern "C"
#endif

VOID WINAPI CTSN_LOADDS STR4_UPPER(LPTSTR lpszString)
{
#if !defined (SYM_NLM)
    while (EOS != *lpszString)
        {
        if (FALSE == DBCSIsLeadByte(*lpszString))
            {
            *lpszString = CharToUpper(*lpszString);
            }
        lpszString = AnsiNext(lpszString);
        }
#else
    STRUPR(lpszString);
#endif
}
