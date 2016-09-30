// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAP/VCS/NAVAPNT.CPv   1.9   09 Dec 1998 17:02:48   RStanev  $
//
// Description:
//      This file contains the entry/exit points of NAVAP's WinNT driver,
//      as well as NAVAP functions specific to WinNT.
//
// Contains:
//      DriverEntry()
//      NAVAPAsyncCreateClose()         (local function)
//      NAVAPAsyncDIOCTL()              (local function)
//      NAVAPNTUnload()                 (local function)
//
//      NAVAPStartupTasks()
//      NAVAPExitTasks()
//
//      SaveNAVAPSYSStartupName()       (local function)
//
//      NAVAPNTWriteStandardEventLog()
//
// See Also:
//      navapvxd.cpp
//
// Note:
//      Supports SYM_NTK only!
//
//***************************************************************************
// $Log:   S:/NAVAP/VCS/NAVAPNT.CPv  $
//
//    Rev 1.9   09 Dec 1998 17:02:48   RStanev
// Added a check for SYMEvent version.
//
//    Rev 1.8   17 Feb 1998 22:20:30   RStanev
// Updated copyright.
//
//    Rev 1.7   13 Sep 1997 15:01:20   RStanev
// Added registry support for controlling the stack swapping level.
//
//    Rev 1.6   09 Aug 1997 01:00:40   RStanev
// Patch for support of old NAVAP APIs.
//
//    Rev 1.5   17 Jun 1997 20:44:48   RStanev
// Added NAVAPExitTasks().
//
//    Rev 1.4   28 May 1997 14:04:44   RStanev
// Added NTSTATUS_NAVAP_Unable_To_Locate_Scan_Directory to
// aUICONTROLLEDINITMESSAGETranslator[].
//
//    Rev 1.3   23 May 1997 23:37:08   RStanev
// Microsoft does not mention in the DDK that ZwQueryValueKey() will return
// STATUS_BUFFER_TOO_SMALL instead of STATUS_BUFFER_OVERFLOW when we don't
// supply a buffer.
//
//    Rev 1.2   21 May 1997 04:56:50   RStanev
// Added aUISELFCHECKERRORTranslator[].  Added pszNAVAPSYSStartupName and
// supporting functions.
//
//    Rev 1.1   15 May 1997 03:07:48   RStanev
// Too many changes to describe.
//
//    Rev 1.0   09 Apr 1997 12:21:44   RStanev
// Initial revision.
//
//***************************************************************************

#if !defined(SYM_NTK)
#error This file supports only SYM_NTK platform!
#endif

#include "platform.h"
#include "drvcomm.h"
#include "navap.h"
#include "navapi.h"
#include "navapnt.h"
#include "apstrs.h"

