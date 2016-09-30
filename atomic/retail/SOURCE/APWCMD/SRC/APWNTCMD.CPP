// Copyright 1996-1997 Symantec Corporation
//************************************************************************
//
// $Header:   S:/APWCMD/VCS/ApwNtCmd.cpv   1.7   29 Jun 1998 22:21:04   RStanev  $
//
// Description:
//    This is where the functions go that communicate through the pipe
//    to the NAVAP Service.
//
// Contains:
//    CmdInit()
//    CmdDeInit()
//    CmdOptions()
//    CmdWaitForStartupPending()
//    CmdDisableDriver()
//    CmdEnableDriver()
//    CmdGetStatusOfDriver()
//    CmdStartDriver()
//    CmdStopDriver()
//    CmdShutdownScan()
//    ConfirmApwComTasksReceived()
//
//************************************************************************
// $Log:   S:/APWCMD/VCS/ApwNtCmd.cpv  $
// 
//    Rev 1.7   29 Jun 1998 22:21:04   RStanev
// Added new functionality to support asynchronous processing of tasks
// triggered by driver notifications. This is a workaround for a problem
// with RPC in Win9x.
//
//    Rev 1.6   01 Jun 1997 12:38:18   DALLEE
// Changed naming scheme to be more readable - "APW" in classes and
// functions is now "Apw".
//
//    Rev 1.5   31 May 1997 19:53:00   JBRENNA
// 1. Use the CApwNtCmd class (which is derived from CAPWCmd) to provide the
//    APW command encapulation. This means all of the Cmd* functions become
//    member functions of CApwNtCmd.
// 2. Add APWCmdNew().
//
//    Rev 1.4   29 May 1997 13:13:40   DALLEE
// 1. Changed DllExport to APWMCMDAPI.
//    This is __declspec(dllexport) when building this dll, and
//    __declspec(dllimport) for all others.
// 2. Changed LPVOID parameter to LPVOID * -- we need to be able to
//    set the value of the LPVOID which our client tracks for us.
//
//    Rev 1.3   28 May 1997 17:32:24   JBRENNA
// 1. Change SYM_EXPORT to DllExport. The DllExport is defined n ApwCmdApi.h.
//    This means that clients of ApwCmd.DLL do not have to include platform.h
//    before including ApwCmdApi.h.
// 2. Change the szValue in GetNavwExePath from "@" to "".
//
//    Rev 1.2   28 May 1997 15:39:10   JBRENNA
// Change all "#include <...>" to '#include "..."'. This enables R:\MK\MkDeps
// to function better.
//
//    Rev 1.1   28 May 1997 15:20:14   JBRENNA
// Implement the CmdOptions() function.
//
//    Rev 1.0   28 May 1997 12:25:34   JBRENNA
// Initial revision.
//
//    Rev 1.0   24 Apr 1997 21:27:52   ksackin
// Initial revision.
//
//    Rev 1.0   13 Mar 1997 15:12:24   RFULLER
// Initial revision
//
//    Rev 1.0   08 Nov 1996 12:13:38   DKEEFER
// Initial revision
//
//    Rev 1.6   04 Aug 1996 22:10:20   DSACKIN
// Added CmdWaitForStartupPending
//
//    Rev 1.5   19 Jul 1996 01:27:34   DSACKIN
// Use our CNAVAPSVCClient object to talk to NAVAPSVC.
//
//    Rev 1.4   18 Jun 1996 22:11:36   DSACKIN
// Updated NAVAPW to support our new packet system.
//
//    Rev 1.3   21 May 1996 15:40:02   DSACKIN
// Added a new command that will scan the A: drive on logoff.
//
//    Rev 1.2   21 May 1996 00:54:44   DSACKIN
// Added a function that will start the NAVAP service.
//
//    Rev 1.1   08 May 1996 20:30:10   DSACKIN
// This file is now dedicated to ONLY hold CmdXXX functions.
//************************************************************************


//************************************************************************
// Include Files *********************************************************
//************************************************************************

#include "platform.h"
#include "navw.h"
#include "ApwNtRes.h"                   // This DLL's resource IDs.
#include "ApwRes.h"                     // This common resource IDs.
#include "ApwNtCmd.h"                   // CApwNtCmd declaration

//************************************************************************
// Externs ***************************************************************
//************************************************************************

extern "C"      HINSTANCE               g_hInstance;

//************************************************************************
// Statics ***************************************************************
//************************************************************************

//************************************************************************
// Defines ***************************************************************
//************************************************************************

#define     TRACE     ERR_TRACE_ID (_NAVAPCMD_CPP)

