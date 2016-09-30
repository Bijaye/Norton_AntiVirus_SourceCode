#if !defined(SYM_NTK)
#error This file supports only SYM_NTK platform!
#endif

#include "platform.h"
#include "drvcomm.h"
#include "navapel.h"

BOOL NAVAPELGlobalInit (
    VOID
    );

//---------------------------------------------------------------------------
//
// Local function prototypes.
//
//---------------------------------------------------------------------------
NTSTATUS NAVAPAsyncCreateClose (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS NAVAPAsyncDIOC (
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
// This is standard NT Driver entry function.  It initializes the NAVAPEL
// device object, creates a symbolic link for device IO control, and then
// initializes NAVAPEL.
//
// Entry:
//      DriverObject - pointer to NAVAPEL's driver object
//      RegistryPath - path to the location in the registry where NAVAPEL
//                     was loaded from.  This looks like:
//                     \Registry\Machine\System\CurrentControlSet\Services\NAVAPEL
//
// Exit:
//      STATUS_SUCCESS if NAVAPEL initialized, or other, depending on
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
    auto UNICODE_STRING    uniNtNameString;
    auto UNICODE_STRING    uniWin32NameString;

    // Initialize NAVAPEL device object.

    RtlInitUnicodeString ( &uniNtNameString, L"\\Device\\NAVAPEL" );

    status = IoCreateDevice ( DriverObject,
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
        DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = NAVAPAsyncDIOC;

        // Create symbolic link so that applications and drivers can open a handle
        // to NAVAPEL and issue DeviceIoControl() calls.

        RtlInitUnicodeString ( &uniWin32NameString, L"\\DosDevices\\NAVAPEL" );

        status = IoCreateSymbolicLink ( &uniWin32NameString, &uniNtNameString );

        if ( NT_SUCCESS ( status ) )
        {
            // Initialize Core.

            if ( CoreGlobalInit() )
            {
                // Now that this is done, do a global
                // initialization of NAVAPEL.

                if ( NAVAPELGlobalInit() )
                {
                    return ( STATUS_SUCCESS );
                }

                if ( !CoreGlobalDeInit() )
                {
                    SYM_ASSERT ( FALSE );
                }
            }

            status = STATUS_UNSUCCESSFUL;

            IoDeleteSymbolicLink ( &uniWin32NameString );
        }

        IoDeleteDevice ( DriverObject->DeviceObject );
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
//      DeviceObject - pointer to NAVAPEL's device object???
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

NTSTATUS NAVAPAsyncDIOC (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    auto PIO_STACK_LOCATION pIrpStack;

    pIrpStack = IoGetCurrentIrpStackLocation ( Irp );

    if ( pIrpStack->Parameters.DeviceIoControl.IoControlCode == DIOCTL_SYM_USER01 )
    {
        if ( Irp->UserBuffer &&
             pIrpStack->Parameters.DeviceIoControl.OutputBufferLength == sizeof(fnQueryNAVAPELInterface) )
        {
            * ((fnQueryNAVAPELInterface *)Irp->UserBuffer) = NAVAPELQueryInterface;

            Irp->IoStatus.Status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(fnQueryNAVAPELInterface);
        }
        else
        {
            Irp->IoStatus.Status = STATUS_NOT_IMPLEMENTED;
            Irp->IoStatus.Information = 0;
        }

        auto NTSTATUS status = Irp->IoStatus.Status;

        IoCompleteRequest ( Irp, IO_NO_INCREMENT );

        return status;
    }

    return DriverCommNTDispatchDIOC ( DeviceObject, Irp );
}