// Copyright 1994 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/NAVAPW.NT/VCS/NAVAPCOM.CPv   1.0   24 Apr 1997 21:27:42   ksackin  $
//
// Description:
//      NAVAPCOM.CPP - This file contains the communication routines for
//      the Norton AntiVirus Auto-Protect Agent.
//
//      The design of this compontent provides a mechanism to allow
//      converstations with the VxD to go on in the background (as to not
//      delay or hang the UI) and to monitor any lockups.  Commands are sent
//      to the VxD asynchronously with a thread created per command issued.
//      A communications monitoring thread will be responsible for seeing if
//      any commands have locked up and will report the problem to the user,
//      terminating the problem command.  This monitor will also be
//      responsible for shutting down any commands that have hung before
//      the agent terminates.
//
//      There is only one command <NAME-HERE> which is not subject to
//      the monitor checking to see if it is hung.  This command is used for
//      asking the VxD when it has data for us and this could take quite a
//      long time (weeks, years...) before the VxD responds.  This mechanism
//      allows us to receive commands from the VxD when ever it feels the
//      need to tell us to do something for it.
//
// Contains:
//      NavApCommInit()
//      NavApCommDeInit()
//      NavApCommMonitorStart()
//      NavApCommMonitorThread()
//
// See Also:
//
//***************************************************************************
// $Log:   S:/NAVAPW.NT/VCS/NAVAPCOM.CPv  $
// 
//    Rev 1.0   24 Apr 1997 21:27:42   ksackin
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:12:22   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 12:13:26   DKEEFER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:52:34   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:14:48   BARRY
// Initial revision.
// 
//    Rev 1.23   12 Jul 1995 12:09:16   MARKL
// No longer spawns a thread for each command.  We simply loop.  We were 
// losing 4K for every 10 threads because MS wouldn't clean up KERNEL32 or
// Ring 0 of VMM until the last thread had terminated.  This was because
// we had a thread launching a thread, launching a thread, etc.  Since they
// keep track of the parent-child relationship of threads, they couldn't do
// any clean up until all of the threads were exited.  Grrrr...  The CORRECT
// solution would be to modify monitor to loop, spawning a thread for each
// command.  This way there wouldn't be a long parent-child relationship list
// for MS to maintain.  This fix, however, was quick and dirty so we could
// ship.
// 
// 
//    Rev 1.22   11 Jul 1995 17:21:12   MARKL
// No longer calls CreateThread() and ExitThread().  The following comes from
// the Microsoft documentation:
//     A thread that uses functions from the C run-time libraries should 
//     use the beginthread and endthread C run-time functions for thread 
//     management rather than CreateThread and ExitThread. Failure to do 
//     so results in small memory leaks when ExitThread is called. 
// 
//    Rev 1.21   08 Jul 1995 13:04:10   KEITH
// Pass OEM filenames to the VXD
// 
//    Rev 1.20   01 Jul 1995 19:35:28   MARKL
// Even though the documentation states that having a thread routine return
// will cause an implicit call to ExitThread, this is not the case.  We now
// call ExitThread() to make sure everything is nice and clean.
// 
//    Rev 1.19   24 Jun 1995 15:22:36   MARKL
// Now uses NotifyUser() instead of NMSGBOX()
// 
//    Rev 1.18   15 Jun 1995 16:02:58   MARKL
// Force VxD loading to be from the same module path.
// 
//    Rev 1.17   06 Jun 1995 17:20:00   MARKL
// Cleaned up globals.
// 
//    Rev 1.16   17 Apr 1995 20:31:02   RSTANEV
// Not disabling the VxD if loaded statically and the agent is terminating
// due to system shutdown.  Also, if the VxD is dynaloaded, we'll send the
// appropriate unload message so that NAVAP can scan floppies.
//
//    Rev 1.15   07 Apr 1995 18:41:36   RSTANEV
// Disabling the VxD when unloading the agent and the VxD is
// statically loaded.
//
//    Rev 1.14   03 Apr 1995 16:59:46   RSTANEV
// Sending a mesage to all top-level windows that the Auto-Protect had loaded
// the VxD.  Upon receiving this message, NAVW will disable the VxD for its
// thread.
//
//    Rev 1.13   07 Mar 1995 20:17:06   RSTANEV
// Not using shell2.h anymore.
//
//    Rev 1.12   21 Dec 1994 16:12:30   MARKL
// Cleaned up some async initialization and cleanup with the VxD by
// added ID_COMMAND_START_ASYNC_COMMANDS and ID_COMMAND_END_ASYNC_COMMANDS.
//
//    Rev 1.11   21 Dec 1994 15:09:24   MARKL
// Uses an event instead of a semaphore to wait for threads to terminate
// before disconnecting communications with the VxD.
//
//    Rev 1.10   21 Dec 1994 10:35:02   MARKL
// When the Agent is terminating, it now waits on a semaphore that
// guarentees that all of the communication threads have terminated before
// the app is allowed to exit.
//
//    Rev 1.9   20 Dec 1994 18:34:22   MARKL
// Now only tells the VxD if it was autoloaded ONLY if it actually was.
//
//    Rev 1.8   16 Dec 1994 14:05:14   MARKL
// Added the ID_COMMAND_VXD_UNLOADING message to inform the VxD we are
// going to unload it if it had been dynaloaded.
//
//    Rev 1.7   16 Dec 1994 13:28:16   MARKL
// We now tell the VxD we are terminating before communication threads are
// terminated.
//
//    Rev 1.6   13 Dec 1994 10:33:26   MARKL
// Some program initialization moved to NAVAPW.CPP where it belongs.
//
//    Rev 1.5   11 Dec 1994 15:19:52   MARKL
// Fixed memory routines where pointers to allocated buffers where not
// being passed properly to thread routines.  This caused GPFs during
// callback processing of asynchronous events.
//
//    Rev 1.4   09 Dec 1994 10:59:02   MARKL
// Smarter about autoloading the VxD and unloading it later.  Also sends a new
// message to the VxD (ID_COMMAND_VXD_AUTOLOADED with Module path) if it
// has been autoloaded.
//
//    Rev 1.3   09 Dec 1994 10:00:22   MARKL
// Did a lot of work on the communications routines.  The monitor is completed
// and the mechanism for the threads per command and callbacks on command
// completion is almost there.
//
//    Rev 1.2   08 Dec 1994 15:24:38   MARKL
// Re-engineered major routines to take on the model of having a thread per
// command when talking to the VxD.  Also added some communication maintenance
// routines as well.  A communications monitor thread was added and its data
// is stored in the first slot for ease.
//
//    Rev 1.1   07 Dec 1994 20:01:34   MARKL
// Added skeletons for communication initialization and deinitialization
// routines.  Also added routine to maintian a thread that does nothing but
// wait for data from the VxD and then respond appropriately.
//
//
//    Rev 1.0   06 Dec 1994 19:14:54   MARKL
// Initial revision.
//***************************************************************************



