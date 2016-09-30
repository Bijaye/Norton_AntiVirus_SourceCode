// Copyright 1996 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexnt.cpv   1.12   04 Jun 1997 14:47:36   DCHI  $
//
// Description:
//      This file contains the entry point of NAVEX's NT driver.
//
// Contains:
//      DriverEntry()
//      DriverUnload()      (local function)
//      NAVEXAsyncMisc()    (local function)
//      NAVEXAsyncDIOC()    (local function)
//
// Note:
//      Supports SYM_NTK only!
//
//***************************************************************************
// $Log:   S:/NAVEX/VCS/navexnt.cpv  $
// 
//    Rev 1.12   04 Jun 1997 14:47:36   DCHI
// Correction for unreferenced local variable in case of !MVP_ENABLED.
// 
//    Rev 1.11   04 Jun 1997 14:10:08   DCHI
// Temporarily disabled MVP for DEC ALPHA autoprotect.
// 
//    Rev 1.10   29 Apr 1997 17:21:00   DCHI
// Removed #include of symevent.h and removed calls to symevent_get_version.
// 
//    Rev 1.9   29 Apr 1997 11:53:48   DCHI
// Readded MVP initialization.
// 
//    Rev 1.8   24 Apr 1997 17:54:14   DCHI
// Removed initialization for MVP support.
// 
//    Rev 1.7   18 Apr 1997 17:55:24   AOONWAL
// Modified during DEV1 malfunction
// 
//    Rev ABID  16 Apr 1997 19:45:32   DCHI
// Changes so that mvpdef.dat is only loaded from NAV directory.
// 
//    Rev 1.6   07 Apr 1997 18:11:08   DCHI
// Added MVP support.
// 
//    Rev 1.5   26 Dec 1996 15:22:56   AOONWAL
// No change.
// 
//    Rev 1.4   02 Dec 1996 14:00:36   AOONWAL
// No change.
// 
//    Rev 1.3   29 Oct 1996 12:59:28   AOONWAL
// No change.
// 
//    Rev 1.2   24 Apr 1996 13:35:26   RSTANEV
// Creating a Win32 symbolic link (\DosDevices\NAVEX) to \Device\NAVEX.
//
//    Rev 1.1   19 Apr 1996 14:27:56   RSTANEV
// Changed the architecture of the module.
//
//    Rev 1.0   16 Apr 1996 14:28:54   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "drvcomm.h"
#include "ctsn.h"
#include "callback.h"
#include "navex.h"
#include "navexnt.h"

#include "mvp.h"

#if !defined(SYM_NTK)
#error This file supports only SYM_NTK platform!
#endif

//---------------------------------------------------------------------------
//
// Prototype local functions.
//
//---------------------------------------------------------------------------
static VOID DriverUnload (
    IN PDRIVER_OBJECT DriverObject
    );

