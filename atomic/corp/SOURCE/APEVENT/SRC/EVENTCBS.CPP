// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APEVENT/VCS/EVENTCBS.CPv   1.12   08 Dec 1998 19:53:40   RStanev  $
//
// Description:
//      This file contains all SYMEvent callbacks for NAVAP.
//
// Contains:
//      FILTER_INTERNAL_NTOS_EVENTS()  (local macro)
//
//      ECBApplicationExecute()        (local function, SYM_VXD only)
//      ECBFileOpen()                  (local function)
//      ECBObjectRename()              (local function)
//      ECBFileCreateBack()            (local function)
//      ECBFileCloseBack()             (local function)
//      ECBFileWrite()                 (local function)
//      ECBObjectSetAttributes()       (local function, SYM_VXD only)
//      ECBDeviceWritePhysical()       (local function, SYM_VXD only)
//      ECBDeviceWriteLogical()        (local function, SYM_VXD only)
//      ECBDeviceFormat()              (local function, SYM_VXD only)
//      ECBDeviceRead1()               (local function, different for each platform)
//      ECBDeviceRead1Async()          (SYM_VXD only)
//      ECBReboot()                    (SYM_VXD only)
//      ECBDeviceMount()               (local function, SYM_NTK only)
//
//      UserStackGenericECB1()         (local function)
//      UserStackGenericECB2()         (local function)
//      UserStackGenericECB3()         (local function)
//
//      InstallEventHooks()
//      UnInstallEventHooks()
//
//      NAVAPVxDInstallAsyncECB()      (local function, SYM_VXD only)
//      NAVAPVxDUnInstallAsyncECB()    (local function, SYM_VXD only)
//      NAVAPVxDScheduleAsyncECB()     (local function, SYM_VXD only)
//      NAVAPVxDCancelAsyncECB()       (local function, SYM_VXD only)
//
// See Also:
//      device.cpp
//
// Notes:
//      Event Manager module.
//
//      The event calbacks in this module are not very well structured,
//      but this is only for better performance.  Be careful when modifying
//      the way the event callbacks work - a lot of them depend on each
//      other, and a small change of the functionality of one callback
//      without making the proper changes to other callbacks may have
//      undesired effects.  Read all comments carefully!
//
//***************************************************************************
// $Log:   S:/APEVENT/VCS/EVENTCBS.CPv  $
//
//    Rev 1.12   08 Dec 1998 19:53:40   RStanev
// Added support for SE_DEV_READ for NT.
//
//    Rev 1.11   10 Jul 1998 15:49:00   RStanev
// Oops. Fixed NT compilations.
//
//    Rev 1.10   08 Jul 1998 16:13:54   RStanev
// Added asynchronous processing of device read events.
//
//    Rev 1.9   18 Feb 1998 22:19:20   RStanev
// Removed unknown virus detection.
//
//    Rev 1.8   17 Feb 1998 18:54:10   RStanev
// Updated stack swapping.  Merged NEC changes (MDUNN).
//
//    Rev 1.7   17 Feb 1998 16:54:20   RStanev
// Updated copyright.
//
//    Rev 1.6   05 Jul 1997 20:09:18   RStanev
// New stack swapping architecture.
//
//    Rev 1.5   18 Jun 1997 14:20:48   RStanev
// Removed reentrancy check in ECBReboot().
//
//    Rev 1.4   17 Jun 1997 21:03:04   RStanev
// Exported ECBReboot().
//
//    Rev 1.3   16 May 1997 02:55:58   RStanev
// Added ECBFileOpenError() to help us clean up virus sensor resources.
//
//    Rev 1.2   15 May 1997 14:41:46   RStanev
// Call UIOutOfMemoryError() whenever CoreLocalInit() failed.
//
//    Rev 1.1   15 May 1997 01:23:38   RStanev
// Too many changes to describe.
//
//    Rev 1.0   26 Feb 1997 04:34:14   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "apevent.h"
#include "pel.h"
#include "monitor.h"
#include "filter.h"

//---------------------------------------------------------------------------
//
// bEventHooksAreEnabled is used to globally enable and disable NAVAP's
// event hooks.  It is used by InstallEventHooks() and UnInstallEventHooks()
// to protect NAVAP from getting events until all event hooks are installed/
// uninstalled.
//
//---------------------------------------------------------------------------
BOOL bEventHooksAreEnabled = FALSE;

#if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// ASYNCEVENTDATA is a structure used to schedule events for asynchronous
// processing.  It has members for maintainig a linked list of these
// structures, as well as event information similar to the one provided by
// SYMEvent.
//
// Notes: pfnCancel should point to: void __cdecl cancel ( PASYNCEVENTDATA );
// pfnCallback should point to a function defined as a SYMEvent callback.
//
// WARNING: The offsets of members of this structure are hard-coded into
//          functions defined in ASYNCECB.ASM.  Keep the code synchronized!
//
//---------------------------------------------------------------------------
typedef struct tagASYNCEVENTDATA
    {
    tagASYNCEVENTDATA *prNext;
    DWORD             dwCancelTimeout;
    DWORD             dwEventHandle;
    PVOID             pfnCancel;
    PVOID             pfnCallback;
    REFDATA           uRefData;
    ED                rED;
    BYTE              abyExtraData[1];
    } ASYNCEVENTDATA, *PASYNCEVENTDATA;

//---------------------------------------------------------------------------
//
// prAsyncEDList is a pointer to a linked list of ASYNCEVENTDATA structures
// maintained by NAVAPVxD*AsyncECB().  Synchronization on this variable is
// done by the means of preventing page faults while working with the list.
// Be very careful when managing the code!
//
//---------------------------------------------------------------------------
#define DRIVER_VXD_LOCKED_DATA_SEGMENT
#include "drvseg.h"

PASYNCEVENTDATA prAsyncEDList = NULL;
PASYNCEVENTDATA prAsyncEDCancelList = NULL;

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Prototype event callbacks.  They should all be extern "C".
//
//---------------------------------------------------------------------------
extern "C" {

#if defined(SYM_VXD)

BOOL SECALLBACK ECBApplicationExecute (
    REFDATA uRefData,
    LPED    lpEventData
    );

#endif // #if defined(SYM_VXD)

BOOL SECALLBACK ECBFileOpen (
    REFDATA uRefData,
    LPED    lpEventData
    );

BOOL SECALLBACK ECBObjectRename (
    REFDATA uRefData,
    LPED    lpEventData
    );

BOOL SECALLBACK ECBFileCreateBack (
    REFDATA uRefData,
    LPED    lpEventData
    );

BOOL SECALLBACK ECBFileCloseBack (
    REFDATA uRefData,
    LPED    lpEventData
    );

BOOL SECALLBACK ECBFileWrite (
    REFDATA uRefData,
    LPED    lpEventData
    );

#if defined(SYM_VXD)

BOOL SECALLBACK ECBObjectSetAttributes (
    REFDATA uRefData,
    LPED    lpEventData
    );

BOOL SECALLBACK ECBDeviceWritePhysical (
    REFDATA uRefData,
    LPED    lpEventData
    );

BOOL SECALLBACK ECBDeviceWriteLogical (
    REFDATA uRefData,
    LPED    lpEventData
    );

BOOL SECALLBACK ECBDeviceFormat (
    REFDATA uRefData,
    LPED    lpEventData
    );

BOOL SECALLBACK ECBDeviceRead1Async (
    REFDATA uRefData,
    LPED    lpEventData
    );

#endif // #if defined(SYM_VXD)

BOOL SECALLBACK ECBDeviceRead1 (
    REFDATA uRefData,
    LPED    lpEventData
    );

#if defined(SYM_NTK)

BOOL SECALLBACK ECBDeviceMount (
    REFDATA uRefData,
    LPED    lpEventData
    );

#endif // #if defined(SYM_NTK)

}