//***************************************************************************
// Include Files
//***************************************************************************
#include "platform.h"
#include "windowsx.h"
#include <process.h>
#include "xapi.h"
#include "msgdisp.h"                    // For error message display
#include "navapw.h"                     // Auto-Protect Agent items
#include "navapcom.h"                   // Agent <-> VxD communication items
#include "navapstb.h"                   // Agent string table ids.



//***************************************************************************
// Global Defs and External Data
//***************************************************************************
#define         TRACE                   ERR_TRACE_ID (_NAVAPCOM_CPP)

typedef struct
    {
    WORD        wThreadIndex;           // Index in comm thread table
    DWORD       dwCommandID;            // Command to send to VxD
    LPVOID      lpvInBuffer;            // Data for VxD
    DWORD       cbInBuffer;             // Size of data for VxD
    LPVOID      lpvOutBuffer;           // Where VxD returns data
    DWORD       cbOutBuffer;            // Size of return buffer
    LPDWORD     lpcbBytesReturned;      // Where VxD returns size of data
    CMDRESPPROC lpfnResponseCallback;   // Response callback proc
    } COMM_THREAD_CMD, FAR * LPCOMM_THREAD_CMD;


extern          NAVAPW_CONFIG           g_Cfg;
extern          NAVAPW_COMM             g_Comm;



