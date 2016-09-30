//////////////////////////////////////////////////////////////////////
//
// navapnotify.h - This file contains the constants used to notify
// scanning applications that the NAVAP service has started, and that
// they need to disable AP protection for thier scanning processes.
//

#if !defined( _NAVAPNOTIFY_H_ )
#define _NAVAPNOTIFY_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// Scanning applications need to create a manual-reset Win32 
// Event object with this name, and have a thread wait on it.
// When the service starts, the event is pulsed, and all clients
// will need to call NAVAPUnprotectProcess().
//
// The reason this is needed is that the NAVAP service can be stopped
// and started during scan operations.  We want to avoid our 
// scanning applications triggering AP alerts.  
// Note that on Win2k and greater systems, you must prepend the string "Global\"
// to ensure that you are using the global mutex.
//
const TCHAR NAVAP_STARTED_EVENT[] =         _T( "NAVAPSTARTED" );

// This event is set when the AP tray icon is up and running. This
// is used by modules that run at start up and want to delay work
// until after the AP tray icon has been displayed. It will be fired
// even if the icon is disabled.
//
const char SYM_NAV_AP_TRAY_ICON_EVENT[] = "SYM_NAV_AP_TRAY_ICON_EVENT";

#endif