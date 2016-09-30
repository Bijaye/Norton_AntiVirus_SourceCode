// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVSDK/VCS/Winmisc.cpv   1.1   11 Dec 1997 16:20:32   DALLEE  $
//
// Description:
//
// Contents:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVSDK/VCS/Winmisc.cpv  $
// 
//    Rev 1.1   11 Dec 1997 16:20:32   DALLEE
// Needed to set dwPathSize before calling RegQueryValue when checking
// for new VxD communication method. This is input as well as output, 
// and contains our buffer size.
// 
//    Rev 1.0   02 Dec 1997 14:29:10   DALLEE
// Initial revision.
//************************************************************************
// From NEWVIR-NAVOEM winmisc.cpv r1.6
//
//    Rev 1.6   04 Nov 1997 17:08:44   tcashin
// Changed the parameter list for CheckForNewVxDMethod().
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "navutil.h"
#include "navapcom.h"
#include "drvcomm.h"

#ifdef SYM_WIN32
#include "apcomm.h"
#include "winbase.h"
#endif


//***************************************************************************
// External Data
//***************************************************************************
extern BOOL     gbUseNAVAPDLL;
extern FARPROC  lpfnNAVAPProtect;
extern FARPROC  lpfnNAVAPUnprotect;


/* **************************************************************************
@Name:
VOID SYM_EXPORT WINAPI TalkToVxD (      // Communicate with VXD
    VXDACTION   wVxDAction              // [in] action to perform
    );

@Description:
This function communicates with the NAV AP VXD.

@Parameters:
$wVxDAction$  Action indicator specifying to message we wana get across.
              At present time it can any of the following:

              VXDACTION_ENABLE:     To enable the VXD for this thread
              VXDACTION_DISABLE:    To disable the VXD for this thread

@Include: navapcom.h
************************************************************************** */

VOID SYM_EXPORT WINAPI TalkToVxD (      // Communicate with VxD
    VXDACTION   wVxDAction              // [in] Action to tell VxD to do
    )
{
                                        // Do this at runtime, so we
                                        // don't have to split the DLL:
#ifdef SYM_WIN32

                                        // Are we supposed to use the new
                                        // VxD communications functions?

    if (gbUseNAVAPDLL == TRUE)
    {
        switch ( wVxDAction )
        {
        case VXDACTION_ENABLE:
            if (lpfnNAVAPProtect != NULL)
                (lpfnNAVAPProtect());
            break;

        case VXDACTION_DISABLE:
            if (lpfnNAVAPUnprotect != NULL)
                (lpfnNAVAPUnprotect());
            break;
        }

        return;
    }

                                        // Use the old DRVCOMM functions

    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    {
        auto    DriverComm    commNAVAP;
                                        // Establish a connection

        if ( commNAVAP.Open( NAVAP_COMMUNICATION_NAME ) )
        {
                                        // Tell the VxD what we want it
                                        // to do.
            switch ( wVxDAction )
            {
            case VXDACTION_ENABLE:
               commNAVAP.Send( NAVAP_COMM_PROTECTPROCESS );
               break;

            case VXDACTION_DISABLE:
               commNAVAP.Send( NAVAP_COMM_UNPROTECTPROCESS );
               break;

            default:
                SYM_ASSERT ( 0 );
            }
        }
    }
    else
    {
                                        // Use DeviceIOControl for Win95
        auto    HANDLE  hNAVAP;
                                        // Open up a communications channel
                                        // with the Vxd

        hNAVAP = CreateFile (NAVAP_VXD_COMM_NAME, 0, 0, 0, OPEN_EXISTING, 0, 0);
        if ( hNAVAP != INVALID_HANDLE_VALUE )
        {
                                        // Tell the VxD what we want it
                                        // to do.
            switch ( wVxDAction )
            {
            case VXDACTION_ENABLE:
                                        // Enable the VxD for the
                                        // current thread.
                DeviceIoControl ( hNAVAP, ID_COMMAND_DELETE_PROCESS_FROM_PEL,
                                  NULL, 0, NULL, 0, NULL, NULL );
                break;

            case VXDACTION_DISABLE:
                                        // Disable the VxD for the
                                        // current thread.
                DeviceIoControl ( hNAVAP, ID_COMMAND_ADD_PROCESS_TO_PEL,
                                  NULL, 0, NULL, 0, NULL, NULL );
                break;

            case VXDACTION_RELOAD_CONFIG:
                                        // Tell VxD to reload its config
                                        // file.
                DeviceIoControl ( hNAVAP, ID_COMMAND_NAVAP_LOAD_CONFIG,
                                  NULL, 0, NULL, 0, NULL, NULL );
                break;

            default:
                SYM_ASSERT ( 0 );
                break;
            }

                                        // Close the communications handle.
            CloseHandle ( hNAVAP );
        }
    }
#endif
}


