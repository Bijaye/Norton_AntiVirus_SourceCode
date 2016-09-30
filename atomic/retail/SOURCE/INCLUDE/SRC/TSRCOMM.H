// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/include/VCS/tsrcomm.h_v   1.0   06 Feb 1997 20:51:36   RFULLER  $
//
// Description:
//      Turn Windows and DOS TSR's on and off.
//      TSR_ON and TSR_OFF calls must come in matching pairs.
//
// Contains:
//      TSR_ON  macro
//      TSR_OFF macro
//
// See Also:
//************************************************************************
// $Log:   S:/include/VCS/tsrcomm.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:36   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:47:42   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:12:06   BARRY
// Initial revision.
// 
//    Rev 1.0   31 Oct 1994 12:51:02   MFALLEN
// Initial revision.
// 
//    Rev 1.4   11 Sep 1993 00:54:08   BRAD
// Made tsrflag 'int'.  That way, we can insure that it doesn't go BELOW its
// legal range.
// 
//    Rev 1.3   08 Aug 1993 15:56:48   MFALLEN
// Removed previous change.
// 
//    Rev 1.2   08 Aug 1993 15:28:10   MFALLEN
// Reversed the toogeling of the tsr flag as per carlton's request.
// 
//    Rev 1.1   28 Jul 1993 14:52:14   DALLEE
// Put in DOS macro defines for TSR_ON and TSR_OFF.  
//************************************************************************

#ifndef _TSRCOMM_H
#define _TSRCOMM_H

#ifdef __cplusplus
extern "C" {
#endif

VOID SYM_EXPORT WINAPI ToggleTsrFlag    (BOOL bIncrement);
VOID SYM_EXPORT WINAPI SetTsrFlagTo     (int iValue);
int  SYM_EXPORT WINAPI GetTsrFlag       (VOID);

#ifdef __cplusplus
}
#endif

#ifndef TSR_ON

#ifdef SYM_WIN
    #define TSR_OFF {ToggleTsrFlag (TRUE);}
    #define TSR_ON  {ToggleTsrFlag (FALSE);}
#else
    #define TSR_ON  {if (TSRIsLoaded(TSR_ID_NAV)) TSREnable(TSR_ID_NAV);}
    #define TSR_OFF {if (TSRIsLoaded(TSR_ID_NAV)) TSRDisable(TSR_ID_NAV);}
#endif  // SYM_WIN

#endif  // !TSR_ON

#endif  // !_TSRCOMM_H