//***************************************************************************
// NavApCommInit()
//
// Description:
//      This routine initializes the communications between the Norton
//      AntiVirus Auto-Protect Agent and its VxD.  It establishes a
//      communications handle to use and then allows the communications
//      manager to begin watching the communications link between the
//      agent and the VxD.
//
// Parameters:
//      hDlg            - [IN] Dialog box handle
//
// Return Value:
//      UINT    NOERR   - Routine worked and now talking with VxD
//              ERR     - Routine didn't work and cannot talk with VxD
//
// See Also:
//      NavApCommMonitorStart()
//
//***************************************************************************
// 12/06/1994 MARKL Function Created.
//***************************************************************************
UINT PASCAL NavApCommInit (             // Initializes communication with VxD
    HWND        hDlg)                   // [IN] Handle of apps dialog box
{
    HANDLE      hVxDComm        = NULL;
    WORD        wThreadIndex    = 0;
    UINT        uActMsg;
#ifdef AUTOLOAD_NAVAP_VXD
    char        szModulePath[SYM_MAX_PATH];
#endif

    SYM_ASSERT (hDlg);
    SYM_ASSERT (CFG_GET_INSTANCE);

                                        // Initialize our communications
                                        // table.
    COMM_SET_COMM_HANDLE (NULL);
    for (wThreadIndex = 0; wThreadIndex < MAX_COMM_THREADS; wThreadIndex++)
        {
        COMM_SET_COMM_THREAD_HANDLE (wThreadIndex, NULL);
        COMM_SET_COMM_THREAD_ID (wThreadIndex, 0);
        }

                                        // Establish a handle to talk with
                                        // the VxD with and store it for
                                        // future use.
    hVxDComm = CreateFile (NAVAP_VXD_COMM_NAME, 0, 0, 0, 0, 0, 0);
#ifdef AUTOLOAD_NAVAP_VXD
    if (hVxDComm == INVALID_HANDLE_VALUE)
        {
        LoadVxD (COMM_GET_VXD_IOCTL_DATA_PTR, 
                 NAVAP_VXD_NAME, 
                 VXDLOAD_MODULE_DIRECTORY);
        hVxDComm = CreateFile (NAVAP_VXD_COMM_NAME, 0, 0, 0, 0, 0, 0);
        COMM_SET_VXD_AUTOLOADED (TRUE);
        if (hVxDComm != INVALID_HANDLE_VALUE)
            {
            uActMsg = RegisterWindowMessage (NAVAP_ACTIVATING_MESSAGE);
            if (uActMsg != NULL)
                {
                SendMessage (HWND_BROADCAST, uActMsg, NULL, NULL);
                }
            }
        }
#endif

                                        // See if we are now talking
                                        // with the VxD
    if (hVxDComm == INVALID_HANDLE_VALUE)
        {
                                        // It isn't possible to talk to
                                        // the VxD since it isn't even
                                        // loaded.
        PostMessage (hDlg, APP_MESSAGE_ERR_NOTIFY,
            IDS_ERR_AP_VXD_NOT_LOADED,
            TRUE);
        return (ERR);
        }

                                        // Store our communications
                                        // handle.
    COMM_SET_COMM_HANDLE (hVxDComm);

                                        // Now start the communications
                                        // monitor that keeps an eye
                                        // on communications between our
                                        // Agent and the VxD.
    if (NavApCommMonitorStart (hDlg) != NOERR)
        {
                                        // No monitor--no communications...
                                        // NavApCommMonitorStart reports
                                        // its own error messages...
        CloseHandle (hVxDComm);
        COMM_SET_COMM_HANDLE (NULL);
        return (ERR);
        }

#ifdef AUTOLOAD_NAVAP_VXD
                                        // If we autoloaded the TSR, it
                                        // needs us to tell it when to
                                        // do initialization.
    if (COMM_GET_VXD_AUTOLOADED)
        {
        GetModuleFileName (CFG_GET_INSTANCE, szModulePath,
                           sizeof (szModulePath));
        NameReturnPath (szModulePath, szModulePath);
                                        // This must be in Oem.
        AnsiToOem ( szModulePath, szModulePath );
        DeviceIoControl (COMM_GET_COMM_HANDLE, ID_COMMAND_VXD_AUTOLOADED,
                         szModulePath, sizeof (szModulePath),
                         NULL, 0, NULL, NULL);
        }
#endif

                                        // Return to caller
    return (NOERR);
} // NavApCommInit()


