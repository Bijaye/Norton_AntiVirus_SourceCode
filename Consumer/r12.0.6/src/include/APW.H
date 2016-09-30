// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/apw.h_v   1.2   29 Jun 1998 22:18:38   RStanev  $
//
// Description:
//  General internal header for NAVAPW - AP Agent UI component.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/apw.h_v  $
// 
//    Rev 1.2   29 Jun 1998 22:18:38   RStanev
// Added a new user message.
//
//    Rev 1.1   11 Aug 1997 17:03:54   DALLEE
// Added APWM_UPDATE_UI - used by service to notify UI to resync displayed
// status.
// Added magic parameters for APWM_SHUTDOWN so we don't respond even if
// somehow we accidentally receive this message.
//
//    Rev 1.0   28 May 1997 11:27:40   DAVROS
// Initial revision.
//************************************************************************

#ifndef __APW_H
#define __APW_H

//************************************************************************
// Defines
//************************************************************************

#define APW_CLASS_NAME  _T("NAVAP Wnd Class")

// Extra data stored in main window.

enum {
    APWL_BASE = DLGWINDOWEXTRA / sizeof( LONG ),
    APWL_MAX                            // Unused - only indicates size of data.
};

// Custom window messages.

enum {
    APWM_BASE = WM_USER + 200,
    APWM_ACTIVATE,                      // Sent to restore/display main window.
    APWM_INITDIALOG,                    // Fake init dialog since we're a window.
    APWM_TRAY_NOTIFY,                   // Tray icon callback message.
    APWM_SHUTDOWN,                      // Signal to shutdown windows.
    APWM_UPDATE_UI,                     // Resync UI w/ current state of driver.
    APWM_COM_TASKS                      // Perform COM tasks requested by NAVAP's callback.
};

// Magic numbers which must be used when posting APWM_SHUTDOWN.
// We don't want to accidentally get this message.

#define APWM_SHUTDOWN_MAGIC_WPARAM      0x73737572
#define APWM_SHUTDOWN_MAGIC_LPARAM      0x696d7573

// Current state values for Auto-Protect and Tray Icon.

enum AP_STATE {
    AP_STATE_ENABLED,
    AP_STATE_DISABLED,
    AP_STATE_LOADING,
    AP_STATE_ERROR
};


//************************************************************************
// Typedefs
//************************************************************************


//************************************************************************
// Prototypes
//************************************************************************


#endif // __APW_H