//---------------------------------------------------------------------------
//
// Prototype user stack functions.
//
//---------------------------------------------------------------------------
VOID UserStackGenericECB1 (
    PEDEXTRA prEDExtra,
    REFDATA  uRefData,
    LPED     lpEventData
    );

VOID UserStackGenericECB2 (
    PBOOL    pbResult,
    PEDEXTRA prEDExtra,
    REFDATA  uRefData,
    LPED     lpEventData
    );

VOID UserStackGenericECB3 (
    PBOOL    pbResult,
    PEDEXTRA prEDExtra,
    REFDATA  uRefData,
    LPED     lpEventData
    );

//---------------------------------------------------------------------------
//
// Prototype functions for asynchronously processing events.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

VOID NAVAPVxDInstallAsyncECB (
    VOID
    );

VOID NAVAPVxDUnInstallAsyncECB (
    VOID
    );

VOID NAVAPVxDScheduleAsyncECB (
    REFDATA  uRefData,
    LPED     lpEventData,
    PVOID    pfnCallback
    );

extern "C" {

VOID __cdecl NAVAPVxDCancelAsyncECB (
    PASYNCEVENTDATA prCancel
    );

DWORD __cdecl ScheduleAsyncEventNotification (
    PASYNCEVENTDATA prNew );

DWORD __cdecl CancelAsyncEventNotification (
    PASYNCEVENTDATA prCancel );

};

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// The following structure is used to build a table with information about
// NAVAP hooks and their status.  The table is used for installing and
// removing SymEvent hooks.  NULL pointer in the EventHandler field of the
// structure marks the end of the table.
//
// All members of the structure, with the exception of hEvent are parameters
// to EventObjectCreate().  The hEvent member is used to save the handle
// returned by EventObjectCreate().
//
//---------------------------------------------------------------------------
typedef struct tagEVENTINITINFO
    {
    EVENTTYPE       EventType;
    EVENTFLAGS      EventFlags;
    EVENTHANDLER    EventHandler;
    REFDATA         RefData;
    LPARAM          lParam;
    HEVENT          hEvent;
    } EVENTINITINFO, *PEVENTINITINFO;

//---------------------------------------------------------------------------
//
// The variables of type SEDEVEVENTPARAMETER defined below are used when
// installing NAVAP's device mount, write, and format hooks.
//
// It's OK for the strings in the variables of SEDEVEVENTPARAMETER not to be
// in string files.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

SEDEVEVENTPARAMETER rDeviceReadLogicalInfo    = { 0, 1, _T("&*") };
SEDEVEVENTPARAMETER rDeviceMountLogicalInfo   = { 0, 0, _T("&*") };

#elif defined(SYM_VXD)

SEDEVEVENTPARAMETER rDeviceWritePhysicalInfo  = { 0, 1, _T(":*") };
SEDEVEVENTPARAMETER rDeviceWriteLogicalInfo   = { 0, 1, _T("&*") };
SEDEVEVENTPARAMETER rDeviceFormatPhysicalInfo = { 0, 0, _T(":*") };
SEDEVEVENTPARAMETER rDeviceReadLogicalInfo    = { 0, 1, _T("&*") };

#endif

//---------------------------------------------------------------------------
//
// The following table contains information about all NAVAP hooks and
// their status.  It is used for installing and removing the SymEvent hooks.
// A NULL pointer in the EventHandler field of the structure marks the end
// of the table.
//
// Notes:
//      A special case in the building of the table is the SE_DEV_WRITE
//      event, which has its LPARAM parameter initialized to NULL.  When
//      the hooks are being installed, and this entry of the table is
//      reached, the initialization function will install multiple hooks
//      to intercept physical writes to the logical boot records of all
//      devices in the device list.
//
//---------------------------------------------------------------------------
EVENTINITINFO rEventListTable[] =
    {
#if defined(SYM_VXD)

    { SE_LDR_APPLICATION_EXECUTE, SE_FLAGS_SYNC,                       &ECBApplicationExecute,  NULL, (LPARAM) NULL,                                      INVALID_EVENT_HANDLE },

#endif // #if defined(SYM_VXD)

    { SE_FS_FILE_OPEN,            SE_FLAGS_SYNC,                       &ECBFileOpen,            NULL, (LPARAM) NULL,                                      INVALID_EVENT_HANDLE },
    { SE_FS_OBJECT_RENAME,        SE_FLAGS_SYNC,                       &ECBObjectRename,        NULL, (LPARAM) NULL,                                      INVALID_EVENT_HANDLE },
    { SE_FS_FILE_CREATE,          SE_FLAGS_SYNC | SE_FLAGS_COMPLETION, &ECBFileCreateBack,      NULL, (LPARAM) NULL,                                      INVALID_EVENT_HANDLE },
    { SE_FS_FILE_CLOSE,           SE_FLAGS_SYNC | SE_FLAGS_COMPLETION, &ECBFileCloseBack,       NULL, (LPARAM) NULL,                                      INVALID_EVENT_HANDLE },
    { SE_FS_FILE_WRITE,           SE_FLAGS_SYNC,                       &ECBFileWrite,           NULL, (LPARAM) NULL,                                      INVALID_EVENT_HANDLE },

#if defined(SYM_VXD)

    { SE_FS_OBJECT_SETATTRIBUTES, SE_FLAGS_SYNC,                       &ECBObjectSetAttributes, NULL, (LPARAM) NULL,                                      INVALID_EVENT_HANDLE },
    { SE_DEV_WRITE,               SE_FLAGS_SYNC | SE_FLAGS_RANGEINFO,  &ECBDeviceWritePhysical, NULL, (LPARAM) &rDeviceWritePhysicalInfo.szDeviceFilter,  INVALID_EVENT_HANDLE },
    { SE_DEV_WRITE,               SE_FLAGS_SYNC | SE_FLAGS_RANGEINFO,  &ECBDeviceWritePhysical, NULL, (LPARAM) NULL,                                      INVALID_EVENT_HANDLE },
    { SE_DEV_WRITE,               SE_FLAGS_SYNC | SE_FLAGS_RANGEINFO,  &ECBDeviceWriteLogical,  NULL, (LPARAM) &rDeviceWriteLogicalInfo.szDeviceFilter,   INVALID_EVENT_HANDLE },
    { SE_DEV_FORMAT,              SE_FLAGS_SYNC,                       &ECBDeviceFormat,        NULL, (LPARAM) &rDeviceFormatPhysicalInfo.szDeviceFilter, INVALID_EVENT_HANDLE },
    { SE_DEV_READ,                SE_FLAGS_SYNC | SE_FLAGS_RANGEINFO,  &ECBDeviceRead1,         NULL, (LPARAM) &rDeviceReadLogicalInfo.szDeviceFilter,    INVALID_EVENT_HANDLE },
    { SE_MSC_REBOOT,              SE_FLAGS_SYNC,                       &ECBReboot,              NULL, (LPARAM) NULL,                                      INVALID_EVENT_HANDLE },

#endif // #if defined(SYM_VXD)

#if defined(SYM_NTK)

    { SE_DEV_READ,                SE_FLAGS_SYNC | SE_FLAGS_RANGEINFO,  &ECBDeviceRead1,         NULL, (LPARAM) &rDeviceReadLogicalInfo.szDeviceFilter,    INVALID_EVENT_HANDLE },
    { SE_DEV_MOUNT,               SE_FLAGS_SYNC,                       &ECBDeviceMount,         NULL, (LPARAM) &rDeviceMountLogicalInfo.szDeviceFilter,   INVALID_EVENT_HANDLE },

#endif // #if defined(SYM_NTK)

                                        // End of hook table

    { 0,                          0,                                   NULL,                    NULL, (LPARAM) NULL,                                      INVALID_EVENT_HANDLE }
    };