//***************************************************************************
// NavAppCommDeInit()
//
// Description:
//      Deinitializes communications between the auto-protect agent and
//      its VxD.
//
// Parameters:
//      hDlg            - [IN] Dialog box handle
//
// Return Value:
//      NONE
//
//***************************************************************************
// 12/08/1994 MARKL Function Created.
//***************************************************************************
VOID PASCAL NavApCommDeInit (           // Deinitializes comm with VxD
    HWND        hDlg)                   // [IN] Handle of apps dialog box
{
    WORD        wThreadIndex            = 0;
    HANDLE      hTerminateEvent         = NULL;

    SYM_ASSERT (hDlg);

    if (COMM_GET_COMM_HANDLE && CFG_GET_TERMINATING_AGENT)
        {
                                        // Tell the VxD we are done
                                        // communicating with it.  After
                                        // setting up event to synchronize
                                        // unloading.
        if (COMM_GET_COMM_NUM_ASYNC_THREADS)
            {
            hTerminateEvent = CreateEvent (NULL, FALSE, FALSE, NULL);
            SYM_ASSERT (hTerminateEvent);
            COMM_SET_COMM_TERMINATE_EVENT (hTerminateEvent);
            }
        DeviceIoControl (COMM_GET_COMM_HANDLE,
                         ID_COMMAND_END_ASYNC_COMMANDS,
                         NULL, 0, NULL, 0, NULL, NULL);

                                        // We need to wait for our async
                                        // communication threads to
                                        // terminate gracefully.  If we
                                        // do not attempt to do this, the
                                        // VxD may unload with serious
                                        // errors.  We'll wait on an event.
        if (COMM_GET_COMM_TERMINATE_EVENT)
            {
            WaitForSingleObject (hTerminateEvent, MAX_COMM_THREAD_TIME);
            CloseHandle (hTerminateEvent);
            COMM_SET_COMM_TERMINATE_EVENT (hTerminateEvent = NULL);
            }

                                        // Tell VxD we are done cleaning
                                        // up our comm threads and that
                                        // we will proceed to terminate
                                        // now.
        DeviceIoControl (COMM_GET_COMM_HANDLE, ID_COMMAND_AGENT_TERMINATING,
                         NULL, 0, NULL, 0, NULL, NULL);

                                        // Disconnect communications handle
                                        // the VxD
#ifdef AUTOLOAD_NAVAP_VXD
        if (COMM_GET_VXD_AUTOLOADED)
            {
                                        // Tell the VxD he is going to be
                                        // unloaded since he wasn't loaded
                                        // before we started.
            DeviceIoControl (COMM_GET_COMM_HANDLE,
                             CFG_GET_UNLOADING_SHUTDOWN ?
                                ID_COMMAND_UNLOAD_VXD_ON_SYSSHUTDOWN :
                                ID_COMMAND_UNLOAD_VXD,
                             NULL, 0, NULL, 0, NULL, NULL);
            }
        else
#endif
            {
                                        // Disable the VxD when unloading
                                        // the agent.  Do this only if the
                                        // VxD is statically loaded and we
                                        // are terminating not due to
                                        // shutdown

            if ( !CFG_GET_UNLOADING_SHUTDOWN )
                {
                DeviceIoControl (COMM_GET_COMM_HANDLE, ID_COMMAND_DISABLE_VXD,
                                 NULL, NULL, NULL, 0, NULL, NULL);
                }
            }

        CloseHandle (COMM_GET_COMM_HANDLE);
        COMM_SET_COMM_HANDLE (NULL);

#ifdef AUTOLOAD_NAVAP_VXD
                                        // If we dynamically loaded the
                                        // VxD then remove our DynaLink.
                                        // He'll stay loaded if he was
                                        // referenced by Windows or somebody
                                        // else.
        if (COMM_GET_VXD_AUTOLOADED)
            {
            FreeVxD (COMM_GET_VXD_IOCTL_DATA_PTR);
            }
#endif
        }

                                        // Return to caller
} // NavApCommDeInit()



