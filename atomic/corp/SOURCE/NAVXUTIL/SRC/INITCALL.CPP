// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/INITCALL.CPv   1.3   19 Jun 1997 15:59:58   MKEATIN  $
//
// Description: This file contains local functions to override functions
//              in the global callback structer.
//
// Contains:    LocalGetConfigInfo()
//              InitCallBacks()
//
// See Also:    The AVCORE/CALLBACKS project
//************************************************************************
// $Log:   S:/NAVXUTIL/VCS/INITCALL.CPv  $
// 
//    Rev 1.3   19 Jun 1997 15:59:58   MKEATIN
// If Hueristics is turned off in navOtps - set level to '0'.
// 
//    Rev 1.2   19 Jun 1997 13:35:22   MKEATIN
// need to check for !STRICMP() not STRICMP() in LocalGetConfigInfo().
// 
//    Rev 1.1   19 Jun 1997 12:06:38   MKEATIN
// Fixed comments
// 
//    Rev 1.0   18 Jun 1997 15:18:38   MKEATIN
// Initial revision.
//************************************************************************

#include "platform.h"
#include "navutil.h"
#include "symcfg.h"
#include "options.h"

#ifdef SYM_DOS
  #define DOS_LOADDS LOADDS
#else
  #define DOS_LOADDS
#endif

//----------------------------------------------------------------------------
// LocalGetConfigInfo()
//
// Description:
//   The function provides configuration info by checking for any desired
//   command line switch.  If lpszID is "HEUR" and it is NOT found on the
//   command line, the configuration is read from navOptions.
//   
//
// Parameters:
//   lpszID        [in]  The command line switch to look for.
//   lpvResult     [out] The text found after the command line.
//                         Example:  /Huer:1   
//                       "1" would by copied into lpvResult.
//   uMaxResultLen [in]  The maximum characters that may be copied into
//                       the provied lpvResult buffer.
//   lpbFound      [out] The value of this pointer is set to TRUE if lpszID 
//                       is found on the command line - FALSE otherwise.  
//                       NOTE:  If lpszID is "HEUR" and is NOT found on the
//                       command line, *lpbFound and (LPSTR)lpvResult are set 
//                       by checking navOptions.
//   lpvCookie     [in]  For future use.
//----------------------------------------------------------------------------
// 06/18/9 MKEATIN - Created.
//----------------------------------------------------------------------------

VOID DOS_LOADDS WINAPI LocalGetConfigInfo
(
    LPTSTR lpszID,
    LPVOID lpvResult,
    UINT   uMaxResultLen,
    LPBOOL lpbImplemented,
    LPVOID lpvCookie
)
{
#if defined(SYM_DOSX) || defined (SYM_WIN32)

    NAVOPTIONS navOptions = {0};
    CFGTEXTREC rSwitch    = {lpszID, CFG_SW_EXACT,};
    char szLevel[2];

    *lpbImplemented = TRUE;

    if (ConfigSwitchSet(&rSwitch, NULL))
    {
        ConfigSwitchGetText(&rSwitch, (LPSTR)lpvResult, uMaxResultLen);
    }
    else if (!STRICMP(lpszID, "HEUR"))  // no switch found on command line -
    {                                   // check options if lpszID is "HEUR"
        ReadNavBin (&navOptions);
        if (navOptions.scanner.bEnableHeuristicScan)
        {
            szLevel[0] = navOptions.scanner.uHeuristicLevel + '0';
            szLevel[1] = EOS;
        }
        else                            
        {                               
            szLevel[0] = '0';           // heuristics is turned off which
            szLevel[1] = EOS;           // is level 0
        }
        strncpy((LPSTR)lpvResult, szLevel, uMaxResultLen);
    }
    else
    {
        *lpbImplemented = FALSE;
    }

#else

    *lpbImplemented = FALSE;

#endif
}


//************************************************************************
// InitCallBacks()
//
// This routine will override specified specified functions in the
// provided global callback structer.  
//
// Parameters:
//      LPAVGLOBALCALLBACKS - the provided callback structer that avapi1
//                            will be expecting.
//
//      DWORD               - Flags to control overiding behaviour
//
//************************************************************************
// 06/18/97 MKEATIN
//************************************************************************

VOID SYM_EXPORT WINAPI InitCallBacks 
(
    LPAVGLOBALCALLBACKS lpstNAVCallBacks,
    DWORD               dwFlags
)
{
    lpstNAVCallBacks->lpNAVEXCallBacks->GetConfigInfo = LocalGetConfigInfo;
}
