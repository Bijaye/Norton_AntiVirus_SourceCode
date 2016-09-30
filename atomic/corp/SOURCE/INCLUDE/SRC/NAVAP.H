// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/NAVAP.H_v   1.16   09 Dec 1998 16:39:44   RStanev  $
//
// Description:
//      This file declares shared NAVAP data.
//
// Contains:
//      ...
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/NAVAP.H_v  $
// 
//    Rev 1.16   09 Dec 1998 16:39:44   RStanev
// Updated comments to match new usage of MSG_ constants.
//
//    Rev 1.15   29 Sep 1998 18:46:10   RStanev
// Added fast clean cache APIs.
//
//    Rev 1.14   08 Jun 1998 20:06:24   RStanev
// Added hBackupFile.
//
//    Rev 1.13   02 Jun 1998 01:14:34   RStanev
// Merged Roswell fixes.
//
//    Rev 1.12   19 Feb 1998 15:42:36   RStanev
// Removed unknown virus detection.
//
//    Rev 1.11   17 Feb 1998 18:59:52   RStanev
// Updated copyright.  Updated stack swapping.
//
//    Rev 1.10   13 Sep 1997 13:35:40   RStanev
// Added messages for dealing with stack swapping level.
//
//    Rev 1.9   05 Jul 1997 19:59:44   RStanev
// Prototyped new functions for stack swapping.
//
//    Rev 1.8   17 Jun 1997 20:55:52   RStanev
// Exported ECBReboot().
//
//    Rev 1.7   01 Jun 1997 17:13:16   RStanev
// Added bUserIntervention to EDEXTRA.
//
//    Rev 1.6   28 May 1997 12:50:02   RStanev
// Added MSG_Unable_To_Locate_Scan_Directory.
//
//    Rev 1.5   21 May 1997 04:37:18   RStanev
// Exported UISelfCheckFailed() for all platforms.  Updated UIINITERROR
// and UICONTROLLEDINITMESSAGE.  Added aUISELFCHECKERRORTranslator[].
//
//    Rev 1.4   16 May 1997 01:50:26   RStanev
// Changed prototype of FileMonitorExchangeInocInfoPtr().
//
//    Rev 1.3   15 May 1997 00:33:34   RStanev
// Too many changes to describe.
//
//    Rev 1.2   30 Apr 1997 10:36:08   RStanev
// Fixed compilation errors introduced by AVENGE changes.
//
//    Rev 1.1   16 Apr 1997 10:56:14   RStanev
// Exported bNAVAPGlobalIsInitialized and some configuration APIs.
//
//    Rev 1.0   09 Apr 1997 12:29:48   RStanev
// Initial revision.
//
//***************************************************************************

#ifndef __cplusplus
#error Need a C++ compiler!
#endif  // __cplusplus

#ifndef _NAVAP_H_
#define _NAVAP_H_

//---------------------------------------------------------------------------
//
// The following files are required for the inclusion of NAVAP.H
//
//---------------------------------------------------------------------------
#include "symevent.h"
#include "avapi.h"
#include "exclude.h"
#include "actions.h"
#include "apcfg.h"

//***************************************************************************
//
// Prototype functions exported by the APCOMM module.
//
//***************************************************************************
VOID NAVAPServiceCallback (
    DWORD  dwServiceCode,
    PVOID  pvInBuffer,
    DWORD  dwInBufferSize,
    PVOID  pvOutBuffer,
    DWORD  dwOutBufferSize,
    PDWORD pdwBytesReturned
    );

VOID NAVAPCancelCallback (
    DWORD dwServiceCode,
    PVOID pvInBuffer,
    DWORD dwInBufferSize
    );

DWORD EnterControlledInit (
    VOID
    );

DWORD EnterControlledDeInit (
    VOID
    );

#if defined(SYM_VXD)

extern "C" {

BOOL WINAPI Int2FChainCallback (
    PCRS prCRS
    );

}

#endif // #if defined(SYM_VXD)

//***************************************************************************
//
// Define structures and prototype functions exported by the Event Manager
// module.
//
//***************************************************************************

