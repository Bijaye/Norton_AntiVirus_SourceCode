// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// DefRB.cpp : Defines the entry point for the console application.
//
// The problem:
//  When the user tries to rollback defs by placing older defs in the 
//  Hawking INCOMING directory, the NAVC client UI shows that defs have
//  rolled back. However, the old engine files are still in use. The defs 
//  do not get unloaded even though NAVC is registered with Hawking with
//  the rollback set. Hawking may even attempt to delete the original defs
//  ( two files will be left behind because they are in use ). Bottom line,
//  Definition rollback through INCOMING directory does not work. At best,
//  rollback fails but we report success. At worst, the user is unprotected.
//
// The (hack) solution:
//  This console application is designed to force definition reload on 
//  NAVC ver 7.xx clients. Runs on WinNT or Win2k. Win9x is not supported
//  because it seems that the registry watch on the ProductControl key
//  is not working. This tool relies on the behavior of rtvscan that when
//  HeuristicLevel is changed ,the engine must be reloaded. Unfortunately,
//  when reload is forced, rtvscan only looks for .vdb files, it ignores the
//  shared ( Hawking ) defs. Our method here is to change HeuristicLevel, set
//  the NewPatternFile reg key ( which is watched ) forcing a def reload. 
//  (Actually this only forces the def unload. If no .vdb files are available,
//  no defs will be loaded. AP will be disabled. Running a manual scan will
//  reload the defs.) We repeat the process to put the HeuristicLevel back. Defs
//  are probably still unloaded. We set the NewPatternFile value a third time,
//  this time defs should be loaded. We are done.

// Usage: DefRLoad.exe [/silent]

#include "stdafx.h"
#include "DefRB.h"
#include <atlbase.h>
#include <vpcommon.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int nRetCode = -1;
    CString strDone, strDefReload, strArg;
    BOOL bSilent = FALSE;
    
    // initialize MFC and print and error on failure
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    {
        // change error code to suit your needs...
        cerr << _T("Fatal Error: MFC initialization failed") << endl;
        nRetCode = 1;
        return nRetCode;
    }
    
    // Check flags.
    if ( argc > 1 )
    {
        strArg = argv [1];
        strArg.MakeUpper();
        if (-1 != strArg.Find ( _T('S') ))
        {
            bSilent = TRUE;
        }
    }
    
    if ( argc > 2 )
    {
        CString strUsage;
        
        strUsage.Format ( IDS_USAGE, argv[0] );
        
        cout << (LPCTSTR)strUsage << endl;
    }
    else
    {
        DWORD dwResult = ForceUpdate ();
        nRetCode = ( int ) dwResult ;
        if ( !bSilent )
        {
            
            switch ( dwResult ) 
            {
            case ERROR_SUCCESS :
                strDone.LoadString ( IDS_SUCCESS );
                break;
            case ERR_NO_NAVC :
                strDone.LoadString ( IDS_USAGE );
                break;
            case ERR_NO_NAVC_CLIENT :
                strDone.LoadString ( IDS_USAGE );
                break;
            case ERR_REGISTRY_WRITE :
                strDone.LoadString ( IDS_REGISTRY_WRITE_FAIL );
                break;
            case ERR_DEFS_UNLOADED :
            default :
                
                strDone.LoadString ( IDS_FAIL );
                break;
            }
            strDefReload.LoadString ( IDS_DEF_RELOAD );
            
            cout << (LPCTSTR)strDefReload << _T("\n") << (LPCTSTR)strDone << endl;
        }
    }
    
    return nRetCode;
}

// Function name	: ForceUpdate
// Argument         : void
// Description	    : 
//  Order of Operation:
//  Save the value of HeuristicLevel under the main key.
//  Change the value of the HeuristicLevel.
//  Set the NewPatternFile ProductControl value to 1.
//  Wait for NewPatternFile ProductControl to revert to 0.
//  Now the current defs should be unloaded.
//  We need to put HeuristicLevel back.
//  Then we need to attempt to load defs. This will fail.
//  We need to attempt to load defs again. This should work.

// Return type		: DWORD 
//    ERROR_SUCCESS         Success.
//    ERR_NO_NAVC           NAVC is not installed.
//    ERR_NO_NAVC_CLIENT    NAVC server is installed.
//    ERR_DEFS_UNLOADED     Reload failed, the defs may have been unloaded. 
//                          (User should restart the service or reboot.)
//    ERR_REGISTRY_WRITE    Couldn't write to the registry.

