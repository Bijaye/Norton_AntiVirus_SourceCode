// Copyright 1996 - 1997 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/SYMKRNL/VCS/DRVCOMMK.CPv   1.7   03 Jun 1997 16:46:16   RStanev  $
//
// Description:
//      This file contains kernel mode routines for driver communication
//      with user mode clients.
//
// Contains:
//      ENTER_FUNCTION()                     (local macro)
//      EXIT_FUNCTION()                      (local macro)
//      ACQUIRE_DRIVERCOMM_LOCK()            (local macro)
//      RELEASE_DRIVERCOMM_LOCK()            (local macro)
//
//      DriverCommInit()                     (SYM_VXD, SYM_NTK)
//      DriverCommDeInit()                   (SYM_VXD, SYM_NTK)
//      DriverCommRegisterServiceCallback()  (SYM_VXD, SYM_NTK)
//      DriverCommRegisterCancelCallback()   (SYM_VXD, SYM_NTK)
//      DriverCommSend()                     (SYM_VXD, SYM_NTK)
//      DriverCommAsyncSend()                (SYM_VXD, SYM_NTK)
//
//      DriverCommDispatchDIOC()             (SYM_VXD, SYM_NTK)
//
//      DriverCommServiceRequest()           (local function)
//      DriverCommServicesInit()             (local function)
//      DriverCommServicesDeInit()           (local function)
//      DriverCommServicesRelease()          (local function)
//      DriverCommGetPacket()                (local function)
//
//      DriverCommNTDispatchDIOC()           (SYM_NTK only)
//      DriverCommW95DispatchDIOC()          (SYM_VXD only)
//
// See Also:
//      DRVCOMM.H, DRVCOMMU.CPP
//
// Platforms supported:
//      SYM_NTK, SYM_VXD
//
//***************************************************************************
// $Log:   S:/SYMKRNL/VCS/DRVCOMMK.CPv  $
// 
//    Rev 1.7   03 Jun 1997 16:46:16   RStanev
// Removed a timeout to release the blocked thread in DriverCommGetPacket()
// every once in a while.
//
//    Rev 1.6   03 Jun 1997 11:38:34   RStanev
// Added a timeout to release the blocked thread in DriverCommGetPacket()
// every once in a while.
//
//    Rev 1.5   06 Apr 1997 15:51:44   RStanev
// Unified communications with NTK and VxD platforms.  Added callbacks
// for cancelled packet requests.  Prepared module for synchronous
// communication.
//
//    Rev 1.4   21 Jan 1997 18:47:20   RSTANEV
// Added DriverCommW95DispatchDIOC().
//
//    Rev 1.3   06 Jan 1997 17:48:44   RSTANEV
// Added WINAPI to the prototypes of DriverCommDispatchDIOC() and
// DriverCommDIOC().
//
//    Rev 1.2   06 Aug 1996 14:01:48   RSTANEV
// Changed DriverCommNTDispatchDIOC() to access its return value through
// a local variable, instead of looking it up in a completed Irp.
//
//    Rev 1.1   04 May 1996 13:46:26   RSTANEV
// Changed to follow the new requirements of SYMSYNC.H.
//
//    Rev 1.0   18 Mar 1996 11:10:44   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "symsync.h"
#include "drvcomm.h"

//---------------------------------------------------------------------------
//
// Define the ENTER_FUNCTION(e) and EXIT_FUNCTION(e) macros for avoiding
// reentrancy in some DriverComm functions.
//
// ENTER_FUNCTION(e), if placed at the beginning of a function will make
// sure that no more than one caller is executing the body of that function.
// All callers, except the one currently in the function will force the
// macro to exit the function immediately, returning the value e.
//
// EXIT_FUNCTION(e) should be used to return from a function, which used
// the ENTER_FUNCTION(e) macro to synchronize its execution.  Use this macro
// instead of the standard "return" statement.
//
// Note: The usage of ENTER_FUNCTION() and EXIT_FUNCTION() will force the
// creation of a static object "lReentrancyFlag" in the current namespace.
// Because lReentrancyFlag is used with interlocked functions, the default
// segment for data in the functions using ENTER_FUNCTION and EXIT_FUNCTION
// should be DRIVER_NTK_LOCKED_DATA_SEGMENT.
//
//---------------------------------------------------------------------------
#define ENTER_FUNCTION(e)   static LONG lReentrancyFlag = 0;                  \
                            if ( SymInterlockedExchange(&lReentrancyFlag,1) ) \
                                {                                             \
                                return e;                                     \
                                }

#define EXIT_FUNCTION(e)    lReentrancyFlag = 0;                              \
                            return e;

//---------------------------------------------------------------------------
//
// Define the ACQUIRE_DRIVERCOMM_LOCK(n) and RELEASE_DRIVERCOMM_LOCK(n)
// macros for protecting shared resources.
//
// These macros are designed to protect a resource from usage by multiple
// clients at a time.  They act almost like mutexes, except that if the
// protected resource (ID is provided by n) is locked by someone else, the
// ACQUIRE_DRIVERCOMM_LOCK() macro will not block - it will return FALSE.
// If the resource was locked successfully, then the macro will return TRUE.
//
// Currently defined resource IDs are: API, QUEUE.
//
//---------------------------------------------------------------------------
#define ACQUIRE_DRIVERCOMM_LOCK(n) ( SymInterlockedExchange(&lDriverCommLock##n,1) ? \
                                     FALSE : TRUE )

#define RELEASE_DRIVERCOMM_LOCK(n) lDriverCommLock##n = 0

#define DRIVER_NTK_LOCKED_DATA_SEGMENT
#include "drvseg.h"

static LONG lDriverCommLockAPI = FALSE;
static LONG lDriverCommLockQUEUE = FALSE;

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// Define DriverComm status variables.
//
// bDriverCommInitialized shows whether the the DriverCommInit() function
// has completed successfully and DriverComm is ready for use.
//
// bDriverCommServicesInitialized will be set to TRUE whenever a DriverComm
// class has registered a service callback to service driver requests.
//
// lDriverCommUsers contains the number of callers to certain DriverComm
// APIs.  This variable is used to protect the DriverComm class from being
// uninitialized due to improper use of the module.
//
// bBufferResizeRequest will be set to TRUE whenever the service request
// thread is asked to increase the length of the buffer where DriverComm
// copies the request packet.
//
//---------------------------------------------------------------------------
#define DRIVER_NTK_LOCKED_DATA_SEGMENT
#include "drvseg.h"

static LONG lDriverCommUsers = 0;

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

static LONG bDriverCommInitialized = FALSE;

static BOOL bDriverCommServicesInitialized = FALSE;

static BOOL bBufferResizeRequest = FALSE;

//---------------------------------------------------------------------------
//
// Define variables for working with the queues and their synchronization.
//
// prServiceQueueHead and prServiceQueueTail point to the first and the last
// members of the main queue.
//
// prSyncServiceQueueHead and prSyncServiceQueueTail point to the first and
// the last members of the queue with synchronous service requests which are
// waiting for user mode response.
//
// Items are added to the tail of the queues, and are removed from the head.
// rServiceQueueMutex is a mutex object protecting prServiceQueueHead,
// prServiceQueueTail, prSyncServiceQueueHead, and prSyncServiceQueueTail.
//
//---------------------------------------------------------------------------
static PDRVCOMMDIOCTLPACKET prServiceQueueHead = NULL;
static PDRVCOMMDIOCTLPACKET prServiceQueueTail = NULL;

static PDRVCOMMDIOCTLPACKET prSyncServiceQueueHead = NULL;
static PDRVCOMMDIOCTLPACKET prSyncServiceQueueTail = NULL;

#define DRIVER_NTK_LOCKED_DATA_SEGMENT
#include "drvseg.h"

static SYMMUTEX rServiceQueueMutex = {0};

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// Define variables for synchronizing the DriverComm service thread.
//
// rServiceThreadSemaphore is a semaphore on which
// DIOCTL_SYM_INTERNAL_GET_DRVCOMMDIOCTLPACKET will block if the queue is
// empty.
//
//---------------------------------------------------------------------------
#define DRIVER_NTK_LOCKED_DATA_SEGMENT
#include "drvseg.h"