//---------------------------------------------------------------------------
//
// Define event packet data and prototype management functions.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
// EDEXTRA is built by the event manager upon an event notification.  It is
// used to carry event data and status of event processing throughout all
// NAVAP modules.
//
// Rules for maintaining:
//
// Static members should always be initialized, before any of the dynamic
// members get accessed.  Dynamic members should be accessed only through
// macros.  This will ensure that they are properly initialized.
//
// prNAVAPCfg points to a locked configuration structure which should be
// used during the processing of this event.
//
// hVGlobalContext contains a locked handle to an AVAPI global context.
// This member is initialized only during events which need to use AVAPI.
//
// prSEEventData always points to the event structure which SYMEvent used
// to notify NAVAP.
//
// uEventType and bEventBack are always set to represent the event which
// this structure is built upon.  These two members are set during
// initialization of the structure.
//
// pszFile and pszAltFile point to the same strings which SYMEvent passed
// to the event callback.  uFileBufferLen and uOEMFileBufferLen are set
// to the lengths of the strings pointed by SYMEvent (in characters),
// including the terminating 0s.  SYM_VXD: uFileCharSet contains a character
// set ID for pszFile and pszAltFile (both strings always use the same
// character set.) uFileAttributes contains the new file attributes (only if
// applicable, otherwise undefined).
//
// uPhysicalDevice and chDeviceID represent the device for which an SE_DEV
// event was generated.  bPartitionable is TRUE if device is HD, FALSE if FD.
// bMBRAccess will be TRUE if it is physical access to sector 1 of device,
// otherwise uSectorLocation will contain the physical location of the
// sector that is being accessed.
//
// If any string from the dynamic section is needed, its pointer is set to
// point to an (initialiy empty - "\0") string, and the buffer length is set
// to the length of the buffer containing the empty string.  pvMemoryBuffer
// is used to store a pointer to a large memory block, containing all of
// the buffers for extra strings in the structure.
//
// To access most of the dynamic members of this structure, use the macros
// defined after the structure.  They will ensure that uninitialized data
// gets initialized.  Upon a first access to a string, the macros will call
// appropriate function which will replace the empty string with a valid
// string.
//
// The miscellaneous members of this structure are event-specific.
//
//---------------------------------------------------------------------------
typedef struct
    {
                                        // Static members

    PNAVAPCFG   prNAVAPCfg;             // Configuration structures
    HVCONTEXT   hVGlobalContext;        // AVAPI global context handle

    LPED        prSEEventData;          // SYMEvent event structure

    UINT        uEventType;             // Event information
    BOOL        bEventBack;             // Event start/completion flag

#if defined(SYM_VXD)
    UINT        uFileCharSet;           // Char set for the current thread
                                        // (applies to pszFile and pszAltFile)
#endif // #if defined(SYM_VXD)

    PTSTR       pszFile;                // Name of file
    UINT        uFileBufferLen;         // Length of name of file + '\0' (in characters)
    PTSTR       pszAltFile;             // Name of alt file
    UINT        uAltFileBufferLen;      // Length of name of alt file + '\0' (in characters)

    UINT        uFileAttributes;        // New attributes for pszFile.

    UINT        uPhysicalDevice;        // Physical device ID
    BOOL        bPartitionable;         // TRUE if HD, FALSE if FD
    UINT        uSectorLocation;        // Physical location of sector
    BOOL        bMBRAccess;             // TRUE if event is accessing MBR
    TCHAR       chDeviceID;             // Text representation of device ID

    void        *pvMemoryBuffer;        // Pointer to memory with buffers

                                        // Dynamic members

    BOOL        bFileIsExecutable;      // TRUE if pszFile is an executable
    BOOL        bFileIsExecutableSet;   // TRUE if bFileIsExecutable is set

    BOOL        bAltFileIsExecutable;   // TRUE if pszAltFile is an executable
    BOOL        bAltFileIsExecutableSet; // TRUE if bAltFileIsExecutable is set

    PTSTR       pszDevice;              // Pointer to SE string with device ID
    UINT        uDeviceBufferLen;       // Length of buffer for pszDevice (in characters)

    PTSTR       pszOwner;               // Name of owner (EXE file)
    UINT        uOwnerBufferLen;        // Length of buffer for pszOwner (in characters)
    BOOL        bOwnerIsKnown;          // Owner of process is known

#if defined(SYM_VXD)
    PSTR        pszOEMFile;             // pszFile, converted to OEM
    UINT        uOEMFileBufferLen;      // Length of buffer for pszOEMFile (in bytes)

    PSTR        pszAltOEMFile;          // pszAltFile, converted to OEM
    UINT        uAltOEMFileBufferLen;   // Length of buffer for pszAltOEMFile (in bytes)

    PSTR        pszOEMDevice;           // pszDevice, converted to OEM
    UINT        uOEMDeviceBufferLen;    // Length of buffer for pszOEMDevice (in bytes)

    PSTR        pszOEMOwner;            // pszOwner, converted to OEM
    UINT        uOEMOwnerBufferLen;     // Length of buffer for pszOEMOwner (in bytes)
#endif // #if defined(SYM_VXD)

                                        // Miscellaneous

    BOOL        bDisplayInfo;           // TRUE if showing current operation

    BOOL        bUserIntervention;      // The last call to a UI function that the kernel made required user intervention

    PTSTR       pszBackupFile;          // Buffer to keep name of backup file, or name of renamed or moved file, in native character set
    UINT        uBackupFileBufferLen;   // Length of buffer for pszBackupFile (in bytes)
    BOOL        bBackupFileCreated;     // TRUE if backup file created
    DWORD       hBackupFile;            // Handle to backup file.  The size of this member must be at least sizeof(HFILE).

    BOOL        bScanOnClose;           // Are we scanning, due to a close?

    BOOL        bItemIsInfected;        // TaskScanBootRecord() sets this value to TRUE/FALSE if boot record is infected/clean
    } EDEXTRA, *PEDEXTRA;