//***************************************************************************
// NavApCommMonitorStart()
//
// Description:
//      This routine starts the communications monitor thread for the
//      Norton AntiVirus Auto-Protect Agent.  The purpose of starting
//      this thread is to provide a mechanism to allow converstations
//      with the VxD to go on in the background (as to not delay or hang
//      the UI) and to monitor any lockups.
//
//      THREAD IS CREATED SUSPENDED SINCE WE NEED TO STORE INFORMATION
//      ABOUT IT AND IT MAY ACTUALLY TERMINATE **BEFORE** WE CAN FINISH
//      INITIALIZING INFO ABOUT IT.  WE RESUME THE THREAD ONCE WE ARE
//      DONE WITH OUR INITIALIZATION.  THREAD DOES ITS OWN CLEANUP.
//
// Parameters:
//      hDlg            - [IN] Dialog box handle
//
// Return Value:
//      UINT    NOERR   - Routine worked and now talking with VxD
//              ERR     - Routine didn't work and cannot talk with VxD
//
// See Also:
//
//***************************************************************************
// 12/08/1994 MARKL Function Created.
//***************************************************************************
UINT PASCAL NavApCommMonitorStart (     // Starts the comm monitor thread
    HWND        hDlg)                   // [IN] Dialog box handle
{
    HANDLE      hThread         = NULL;
    DWORD       dwThreadId      = 0;

    SYM_ASSERT (hDlg);
    SYM_ASSERT (COMM_GET_COMM_HANDLE);

                                        // Start a thread that monitors
                                        // all communications with the
                                        // VxD for the Agent.
#if 0
    hThread = CreateThread (NULL, 0,
                  (LPTHREAD_START_ROUTINE) NavApCommMonitorThread,
                  NULL, CREATE_SUSPENDED, &dwThreadId);
#else
    UINT        uThreadId;
    hThread = (HANDLE)_beginthreadex (NULL, 0,
                  NavApCommMonitorThread,
                  NULL, CREATE_SUSPENDED, &uThreadId);
    dwThreadId = uThreadId;
#endif


    if (hThread == NULL)
        {
                                        // Cannot start the monitor thread
                                        // so don't even bother talking to
                                        // the VxD...
        PostMessage (hDlg, APP_MESSAGE_ERR_NOTIFY,
            IDS_ERR_AP_VXD_CANNOT_TALK,
            TRUE);
        return (ERR);
        }

                                        // Store the information about our
                                        // communications monitor thread.
    COMM_SET_COMM_MON_HANDLE (hThread);
    COMM_SET_COMM_MON_ID (dwThreadId);
#if 0
    ResumeThread (hThread);
#else
    ResumeThread (hThread);
#endif

                                        // Return to caller
    return (NOERR);
} // NavApCommMonitorStart()



//***************************************************************************
// NavApCommMonitorThread()
//
// Description:
//      This thread is responsible for watching the communications threads
//
// Parameters:
//      lpvThreadParam                  - [IN] Thread parameters
//
// Return Value:
//
// See Also:
//
//***************************************************************************
// 12/08/1994 MARKL Function Created.
//***************************************************************************
UINT SYM_EXPORT PASCAL NavApCommMonitorThread ( // Monitors VxD comms
    LPVOID      lpvThreadParam)         // [IN] Thread parameters
{
    WORD        wThreadIndex    = 0;
    BOOL        bErrorReported  = FALSE;
    DWORD       dwCurTime       = 0;

    SYM_ASSERT (CFG_GET_HDLG);
    SYM_ASSERT (COMM_GET_COMM_HANDLE);
    SYM_ASSERT (COMM_GET_COMM_MON_HANDLE);
    SYM_ASSERT (COMM_GET_COMM_MON_ID);

                                        // We want to loop until our agent
                                        // has been shut down.
    while (TRUE)
        {
#ifdef MONITOR_ENABLED
                                        // Initialize for this round of
                                        // checking things out.
        bErrorReported  = FALSE;
        dwCurTime       = timeGetTime();

                                        // Go through all of our current
                                        // communication threads and see
                                        // if any have hung on us.
        for (wThreadIndex = 0;
             wThreadIndex < MAX_COMM_THREADS;
             wThreadIndex++)
            {
            if ((wThreadIndex == MONITOR_THREAD_INDEX) ||
                (COMM_GET_COMM_THREAD_HANDLE(wThreadIndex) == NULL) ||
                (COMM_GET_COMM_THREAD_COMMAND(wThreadIndex) ==
                 ID_COMMAND_GET_CMD_FROM_VXD))
                {
                                        // If this isn't a valid thread or
                                        // the monitor thread, or a wait
                                        // for VxD command thread nobody
                                        // cares how long it is idle.
                continue;
                }

            if (dwCurTime - COMM_GET_COMM_THREAD_START (wThreadIndex)
                > MAX_COMM_THREAD_TIME)
                {
                                        // Communications thread appears to
                                        // be hung.  We need to kill it
                                        // and only report the error once
                                        // per round as to not overload
                                        // the user with error boxes.
                NavApCommThreadTerminate (wThreadIndex, TRUE);
                if (!bErrorReported)
                    {
                    bErrorReported = TRUE;
                    PostMessage (CFG_GET_HDLG, APP_MESSAGE_ERR_NOTIFY,
                        IDS_ERR_AP_VXD_CANNOT_TALK,
                        FALSE);
                    }
                }
            }
#endif

                                        // Go to sleep until it is time
                                        // to check our communication
                                        // threads again.
        Sleep (MONITOR_SLEEP_TIME);
        }

                                        // Terminate thread.
#if 0
    ExitThread (0);
#else
    _endthreadex (0);
#endif
    return (NOERR);
} // NavApCommMonitorThread()



