// Copyright 1996 Symantec Corporation
//****************************************************************************
//
// $Header:   S:/CDSTART/VCS/autorun.h_v   1.0   18 Apr 1997 13:45:06   DSACKIN  $
//
//****************************************************************************
// $Log:   S:/CDSTART/VCS/autorun.h_v  $
// 
//    Rev 1.0   18 Apr 1997 13:45:06   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:08:00   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:45:32   TSmith
// Initial revision.
//****************************************************************************
#include <windows.h>

#define AUTORUN_KEYNAME     "Software\\Symantec\\Norton AntiVirus NT"
#define AUTORUN_VALUENAME   "AutoRun"

// Turns off AutoRun for Norton AntiVirus NT CD AutoRun application.
//      Returns: TRUE - Successfully turned off.
//               FALSE - Unsuccessful.
inline BOOL TurnOffAutoRun()
    {
    HKEY AUTORUN_KEY = { 0 };
    
    if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, AUTORUN_KEYNAME, 0, KEY_EXECUTE | KEY_READ, &AUTORUN_KEY ) != ERROR_SUCCESS )
        return FALSE;

    BYTE Buff = 0;
    BOOL fRet = TRUE;
    
    fRet = (RegSetValueEx( AUTORUN_KEY, AUTORUN_VALUENAME, NULL, REG_BINARY, (CONST BYTE *)&Buff, sizeof( BYTE ) )
        == ERROR_SUCCESS );

    RegCloseKey( AUTORUN_KEY );
    
    return fRet;
    }

// Turns on AutoRun for Norton AntiVirus NT CD AutoRun application.
//      Returns: TRUE - Successfully turned on.
//               FALSE - Unsuccessful.
inline BOOL TurnOnAutoRun()
    {
    HKEY AUTORUN_KEY = { 0 };
    
    if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, AUTORUN_KEYNAME, 0, KEY_EXECUTE | KEY_READ, &AUTORUN_KEY ) != ERROR_SUCCESS )
        return FALSE;

    BYTE Buff = 1;
    BOOL fRet = TRUE;
    
    fRet = (RegSetValueEx( AUTORUN_KEY, AUTORUN_VALUENAME, NULL, REG_BINARY, (CONST BYTE *)&Buff, sizeof( BYTE ) )
        == ERROR_SUCCESS );

    RegCloseKey( AUTORUN_KEY );

    return fRet;
    }

// Indicates whether the Norton AntiVirus NT CD AutoRun application should execute. 
//    Returns: TRUE - AutoRun app should execute.
//             FALSE - AutoRun app should not execute.
inline BOOL QueryAutoRun()
    {
    HKEY AUTORUN_KEY = { 0 };
    BOOL fRet = TRUE;
    
    // Open the key. On failure, return fRet default (TRUE) which indicates that AutoRun app should execute.
    if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, AUTORUN_KEYNAME, 0, KEY_EXECUTE | KEY_READ, &AUTORUN_KEY ) != ERROR_SUCCESS )
        return fRet;

    DWORD Type = REG_DWORD;
    BYTE Buff = 0;
    DWORD Size = sizeof( BYTE );
    
    // Query the value. If successful set the return value to whatever the value is. Otherwise,
    // leave the default value (TRUE).
    if( RegQueryValueEx( AUTORUN_KEY, AUTORUN_VALUENAME, NULL, &Type, (LPBYTE)&Buff, &Size ) == ERROR_SUCCESS )
        fRet = (BOOL)MAKELONG( Buff, 0 );

    RegCloseKey( AUTORUN_KEY );

    return fRet;
    }