static SYMSEMAPHORE rServiceThreadSemaphore = {0};

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// Define DriverComm callbacks.
//
//---------------------------------------------------------------------------
static PDRVCOMMCALLBACK pfnCallback = NULL;

static PDRVCOMMCANCELCALLBACK pfnCancelCallback = NULL;

//---------------------------------------------------------------------------
//
// Local function prototypes.
//
//---------------------------------------------------------------------------
static DWORD DriverCommServiceRequest (
    PDRVCOMM_DIOC prSendIOParams
    );

static DWORD DriverCommServicesInit (
    VOID
    );

static DWORD DriverCommServicesDeInit (
    VOID
    );

static DWORD DriverCommServicesRelease (
    VOID
    );

static DWORD DriverCommGetPacket (
    PDRVCOMM_DIOC prSendIOParams
    );

//---------------------------------------------------------------------------
//
// DWORD DriverCommInit (
//      VOID
//      );
//
// This routine initializes the kernel mode portion of DriverComm.  It sets
// up a queue that maintains list of request packets that the driver wants
// to be serviced by a user mode application.
//
// Before calling DriverCommInit(), make sure that Core is properly
// initialized by calling CoreGlobalInit() and CoreLocalInit().
//
// Entry:
//      -
//
// Exit:
//      This function returns one of the following:
//          ERROR_DRVCOMM_SUCCESS,
//          ERROR_DRVCOMM_UNSAFE,
//          ERROR_DRVCOMM_ALREADY_INITIALIZED,
//          ERROR_DRVCOMM_NO_MUTEX,
//          ERROR_DRVCOMM_NO_SEMAPHORE.
//
// Synchronization:
//      DriverCommInit() should be called, and should successfully complete
//      before any of the other DriverComm...() functions are called.  Once
//      this function completes successfully, you should not call it again,
//      unless a successful call to DriverCommDeInit() is made.
//
//      This function is protected by the API DriverComm lock.  If this
//      function is entered while another function protected by the same
//      lock is executing, this function will return ERROR_DRVCOMM_UNSAFE.
//      In this case, the client should call the function after a small
//      delay.
//
//---------------------------------------------------------------------------
DWORD DriverCommInit (
    VOID
    )
    {
                                        // Acquire the API DriverComm lock
                                        // and tell all other functions using
                                        // the same lock to fail.

    if ( !ACQUIRE_DRIVERCOMM_LOCK(API) )
        {
        return ( ERROR_DRVCOMM_UNSAFE );
        }

                                        // Now we are the "exclusive" owners
                                        // of bDriverCommInitialized.  Only
                                        // we are guaranteed to be able to
                                        // modify it.  But, changing of this
                                        // variable is still very delicate,
                                        // since other DriverComm clients not
                                        // using the API lock (such as DIOCTL
                                        // or callback registration APIs) may
                                        // verify its status while we are
                                        // working.  Together with
                                        // lDriverCommUsers we can properly
                                        // synchronize the DriverComm APIs.

    if ( bDriverCommInitialized )
        {
                                        // bDriverCommInitialized is already
                                        // set.  The client is improperly
                                        // calling DriverCommInit().

        RELEASE_DRIVERCOMM_LOCK(API);

        return ( ERROR_DRVCOMM_ALREADY_INITIALIZED );
        }

                                        // Make sure no DriverComm APIs not
                                        // protected by the API lock are
                                        // past the point of where they rely
                                        // on bDriverCommInitialized staying
                                        // unchanged.

    if ( lDriverCommUsers )
        {
                                        // There is someone who has "shared"
                                        // ownership of
                                        // bDriverCommInitialized.  We can't
                                        // continue until
                                        // bDriverCommInitialized is not in
                                        // use.  Note, that eventhough a
                                        // DriverComm function has
                                        // incremented the usage count, it
                                        // will fail almost immediately due
                                        // to the fact that
                                        // bDriverCommInitialized == FALSE.

        RELEASE_DRIVERCOMM_LOCK(API);

        return ( ERROR_DRVCOMM_UNSAFE );
        }

                                        // At this point, we are sure that
                                        // no DriverComm APIs will start
                                        // using the resources which we will
                                        // initialize now.

                                        // Initialize service queue mutex.

    if ( !SymCreateMutex ( &rServiceQueueMutex ) )
        {
        RELEASE_DRIVERCOMM_LOCK(API);

        return ( ERROR_DRVCOMM_NO_MUTEX );
        }

                                        // Create semaphore for blocking
                                        // the service thread.

    if ( !SymCreateSemaphore ( &rServiceThreadSemaphore, 0 ) )
        {
        SymDestroyMutex ( &rServiceQueueMutex );

        RELEASE_DRIVERCOMM_LOCK(API);

        return ( ERROR_DRVCOMM_NO_SEMAPHORE );
        }

                                        // Allow DIOCTL or callback
                                        // registration APIs to work.

    bDriverCommInitialized = TRUE;

                                        // Release the API DriverComm lock.

    RELEASE_DRIVERCOMM_LOCK(API);

    return ( ERROR_DRVCOMM_SUCCESS );
    }