static NTSTATUS NAVEXAsyncMisc (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

static NTSTATUS NAVEXAsyncDIOC (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

//---------------------------------------------------------------------------
//
// NTSTATUS DriverEntry (
//      IN PDRIVER_OBJECT DriverObject,
//      IN PUNICODE_STRING RegistryPath
//      );
//
// This is the standard NT Driver entry function.  It initializes the
// NAVEX device object, creates symbolic links, and initializes NAVEX.
//
// Entry:
//      DriverObject - pointer to NAVEX's driver object
//      RegistryPath - path to the location in the registry where NAVEX
//                     was loaded from.  This looks like:
//                     \Registry\Machine\System\CurrentControlSet\Services\NAVEX
//
// Exit:
//      STATUS_SUCCESS if NAVEX loaded OK, or STATUS_DRIVER_INTERNAL_ERROR
//      if failure.
//
//---------------------------------------------------------------------------
NTSTATUS DriverEntry (
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
    {
    auto NTSTATUS status;
    auto PDEVICE_OBJECT deviceObject = NULL;
    auto UNICODE_STRING uniNtNameString;
    auto UNICODE_STRING uniWin32NameString;

                                        // Initialize NAVEX device object

    RtlInitUnicodeString ( &uniNtNameString, szNAVEXDeviceName );

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

        DriverObject->MajorFunction[IRP_MJ_CREATE]         = NAVEXAsyncMisc;
        DriverObject->MajorFunction[IRP_MJ_CLOSE]          = NAVEXAsyncMisc;
        DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = NAVEXAsyncDIOC;

        DriverObject->DriverUnload = DriverUnload;

                                        // Create W32 symbolic link so that
                                        // apps can open a handle to NAVEX.

        RtlInitUnicodeString ( &uniWin32NameString,
                               szNAVEXDeviceCommName );

        status = IoCreateSymbolicLink ( &uniWin32NameString,
                                        &uniNtNameString );

#ifdef MVP_ENABLED
        if ( NT_SUCCESS(status) )
            {
                // Initialize Core

                if ( CoreGlobalInit() )
                    {
                    PVOID pvCoreInit;

                    // Now that this is done, do a local
                    //  initialization of Core.

                    if ( pvCoreInit = CoreLocalInit() )
                        {
                        // Initialize MVP.

                        MVPLoadData(NULL);

                        CoreLocalDeInit( pvCoreInit );
                        }
                    else
                    if ( !CoreGlobalDeInit() )
                        {
                        status = STATUS_DRIVER_INTERNAL_ERROR;

                        SYM_ASSERT ( FALSE );
                        }
                    }
                else
                    {
                        status = STATUS_DRIVER_INTERNAL_ERROR;

                        SYM_ASSERT ( FALSE );
                    }
            }
#endif // #ifdef MVP_ENABLED

        if ( !NT_SUCCESS(status) )
            {
            IoDeleteDevice ( DriverObject->DeviceObject );
            }

        }

    return ( status );
    }

//---------------------------------------------------------------------------
//
// static VOID DriverUnload (
//      IN PDRIVER_OBJECT DriverObject
//      );
//
// This function gets called when NAVEX is unloaded.  It uninitializes
// Core, and returns.
//
// Entry:
//      DriverObject - pointer to NAVEX's driver object.
//
// Exit:
//      No return value.
//
//---------------------------------------------------------------------------
static VOID DriverUnload (
    IN PDRIVER_OBJECT DriverObject
    )
    {
    auto UNICODE_STRING uniWin32NameString;

                                        // Delete the symbolic link for W32
                                        // apps.

    RtlInitUnicodeString ( &uniWin32NameString, szNAVEXDeviceCommName );
    IoDeleteSymbolicLink ( &uniWin32NameString );

                                        // Delete device object

    IoDeleteDevice ( DriverObject->DeviceObject );

#ifdef MVP_ENABLED
    PVOID pvCoreInit;

    // Do a local initialization of Core.

    if ( pvCoreInit = CoreLocalInit() )
        {
        // Uninitialize MVP.

        MVPUnloadData();

        CoreLocalDeInit( pvCoreInit );
        }

    if ( !CoreGlobalDeInit() )
        {
        SYM_ASSERT ( FALSE );
        }
#endif // #ifdef MVP_ENABLED
    }

//---------------------------------------------------------------------------
//
// static NTSTATUS NAVEXAsyncMisc (
//      IN PDEVICE_OBJECT DeviceObject,
//      IN PIRP Irp
//      );
//
// This is the dispatch routine for IRP_MJ_CREATE and IRP_MJ_CLOSE.  It
// only marks the request complete and returns to the I/O manager.
//
// Entry:
//      DeviceObject - pointer to NAVEX's device object???
//      Irp          - pointer to the IRP
//
// Exit:
//      Returns STATUS_SUCCESS.
//
//---------------------------------------------------------------------------
static NTSTATUS NAVEXAsyncMisc (
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
// static NTSTATUS NAVEXAsyncDIOC (
//      IN PDEVICE_OBJECT DeviceObject,
//      IN PIRP Irp
//      );
//
// This is NAVEX's dispatch routine for IRP_MJ_DEVICE_CONTROL.  It services
// the following DIOC calls:
//
//      DIOCTL_SYM_USER00:
//          Returns at the output buffer the NAVEX version number, as a
//          DWORD.  The returned value will be formed as follows:
//          ( NAVEX_MAJOR_VERSION << 8 ) | NAVEX_MINOR_VERSION
//
//      DIOCTL_SYM_USER01:
//          Returns at the output buffer the export table of NAVEX, as
//          a PEXPORT_TABLE_TYPE.
//
// Entry:
//      DeviceObject - look at NT DDK.
//      Irp          - look at NT DDK.
//
// Exit:
//      Returns STATUS_SUCCESS if DeviceIoControl call serviced, otherwise
//      returns STATUS_NOT_IMPLEMENTED.
//
//---------------------------------------------------------------------------
static NTSTATUS NAVEXAsyncDIOC (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
    {
    auto PIO_STACK_LOCATION pIrpStack;

                                        // Get the IRP stack location, so
                                        // that we can access the DIOC call
                                        // information.

    pIrpStack = IoGetCurrentIrpStackLocation ( Irp );

                                        // Process the DIOC request.

    switch ( pIrpStack->Parameters.DeviceIoControl.IoControlCode )
        {
        case DIOCTL_SYM_USER00:
                                        // Get version.

            if ( Irp->UserBuffer &&
                 pIrpStack->Parameters.DeviceIoControl.OutputBufferLength == sizeof(DWORD) )
                {
                *(DWORD *)Irp->UserBuffer = ( NAVEX_MAJOR_VERSION << 8 ) | NAVEX_MINOR_VERSION;

                Irp->IoStatus.Status = STATUS_SUCCESS;
                Irp->IoStatus.Information = sizeof(DWORD);
                }
            else
                {
                goto not_implemented;
                }

            break;

        case DIOCTL_SYM_USER01:
                                        // Get export table.

            if ( Irp->UserBuffer &&
                 pIrpStack->Parameters.DeviceIoControl.OutputBufferLength == sizeof(PEXPORT_TABLE_TYPE) )
                {
                *(PEXPORT_TABLE_TYPE *)Irp->UserBuffer = &export_table;

                Irp->IoStatus.Status = STATUS_SUCCESS;
                Irp->IoStatus.Information = sizeof(PEXPORT_TABLE_TYPE);
                }
            else
                {
                goto not_implemented;
                }

            break;

        default:

not_implemented:

            Irp->IoStatus.Status = STATUS_NOT_IMPLEMENTED;
            Irp->IoStatus.Information = 0;
        }

    IoCompleteRequest ( Irp, IO_NO_INCREMENT );

    return ( Irp->IoStatus.Status );
    }