//---------------------------------------------------------------------------
//
// Macros for accessing dynamic members of the EDEXTRA structure and
// auto initializing them when necessary.
//
// Each one of these macros will accept a pointer to EDEXTRA structure, and
// will return the requested value.  If the accessed member has not yet been
// initialized, a function is called to initialize it.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

#define EDXpszOEMFile(p)                ( !(p)->pszOEMFile ?                \
                                          NULL :                            \
                                          ( *(p)->pszOEMFile == '\0' ?      \
                                            EDXSetpszOEMFile(p) :           \
                                            (p)->pszOEMFile )               \
                                        )

#define EDXpszAltOEMFile(p)             ( !(p)->pszAltOEMFile ?             \
                                          NULL :                            \
                                          ( *(p)->pszAltOEMFile == '\0' ?   \
                                            EDXSetpszAltOEMFile(p) :        \
                                            (p)->pszAltOEMFile )            \
                                        )

#define EDXpszOEMOwner(p)               ( !(p)->pszOEMOwner ?               \
                                          NULL :                            \
                                          ( *(p)->pszOEMOwner == '\0' ?     \
                                            EDXSetpszOEMOwner(p) :          \
                                            (p)->pszOEMOwner )              \
                                        )

#define EDXpszOEMDevice(p)              ( !(p)->pszOEMDevice ?              \
                                          NULL :                            \
                                          ( *(p)->pszOEMDevice == '\0' ?    \
                                            EDXSetpszOEMDevice(p) :         \
                                            (p)->pszOEMDevice )             \
                                        )
#else

#define EDXpszOEMFile(p)                ( (p)->pszFile )

#define EDXpszAltOEMFile(p)             ( (p)->pszAltFile )

#define EDXpszOEMOwner(p)               ( EDXpszOwner(p) )

#define EDXpszOEMDevice(p)              ( (p)->pszDevice )

#endif

#define EDXbFileIsExecutable(p)         ( (p)->bFileIsExecutableSet ?       \
                                          (p)->bFileIsExecutable :          \
                                          EDXSetbFileIsExecutable(p)        \
                                        )

#define EDXbAltFileIsExecutable(p)      ( (p)->bAltFileIsExecutableSet ?    \
                                          (p)->bAltFileIsExecutable :       \
                                          EDXSetbAltFileIsExecutable(p)     \
                                        )

#define EDXpszOwner(p)                  ( !(p)->pszOwner ?                  \
                                          NULL :                            \
                                          ( *(p)->pszOwner == '\0' ?        \
                                            EDXSetpszOwner(p) :             \
                                            (p)->pszOwner )                 \
                                        )