/* **************************************************************************
@Name: CheckForNewVxDMethod

BOOL WINAPI CheckForNewVxDMethod ()

@Description:
Check to see if the new VxD communication method should be used.

Look for the "InstalledApps" reg key. Read the path it contains and load
NAVAP32.DLL from that directory. If that all works then get the proc addresses
for the NAVAPProtectProcess and NAVAPUnprotectProcess functions and return
TRUE.

Alway return FALSE for 16-bit.

@Parameters:


************************************************************************** */

BOOL WINAPI CheckForNewVxDMethod (LPHINSTANCE lphInst)
{
    auto        BOOL            ret_val = FALSE;

#ifdef SYM_WIN32

    #define     NAV_AP_DLL      "\\NAVAP32.DLL"

    auto        DWORD           dwPathSize;
    auto        HINSTANCE       hInst;
    auto        HKEY            hKey ;
    auto        long            lResult = 0L;
    auto        char            szPath[SYM_MAX_PATH] ;

    auto        LPCTSTR         lpszNAVINSTALLKEY  = "SOFTWARE\\Symantec\\InstalledApps";
    auto        LPTSTR          lpszNAVNT          = "NAVNT";
    auto        LPTSTR          lpszNAV95          = "NAV95";

    auto        UINT            uOldErrorMode = SEM_FAILCRITICALERRORS;

                                        // Check the InstalledApps reg key. If we cannot
                                        // open it or get the value, we're done.

    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpszNAVINSTALLKEY, 0, KEY_QUERY_VALUE, &hKey);

    if ( lResult == ERROR_SUCCESS )
    {
        dwPathSize = sizeof( szPath );

        if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
            lResult = RegQueryValueEx( hKey, lpszNAVNT, (LPDWORD)NULL, NULL, (LPBYTE)&szPath, &dwPathSize) ;
        else
            lResult = RegQueryValueEx( hKey, lpszNAV95, (LPDWORD)NULL, NULL, (LPBYTE)&szPath, &dwPathSize) ;

        RegCloseKey( hKey ) ;

        if (lResult == ERROR_SUCCESS )
        {
                                        // Append the DLL name to the value from the reg
                                        // key.
            STRCAT(szPath, NAV_AP_DLL);

            uOldErrorMode = SetErrorMode( SEM_FAILCRITICALERRORS );

                                        // Load the NAVAP32 DLL. If this fails, we're done.

            hInst = LoadLibrary(szPath);
            if (hInst != NULL)
            {
                                        // Get the addresses of NAVAPProtectProcess and
                                        // NAVAPUnprotectProcess

                lpfnNAVAPProtect = GetProcAddress(hInst,  szNAVAPProtectProcess);
                lpfnNAVAPUnprotect = GetProcAddress(hInst, szNAVAPUnprotectProcess);

                if ((lpfnNAVAPProtect != NULL) && (lpfnNAVAPUnprotect != NULL))
                {
                    *lphInst = hInst;
                    ret_val = TRUE;
                }
            }
        }
    }

    if ( SEM_FAILCRITICALERRORS != uOldErrorMode )
        SetErrorMode( uOldErrorMode );

#endif

    return ret_val;
}