#define     ERROR_CALLNAMEDPIPE   (0x1000)
#define     ERROR_LOADSTRING      (0x1001)

//************************************************************************
// Static Functions ******************************************************
//************************************************************************

//////////////////////////////////////////////////////////////////////////
// GetNavwExePath
//
// Purpose:
//    Set lpszNavw to the full path to NAVW.
//
// Parameters:
//    lpszNavw  [out] - The full path to NAVW.
//    lpszDir   [out] - The directory portion of lpszNavw.
//
//////////////////////////////////////////////////////////////////////////
// 05/28/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////
void CApwNtCmd::GetNavwExePath (LPSTR lpszNavw, LPSTR lpszDir)
{
    LPCTSTR    szKey        = _T("Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\NAVWNT.EXE");
    LPCTSTR    szValue      = _T("");

    TCHAR      szValueBuf[SYM_MAX_PATH];
    DWORD      dwValueBufLen = sizeof (szValueBuf);
    HKEY       hBaseKey;
    DWORD      dwValueType;

    // Empty the strings.
    *lpszNavw = '\0';
    *lpszDir  = '\0';

    //  Get the OtherSwitches value from the CmdLines key.
    if (ERROR_SUCCESS == RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKey, 0, KEY_ALL_ACCESS, &hBaseKey))
    {
        if (ERROR_SUCCESS == RegQueryValueEx (hBaseKey, szValue, 0, &dwValueType, (LPBYTE)szValueBuf, &dwValueBufLen))
        {
            if (dwValueType == REG_SZ && dwValueBufLen)
                _tcscpy (lpszNavw, szValueBuf);
        }
    }

    RegCloseKey (hBaseKey);

    // If we were successful at setting up lpszNavw, copy the path portion
    // to lpszDir.
    if (*lpszNavw)
    {
        LPSTR lpszCurSrc  = lpszNavw;
        LPSTR lpszCurDest = lpszDir;
        LPSTR lpszLastDir = lpszDir;
        int   iChar;

        while (*lpszCurSrc != '\0')
        {
            iChar = *lpszCurSrc;

            // Copy the character.
            *lpszCurDest = *lpszCurSrc;
            ++lpszCurSrc;
            ++lpszCurDest;

            if (isleadbyte (iChar))
                continue;

            // Now, we are guaranteed that the character is SBCS.

            // Memorize the position of the last \ in the destination string.
            if (iChar == '\\')
                lpszLastDir = (lpszCurDest - 1);
        }

        // Terminate the lpszDir string at the last \ character. (Does not
        // include the \ character).
        *lpszLastDir = '\0';
    }
}

//************************************************************************
// External Function Definitions *****************************************
//************************************************************************

//////////////////////////////////////////////////////////////////////////
// ~CApwNtCmd
//
// Purpose:
//    Do nothing. Have this available in case we need to destroy some
//    data or something.
//
//////////////////////////////////////////////////////////////////////////
// 05/31/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////
CApwNtCmd::~CApwNtCmd()
{
}

//////////////////////////////////////////////////////////////////////////
// ApwCmdNew
//
// Purpose:
//    Create an instance of our derived APWCMD class. To free this class
//    call the C++ "delete" function.
//
// Paramters:
//    none
//
// Returns:
//    A "new"ly allocated CApwNtCmd command class.
//
//////////////////////////////////////////////////////////////////////////
// 05/31/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////

APWCMDAPI CApwCmd *WINAPI ApwCmdNew ()
{
    return (new CApwNtCmd);
}