BOOL EDXSetbFileIsExecutable (
    PEDEXTRA prEDExtra
    );

BOOL EDXSetbAltFileIsExecutable (
    PEDEXTRA prEDExtra
    );

PTSTR EDXSetpszOwner (
    PEDEXTRA prEDExtra
    );

#if defined(SYM_VXD)

PSTR EDXSetpszOEMFile (
    PEDEXTRA prEDExtra
    );

PSTR EDXSetpszAltOEMFile (
    PEDEXTRA prEDExtra
    );

PSTR EDXSetpszOEMDevice (
    PEDEXTRA prEDExtra
    );

PSTR EDXSetpszOEMOwner (
    PEDEXTRA prEDExtra
    );

inline void EDXInvalidatepszOEMDevice (
    PEDEXTRA prEDExtra
    )
    {
    if ( prEDExtra->pszOEMDevice )
        {
        *prEDExtra->pszOEMDevice = '\0';
        }
    }

#endif // #if defined(SYM_VXD)

BOOL SECALLBACK ECBReboot (
    REFDATA uRefData,
    LPED    lpEventData
    );

//---------------------------------------------------------------------------
//
// The following declaration of bRebootEventProcessed is somewhat of a patch.
// It should be exported as a function which sets it or clears it, but even
// this is not the correct way of synchronizing shutdown scan.  The real
// solution is to not have the patch at all, and change our driver/agent
// model.
//
//---------------------------------------------------------------------------
extern BOOL bRebootEventProcessed;

//---------------------------------------------------------------------------
//
// TASK enumerates the tasks that the event manager posts with an event
// packet when it notifies the kernel.
//
// EVENTPACKET is a structure which the event manager builds for each
// EDEXTRA structure, and passes it to the kernel for processing.
//
//---------------------------------------------------------------------------
typedef enum
    {
    TASK_SCAN_FILE,
    TASK_NOTIFY_FILEATTR_CHANGE,
    TASK_NOTIFY_FILE_WRITE,
    TASK_NOTIFY_HDFORMAT,
    TASK_NOTIFY_HDMBR_WRITE,
    TASK_NOTIFY_HDBOOT_WRITE,
    TASK_NOTIFY_FDBOOT_WRITE,
    TASK_SCAN_BOOT,
    TASK_SCAN_DEVICES
    } TASK, *PTASK;

#define TASK_QUEUE_SIZE     10

typedef struct
    {
    PEDEXTRA prEDExtra;                 // Event Information
    BOOL     bCallbackResult;           // Status of the current packet

    UINT     uCurrentTask;              // Task queue: current task
    UINT     uNextTask;                 // Task queue: where to add new task
    TASK     aeTaskQueue[TASK_QUEUE_SIZE];
    } EVENTPACKET, *PEVENTPACKET;

typedef PEVENTPACKET HEVENTPACKET, *PHEVENTPACKET;

BOOL EventPacketGetResult (
    HEVENTPACKET prEventPacket
    );

VOID EventPacketSetResult (
    HEVENTPACKET prEventPacket,
    BOOL         bResult
    );

BOOL EventPacketGetTask (
    HEVENTPACKET prEventPacket,
    PTASK        peTask
    );

//---------------------------------------------------------------------------
//
// SEDEVEVENTPARAMETER is a SymEvent structure used when creating device
// event hooks.
//
//---------------------------------------------------------------------------
#include <pshpack1.h>

typedef struct
    {
    DWORD dwStartingSector;
    DWORD dwNumberOfSectors;
    TCHAR szDeviceFilter[3];
    } SEDEVEVENTPARAMETER;

#include <poppack.h>