//***************************************************************************
// NavApCommThreadTerminate()
//
// Description:
//      Terminates a communication thread for a given thread index and does
//      all of the required cleanup.
//
// Parameters:
//      wThreadIndex                    - [IN] Refrences thread to terminate
//      bTerminate                      - [IN] TRUE if more than cleanup
//
// Return Value:
//
//***************************************************************************
// 12/08/1994 MARKL Function Created.
//***************************************************************************
VOID PASCAL NavApCommThreadTerminate (  // Terminates a communication thread
    WORD wThreadIndex,                  // [IN] Index to thread to terminate
    BOOL bTerminate)                    // [IN] TRUE if more than cleanup
{
    DWORD       dwLastError;
                                        // Terminate the desired thread.
    if (bTerminate)
        {
        if (!TerminateThread (COMM_GET_COMM_THREAD_HANDLE (wThreadIndex),
                              NULL))
            {
            dwLastError = GetLastError();
            }
        }

                                        // Perform the required cleanup.
    COMM_SET_COMM_THREAD_HANDLE (wThreadIndex, NULL);
    COMM_SET_COMM_THREAD_ID (wThreadIndex, 0);
    COMM_SET_COMM_THREAD_COMMAND (wThreadIndex, ID_COMMAND_NO_COMMAND);
    COMM_SET_COMM_THREAD_START (wThreadIndex, 0);
    COMM_SET_COMM_NUM_ASYNC_THREADS (COMM_GET_COMM_NUM_ASYNC_THREADS - 1);

                                        // If we are shutting down the
                                        // Agent, there is an event
                                        // waiting for all of the threads
                                        // to go away so we need to tell it
                                        // we are exiting.
    if (CFG_GET_TERMINATING_AGENT && COMM_GET_COMM_TERMINATE_EVENT)
        {
        if (COMM_GET_COMM_NUM_ASYNC_THREADS == 0)
            {
            SetEvent (COMM_GET_COMM_TERMINATE_EVENT);
            }
        }

                                        // Return to caller
} // NavApCommThreadTerminate()