//---------------------------------------------------------------------------
//
// Local function prototypes.
//
//---------------------------------------------------------------------------
NTSTATUS NAVAPAsyncCreateClose (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS NAVAPAsyncDIOCTL (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID NAVAPNTUnload (
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS SaveNAVAPSYSStartupName (
    IN PUNICODE_STRING NAVAPKey
    );

//---------------------------------------------------------------------------
//
// NAVAPDriverObject is used by NAVAPNTWriteStandardEventLog() to identify
// our driver when writing to the event log.
//
//---------------------------------------------------------------------------
PDRIVER_OBJECT NAVAPDriverObject = NULL;

//---------------------------------------------------------------------------
//
// aUIINITERRORTranslator[] is used by the SYM_NTK implementation of
// UIInitError() to translate the platform independent MSG_ enumerators from
// UIINITERROR into NT-specific event log message IDs.  The translated IDs
// are passed to NAVAPNTWriteStandardEventLog().  Any changes to UIINITERROR
// should be immediately reflected into this table.
//
// aUICONTROLLEDINITMESSAGETranslator[] is used by the SYM_NTK implementation
// of UIControlledInitMessage() to translate the platform independent MSG_
// enumerators from UICONTROLLEDINITMESSAGE into NT-specific event log
// message IDs.  The translated IDs are passed to
// NAVAPNTWriteStandardEventLog().  Any changes to UICONTROLLEDINITMESSAGE
// should be immediately reflected into this table.
//
// aUISELFCHECKERRORTranslator[] is used by the SYM_NTK implementation of
// UISelfCheckFailed() to report the status of the integrity validation.
//
// aOutOfMemoryErrorMessageTranslator[] is only used to make
// NTSTATUS_NAVAP_Out_Of_Memory_Error accessible to other AP modules.  The
// only reason it is defined as an array is so that the translation is
// consistent with all other tables.
//
//---------------------------------------------------------------------------
const NTSTATUS aUIINITERRORTranslator[] =
    {
    NTSTATUS_NAVAP_SYMEvent_Not_Loaded,
    NTSTATUS_NAVAP_SYMEvent_Incompatible_Version,
    NTSTATUS_NAVAP_Core_Init_Failed,
    NTSTATUS_NAVAP_Cant_Save_Persistent_Data,
    NTSTATUS_NAVAP_Cant_Allocate_PEL_ThreadVM_Data,
    NTSTATUS_NAVAP_DriverCommInit_Error,
    NTSTATUS_NAVAP_DriverComm_Register_Callbacks_Error,
    NTSTATUS_NAVAP_Cant_Allocate_Open_File_Data,
    NTSTATUS_NAVAP_Cant_Initialize_File_Caching,
    NTSTATUS_NAVAP_Unable_To_Build_NAVAP_SYS_Name,
    NTSTATUS_NAVAP_Unable_To_Create_Device,
    NTSTATUS_NAVAP_Unable_To_Create_Symbolic_Link,
    NTSTATUS_NAVAP_Unable_To_Open_RegistryPath,
    NTSTATUS_NAVAP_Invalid_Stack_Swapping_Level,
    NTSTATUS_NAVAP_Unable_To_Close_RegistryPath,
    NTSTATUS_NAVAP_SID_Length_Increased,
    0,
    0,
    0,
    0
    };

const NTSTATUS aUICONTROLLEDINITMESSAGETranslator[] =
    {
    NTSTATUS_NAVAP_Unable_To_Synchronize_Configuration,
    NTSTATUS_NAVAP_Unable_To_Locate_Configuration,
    NTSTATUS_NAVAP_Unable_To_Locate_Scan_Directory,
    NTSTATUS_NAVAP_Unable_To_Determine_Startup_Name,
    NTSTATUS_NAVAP_Config_Bad_Path_Specified,
    NTSTATUS_NAVAP_Config_Unable_To_Read,
    NTSTATUS_NAVAP_Config_Load_Defaults_Used,
    NTSTATUS_NAVAP_Config_NAVAPInitExclusionAPI_Failed,
    NTSTATUS_NAVAP_Config_VLScanInit_Failed,
    NTSTATUS_NAVAP_Config_VLRegisterVirusDB_Failed,
    0,
    0,
    NTSTATUS_NAVAP_Cant_Create_Device_List,
    NTSTATUS_NAVAP_Invalid_Event_Handles
    };

const NTSTATUS aUISELFCHECKERRORTranslator[] =
    {
    NTSTATUS_NAVAP_SelfCheckError_No_CRC,
    NTSTATUS_NAVAP_SelfCheckError_CRC_Changed,
    NTSTATUS_NAVAP_SelfCheckError_File_IO,
    NTSTATUS_NAVAP_SelfCheckError_Internal
    };

const NTSTATUS aOutOfMemoryErrorMessageTranslator[] =
    {
    NTSTATUS_NAVAP_Out_Of_Memory_Error
    };

//---------------------------------------------------------------------------
//
// SYMEventFileObject keeps a lock on the SymEvent driver while NAVAP is
// using it.
//
//---------------------------------------------------------------------------
PFILE_OBJECT SYMEventFileObject = NULL;

//---------------------------------------------------------------------------
//
// pszNAVAPSYSStartupName contains a pointer to the name of the NAVAP.SYS
// image.  It is initialized by SaveNAVAPSYSStartupName().
//
//---------------------------------------------------------------------------
PWSTR pszNAVAPSYSStartupName = NULL;

//---------------------------------------------------------------------------
//
// NTSTATUS DriverEntry (
//      IN PDRIVER_OBJECT DriverObject,
//      IN PUNICODE_STRING RegistryPath
//      );
//
// This is standard NT Driver entry function.  It initializes the NAVAP
// device object, creates a symbolic link for device IO control, locks
// SymEvent in memory, and then initializes global NAVAP modules.
//
// Upon successful completion of this function, the NAVAP driver itself will
// not be functional.  The global initialization of NAVAP only prepares the
// driver for full initialization.  Full driver initialization and enabling
// NAVAP is completed upon request by a user-mode application.
//
// DriverEntry() will communicate to the user any errors and warnings
// generated during processing.
//
// Entry:
//      DriverObject - pointer to NAVAP's driver object
//      RegistryPath - path to the location in the registry where NAVAP
//                     was loaded from.  This looks like:
//                     \Registry\Machine\System\CurrentControlSet\Services\NAVAP
//
// Exit:
//      STATUS_SUCCESS if NAVAP initialized, or other, depending on
//      the error.
//
// Note:
//      The code for DriverEntry() will be discarded after initialization.
//
//---------------------------------------------------------------------------
#define DRIVER_INIT_CODE_SEGMENT
#include "drvseg.h"

NTSTATUS DriverEntry (
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
    {
    auto NTSTATUS          status;
    auto PDEVICE_OBJECT    deviceObject = NULL;
    auto PDEVICE_OBJECT    SEdeviceObject = NULL;
    auto UNICODE_STRING    uniNtNameString;
    auto UNICODE_STRING    uniWin32NameString;
    auto OBJECT_ATTRIBUTES KeyObjectAttributes;
    auto HANDLE            KeyHandle;
    auto UNICODE_STRING    uniKeyValueName;
    auto struct {
         KEY_VALUE_PARTIAL_INFORMATION info;
         DWORD                         filler;
         }                 KeyValue;
    auto ULONG             KeyValueLength;

                                        // Before doing anything big, set up
                                        // the NT error log function.

    NAVAPDriverObject = DriverObject;

                                        // Save the image name for NAVAP.SYS
                                        // in our own buffer.

    status = SaveNAVAPSYSStartupName ( RegistryPath );

    if ( NT_SUCCESS(status) )
        {
                                        // Initialize NAVAP device object.

        RtlInitUnicodeString ( &uniNtNameString, szNAVAPNTDeviceName );

        status = IoCreateDevice (
                     DriverObject,
                     0,
                     &uniNtNameString,
                     FILE_DEVICE_UNKNOWN,
                     0,
                     FALSE,
                     &deviceObject
                     );

        if ( NT_SUCCESS(status) )
            {
                                        // Initialize driver object.

            DriverObject->MajorFunction[IRP_MJ_CREATE]         = NAVAPAsyncCreateClose;
            DriverObject->MajorFunction[IRP_MJ_CLOSE]          = NAVAPAsyncCreateClose;
            DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = NAVAPAsyncDIOCTL;

            DriverObject->DriverUnload                         = NAVAPNTUnload;

                                        // Create symbolic link so that
                                        // applications and drivers can open
                                        // a handle to NAVAP and issue
                                        // DeviceIoControl() calls.

            RtlInitUnicodeString ( &uniWin32NameString,
                                   szNAVAPNTDeviceCommName );

            status = IoCreateSymbolicLink ( &uniWin32NameString,
                                            &uniNtNameString );

            if ( NT_SUCCESS(status) )
                {
                                        // Lock SymEvent.

                RtlInitUnicodeString ( &uniNtNameString, szSYMEVENTDeviceName );

                status = IoGetDeviceObjectPointer ( &uniNtNameString,
                                                    FILE_ALL_ACCESS,
                                                    &SYMEventFileObject,
                                                    &SEdeviceObject );

                if ( NT_SUCCESS ( status ) )
                    {
                                        // Validate the SYMEvent version.

                    if ( SYMEvent_Get_Version() >= SYMEVENT_VERSION_REQUIRED_BY_NAVAP )
                        {
                                        // Prior to calling CoreGlobalInit()
                                        // we need to set some internal Core
                                        // variables for dealing with
                                        // security and stack issues.

                        SYMNTK_KernelModeXfer_SetDevice ( szNAVAPNTDeviceName );

                        InitializeObjectAttributes ( &KeyObjectAttributes,
                                                     RegistryPath,
                                                     OBJ_CASE_INSENSITIVE,
                                                     NULL,
                                                     NULL );

                        status = ZwOpenKey ( &KeyHandle,
                                             KEY_QUERY_VALUE,
                                             &KeyObjectAttributes );

                        if ( NT_SUCCESS ( status ) )
                            {
                            RtlInitUnicodeString ( &uniKeyValueName,
                                                   szNAVAPStackSwappingLevelRegKeyname );

                            status = ZwQueryValueKey ( KeyHandle,
                                                       &uniKeyValueName,
                                                       KeyValuePartialInformation,
                                                       &KeyValue,
                                                       sizeof(KeyValue),
                                                       &KeyValueLength );

                            if ( NT_SUCCESS ( status ) )
                                {
                                if ( KeyValue.info.Type == REG_DWORD &&
                                     KeyValue.info.DataLength == sizeof(DWORD) &&
                                     *((PDWORD)KeyValue.info.Data) == 1 )
                                    {
                                    SYMNTK_SetStackSwappingLevel ( 1 );
                                    }
                                else
                                    {
                                    UIInitError ( MSG_Invalid_Stack_Swapping_Level );
                                    }
                                }
                            else
                                {
                                if ( status != STATUS_OBJECT_NAME_NOT_FOUND )
                                    {
                                    UIInitError ( MSG_Invalid_Stack_Swapping_Level );
                                    }
                                }

                            status = ZwClose ( KeyHandle );

                            if ( !NT_SUCCESS ( status ) )
                                {
                                UIInitError ( MSG_Unable_To_Close_RegistryPath );
                                }
                            }
                        else
                            {
                            UIInitError ( MSG_Unable_To_Open_RegistryPath );
                            }

                                        // Initialize Core.

                        if ( CoreGlobalInit() )
                            {
                                        // Now that this is done, do a global
                                        // initialization of NAVAP.

                            if ( NAVAPGlobalInitEx() )
                                {
                                return ( STATUS_SUCCESS );
                                }

                            if ( !CoreGlobalDeInit() )
                                {
                                SYM_ASSERT ( FALSE );
                                }
                            }
                        else
                            {
                            UIInitError ( MSG_Core_Init_Failed );
                            }
                        }
                    else
                        {
                        UIInitError ( MSG_SYMEvent_Incompatible_Version );
                        }

                    status = STATUS_UNSUCCESSFUL;
                    }
                else
                    {
                    UIInitError ( MSG_SYMEvent_Not_Loaded );
                    }

                                        // SYMEventFileObject may be
                                        // referenced only if we have
                                        // acquired pointer to the device
                                        // object, but since the docs aren't
                                        // clear on that, dereference it as
                                        // long as it is a valid pointer.

                if ( SYMEventFileObject )
                    {
                    ObDereferenceObject ( SYMEventFileObject );
                    }

                IoDeleteSymbolicLink ( &uniWin32NameString );
                }
            else
                {
                UIInitError ( MSG_Unable_To_Create_Symbolic_Link );
                }

            IoDeleteDevice ( DriverObject->DeviceObject );
            }
        else
            {
            UIInitError ( MSG_Unable_To_Create_Device );
            }
        }
    else
        {
        UIInitError ( MSG_Unable_To_Build_NAVAP_SYS_Name );
        }

    return ( status );
    }

#define DRIVER_DEFAULT_CODE_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// NTSTATUS NAVAPAsyncCreateClose (
//      IN PDEVICE_OBJECT DeviceObject,
//      IN PIRP Irp
//      );
//
// This is the dispatch routine for IRP_MJ_CREATE and IRP_MJ_CLOSE.  It
// only marks the request complete and returns to the I/O manager.
//
// Entry:
//      DeviceObject - pointer to NAVAP's device object???
//      Irp          - pointer to the IRP
//
// Exit:
//      Returns STATUS_SUCCESS.
//
//---------------------------------------------------------------------------
NTSTATUS NAVAPAsyncCreateClose (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
    {
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest ( Irp, IO_NO_INCREMENT );

    return ( STATUS_SUCCESS );
    }

//---------------------------------------------------------------------------
//
// NTSTATUS NAVAPAsyncDIOCTL (
//      IN PDEVICE_OBJECT DeviceObject,
//      IN PIRP Irp
//      );
//
// This is the dispatch routine for IRP_MJ_DEVICE_CONTROL.  It first calls
// SymKrnl to check for intenral DIOCTL requests (such as change of last
// mode to kernel mode), and then it calls DriverComm to complete the
// processing of the IRP.
//
// Entry:
//      DeviceObject - pointer to NAVAP's device object???
//      Irp          - pointer to the IRP
//
// Exit:
//      Returns STATUS_SUCCESS.
//
//---------------------------------------------------------------------------
NTSTATUS NAVAPAsyncDIOCTL (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
    {
    if ( SYMNTK_KernelModeXfer_CheckDIOCTL ( DeviceObject, Irp ) )
        {
        return ( STATUS_SUCCESS );
        }

//------------------------------------------------------------------ CUT HERE
// This is a patch for supporting the process protection APIs
// provided by older versions of NAVAP (Luigi and Kirin).  This
// patch should be removed as soon as all products using the old
// APIs are updated and distributed.
//
{
#define OLDSYMDIOCTLGEN(a) CTL_CODE(FILE_DEVICE_UNKNOWN, 0xAA0+a, METHOD_NEITHER, FILE_ANY_ACCESS)

#define OLD_UNPROTECTPROCESS   OLDSYMDIOCTLGEN(28)
#define OLD_PROTECTPROCESS     OLDSYMDIOCTLGEN(29)

#include "apcomm.h"

auto PIO_STACK_LOCATION pIrpStack;

pIrpStack = IoGetCurrentIrpStackLocation ( Irp );

if ( pIrpStack->Parameters.DeviceIoControl.Type3InputBuffer &&
     pIrpStack->Parameters.DeviceIoControl.InputBufferLength == 7 * sizeof(DWORD) &&
     Irp->UserBuffer &&
     pIrpStack->Parameters.DeviceIoControl.OutputBufferLength == sizeof(DWORD) )
    {
    switch ( pIrpStack->Parameters.DeviceIoControl.IoControlCode )
        {
        case OLD_UNPROTECTPROCESS:

            NAVAPServiceCallback ( NAVAP_COMM_UNPROTECTPROCESS,
                                   NULL,
                                   0,
                                   NULL,
                                   0,
                                   NULL );
            *(PDWORD)Irp->UserBuffer = 0;
            Irp->IoStatus.Information = sizeof(DWORD);
            Irp->IoStatus.Status = STATUS_SUCCESS;
            IoCompleteRequest ( Irp, IO_NO_INCREMENT );

            return ( STATUS_SUCCESS );

        case OLD_PROTECTPROCESS:

            NAVAPServiceCallback ( NAVAP_COMM_PROTECTPROCESS,
                                   NULL,
                                   0,
                                   NULL,
                                   0,
                                   NULL );

            *(PDWORD)Irp->UserBuffer = 0;
            Irp->IoStatus.Information = sizeof(DWORD);
            Irp->IoStatus.Status = STATUS_SUCCESS;
            IoCompleteRequest ( Irp, IO_NO_INCREMENT );

            return ( STATUS_SUCCESS );
        }
    }
}
//------------------------------------------------------------------ CUT HERE

    return ( DriverCommNTDispatchDIOC ( DeviceObject, Irp ) );
    }

//---------------------------------------------------------------------------
//
// VOID NAVAPNTUnload (
//      IN PDRIVER_OBJECT DriverObject
//      );
//
// This function is called when NAVAP is about to get unloaded.  First, it
// makes sure that the driver is disabled and uninitialized.  This refers to
// the initialization and enabling done upon request by user-mode
// application - refer to the header of DriverEntry().  After NAVAP is
// disabled and uninitialized, this function proceeds to undo everything
// that DriverEntry() did.
//
// Entry:
//      DriverObject - pointer to NAVAP's driver object.
//
// Exit:
//      No return value.
//
//---------------------------------------------------------------------------
VOID NAVAPNTUnload (
    IN PDRIVER_OBJECT DriverObject
    )
    {
    auto UNICODE_STRING uniWin32NameString;
    auto PVOID          pvCoreInit;

                                        // Make sure NAVAP is disabled and
                                        // uninitialized.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();

        SYM_ASSERT ( FALSE );
        }

    if ( EnterControlledDeInit() != ERROR_SUCCESS )
        {
        SYM_ASSERT ( FALSE );
        }

    CoreLocalDeInit ( pvCoreInit );

                                        // Uninitialize NAVAP globally.

    if ( !NAVAPGlobalDeInitEx() )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Stop using Core.

    if ( !CoreGlobalDeInit() )
        {
        SYM_ASSERT ( FALSE );
        }

                                        // Unlock SymEvent.

    if ( SYMEventFileObject )
        {
        ObDereferenceObject ( SYMEventFileObject );
        }

                                        // Delete the symbolic link for
                                        // communicating with drivers and
                                        // applications.

    RtlInitUnicodeString ( &uniWin32NameString, szNAVAPNTDeviceCommName );
    IoDeleteSymbolicLink ( &uniWin32NameString );

                                        // Delete device object.

    IoDeleteDevice ( DriverObject->DeviceObject );

                                        // Release pszNAVAPSYSStartupName.

    SYM_ASSERT ( pszNAVAPSYSStartupName );

    ExFreePool ( pszNAVAPSYSStartupName );
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPStartupTasks (
//      VOID
//      );
//
// This function is called during controlled initialization and performs the
// startup tasks that NAVAP is configured to do.  These tasks are the
// following:
//
// 1) NAVAP integrity self test.
//
// The SYM_VXD implementation of NAVAPStartupTasks() may perform different
// tasks.
//
// NAVAPStartupTasks() will communicate to the user any errors and warnings
// generated during processing.
//
// Entry:
//      -
//
// Exit:
//      TRUE if the NAVAP controlled initialization should continue, or
//      FALSE if the initialization should be failed.
//
//---------------------------------------------------------------------------
BOOL NAVAPStartupTasks (
    VOID
    )
    {
                                        // Integrity self test.

    return ( CheckIfNAVAPIntact() );
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPExitTasks (
//      VOID
//      );
//
// This function is called during controlled deinitialization and performs
// the exit tasks that NAVAP is configured to do.  No exit tasks are
// currently defined.
//
// The SYM_VXD implementation of NAVAPExitTasks() may perform different
// tasks.
//
// NAVAPExitTasks() will communicate to the user any errors and warnings
// generated during processing.
//
// Entry:
//      -
//
// Exit:
//      Always TRUE.
//
//---------------------------------------------------------------------------
BOOL NAVAPExitTasks (
    VOID
    )
    {
    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// NTSTATUS SaveNAVAPSYSStartupName (
//      IN PUNICODE_STRING NAVAPKey
//      );
//
// This function retrieves and saves the name of the driver which is
// currently executing.  The name of the driver is saved in a memory block
// pointed to by pszNAVAPSYSStartupName.  This memroy block is allocated
// through ExAllocatePool() and should be released by the client.
//
// Entry:
//      NAVAPKey - path to the location in the registry where the driver
//                 was loaded from.  This looks like:
//                 \Registry\Machine\System\CurrentControlSet\Services\NAVAP
//
// Exit:
//      STATUS_SUCCESS if successful, otherwise error.
//
// Note:
//      The code for SaveNAVAPSYSStartupName() will be discarded after
//      initialization.
//
//---------------------------------------------------------------------------
#define DRIVER_INIT_CODE_SEGMENT
#include "drvseg.h"

NTSTATUS SaveNAVAPSYSStartupName (
    IN PUNICODE_STRING NAVAPKey
    )
    {
    auto NTSTATUS                    status;
    auto OBJECT_ATTRIBUTES           objectAttributes;
    auto HANDLE                      hKey;
    auto UNICODE_STRING              imagepathName;
    auto ULONG                       ulLength;
    auto PKEY_VALUE_FULL_INFORMATION KeyValueImagePath = NULL;
    auto PWSTR                       pszImagePath;

                                        // Validate the registry key.  This
                                        // shouldn't fail, and if it does we
                                        // will assume that the key entry
                                        // given to us is invalid, so any
                                        // further processing would be
                                        // useless.

    InitializeObjectAttributes ( &objectAttributes,
                                 NAVAPKey,
                                 OBJ_CASE_INSENSITIVE,
                                 NULL,
                                 NULL );

    status = ZwOpenKey ( &hKey, KEY_READ, &objectAttributes );

    if ( !NT_SUCCESS ( status ) )
        {
        return ( status );
        }

                                        // First, check if the registry
                                        // entry for the driver given to us
                                        // has an image path specified.  Any
                                        // errors with the registry from
                                        // here on will force us to generate
                                        // the name using
                                        // \\SystemRoot\\System32\\Drivers\\.

    RtlInitUnicodeString ( &imagepathName, szImagePathValueName );

    status = ZwQueryValueKey ( hKey,
                               &imagepathName,
                               KeyValueFullInformation,
                               NULL,
                               0,
                               &ulLength );

    if ( ( status == STATUS_BUFFER_OVERFLOW ||
           status == STATUS_BUFFER_TOO_SMALL ) &&

         ( KeyValueImagePath = (PKEY_VALUE_FULL_INFORMATION)
               ExAllocatePool ( NonPagedPool, ulLength + sizeof(WCHAR) ) ) &&

         NT_SUCCESS ( ZwQueryValueKey ( hKey,
                                        &imagepathName,
                                        KeyValueFullInformation,
                                        KeyValueImagePath,
                                        ulLength,
                                        &ulLength ) ) &&

         ( KeyValueImagePath->Type == REG_SZ ||
           KeyValueImagePath->Type == REG_EXPAND_SZ ) &&

         KeyValueImagePath->DataLength > 1
       )
        {
                                        // We have an ImagePath section.
                                        // Fully qualified names should
                                        // directly be copied to our
                                        // destination, and relative names
                                        // should have \SystemRoot\
                                        // prepended.

        pszImagePath = (PWSTR) ( (ULONG)KeyValueImagePath +
                                 KeyValueImagePath->DataOffset );

        pszImagePath[KeyValueImagePath->DataLength / sizeof(WCHAR)] = L'\0';

        if ( *pszImagePath == L'\\' )
            {
            pszNAVAPSYSStartupName = (PWSTR)
                ExAllocatePool ( NonPagedPool,
                                 sizeof(WCHAR) *
                                     ( wcslen ( pszImagePath ) + 1 ) );

            if ( pszNAVAPSYSStartupName )
                {
                wcscpy ( pszNAVAPSYSStartupName, pszImagePath );
                }
            }
        else
            {
            pszNAVAPSYSStartupName = (PWSTR)
                ExAllocatePool ( NonPagedPool,
                                 sizeof(WCHAR) *
                                     ( wcslen ( szSystemRoot ) +
                                       wcslen ( pszImagePath ) + 1 ) );

            if ( pszNAVAPSYSStartupName )
                {
                wcscpy ( pszNAVAPSYSStartupName, szSystemRoot );
                wcscat ( pszNAVAPSYSStartupName, pszImagePath );
                }
            }
        }

    if ( KeyValueImagePath )
        {
        ExFreePool ( KeyValueImagePath );
        }

    status = ZwClose ( hKey );

    if ( pszNAVAPSYSStartupName )
        {
        if ( !NT_SUCCESS ( status ) )
            {
            ExFreePool ( pszNAVAPSYSStartupName );
            pszNAVAPSYSStartupName = NULL;
            }
        return ( status );
        }

                                        // We don't have ImagePath, so build
                                        // image name based on
                                        // \\SystemRoot\\System32\\Drivers\\.

    imagepathName = *NAVAPKey;

    imagepathName.MaximumLength = imagepathName.Length;

    if ( ( ulLength = imagepathName.Length / sizeof(WCHAR) ) &&
         imagepathName.Buffer[ulLength - 1] == L'\\' )
        {
        imagepathName.MaximumLength -= sizeof(WCHAR);

        ulLength--;
        }

    if ( !ulLength ||
         imagepathName.MaximumLength < 2 ||
         ( imagepathName.MaximumLength & 1 ) )
        {
        return ( STATUS_INVALID_PARAMETER );
        }

    for ( ; imagepathName.Buffer[--ulLength] != L'\\' && ulLength; );

    if ( imagepathName.Buffer[ulLength] == L'\\' )
        {
        ulLength += 1;
        }

    imagepathName.Length = (USHORT)ulLength * sizeof(WCHAR);

    if ( imagepathName.Length >= imagepathName.MaximumLength )
        {
        return ( STATUS_INVALID_PARAMETER );
        }

    ulLength = imagepathName.MaximumLength - imagepathName.Length +
               sizeof(WCHAR) * ( wcslen ( szSystem32Drivers ) +
                                 wcslen ( szDriverExtension ) + 1 );

    pszNAVAPSYSStartupName = (PWSTR) ExAllocatePool ( NonPagedPool, ulLength );

    if ( !pszNAVAPSYSStartupName )
        {
        return ( STATUS_INSUFFICIENT_RESOURCES );
        }

    memset ( pszNAVAPSYSStartupName, 0, ulLength );

    wcscpy ( pszNAVAPSYSStartupName, szSystem32Drivers );

    memcpy ( &pszNAVAPSYSStartupName[wcslen(pszNAVAPSYSStartupName)],
             &imagepathName.Buffer[imagepathName.Length / sizeof(WCHAR)],
             imagepathName.MaximumLength - imagepathName.Length );

    wcscat ( pszNAVAPSYSStartupName, szDriverExtension );

    return ( STATUS_SUCCESS );
    }

#define DRIVER_DEFAULT_CODE_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// VOID NAVAPNTWriteStandardEventLog (
//      NTSTATUS Status
//      );
//
// This function writes a standard NT event log entry.  Several SYM_NTK
// implementations of APUI functions translate to direct calls to this API.
// These functions are: UIInitError() and UIOutOfMemoryError().
//
// Entry:
//      Status from the NAVAPNT.MC file.  This status translates into a log
//      message.  No dump data will be saved.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
#define DUMP_DATA_ENTRIES 0

VOID NAVAPNTWriteStandardEventLog (
    NTSTATUS Status
    )
    {
    auto PIO_ERROR_LOG_PACKET LogPacket;

                                        // Don't do anything if we don't
                                        // have NAVAP's driver object.

    SYM_ASSERT ( NAVAPDriverObject );

    if ( NAVAPDriverObject )
        {
                                        // Allocate error log entry.

        LogPacket = (PIO_ERROR_LOG_PACKET) IoAllocateErrorLogEntry (
                                               NAVAPDriverObject,
                                               sizeof(IO_ERROR_LOG_PACKET) );

        if ( LogPacket )
            {
                                        // Initialize the entry.

            LogPacket->MajorFunctionCode = 0;
            LogPacket->RetryCount = 0;
            LogPacket->DumpDataSize = 0;
            LogPacket->NumberOfStrings = 0;
            LogPacket->StringOffset = sizeof(IO_ERROR_LOG_PACKET) -
                                      sizeof(ULONG);
            LogPacket->EventCategory = 0;
            LogPacket->ErrorCode = Status;
            LogPacket->UniqueErrorValue = 0;
            LogPacket->FinalStatus = STATUS_SUCCESS;
            LogPacket->SequenceNumber = 0;
            LogPacket->IoControlCode = 0;

                                        // Now log it.

            IoWriteErrorLogEntry ( LogPacket );
            }
        }
    }