//---------------------------------------------------------------------------
//
// The following structure and the functions below are used to build and
// manage the device list.
//
// The device list is link of DEVICEINFOREC structures.  Here is a
// description of each member of DEVICEINFOREC:
//
//   prNextDeviceInfo    - Link to the next entry in the device list;
//   uPhysicalDevice     - Device number (int 13 standard);
//   bVisible            - Device / Partition visible to user;
//   bPartitionable      - Device supports partitioning;
//   uPartition          - Partition number (0 through 3);
//   uExtPartition       - Extended partition number in chain;
//   bPartitionBootable  - Partition is bootable;
//   uBootSectorLocation - Boot sector's physical location;
//   chPhysicalID        - Entry identifier for user interface;
//   chLogicalID         - Entry identifier for user interface;
//   hEvent              - Handle returned bu EventObjectCreate() when
//                         installing device event hooks;
//   rSElParam           - lParam for EventObjectCreate() when installing
//                         device event hooks;
//
//---------------------------------------------------------------------------
typedef struct tagDEVICEINFOREC
    {
    tagDEVICEINFOREC*   prNextDeviceInfo;
    UINT                uPhysicalDevice;
    BOOL                bVisible;
    BOOL                bPartitionable;
    UINT                uPartition;
    UINT                uExtPartition;
    BOOL                bPartitionBootable;
    UINT                uBootSectorLocation;
    TCHAR               chPhysicalID;
    TCHAR               chLogicalID;
    HEVENT              hEvent;
    SEDEVEVENTPARAMETER rSElParam;
    } DEVICEINFOREC, *PDEVICEINFOREC;

//---------------------------------------------------------------------------
//
// Prototype functions for building and managing the list of devices.
//
//---------------------------------------------------------------------------
BOOL CreateDeviceList (
    VOID
    );

BOOL DestroyDeviceList (
    VOID
    );

PDEVICEINFOREC GetNextRemovableDevice (
    PDEVICEINFOREC prDeviceInfo
    );

//---------------------------------------------------------------------------
//
// Prototype functions for managing NAVAP's event hooks.
//
//---------------------------------------------------------------------------
BOOL InstallEventHooks (
    VOID
    );

BOOL UnInstallEventHooks (
    VOID
    );

//---------------------------------------------------------------------------
//
// File monitor module.
//
//---------------------------------------------------------------------------
enum
    {
    e_FILEMONITOR_BABYSITTING,
    e_FILEMONITOR_CHECKONCLOSE,
    e_FILEMONITOR_MODIFIED,
    e_FILEMONITOR_CREATED,
    e_FILEMONITOR_SIZE0ATOPEN,
    e_FILEMONITOR_CONFIRMWRITE
    };

#define FILEMONITOR_BABYSITTING      ( 1 << e_FILEMONITOR_BABYSITTING )
#define FILEMONITOR_CHECKONCLOSE     ( 1 << e_FILEMONITOR_CHECKONCLOSE )
#define FILEMONITOR_MODIFIED         ( 1 << e_FILEMONITOR_MODIFIED )
#define FILEMONITOR_CREATED          ( 1 << e_FILEMONITOR_CREATED )
#define FILEMONITOR_SIZE0ATOPEN      ( 1 << e_FILEMONITOR_SIZE0ATOPEN )
#define FILEMONITOR_CONFIRMWRITE     ( 1 << e_FILEMONITOR_CONFIRMWRITE )

BOOL FileMonitorInit (
    VOID
    );

BOOL FileMonitorDeInit (
    VOID
    );

VOID FileMonitorEnable (
    VOID
    );

VOID FileMonitorDisable (
    VOID
    );

BOOL FileMonitorClearAttributes (
    LPED lpEventData,
    UINT uAttributes
    );

inline BOOL FileMonitorClearAttributes (
    HEVENTPACKET hEventPacket,
    UINT         uAttributes
    )
    {
    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );

    return ( FileMonitorClearAttributes ( hEventPacket->prEDExtra->prSEEventData,
                                          uAttributes ) );
    }

#if defined(SYM_NTK)

DWORD FileMonitorQuerySavedUserSID (
    LPED lpEventData,
    PSID pSID
    );

#endif // #if defined(SYM_NTK)

//---------------------------------------------------------------------------
//
// PEL module.
//
//---------------------------------------------------------------------------
BOOL PELInit (
    VOID
    );

BOOL PELDeInit (
    VOID
    );

BOOL IsCurrentThreadInPEL1 (
    VOID
    );

BOOL AddCurrentProcessToPEL2 (
    VOID
    );

int GetCurrentProcessPEL2Count (
    VOID
    );

VOID DeleteCurrentProcessFromPEL2 (
    VOID
    );

