// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/APWUI/VCS/Apwtray.h_v   1.0   02 Jun 1997 13:56:06   DALLEE  $
//
// Description:
//  Header for NAVAPW shell tray icon management.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/APWUI/VCS/Apwtray.h_v  $
// 
//    Rev 1.0   02 Jun 1997 13:56:06   DALLEE
// Initial revision.
//************************************************************************

#ifndef __APWTRAY_H
#define __APWTRAY_H

// ID of NAVAPW's tray icon.

#define APW_TRAY_ICON   200

// Tray notification callback routine. See APW.H for APWM_TRAY_NOTIFY custom
// message define for tray notifications.

LRESULT ApwTrayNotify ( HWND hWnd, WPARAM wParam, LPARAM lParam );
BOOL ApwUpdateTrayIcon (bool refreshIcon = false);


#endif // __APWTRAY_H
