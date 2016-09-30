// Copyright 1994 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/NAVAPW/VCS/navapw.h_v   1.4   26 Mar 1998 15:53:12   DALLEE  $
//
// Description:
//      Contains dialog box and control ids for the NAVAPW component of the
// Norton AntiVirus Auto-Protect for Windows.
//
// NOTE:
//      If AUTOLOAD_NAVAP_VXD is defined, then we will auto-load the Norton
//      AntiVirus Auto-Protect VxD for referencing, and dereference it when
//      we are done.
//
// See Also:
//      NAVAPW.RC
//      NAVAPW.CPP
//
//***************************************************************************
// $Log:   S:/NAVAPW/VCS/navapw.h_v  $
// 
//    Rev 1.4   26 Mar 1998 15:53:12   DALLEE
// Removed opening bitmap defines and prototypes. We're no longer
// displaying a splash screen.
// 
//    Rev 1.3   16 Mar 1998 14:58:08   tcashin
// Added functions to start Navw or activate the correct Navw window.
// 
//    Rev 1.2   11 Feb 1998 10:52:44   DALLEE
// Added config setting ALLOW_CLOSE_AGENT. By default, close commands are
// now only minimizing the agent. Set the reg value "AllowCloseAgent" to revert
// back to the old behavior under NT.
// 
//    Rev 1.1   11 Jul 1997 12:14:02   DBUCHES
// Removed unused structure which would break compiliation with recent versions of MSVC
// 
//    Rev 1.0   24 Apr 1997 21:27:48   ksackin
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:12:22   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Nov 1996 12:13:30   DKEEFER
// Initial revision
// 
//    Rev 1.8   08 Aug 1996 02:12:28   DSACKIN
// Added the bIgnoreShutDown variable.
//
//    Rev 1.7   04 Aug 1996 22:16:32   DSACKIN
// Added 2 new macros for getting and setting startup peding state.
//
//    Rev 1.6   04 Aug 1996 22:11:56   DSACKIN
// Added prototype for CmdWaitForStartupPending
//
//    Rev 1.5   15 Jul 1996 16:58:00   DSACKIN
// Added a prototype for our prev instance function.
//
//    Rev 1.4   21 May 1996 15:40:26   DSACKIN
// Added prototype for CmdShutdownScan.
//
//    Rev 1.3   21 May 1996 00:54:24   DSACKIN
// Prototyped a new function that takes care of starting the NAVAP service.
//
//    Rev 1.2   08 May 1996 20:31:48   DSACKIN
// Added prototypes of the CmdXXX function.
//
//    Rev 1.1   06 May 1996 19:43:36   DSACKIN
// Updated function prototypes for first NT Version.
//
//    Rev 1.0   30 Jan 1996 15:52:36   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 10:14:50   BARRY
// Initial revision.
//***************************************************************************

#ifndef _NAVAPW_H
#define _NAVAPW_H



//***************************************************************************
// Include required NAV 3.0 include files for backwards compatability
//***************************************************************************
#include "navbin.h"                     // For reading NAVOPTS
#include "navconv.h"
#include "oldnavop.h"



//***************************************************************************
// Define what functionality we should have in the agent.
//***************************************************************************
#define AUTOLOAD_NAVAP_VXD
//#define MONITOR_ENABLED
//#define VXD_COMM_EMULATED
//#define ENABLE_DISABLE_BUTTON


                                        // Do we want the Agent to auto-
                                        // load the VxD???
#ifdef AUTOLOAD_NAVAP_VXD
    #include <vmmint.h>
    #include "vsymkrnl.h"
    #include "vxdload.h"
#endif

                                        // Do we want to emulate VxD
                                        // communications for debugging
                                        // purposes?
#ifdef VXD_COMM_EMULATED
    #undef      CreateFile
    #define     CreateFile      MyCreateFile
    #define     CloseHandle     MyCloseHandle
    #define     LoadVxD         MyLoadVxD
    #define     FreeVxD         MyFreeVxD
    #define     DeviceIoControl MyDeviceIoControl
#endif