//---------------------------------------------------------------------------
//
// Filtering modules - clean file cache, file open/create synchronization,
// open-after-exec cache.
//
//---------------------------------------------------------------------------
BOOL InitCleanCache (
    VOID
    );

BOOL DeInitCleanCache (
    VOID
    );

BOOL AddFileToCleanCache (
    LPED lpEventData
    );

BOOL FlushCleanCache (
    VOID
    );

BOOL FileOpenSyncInit (
    VOID
    );

BOOL FileOpenSyncDeInit (
    VOID
    );

VOID FileOpenSyncCleanup (
    VOID
    );

#if defined(SYM_VXD)

VOID CleanupOpenAfterExec (
    BOOL bCleanupAll,
    PTCB hThread,
    HVM  hVM
    );

#endif // #if defined(SYM_VXD)

//***************************************************************************
//
// Prototype functions exported by the Kernel module.
//
//***************************************************************************
VOID KernelNotify (
    HEVENTPACKET hEventPacket
    );

VOID InitFastCleanCache ( VOID );
VOID DeInitFastCleanCache ( VOID );

//***************************************************************************
//
// Define structures and prototype functions exported by the UI module.
//
//***************************************************************************

//---------------------------------------------------------------------------
//
// Define event-related UI data and prototype functions.
//
//---------------------------------------------------------------------------
AVACTION UIFileInfected (
    HEVENTPACKET  hEventPacket,
    HVLVIRUS      hVLVirus,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    );

#if defined(SYM_VXD)

VOID UIInfoScanningBoot (
    HEVENTPACKET hEventPacket,
    BOOL         bShowOrHide
    );

AVACTION UIBootInfected (
    HEVENTPACKET  hEventPacket,
    HVLVIRUS      hVLVirus,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    );

AVACTION UIMemoryInfected (
    HEVENTPACKET  hEventPacket,
    HVLVIRUS      hVLVirus,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    );

#endif // #if defined(SYM_VXD)

AVACTION UISelfCheckFailed (
    HEVENTPACKET  hEventPacket,
    UINT          uReason,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    );

#if defined(SYM_VXD)

AVACTION UIVirusLikeActivity (
    HEVENTPACKET  hEventPacket,
    AVCONTEXT     uAVContext,
    AVACTION      uLastAVAction,
    BOOL          bLastActionCompleted,
    PAVACTIONDATA prAVActionData
    );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// Define initialization UI data and declare functions.
//
// Notes:
//      To the right of each member of UIINITERROR and
//      UICONTROLLEDINITMESSAGE is a comment with the platform that each
//      error/message is supported by.
//      Any changes to UIINITERROR and UICONTROLLEDINITMESSAGE should be
//      reflected in the appropriate string translation tables.
//
//---------------------------------------------------------------------------
typedef enum
    {
    MSG_SYMEvent_Not_Loaded,                    // NTK VXD
    MSG_SYMEvent_Incompatible_Version,          // NTK VXD
    MSG_Core_Init_Failed,                       // NTK VXD
    MSG_Cant_Save_Persistent_Data,              // NTK VXD
    MSG_Cant_Allocate_PEL_ThreadVM_Data,        // NTK VXD
    MSG_DriverCommInit_Error,                   // NTK VXD
    MSG_DriverComm_Register_Callbacks_Error,    // NTK VXD
    MSG_Cant_Allocate_Open_File_Data,           // NTK VXD
    MSG_Cant_Initialize_File_Caching,           // NTK VXD
    MSG_Unable_To_Build_NAVAP_SYS_Name,         // NTK
    MSG_Unable_To_Create_Device,                // NTK
    MSG_Unable_To_Create_Symbolic_Link,         // NTK
    MSG_Unable_To_Open_RegistryPath,            // NTK
    MSG_Invalid_Stack_Swapping_Level,           // NTK
    MSG_Unable_To_Close_RegistryPath,           // NTK
    MSG_SID_Length_Increased,                   // NTK
    MSG_NAVTSR_Error_Disabling,                 //     VXD
    MSG_No_Scanner_APIs,                        //     VXD
    MSG_CriticalErrorHandler_Error,             //     VXD
    MSG_BreakHandlerInit_Error                  //     VXD
    } UIINITERROR;

VOID UIInitError (
    UIINITERROR eError
    );