//***************************************************************************
// NavApCommIssueAsyncCommand()
//
// Description:
//      This routine is called whenever the Auto-Protect Agent wishes to
//      have the VxD perform an asynchronous command for it.  It packages
//      the command request and the data packet and then spawns the
//      appropriate thread to service that request.  Once the thread has
//      completely serviced the request and the VxD has responded, the
//      optional lpfnResponseCallback routine is called to process the
//      results.
//
// Parameters:
//      dwCommandID                     - [IN] Command for VxD
//      lpvInBuffer                     - [IN] Data for VxD
//      cbInBuffer                      - [IN] Size of data for VxD
//      lpvOutBuffer                    - [IN] Where VxD returns data
//      cbOutBuffer                     - [IN] Size of return buffer
//      lpcbBytesReturnd                - [IN] Where VxD returns size of data
//      lpfnResponseCallback            - [IN] Response callback routine
//
// NOTE:
//      Use DeviceIoControl() for synchronous commands!!!  Much simpler...
//      This routine is really only for asynchronous commands or where
//      you want callbacks called once the VxD has processed the data.
//
//      ALL OF THE POINTERS TO BUFFERS MUST BE FROM MEMORY ALLOCATIONS
//      AND NOT VALUES OF POINTERS FROM BUFFERS ON THE STACK!!!
//      Since a thread will handle the execution of the command, the routine
//      that initiated this call could be long gone...
//
// Return Value:
//      UINT    NOERR   - Routine worked and now talking with VxD
//              ERR     - Routine didn't work and cannot talk with VxD
//
// See Also:
//
//***************************************************************************
// 12/08/1994 MARKL Function Created.
//***************************************************************************
UINT PASCAL NavApCommIssueAsyncCommand (// Issues async cmd to VxD via comm
    DWORD       dwCommandID,            // [IN] Command to send to VxD
    LPVOID      lpvInBuffer,            // [IN] Data for VxD
    DWORD       cbInBuffer,             // [IN] Size of data for VxD
    LPVOID      lpvOutBuffer,           // [IN] Where VxD returns data
    DWORD       cbOutBuffer,            // [IN] Size of return buffer
    LPDWORD     lpcbBytesReturned,      // [IN] Where VxD returns size of data
    CMDRESPPROC lpfnResponseCallback)   // [IN] Response callback proc
{
    WORD                wThreadIndex            = 0;
    HANDLE              hThread                 = NULL;
    DWORD               dwThreadId              = 0;
    LPCOMM_THREAD_CMD   lpCommThreadCmdRec      = NULL;

    SYM_ASSERT (CFG_GET_HDLG);
    SYM_ASSERT (dwCommandID);

                                        // Allocate memory for
                                        // passing data to our thread
                                        // since we cannot pass data
                                        // to it from the stack.  The
                                        // thread will free the memory
                                        // for us.
    lpCommThreadCmdRec =
        (LPCOMM_THREAD_CMD)MemAllocPtr (GMEM_MOVEABLE | GMEM_ZEROINIT,
                                        sizeof (COMM_THREAD_CMD));
    if (lpCommThreadCmdRec == NULL)
        {
        return (ERR);
        }

                                        // Package our parameters for the
                                        // thread.
    lpCommThreadCmdRec->dwCommandID          = dwCommandID;
    lpCommThreadCmdRec->lpvInBuffer          = lpvInBuffer;
    lpCommThreadCmdRec->cbInBuffer           = cbInBuffer;
    lpCommThreadCmdRec->lpvOutBuffer         = lpvOutBuffer;
    lpCommThreadCmdRec->cbOutBuffer          = cbOutBuffer;
    lpCommThreadCmdRec->lpcbBytesReturned    = lpcbBytesReturned;
    lpCommThreadCmdRec->lpfnResponseCallback = lpfnResponseCallback;

                                        // See which slot is vacant and
                                        // available for keeping track of
                                        // a thread for us.
    do
        {
        if (COMM_GET_COMM_THREAD_HANDLE (wThreadIndex))
            {
            continue;
            }

                                        // We have a valid slot so let's
                                        // begin work. We want to start a
                                        // new communications thread and pass
                                        // this data on to him such that
                                        // he can talk with the VxD for
                                        // us.
        lpCommThreadCmdRec->wThreadIndex = wThreadIndex;
#if 0
        hThread = CreateThread (NULL, 0,
                      (LPTHREAD_START_ROUTINE) NavApCommVxDCommandThread,
                      lpCommThreadCmdRec, NULL, &dwThreadId);
#else
    UINT        uThreadId;
        hThread = (HANDLE)_beginthreadex (NULL, 0,
                      NavApCommVxDCommandThread,
                      lpCommThreadCmdRec, NULL, &uThreadId);
    dwThreadId = uThreadId;
#endif

        if (hThread == NULL)
            {
                                        // Cannot create a thread so don't
                                        // even bother trying to talk to the
                                        // VxD about this command.
            PostMessage (CFG_GET_HDLG, APP_MESSAGE_ERR_NOTIFY,
                IDS_ERR_AP_VXD_CANNOT_TALK,
                FALSE);
            MemFreePtr (lpCommThreadCmdRec);
            return (ERR);
            }

                                        // Store the information about our
                                        // communications monitor thread.
        COMM_SET_COMM_THREAD_HANDLE (wThreadIndex, hThread);
        COMM_SET_COMM_THREAD_ID (wThreadIndex, dwThreadId);
        COMM_SET_COMM_THREAD_START (wThreadIndex, timeGetTime());
        COMM_SET_COMM_NUM_ASYNC_THREADS (COMM_GET_COMM_NUM_ASYNC_THREADS + 1);

                                        // Return to caller now that
                                        // thread is doing work for us.
        return (NOERR);
        } while (++wThreadIndex < MAX_COMM_THREADS);

                                        // Error, we are out of threads to
                                        // communicate.  Something must
                                        // be wrong with the VxD.  Maybe?
    PostMessage (CFG_GET_HDLG, APP_MESSAGE_ERR_NOTIFY,
        IDS_ERR_AP_VXD_CANNOT_TALK,
        FALSE);
    return (ERR);
} // NavApCommIssueAsyncCommand()