//////////////////////////////////////////////////////////////////////////
// CmdInit
//
// Purpose:
//    This function initializes the data. As there currently is
//    no data to initialize, we simply return TRUE.
//
// Parameters:
//    hParent - ignored in NT.
//
// Return Value:
//    TRUE  - success.
//    FALSE - failure
//
//////////////////////////////////////////////////////////////////////////
// 05/27/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::Init( HWND hParent )
{
    m_TalkAPService.Connect();

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// CmdDeInit
//
// Purpose:
//    Matching uninitialization for command object.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - success.
//    FALSE - failure
//
//////////////////////////////////////////////////////////////////////////
// 05/27/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::DeInit()
{
    m_TalkAPService.DisConnect();

    return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// CmdOptions
//
// Purpose:
//    This routine displays the NAV options panel.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - success.
//    FALSE - failure
//
//////////////////////////////////////////////////////////////////////////
// 05/27/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::Options (HWND hParent)
{
    BOOL            bSuccess = TRUE;
    HINSTANCE       hInstModule;
    char            szModulePath[SYM_MAX_PATH];
    char            szStartDir[SYM_MAX_PATH];

    GetNavwExePath (szModulePath, szStartDir);

    HWND hM = FindWindow(NAV_MAIN_WND_CLASS, NULL);
    if (hM)
        SetForegroundWindow(hM);
    else
        hInstModule = ShellExecute ( hParent,
                                 "open",
                                 szModulePath,
                                 "/O:1",
                                 szStartDir,
                                 SW_SHOWNORMAL );

    if ((WORD)hInstModule <= 32)
    {
        unsigned    uMsgID;
        int         nRet;
        char        szMessage[256];
        char        szTitle[128];

        switch ((WORD)hInstModule)
        {
            case 0:
            case 8:
                uMsgID = IDS_ERR_AP_CANT_EXEC_NAVW_MEM;
                break;

            case 2:
            case 3:
            case 11:
                uMsgID = IDS_ERR_AP_CANT_EXEC_NAVW_BAD_FILE;
                break;

            default:
                uMsgID = IDS_ERR_AP_CANT_EXEC_NAVW;
                break;
        }


        nRet = LoadString(g_hInstance,
                          uMsgID,
                          szMessage,
                          sizeof (szMessage));

        if (nRet > 0)
        {
            nRet = LoadString(g_hInstance,
                              IDS_APP_CAPTION,
                              szTitle,
                              sizeof (szTitle));
        }

        if (nRet > 0)                        // Did our strings load?
        {
            MessageBox(hParent, szMessage, szTitle,
                       MB_OK | MB_ICONHAND);
        }

        bSuccess = FALSE;
    }

    return bSuccess;
}


//////////////////////////////////////////////////////////////////////////
// CmdWaitForStartupPending
//
// Purpose:
//    This function will send a code to NAVAPSVC that will only return
//    when it is either done starting up or an error happens.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - Command sent successfully.
//    FALSE - Command failure
//
//////////////////////////////////////////////////////////////////////////
// 05/08/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
// 05/27/1997 JBRENNA - Updated so that NAVAPW can link to this DLL
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::WaitForStartupPending()
{
    BOOL bReturn = TRUE;                    // Assume Success

   if ( NAVAPSVC_SUCCESS != m_TalkAPService.SendCommand(
                               NAVAPSVC_WAIT_FOR_STARTUP ) )
      bReturn = FALSE;

   return bReturn;
}


//////////////////////////////////////////////////////////////////////////
// CmdDisableDriver
//
// Purpose:
//    This function will send a code through a pipe to the NAVAP
//    service which will in turn tell the Device driver to disable.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - Command sent successfully.
//    FALSE - Command failure
//
//////////////////////////////////////////////////////////////////////////
// 05/08/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
// 05/27/1997 JBRENNA - Updated so that NAVAPW can link to this DLL
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::DisableDriver()
{
BOOL bReturn = TRUE;                    // Assume Success

   if ( NAVAPSVC_DISABLED != m_TalkAPService.SendCommand(
                                NAVAPSVC_AP_DISABLE ) )
      bReturn = FALSE;

   return bReturn;
}


//////////////////////////////////////////////////////////////////////////
// CmdEnableDriver
//
// Purpose:
//    This function will send a code through a pipe to the NAVAP
//    service which will in turn tell the Device driver to enable.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - Command sent successfully.
//    FALSE - Command failure
//
//////////////////////////////////////////////////////////////////////////
// 05/08/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
// 05/27/1997 JBRENNA - Updated so that NAVAPW can link to this DLL
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::EnableDriver()
{
BOOL bReturn = TRUE;                    // Assume Success

   if ( NAVAPSVC_ENABLED != m_TalkAPService.SendCommand(
                               NAVAPSVC_AP_ENABLE ) )
      bReturn = FALSE;

   return bReturn;
}



//////////////////////////////////////////////////////////////////////////
// CmdGetStatusOfDriver
//
// Purpose:
//    This function will send a code through a pipe to the NAVAP
//    service which will query the status of the driver.
//
// Parameters:
//    None.
//
// Return Value:
//    AP_STATE_ENABLED  - Device is enabled.
//    AP_STATE_DISABLED - Device is disabled.
//    AP_STATE_ERROR    - A problem occured.
//
//////////////////////////////////////////////////////////////////////////
// 05/08/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
// 05/27/1997 JBRENNA - Updated so that NAVAPW can link to this DLL
//////////////////////////////////////////////////////////////////////////
AP_STATE CApwNtCmd::GetStatusOfDriver()
{
	DWORD    dwState = m_TalkAPService.SendCommand(NAVAPSVC_GET_STATUS);
    AP_STATE ApState = AP_STATE_ERROR;

    // map the NAVAPSVC state to AP_STATE.
    switch (dwState)
    {
        case NAVAPSVC_ENABLED:   ApState = AP_STATE_ENABLED;    break;
        case NAVAPSVC_DISABLED:  ApState = AP_STATE_DISABLED;   break;
    }

	return ApState;
}


//////////////////////////////////////////////////////////////////////////
// CmdStartDriver
//
// Purpose:
//    This function will attempt to start the AP Service which will
//    in turn start all of the device drivers.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - Drivers started successfully.
//    FALSE - Failure to start them.
//
//////////////////////////////////////////////////////////////////////////
// 05/20/1996 DSACKIN - Function Created
// 05/27/1997 JBRENNA - Updated so that NAVAPW can link to this DLL
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::StartDriver()
{
CServiceControl cNavapsvc;
BOOL            bReturn = TRUE;         // Assume success until failure
char            szServiceName[SYM_MAX_PATH];
int             nRet;

   nRet = LoadString(
             g_hInstance,
             IDS_NAVAP_SERVICE_NAME,
             szServiceName,
             sizeof (szServiceName) );

   if ( nRet > 0 )
   {
      if (ERROR_SUCCESS == cNavapsvc.Open(szServiceName))
      {
                                        // Try to start it.
         if (ERROR_SUCCESS != cNavapsvc.Start())
            bReturn = FALSE;            // Did we fail?

                                        // If we opened it, we should close it.
         cNavapsvc.Close();
      }
      else
         bReturn = FALSE;
   }
   else
      bReturn = FALSE;

   return bReturn;
}

//////////////////////////////////////////////////////////////////////////
// CmdStopDriver
//
// Purpose:
//    This function will attempt to stop the AP Service. This is
//    not currently implemented.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - Drivers started successfully.
//    FALSE - Failure to start them.
//
//////////////////////////////////////////////////////////////////////////
// 05/27/1997 JBRENNA - Function Created
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::StopDriver()
{
    return FALSE; // Fail - this function is not implemented.
}

//////////////////////////////////////////////////////////////////////////
// CmdShutdownScan
//
// Purpose:
//    This function will call the device to scan the A drive.  If the
//    floppy is infected, the user will be prompted to remove the
//    floppy from the A drive and the scan will happen again.
//
// Parameters:
//    None.
//
// Return Value:
//    TRUE  - Continue shutdown
//    FALSE - Fail shutdown
//
//////////////////////////////////////////////////////////////////////////
// 05/20/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
// 05/27/1997 JBRENNA - Updated so that NAVAPW can link to this DLL
//////////////////////////////////////////////////////////////////////////
BOOL CApwNtCmd::ShutdownScan(
   HWND   hDlg)                         // [in] - Parent window
{
DWORD           dwReturn;
int             nRet;
char            szMessage[256];
char            szTitle[128];
BOOL            bShutdown = TRUE;

   nRet = LoadString(
             g_hInstance,
             IDS_NAVAP_FLOPPYAINFECTED,
             szMessage,
             sizeof (szMessage) );

   if ( nRet > 0 )
   {
      nRet = LoadString(
               g_hInstance,
               IDS_NAVAP_INFECTION_TITLE,
               szTitle,
               sizeof (szTitle) );
   }

   if ( nRet > 0 )                      // Did our strings load?
   {
      dwReturn = m_TalkAPService.SendCommand(NAVAPSVC_AP_SCANFLOPPYA);

      while ( (NAVAPSVC_FLOPPYINFECTED == dwReturn) &&
              (TRUE == bShutdown))
      {
                                          // Here we need to put up a systemmodal
                                          // message to MAKE them remove the floppy.
         if (IDCANCEL == MessageBox(
                           hDlg,
                           szMessage,
                           szTitle,
                           MB_OKCANCEL | MB_ICONHAND | MB_SYSTEMMODAL))
         {
            bShutdown = FALSE;
         }
         else
            dwReturn = m_TalkAPService.SendCommand(NAVAPSVC_AP_SCANFLOPPYA);
      }
   }

   return bShutdown;
}

//////////////////////////////////////////////////////////////////////////
// ConfirmApwComTasksReceived
//
// Purpose:
//    This function does nothing in NT.
//
// Parameters:
//    None.
//
// Return Value:
//    None.
//
//////////////////////////////////////////////////////////////////////////
VOID CApwNtCmd::ConfirmApwComTasksReceived( VOID )
{
}