typedef enum
    {
    MSG_Unable_To_Synchronize_Configuration,    // NTK VXD
    MSG_Unable_To_Locate_Configuration,         // NTK VXD
    MSG_Unable_To_Locate_Scan_Directory,        // NTK VXD
    MSG_Unable_To_Determine_Startup_Name,       // NTK VXD
    MSG_Config_Bad_Path_Specified,              // NTK VXD
    MSG_Config_Unable_To_Read,                  // NTK VXD
    MSG_Config_Load_Defaults_Used,              // NTK VXD
    MSG_Config_NAVAPInitExclusionAPI_Failed,    // NTK VXD
    MSG_Config_VLScanInit_Failed,               // NTK VXD
    MSG_Config_VLRegisterVirusDB_Failed,        // NTK VXD
    MSG_NAVTSR_Not_Loaded,                      //     VXD
    MSG_Unable_To_Load_Agent,                   //     VXD
    MSG_Cant_Create_Device_List,                // NTK VXD
    MSG_Invalid_Event_Handles                   // NTK VXD
    } UICONTROLLEDINITMESSAGE;

VOID UIControlledInitMessage (
    UICONTROLLEDINITMESSAGE eMessage
    );

//---------------------------------------------------------------------------
//
// Prototype helper UI functions and define helper data.
//
//---------------------------------------------------------------------------
VOID UIOutOfMemoryError (
    VOID
    );

typedef enum
    {
    MSG_Unable_To_Initiate_Shutdown,
    MSG_Shutdown_Aborted
    } UISHUTDOWNERROR;

VOID UIShutdownError (
    UISHUTDOWNERROR eError
    );

#if defined(_APCOMM_H_)

BOOL UIGetUserInfo (
    LPED                     prEventData,
    PNAVAP_USERINFOSUBPACKET prUserInfo
    );

#endif // #if defined(_APCOMM_H_)

#if defined(SYM_NTK)

DWORD UIGetCurrentUserSID (
    PSID  pSID,
    DWORD cbSID
    );

#endif // #if defined(SYM_NTK)

//***************************************************************************
//
// Prototype functions and declare data exported by the main NAVAP module.
//
//***************************************************************************

//---------------------------------------------------------------------------
//
// Global NAVAP status variables.
//
//---------------------------------------------------------------------------
extern BOOL bNAVAPIsEnabled;

extern BOOL bNAVAPIsInShutdownMode;

//---------------------------------------------------------------------------
//
// Prototype exported exclusion functions.
//
//---------------------------------------------------------------------------
BOOL NAVAPIsFileExcluded (
    HEVENTPACKET hEventPacket,
    PTSTR        pszFileName,
    DWORD        dwFlags
    );

BOOL NAVAPExcludeFile (
    HEVENTPACKET hEventPacket,
    PTSTR        pszFileName,
    DWORD        dwFlags
    );

//---------------------------------------------------------------------------
//
// Prototype init/deinit functions.
//
//---------------------------------------------------------------------------
BOOL NAVAPControlledInit (
    VOID
    );

BOOL NAVAPControlledDeInit (
    VOID
    );

//---------------------------------------------------------------------------
//
// Prototype global NAVAP helper functions and data.
//
//---------------------------------------------------------------------------
VOID InitiateShutdown (
    VOID
    );

//---------------------------------------------------------------------------
//
// Prototype NAVAP NT helper functions and data.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

extern const NTSTATUS aUIINITERRORTranslator[];

extern const NTSTATUS aUICONTROLLEDINITMESSAGETranslator[];

typedef enum
    {
    UISELFCHECKERROR_NO_CRC,
    UISELFCHECKERROR_CRC_CHANGED,
    UISELFCHECKERROR_FILE_IO,
    UISELFCHECKERROR_INTERNAL
    };

extern const NTSTATUS aUISELFCHECKERRORTranslator[];

extern const NTSTATUS aOutOfMemoryErrorMessageTranslator[];

VOID NAVAPNTWriteStandardEventLog (
    NTSTATUS Status
    );

#endif // #if defined(SYM_NTK)

//---------------------------------------------------------------------------
//
// Prototype NAVAP VxD helper functions and data.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

