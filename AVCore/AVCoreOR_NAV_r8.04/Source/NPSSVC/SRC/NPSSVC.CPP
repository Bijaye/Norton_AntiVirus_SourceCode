/////////////////////////////////////////////////////////////////////
// npssvc.cpp
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NPSSVC/VCS/npssvc.cpv   1.8   28 Aug 1998 17:11:46   JBRENNA  $
//
// Description:
//      This is the main implementation file for the CNPSService
//      class.  Here we derive from the base CService object and
//      override functions to get the Norton Program Scheduler service.
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/NPSSVC/VCS/npssvc.cpv  $
// 
//    Rev 1.8   28 Aug 1998 17:11:46   JBRENNA
// Port QAKN changes to trunk.
// 
//    Rev 1.7   11 Jun 1998 16:47:00   mdunn
// Fixed #116043 - missed events were being run erroneously by NpsEventsCheck()
//
//    Rev 1.6   17 Apr 1998 14:55:08   mdunn
// Revamped missed event handling
//
//    Rev 1.5   09 Apr 1998 13:17:30   JALLEE
// Check localtime() function return for NULL ptr before use.
// Fixes crash when date > 1/18/2038. Now fail silently.
//
//    Rev 1.4   30 Mar 1998 10:26:02   mdunn
// Added support for running missed events.
//
//    Rev 1.2   10 Feb 1998 22:50:16   KSACKIN
// Fixed a minor problem with our starting of the FileWatcher thread for the
// schedule.dat file before we set our callback function.  If we received a
// change notification immediately, we would not have our functions registered
// yet.
//
//    Rev 1.1   02 Jun 1997 01:31:26   KSACKIN
// Updated service to not give critical errors when the schedule.dat file is not
// present.  This is normal functionality if no scheduled items exist.
//
//    Rev 1.0   19 May 1997 00:03:06   KSACKIN
// Initial revision.
//
//    Rev 1.1   09 Apr 1996 11:43:16   DSACKING
// Changed NSTDHELP.H to STDHELP.H.
//
//    Rev 1.0   09 Apr 1996 11:35:30   DSACKING
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

// #define MAIN
#include <platform.h>
#include <process.h>

#include <xapi.h>
#include "stdhelp.h"
#include "service.h"                    // Prototypes of CService class

#include "npssvstr.h"                   // For string IDs

#include "npssvc.h"                   // For prototypes
#include "nps.h"
#include "npsstb.h"                     // For string resource IDs

#include "navrpc.h"                     // For protocol definitions.
#include "npsrpc.h"                     // Commands for NPS Service
#include "linksrv.h"                    // For CLinkServer
#include "FileMon.h"                    // For CFileMonitor

#include "time.h"

//***************************************************************************
// Local definitions
//***************************************************************************
#undef TRACE
#define TRACE ERR_TRACE_ID (_NPS_C)

                                        // It looks like the change to MSVC 5
                                        // broke the _CRTAPI1 macro. From
                                        // looking at preprocessor output,
                                        // under 4.2 it resolved to __cdecl
#ifndef _CRTAPI1
#define _CRTAPI1 __cdecl
#endif

//***************************************************************************
// Local constants
//***************************************************************************


//***************************************************************************
// Local typedefs
//***************************************************************************


/************************************************************************/
/* Local component prototypes                                           */
/************************************************************************/
VOID _CRTAPI1 main(void);

DWORD NPSSendCommandProc(
   LPVOID pPacket);                     // [in] - Packet from Client

BOOL AutorunOptionSet();

/************************************************************************/
/* Global Variables                                                     */
/************************************************************************/
DECLARE_SERVICE(CNPSService, SchedService, ID_ERR_REGISTERSERVICECTRLR_FAILED);

CLinkServer  g_NPSServer;               // Declare our RPC server object.
HELPSTRUCT   g_rHelpInfo;



/************************************************************************/
/* NPS main routines                                                    */
/************************************************************************/