//---------------------------------------------------------------------------
//
// FILTER_INTERNAL_NTOS_EVENTS() is a SYM_NTK macro which checks whether an
// event is generated under the context of "NT AUTHORITY".  It does that by
// checking if the current event is generated in a non-impersonating thread
// of the system process.  Operations generating such events are assumed by
// NAVAP to be safe and done by the system only under extreme situations.
//
// This macro should be called directly from a SymEvent event hook.  If it
// determines that the current event is generated under a "system" context,
// the macro will automatically force the hook to return TRUE.  Note, that
// this is NOT a conventional method of using macros!  The proper
// implementation is as an inline function, but for whatever reason, the
// inline function did not optimize properly.
//
// FILTER_INTERNAL_NTOS_EVENTS() translates to ((void)0) if the platform is
// not SYM_NTK.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

#define FILTER_INTERNAL_NTOS_EVENTS()                                   \
                                                                        \
    if ( CoreIsThisNTSystemProcess() )                                  \
        {                                                               \
        auto NTSTATUS status;                                           \
        auto HANDLE   hToken;                                           \
                                                                        \
        status = ZvOpenThreadToken ( NtCurrentThread(),                 \
                                     TOKEN_QUERY,                       \
                                     TRUE,                              \
                                     &hToken );                         \
                                                                        \
        if ( NT_SUCCESS ( status ) )                                    \
            {                                                           \
            ZvClose ( hToken );                                         \
            }                                                           \
        else if ( status == STATUS_NO_TOKEN )                           \
            {                                                           \
            return ( TRUE );                                            \
            }                                                           \
        }                                                               \

#else

#define FILTER_INTERNAL_NTOS_EVENTS() ((void)0)

