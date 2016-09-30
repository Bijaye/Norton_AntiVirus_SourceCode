// Copyright 1996 Symantec Corporation
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NAVAPW.NT/VCS/NAVAPCMD.CPv   1.0   24 Apr 1997 21:27:52   ksackin  $
//
// Description:
//    This is where the functions go that communicate through the pipe
//    to the NAVAP Service.
//
// Contains:
//    CmdDisableDriver()
//    CmdEnableDriver()
//    CmdGetStatusOfDriver()
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/NAVAPW.NT/VCS/NAVAPCMD.CPv  $
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
//
/////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////
// Include Files

#include "platform.h"
#include "navapw.h"                     // Auto-Protect Agent items
#include "msgdisp.h"
#include "navutil.h"                    // For NavXUtil.DLL routines
#include "ngui.h"
#include "navapstb.h"                   // Agent string table ids.

#include "svcctrl.h"                    // For Service Communications
#include "apscomm.h"

#include "apscli.h"                     // For communications with NAVAPSVC


/////////////////////////////////////////////////////////////////////
// Externs

extern          NAVAPW_CONFIG           g_Cfg;

extern          CNAVAPSVCClient         g_TalkAPService;

/////////////////////////////////////////////////////////////////////
// Defines

#define     TRACE     ERR_TRACE_ID (_NAVAPCMD_CPP)

#define     ERROR_CALLNAMEDPIPE   (0x1000)
#define     ERROR_LOADSTRING      (0x1001)

/////////////////////////////////////////////////////////////////////
// Local function prototypes

/////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////
// 05/08/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
/////////////////////////////////////////////////////////////////////
BOOL CmdWaitForStartupPending(VOID)
{
BOOL bReturn = TRUE;                    // Assume Success

   if ( NAVAPSVC_SUCCESS != g_TalkAPService.SendCommand(
                               NAVAPSVC_WAIT_FOR_STARTUP ) )
      bReturn = FALSE;

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////
// 05/08/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
/////////////////////////////////////////////////////////////////////
BOOL CmdDisableDriver(VOID)
{
BOOL bReturn = TRUE;                    // Assume Success

   if ( NAVAPSVC_DISABLED != g_TalkAPService.SendCommand(
                                NAVAPSVC_AP_DISABLE ) )
      bReturn = FALSE;

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////
// 05/08/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
/////////////////////////////////////////////////////////////////////
BOOL CmdEnableDriver(VOID)
{
BOOL bReturn = TRUE;                    // Assume Success

   if ( NAVAPSVC_ENABLED != g_TalkAPService.SendCommand(
                               NAVAPSVC_AP_ENABLE ) )
      bReturn = FALSE;

   return bReturn;
}



/////////////////////////////////////////////////////////////////////
// CmdGetStatusOfDriver
//
// Purpose:
//    This function will send a code through a pipe to the NAVAP
//    service which will query the status of the driver
//
// Parameters:
//    None.
//
// Return Value:
//    NAVAPSVC_ENABLED  - Device is enabled.
//    NAVAPSVC_DISABLED - Device is disabled.
//
/////////////////////////////////////////////////////////////////////
// 05/08/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
/////////////////////////////////////////////////////////////////////
DWORD CmdGetStatusOfDriver(VOID)
{
   return (g_TalkAPService.SendCommand(NAVAPSVC_GET_STATUS));
}


/////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////
// 05/20/1996 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CmdStartDriver(VOID)
{
CServiceControl cNavapsvc;
BOOL            bReturn = TRUE;         // Assume success until failure
char            szServiceName[SYM_MAX_PATH];
int             nRet;

   nRet = LoadString(
             CFG_GET_INSTANCE,
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


/////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////
// 05/20/1996 DSACKIN - Function Created
// 07/17/1996 DSACKIN - Reworked to use our RPC based object
/////////////////////////////////////////////////////////////////////
BOOL CmdShutdownScan(
   HWND   hDlg)                         // [in] - Parent window
{
DWORD           dwReturn;
int             nRet;
char            szMessage[256];
char            szTitle[128];
BOOL            bShutdown = TRUE;

   nRet = LoadString(
             CFG_GET_INSTANCE,
             IDS_NAVAP_FLOPPYAINFECTED,
             szMessage,
             sizeof (szMessage) );

   if ( nRet > 0 )
   {
      nRet = LoadString(
               CFG_GET_INSTANCE,
               IDS_NAVAP_INFECTION_TITLE,
               szTitle,
               sizeof (szTitle) );
   }

   if ( nRet > 0 )                      // Did our strings load?
   {
      dwReturn = g_TalkAPService.SendCommand(NAVAPSVC_AP_SCANFLOPPYA);

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
            dwReturn = g_TalkAPService.SendCommand(NAVAPSVC_AP_SCANFLOPPYA);
      }
   }

   return bShutdown;
}


