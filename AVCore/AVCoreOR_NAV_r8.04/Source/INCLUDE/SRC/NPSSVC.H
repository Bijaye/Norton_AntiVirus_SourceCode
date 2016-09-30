/////////////////////////////////////////////////////////////////////
// NPSServe.h
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/INCLUDE/VCS/npssvc.h_v   1.6   28 Aug 1998 17:51:04   JBRENNA  $
//
// Description:
//      This is the declaration file for the CNPSService class.
//
/////////////////////////////////////////////////////////////////////
// $Log:   S:/INCLUDE/VCS/npssvc.h_v  $
// 
//    Rev 1.6   28 Aug 1998 17:51:04   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.5.1.1   08 Jul 1998 10:35:50   mdunn
//    Nuked prototype for NpsRepeatingEventExpiredDisplay().
// 
//    Rev 1.5   11 Jun 1998 16:47:42   mdunn
// Changed prototype for NpsEventsCheckOld().
// 
//    Rev 1.4   17 Apr 1998 14:57:12   mdunn
// Added security descriptor member variable.
// 
//    Rev 1.3   30 Mar 1998 10:51:16   mdunn
// Added functions to support running missed events.
// 
//    Rev 1.2   18 Jul 1997 16:19:10   KSACKIN
// Removed the MkEventStr() function from this file.
//
//    Rev 1.1   02 Jun 1997 01:33:48   KSACKIN
// Updated the scheduler service to call functions from NPSAPI versus local
// fucntion calls for this class.
//
//    Rev 1.0   19 May 1997 00:11:02   KSACKIN
// Initial revision.
//
//    Rev 1.0   09 Apr 1996 11:30:22   DSACKING
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__NPSSERVE_H)
#define __NPSSERVE_H

#if !defined(__SERVICE_H)
#include "service.h"
#endif

#include "FileMon.h"                    // For CFileMonitor

/////////////////////////////////////////////////////////////////////
// Defines

#define MAX_THREAD_STACK_SIZE (4096)    // Max Stack size
#define SLEEP_TIME_SLICE      (5000)    // How many milliseconds to sleep.


/////////////////////////////////////////////////////////////////////
// Function Prototypes.

                                        // File Change Callback.
void NPSFileChangeNotificationCallBack( LPTSTR szChangedFileName );

/////////////////////////////////////////////////////////////////////
// CNPSService

class CNPSService : public CService
{
private:
   BOOL             m_bExiting;         // Flags for us when to exit
   BOOL             m_bReloadEvents;    // Flag for reloading events.
   BOOL             m_bCheckedOldEvents;
   CFileMonitor     m_cScheduleDataFileMonitor; // Watch Schedule.dat for change.

   PSECURITY_DESCRIPTOR m_pSecurityDescriptor;

public:
   CNPSService(LPSERVICE_MAIN_FUNCTIONW pMain,       // Pointer to our Main callback
               LPHANDLER_FUNCTION       pHandler);   // Pointer to our Handler callback

   ~CNPSService();                      // Destructor

   VOID InitService(VOID);              // Where we start it turning

   VOID WorkerThread(void);             // waits and calls events.

   VOID ServiceControlStop(VOID);

                                        // RPC entry point
   DWORD NPSSendCommand(
      LPVOID pPacket);                  // [in] - Packet from Client

                                        // Starting npsevnts.cpp declarations

   VOID MkEvent (                       /* Makes an event               */
       int         iEvent);             /* Event to make                */

//   VOID NpsGetMyPath(                   // Get command line and extracts path
//       LPSTR       lpszPath);           // This variable gets the path

//   WORD NpsEventsGet(                   /* Gets events from NPS.BIN file*/
//       BOOL        bInitLoad);          /* TRUE if initial load         */

//   WORD NpsEventGet(                    /* Gets events from NPS.BIN file*/
//       HFILE       fd,                  /* Handle to file               */
//       LPWORD      lpwEvent);           /* Buffer to current event cntr */

//   WORD NpsEventsPut(VOID);             /* Write events to NPS.BIN      */

   WORD NpsEventsCheckOld(BOOL bJustMarkMissedEvents);

   WORD NpsHandleMissedRepeatingEvent ( LPWORD pwEvent );

   WORD HandleMissedOneTimeEvent ( LPWORD pwEvent );

   WORD NpsEventExpiredDisplay ( WORD wEvent, int* pnAction );

   WORD NpsMakeNpsBak (                 /* Creates NpsBin.Bak file      */
       LPSTR       lpszBinFile,         /* Name of Bin file             */
       LPSTR       lpszBinBakFile);     /* Name of Backup file          */

                                        // Starting npsdo.cpp declarations

   WORD NpsEventsCheck (VOID);          /* Do we need to do an event?   */

   BOOL NpsCheckHourMin (
       WORD        wEventHour,
       WORD        wEventMin,
       WORD        wLastHour,
       WORD        wLastMin,
       WORD        wCurHour,
       WORD        wCurMin,
       BOOL        bCheckHour);

   WORD NpsEventDo (                    /* Executes event               */
       WORD        wEvent,              /* Index to event to execute    */
       PBOOL       pbRemoved);          /* TRUE if removed from list    */

   WORD NpsAnnounceEvent (              /* Anounces event with melody   */
       BOOL    bSound);                 /* Sound?                       */

   VOID NpsReturnCmdParams (            // Return command and its parameters
       LPCSTR      lpszFullCmdLine,     // [in] Full command line to parse
       LPSTR       lpszCmd,             // [out] Buffer to receive command
       LPSTR       lpszParams);         // [out] Buffer to receive parameters

   WORD NpsExecute (                    // Comvert to short params and exec.
       LPSTR   lpszCmd,                 // [in] command to execute
       LPSTR   lpszParams,              // [in] parameters
       LPSTR   lpszStartupDir,          // [in] startup dir
       int     nRunStyle);              // [in] run style

   void NpsSetReloadDataFlag( void );   // Set the data reloading flag to TRUE.

};


#endif

