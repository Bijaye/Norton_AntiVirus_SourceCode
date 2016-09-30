// Copyright 1997 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/ApwCmd.h_v   1.3   29 Jun 1998 22:19:00   RStanev  $
//
// Description:
//   Contains the exported DLL functions that all ApwCmd DLLs must implement.
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/ApwCmd.h_v  $
// 
//    Rev 1.3   29 Jun 1998 22:19:00   RStanev
// Added new members to CApwCmd for support of asynchronous processing
// of tasks triggered by driver notifications.
//
//    Rev 1.2   01 Jun 1997 12:39:08   DALLEE
// Changed naming scheme to be more readable - "APW" in classes and
// functions is now "Apw".
//
//    Rev 1.1   31 May 1997 19:49:30   JBRENNA
// 1. Rename the file to ApwCmd.h (was ApwCmdApi.h).
// 2. This now provides a pure virtual base class for all ApwCmd project
//    classes. Added APWCmdNew.
//
//    Rev 1.0   31 May 1997 19:47:44   JBRENNA
// Initial revision.
//
//    Rev 1.4   29 May 1997 13:16:28   DALLEE
// 1. Changed DllExport to APWMCMDAPI.
//    This is __declspec(dllexport) when building this dll, and
//    __declspec(dllimport) for all others.
// 2. Changed LPVOID parameter to LPVOID * -- we need to be able to
//    set the value of the LPVOID which our client tracks for us.
//
//    Rev 1.3   28 May 1997 17:35:00   JBRENNA
// Change SYM_EXPORT to APWCMDAPI. By defining APWCMDAPI here, we do not force
// the inclusion of platform.h.
//
//    Rev 1.2   28 May 1997 16:58:26   JBRENNA
// Ops ... listed CmdStartDriver twice. The second one is now CmdStopDriver().
//
//    Rev 1.1   28 May 1997 15:41:06   JBRENNA
// Change all '#include <...>' to '#include "..."'. This enables R:\MK\MkDeps
// to function better.
//
//    Rev 1.0   28 May 1997 15:21:40   JBRENNA
// Initial revision.
//
//***************************************************************************

#ifndef __APWCMD_H
#define __APWCMD_H

#ifndef __APW_H
#include "apw.h"
#endif


// BUILDING_APWCMD must be defined and only defined when building
// the APWCMD project.

#ifdef  BUILDING_APWCMD
  #define APWCMDAPI __declspec( dllexport )
#else
  #define APWCMDAPI __declspec( dllimport )
#endif


//************************************************************************
// class CApwCmd
//
// Abstract base class for APW command class.

class CApwCmd
{
// Operations
public:
    virtual BOOL     Init( HWND hParent ) = 0;
    virtual BOOL     DeInit( VOID ) = 0;

    virtual BOOL     Options( HWND hParent ) = 0;
    virtual BOOL     StartDriver( VOID ) = 0;
    virtual BOOL     StopDriver( VOID ) = 0;
    virtual BOOL     WaitForStartupPending( VOID ) = 0;
    virtual BOOL     EnableDriver( VOID ) = 0;
    virtual BOOL     DisableDriver( VOID ) = 0;
    virtual AP_STATE GetStatusOfDriver( VOID ) = 0;
    virtual BOOL     ShutdownScan( HWND hParent ) = 0;
    virtual VOID     ConfirmApwComTasksReceived( VOID ) = 0;

// Attributes
public:

// Implementation
public:
    virtual ~CApwCmd() {};
};


#ifdef __cplusplus
extern "C" {
#endif

// Get an instance of a CApwCmd class from the APWCMD DLL.
// Free instance with "delete".

APWCMDAPI CApwCmd * WINAPI ApwCmdNew ();

#ifdef __cplusplus
}
#endif

#endif // __APWCMD_H