//---------------------------------------------------------------------------
//
// DWORD DriverCommDeInit (
//      VOID
//      );
//
// This routine deinitializes the DriverComm module.  It cancels all
// outstanding service requests, frees up allocated memory, releases
// resources, etc.
//
// Entry:
//      -
//
// Exit:
//      This function returns one of the following:
//          ERROR_DRVCOMM_SUCCESS,
//          ERROR_DRVCOMM_UNSAFE,
//          ERROR_DRVCOMM_NOT_INITIALIZED.
//
// Synchronization:
//      DriverCommDeInit() should be only after a call to DriverCommInit()
//      has successfully completed.
//
//      This function is protected by the API DriverComm lock.  If this
//      function is entered while another function protected by the same
//      lock is executing, this function will return ERROR_DRVCOMM_UNSAFE.
//      In this case, the client should call the function after a small
//      delay.
//
//---------------------------------------------------------------------------
DWORD DriverCommDeInit (
    VOID
    )
    {
                                        // Acquire the API DriverComm lock
                                        // and tell all other functions using
                                        // the same lock to fail.

    if ( !ACQUIRE_DRIVERCOMM_LOCK(API) )
        {
        return ( ERROR_DRVCOMM_UNSAFE );
        }

                                        // Now we are the "exclusive" owners
                                        // of bDriverCommInitialized.  Only
                                        // we are guaranteed to be able to
                                        // modify it.  But, changing of this
                                        // variable is still very delicate,
                                        // since other DriverComm clients not
                                        // using the API lock (such as DIOCTL
                                        // or callback registration APIs) may
                                        // verify its status while we are
                                        // working.  Together with
                                        // lDriverCommUsers we can properly
                                        // synchronize the DriverComm APIs.

    if ( !bDriverCommInitialized )
        {
                                        // bDriverCommInitialized was not
                                        // set.  The client is misusing the
                                        // DriverComm APIs.

        RELEASE_DRIVERCOMM_LOCK(API);

        return ( ERROR_DRVCOMM_NOT_INITIALIZED );
        }

                                        // We are starting to uninitialize.
                                        // Don't allow anyone to use
                                        // DriverComm anymore.

    bDriverCommInitialized = FALSE;

                                        // Cancel all outstanding packets.
                                        // This will ensure that we release
                                        // all blocked DriverComm users which
                                        // are currently waiting for their
                                        // synchronous requests to be
                                        // serviced.  If we don't do this we
                                        // won't be able to complete the
                                        // function due to shared owners of
                                        // bDriverCommInitialized being
                                        // active.

    SymWaitForMutex ( &rServiceQueueMutex );

    auto PDRVCOMMDIOCTLPACKET prCurrentQueueItem;

                                        // Tell all blocked synchronous
                                        // clients whose packets have been
                                        // sent to user mode that their
                                        // packets may have been serviced,
                                        // or they may not have been
                                        // serviced.  This way the clients
                                        // will take appropriate action.

    while ( prCurrentQueueItem = prSyncServiceQueueHead )
        {
                                        // Remove the current packet entry
                                        // from the head of the queue.

        prSyncServiceQueueHead = prSyncServiceQueueHead->pNext;

                                        // ~~~TBD~~~ Release the blocked
                                        // thread telling the client that its
                                        // request was sent to user mode, but
                                        // we have not received any results.
        }

                                        // Cancel all outstanding main queue
                                        // entries.

    while ( prCurrentQueueItem = prServiceQueueHead )
        {
                                        // Remove the current service packet
                                        // entry from the head of the queue.

        prServiceQueueHead = prServiceQueueHead->pNext;

                                        // Cancel the packet.

                                        // ~~TBD~~ The TRUE constant in the
                                        // the if() statement below should
                                        // be replaced with an expression
                                        // returning TRUE if
                                        // prCurrentQueueItem is an
                                        // asynchronous request, FALSE if
                                        // prCurrentQueueItem is a
                                        // synchronous request.

        if ( TRUE )
            {
                                        // Notify the callback of request
                                        // cancellation.

            if ( pfnCancelCallback )
                {
                pfnCancelCallback ( prCurrentQueueItem->dwServiceCode,
                                    prCurrentQueueItem->dwInBufferSize ?
                                        &prCurrentQueueItem->abyData[0] : NULL,
                                    prCurrentQueueItem->dwInBufferSize );

                }
                                        // Release that packet from memory.

            MemFreePtr ( prCurrentQueueItem );
            }
        else
            {
                                        // ~~~TBD~~~ Release the blocked
                                        // thread telling the client that its
                                        // request can't be serviced because
                                        // there is no user mode code to
                                        // service it.
            }
        }

                                        // Make sure no DriverComm APIs not
                                        // protected by the API lock are
                                        // past the point of where they rely
                                        // on bDriverCommInitialized staying
                                        // unchanged.  Note, that some of
                                        // these APIs may even add packets
                                        // to the queue as soon as we release
                                        // the mutex.

    if ( lDriverCommUsers )
        {
                                        // Someone is using DriverComm APIs.
                                        // This client can be in one of the
                                        // following states: fully utilizing
                                        // DriverComm - it is a "shared"
                                        // owner of bDriverCommInitialized
                                        // and it knows that it is ok to use
                                        // the APIs, this client will
                                        // continue to work fine; a client
                                        // that has incremented the usage
                                        // count, but may not have checked
                                        // bDriverCommInitialized yet - it
                                        // may see that we are either
                                        // initialized, or not, and it may
                                        // either fail, or keep processing
                                        // (depending on when it checks the
                                        // status); In case that client
                                        // fails, we don't care, because if
                                        // the client had made the call a
                                        // a few CPU cycles later, it
                                        // would've failed anyway due to us
                                        // executing this function right now;
                                        // In case that client succeeds,
                                        // that's also ok because we will not
                                        // uninitialize right now (in fact,
                                        // we will restore back
                                        // bDriverCommInitialized).

        SymReleaseMutex ( &rServiceQueueMutex );

        bDriverCommInitialized = TRUE;

        RELEASE_DRIVERCOMM_LOCK(API);

        return ( ERROR_DRVCOMM_UNSAFE );
        }

                                        // At this point, we are sure that
                                        // no APIs will start using any of
                                        // the resources which we will
                                        // destroy.

    SymReleaseMutex ( &rServiceQueueMutex );

                                        // Destroy the semaphore.

    SymDestroySemaphore ( &rServiceThreadSemaphore );

                                        // Destroy queue mutex.

    SymDestroyMutex ( &rServiceQueueMutex );

                                        // Clear the callbacks.

    pfnCallback = NULL;

    pfnCancelCallback = NULL;

                                        // Release the API DriverComm lock.

    RELEASE_DRIVERCOMM_LOCK(API);

    return ( ERROR_DRVCOMM_SUCCESS );
    }

//---------------------------------------------------------------------------
//
// DWORD DriverCommRegisterServiceCallback (
//      PDRVCOMMCALLBACK pfnServiceCallback
//      );
//
// This function registers a service callback which will be called whenever
// a user-mode client calls DriverComm::Send().
//
// Entry:
//      pfnServiceCallback - pointer to a function for servicing DriverComm
//                           requests from user mode.
//
// Exit:
//      This function returns one of the following:
//          ERROR_DRVCOMM_SUCCESS,
//          ERROR_DRVCOMM_NOT_INITIALIZED,
//          ERROR_DRVCOMM_BAD_ARGUMENT.
//
// Synchronization:
//      This function is fully reentrant, and will protect itself from
//      simultaneous calls to DriverCommDeInit().  Even though it is safe
//      to be called several times, and be reentered, this function should
//      be called only once - after a successful call to DriverCommInit().
//
//---------------------------------------------------------------------------
DWORD DriverCommRegisterServiceCallback (
    PDRVCOMMCALLBACK pfnServiceCallback
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( pfnServiceCallback );

    if ( !pfnServiceCallback )
        {
        return ( ERROR_DRVCOMM_BAD_ARGUMENT );
        }

                                        // Increment the number of users of
                                        // DriverComm.  This will prevent
                                        // DriverCommInit() and
                                        // DriverCommDeInit() from changing
                                        // the status.

    SymInterlockedIncrement ( &lDriverCommUsers );

                                        // If DriverComm is not initialized,
                                        // then this is a bogus call.

    if ( !bDriverCommInitialized )
        {
        SymInterlockedDecrement ( &lDriverCommUsers );

        return ( ERROR_DRVCOMM_NOT_INITIALIZED );
        }

                                        // Save the callback.

    SYM_ASSERT ( !pfnCallback );

    pfnCallback = pfnServiceCallback;

                                        // Make sure that DriverCommInit()/
                                        // DriverCommDeInit() can change the
                                        // status from now on.

    SymInterlockedDecrement ( &lDriverCommUsers );

    return ( ERROR_DRVCOMM_SUCCESS );
    }

//---------------------------------------------------------------------------
//
// DWORD DriverCommRegisterCancelCallback (
//      PDRVCOMMCANCELCALLBACK pfnServiceCancelCallback
//      );
//
// This function registers a service cancel callback which will be called
// whenever a queued request packet needs to be cancelled.
//
// Entry:
//      pfnServiceCancelCallback - pointer to a function for servicing the
//                                 cancel notifications.
//
// Exit:
//      This function returns one of the following:
//          ERROR_DRVCOMM_SUCCESS,
//          ERROR_DRVCOMM_NOT_INITIALIZED,
//          ERROR_DRVCOMM_BAD_ARGUMENT.
//
// Synchronization:
//      This function is fully reentrant, and will protect itself from
//      simultaneous calls to DriverCommDeInit().  Even though it is safe
//      to be called several times, and be reentered, this function should
//      be called only once - after a successful call to DriverCommInit().
//
//---------------------------------------------------------------------------
DWORD DriverCommRegisterCancelCallback (
    PDRVCOMMCANCELCALLBACK pfnServiceCancelCallback
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( pfnServiceCancelCallback );

    if ( !pfnServiceCancelCallback )
        {
        return ( ERROR_DRVCOMM_BAD_ARGUMENT );
        }

                                        // Increment the number of users of
                                        // DriverComm.  This will prevent
                                        // DriverCommInit() and
                                        // DriverCommDeInit() from changing
                                        // the status.

    SymInterlockedIncrement ( &lDriverCommUsers );

                                        // If DriverComm is not initialized,
                                        // then this is a bogus call.

    if ( !bDriverCommInitialized )
        {
        SymInterlockedDecrement ( &lDriverCommUsers );

        return ( ERROR_DRVCOMM_NOT_INITIALIZED );
        }

                                        // Save the callback.

    SYM_ASSERT ( !pfnCancelCallback );

    pfnCancelCallback = pfnServiceCancelCallback;

                                        // Make sure that DriverCommInit()/
                                        // DriverCommDeInit() can change the
                                        // status from now on.

    SymInterlockedDecrement ( &lDriverCommUsers );

    return ( ERROR_DRVCOMM_SUCCESS );
    }