#endif

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBApplicationExecute (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when SYMEvent intercepts application execution.
//
// Entry:
//      Refer to SYMEvent documentation
//
// Exit:
//      TRUE  if SYMEvent should continue with the event
//      FALSE if SYMEvent should stop the event
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL SECALLBACK ECBApplicationExecute (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto BOOL    bResult;
    auto PVOID   pvCoreInit;
    auto EDEXTRA rEDExtra;

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( GetDefaultEventResult ( lpEventData, NULL ) );
        }

                                        // Add process to PEL.

    if ( !AddCurrentThreadToPEL1() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Continue processing on our own
                                        // stack.

    if ( !CallOnNAVAPStack ( UserStackGenericECB3,
                             &bResult,
                             &rEDExtra,
                             uRefData,
                             lpEventData ) )
        {
        bResult = GetDefaultEventResult ( lpEventData, NULL );
        }

                                        // Enable ourselves again.

    DeleteCurrentThreadFromPEL1();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( bResult );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBFileOpen (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when SYMEvent intercepts file open.
//
// Entry:
//      Refer to SYMEvent documentation
//
// Exit:
//      TRUE  if SYMEvent should continue with the event
//      FALSE if SYMEvent should stop the event
//
//---------------------------------------------------------------------------
BOOL SECALLBACK ECBFileOpen (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto BOOL    bResult;
    auto PVOID   pvCoreInit, pvSyncObject;
    auto EDEXTRA rEDExtra;

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Ignore internal NTOS events.

    FILTER_INTERNAL_NTOS_EVENTS();

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( GetDefaultEventResult ( lpEventData, NULL ) );
        }

                                        // Add process to PEL.

    if ( !AddCurrentThreadToPEL1() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Synchronize the open/create events
                                        // for the object at lpEventData.

    if ( ! ( pvSyncObject = FileOpenSyncEnter ( lpEventData ) ) )
        {
        DeleteCurrentThreadFromPEL1();
        CoreLocalDeInit ( pvCoreInit );
        return ( GetDefaultEventResult ( lpEventData, NULL ) );
        }

                                        // Continue processing on our own
                                        // stack.

    if ( !CallOnNAVAPStack ( UserStackGenericECB2,
                             &bResult,
                             &rEDExtra,
                             uRefData,
                             lpEventData ) )
        {
        bResult = GetDefaultEventResult ( lpEventData, NULL );
        }

                                        // End the open/create event
                                        // synchronization.

    FileOpenSyncExit ( pvSyncObject );

                                        // Enable ourselves again.

    DeleteCurrentThreadFromPEL1();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( bResult );
    }

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBObjectRename (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when SYMEvent intercepts file/dir rename.
//
// Entry:
//      Refer to SYMEvent documentation
//
// Exit:
//      TRUE  if SYMEvent should continue with the event
//      FALSE if SYMEvent should stop the event
//
// Note:
//      Renaming OS reserved filenames is an invalid procedure, therefore
//      we can assume that calling IsObjectAFile() from this event
//      callback is safe.
//
//---------------------------------------------------------------------------
BOOL SECALLBACK ECBObjectRename (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto BOOL    bResult;
    auto PVOID   pvCoreInit, pvSyncObject;
    auto EDEXTRA rEDExtra;

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Ignore internal NTOS events.

    FILTER_INTERNAL_NTOS_EVENTS();

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( GetDefaultEventResult ( lpEventData, NULL ) );
        }

                                        // Add process to PEL.

    if ( !AddCurrentThreadToPEL1() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Synchronize the open/create events
                                        // for the object at lpEventData.

    if ( ! ( pvSyncObject = FileOpenSyncEnter ( lpEventData ) ) )
        {
        DeleteCurrentThreadFromPEL1();
        CoreLocalDeInit ( pvCoreInit );
        return ( GetDefaultEventResult ( lpEventData, NULL ) );
        }

                                        // Ignore events that are generated
                                        // for objects other than files.

    if ( IsObjectAFile ( lpEventData->SE_ED_lpPathSpec ) )
        {
                                        // Continue processing on our own
                                        // stack.

        if ( !CallOnNAVAPStack ( UserStackGenericECB2,
                                 &bResult,
                                 &rEDExtra,
                                 uRefData,
                                 lpEventData ) )
            {
            bResult = GetDefaultEventResult ( lpEventData, NULL );
            }
        }
    else
        {
        bResult = TRUE;
        }

                                        // End the open/create event
                                        // synchronization.

    FileOpenSyncExit ( pvSyncObject );

                                        // Enable ourselves again.

    DeleteCurrentThreadFromPEL1();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( bResult );
    }

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBFileCreateBack (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when SYMEvent intercepts return from a file
// creation API call.
//
// Entry:
//      Refer to SYMEvent documentation
//
// Exit:
//      always TRUE
//
//---------------------------------------------------------------------------
BOOL SECALLBACK ECBFileCreateBack (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto PVOID   pvCoreInit;
    auto EDEXTRA rEDExtra;

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Ignore internal NTOS events.

    FILTER_INTERNAL_NTOS_EVENTS();

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( TRUE );
        }

                                        // Add process to PEL.

    if ( !AddCurrentThreadToPEL1() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Continue processing on our own
                                        // stack.

    CallOnNAVAPStack ( UserStackGenericECB1,
                       &rEDExtra,
                       uRefData,
                       lpEventData );

                                        // Enable ourselves again.

    DeleteCurrentThreadFromPEL1();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBFileCloseBack (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when SYMEvent intercepts return from a file
// closing API call.
//
// Entry:
//      Refer to SYMEvent documentation
//
// Exit:
//      always TRUE
//
// Note:
//      This function will process the event even if NAVAP was disabled.
//      The reason for this is because we need to release the per-file
//      resources which we might have allocated.
//
//---------------------------------------------------------------------------
BOOL SECALLBACK ECBFileCloseBack (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto UINT    uAttributes;
    auto PVOID   pvCoreInit, pvSyncObject;
    auto EDEXTRA rEDExtra;

#if defined(SYM_NTK)
	HKEY  hKey;
	DWORD nType, nValue, nSize;
#endif

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||

                                        // Check here some of the file
                                        // monitor flags before we proceed
                                        // with creating an event packet.

         !FileMonitorGetAttributes ( lpEventData, &uAttributes ) ||

                                        // If we have not seen the open of
                                        // this file, or if we have decided
                                        // to blow it off, then just return.

         ! ( uAttributes & FILEMONITOR_BABYSITTING ) ||

                                        // If we don't have anything to do on
                                        // close (such as scan on create or
                                        // modify), then ignore the event.

         ! ( ( uAttributes & FILEMONITOR_CHECKONCLOSE ) &&
             ( ( uAttributes & ( FILEMONITOR_CREATED |
                                 FILEMONITOR_SIZE0ATOPEN |
                                 FILEMONITOR_MODIFIED ) ) ||
               ( ( lpEventData->SE_ED_dwFlags2 & SE_FLAGS2_MODIFIED ) &&
                 FileMonitorSetAttributes ( lpEventData, FILEMONITOR_MODIFIED )
               )
             )
           )
       )
        {
        return ( TRUE );
        }


	// NAV exhibits a major slowdown when files are copied from an NT machine
	// to a remote NT machine mapped via Windows networking.  This is due to NAV 
	// scanning the file on the remote machine after it is copied and the close event
	// taking an extended amount of time to succeed on the remote machine.  A 
	// workaround for this is to not scan the file.  The following code reads 
	// a registry key to facilitate this.  (STS Incident #186718)

#if defined(SYM_NTK)

	if ( ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
					  _T("Software\\Symantec\\Norton AntiVirus\\Auto-Protect"),
					  0, 
					  KEY_READ, 
					  &hKey))
	{
		nSize = sizeof(DWORD);

		if ( ERROR_SUCCESS == RegQueryValueEx(hKey,
								_T("NetScanDisable"),
								0,
								&nType,
								(PBYTE)&nValue,
								&nSize ) )
		{
			// Found the key, see if it's set
			if (1 == nValue)
			{
				// It's set, so exit and do not scan the file
				RegCloseKey (hKey);
				return (TRUE);
			}
		}

		RegCloseKey (hKey);
	}

#endif

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( TRUE );
        }

                                        // Add to PEL.

    if ( !AddCurrentThreadToPEL1() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Synchronize the open/create events
                                        // for the object at lpEventData.

    if ( ! ( pvSyncObject = FileOpenSyncEnter ( lpEventData ) ) )
        {
        DeleteCurrentThreadFromPEL1();
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Continue processing on our own
                                        // stack.

    CallOnNAVAPStack ( UserStackGenericECB1,
                       &rEDExtra,
                       uRefData,
                       lpEventData );

                                        // End the synchronization of the
                                        // open/create events for the object
                                        // at lpEventData.

    FileOpenSyncExit ( pvSyncObject );

                                        // Enable ourselves again.

    DeleteCurrentThreadFromPEL1();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBFileWrite (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when SYMEvent intercepts file writes.
//
// Entry:
//      Refer to SYMEvent documentation
//
// Exit:
//      TRUE  if SYMEvent should continue with the event
//      FALSE if SYMEvent should stop the event
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

BOOL SECALLBACK ECBFileWrite (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto UINT uAttributes;

                                        // Get the monitor attributes for
                                        // this file.

    if ( FileMonitorGetAttributes ( lpEventData, &uAttributes ) )
        {
        if ( ! ( uAttributes & FILEMONITOR_MODIFIED ) )
            {
                                        // Mark this file as written to (even
                                        // if the write fails later!)  Also,
                                        // we don't need to check whether the
                                        // FILEMONITOR_BABYSITTING flag is
                                        // set.  The code which uses
                                        // FILEMONITOR_MODIFIED will take
                                        // care of that check.

            FileMonitorSetAttributes ( lpEventData, FILEMONITOR_MODIFIED );

                                        // Remove this file from the cache
                                        // with clean files.

            RemoveFileFromCleanCache ( lpEventData );
            }
        }

    return ( TRUE );
    }

#else

BOOL SECALLBACK ECBFileWrite (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto BOOL    bResult;
    auto PVOID   pvCoreInit;
    auto EDEXTRA rEDExtra;

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( GetDefaultEventResult ( lpEventData, NULL ) );
        }

                                        // Add process to PEL.

    if ( !AddCurrentThreadToPEL1() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Continue processing on our own
                                        // stack.

    if ( !CallOnNAVAPStack ( UserStackGenericECB2,
                             &bResult,
                             &rEDExtra,
                             uRefData,
                             lpEventData ) )
        {
        bResult = GetDefaultEventResult ( lpEventData, NULL );
        }

                                        // Enable ourselves again.

    DeleteCurrentThreadFromPEL1();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( bResult );
    }

#endif

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBObjectSetAttributes (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when SYMEvent intercepts change of file/dir
// attributes.
//
// Entry:
//      Refer to SYMEvent documentation
//
// Exit:
//      TRUE  if SYMEvent should continue with the event
//      FALSE if SYMEvent should stop the event
//
// Note:
//      Changing the attributes of OS reserved filenames is probably an
//      invalid action, therefore we can assume that calling IsObjectAFile()
//      from this event callback is safe.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL SECALLBACK ECBObjectSetAttributes (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto BOOL    bResult;
    auto PVOID   pvCoreInit;
    auto EDEXTRA rEDExtra;

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( GetDefaultEventResult ( lpEventData, NULL ) );
        }

                                        // Add process to PEL.

    if ( !AddCurrentThreadToPEL1() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // We care only about change of the
                                        // attributes of real files.

    if ( IsObjectAFile ( lpEventData->SE_ED_lpPathSpec ) )
        {
                                        // Continue processing on our own
                                        // stack.

        if ( !CallOnNAVAPStack ( UserStackGenericECB2,
                                 &bResult,
                                 &rEDExtra,
                                 uRefData,
                                 lpEventData ) )
            {
            bResult = GetDefaultEventResult ( lpEventData, NULL );
            }
        }
    else
        {
        bResult = TRUE;
        }

                                        // Enable ourselves again.

    DeleteCurrentThreadFromPEL1();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( bResult );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBDeviceWritePhysical (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when SYMEvent intercepts writes to the first
// physical sector on a device.
//
// Entry:
//      Refer to SYMEvent documentation
//
// Exit:
//      TRUE  if SYMEvent should continue with the event
//      FALSE if SYMEvent should stop the event
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL SECALLBACK ECBDeviceWritePhysical (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto BOOL    bResult;
    auto PVOID   pvCoreInit;
    auto EDEXTRA rEDExtra;

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( GetDefaultEventResult ( lpEventData, NULL ) );
        }

                                        // Add process to PEL.

    if ( !AddCurrentThreadToPEL1() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Continue processing on our own
                                        // stack.

    if ( !CallOnNAVAPStack ( UserStackGenericECB2,
                             &bResult,
                             &rEDExtra,
                             uRefData,
                             lpEventData ) )
        {
        bResult = GetDefaultEventResult ( lpEventData, NULL );
        }

                                        // Enable ourselves again.

    DeleteCurrentThreadFromPEL1();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( bResult );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBDeviceWriteLogical (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when SYMEvent intercepts writes to the first
// logical sector on a device.
//
// Entry:
//      Refer to SYMEvent documentation
//
// Exit:
//      TRUE  if SYMEvent should continue with the event
//      FALSE if SYMEvent should stop the event
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL SECALLBACK ECBDeviceWriteLogical (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto BOOL    bResult;
    auto PVOID   pvCoreInit;
    auto EDEXTRA rEDExtra;

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( GetDefaultEventResult ( lpEventData, NULL ) );
        }

                                        // Add process to PEL.

    if ( !AddCurrentThreadToPEL1() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Continue processing on our own
                                        // stack.

    if ( !CallOnNAVAPStack ( UserStackGenericECB2,
                             &bResult,
                             &rEDExtra,
                             uRefData,
                             lpEventData ) )
        {
        bResult = GetDefaultEventResult ( lpEventData, NULL );
        }

                                        // Enable ourselves again.

    DeleteCurrentThreadFromPEL1();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( bResult );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBDeviceFormat (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when SYMEvent intercepts format of a device.
//
// Entry:
//      Refer to SYMEvent documentation
//
// Exit:
//      TRUE  if SYMEvent should continue with the event
//      FALSE if SYMEvent should stop the event
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL SECALLBACK ECBDeviceFormat (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto BOOL    bResult;
    auto PVOID   pvCoreInit;
    auto EDEXTRA rEDExtra;

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( GetDefaultEventResult ( lpEventData, NULL ) );
        }

                                        // Add process to PEL.

    if ( !AddCurrentThreadToPEL1() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Continue processing on our own
                                        // stack.

    if ( !CallOnNAVAPStack ( UserStackGenericECB2,
                             &bResult,
                             &rEDExtra,
                             uRefData,
                             lpEventData ) )
        {
        bResult = GetDefaultEventResult ( lpEventData, NULL );
        }

                                        // Enable ourselves again.

    DeleteCurrentThreadFromPEL1();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( bResult );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBDeviceRead1 (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when SYMEvent intercepts reads to logical sector
// 1 of a device.
//
// Entry:
//      Refer to SYMEvent documentation
//
// Exit:
//      always TRUE
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL SECALLBACK ECBDeviceRead1 (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto PVOID pvCoreInit;

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( TRUE );
        }

                                        // Add process to PEL.

    if ( !AddCurrentThreadToPEL1() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Schedule an asynchronous event for
                                        // scanning the boot sector.

    NAVAPVxDScheduleAsyncECB ( uRefData,
                               lpEventData,
                               ECBDeviceRead1Async );

                                        // Enable ourselves again.

    DeleteCurrentThreadFromPEL1();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( TRUE );
    }

#elif defined(SYM_NTK)

#define DRIVER_LOCKED_CODE_SEG
#include "drvseg.h"

BOOL SECALLBACK ECBDeviceRead1 (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto PVOID   pvCoreInit;
    auto EDEXTRA rEDExtra;

                                        // Don't go any further if we were
                                        // not called at APC_LEVEL or lower.

    if ( KeGetCurrentIrql() > APC_LEVEL )
        {
        return ( TRUE );
        }

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( TRUE );
        }

                                        // Add process to PEL.

    if ( !AddCurrentThreadToPEL1() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Continue processing on our own
                                        // stack.

    CallOnNAVAPStack ( UserStackGenericECB1,
                       &rEDExtra,
                       uRefData,
                       lpEventData );

                                        // Enable ourselves again.

    DeleteCurrentThreadFromPEL1();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( TRUE );
    }

#define DRIVER_DEFAULT_CODE_SEG
#include "drvseg.h"

#endif // #if defined(SYM_VXD) ... #elif defined(SYM_NTK)

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBDeviceRead1Async (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when the event scheduled by ECBDeviceRead1() is
// signaled.
//
// Entry:
//      Same as in ECBDeviceRead1().
//
// Exit:
//      always TRUE
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL SECALLBACK ECBDeviceRead1Async (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto PVOID   pvCoreInit;
    auto BOOL    bAlreadyInPEL1;
    auto EDEXTRA rEDExtra;

                                        // Check if we are enabled and if
                                        // we can enter this function.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( TRUE );
        }

                                        // Force this thread into PEL1, since
                                        // since we are called asynchronously
                                        // and we DO want to process the
                                        // event, no matter whos context we
                                        // are working in.

    if ( ! ( bAlreadyInPEL1 = IsCurrentThreadInPEL1() ) )
        {
        if ( !ForceCurrentThreadIntoPEL1() )
            {
            CoreLocalDeInit ( pvCoreInit );
            return ( TRUE );
            }
        }

                                        // Continue processing on our own
                                        // stack.

    CallOnNAVAPStack ( UserStackGenericECB1,
                       &rEDExtra,
                       uRefData,
                       lpEventData );

                                        // Enable ourselves again.

    if ( !bAlreadyInPEL1 )
        {
        DeleteCurrentThreadFromPEL1();
        }

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( TRUE );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBReboot (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when SYMEvent intercepts system reboot.
//
// Entry:
//      Refer to SYMEvent documentation
//
// Exit:
//      always TRUE
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL SECALLBACK ECBReboot (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto PVOID   pvCoreInit;
    auto EDEXTRA rEDExtra;

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( TRUE );
        }

                                        // Add process to PEL.  Special
                                        // handling is required because
                                        // of the fact that ECBReboot()
                                        // is simulated when uninitializing.

    if ( !AddCurrentProcessToPEL2() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Continue processing on our own
                                        // stack.

    CallOnNAVAPStack ( UserStackGenericECB1,
                       &rEDExtra,
                       uRefData,
                       lpEventData );

                                        // Enable ourselves again.

    DeleteCurrentProcessFromPEL2();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( TRUE );
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// BOOL SECALLBACK ECBDeviceMount (
//      REFDATA uRefData,
//      LPED    lpEventData
//      );
//
// This function is called when SYMEvent intercepts device mount.
//
// Entry:
//      Refer to SYMEvent documentation
//
// Exit:
//      always TRUE
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

#define DRIVER_LOCKED_CODE_SEG
#include "drvseg.h"

BOOL SECALLBACK ECBDeviceMount (
    REFDATA uRefData,
    LPED    lpEventData
    )
    {
    auto PVOID   pvCoreInit;
    auto EDEXTRA rEDExtra;

                                        // Don't go any further if we were
                                        // not called at APC_LEVEL or lower.

    if ( KeGetCurrentIrql() > APC_LEVEL )
        {
        return ( TRUE );
        }

                                        // Check if we are enabled and if
                                        // we can enter this hook.

    if ( !bEventHooksAreEnabled ||
         !bNAVAPIsEnabled )
        {
        return ( TRUE );
        }

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();
        return ( TRUE );
        }

                                        // Add process to PEL.

    if ( !AddCurrentThreadToPEL1() )
        {
        CoreLocalDeInit ( pvCoreInit );
        return ( TRUE );
        }

                                        // Continue processing on our own
                                        // stack.

    CallOnNAVAPStack ( UserStackGenericECB1,
                       &rEDExtra,
                       uRefData,
                       lpEventData );

                                        // Enable ourselves again.

    DeleteCurrentThreadFromPEL1();

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );

    return ( TRUE );
    }

#define DRIVER_DEFAULT_CODE_SEG
#include "drvseg.h"

#endif // #if defined(SYM_NTK)

//---------------------------------------------------------------------------
//
// VOID UserStackGenericECB1 (
//      PEDEXTRA prEDExtra,
//      REFDATA  uRefData,
//      LPED     lpEventData
//      );
//
// This is a generic function that is called by one of the event callback
// routines after the stack has been swapped.
//
// Entry:
//      prEDExtra   - needed for processing the event.
//      uRefData    - needed for processing the event.
//      lpEventData - needed for processing the event.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID UserStackGenericECB1 (
    PEDEXTRA prEDExtra,
    REFDATA  uRefData,
    LPED     lpEventData
    )
    {
                                        // Process event.

    if ( InitEDExtra ( prEDExtra, uRefData, lpEventData ) )
        {
        ProcessEDExtra ( prEDExtra );

        CleanUpEDExtra ( prEDExtra );
        }
    }

//---------------------------------------------------------------------------
//
// VOID UserStackGenericECB2 (
//      PBOOL    pbResult,
//      PEDEXTRA prEDExtra,
//      REFDATA  uRefData,
//      LPED     lpEventData
//      );
//
// This is a generic function that is called by one of the event callback
// routines after the stack has been swapped.
//
// Entry:
//      pbResult    - needed for processing the event.
//      prEDExtra   - needed for processing the event.
//      uRefData    - needed for processing the event.
//      lpEventData - needed for processing the event.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID UserStackGenericECB2 (
    PBOOL    pbResult,
    PEDEXTRA prEDExtra,
    REFDATA  uRefData,
    LPED     lpEventData
    )
    {
                                        // Process event.

    if ( InitEDExtra ( prEDExtra, uRefData, lpEventData ) )
        {
        *pbResult = ProcessEDExtra ( prEDExtra );

        CleanUpEDExtra ( prEDExtra );
        }
    else
        {
        *pbResult = GetDefaultEventResult ( lpEventData, NULL );
        }
    }

//---------------------------------------------------------------------------
//
// VOID UserStackGenericECB3 (
//      PBOOL    pbResult,
//      PEDEXTRA prEDExtra,
//      REFDATA  uRefData,
//      LPED     lpEventData
//      );
//
// This is a generic function that is called by one of the event callback
// routines after the stack has been swapped.
//
// Entry:
//      pbResult    - needed for processing the event.
//      prEDExtra   - needed for processing the event.
//      uRefData    - needed for processing the event.
//      lpEventData - needed for processing the event.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID UserStackGenericECB3 (
    PBOOL    pbResult,
    PEDEXTRA prEDExtra,
    REFDATA  uRefData,
    LPED     lpEventData
    )
    {
                                        // Process event.

    if ( InitEDExtra ( prEDExtra, uRefData, lpEventData ) )
        {
        if ( IsFileInCleanCache ( lpEventData ) )
            {
                                        // If file was in "clean" cache, then
                                        // don't process it.  Note, that in
                                        // this case the file will not be
                                        // added to the open-after-exec
                                        // list either.

        *pbResult = TRUE;
            }
        else
            {
            if ( *pbResult = ProcessEDExtra ( prEDExtra ) )
                {
                                        // Build table of executed files.

                CreateOpenAfterExecEntry ( prEDExtra );
                }
            }

        CleanUpEDExtra ( prEDExtra );
        }
    else
        {
        *pbResult = GetDefaultEventResult ( lpEventData, NULL );
        }
    }

//---------------------------------------------------------------------------
//
// BOOL InstallEventHooks (
//      VOID
//      );
//
// This function installs NAVAP event hooks based on the contents of
// rEventListTable[].  If rEventListTable[] contains a member specifying
// an "undefined" SE_DEV_WRITE hook (lParam == NULL),
// InstallDeviceEventHooks() will be called to install such hooks for all
// devices in the device list.
//
// When this function is called, bEventHooksAreEnabled should be set to
// FALSE, thus protecting NAVAP from processing events which require
// allocation of resources which are released upon other event
// notifications.  Upon successful completion, bEventHooksAreEnabled will
// be set to TRUE by this function.
//
// Entry:
//      -
//
// Exit:
//      TRUE if all hooks were successfully installed, FALSE if there was
//      an error.  In case of an error, this function will try to undo
//      everything that it did until the error occurred.
//
//---------------------------------------------------------------------------
BOOL InstallEventHooks (
    VOID
    )
    {
    auto int    iCount, iInvalidHandles;
    auto HEVENT hEvent;

                                        // Processing of any event
                                        // notifications should be disabled
                                        // until all hooks are installed.

    SYM_ASSERT ( !bEventHooksAreEnabled );

                                        // Initialize scheduled event hooks.

#if defined(SYM_VXD)

    NAVAPVxDInstallAsyncECB();

#endif

                                        // Loop through rEventListTable[].

    for ( iCount = iInvalidHandles = 0;
          rEventListTable[iCount].EventHandler;
          iCount++ )
        {
                                        // Protect already existing hooks.

        if ( rEventListTable[iCount].hEvent == INVALID_EVENT_HANDLE )
            {
                                        // Filter device virus-like activity
                                        // hooks on NEC hardware under Win95.

#if defined(SYM_VXD)

            if ( bHWIsNEC &&
                 ( rEventListTable[iCount].EventType == SE_DEV_WRITE ||
                   rEventListTable[iCount].EventType == SE_DEV_FORMAT )
               )
                {
                continue;
                }

#endif

            if ( rEventListTable[iCount].EventType == SE_DEV_WRITE &&
                 rEventListTable[iCount].lParam == NULL
               )
                {
                                        // Install hooks for physical
                                        // writing to the boot sectors of
                                        // devices.

                hEvent = InstallDeviceEventHooks (
                            rEventListTable[iCount].EventType,
                            rEventListTable[iCount].EventFlags,
                            rEventListTable[iCount].EventHandler,
                            rEventListTable[iCount].RefData ) ?
                                ~INVALID_EVENT_HANDLE :
                                INVALID_EVENT_HANDLE;
                }
            else
                {
                                        // Install all other hooks.

                hEvent = EventObjectCreate (
                            rEventListTable[iCount].EventType,
                            rEventListTable[iCount].EventFlags,
                            rEventListTable[iCount].EventHandler,
                            rEventListTable[iCount].RefData,
                            rEventListTable[iCount].lParam );
                }

            rEventListTable[iCount].hEvent = hEvent;

            if ( hEvent == INVALID_EVENT_HANDLE )
                {
#if defined(SYM_NTK) // make SE_DEV_READ & SE_DEV_MOUNT events optional so NAVAP installs with an older SymEvent.SYS
                if ( rEventListTable[iCount].EventType != SE_DEV_READ &&
                     rEventListTable[iCount].EventType != SE_DEV_MOUNT)
#endif // #if defined(SYM_NTK)
                    iInvalidHandles++;
                }
            }
        else
            {
                                        // UnInstallEventHooks() must have
                                        // failed for whatever reason.  At
                                        // this point, we should not allow
                                        // reinstalling of all other hooks.
                                        // Instead, we should fail the
                                        // function and try to uninstall the
                                        // hooks again.

            iInvalidHandles++;
            }
        }

                                        // If we failed to install one or
                                        // more hooks, then we should undo
                                        // everything that we just did and
                                        // fail the function.

    if ( iInvalidHandles )
        {
        if ( !UnInstallEventHooks() )
            {
            SYM_ASSERT ( FALSE );
            }

        return ( FALSE );
        }

                                        // We have successfully installed
                                        // all hooks.

    return ( bEventHooksAreEnabled = TRUE );
    }

//---------------------------------------------------------------------------
//
// BOOL UnInstallEventHooks (
//      VOID
//      );
//
// This function removes all active NAVAP event hooks.  The "active" hooks
// are marked by rEventListTable[] and by the device list.
//
// UnInstallEventHooks() will set bEventHooksAreEnabled to FALSE before
// proceeding with removal of all event hooks.  Because of the this, and
// because event notifications may be currently executing in NAVAP, the
// caller must ensure that no event data, the deallocation of which is
// dependent on other hooks, will be allocated during the execution of
// UnInstallEventHooks().  All such data should be deallocated before
// calling this function.
//
// UnInstallEventHooks() will wait for all event notifications currently
// executing in NAVAP to complete before returning to the caller.
//
// Entry:
//      -
//
// Exit:
//      TRUE if all hooks were successfully removed, FALSE if there was an
//      error during processing.
//
//---------------------------------------------------------------------------
BOOL UnInstallEventHooks (
    VOID
    )
    {
    auto int iCount, iEventErrors;

                                        // Noone should enter the event
                                        // hooks from now on.

    bEventHooksAreEnabled = FALSE;

                                        // Loop through rEventListTable[].

    for ( iCount = iEventErrors = 0;
          rEventListTable[iCount].EventHandler;
          iCount++ )
        {
                                        // Uninstall only hooks that are
                                        // active.

        if ( rEventListTable[iCount].hEvent != INVALID_EVENT_HANDLE )
            {
            if ( rEventListTable[iCount].EventType == SE_DEV_WRITE &&
                 !rEventListTable[iCount].lParam
               )
                {
                                        // Uninstall hooks for physical
                                        // writing to the boot sectors of
                                        // devices.

                if ( UninstallDeviceEventHooks (
                         rEventListTable[iCount].EventType,
                         rEventListTable[iCount].EventFlags,
                         rEventListTable[iCount].EventHandler,
                         rEventListTable[iCount].RefData )
                   )
                    {
                    rEventListTable[iCount].hEvent = INVALID_EVENT_HANDLE;
                    }
                else
                    {
                    iEventErrors++;
                    }
                }
            else
                {
                                        // Uninstall all other hooks.

                if ( EventObjectDestroy ( rEventListTable[iCount].hEvent ) )
                    {
                    rEventListTable[iCount].hEvent = INVALID_EVENT_HANDLE;
                    }
                else
                    {
                    iEventErrors++;
                    }
                }
            }
        }

                                        // Make sure that we are not
                                        // processing any scheduled events.

#if defined(SYM_VXD)

    NAVAPVxDUnInstallAsyncECB();

#endif

                                        // Return status to caller.

    return ( iEventErrors ? FALSE : TRUE );
    }

#if defined(SYM_VXD)

#define DRIVER_VXD_LOCKED_CODE_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// VOID NAVAPVxDInstallAsyncECB (
//      VOID
//      );
//
// This function initializes the asynchronous event processing queue.
//
// Entry:
//      -
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID NAVAPVxDInstallAsyncECB (
    VOID
    )
    {
                                        // Not too much done here.  These
                                        // are the only two variables that
                                        // need to be initialized.

    prAsyncEDList = NULL;
    prAsyncEDCancelList = NULL;
    }

//---------------------------------------------------------------------------
//
// VOID NAVAPVxDUnInstallAsyncECB (
//      VOID
//      );
//
// This function uninitializes the asynchronous event processing queue.  It
// will cancel all outstanding events, and will wait for the completion of
// all outstanding events which are being processed at the moment.
//
// Entry:
//      -
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID NAVAPVxDUnInstallAsyncECB (
    VOID
    )
    {
    auto PASYNCEVENTDATA prFree;

                                        // Cancel all events that have not
                                        // been notified yet, and wait for
                                        // the ones that are being completed
                                        // by someone else.

    while ( prAsyncEDList )
        {
        if ( CancelAsyncEventNotification ( prAsyncEDList ) )
            {
            prFree = prAsyncEDList;

            prAsyncEDList = prAsyncEDList->prNext;

            MemFreePtr ( prFree );
            }
        else
            {
            Sleep ( 1000 );
            }
        }

                                        // Release any outstanding cancel
                                        // entries.

    while ( prAsyncEDCancelList )
        {
        prFree = prAsyncEDCancelList;

        prAsyncEDCancelList = prAsyncEDCancelList->prNext;

        MemFreePtr ( prFree );
        }
    }

//---------------------------------------------------------------------------
//
// VOID NAVAPVxDScheduleAsyncECB (
//      REFDATA uRefData,
//      LPED    lpEventData,
//      PVOID   pfnCallback
//      );
//
// This function schedules asynchronous processing of an event.
//
// Entry:
//      uRefData    - Same as in SYMEvent callbacks;
//      lpEventData - Same as in SYMEvent callbacks;
//      pfnCallback - Pointer to a SYMEvent callback to process the event.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID NAVAPVxDScheduleAsyncECB (
    REFDATA uRefData,
    LPED    lpEventData,
    PVOID   pfnCallback
    )
    {
    auto PASYNCEVENTDATA prNew, prCurrent, prPrev;

                                        // Validate input parameters.

    SYM_ASSERT ( lpEventData );
    SYM_ASSERT ( pfnCallback );

                                        // Allocate memory for asynchronous
                                        // event entry.  Reuse a trashed
                                        // entry if one is available.

    if ( prAsyncEDCancelList )
        {
        prNew = prAsyncEDCancelList;

        prAsyncEDCancelList = prAsyncEDCancelList->prNext;
        }
    else
        {
        prNew = (PASYNCEVENTDATA)
            MemAllocPtr ( GMEM_FIXED,
                          sizeof ( *prNew ) +
                          ( STRLEN ( lpEventData->SE_ED_lpPathSpec ) + 1 ) * sizeof(TCHAR) );
        }

    if ( prNew )
        {
                                        // Since currently this function is
                                        // used only by ECBDeviceRead1(),
                                        // the next two sections will perform
                                        // some SE_DEV_READ specific tasks in
                                        // addition to tasks required if this
                                        // function was used by more event
                                        // handlers.  In case it starts being
                                        // used more often, we need to review
                                        // this code.

        SYM_ASSERT ( lpEventData->SE_ED_etEventType == SE_DEV_READ );

                                        // Initialize the event parameters
                                        // of the new asynchronous event
                                        // entry.

        prNew->prNext = NULL;

        prNew->dwCancelTimeout = 500;

        prNew->dwEventHandle = NULL;

        prNew->pfnCancel = NAVAPVxDCancelAsyncECB;

        prNew->pfnCallback = pfnCallback;

        prNew->uRefData = uRefData;

        prNew->rED = *lpEventData;

        prNew->rED.SE_ED_lpPathSpec = (PTSTR)prNew->abyExtraData;
        prNew->rED.SE_ED_lpAltPathSpec = NULL;

        STRCPY ( prNew->rED.SE_ED_lpPathSpec, lpEventData->SE_ED_lpPathSpec );

                                        // Check if we have an entry for
                                        // scanning the drive in question
                                        // already.

        for ( prPrev = NULL, prCurrent = prAsyncEDList;
              prCurrent;
              prPrev = prCurrent, prCurrent = prCurrent->prNext )
            {
                                        // This seems to be the only way to
                                        // force intrinsic strcmp()/wcscmp().
                                        // Note, that this does not need any
                                        // DBCS support.

            auto int i;

            for ( i = 0;
                  lpEventData->SE_ED_lpPathSpec[i] &&
                  lpEventData->SE_ED_lpPathSpec[i] == prCurrent->rED.SE_ED_lpPathSpec[i];
                  i++ );

            if ( !lpEventData->SE_ED_lpPathSpec[i] &&
                 !prCurrent->rED.SE_ED_lpPathSpec[i] )
                {
                break;
                }
            }

                                        // If we don't have an entry for the
                                        // drive in question, then schedule
                                        // an event notification.

        if ( !prCurrent )
            {
            if ( ScheduleAsyncEventNotification ( prNew ) )
                {
                prNew->prNext = prAsyncEDList;

                prAsyncEDList = prNew;

                prNew = NULL;
                }
            else
                {
                UIOutOfMemoryError();
                }
            }

        if ( prNew )
            {
            MemFreePtr ( prNew );
            }
        }
    else
        {
        UIOutOfMemoryError();
        }
                                        // Release any outstanding cancel
                                        // entries.  This is just cleanup
                                        // code to help release trashed
                                        // memory.

    while ( prAsyncEDCancelList )
        {
        prNew = prAsyncEDCancelList;

        prAsyncEDCancelList = prAsyncEDCancelList->prNext;

        MemFreePtr ( prNew );
        }
    }

//---------------------------------------------------------------------------
//
// VOID __cdecl NAVAPVxDCancelAsyncECB (
//      PASYNCEVENTDATA prCancel
//      );
//
// This function cancels an already scheduled asynchronous event entry.  If
// the event could be cancelled, it will be moved to the list of trashed
// entries.  If someone else is processing it at the moment, no change will
// be done.  This function should be used only as the pfnCancel member of
// the ASYNCEVENTDATA structure.
//
// Entry:
//      prCancel - pointer to an entry in the linked list of asynchronous
//                 events.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID __cdecl NAVAPVxDCancelAsyncECB (
    PASYNCEVENTDATA prCancel
    )
    {
    auto PASYNCEVENTDATA prCurrent, prLast;

                                        // Loop through the list of scheduled
                                        // events.

    for ( prLast = NULL, prCurrent = prAsyncEDList;
          prCurrent;
          prLast = prCurrent, prCurrent = prCurrent->prNext )
        {
                                        // If we have a match with prCancel,
                                        // then move it to the trash and
                                        // quit.

        if ( prCurrent == prCancel )
            {
            if ( prLast )
                {
                prLast->prNext = prCurrent->prNext;
                }
            else
                {
                prAsyncEDList = prCurrent->prNext;
                }

            prCancel->prNext = prAsyncEDCancelList;

            prAsyncEDCancelList = prCancel;

            break;
            }
        }
    }

#define DRIVER_DEFAULT_CODE_SEGMENT
#include "drvseg.h"

#endif // #if defined(SYM_VXD)
