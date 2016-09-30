// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/navor.h_v   1.2   14 May 1997 13:08:10   JBRENNA  $
//
// Description: Prototypes for NAVOR32.DLL's export functions
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/navor.h_v  $
// 
//    Rev 1.2   14 May 1997 13:08:10   JBRENNA
// Make sure no one uses this file on the AVCORE trunk. This needs to
// This file must remain in VCS until the QAKF branch disappears.
// 
//    Rev 1.1   17 Jun 1996 16:21:32   MKEATIN
// Added BOOL bReboot parameter to CallOnramp() prototype
// 
//    Rev 1.0   17 Jun 1996 15:39:42   MKEATIN
// Initial revision.
//************************************************************************

#error NAVOR.H is obsolette. The NavLu project should be used instead.
#error if you have any questions contact me (Jim Brennan 6-310-7006).

BOOL SYM_EXPORT WINAPI CallOnramp (
     HWND      hWnd,                        
     BOOL      bReboot
     );

BOOL SYM_EXPORT WINAPI GetOnrampPathName (
     LPTSTR lpszOnrampPathName, 
     DWORD dwPathSize
     );

BOOL SYM_EXPORT WINAPI OnrampIsInstalled (
     VOID
     );