#ifdef __cplusplus
extern "C" {
#endif



//***************************************************************************
// TRACE IDs for NMsgBox Message Display Routines
//***************************************************************************
#define _NAVAPW_CPP                     100
#define _NAVAPCOM_CPP                   101
#define _NAVAPCMD_CPP                   102
#define _NAVAPLOG_CPP                   103



//***************************************************************************
// Application defined window messages
//***************************************************************************
#define         APP_MESSAGE_TRAY_NOTIFY         (WM_APP + 100)
#define         APP_MESSAGE_REFRESH_TRAY        (WM_APP + 101)
#define         APP_MESSAGE_DO_INIT             (WM_APP + 102)
#define         APP_MESSAGE_DO_COMM_INIT        (WM_APP + 103)
#define         APP_MESSAGE_ERR_NOTIFY          (WM_APP + 104)
#define         APP_MESSAGE_DO_SHUTDOWN         (WM_APP + 105)
#define         APP_MESSAGE_DISPLAY_DIALOG      (WM_APP + 106)
#define         APP_MESSAGE_DO_RESTART          (WM_APP + 107)



//***************************************************************************
// Global Typedefs, Definitions, and Variables
//***************************************************************************
                                        // Type def and defines for Agent
                                        // configuration struture.
#define         AP_CAPTION_TEXT_SIZE            80
#define         BUTTON_TEXT_SIZE                40
#define         TRAY_TIP_TEXT_SIZE              80
typedef struct
    {
    HINSTANCE           hInst;
    HWND                hDlg;
    int                 nCmdShow;
    char                szAPCaptionText[AP_CAPTION_TEXT_SIZE];
    BOOL                bAutoProtectEnabled;
    BOOL                bHideIcon;
    BOOL                bTerminatingAgent;
    BOOL                bAllowEnableDisable;
    BOOL                bAllowUnload;
    BOOL                bUnloadingDueToShutdown;
    BOOL                bActivatePrevInstance;
    char                szActivityLogSpoolFile[SYM_MAX_PATH];
    UINT                uMessageFromNAVW32;
    UINT                uTrayTimerID;
    HICON               hTrayIconEnabled;
    HICON               hTrayIconDisabled;
    char                szButtonEnable[BUTTON_TEXT_SIZE];
    char                szButtonDisable[BUTTON_TEXT_SIZE];
    char                szTrayTipEnabled[TRAY_TIP_TEXT_SIZE];
    char                szTrayTipDisabled[TRAY_TIP_TEXT_SIZE];
    UINT                uWindowsVersion;
    BOOL                bNAVAPSVCStartPending;
    BOOL                bIgnoreShutDown;
    BOOL                bAllowCloseAgent;
    } NAVAPW_CONFIG, FAR * LPNAVAPW_CONFIG;

#define         CFG_GET_INSTANCE                   (g_Cfg.hInst)
#define         CFG_SET_INSTANCE(h)                (g_Cfg.hInst = (h))
#define         CFG_GET_HDLG                       (g_Cfg.hDlg)
#define         CFG_SET_HDLG(h)                    (g_Cfg.hDlg = (h))
#define         CFG_GET_CMDSHOW                    (g_Cfg.nCmdShow)
#define         CFG_SET_CMDSHOW(i)                 (g_Cfg.nCmdShow = (i))
#define         CFG_GET_CAPTION_TEXT_PTR           (g_Cfg.szAPCaptionText)
#define         CFG_SET_CAPTION_TEXT(t)            (STRCPY (CFG_GET_CAPTION_TEXT_PTR, (t))
#define         CFG_GET_ENABLE                     (g_Cfg.bAutoProtectEnabled)
#define         CFG_SET_ENABLE(b)                  (g_Cfg.bAutoProtectEnabled = (b))
#define         CFG_GET_HIDE_ICON                  (g_Cfg.bHideIcon)
#define         CFG_SET_HIDE_ICON(b)               (g_Cfg.bHideIcon = (b))
#define         CFG_GET_TERMINATING_AGENT          (g_Cfg.bTerminatingAgent)
#define         CFG_SET_TERMINATING_AGENT(b)       (g_Cfg.bTerminatingAgent = (b))
#define         CFG_GET_ALLOW_ENABLE_DISABLE       (g_Cfg.bAllowEnableDisable)
#define         CFG_SET_ALLOW_ENABLE_DISABLE(b)    (g_Cfg.bAllowEnableDisable = (b))
#define         CFG_GET_ALLOW_UNLOAD               (g_Cfg.bAllowUnload)
#define         CFG_SET_ALLOW_UNLOAD(b)            (g_Cfg.bAllowUnload = (b))
#define         CFG_SET_UNLOADING_SHUTDOWN(b)      (g_Cfg.bUnloadingDueToShutdown = (b))
#define         CFG_GET_UNLOADING_SHUTDOWN         (g_Cfg.bUnloadingDueToShutdown)
#define         CFG_SET_ACTIVATE_PREV_INST(b)      (g_Cfg.bActivatePrevInstance = (b))
#define         CFG_GET_ACTIVATE_PREV_INST         (g_Cfg.bActivatePrevInstance)
#define         CFG_GET_ACTLOG_SPOOL_FILE_PTR      (g_Cfg.szActivityLogSpoolFile)
#define         CFG_SET_MSG_ID_FROM_NAVW(u)        (g_Cfg.uMessageFromNAVW32 = (u))
#define         CFG_GET_MSG_ID_FROM_NAVW           (g_Cfg.uMessageFromNAVW32)
#define         CFG_SET_TRAY_TIMER_ID(u)           (g_Cfg.uTrayTimerID = (u))
#define         CFG_GET_TRAY_TIMER_ID              (g_Cfg.uTrayTimerID)
#define         CFG_SET_TRAY_ICON_ENABLED(h)       (g_Cfg.hTrayIconEnabled = ((HICON)h))
#define         CFG_GET_TRAY_ICON_ENABLED          (g_Cfg.hTrayIconEnabled)
#define         CFG_SET_TRAY_ICON_DISABLED(h)      (g_Cfg.hTrayIconDisabled = ((HICON)h))
#define         CFG_GET_TRAY_ICON_DISABLED         (g_Cfg.hTrayIconDisabled)
#define         CFG_GET_BUTTON_ENABLE_PTR          (g_Cfg.szButtonEnable)
#define         CFG_GET_BUTTON_DISABLE_PTR         (g_Cfg.szButtonDisable)
#define         CFG_GET_TRAY_TIP_ENABLED_PTR       (g_Cfg.szTrayTipEnabled)
#define         CFG_GET_TRAY_TIP_DISABLED_PTR      (g_Cfg.szTrayTipDisabled)
#define         CFG_SET_WINDOWS_VERSION(u)         (g_Cfg.uWindowsVersion = (u))
#define         CFG_GET_WINDOWS_VERSION            (g_Cfg.uWindowsVersion)
#define         CFG_GET_STARTUP_PENDING            (g_Cfg.bNAVAPSVCStartPending)
#define         CFG_SET_STARTUP_PENDING(b)         (g_Cfg.bNAVAPSVCStartPending = (b))
#define         CFG_GET_IGNORE_SHUTDOWN            (g_Cfg.bIgnoreShutDown)
#define         CFG_SET_IGNORE_SHUTDOWN(b)         (g_Cfg.bIgnoreShutDown = (b))
#define         CFG_GET_ALLOW_CLOSE_AGENT          (g_Cfg.bAllowCloseAgent)
#define         CFG_SET_ALLOW_CLOSE_AGENT(b)       (g_Cfg.bAllowCloseAgent = (b))


                                        // Type def and defines for Agent
                                        // communications struture.
#define         MAX_COMM_THREADS                10
#define         MONITOR_SLEEP_TIME              5000
                                        // Max time comm thread is to be
                                        // left hanging in miliseconds.
                                        // Ten seconds for now.
#define         MAX_COMM_THREAD_TIME    10000
#define         MONITOR_THREAD_INDEX    0

#if 0
typedef struct
    {
    HANDLE              hVxD;
    WORD                wNumAsyncThreads;
    HANDLE              hTerminateEvent;
    HANDLE              hThreads[MAX_COMM_THREADS];
    DWORD               dwThreadIDs[MAX_COMM_THREADS];
    DWORD               dwThreadStartTime[MAX_COMM_THREADS];
    DWORD               dwThreadCommandID[MAX_COMM_THREADS];
#ifdef AUTOLOAD_NAVAP_VXD
    BOOL                bVxDAutoloaded;
//    W32IOCTLDATA        wVxD32IoCtlData;
#endif
    } NAVAPW_COMM, FAR * LPNAVAPW_COMM;
#endif
    
#define         COMM_GET_COMM_HANDLE                (g_Comm.hVxD)
#define         COMM_SET_COMM_HANDLE(h)             (g_Comm.hVxD = (h))
#define         COMM_GET_COMM_NUM_ASYNC_THREADS     (g_Comm.wNumAsyncThreads)
#define         COMM_SET_COMM_NUM_ASYNC_THREADS(x)  (g_Comm.wNumAsyncThreads = (x))
#define         COMM_GET_COMM_TERMINATE_EVENT       (g_Comm.hTerminateEvent)
#define         COMM_SET_COMM_TERMINATE_EVENT(h)    (g_Comm.hTerminateEvent = (h))
#define         COMM_GET_COMM_THREAD_HANDLE(i)      (g_Comm.hThreads[(i)])
#define         COMM_SET_COMM_THREAD_HANDLE(i,h)    (g_Comm.hThreads[(i)] = (h))
#define         COMM_GET_COMM_THREAD_ID(i)          (g_Comm.dwThreadIDs[(i)])
#define         COMM_SET_COMM_THREAD_ID(i,id)       (g_Comm.dwThreadIDs[(i)] = (id))
#define         COMM_GET_COMM_THREAD_START(i)       (g_Comm.dwThreadStartTime[(i)])
#define         COMM_SET_COMM_THREAD_START(i,t)     (g_Comm.dwThreadStartTime[(i)] = (t))
#define         COMM_GET_COMM_THREAD_COMMAND(i)     (g_Comm.dwThreadCommandID[(i)])
#define         COMM_SET_COMM_THREAD_COMMAND(i,id)  (g_Comm.dwThreadCommandID[(i)] = (id))

                                        // NOTE:  The thread that monitors
                                        // all of our communications with
                                        // the VxD will be placed at index
                                        // 0 in the thread/id table.
#define         COMM_GET_COMM_MON_HANDLE            (COMM_GET_COMM_THREAD_HANDLE(MONITOR_THREAD_INDEX))
#define         COMM_SET_COMM_MON_HANDLE(h)         (COMM_SET_COMM_THREAD_HANDLE(MONITOR_THREAD_INDEX,(h)))
#define         COMM_GET_COMM_MON_ID                (COMM_GET_COMM_THREAD_ID(MONITOR_THREAD_INDEX))
#define         COMM_SET_COMM_MON_ID(id)            (COMM_SET_COMM_THREAD_ID(MONITOR_THREAD_INDEX,(id)))
#define         COMM_SET_COMM_MON_START             (COMM_SET_COMM_THREAD_START(MONITOR_THREAD_INDEX,t))
#ifdef AUTOLOAD_NAVAP_VXD
   #define      COMM_GET_VXD_AUTOLOADED             (g_Comm.bVxDAutoloaded)
   #define      COMM_SET_VXD_AUTOLOADED(b)          (g_Comm.bVxDAutoloaded = (b))
   #define      COMM_GET_VXD_IOCTL_DATA             (g_Comm.wVxD32IoCtlData)
   #define      COMM_GET_VXD_IOCTL_DATA_PTR         (&(g_Comm.wVxD32IoCtlData))
   #define      COMM_SET_VXD_IOCTL_DATA(x)          (g_Comm.wVxD32IoCtlData = (x))
#endif

                                        // Callback for optional response
                                        // routine for when VxD actually
                                        // returns from our command.
typedef UINT (CALLBACK* CMDRESPPROC)(LPVOID, LPDWORD);


//***************************************************************************
// Function Prototypes
//***************************************************************************
VOID PASCAL MainDlgRegisterClass(       // Registers class used by agent
    VOID);                              // No parameters

VOID PASCAL ActivatePrevInstance(       // Activates an already running agent
    VOID);                              // No parameters

HWND PASCAL MainDlgCreate(              // Creates the Main Dlg Window
    VOID);                              // No parameters

LRESULT SYM_EXPORT CALLBACK MainDlgProc ( // Main Dialog Box Proc
    HWND        hDlg,                   // [IN] Dialog box handle
    UINT        uMsg,                   // [IN] Message to process
    WPARAM      wParam,                 // [IN] wParam of message
    LPARAM      lParam);                // [IN] lParam of message

VOID PASCAL MainDlgInit (               // Initialize main dialog
    HWND        hDlg);                  // [IN] Dialog box handle

VOID PASCAL MainDlgDeInit (             // DeInitialize main dialog
    HWND        hDlg);                  // [IN] Dialog box handle

VOID PASCAL MainDlgCfgInit (            // Initializes configuration block
    HWND        hDlg);                  // [IN] Dialog box handle

UINT PASCAL MainDlgDeviceInit (         // Initializes Device for the Agent
    VOID);                              // No parameters

VOID PASCAL MainDlgDeviceCommInit (     // Initializes comm with VxD
    HWND        hDlg);                  // [IN] Dialog box handle

VOID PASCAL MainDlgEnableDisable (      // Enable/Disable Auto-Protect
    HWND        hDlg);                  // [IN] Dialog box handle

VOID PASCAL MainDlgShowEnableDisable(   // Shows appropriate states
    HWND        hDlg);                  // [IN] Dialog box handle

BOOL PASCAL MainDlgWM_COMMAND (         // Handles WM_COMMAND message
    HWND        hDlg,                   // [IN] Dialog box handle
    WPARAM      wParam,                 // [IN] wParam of message
    LPARAM      lParam);                // [IN] lParam of message

VOID PASCAL MainDlgUpdateInfoBox (      // Updates Info box in main dailog
    LPSTR       pszInfo);               // [IN] String (or string ID)

VOID PASCAL MainDlgErrNotify (          // Notify user about an error
    HWND        hDlg,                   // [IN] Dialog box handle
    UINT        uErrID,                 // [IN] Error message to display
    BOOL        bTerminate);            // [IN] TRUE to terminate app

BOOL PASCAL MainDlgTrayNotify (         // Handles tray notifications
    HWND        hDlg,                   // [IN] Dialog box handle
    WPARAM      wParam,                 // [IN] wParam of message
    LPARAM      lParam);                // [IN] lParam of message

VOID PASCAL MainDlgPopupTrayMenu (      // Build and display tray popup menu
    HWND        hDlg);                  // [IN] Dialog box handle

BOOL PASCAL MainDlgActivateNavw (       // Activate NAVW from tray's open menu item
    VOID);                              // No parameters

BOOL PASCAL CALLBACK MainDlgNavwThrdWinEnum ( // Identify NAVW's Scan Window
    HWND        hWnd,                   // [IN]  Window handle
    LPARAM      lParam);                // [OUT] Pointer to Scan Window's hWnd

void PASCAL MainDlgGetNavwExePath (     // Get the path to the NAV executable
    LPSTR       lpszNavw,               // [OUT] Full path to NAV
    LPSTR       lpszDir );              // [OUT] NAV directory

BOOL PASCAL MainDlgGetCurrentNavWindow (// Get the handle of the current Nav window
    LPHANDLE    lpMainWindow,           // [OUT] Handle of Navw's main window
    LPHANDLE    lpCurrentWindow);       // [OUT] Handle of Navw's current open window

BOOL PASCAL SendTrayMessage (           // Setup for call to Shell_NotifyIcon
    HWND        hDlg,                   // [IN] Dialog hwnd owning tray icon
    DWORD       dwMessage,              // [IN] Message to send tray icon
    HICON       hIcon,                  // [IN] Icon to set tray icon to
    LPSTR       lpszTip);               // [IN] Tip to assign to tray icon

HWND WINAPI OpeningBmpWindowCreate (    // Creates opening credits wnd
    HWND        hWndParent              // [in] Handle of parent
    );

UINT PASCAL NavApCommInit (             // Initializes communication with VxD
    HWND        hDlg);                  // [IN] Handle of apps dialog box

VOID PASCAL NavApCommDeInit (           // Deinitializes comm with VxD
    HWND        hDlg);                  // [IN] Handle of apps dialog box

UINT PASCAL NavApCommMonitorStart (     // Starts the comm monitor thread
    HWND        hDlg);                  // [IN] Dialog box handle

UINT SYM_EXPORT PASCAL NavApCommMonitorThread ( // Monitors VxD comms
    LPVOID      lpvThreadParam);        // [IN] Thread parameters

VOID PASCAL NavApCommThreadTerminate (  // Terminates a communication thread
    WORD wThreadIndex,                  // [IN] Index to thread to terminate
    BOOL bTerminate);                   // [IN] TRUE if more than cleanup

UINT PASCAL NavApCommIssueAsyncCommand (// Issues async cmd to VxD via comm
    DWORD       dwCommandID,            // [IN] Command to send to VxD
    LPVOID      lpvInBuffer,            // [IN] Data for VxD
    DWORD       cbInBuffer,             // [IN] Size of data for VxD
    LPVOID      lpvOutBuffer,           // [IN] Where VxD returns data
    DWORD       cbOutBuffer,            // [IN] Size of return buffer
    LPDWORD     lpcbBytesReturned,      // [IN] Where VxD returns size of data
    CMDRESPPROC lpfnResponseCallback);  // [IN] Response callback proc

UINT SYM_EXPORT PASCAL NavApCommVxDCommandThread (// Sends command to VxD
    LPVOID      lpvThreadParam);        // [IN] Really lpCommThreadCmdRec

UINT PASCAL NavApDoWaitForVxDCommand (  // Asks VxD for work to do
    VOID);                              // No parameters

UINT SYM_EXPORT CALLBACK NavApDoCommandForVxD (// Does cmds in W32 land for VxD
    LPVOID      lpCmdBuffer,            // [IN] Buffer with commands from VxD
    LPDWORD     lpdwBytesInBuffer);     // [IN] How many bytes in buffer

UINT PASCAL NavApLoadConfig (           // Loads configuration file for agent
    VOID);                              // No parameters

BOOL  CmdWaitForStartupPending(VOID);   // Call to wait for services to start.
BOOL  CmdDisableDriver(VOID);           // Call to disable the driver
BOOL  CmdEnableDriver(VOID);            // Call to enable
BOOL  CmdStartDriver(VOID);             // Call to start the driver
DWORD CmdGetStatusOfDriver(VOID);       // Call to get status

BOOL CmdShutdownScan(
   HWND   hDlg);                        // [in] - Parent window



//***************************************************************************
//  Communications Emulation Prototypes if VXD_COMM_EMULATED is defined
//***************************************************************************

#ifdef VXD_COMM_EMULATED

HANDLE PASCAL MyCreateFile (
    LPCSTR      lpszName,
    DWORD       fdwAccess,
    DWORD       fdwShareMode,
    LPSECURITY_ATTRIBUTES lpsa,
    DWORD       fdwCreate,
    DWORD       fdwAttrsAndFlags,
    HANDLE      hTemplateFile);

BOOL PASCAL MyCloseHandle (
    HANDLE hObject);

BOOL PASCAL MyDeviceIoControl (
    HANDLE      hDevice,
    DWORD       dwIoControlCode,
    LPVOID      lpvInBuffer,
    DWORD       cbInBuffer,
    LPVOID      lpvOutBuffer,
    DWORD       cbOutBuffer,
    LPDWORD     lpcbBytesReturned,
    LPOVERLAPPED lpoOverlapped);

#ifdef AUTOLOAD_NAVAP_VXD

UINT PASCAL MyLoadVxD (
    W32IOCTLDATA * pData,
    char const * lpszVxDFile,
    WORD wFlags );

UINT PASCAL MyFreeVxD (
    W32IOCTLDATA * pData);

#endif

#endif


#ifdef __cplusplus
}
#endif

#endif