//---------------------------------------------------------------------------
//
// DWORD DriverCommSend (
//      DWORD  dwServiceCode,
//      PVOID  pvInBuffer,
//      DWORD  dwInBufferSize,
//      PVOID  pvOutBuffer,
//      DWORD  dwOutBufferSize,
//      PDWORD pdwBytesReturned
//      );
//
// This function sends a synchronous request to the Win32 application
// servicing driver's requests.  DriverCommSend() transfers control to the
// service callback specified by the DriverComm class which established
// connection with this driver.  If no DriverComm class is ready to service
// the request, this function will fail immediately.
//
// Entry:
//      dwServiceCode    - non-0 service request code.
//      pvInBuffer       - pointer to a memory block containing input data
//                         for the application.  If this value is NULL, then
//                         dwInBufferSize is ignored.
//      dwInBufferSize   - length of memory block at pvInBuffer.
//      pvOutBuffer      - pointer to a memory block where the application
//                         should copy any data resulting from servicing the
//                         request.  If this parameter is NULL, then
//                         dwOutBufferSize and pdwBytesReturned are ignored.
//      dwOutBufferSize  - length of memory block at pvOutBuffer.
//      pdwBytesReturned - pointer to a DWORD to receive the number of
//                         bytes copied to pvOutBuffer when the call returns.
//                         This parameter should not be NULL if pvOutBuffer
//                         is supplied.
//
// Exit:
//      This function always returns ERROR_DRVCOMM_NOT_IMPLEMENTED.
//
// Synchronization:
//      This function is fully reentrant, and will protect itself from
//      simultaneous calls to DriverCommDeInit().
//
//---------------------------------------------------------------------------
DWORD DriverCommSend (
    DWORD  dwServiceCode,
    PVOID  pvInBuffer,
    DWORD  dwInBufferSize,
    PVOID  pvOutBuffer,
    DWORD  dwOutBufferSize,
    PDWORD pdwBytesReturned
    )
    {
                                        // Currently synchronous requests
                                        // are not supported.

    return ( ERROR_DRVCOMM_NOT_IMPLEMENTED );
    }

