//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexent.cpv   1.1   08 May 1997 13:00:18   MKEATIN  $
//
// Description:
//      Contains NAVEX EXTStartUp and EXTShutDown functions
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexent.cpv  $
// 
//    Rev 1.1   08 May 1997 13:00:18   MKEATIN
// Use n30type.h instead of ctsn.h
// 
//    Rev 1.0   02 May 1997 13:28:54   CNACHEN
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "n30type.h"
#include "callbk15.h"
#include "navex15.h"

// EXTStartUp
//  lpstCallBacks       Callbacks provided by cleint
//  wClientVersion      Version number of client for compatability checking
//  lpstChain           Pointer to the ENGINE1.5 export table, to be passed
//                      in to the NAVEX1.5 EXTStartUp function.
//  lpszNAVEXDataDir    Path, including trailing backslash to data file dir.
//  lpszNAVEXINIFile    Full path and filename to NAVEX1.5 .ini file.
//  lpwServerVersion    Pointer to a WORD to contain the NAVEX/ENGINE1.5 version #
//
// Description:
//  This should only be called for the NAVEX1.5 module; it will automatically
//  call the ENGINE1.5 EXTStartUp if lpstChain points to its export table.  This
//  function loads all data files and sets up all global data for NAVEX scanning.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_MEM_ERROR           if memory error
//                        EXTSTATUS_FILE_ERROR          if file error
//                        EXTSTATUS_INIT_ERROR          general init. error
//

EXTSTATUS FAR WINAPI NLOADDS EXTStartUp
(
    LPCALLBACKREV2                          lpstCallBacks,
    WORD                                    wClientVersion,
    struct tag_EXPORT15_TABLE FAR *         lpstChain,
    LPTSTR                                  lpszNAVEXDataDir,
    LPTSTR                                  lpszNAVEXINIFile,
    LPWORD                                  lpwServerVersion
)
{
    return ( EXTSTATUS_OK );
}


// EXTShutDown
//
//  lpstCallBacks       Callbacks provided by cleint
//
// Description:
//
//  Call this function only for NAVEX1.5 module.  Do not call for ENGINE1.5.
//  This func. frees all global scanning data allocated by NAVEX/ENGINE.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_MEM_ERROR           if memory error
//                        EXTSTATUS_FILE_ERROR          if file error
//                        EXTSTATUS_INIT_ERROR          general init. error
//

EXTSTATUS FAR WINAPI NLOADDS EXTShutDown
(
    LPCALLBACKREV2                          lpstCallBacks
)
{
    return ( EXTSTATUS_OK );
}