DWORD ForceUpdate ( void )
{
    CRegKey regMain;
    CRegKey regProductControl;
    DWORD dwCount, dwHeuristicLevel, dwNewPatternFile = 1,dwType = 1;
    long lRet;
    DWORD dwReturn = ERROR_SUCCESS;
    
    if ( (lRet = regMain.Open ( 
        HKEY_LOCAL_MACHINE, 
        _T("SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion"),
        KEY_READ | KEY_WRITE )) ||
        (lRet = regProductControl.Open ( 
        HKEY_LOCAL_MACHINE, 
        _T("SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion\\ProductControl"),
        KEY_READ | KEY_WRITE )))
    {
        return ERR_NO_NAVC;
    }
    
    // Check for Type == 1 which implies a client.
    if ( ERROR_SUCCESS != regMain.QueryValue ( dwType, _T( "Type" )   ) || 1 != dwType  )
    {
        return ERR_NO_NAVC_CLIENT;
    }
    
    if ( ERROR_SUCCESS != regMain.QueryValue ( dwHeuristicLevel, _T( "HeuristicLevel" )   )   )
    {
        dwHeuristicLevel = 2;
    }
    
    // Change HeuristicLevel to 1, 2 or 3.
    if ( ERROR_SUCCESS != regMain.SetValue ( (dwHeuristicLevel % 3) + 1, _T( "HeuristicLevel" )))
    {
        // Uh oh.
        return ERR_REGISTRY_WRITE;
    }
    
    // Reload the defs.
    if ( ERROR_SUCCESS != regProductControl.SetValue ( 1, _T( "NewPatternFile" )))
    {
        // Uh oh. Try to put the HeuristicLevel back.
        regMain.SetValue ( dwHeuristicLevel, _T( "HeuristicLevel" ));
        return ERR_DEFS_UNLOADED;
    }
    
    // Wait until the defs have been processed. Poll the NewPatternFile value.
    dwNewPatternFile =1;
    dwCount = 0;
    while ( dwNewPatternFile )
    {
        if ( dwCount++ > 4 * 10 ) // It should not take more than 10 seconds.
        {
            // Continue, but we can't be sure of success.
            dwReturn = ERR_DEFS_UNLOADED;
            break;
        }
        
        Sleep ( 250 );
        regProductControl.QueryValue ( dwNewPatternFile , _T( "NewPatternFile" ));
        
    }
    
    // Put it back.
    if ( ERROR_SUCCESS != regMain.SetValue ( dwHeuristicLevel, _T( "HeuristicLevel" )))
    {
        // Uh oh.
        // This is bad, but very unlikely.
        // At this point we have probably unloaded the defs.
        // We might as well continue.
        dwReturn = ERR_DEFS_UNLOADED;
    }
    
    // Attempt to reload the defs. This will fail. When the HeuristicLevel has changed, rtvscan forces
    // a reload of the defs from disk, regardless of whether they are newer than the defs in use.
    // However, because of a buggy interaction with code targeted for console rollback, Hawking defs are
    // ignored. This tool is targeted for an unmanaged client, so we must assume that there is no .vdb file
    // available. This means that defs will be unloaded, then the reload will fail.
    if ( ERROR_SUCCESS != regProductControl.SetValue ( 1, _T( "NewPatternFile" )))
    {
        // Uh oh.
        // Again, no reason to quit now. We might as well try to finish even if this call fails.
        dwReturn = ERR_DEFS_UNLOADED;
    }
    
    // Wait until the defs have been processed. Poll the NewPatternFile value.
    dwNewPatternFile =1;
    dwCount = 0;
    while ( dwNewPatternFile )
    {
        if ( dwCount++ > 4 * 10 ) // It should not take more than 10 seconds.
        {
            // Continue, but we can't be sure of success.
            dwReturn = ERR_DEFS_UNLOADED;
            break;
        }
        
        Sleep ( 250 );
        regProductControl.QueryValue ( dwNewPatternFile , _T( "NewPatternFile" ));
        
    }
    
    // Reload the defs. This time HeuristicLevel has not changed. We should reload the latest defs available.
    if ( ERROR_SUCCESS != regProductControl.SetValue ( 1, _T( "NewPatternFile" )))
    {
        // Uh oh.
        dwReturn = ERR_DEFS_UNLOADED;
    }
    
    return dwReturn;
}