/////////////////////////////////////////////////////////////////////
// CNPSService : Constructor
//
// Our constructor will initialize any variables that we will need.
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 04/08/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
CNPSService::CNPSService(
   LPSERVICE_MAIN_FUNCTIONW pMain,      // [in] - Pointer to our Main callback
   LPHANDLER_FUNCTION       pHandler) : // [in] - Pointer to our Handler callback
      CService(pMain, pHandler)         // Pass through initialization to
                                        // our base CService.
{
TCHAR szHelpFilePath [MAX_PATH];
//UINT  uHelpBrowseFileMsg;

   m_bExiting = FALSE;
   m_bReloadEvents = FALSE;
   m_bCheckedOldEvents = FALSE;
   m_pSecurityDescriptor = NULL;

   g_rHelpInfo.hinst = GetModuleHandle ( NULL );
   g_rHelpInfo.hwnd  = GetDesktopWindow();

   GetModuleFileName (g_rHelpInfo.hinst, szHelpFilePath, sizeof (szHelpFilePath));
   NameReturnPath (szHelpFilePath, szHelpFilePath);
   NameAppendFile (szHelpFilePath, _T("nsched32.hlp") );
   STRCPY (g_rHelpInfo.helpFileName, szHelpFilePath);
   StdHelpExInit (&g_rHelpInfo);

   //uHelpBrowseFileMsg = RegisterWindowMessage (HELPMSGSTRING);
}


/////////////////////////////////////////////////////////////////////
// CNPSService : Destructor
//
// This function does our cleanup
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 04/08/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
CNPSService::~CNPSService()
{
    if ( NULL != m_pSecurityDescriptor )
        {
        GlobalFree ( m_pSecurityDescriptor );
        m_pSecurityDescriptor = NULL;
        }
}


/////////////////////////////////////////////////////////////////////
// CNPSService :
//
// This function does our cleanup
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 04/08/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CNPSService::ServiceControlStop(VOID)
{
   m_bExiting = TRUE;

   ReportStatusToSCMgr (
      SERVICE_STOPPED,
      NO_ERROR,
      0);
}

/////////////////////////////////////////////////////////////////////
// void CNPSService::NpsSetReloadFlag( void );
//
// Purpose:
//  To set the reload flag for our data file.  This can be called
//  from clients of this class to let it know to reload it's events.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 05-18-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CNPSService::NpsSetReloadDataFlag( void )
{
                                        // Set our reload events flag to TRUE.
   m_bReloadEvents = TRUE;
}

