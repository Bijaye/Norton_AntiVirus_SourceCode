/////////////////////////////////////////////////////////////////////
// ALERTRPC.CPP - Created - 08/12/97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTSVC/VCS/AlertRPC.cpv   1.0   14 Aug 1997 18:20:32   DSACKIN  $
//
// Description:
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTSVC/VCS/AlertRPC.cpv  $
// 
//    Rev 1.0   14 Aug 1997 18:20:32   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//      Includes

// Quake & System Include Files
#include    "afxtempl.h"
#include    "platform.h"
#include    "process.h"
#include    "winnetwk.h"
#include    "lmcons.h"
#include    "lmmsg.h"
#include    "lmerr.h"

#include    "nonav.h"
#include    "inoc.h"
#include    "navalog.h"
#include    "navaplog.h"
#include    "navbin.h"
#include    "actions.h"
#include    "symalog.h"
#include    "navutil.h"
#include    "xapi.h"
#include    "file.h"
#include    "symstat.h"
#include    "navstat.h"

#include    "linksrv.h"
#include    "servenfy.h"

// NAV & AP Driver Include Files
#include    "drvcomm.h"
#include    "apcomm.h"
#include    "apscomm.h"             // For apps to communicate with me.

// Project Include files.
#include    "service.h"
#include    "svcctrl.h"
#include    "ALERTSVC.h"
#include    "ALERTSTR.H"

//***************************************************************************/
//      Constants
//***************************************************************************/
#define         MAX_THREAD_STACK_SIZE           (10240)  // Max Stack size
#define         MAX_AP_BUFSIZE                  (10240)
#define         MAX_SERVICE_NAME                (256)


/////////////////////////////////////////////////////////////////////
// Globals

/////////////////////////////////////////////////////////////////////
// Local Function Prototypes.

/////////////////////////////////////////////////////////////////////
// Functions



/////////////////////////////////////////////////////////////////////
// BOOL GetNOSAPDatFilename()
//
// Parameters:
//    none
//
// Returns:
//    TRUE  - We were able to get the nosap.dat filename.
//    FALSE - Unable to get the filename.
//
// Notes:
//    The filename is going to be put into m_szNOSAPDat.
//
/////////////////////////////////////////////////////////////////////
// 08/19/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CAlertService::GetNOSAPDatFilename(VOID)
{
BOOL   bReturn = FALSE;
DWORD  dwReturn;
LPTSTR lpszStringPointer;
TCHAR  szAlertSvcExe[SYM_MAX_PATH];
TCHAR  szNOSAPDat[SYM_MAX_PATH];
                                       // First build our ini filename
   dwReturn = GetModuleFileName(
                 m_hInst,
                 m_szNOSAPDat,
                 sizeof (m_szNOSAPDat) );

                                        // Check to make sure that we got a
                                        // filename.
   if (0 == dwReturn)
   {
      goto Cleanup;
   }

                                        // Get the name of the executable.
   dwReturn = LoadString (
                 m_hInst,
                 IDS_NAV_ALERT_PROGRAM_NAME,
                 szAlertSvcExe,
                 sizeof (szAlertSvcExe) / sizeof (TCHAR) );

   if (0 != dwReturn)
   {
                                        // Get the name of the executable.
      dwReturn = LoadString (
                    m_hInst,
                    IDS_NAV_ALERT_NOSAP_DAT,
                    szNOSAPDat,
                    sizeof (szNOSAPDat) / sizeof (TCHAR) );
   }

   if (0 == dwReturn)
   {
      goto Cleanup;
   }

                                        // Uppercase the modulefilename
   STRUPR (m_szNOSAPDat);

   lpszStringPointer = STRSTR (
                          m_szNOSAPDat,
                          szAlertSvcExe);

   if (NULL == lpszStringPointer)
   {
      goto Cleanup;
   }


                                        // Now add the NOSAP.DAT filename to
                                        // the end of our member variable.
   STRCPY (
      lpszStringPointer,
      szNOSAPDat);

                                        // Indicate our success state.
   bReturn = TRUE;

Cleanup:

                                        // If we fail to get the filename, we
                                        // should reset the member variable to
                                        // the state that it was in.
   if (FALSE == bReturn)
   {
      m_szNOSAPDat[0] = NULL;
   }

   return bReturn;
}


/////////////////////////////////////////////////////////////////////
// CheckRPCServer()
//
// Parameters:
//    none
//
// Returns:
//    TRUE  - Everything checks out fine
//    FALSE - Unable to start the RPC Server.
//
// Notes:
//    This function will turn on the RPC Server if it is off and we
//    are starting up.  It will turn OFF the RPC Server if the service
//    is shutting down.  It will also check to see if the status of
//    SAP enabling has changed and re-start the RPC Server with
//    the new settings if necessary.
//
/////////////////////////////////////////////////////////////////////
// 08/19/96 - DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
BOOL CAlertService::CheckRPCServer(VOID)
{
BOOL   bReturn = FALSE;
BOOL   bLocalEnableSAP;
DWORD  dwReturn;

                                        // Only one person at a time can be
                                        // mucking with the RPC Server.
   EnterCriticalSection(&m_csRPCServer);

                                        // Handle the case when we are stopping
                                        // the service first.
   if (m_bStoppingService)
   {
      if (m_bRPCServerListening)
      {
         m_RpcAlertServer.StopListen();
      }
      bReturn = TRUE;

      goto Cleanup;
   }

                                        // Check to see if we have figured out
                                        // what the filename is yet.
   if (NULL == m_szNOSAPDat[0])
   {
                                        // Try to get the NOSAP.DAT filename.
      if (!GetNOSAPDatFilename())
      {
                                        // If it failed, we can't continue.
         goto Cleanup;
      }

   }

                                        // First thing we need to figure out is
                                        // if we should enable sapping.
   dwReturn = GetFileAttributes(
                 m_szNOSAPDat);

                                        // The function GetFileAttributes
                                        // returnes 0xFFFFFFFF if there is an
                                        // error.  The docs don't tell of a
                                        // define for this.
   if (0xFFFFFFFF == dwReturn)
      bLocalEnableSAP = TRUE;
   else
      bLocalEnableSAP = FALSE;

                                        // Check for the case where everything
                                        // is fine so we can just exit.
   if ( m_bRPCServerListening &&
        (m_bEnableSAP == bLocalEnableSAP))
   {
                                        // OK.  Everything is under control.
      bReturn = TRUE;
      goto Cleanup;
   }


                                        // OK.  If the state is changing, we
                                        // must stop the RPC Server.
   if ( m_bRPCServerListening )
   {
      m_RpcAlertServer.StopListen();
      m_bRPCServerListening = FALSE;
   }

                                        // Update our member variable to our
                                        // NEW value.
   m_bEnableSAP = bLocalEnableSAP;

                                        // Set up our new sapping protocols.
                                        // Start our RPC server using
                                        // dynamic endpoint resolution.
   m_RpcAlertServer.SetProtocol(
      ( LPWSTR )NULL,
      ( LPWSTR )NULL,
      FALSE,                            // Don't use security!!!
      m_bEnableSAP);                    // Use our SAPPING status

                                        // Now start listening
   if (!m_RpcAlertServer.StartListen())
   {
      ErrorHandler (
        ID_ERR_STARTING_RPC_SERVER);

      goto Cleanup;
   }

                                        // If we got here, all is well and the
                                        // RPC Server is up and running.

   m_bRPCServerListening = TRUE;
   bReturn = TRUE;


Cleanup:

                                        // Let the next guy through that needs
                                        // to check on the RPC Server.
   LeaveCriticalSection(&m_csRPCServer);

   return bReturn;
}