//***************************************************************************
// NavAppCommVxDCommandThread()
//
// Description:
//      This routine is responsible for issueing a command to the VxD
//      and hanging around until there is a response.  Depending on the
//      command the VxD is asked to execute, this routine may need to call
//      a callback to process the information returned by the VxD.
//
// NOTE:
//      This is how the ID_COMMAND_AGENT_WAIT_FOR_CMD command is implemented.
//      The Agent sends this command to the VxD knowing that the VxD will
//      but the thread to sleep and then respond when it has something that
//      it wants the agent to do for it.
//
//      This thread uses DeviceIOControl calls to communicate with the VxD.
// 
// Parameters:
//      lpvThreadParam                  - [IN] Really lpCommThreadCmdRec
// 
// Return Value: 
// 
// See Also: 
// 
//***************************************************************************
// 12/06/1994 MARKL Function Created.                                   
//***************************************************************************
UINT SYM_EXPORT PASCAL NavApCommVxDCommandThread (// Sends command to VxD
    LPVOID      lpvThreadParam)         // [IN] Really lpCommThreadCmdRec
{
    WORD wThreadIndex;

    SYM_ASSERT (COMM_GET_COMM_HANDLE);
    SYM_ASSERT (lpvThreadParam);

                                        // Obtain our thread index
                                        // for lookups in the table.
    wThreadIndex = ((LPCOMM_THREAD_CMD)lpvThreadParam)->wThreadIndex;
    SYM_ASSERT (wThreadIndex);

                                        // If we are NOT shutting down then
                                        // ask the VxD if there is something
                                        // else we can do for it today.
    while ( ! CFG_GET_UNLOADING_SHUTDOWN )
        {
                                        // Tell the VxD what it is we 
                                        // want him to do and wait (however
                                        // long) for his response.
        if (!DeviceIoControl (COMM_GET_COMM_HANDLE,
                 ((LPCOMM_THREAD_CMD)lpvThreadParam)->dwCommandID,
                 ((LPCOMM_THREAD_CMD)lpvThreadParam)->lpvInBuffer,
                 ((LPCOMM_THREAD_CMD)lpvThreadParam)->cbInBuffer,
                 ((LPCOMM_THREAD_CMD)lpvThreadParam)->lpvOutBuffer,
                 ((LPCOMM_THREAD_CMD)lpvThreadParam)->cbOutBuffer,
                 ((LPCOMM_THREAD_CMD)lpvThreadParam)->lpcbBytesReturned,
                 NULL))
             {
                                        // Exit the loop since the VxD
                                        // flagged us with an error.
             break;
             }

                                        // Call the callback that may be
                                        // specified with this command 
                                        // message.
        if (((LPCOMM_THREAD_CMD)lpvThreadParam)->lpfnResponseCallback)
            {
            CMDRESPPROC lpfnResponseCallback;
            LPVOID      lpvOutBuffer;
            LPVOID      lpcbBytesReturned;

            lpfnResponseCallback = ((LPCOMM_THREAD_CMD)lpvThreadParam)->lpfnResponseCallback;
            lpvOutBuffer = ((LPCOMM_THREAD_CMD)lpvThreadParam)->lpvOutBuffer;
            lpcbBytesReturned = ((LPCOMM_THREAD_CMD)lpvThreadParam)->lpcbBytesReturned;

            (lpfnResponseCallback)((LPVOID)lpvOutBuffer, 
                                   (LPDWORD)lpcbBytesReturned);
            }
                
                                        // Continue looping until it is
                                        // time to shut down.
    }

                                        // We are done with this thread
                                        // and wish to exit it properly
                                        // so instead of having it
                                        // terminated, we will exit it 
                                        // properly.  Note that MS docs are
                                        // incorrect, simply returning from
                                        // the thread routine DOES NOT
                                        // implicitly call ExitThread!!!
                                        // Free up any memory we may have
                                        // had allocated for this
                                        // command.
    if (((LPCOMM_THREAD_CMD)lpvThreadParam)->lpvOutBuffer)
        {
        MemFreePtr (((LPCOMM_THREAD_CMD)lpvThreadParam)->lpvOutBuffer);
        }
    if (((LPCOMM_THREAD_CMD)lpvThreadParam)->lpcbBytesReturned)
        {
        MemFreePtr (((LPCOMM_THREAD_CMD)lpvThreadParam)->lpcbBytesReturned);
        }
    if (lpvThreadParam)
        {
        MemFreePtr (lpvThreadParam);
        }
    NavApCommThreadTerminate (wThreadIndex, FALSE);
#if 0
    ExitThread (0);
#else
    _endthreadex (0);
#endif
    return 0;
} // NavApCommVxDCommandThread()