extern "C" {

VOID WINAPI SleepDead (
    UINT uMilliseconds
    );

}

#endif // #if defined(SYM_VXD)

//***************************************************************************
//
// Miscellaneous NAVAP definitions.
//
//***************************************************************************

//---------------------------------------------------------------------------
//
// Define inline functions for getting the configuration associated with
// a particular event data structure or particular event packet.
//
//---------------------------------------------------------------------------
inline PNAVAPCFG prAPCfg (
    PEDEXTRA prEDExtra
    )
    {
    SYM_ASSERT ( prEDExtra );
    SYM_ASSERT ( prEDExtra->prNAVAPCfg );

    return ( prEDExtra->prNAVAPCfg );
    }

inline PNAVAPCFG prAPCfg (
    HEVENTPACKET hEventPacket
    )
    {
    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( hEventPacket->prEDExtra->prNAVAPCfg );

    return ( hEventPacket->prEDExtra->prNAVAPCfg );
    }

inline PNAVAPCFG prAPCfg (
    PHEVENTPACKET phEventPacket
    )
    {
    SYM_ASSERT ( phEventPacket );
    SYM_ASSERT ( *phEventPacket );
    SYM_ASSERT ( (*phEventPacket)->prEDExtra );
    SYM_ASSERT ( (*phEventPacket)->prEDExtra->prNAVAPCfg );

    return ( (*phEventPacket)->prEDExtra->prNAVAPCfg );
    }

//---------------------------------------------------------------------------
//
// Prototype CallOnNAVAPStack() and all different functions that NAVAP
// needs to call through it.
//
//---------------------------------------------------------------------------
typedef VOID ( *PFNSWAPPEDSTACK1 ) (
    VOID
    );

typedef VOID ( *PFNSWAPPEDSTACK2 ) (
    PBOOL pbParam1
    );

typedef VOID ( *PFNSWAPPEDSTACK3 ) (
    HEXCLUDEN hExcludeN
    );

typedef VOID ( *PFNSWAPPEDSTACK4 ) (
    PEDEXTRA prEDExtra,
    REFDATA  uRefData,
    LPED     lpEventData
    );

typedef VOID ( *PFNSWAPPEDSTACK5 ) (
    PBOOL    pbParam1,
    PEDEXTRA prEDExtra,
    REFDATA  uRefData,
    LPED     lpEventData
    );

BOOL CallOnNAVAPStack (
    PFNSWAPPEDSTACK1 pfnSwappedStackFunction
    );

BOOL CallOnNAVAPStack (
    PFNSWAPPEDSTACK2 pfnSwappedStackFunction,
    PBOOL            pbParam1
    );

BOOL CallOnNAVAPStack (
    PFNSWAPPEDSTACK3 pfnSwappedStackFunction,
    HEXCLUDEN        hExcludeN
    );

BOOL CallOnNAVAPStack (
    PFNSWAPPEDSTACK4 pfnSwappedStackFunction,
    PEDEXTRA         prEDExtra,
    REFDATA          uRefData,
    LPED             lpEventData
    );

BOOL CallOnNAVAPStack (
    PFNSWAPPEDSTACK5 pfnSwappedStackFunction,
    PBOOL            pbParam1,
    PEDEXTRA         prEDExtra,
    REFDATA          uRefData,
    LPED             lpEventData
    );

//---------------------------------------------------------------------------
//
// PERSISTENTDATA is a structure into which NAVAP stores data which cannot
// be deallocated when NAVAP unloads, and then reallocated again when NAVAP
// loads.  The memory block for this structure is allocted and managed by
// SymEvent.  Whenever this structure, or the information held by it changes,
// a new SEPDID should be assigned to identify it.
//
// prNAVAPPersistentData is a variable containing a pointer to the current
// NAVAP persistent data.  This variable is initialized when NAVAP loads,
// and is used by all modules which need to use the persistent data area.
// NAVAP will not run without prNAVAPPersistentData initialized.
//
//---------------------------------------------------------------------------
typedef struct
    {
    HOFDATA hPELThreadData;
    HOFDATA hOFData;
    DWORD   dwOFDataSIDLength;
    } PERSISTENTDATA;

extern PERSISTENTDATA *prNAVAPPersistentData;

#endif  // _NAVAP_H_