//---------------------------------------------------------------------------
//
// DWORD DriverCommAsyncSend (
//      DWORD dwServiceCode,
//      PVOID pvInBuffer,
//      DWORD dwInBufferSize
//      );
//
// This function sends an asynchronous request to the Win32 application
// servicing driver's requests.  DriverCommAsyncSend() schedules a request
// packet to be sent to the service callback specified by the DriverComm
// class which established connection with this driver.  The packet will be
// picked up by the DriverComm class and processed whenever the application
// is available to do so.  This function will succeed even if no DriverComm
// class is servicing driver's requests at the moment.  All requests will be
// queued until a DriverComm class is ready to service them.
//
// Entry:
//      dwServiceCode  - non-0 service request code.
//      pvInBuffer     - pointer to a memory block containing input data
//                       for the application.  If this value is NULL, then
//                       dwInBufferSize is ignored.
//      dwInBufferSize - length of memory block at pvInBuffer.
//
// Exit:
//      This function returns one of the following:
//          ERROR_DRVCOMM_SUCCESS,
//          ERROR_DRVCOMM_NOT_INITIALIZED,
//          ERROR_DRVCOMM_BAD_ARGUMENT,
//          ERROR_DRVCOMM_OUT_OF_MEMORY.
//
// Synchronization:
//      This function is fully reentrant, and will protect itself from
//      simultaneous calls to DriverCommDeInit().
//
//---------------------------------------------------------------------------
DWORD DriverCommAsyncSend (
    DWORD dwServiceCode,
    PVOID pvInBuffer,
    DWORD dwInBufferSize
    )
    {
    auto PDRVCOMMDIOCTLPACKET prPacket;

                                        // Validate and fix up parameters.

    SYM_ASSERT ( dwServiceCode );
    SYM_ASSERT ( !pvInBuffer || dwInBufferSize );

    if ( !dwServiceCode ||
         ( pvInBuffer && !dwInBufferSize ) )
        {
        return ( ERROR_DRVCOMM_BAD_ARGUMENT );
        }

    if ( !pvInBuffer )
        {
        dwInBufferSize = 0;
        }

                                        // Increment the number of users of
                                        // DriverComm.  This will prevent
                                        // DriverCommInit() and
                                        // DriverCommDeInit() from changing
                                        // the status.

    SymInterlockedIncrement ( &lDriverCommUsers );

                                        // If DriverComm is not initialized,
                                        // then this is a bogus call.

    if ( !bDriverCommInitialized )
        {
        SymInterlockedDecrement ( &lDriverCommUsers );

        return ( ERROR_DRVCOMM_NOT_INITIALIZED );
        }

                                        // We can't hold on to client's
                                        // buffers while the request is
                                        // queued, so allocate memory for
                                        // and build a packet for the queue.

    prPacket = (PDRVCOMMDIOCTLPACKET) MemAllocPtr ( GMEM_MOVEABLE,
                                                    sizeof(DRVCOMMDIOCTLPACKET) +
                                                    dwInBufferSize );

    if ( !prPacket )
        {
        SymInterlockedDecrement ( &lDriverCommUsers );

        return ( ERROR_DRVCOMM_OUT_OF_MEMORY );
        }

                                        // Copy client's data to the packet.

    prPacket->dwSize = sizeof(DRVCOMMDIOCTLPACKET) + dwInBufferSize;

    prPacket->dwServiceCode = dwServiceCode;

    if ( prPacket->dwInBufferSize = dwInBufferSize )
        {
        SYM_VERIFY_BUFFER ( pvInBuffer, dwInBufferSize );
        SYM_VERIFY_BUFFER ( &prPacket->abyData[0], dwInBufferSize );

        memcpy ( &prPacket->abyData[0], pvInBuffer, dwInBufferSize );
        }

                                        // Add the packet to the tail of the
                                        // queue.

    prPacket->pNext = NULL;

    SymWaitForMutex ( &rServiceQueueMutex );

    if ( prServiceQueueHead )
        {
                                        // List is not empty - make the
                                        // packet the tail of the list.

        SYM_ASSERT ( prServiceQueueTail );
        SYM_ASSERT ( !prServiceQueueTail->pNext );

        prServiceQueueTail->pNext = prPacket;

        prServiceQueueTail = prPacket;
        }
    else
        {
                                        // List is empty - make the packet
                                        // both head and tail of the list.

        prServiceQueueHead = prServiceQueueTail = prPacket;
        }

    SymReleaseMutex ( &rServiceQueueMutex );

                                        // Signal semaphore that something
                                        // has been added to the queue.

    SymReleaseSemaphore ( &rServiceThreadSemaphore );

                                        // Make sure that DriverCommInit()/
                                        // DriverCommDeInit() can change the
                                        // status from now on.

    SymInterlockedDecrement ( &lDriverCommUsers );

    return ( ERROR_DRVCOMM_SUCCESS );
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI DriverCommDispatchDIOC (
//      PDIOCPARAMETERS prDIOCParameters
//      );
//
// This function is called by the driver when it receives a DeviceIoControl()
// call.  It examines the request, and if it is a valid DriverComm class
// request, it services it directly.
//
// If the DeviceIoControl() call was properly issued by our DriverComm class,
// the following members of the prDIOCParameters structure will be set as
// follows:
//
//      dwIoControlCode   - DIOCTL_SYM_INTERNAL_??? from DRVCOMM.H
//      lpvInBuffer       - pointer to a DRVCOMM_DIOC structure containing
//                          the parameters given to DriverComm::SendIO().
//      cbInBuffer        - sizeof(DRVCOMM_DIOC)
//      lpvOutBuffer      - pointer to a DWORD to receive the status of
//                          processing of dwIoControlCode.
//      cbOutBuffer       - sizeof(DWORD)
//      lpcbBytesReturned - pointer to a DWORD to receive the number of
//                          bytes written to lpvOutBuffer once request
//                          was serviced.
//
// Other prDIOCParameters members (if any at all) will be ignored.
//
// A properly initializated DRVCOMM_DIOC structure has its members set as
// follows:
//
//      dwTag1            - '>>SR'
//      dwServiceCode     - same as in DriverComm::SendIO()
//      lpvInBuffer       - same as in DriverComm::SendIO()
//      cbInBuffer        - same as in DriverComm::SendIO()
//      lpvOutBuffer      - same as in DriverComm::SendIO()
//      cbOutBuffer       - same as in DriverComm::SendIO()
//      lpcbBytesReturned - same as in DriverComm::SendIO()
//      dwTag2            - 'SR<<'
//
// Supported DIOCTL_SYM_INTERNAL_??? values are:
//
//      DIOCTL_SYM_INTERNAL_VALIDATE:
//
//          Reserved for future use.
//
//      DIOCTL_SYM_INTERNAL_SERVICE_REQUEST:
//
//          This tells the driver that DriverComm::Send() is asking it to
//          service a request.  The request is directly dispatched to the
//          service callback registered by calling
//          DriverCommRegisterServiceCallback().  The parameters to the
//          callback are extracted from the DRVCOMM_DIOC structure.
//
//          For more information on this call, refer to the header of
//          DriverCommServiceRequest().
//
//      DIOCTL_SYM_INTERNAL_INIT_SERVICES
//
//          This value tells the driver that a DriverComm class wants to
//          start issuing DIOCTL_SYM_INTERNAL_GET_DRVCOMMDIOCTL_PACKET calls
//          to service requests.
//
//          For more information on this call, refer to the header of
//          DriverCommServicesInit().
//
//      DIOCTL_SYM_INTERNAL_DEINIT_SERVICES
//
//          This value tells the driver that the DriverComm class which has
//          been given the rights to service requests wants to stop servicing
//          those requests.
//
//          For more information on this request, refer to the header of
//          DriverCommServicesDeInit().
//
//      DIOCTL_SYM_INTERNAL_RELEASE_SERVICES
//
//          This value tells the driver that the DriverComm class which
//          has been given the rights to service requests wants us to release
//          a thread which might potentially be blocked by it.
//
//          For more information on this request, refer to the header of
//          DriverCommServicesRelease().
//
//      DIOCTL_SYM_INTERNAL_GET_DRVCOMMDIOCTL_PACKET
//
//          This value tells the driver to send a service request packet to
//          a DriverComm class for processing.
//
//          For more information on this request, refer to the header of
//          DriverCommGetPacket().
//
// Entry:
//      prDIOCParameters - pointer to a structure, filled with data supplied
//                         by the caller of DeviceIoControl(), which in most
//                         cases would be DriverComm::SendIO().
//
// Exit:
//      This function returns TRUE if prDIOCParameters was formatted properly
//      by DriverComm::SendIO() and recognized as a valid request.  The
//      result of processing the DIOCTL_SYM_INTERNAL_??? request will be
//      placed at *(PDWORD)prDIOCParams->lpvOutBuffer.
//
//      This function returns TRUE with ERROR_DRVCOMM_NOT_INITIALIZED in
//      *(PDWORD)prDIOCParams->lpvOutBuffer if it is called when DriverComm
//      is not initialized.
//
//      This funciton returns FALSE if prDIOCParameters was not formatted
//      properly, as if by DriverComm:SendIO(), or it was not recognized
//      as a valid request.
//
// Synchronization:
//      This is a fully reentrant function.  No synchronization is necessary.
//
//---------------------------------------------------------------------------
BOOL WINAPI DriverCommDispatchDIOC (
    PDIOCPARAMETERS prDIOCParameters
    )
    {
                                        // Validate parameters.

    SYM_ASSERT ( prDIOCParameters );

    SYM_ASSERT ( prDIOCParameters->lpvInBuffer );
    SYM_ASSERT ( prDIOCParameters->cbInBuffer == sizeof(DRVCOMM_DIOC) );

    SYM_ASSERT ( prDIOCParameters->lpvOutBuffer );
    SYM_ASSERT ( prDIOCParameters->cbOutBuffer == sizeof(DWORD) );

    SYM_ASSERT ( prDIOCParameters->lpcbBytesReturned );

    if ( !prDIOCParameters ||
         !prDIOCParameters->lpvInBuffer ||
         prDIOCParameters->cbInBuffer != sizeof(DRVCOMM_DIOC) ||
         !prDIOCParameters->lpvOutBuffer ||
         prDIOCParameters->cbOutBuffer != sizeof(DWORD) ||
         !prDIOCParameters->lpcbBytesReturned
       )
        {
        return ( FALSE );
        }

    auto PDRVCOMM_DIOC prSendIOParams;

    prSendIOParams = (PDRVCOMM_DIOC) prDIOCParameters->lpvInBuffer;

    SYM_ASSERT ( prSendIOParams->dwTag1 == '>>SR' );
    SYM_ASSERT ( prSendIOParams->dwTag2 == '<<SR' );

    if ( prSendIOParams->dwTag1 != '>>SR' ||
         prSendIOParams->dwTag2 != '<<SR' )
        {
        return ( FALSE );
        }

                                        // Process the DeviceIoControl()
                                        // call.

    switch ( prDIOCParameters->dwIoControlCode )
        {
        case DIOCTL_SYM_INTERNAL_VALIDATE:

                                        // Reserved for future use.  We
                                        // should act like we know about it,
                                        // but we don't service this call.

            *(PDWORD)prDIOCParameters->lpvOutBuffer = ERROR_DRVCOMM_NOT_IMPLEMENTED;

            break;

        case DIOCTL_SYM_INTERNAL_SERVICE_REQUEST:

                                        // Service the DriverComm::Send()
                                        // request.

            *(PDWORD)prDIOCParameters->lpvOutBuffer =
                DriverCommServiceRequest ( prSendIOParams );

            break;

        case DIOCTL_SYM_INTERNAL_INIT_SERVICES:

                                        // We are being notified that a
                                        // DriverComm class wants to service
                                        // our requests.  Initialize whatever
                                        // is necessary to support the
                                        // DriverComm service thread.

            *(PDWORD)prDIOCParameters->lpvOutBuffer =
                DriverCommServicesInit();

            break;

        case DIOCTL_SYM_INTERNAL_DEINIT_SERVICES:

                                        // We are being notified that the
                                        // DriverComm class wants to stop
                                        // servicing our requests.

            *(PDWORD)prDIOCParameters->lpvOutBuffer =
                DriverCommServicesDeInit();

            break;

        case DIOCTL_SYM_INTERNAL_RELEASE_SERVICES:

                                        // We are being told by a DriverComm
                                        // class that it needs us to release
                                        // its service thread, if it is
                                        // blocked.

            *(PDWORD)prDIOCParameters->lpvOutBuffer =
                DriverCommServicesRelease();

            break;

        case DIOCTL_SYM_INTERNAL_GET_DRVCOMMDIOCTL_PACKET:

                                        // The service thread of a DriverComm
                                        // class is ready to service our
                                        // requests and is asking us to give
                                        // it whatever we have.

            *(PDWORD)prDIOCParameters->lpvOutBuffer = DriverCommGetPacket (
                                                          prSendIOParams );

            break;

        default:
                                        // Unknown dwIoControlCode.  We
                                        // should fail the DeviceIoControl()
                                        // call.

            return ( FALSE );
        }

                                        // Return to caller, telling it that
                                        // we have copied sizeof(DWORD)
                                        // to it's lpvOutBuffer.  This is
                                        // the DWORD status of processing
                                        // the DeviceIoControl() call.

    *(PDWORD)prDIOCParameters->lpcbBytesReturned = sizeof(DWORD);

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// static DWORD DriverCommServiceRequest (
//      PDRVCOMM_DIOC prSendIOParams
//      );
//
// This routine services a user request sent to us through
// DriverComm::SendIO() with dwIoControlCode ==
// DIOCTL_SYM_INTERNAL_SERVICE_REQUEST.
//
// DriverCommServiceRequest() is called internally when a DriverComm class
// is requesting DIOCTL_SYM_INTERNAL_SERVICE_REQUEST.
//
// Entry:
//      prSendIOParams - parameters passed to DriverComm::SendIO(), which
//                       are directly translated from the parameters passed
//                       to DriverComm:Send().
//
// Exit:
//      This function returns one of the following:
//          ERROR_DRVCOMM_SUCCESS,
//          ERROR_DRVCOMM_NOT_INITIALIZED,
//          ERROR_DRVCOMM_NO_CALLBACK.
//
// Synchronization:
//      This is a fully reentrant function.  No synchronization is necessary.
//
//---------------------------------------------------------------------------
static DWORD DriverCommServiceRequest (
    PDRVCOMM_DIOC prSendIOParams
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( prSendIOParams );
    SYM_VERIFY_BUFFER ( prSendIOParams, sizeof(*prSendIOParams) );

                                        // Increment the number of users of
                                        // DriverComm.  This will prevent
                                        // DriverCommInit() and
                                        // DriverCommDeInit() from changing
                                        // the status.

    SymInterlockedIncrement ( &lDriverCommUsers );

                                        // If DriverComm is not initialized,
                                        // then this is a bogus call.

    if ( !bDriverCommInitialized )
        {
        SymInterlockedDecrement ( &lDriverCommUsers );

        return ( ERROR_DRVCOMM_NOT_INITIALIZED );
        }

                                        // Call the callback, if it has been
                                        // registered.

    if ( pfnCallback )
        {
        pfnCallback ( prSendIOParams->dwServiceCode,
                      (PVOID)prSendIOParams->lpvInBuffer,
                      prSendIOParams->cbInBuffer,
                      (PVOID)prSendIOParams->lpvOutBuffer,
                      prSendIOParams->cbOutBuffer,
                      (PDWORD)prSendIOParams->lpcbBytesReturned );

        SymInterlockedDecrement ( &lDriverCommUsers );

        return ( ERROR_DRVCOMM_SUCCESS );
        }

                                        // DriverCommDeInit() can now change
                                        // the status.

    SymInterlockedDecrement ( &lDriverCommUsers );

    return ( ERROR_DRVCOMM_NO_CALLBACK );
    }

//---------------------------------------------------------------------------
//
// static DWORD DriverCommServicesInit (
//      VOID
//      );
//
// This routine prepares the driver for managing a DriverComm service
// thread.
//
// DriverCommServicesInit() is called internally when a DriverComm class is
// requesting DIOCTL_SYM_INTERNAL_INIT_SERVICES.
//
// Entry:
//      -
//
// Exit:
//      ERROR_DRVCOMM_SUCCESS,
//      ERROR_DRVCOMM_UNSAFE,
//      ERROR_DRVCOMM_NOT_INITIALIZED,
//      ERROR_DRVCOMM_ALREADY_INITIALIZED.
//
// Synchronization:
//      This is a fully reentrant function.  No synchronization is necessary.
//
//---------------------------------------------------------------------------
static DWORD DriverCommServicesInit (
    VOID
    )
    {
                                        // Acquire the API DriverComm lock
                                        // and tell all other functions using
                                        // the same lock to fail.

    if ( !ACQUIRE_DRIVERCOMM_LOCK(API) )
        {
        return ( ERROR_DRVCOMM_UNSAFE );
        }

                                        // If DriverComm is not initialized,
                                        // then this is a bogus call.

    if ( !bDriverCommInitialized )
        {
        RELEASE_DRIVERCOMM_LOCK(API);

        return ( ERROR_DRVCOMM_NOT_INITIALIZED );
        }

                                        // If the services are already
                                        // initialized, then this is a bogus
                                        // call.

    if ( bDriverCommServicesInitialized )
        {
        RELEASE_DRIVERCOMM_LOCK(API);

        return ( ERROR_DRVCOMM_ALREADY_INITIALIZED );
        }

                                        // Make sure that there is no thread
                                        // already waiting for packets.  If
                                        // we can't acquire the QUEUE lock,
                                        // this means that a DriverComm class
                                        // is already asking for packets, so
                                        // we can't continue.

    if ( !ACQUIRE_DRIVERCOMM_LOCK(QUEUE) )
        {
        RELEASE_DRIVERCOMM_LOCK(API);

        return ( ERROR_DRVCOMM_UNSAFE );
        }

                                        // Increment the number of users of
                                        // DriverComm.  This will prevent
                                        // DriverCommDeInit() from changing
                                        // the status until the DriverComm
                                        // class tells us that it doesn't
                                        // want to service our requests
                                        // anymore.

    SymInterlockedIncrement ( &lDriverCommUsers );

                                        // Allow the APIs for releasing
                                        // service request packets to work.

    bDriverCommServicesInitialized = TRUE;

                                        // Release the QUEUE and API
                                        // DriverComm locks.

    RELEASE_DRIVERCOMM_LOCK(QUEUE);

    RELEASE_DRIVERCOMM_LOCK(API);

    return ( ERROR_DRVCOMM_SUCCESS );
    }

//---------------------------------------------------------------------------
//
// static DWORD DriverCommServicesDeInit (
//      VOID
//      );
//
// This routine tells the driver that the DriverComm class which has been
// servicing its requests doesn't want to do so anymore.
//
// DriverCommServicesDeInit() is called internally when a DriverComm class
// is requesting DIOCTL_SYM_INTERNAL_DEINIT_SERVICES.
//
// Entry:
//      -
//
// Exit:
//      ERROR_DRVCOMM_SUCCESS,
//      ERROR_DRVCOMM_UNSAFE,
//      ERROR_DRVCOMM_NOT_INITIALIZED.
//
// Synchronization:
//      This is a fully reentrant function.  No synchronization is necessary.
//
//---------------------------------------------------------------------------
static DWORD DriverCommServicesDeInit (
    VOID
    )
    {
                                        // Acquire the API DriverComm lock
                                        // and tell all other functions using
                                        // the same lock to fail.

    if ( !ACQUIRE_DRIVERCOMM_LOCK(API) )
        {
        return ( ERROR_DRVCOMM_UNSAFE );
        }

                                        // Make sure that there is no thread
                                        // already waiting for packets.  If
                                        // we can't acquire the QUEUE lock,
                                        // this means that a DriverComm class
                                        // is already asking for packets, so
                                        // we can't continue.

    if ( !ACQUIRE_DRIVERCOMM_LOCK(QUEUE) )
        {
        RELEASE_DRIVERCOMM_LOCK(API);

        return ( ERROR_DRVCOMM_UNSAFE );
        }

                                        // Make sure we are not trying to
                                        // uninitialize something that was
                                        // never initialized.

    if ( !bDriverCommServicesInitialized )
        {
        RELEASE_DRIVERCOMM_LOCK(QUEUE);

        RELEASE_DRIVERCOMM_LOCK(API);

        return ( ERROR_DRVCOMM_NOT_INITIALIZED );
        }

                                        // Make sure we don't send any more
                                        // requests to user mode, and also
                                        // DriverComm should not ask for
                                        // packets anymore.

    bDriverCommServicesInitialized = FALSE;

                                        // DriverCommDeInit() can now change
                                        // the status.

    SymInterlockedDecrement ( &lDriverCommUsers );

                                        // Release the QUEUE and API
                                        // DriverComm locks.

    RELEASE_DRIVERCOMM_LOCK(QUEUE);

    RELEASE_DRIVERCOMM_LOCK(API);

    return ( ERROR_DRVCOMM_SUCCESS );
    }

//---------------------------------------------------------------------------
//
// static DWORD DriverCommServicesRelease (
//      VOID
//      );
//
// This routine forces the release of a potentially blocked DriverComm
// service thread.
//
// DriverCommServicesRelease() is called internally when a DriverComm class
// is requesting DIOCTL_SYM_INTERNAL_RELEASE_SERVICES.
//
// Entry:
//      -
//
// Exit:
//      ERROR_DRVCOMM_SUCCESS,
//      ERROR_DRVCOMM_NOT_INITIALIZED.
//
// Synchronization:
//      This is a fully reentrant function.  No synchronization is necessary.
//
//---------------------------------------------------------------------------
#define DRIVER_NTK_LOCKED_DATA_SEGMENT
#include "drvseg.h"

static DWORD DriverCommServicesRelease (
    VOID
    )
    {
                                        // Increment the number of users of
                                        // DriverComm.  This will prevent
                                        // DriverCommInit() and
                                        // DriverCommDeInit() from changing
                                        // the status.

    SymInterlockedIncrement ( &lDriverCommUsers );

                                        // If DriverComm is not initialized,
                                        // then this is a bogus call.

    if ( !bDriverCommInitialized )
        {
        SymInterlockedDecrement ( &lDriverCommUsers );

        return ( ERROR_DRVCOMM_NOT_INITIALIZED );
        }

                                        // Signal semaphore, thus unblocking
                                        // a potentially blocked DriverComm
                                        // service thread.

    SymReleaseSemaphore ( &rServiceThreadSemaphore );

                                        // DriverCommDeInit() can now change
                                        // the status.

    SymInterlockedDecrement ( &lDriverCommUsers );

    return ( ERROR_DRVCOMM_SUCCESS );
    }

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// static DWORD DriverCommGetPacket (
//      PDRVCOMM_DIOC prSendIOParams
//      );
//
// This routine examines the queue for service request packets.  If there
// are no packets in the queue, then this function will block until something
// is added by DriverCommSend() or DriverCommAsyncSend(), or until it is
// signaled to return.
//
// When a packet (DRVCOMMDIOCTLPACKET) becomes available, it will be copied
// to prSendIOParams->lpvOutBuffer, and prSendIOParams->lpcbBytesReturned
// will be updated with the number of bytes copied to the buffer.
//
// Entry:
//      prSendIOParams - Non-NULL pointer to a structure containing the
//                       parameters passed to DriverComm::SendIO().  The
//                       following members of prSendIOParams should contain:
//
//                       dwServiceCode     - 0;
//                       lpvInBuffer       - NULL;
//                       cbInBuffer        - 0;
//                       lpvOutBuffer      - buffer for DRVCOMMDIOCTLPACKET;
//                       cbOutBuffer       - length of lpvOutBuffer;
//                       lpcbBytesReturned - pointer to a DWORD to receive
//                                           the number of bytes copied to
//                                           lpvOutBuffer.
//
// Exit:
//      This function returns one of the following:
//          ERROR_DRVCOMM_SUCCESS,
//          ERROR_DRVCOMM_BAD_ARGUMENT,
//          ERROR_DRVCOMM_UNSAFE,
//          ERROR_DRVCOMM_NOT_INITIALIZED.
//
//      If this function returned ERROR_DRVCOMM_BUFFER_OVERFLOW, the DWORD
//      pointed to by prSendIOParams->lpcbBytesReturned will contain the size
//      of lpvOutBuffer that the next packet needs.  This function will give
//      its caller one chance to increase the length of lpvOutBuffer.  If
//      upon next call to the function the buffer at lpvOutBuffer is still
//      small, the packet will be cancelled.
//
//      If there are no request packets in the queue, and the function is
//      signaled to return the next packet, it will return
//      ERROR_DRVCOMM_SUCCESS, with 0 written at the DWORD pointed to by
//      lpcbBytesReturned.
//
// Synchronization:
//      This is non-reentrant, but it will properly synchronize multiple
//      clients by returning ERROR_DRVCOMM_UNSAFE to anyone that reenters it.
//
//---------------------------------------------------------------------------
#define DRIVER_NTK_LOCKED_DATA_SEGMENT
#include "drvseg.h"

static DWORD DriverCommGetPacket (
    PDRVCOMM_DIOC prSendIOParams
    )
    {
                                        // Only one thread at a time can
                                        // execute this function.

    ENTER_FUNCTION ( ERROR_DRVCOMM_UNSAFE );

                                        // Validate input parameters.

    SYM_ASSERT ( prSendIOParams );
    SYM_VERIFY_BUFFER ( prSendIOParams, sizeof(*prSendIOParams) );

    SYM_ASSERT ( !prSendIOParams->dwServiceCode );
    SYM_ASSERT ( !prSendIOParams->lpvInBuffer );
    SYM_ASSERT ( !prSendIOParams->cbInBuffer );
    SYM_ASSERT ( prSendIOParams->lpvOutBuffer );
    SYM_ASSERT ( prSendIOParams->cbOutBuffer >= sizeof(DRVCOMMDIOCTLPACKET) );
    SYM_ASSERT ( prSendIOParams->lpcbBytesReturned );

    if ( prSendIOParams->dwServiceCode ||
         prSendIOParams->lpvInBuffer ||
         prSendIOParams->cbInBuffer ||
         !prSendIOParams->lpvOutBuffer ||
         prSendIOParams->cbOutBuffer < sizeof(DRVCOMMDIOCTLPACKET) ||
         !prSendIOParams->lpcbBytesReturned
       )
        {
        EXIT_FUNCTION ( ERROR_DRVCOMM_BAD_ARGUMENT );
        }

                                        // Make sure the service thread
                                        // status doesn't change underneath
                                        // us.

    if ( !ACQUIRE_DRIVERCOMM_LOCK(QUEUE) )
        {
        EXIT_FUNCTION ( ERROR_DRVCOMM_UNSAFE );
        }

                                        // Make sure the service thread is
                                        // initialized.

    if ( !bDriverCommServicesInitialized )
        {
        RELEASE_DRIVERCOMM_LOCK(QUEUE);

        EXIT_FUNCTION ( ERROR_DRVCOMM_NOT_INITIALIZED );
        }

                                        // Wait on the semaphore until
                                        // something is inserted in the
                                        // queue, or until the user mode
                                        // thread has to be terminated.

    SymWaitForSemaphore ( &rServiceThreadSemaphore );

                                        // If there are no packets in the
                                        // queue, then we were probably told
                                        // that the DriverComm class wants
                                        // to stop servicing our requests.

    SymWaitForMutex ( &rServiceQueueMutex );

    if ( !prServiceQueueHead )
        {
        bBufferResizeRequest = FALSE;

        SymReleaseMutex ( &rServiceQueueMutex );

        *(PDWORD)prSendIOParams->lpcbBytesReturned = 0;

        RELEASE_DRIVERCOMM_LOCK(QUEUE);

        EXIT_FUNCTION ( ERROR_DRVCOMM_SUCCESS );
        }

                                        // Return the packet at the head of
                                        // the list.

    auto PDRVCOMMDIOCTLPACKET prPacket;

                                        // See if the DriverComm class has
                                        // given us large enough buffer to
                                        // put the packet in.

    if ( prServiceQueueHead->dwSize <= prSendIOParams->cbOutBuffer )
        {
                                        // If yes, then remove the packet
                                        // from the head of the list and
                                        // give it to the DriverComm class.

        SYM_VERIFY_BUFFER ( prSendIOParams->lpvOutBuffer,
                            prServiceQueueHead->dwSize );

        memcpy ( (PVOID)prSendIOParams->lpvOutBuffer,
                 prServiceQueueHead,
                 prServiceQueueHead->dwSize );

        *(PDWORD)prSendIOParams->lpcbBytesReturned = prServiceQueueHead->dwSize;

        prPacket = prServiceQueueHead;

        prServiceQueueHead = prServiceQueueHead->pNext;

                                        // If this is a synchronous packet
                                        // packet, then move it to the
                                        // linked list of synchronous packets
                                        // sent to user mode.

                                        // ~~~TBD~~~ The FALSE constant in
                                        // the if() statement below should
                                        // be replaced with a check returning
                                        // TRUE if packet is asynchronous,
                                        // and FALSE if packet is
                                        // synchronous.

        if ( FALSE )
            {
                                        // Add the packet to the tail of the
                                        // queue.

            prPacket->pNext = NULL;

            if ( prSyncServiceQueueHead )
                {
                                        // List is not empty - make the
                                        // packet the tail of the list.

                SYM_ASSERT ( prSyncServiceQueueTail );
                SYM_ASSERT ( !prSyncServiceQueueTail->pNext );

                prSyncServiceQueueTail->pNext = prPacket;

                prSyncServiceQueueTail = prPacket;
                }
            else
                {
                                        // List is empty - make the packet
                                        // both head and tail of the list.

                prSyncServiceQueueHead = prSyncServiceQueueTail = prPacket;
                }

                                        // We want to keep the packet around
                                        // until it is processed.

            prPacket = NULL;
            }

        bBufferResizeRequest = FALSE;

        SymReleaseMutex ( &rServiceQueueMutex );
        }
    else
        {
        if ( !bBufferResizeRequest )
            {
                                        // If we don't have large enough
                                        // lpvOutBuffer, and we have not
                                        // asked for buffer resize yet, then
                                        // ask for it now.

            bBufferResizeRequest = TRUE;

            *(PDWORD)prSendIOParams->lpcbBytesReturned =
                prServiceQueueHead->dwSize;

            SymReleaseMutex ( &rServiceQueueMutex );

            SymReleaseSemaphore ( &rServiceThreadSemaphore );

            RELEASE_DRIVERCOMM_LOCK(QUEUE);

            EXIT_FUNCTION ( ERROR_DRVCOMM_BUFFER_OVERFLOW );
            }
        else
            {
                                        // If lpvOutBuffer is not large
                                        // enough, and this is the second
                                        // time we are asked to copy the same
                                        // packet to a small buffer, then
                                        // cancel the packet and release it.

            bBufferResizeRequest = FALSE;

            prPacket = prServiceQueueHead;

            prServiceQueueHead = prServiceQueueHead->pNext;

            SymReleaseMutex ( &rServiceQueueMutex );

                                        // ~~~TBD~~~ The TRUE constant in
                                        // the if() statement below should
                                        // be replaced with a check returning
                                        // TRUE if packet is asynchronous,
                                        // and FALSE if packet is
                                        // synchronous.

            if ( TRUE )
                {
                                        // Notify the callback of request
                                        // cancellation.

                if ( pfnCancelCallback )
                    {
                    pfnCancelCallback ( prPacket->dwServiceCode,
                                        prPacket->dwInBufferSize ?
                                            &prPacket->abyData[0] : NULL,
                                        prPacket->dwInBufferSize );
                    }
                }
            else
                {
                                        // ~~~TBD~~~ Release the blocked
                                        // thread telling the client that its
                                        // request can't be serviced because
                                        // the DriverComm class can't provide
                                        // enough memory to receive the
                                        // request packet.

                                        // This packet will be freed up by
                                        // the just released thread.

                prPacket = NULL;
                }
            }
        }

                                        // Free the memory allocated for
                                        // the packet.

    if ( prPacket )
        {
        MemFreePtr ( prPacket );
        }

                                        // Allow uninitialization of service
                                        // thread APIs.

    RELEASE_DRIVERCOMM_LOCK(QUEUE);

    EXIT_FUNCTION ( ERROR_DRVCOMM_SUCCESS );
    }

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// NTSTATUS DriverCommNTDispatchDIOC (
//      IN PDEVICE_OBJECT DeviceObject,
//      IN PIRP Irp
//      );
//
// This is a default NT dispatch routine for IRP_MJ_DEVICE_CONTROL.  NT
// clients of DriverComm, who do not need to do special processing before
// calling DriverCommDispatchDIOC() may directly register this routine to
// handle all IRP_MJ_DEVICE_CONTROL requests.  All the client has to do is:
//
// DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverCommNTDispatchDIOC;
//
// right after successfully creating a device object for use by the driver.
//
// Entry:
//      DeviceObject - look at NT DDK.
//      Irp          - look at NT DDK.
//
// Exit:
//      Returns STATUS_SUCCESS if the DeviceIoControl() call was properly
//      recognized as a valid DriverComm class call, or returns
//      STATUS_NOT_IMPLEMENTED if the call was not recognized as a valid
//      DriverComm class call.
//
// Note:
//      This is an NT specific implementation.  VxD drivers should use
//      DriverCommW95DispatchDIOC().
//      STATUS_NOT_IMPLEMENTED translates into ERROR_INVALID_FUNCTION.
//      STATUS_SUCCESS translates into ERROR_SUCCESS.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)
NTSTATUS DriverCommNTDispatchDIOC (
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
    {
    auto NTSTATUS           Result;
    auto PIO_STACK_LOCATION pIrpStack;
    auto DIOCPARAMETERS     rDIOCParameters;
    auto DWORD              dwBytesReturned;

                                        // Get the IRP stack location, so
                                        // that we can access the DIOC call
                                        // information.

    pIrpStack = IoGetCurrentIrpStackLocation ( Irp );

                                        // Create a DIOC packet, in the
                                        // format used by
                                        // DriverCommDispatchDIOC().  This is
                                        // the same format that is used by
                                        // the VWIN32 VXD in Windows 95.

    rDIOCParameters.dwIoControlCode   = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
    rDIOCParameters.lpvInBuffer       = (DWORD)pIrpStack->Parameters.DeviceIoControl.Type3InputBuffer;
    rDIOCParameters.cbInBuffer        = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
    rDIOCParameters.lpvOutBuffer      = (DWORD)Irp->UserBuffer;
    rDIOCParameters.cbOutBuffer       = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
    rDIOCParameters.lpcbBytesReturned = (DWORD)&dwBytesReturned;

                                        // Dispatch the call and then
                                        // complete the IRP.

    if ( DriverCommDispatchDIOC ( &rDIOCParameters ) )
        {
        Result = STATUS_SUCCESS;
        Irp->IoStatus.Information = dwBytesReturned;
        }
    else
        {
        Result = STATUS_NOT_IMPLEMENTED;
        Irp->IoStatus.Information = 0;
        }

    Irp->IoStatus.Status = Result;

    IoCompleteRequest ( Irp, IO_NO_INCREMENT );

    return ( Result );
    }
#endif // #if defined(SYM_NTK)

//---------------------------------------------------------------------------
//
// DWORD WINAPI DriverCommW95DispatchDIOC (
//      PDIOCPARAMETERS prDIOCParameters
//      );
//
// This is a default Windows 95 dispatch routine for W32_DEVICEIOCONTROL
// message.  VxD clients of DriverComm, who do not need to do special
// processing before calling DriverCommDispatchDIOC() may directly call
// DriverCommW95DispatchDIOC() from VxD's control procedure:
//
//      EXTRN _DriverCommW95DispatchDIOC@4:PROC
//
//      BeginProc  MYDRIVER_Control
//          .
//          .
//          .
//          Control_Dispatch  ...
//          Control_Dispatch  W32_DEVICEIOCONTROL, QuickDIOCHandler
//          Control_Dispatch  ...
//          .
//          .
//          .
//      EndProc  MYDRIVER_Control
//
//      BeginProc  QuickDIOCHandler
//          mov     ebx,[esi].DIOCParams.dwIoControlCode
//          cmp     ebx,DIOC_GETVERSION
//          jne     QuickDIOCHandler_CheckClose
//          mov     ecx,( (MYDRIVER_MAJOR_VER SHL 8) + MYDRIVER_MINOR_VER )
//          jmp     QuickDIOCHandler_Return
//      QuickDIOCHandler_CheckClose:
//          cmp     ebx,DIOC_CLOSEHANDLE
//          je      QuickDIOCHandler_Return
//          cld
//          push    esi
//          call    _DriverCommW95DispatchDIOC@4
//          clc
//          jmp     QuickDIOCHandler_End
//      QuickDIOCHandler_Return:
//          xor     eax,eax
//      QuickDIOCHandler_End:
//          ret
//      EndProc  QuickDIOCHandler
//
// Entry:
//      prDIOCParameters - a structure, filled with data supplied by
//                         the caller of DeviceIoControl()
//
// Exit:
//      Returns ERROR_SUCCESS if the DeviceIoControl() call was properly
//      recognized as a valid DriverComm class call, or returns
//      ERROR_INVALID_FUNCTION if the call was not recognized as a valid
//      DriverComm class call.
//
// Note:
//      This is a Windows 95 specific implementation.  Windows NT drivers
//      should use DriverCommNTDispatchDIOC().
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)
DWORD WINAPI DriverCommW95DispatchDIOC (
    PDIOCPARAMETERS prDIOCParameters
    )
    {
    return ( DriverCommDispatchDIOC ( prDIOCParameters ) ?
                 ERROR_SUCCESS :
                 ERROR_INVALID_FUNCTION
           );
    }
#endif // #if defined(SYM_VXD)