/////////////////////////////////////////////////////////////////////
// CNPSService : WorkerThread
//
// This function does the actual work
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 04/08/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CNPSService::WorkerThread(VOID)
{
DWORD               dwReturn = WAIT_TIMEOUT;
HANDLE              hLoginEvent = NULL;
SECURITY_ATTRIBUTES rSA;

                                        // Create a security descriptor with
                                        // no access restrictions, so that
                                        // the user-mode NPSCHECK can access
                                        // the event we create.

   try
      {
      m_pSecurityDescriptor = GlobalAlloc ( GHND,
                                            SECURITY_DESCRIPTOR_MIN_LENGTH );

      if ( NULL == m_pSecurityDescriptor )
         throw 0;

      if ( !InitializeSecurityDescriptor (
                m_pSecurityDescriptor,
                SECURITY_DESCRIPTOR_REVISION ) )
         throw 0;

      if (!SetSecurityDescriptorDacl (
               m_pSecurityDescriptor,   // Pointer to our SID
               TRUE,                    // Are we adding a Dacl?
               (PACL) NULL,             // What Dacl? One that allows all access
               FALSE))                  // bDaclDefaulted - Specified by user
         throw 0;


      rSA.nLength = sizeof(SECURITY_ATTRIBUTES);
      rSA.lpSecurityDescriptor = m_pSecurityDescriptor;
      rSA.bInheritHandle = FALSE;

                                        // Create or open the event that
                                        // tells us when a user logs in. This
                                        // event is signaled by the NPSCHECK
                                        // app that runs from the Run key.
      hLoginEvent = CreateEvent ( &rSA, FALSE, FALSE, NPS_LOGIN_EVENT_NAME );

      if ( NULL == hLoginEvent )
         throw 0;

      if ( hLoginEvent && GetLastError()==ERROR_ALREADY_EXISTS )
         Sleep(10);
      }
   catch (int)
      {
      if ( NULL != m_pSecurityDescriptor )
         {
         GlobalFree ( m_pSecurityDescriptor );
         m_pSecurityDescriptor = NULL;
         }
      }


   // Do a preliminary pass thru the event list and mark any missed
   // events so that they don't get run by NpsEventsCheck() yet.

   NpsEventsGet(FALSE);
   NpsEventsCheckOld(TRUE);

   //
   // Used to prevent multiple disk hits preventing systems from going into standby mode.
   //
   BOOL bCheckIfCalledAutoRun = FALSE;
   BOOL bAutoRunMissedEvents  = FALSE;

   do
   {
                                        // Check to see if our reload flag has
                                        // been set.
      if ( TRUE == m_bReloadEvents )
         {
                                        // Reload the events.
         NpsEventsGet( FALSE );

                                        // Reset our flag to FALSE.
         m_bReloadEvents = FALSE;
         }


                                        // Check for any events that should
                                        // have been started, but were missed.
      if ( ! m_bCheckedOldEvents )
         {
         // We do the check here if a) the AutorunMissedEvents option is
         // set, or b) NPSCHECK has set the login event object to the
         // signaled state.

         //
         // Only hit the disk once...
         //
		 if ( ! bCheckIfCalledAutoRun )
		 	{
           
			bCheckIfCalledAutoRun = TRUE;
			bAutoRunMissedEvents  = AutorunOptionSet();

			}

		 if (  bAutoRunMissedEvents ||
			( NULL != hLoginEvent && WAIT_OBJECT_0 == WaitForSingleObject ( hLoginEvent, 0 ) ) )
		    {

            if ( NOERR != NpsEventsCheckOld(FALSE) )
               ErrorHandler ( ID_ERR_CHECKEVENTS );
            else
               {
               NpsEventsPut();       // Save changes made during the
                                     // missed events checking.  This
                                     // calls SignalReload().


                                     // If the Scheduler UI is up,
                                     // send WM_TIMECHANGE to make it reload
                                     // its event list.  SignalReload()
                                     // isn't doing the job.

               HWND hSchedUI = FindWindow ( _T("npsWClass"), NULL );

               if ( NULL != hSchedUI )
                  {
                  PostMessage ( hSchedUI, WM_TIMECHANGE, 0, 0 );
                  }
               }

            m_bCheckedOldEvents = TRUE;
            }   // end if (AutorunOptionSet()...)
         }  // end if (!m_bCheckedOldEvents)


      if (NOERR != NpsEventsCheck())
         ErrorHandler(ID_ERR_CHECKEVENTS);

      Sleep (SLEEP_TIME_SLICE);
   }
   while (FALSE == m_bExiting);         // Flag to tell us to exit.


   if ( NULL != hLoginEvent )
      {
      CloseHandle ( hLoginEvent );
      }
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    AutorunOptionSet()
//
// Description:
//  Reads SCHEDULE.INI and checks the value of the AutorunMissedEvents
//  entry.
//
// Input:
//  Nothing.
//
// Returns:
//  TRUE if the option is on (set to a nonzero value), or FALSE if the
//  option is off (zero).
//
//////////////////////////////////////////////////////////////////////////
// 3/30/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

BOOL AutorunOptionSet()
{
static BOOL s_bFirstCall = TRUE;

    if ( s_bFirstCall )
        {
                                        // Initialize a few global strings.
        s_bFirstCall = FALSE;

        ghInst = GetModuleHandle ( NULL );

        if ( NULL == ghInst ||
             0 == LoadString ( ghInst, IDS_NPS_TITLE, szTitle, ESIZE ) ||
             0 == LoadString ( ghInst, IDS_NPS_INI_TITLETIME, szTitleTime,
                               SSIZE )  ||
             0 == LoadString ( ghInst, IDS_NPS_NORTON_INI, szIniName, SSIZE ) )
            {
            return TRUE;                // If we can't load strings, default
                                        // to having autorun ON.
            }
        }

    return (0 != GetPrivateProfileInt ( szTitle, szTitleTime, 1, szIniName ))
           ? TRUE : FALSE;
}


/////////////////////////////////////////////////////////////////////
// WorkerThreadProc
//
// This function is a pass through function for our class's worker
// thread.  (CreateThread doesn pass the this parameter)
//
// Parameters:
//    LPVOID - Not used/thrown away.
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 04/08/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID WorkerThreadProc(void *lpObject)
{
CNPSService *lpThis;

   lpThis = (CNPSService *) lpObject;

   lpThis->WorkerThread();

                                        // Don't return.  We were spawned.
   _endthread();
}


/////////////////////////////////////////////////////////////////////
// CNPSService : InitService
//
// This function starts our service doing the work that it needs to.
//
// Parameters:
//    none
//
// Return Value:
//    none
//
/////////////////////////////////////////////////////////////////////
// 03/25/96 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
VOID CNPSService::InitService(VOID)
{
HANDLE          hThread;
LINKCALLBACKS   rCbk = { NPSSendCommandProc };
TCHAR           szScheduleDataFileName[SYM_MAX_PATH];
extern          char    *szNPSBinFile;

                                        // Get the path of the data file.
   NpsGetMyPath( szScheduleDataFileName );

                                        // Concatenate the data file name on to
                                        // our local path.
   STRCAT( szScheduleDataFileName, _T("\\") );
   STRCAT( szScheduleDataFileName, szNPSBinFile );

                                        // Set the file name of the data file
                                        // we wish to watch.
   m_cScheduleDataFileMonitor.SetFile( szScheduleDataFileName );

                                        // Set up our callback notification
                                        // function for when the Schedule.dat
                                        // file changes.
   m_cScheduleDataFileMonitor.SetCallback(
        (FM_FILE_CHANGE_CALLBACK) NPSFileChangeNotificationCallBack
        );

                                        // This function will start this file
                                        // monitor up watching this file on a
                                        // new thread and call our notification
                                        // function when any change occurrs.
   m_cScheduleDataFileMonitor.BeginWatchThread();


                                        // Load Events - TRUE means first time
                                        // This file does not need to be there.
                                        // If it is not, we ignore the return
                                        // value.
   NpsEventsGet(TRUE);


                                        // Initialize our server.
   g_NPSServer.SetProtocol(
      NPS_PROTOCOL,
      NPS_ENDPOINT);

                                        // Set up the callback
   g_NPSServer.SetCallbacks(
      &rCbk);
                                        // Now Create start listening
   if (!g_NPSServer.StartListen())
   {
      ErrorHandler(
         ID_ERR_STARTLISTEN |
         ID_ERR_FATAL);

      goto Cleanup;
   }

                                        // Start the scheduler thread

   hThread = (HANDLE)_beginthread (     //   CreateThread();
                           WorkerThreadProc,      // start Addr passes back to WorkerThread Member
                           MAX_THREAD_STACK_SIZE, // stack size
                           (LPVOID) this );       // Arg

   if (!hThread)
   {
      ErrorHandler(
         ID_ERR_BEGINTHREAD_FAILED |
         ID_ERR_FATAL);

      goto Cleanup;
   }

   if (!m_bExiting)
   {
      ReportStatusToSCMgr (
         SERVICE_RUNNING,
         NO_ERROR,
         0);
   }

Cleanup:

   return;
}


/////////////////////////////////////////////////////////////////////
// CNPSService::NPSSendCommand
//
// This is the procedure that will handle the incoming calls from our
// RPC server.
//
// Parameters:
//    LPVOID - lpPacket - Packet from client guy
//
// Return Value:
//    DWORD - TRUE - valid packet
//    DWORD - FALSE - invalid packet
//
/////////////////////////////////////////////////////////////////////
// 10/03/1996 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////
DWORD CNPSService::NPSSendCommand(
   LPVOID pPacket)                      // [in] - Packet from Client
{
DWORD dwReturn;
DWORD *lpdwInfo;

                                        // Check for invalid pointer
   if (NULL == pPacket)
   {
      dwReturn = FALSE;
      goto Cleanup;
   }

                                        // Set up our pointer
   lpdwInfo = (DWORD *) pPacket;

                                        // Make sure we got enough data
   if (*lpdwInfo < (2 * sizeof (DWORD)))
   {
      dwReturn = FALSE;
      goto Cleanup;
   }

                                        // Bump to the next DWORD (our command)
   lpdwInfo++;

   switch (*lpdwInfo)
   {
                                        // If we get the right message,
                                        // pass on to the main window
      case ID_NPS_RELOAD_EVENTS:
                                        // NpsEventsGet with FALSE signals that
                                        // this is not the first time that the
                                        // events are being loaded.
         if (NOERR != NpsEventsGet (FALSE))
         {
            ErrorHandler(
               ID_ERR_LOADEVENTS |
               ID_ERR_FATAL);

            dwReturn = FALSE;

            goto Cleanup;
         }
         break;
   }

Cleanup:

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
//  NPSSendCommandProc
//
// Purpose:
//    This is the entry point for our RPC calls
//
// Parameters:
//
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 10/03/96 - DSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
DWORD NPSSendCommandProc(
   LPVOID pPacket)                      // [in] - Packet from Client
{
DWORD dwReturn;

                                        // Call the member function in our
                                        // service that will take care of the
                                        // command.
   dwReturn = SchedService.NPSSendCommand(
                 pPacket);

   return dwReturn;
}


/////////////////////////////////////////////////////////////////////
// void NPSFileChangeNotificationCallBack( LPTSTR szChangedFileName )
//
// Purpose:
//    This function will get called by the CFileMonitor class that is
//    watching the Schedule.Dat file.  When the file changes, the
//    szChangedFileName parameter will be the full path of the file
//    that changed.
//
// Parameters:
//    LPTSTR szChangedFileName - [IN] Full path spec. to the file that
//                                    changed. (Schedule.dat)
//
// Returns:
//
//
// See Also:
//
//
/////////////////////////////////////////////////////////////////////
// 05-18-97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void NPSFileChangeNotificationCallBack( LPTSTR szChangedFileName )
{
                                        // Flag the service to reload the data
                                        // file at the next opportunity.
   SchedService.NpsSetReloadDataFlag();
}


/************************************************************************/
/* Description:                                                         */
/*      Check for old events that should be deleted.  This also         */
/*      initializes LastTime.                                           */
/*                                                                      */
/* VOID PASCAL NpsEventsCheckOld                                        */
/*                                                                      */
/* Input:                                                               */
/*  bJustMarkMissedEvents: [in] If TRUE, this fn does not run missed    */
/*      events, but instead sets the bMissedAlertPending member of      */
/*      the event's struct.  This prevents NpsEventsCheck() from        */
/*      erroneously running the event before the user has been          */
/*      notified.                                                       */
/*                                                                      */
/* Return values:                                                       */
/*              nothing                                                 */
/*                                                                      */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 03/29/91 MSL Made cleanup pass and query user to see if he really    */
/*              wants to have the event removed from the list.          */
/* 04/22/91 MSL Initializes LastTime so we know at the earliest time    */
/*              in which an event was last processed.                   */
/* 05/06/91 MSL Deletes event if on this minute.                        */
/* 10/14/93 BG  Made function global                                    */
/* 04/09/98 JALLEE Check return localtime() for NULL ptr.               */
/* 6/11/98 Mike Dunn -- added bJustMarkMissedEvents functionality       */
/************************************************************************/

WORD CNPSService::NpsEventsCheckOld ( BOOL bJustMarkMissedEvents )
{
    WORD                wRet;
    WORD                wEvent;
    struct      tm      *pCurtm, curtm;
    struct      _timeb  curtimeb;
    extern struct _timeb LastTime;      /* Last time processed.         */
    extern      NPSEVNT Events[];       /* The actual events            */


                                        /* Get the current date and     */
                                        /* time.                        */
    wRet = NOERR;
    _ftime(&curtimeb);
    LastTime = curtimeb;
                                        // Bug fix 4/9/98
                                        // localtime() returns NULL
                                        // if date > jan 18, 2038.
                                        // Don't need to run,
                                        // but we cannot crash.
                                        // => Fail silently.
    pCurtm = localtime (&curtimeb.time);

    if ( NULL != pCurtm )
    {
                                        // Copy the time returned by localtime()
                                        // to our own tm struct.  The other time
                                        // functions called later by CTime all 
                                        // share a static buffer, and blast the
                                        // time that localtime() just returned.
        CopyMemory ( &curtm, pCurtm, sizeof(struct tm) );
        pCurtm = &curtm;

        /* Check to see if we need to   */
        /* remove any expired events.   */
        for (wEvent = 0; wEvent < wNumEvents;)
        {
            if ( bJustMarkMissedEvents )
            {
                Events[wEvent].bMissedAlertPending = FALSE;
            }

            if (Events[wEvent].wFreq != NPS_FREQ_ONETIME)
            {
                if ( CTime ( Events[wEvent].rNextRunTime ) <=
                     CTime::GetCurrentTime() )
                    {
                    if ( bJustMarkMissedEvents )
                        {
                        Events[wEvent].bMissedAlertPending = TRUE;
                        wEvent++;
                        }
                    else
                        NpsHandleMissedRepeatingEvent ( &wEvent );
                    }
                else
                    {
                    wEvent++;
                    }

                continue;
            }

            if ((WORD)pCurtm->tm_year+1900 < Events[wEvent].wYear)
            {
                wEvent++;                   // future year
                continue;
            }
            if ((WORD)pCurtm->tm_year+1900 > Events[wEvent].wYear)
            {
                /* Previous year                */
                if ( bJustMarkMissedEvents )
                    {
                    Events[wEvent].bMissedAlertPending = TRUE;
                    wEvent++;
                    }
                else
                    HandleMissedOneTimeEvent (&wEvent);

                continue;
            }
            if ((WORD)pCurtm->tm_mon < Events[wEvent].wMonth)
            {
                wEvent++;                   // future month
                continue;
            }
            if ((WORD)pCurtm->tm_mon > Events[wEvent].wMonth)
            {
                /* Previous month               */
                if ( bJustMarkMissedEvents )
                    {
                    Events[wEvent].bMissedAlertPending = TRUE;
                    wEvent++;
                    }
                else
                    HandleMissedOneTimeEvent (&wEvent);

                continue;
            }
            if ((WORD)pCurtm->tm_mday < Events[wEvent].wDate)
            {
                wEvent++;
                continue;
            }
            if ((WORD)pCurtm->tm_mday > Events[wEvent].wDate)
            {
                if ( bJustMarkMissedEvents )
                    {
                    Events[wEvent].bMissedAlertPending = TRUE;
                    wEvent++;
                    }
                else
                    HandleMissedOneTimeEvent (&wEvent);

                continue;
            }
            if ((WORD)pCurtm->tm_hour < Events[wEvent].wHour)
            {
                wEvent++;
                continue;
            }
            if ((WORD)pCurtm->tm_hour > Events[wEvent].wHour)
            {
                if ( bJustMarkMissedEvents )
                    {
                    Events[wEvent].bMissedAlertPending = TRUE;
                    wEvent++;
                    }
                else
                    HandleMissedOneTimeEvent (&wEvent);

                continue;
            }
            if ((WORD)pCurtm->tm_min >= Events[wEvent].wMin)
            {
                if ( bJustMarkMissedEvents )
                    {
                    Events[wEvent].bMissedAlertPending = TRUE;
                    wEvent++;
                    }
                else
                    HandleMissedOneTimeEvent (&wEvent);

                continue;
            }
            wEvent++;
        }
    }
    return (wRet);
} /* NpsEventsCheckOld() */


//////////////////////////////////////////////////////////////////////////
//
// Function:    NpsHandleMissedOneTimeEvent
//
// Description:
//  Notifies the user that a one-time event didn't kick off because the
//  computer was off or NPS wasn't running, prompts the user for an action
//  (if the options allow that), and takes care of the event.
//
// Input:
//  pwEvent: [in/out] Event number.
//
// Returns:
//  NOERR if everything went OK.
//
//////////////////////////////////////////////////////////////////////////
// 3/20/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

WORD CNPSService::HandleMissedOneTimeEvent ( LPWORD pwEvent )
{
WORD wRet;
int nAction;
BOOL bRemove = FALSE;

    Events[*pwEvent].bMissedAlertPending = FALSE;

    if ( AutorunOptionSet() )
        {
        wRet = NpsEventDo ( *pwEvent, &bRemove );

                                        // If NpsEventDo() didn't remove the
                                        // event, then update *pwEvent.
        if ( wRet == NOERR  &&  !bRemove )
            {
            *pwEvent += 1;
            }
        }
    else
        {
                                        // Ask the user how to deal with
                                        // the event.
        NpsEventExpiredDisplay ( *pwEvent, &nAction );

        switch ( nAction )
            {
            case NPS_EXPIRED_RUN_NOW:
                wRet = NpsEventDo ( *pwEvent, &bRemove );

                                        // If NpsEventDo() didn't remove the
                                        // event, then update *pwEvent.
                if ( wRet == NOERR  &&  !bRemove )
                    {
                    *pwEvent += 1;
                    }
            break;

            case NPS_EXPIRED_DELETE:
                wRet = NpsEventRemove (*pwEvent);
            break;

            case NPS_EXPIRED_LEAVE_AS_IS:
                *pwEvent += 1;
            break;

            default:
                SYM_ASSERT(0);          // should never get here
            break;
            }
        }

                                        /* Return to caller.            */
    return (wRet);
}


//////////////////////////////////////////////////////////////////////////
//
// Function:    NpsHandleMissedRepeatingEvent
//
// Description:
//  Notifies the user that a repeating event didn't kick off because the
//  computer was off or NPS wasn't running, prompts the user for an action
//  (if the options allow that), and takes care of the event.
//
// Input:
//  pwEvent: [in/out] Event number.
//
// Returns:
//  NOERR if everything went OK.
//
//////////////////////////////////////////////////////////////////////////
// 3/20/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

WORD CNPSService::NpsHandleMissedRepeatingEvent ( LPWORD pwEvent )
{
WORD           wRet;
int            nAction;
BOOL           bRemove;
extern WORD    wNumEvents;
extern NPSEVNT Events[];                /* The actual events            */

                                        /* Initialization               */
    Events[*pwEvent].bMissedAlertPending = FALSE;
    bRemove = FALSE;
    wRet    = NOERR;

    if ( AutorunOptionSet() )
        {
        wRet = NpsEventDo ( *pwEvent, &bRemove );

                                        // If NpsEventDo() didn't remove the
                                        // event, then update *pwEvent.
        if ( wRet == NOERR  &&  !bRemove )
            {
            *pwEvent += 1;
            }
        }
    else
        {
                                        // Ask the user how to deal with
                                        // the event.
        NpsEventExpiredDisplay ( *pwEvent, &nAction );

        switch ( nAction )
            {
            case NPS_EXPIRED_RUN_NOW:
                wRet = NpsEventDo ( *pwEvent, &bRemove );

                                        // If NpsEventDo() didn't remove the
                                        // event, then update *pwEvent.
                if ( wRet == NOERR  &&  !bRemove )
                    {
                    *pwEvent += 1;
                    }
            break;

            case NPS_EXPIRED_DELETE:
                wRet = NpsEventRemove (*pwEvent);
            break;

            case NPS_EXPIRED_LEAVE_AS_IS:
                                        // Update the last-run time so we
                                        // won't think the event expired
                                        // again the next time NPS is run.

                CalculateNextRunTime ( &Events[*pwEvent] );

                *pwEvent += 1;
            break;

            default:
                SYM_ASSERT(0);          // should never get here
            break;
            }
        }

                                        /* Return to caller.            */
    return (wRet);
}


/************************************************************************/
/* Description:                                                         */
/*      This routine is the main entry point for the Scheduler.         */
/*                                                                      */
/************************************************************************/
/* 03/22/96 DRS Function created.                                       */
/************************************************************************/
VOID _CRTAPI1 main(void)
{
   SchedService.Create(TEXT("Norton Program Scheduler"));
}


//////////////////////////////////////////////////////////////////////////


