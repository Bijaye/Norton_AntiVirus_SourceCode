// Copyright 1997 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APCOMM/VCS/APCOMMK.CPv   1.10   02 Jun 1998 01:16:46   RStanev  $
//
// Description:
//      This file defines the kernel mode functions for servicing user mode
//      requests to NAVAP.
//
// Contains:
//      NAVAPServiceCallback()
//      NAVAPCancelCallback()
//
//      EnterEnableDisable()
//      EnterReloadConfig()
//      UserStackReloadConfig()             (local function)
//      EnterLoadAVAPI()
//      UserStackLoadAVAPI()                (local function)
//      EnterUnloadAVAPI()
//      UserStackUnloadAVAPI()              (local function)
//      EnterControlledInit()
//      UserStackControlledInit()           (local function)
//      EnterControlledDeInit()
//      UserStackControlledDeInit()         (local function)
//
//      GetInitializedModulesStatus()       (local function)
//
//      Int2FChainCallback()                (SYM_VXD only)
//
// See Also:
//      apcomm.h
//
// Notes:
//
//***************************************************************************

#include "platform.h"
#include "symsync.h"
#include "navap.h"
#include "apcommi.h"
#include "commstrs.h"

//---------------------------------------------------------------------------
//
// Prototype local functions.  Some of these functions are defined without
// any header right after their corresponding Enter...() functions.
//
//---------------------------------------------------------------------------
VOID UserStackReloadConfig (
    PNAVAP_CONFIG prConfig,
    PBOOL         pbResult
    );

VOID UserStackLoadAVAPI (
    PSTR  pszPath,
    PBOOL pbResult
    );

VOID UserStackUnloadAVAPI (
    PBOOL pbResult
    );

VOID UserStackControlledInit (
    PNAVAP_CONFIG prConfig,
    PBOOL         pbResult
    );

VOID UserStackControlledDeInit (
    PBOOL pbResult
    );

DWORD GetInitializedModulesStatus (
    VOID
    );

//---------------------------------------------------------------------------
//
// bControlledInitComplete together with lControlledInitUsage protect the
// NAVAPControlledInit() and NAVAPControlledDeInit() APIs.
//
//---------------------------------------------------------------------------
BOOL bControlledInitComplete = FALSE;

#define DRIVER_NTK_LOCKED_DATA_SEGMENT
#include "drvseg.h"

LONG lControlledInitUsage = 0;

#define DRIVER_NTK_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// Global variables that keep AP statistics.
//
//---------------------------------------------------------------------------
DWORD dwAPCOMMFilesScannedSinceStartup = 0;
DWORD dwAPCOMMInfectedFilesFoundSinceStartup = 0;
NAVAP_SYSTEMTIME rAPCOMMSystemTimeWhenLastInfectedFileFound = {0};
NAVAP_USERINFOSUBPACKET rAPCOMMUserOfLastInfectedFileFound = {0};
TCHAR szAPCOMMLastFileScanned[SYM_MAX_PATH] = {0};
TCHAR szAPCOMMLastInfectedFileFound[SYM_MAX_PATH] = {0};
TCHAR szAPCOMMLastVirusFound[SIZE_VIRUSNAME + 1] = {0};

//---------------------------------------------------------------------------
//
// VOID NAVAPServiceCallback (
//      DWORD  dwServiceCode,
//      PVOID  pvInBuffer,
//      DWORD  dwInBufferSize,
//      PVOID  pvOutBuffer,
//      DWORD  dwOutBufferSize,
//      PDWORD pdwBytesReturned
//      );
//
// This function services user mode requests sent to NAVAP through
// DriverComm.  This function contains one huge case statement which
// dispatches dwServiceCode.  At the beginning of the code for servicing
// each request, you can find information on the input parameters.
//
// NAVAPServiceCallback() will service the small and easy requests right
// in its body.  Longer requests, that requrie stack swapping, or extra
// synchronization will be serviced by separate functions.
//
// Entry:
//      dwServiceCode    - service request code from APCOMM.H;
//      pvInBuffer       - buffer with data sent to us by user mode caller;
//      dwInBufferSize   - length of pvInBuffer;
//      pvOutBuffer      - buffer where the caller wants us to store result
//                         of processing;
//      dwOutBufferSize  - length of pvOutBuffer;
//      pdwBytesReturned - pointer to a DWORD to receive the number of
//                         bytes we copy to pvOutBuffer before returning.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID NAVAPServiceCallback (
    DWORD  dwServiceCode,
    PVOID  pvInBuffer,
    DWORD  dwInBufferSize,
    PVOID  pvOutBuffer,
    DWORD  dwOutBufferSize,
    PDWORD pdwBytesReturned
    )
    {
    auto PVOID pvCoreInit;

                                        // Assume we return nothing in
                                        // pvOutBuffer.

    if ( pdwBytesReturned )
        {
        *pdwBytesReturned = 0;
        }

                                        // Initialize Core.

    if ( ! ( pvCoreInit = CoreLocalInit() ) )
        {
        UIOutOfMemoryError();

        return;
        }

                                        // Dispatch the request that the
                                        // user mode client is issuing.

    switch ( dwServiceCode )
        {
        case NAVAP_COMM_GETENABLEDDISABLED:

                                        // This request returns the current
                                        // status of NAVAP (enabled or
                                        // disabled).  The actions that are
                                        // taken here are pretty harmless,
                                        // so we will not acquire any locks.

                                        // pvOutBuffer should be pointing to
                                        // a DWORD to receive the BOOL
                                        // status of NAVAP;
                                        // pdwBytesReturned is mandatory.

            SYM_ASSERT ( pvOutBuffer );
            SYM_ASSERT ( dwOutBufferSize == sizeof(DWORD) );
            SYM_ASSERT ( pdwBytesReturned );

            if ( pvOutBuffer &&
                 dwOutBufferSize == sizeof(DWORD) &&
                 pdwBytesReturned
               )
                {
                *(PDWORD)pvOutBuffer = (DWORD)bNAVAPIsEnabled;

                *pdwBytesReturned = sizeof(DWORD);
                }

            break;

        case NAVAP_COMM_SETENABLEDDISABLED:

                                        // This request is used to set new
                                        // status of NAVAP (enabled or
                                        // disabled).  A separate function
                                        // will service this request, since
                                        // appropriate locks need to be
                                        // acquired, and cleanup must be
                                        // done prior to enabling/disabling.

                                        // pvInBuffer should be pointing to
                                        // a DWORD containing the new BOOL
                                        // status of NAVAP.
                                        // dwInBufferSize == sizeof(DWORD)

            SYM_ASSERT ( pvInBuffer );
            SYM_ASSERT ( dwInBufferSize == sizeof(DWORD) );

            if ( pvInBuffer &&
                 dwInBufferSize == sizeof(DWORD) )
                {
                EnterEnableDisable ( (BOOL)*(PDWORD)pvInBuffer );
                }

            break;

        case NAVAP_COMM_PROTECTPROCESS:

                                        // This request tells NAVAP to start
                                        // working under the context of the
                                        // process which issued the request.
                                        // The modules that are used to
                                        // service this request will always
                                        // be initialized (they are the
                                        // "base" of DriverCom), so we don't
                                        // need to add any syncrhonization.

            DeleteCurrentProcessFromPEL2();

            break;

        case NAVAP_COMM_UNPROTECTPROCESS:

                                        // This request tells NAVAP to stop
                                        // working under the context of the
                                        // process which issued the request.
                                        // The modules that are used to
                                        // service this request will always
                                        // be initialized (they are the
                                        // "base" of DriverCom), so we don't
                                        // need to add any syncrhonization.

            AddCurrentProcessToPEL2();

            break;

        case NAVAP_COMM_RELOADCONFIG:

                                        // This request tells NAVAP to
                                        // reload its configuration.  A
                                        // separate function will service
                                        // this request, since appropriate
                                        // synchronization locks need to be
                                        // acquired.

                                        // pvOutBuffer should be pointing
                                        // to a DWORD to receive the status
                                        // of the operation;
                                        // pdwBytesReturned is mandatory.

            SYM_ASSERT ( pvOutBuffer );
            SYM_ASSERT ( dwOutBufferSize == sizeof(DWORD) );
            SYM_ASSERT ( pdwBytesReturned );

            if ( pvOutBuffer &&
                 dwOutBufferSize == sizeof(DWORD) &&
                 pdwBytesReturned
               )
                {
                *pdwBytesReturned = sizeof(DWORD);

                *(PDWORD)pvOutBuffer = EnterReloadConfig(NULL);
                }

            break;

        case NAVAP_COMM_RELOADCONFIGEX:

                                        // This request tells NAVAP to
                                        // reload its configuration, giving
                                        // it its configuration.  A separate
                                        // function will service this
                                        // request, since appropriate
                                        // synchronization locks need to be
                                        // acquired.

                                        // pvInBuffer should be pointing to
                                        // a NAVAP_CONFIG structure;
                                        // pvOutBuffer should be pointing
                                        // to a DWORD to receive the status
                                        // of the operation;
                                        // pdwBytesReturned is mandatory.

            SYM_ASSERT ( pvInBuffer );
            SYM_ASSERT ( dwInBufferSize );
            SYM_ASSERT ( pvOutBuffer );
            SYM_ASSERT ( dwOutBufferSize == sizeof(DWORD) );
            SYM_ASSERT ( pdwBytesReturned );

            if ( pvOutBuffer &&
                 dwOutBufferSize == sizeof(DWORD) &&
                 pdwBytesReturned
               )
                {
                *pdwBytesReturned = sizeof(DWORD);

                *(PDWORD)pvOutBuffer = EnterReloadConfig((PNAVAP_CONFIG)pvInBuffer);
                }

            break;

        case NAVAP_COMM_LOADAVAPI:

                                        // This request tells NAVAP to
                                        // load the virus definitions.
                                        // A separate function will service
                                        // this request, since appropriate
                                        // synchronization locks need to be
                                        // acquired.

                                        // pvOutBuffer should be pointing
                                        // to a DWORD to receive the status
                                        // of the operation;
                                        // pdwBytesReturned is mandatory.

            SYM_ASSERT ( pvOutBuffer );
            SYM_ASSERT ( dwOutBufferSize == sizeof(DWORD) );
            SYM_ASSERT ( pdwBytesReturned );

            if ( pvOutBuffer &&
                 dwOutBufferSize == sizeof(DWORD) &&
                 pdwBytesReturned
               )
                {
                *pdwBytesReturned = sizeof(DWORD);

                *(PDWORD)pvOutBuffer = EnterLoadAVAPI ( NULL );
                }

            break;

        case NAVAP_COMM_LOADAVAPIEX:

                                        // This request tells NAVAP to
                                        // load the virus definitions.
                                        // A separate function will service
                                        // this request, since appropriate
                                        // synchronization locks need to be
                                        // acquired.

                                        // pvInBuffer should point to an OEM
                                        // string containing the location of
                                        // the virus defs.
                                        // pvOutBuffer should be pointing
                                        // to a DWORD to receive the status
                                        // of the operation;
                                        // pdwBytesReturned is mandatory.

            SYM_ASSERT ( pvInBuffer );
            SYM_ASSERT ( dwInBufferSize );
            SYM_ASSERT ( pvOutBuffer );
            SYM_ASSERT ( dwOutBufferSize == sizeof(DWORD) );
            SYM_ASSERT ( pdwBytesReturned );

            if ( pvOutBuffer &&
                 dwOutBufferSize == sizeof(DWORD) &&
                 pdwBytesReturned
               )
                {
                *pdwBytesReturned = sizeof(DWORD);

                *(PDWORD)pvOutBuffer = EnterLoadAVAPI ( (PSTR)pvInBuffer );
                }

            break;

        case NAVAP_COMM_UNLOADAVAPI:

                                        // This request tells NAVAP to
                                        // unload the virus definitions.
                                        // A separate function will service
                                        // this request, since appropriate
                                        // synchronization locks need to be
                                        // acquired.

                                        // pvOutBuffer should be pointing
                                        // to a DWORD to receive the status
                                        // of the operation;
                                        // pdwBytesReturned is mandatory.

            SYM_ASSERT ( pvOutBuffer );
            SYM_ASSERT ( dwOutBufferSize == sizeof(DWORD) );
            SYM_ASSERT ( pdwBytesReturned );

            if ( pvOutBuffer &&
                 dwOutBufferSize == sizeof(DWORD) &&
                 pdwBytesReturned
               )
                {
                *pdwBytesReturned = sizeof(DWORD);

                *(PDWORD)pvOutBuffer = EnterUnloadAVAPI();;
                }

            break;

        case NAVAP_COMM_GETSTATUS:

                                        // This request returns information
                                        // about NAVAP.

                                        // pvOutBuffer should be pointing
                                        // to memory for NAVAP_STATUSPACKET
                                        // to receive information about the
                                        // driver.
                                        // pdwBytesReturned is mandatory.
                                        // If pvOutBuffer is not large for
                                        // szStringArray[], dwErrorCode will
                                        // be set to ERROR_NOT_ENOUGH_MEMORY
                                        // and dwSize will contain the size
                                        // of buffer needed.

            SYM_ASSERT ( pvOutBuffer );
            SYM_ASSERT ( dwOutBufferSize >= sizeof(NAVAP_STATUSPACKET) );
            SYM_ASSERT ( pdwBytesReturned );

            if ( pvOutBuffer &&
                 dwOutBufferSize >= sizeof(NAVAP_STATUSPACKET) &&
                 pdwBytesReturned
               )
                {
                                        // Validate buffer.

                SYM_VERIFY_BUFFER ( pvOutBuffer, dwOutBufferSize );

                                        // Initialize buffer.  Note, that
                                        // currently we will clear only the
                                        // memory that we are going to use.

                memset ( pvOutBuffer, 0, sizeof(NAVAP_STATUSPACKET) );

                                        // Calculate the size of the returned
                                        // packet.

                SymAcquireFastGlobalLock();

                auto DWORD dwStringLengths = sizeof(WCHAR) *
                                             ( STRLEN ( szAPCOMMLastFileScanned ) +
                                               STRLEN ( szAPCOMMLastInfectedFileFound ) +
                                               STRLEN ( szAPCOMMLastVirusFound ) + 3 );

                                        // Set the size of the returned
                                        // packet.

                ((PNAVAP_STATUSPACKET)pvOutBuffer)->dwSize = sizeof(NAVAP_STATUSPACKET) + dwStringLengths;

                *pdwBytesReturned = sizeof(NAVAP_STATUSPACKET);

                                        // Continue if the output buffer is
                                        // large enough.

                if ( dwOutBufferSize >= ((PNAVAP_STATUSPACKET)pvOutBuffer)->dwSize )
                    {
                    *pdwBytesReturned = ((PNAVAP_STATUSPACKET)pvOutBuffer)->dwSize;

                                        // Ignore rSystemTimeAtStartup.

                                        // Set dwDriverImageUnloadable.

                    ((PNAVAP_STATUSPACKET)pvOutBuffer)->dwDriverImageUnloadable =
#if defined(SYM_VXD)
                        bVxDIsDynaloaded;
#else
                        TRUE;
#endif

                                        // Set dwInitializedModules.

                    ((PNAVAP_STATUSPACKET)pvOutBuffer)->dwInitializedModules =
                        GetInitializedModulesStatus();

                                        // Set dwFilesScannedSinceStartup.

                    ((PNAVAP_STATUSPACKET)pvOutBuffer)->dwFilesScannedSinceStartup = dwAPCOMMFilesScannedSinceStartup;

                                        // Set dwInfectedFilesFoundSinceStartup.

                    ((PNAVAP_STATUSPACKET)pvOutBuffer)->dwInfectedFilesFoundSinceStartup = dwAPCOMMInfectedFilesFoundSinceStartup;

                                        // Set rSystemTimeWhenLastInfectedFileFound.

                    memcpy ( &((PNAVAP_STATUSPACKET)pvOutBuffer)->rSystemTimeWhenLastInfectedFileFound, &rAPCOMMSystemTimeWhenLastInfectedFileFound, sizeof(((PNAVAP_STATUSPACKET)pvOutBuffer)->rSystemTimeWhenLastInfectedFileFound) );

                                        // Set rUserOfLastInfectedFileFound.

                    memcpy ( &((PNAVAP_STATUSPACKET)pvOutBuffer)->rUserOfLastInfectedFileFound, &rAPCOMMUserOfLastInfectedFileFound, sizeof(((PNAVAP_STATUSPACKET)pvOutBuffer)->rUserOfLastInfectedFileFound) );

                                        // Copy the file name and set dwLastFileScannedIndex.

                    auto int iCurrent = 0;

#if defined(UNICODE)

                    wcscpy ( &((PNAVAP_STATUSPACKET)pvOutBuffer)->szStringArray[iCurrent], szAPCOMMLastFileScanned );

#else

                    if ( MultiByteToWideChar ( CP_OEMCP,
                                               0,
                                               szAPCOMMLastFileScanned,
                                               -1,
                                               &((PNAVAP_STATUSPACKET)pvOutBuffer)->szStringArray[iCurrent],
                                               strlen(szAPCOMMLastFileScanned) + 1 ) == 0 )
                        ((PNAVAP_STATUSPACKET)pvOutBuffer)->szStringArray[iCurrent] = 0;

#endif

                    ((PNAVAP_STATUSPACKET)pvOutBuffer)->dwLastFileScannedIndex = iCurrent;

                    iCurrent += wcslen ( &((PNAVAP_STATUSPACKET)pvOutBuffer)->szStringArray[iCurrent] ) + 1;

                                        // Copy the file name and set dwLastInfectedFileFoundIndex.

#if defined(UNICODE)

                    wcscpy ( &((PNAVAP_STATUSPACKET)pvOutBuffer)->szStringArray[iCurrent], szAPCOMMLastInfectedFileFound );

#else

                    if ( MultiByteToWideChar ( CP_OEMCP,
                                               0,
                                               szAPCOMMLastInfectedFileFound,
                                               -1,
                                               &((PNAVAP_STATUSPACKET)pvOutBuffer)->szStringArray[iCurrent],
                                               strlen(szAPCOMMLastInfectedFileFound) + 1 ) == 0 )
                        ((PNAVAP_STATUSPACKET)pvOutBuffer)->szStringArray[iCurrent] = 0;

#endif

                    ((PNAVAP_STATUSPACKET)pvOutBuffer)->dwLastInfectedFileFoundIndex = iCurrent;

                    iCurrent += wcslen ( &((PNAVAP_STATUSPACKET)pvOutBuffer)->szStringArray[iCurrent] ) + 1;

                                        // Copy the virus name and set dwLastVirusFoundIndex.

#if defined(UNICODE)

                    wcscpy ( &((PNAVAP_STATUSPACKET)pvOutBuffer)->szStringArray[iCurrent], szAPCOMMLastVirusFound );

#else

                    if ( MultiByteToWideChar ( CP_OEMCP,
                                               0,
                                               szAPCOMMLastVirusFound,
                                               -1,
                                               &((PNAVAP_STATUSPACKET)pvOutBuffer)->szStringArray[iCurrent],
                                               strlen(szAPCOMMLastVirusFound) + 1 ) == 0 )
                        ((PNAVAP_STATUSPACKET)pvOutBuffer)->szStringArray[iCurrent] = 0;

#endif

                    ((PNAVAP_STATUSPACKET)pvOutBuffer)->dwLastVirusFoundIndex = iCurrent;

                                        // Set return code.

                    ((PNAVAP_STATUSPACKET)pvOutBuffer)->dwErrorStatus = ERROR_SUCCESS;
                    }
                else
                    ((PNAVAP_STATUSPACKET)pvOutBuffer)->dwErrorStatus = ERROR_NOT_ENOUGH_MEMORY;

                SymReleaseFastGlobalLock();
                }

            break;

        case NAVAP_COMM_CONTROLLEDINIT:

                                        // This request tells NAVAP to
                                        // complete its initialization (if
                                        // it hasn't yet).  The function
                                        // EnterControlledInit() will
                                        // service the request, and its
                                        // header has full information on
                                        // the functionality and the return
                                        // values.

                                        // pvOutBuffer should be pointing
                                        // to a DWORD to receive the status
                                        // of the operation;
                                        // pdwBytesReturned is mandatory.

            SYM_ASSERT ( pvOutBuffer );
            SYM_ASSERT ( dwOutBufferSize == sizeof(DWORD) );
            SYM_ASSERT ( pdwBytesReturned );

            if ( pvOutBuffer &&
                 dwOutBufferSize == sizeof(DWORD) &&
                 pdwBytesReturned
               )
                {
                *pdwBytesReturned = sizeof(DWORD);

                *(PDWORD)pvOutBuffer = EnterControlledInit(NULL);
                }

            break;

        case NAVAP_COMM_CONTROLLEDINITEX:

                                        // This request tells NAVAP to
                                        // complete its initialization (if
                                        // it hasn't yet).  The function
                                        // EnterControlledInit() will
                                        // service the request, and its
                                        // header has full information on
                                        // the functionality and the return
                                        // values.

                                        // pvInBuffer should be pointing to
                                        // a NAVAP_CONFIG structure;
                                        // pvOutBuffer should be pointing
                                        // to a DWORD to receive the status
                                        // of the operation;
                                        // pdwBytesReturned is mandatory.

            SYM_ASSERT ( pvInBuffer );
            SYM_ASSERT ( dwInBufferSize );
            SYM_ASSERT ( pvOutBuffer );
            SYM_ASSERT ( dwOutBufferSize == sizeof(DWORD) );
            SYM_ASSERT ( pdwBytesReturned );

            if ( pvInBuffer &&
                 pvOutBuffer &&
                 dwOutBufferSize == sizeof(DWORD) &&
                 pdwBytesReturned
               )
                {
                *pdwBytesReturned = sizeof(DWORD);

                *(PDWORD)pvOutBuffer = EnterControlledInit((PNAVAP_CONFIG)pvInBuffer);
                }

            break;

        case NAVAP_COMM_CONTROLLEDDEINIT:

                                        // This request tells NAVAP to
                                        // undo its controlled
                                        // initialization.  The function
                                        // EnterControlledDeInit() will
                                        // service the request, and its
                                        // header has full information on
                                        // the functionality and the return
                                        // values.

                                        // pvOutBuffer should be pointing
                                        // to a DWORD to receive the status
                                        // of the operation;
                                        // pdwBytesReturned is mandatory.

            SYM_ASSERT ( pvOutBuffer );
            SYM_ASSERT ( dwOutBufferSize == sizeof(DWORD) );
            SYM_ASSERT ( pdwBytesReturned );

            if ( pvOutBuffer &&
                 dwOutBufferSize == sizeof(DWORD) &&
                 pdwBytesReturned
               )
                {
                *pdwBytesReturned = sizeof(DWORD);

                *(PDWORD)pvOutBuffer = EnterControlledDeInit();
                }

            break;

        case NAVAP_COMM_WIN95AGENTINIT:

                                        // This request tells NAVAP that
                                        // the Win95 agent is starting.  Upon
                                        // this request NAVAP will force the
                                        // event manager to process the next
                                        // reboot event.

            bRebootEventProcessed = FALSE;

            break;

        case NAVAP_COMM_WIN95AGENTDEINIT:

                                        // This request tells NAVAP that
                                        // the Win95 agent is terminating.
                                        // Upon this request, NAVAP will
                                        // simulate a reboot event, which in
                                        // turn will force a scan of the
                                        // floppy drives.  This allows NAVAP
                                        // to scan the floppy drives while
                                        // we still are safer at switching to
                                        // message mode.

#if defined(SYM_VXD)

                                        // Simulate what SYMEvent would have
                                        // done if it detected reboot.

            auto ED rEventData;

            rEventData.SE_ED_etEventType = SE_MSC_REBOOT;

            ECBReboot ( NULL, &rEventData );

#endif

            break;

        default:

                                        // Unknown request code.

            SYM_ASSERT ( FALSE );
        }

                                        // Deinitialize Core.

    CoreLocalDeInit ( pvCoreInit );
    }

//---------------------------------------------------------------------------
//
// VOID NAVAPCancelCallback (
//      DWORD dwServiceCode,
//      PVOID pvInBuffer,
//      DWORD dwInBufferSize
//      );
//
// This function receives all cancelled asynchronous packets.
// NAVAPCancelCallback() filters out all but NAVAP_COMM_EVENTPROGRESSREPORT
// packets, and writes them to a spool file.  The location of the spool file
// is part of the packet.  The spool file is an array of DWORDs, each
// followed by NAVAP_COMM_EVENTPROGRESSREPORT structure.  The DWORD contains
// the dwInBufferSize parameter given to NAVAPCancelCallback(), and the
// structure contains the buffer pointed to by pvInBuffer.
//
// All cancelled packets are the ones that NAVAP has sent earlier to the
// user mode application for servicing specific driver requests.  It is most
// likely, that NAVAP started unloading before all outstanding packets were
// serviced by our application.  Writing them to a spool file will allow the
// application to pick them up whenever is convenient and extract all useful
// information.
//
// Entry:
//      dwServiceCode  - service request code sent by NAVAP to the
//                       application;
//      pvInBuffer     - buffer with data sent to the application by
//                       NAVAP;
//      dwInBufferSize - length of pvInBuffer.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID NAVAPCancelCallback (
    DWORD dwServiceCode,
    PVOID pvInBuffer,
    DWORD dwInBufferSize
    )
    {
    auto PWSTR pszSpoolFile;
    auto HFILE hFile;
    auto LONG  lLength;

    #define prPacket ((PNAVAP_EVENTPROGRESSREPORTPACKET)pvInBuffer)

                                        // We currently write only event
                                        // progress report packets to the
                                        // spool file.

    if ( dwServiceCode == NAVAP_COMM_EVENTPROGRESSREPORT )
        {
                                        // Validate input parameters.

        SYM_ASSERT ( pvInBuffer );
        SYM_ASSERT ( dwInBufferSize );
        SYM_VERIFY_STRING ( &prPacket->szStringArray[prPacket->dwSpoolFilePathIndex] );

                                        // Allocate memory for and build
                                        // the name of the spool file.

        if ( pszSpoolFile = (PWSTR) MemAllocPtr (
                 GHND,
                 sizeof(WCHAR) *
                 ( wcslen ( &prPacket->szStringArray[prPacket->dwSpoolFilePathIndex] ) +
                   wcslen ( L"\\" ) +
                   wcslen ( szSpoolFileName ) +
                   1 ) )
           )
            {
            wcscpy ( pszSpoolFile,
                     &prPacket->szStringArray[prPacket->dwSpoolFilePathIndex] );

            if ( wcslen(pszSpoolFile) &&
                 pszSpoolFile[wcslen(pszSpoolFile) - 1] != L'\\' )
                {
                pszSpoolFile[wcslen(pszSpoolFile)] = L'\\';
                }

            wcscat ( pszSpoolFile, szSpoolFileName );

                                        // Open the spool file.  Sorry about
                                        // the mess, but I have to handle
                                        // a lot of character sets in a lot
                                        // of platforms.

#if defined(UNICODE)

            #define pszOSSpoolFile pszSpoolFile

#elif !defined(SYM_VXD)

            #error No pszOSSpoolFile for current platform.

#else

            auto PTSTR pszOSSpoolFile;

            pszOSSpoolFile = (PTSTR) MemAllocPtr ( GHND,
                                                   sizeof(WCHAR) *
                                                   ( wcslen ( pszSpoolFile ) + 1 ) );

            if ( pszOSSpoolFile )
                {
                VxDGenericStringConvert ( pszSpoolFile, CHARSET_UNICODE,
                                          pszOSSpoolFile, CHARSET_CURRENT );

#endif

                if ( ( hFile = FileOpen ( pszOSSpoolFile,
                                          OF_WRITE | OF_SHARE_EXCLUSIVE ) ) !=
                     HFILE_ERROR ||

                     ( hFile = FileCreate ( pszOSSpoolFile,
                                            FILE_ATTRIBUTE_NORMAL ) ) !=
                     HFILE_ERROR
                   )
                    {
                                        // Get length of file before we start
                                        // writing.

                    if ( ( lLength = FileLength ( hFile ) ) != HFILE_ERROR )
                        {
                                        // Attempt to write to file.

                        if ( FileWrite ( hFile,
                                         &dwInBufferSize,
                                         sizeof(dwInBufferSize) ) !=
                             sizeof(dwInBufferSize) ||

                             FileWrite ( hFile,
                                         pvInBuffer,
                                         dwInBufferSize ) !=
                             dwInBufferSize
                           )
                            {
                                        // If we failed to write, truncate
                                        // the file to its original length.

                            if ( FileSeek ( hFile, lLength, SEEK_SET ) !=
                                 lLength )
                                {
                                FileWrite ( hFile, pvInBuffer, 0 );
                                }
                            }
                        }
                    else
                        {
                        SYM_ASSERT ( FALSE );
                        }

                    FileClose ( hFile );
                    }
                else
                    {
                    SYM_ASSERT ( FALSE );
                    }


#if defined(SYM_VXD)

                MemFreePtr ( pszOSSpoolFile );
                }
            else
                {
                UIOutOfMemoryError();
                }
#endif

            MemFreePtr ( pszSpoolFile );
            }
        else
            {
            UIOutOfMemoryError();
            }
        }

    #undef prPacket
    }

//---------------------------------------------------------------------------
//
// DWORD EnterEnableDisable (
//      BOOL bNewStatus
//      );
//
// This function enables or disables NAVAP, depending on the status of
// bNewStatus.  Change in NAVAP's status will be allowed only when
// NAVAP's controlled initialization has completed successfully.
//
// Entry:
//      bNewStatus - TRUE if NAVAP is to be enabled, FALSE if NAVAP is to
//                   be disabled.
//
// Exit:
//      ERROR_SUCCESS if NAVAP was fully initialized, and changing of the
//      status was done (because it made sense), or ERROR_NOT_READY if
//      NAVAP is not initialized and we shouldn't be messin' with it.
//
// Syncrhonization:
//      This function is non-reentrant, but is currently lacking protective
//      synchronization.  For now, we will rely on our clients to protect
//      us from reentrancy problems.
//
//---------------------------------------------------------------------------
DWORD EnterEnableDisable (
    BOOL bNewStatus
    )
    {
                                        // Make sure that
                                        // bControlledInitComplete stays
                                        // the same as we change NAVAP's
                                        // status.

    SymAcquireFastGlobalLock();

    if ( bControlledInitComplete )
        {
                                        // Set the status, but make sure we
                                        // get only our own TRUE or FALSE,
                                        // not what was given to us by user
                                        // mode.

        bNAVAPIsEnabled = bNewStatus ? TRUE : FALSE;

                                        // Unlock bControlledInitComplete.

        SymReleaseFastGlobalLock();

                                        // We should flush the cache if we
                                        // are being disabled.

        if ( bNewStatus )
            {
            FlushCleanCache();
            }

                                        // We have successfully completed
                                        // the function.

        return ( ERROR_SUCCESS );
        }

                                        // Unlock bControlledInitComplete.

    SymReleaseFastGlobalLock();

                                        // Return error.

    return ( ERROR_NOT_READY );
    }

//---------------------------------------------------------------------------
//
// DWORD EnterReloadConfig (
//      PNAVAP_CONFIG prConfig
//      );
//
// This function reloads the NAVAP configuration.  EnterReloadConfig()
// should be called with only a local instance of Core initialized; it will
// allocate large stack on its own.
//
// Entry:
//      prConfig - Pointer to NAVAP_CONFIG structure.
//
// Exit:
//      ERROR_SUCCESS if NAVAP's configuration was fully reloaded,
//      ERROR_NOT_READY if NAVAP is not initialized,
//      ERROR_INVALID_FUNCTION if NAVAP failed to reload the configuration,
//      ERROR_NOT_ENOUGH_MEMORY if memory allocation failed.
//
// Syncrhonization:
//      This function is non-reentrant, but is currently lacking protective
//      synchronization.  For now, we will rely on our clients to protect
//      us from reentrancy problems.
//
//---------------------------------------------------------------------------
DWORD EnterReloadConfig (
    PNAVAP_CONFIG prConfig
    )
    {
    SYM_ASSERT ( prConfig );

    auto BOOL bResult;

                                        // Before entering this function,
                                        // make sure that we are initialized
                                        // and EnterControlledInit()
                                        // doesn't mess something up.  When
                                        // this check is done, an usage
                                        // count will be updated.

    SymAcquireFastGlobalLock();

    if ( !bControlledInitComplete )
        {
        SymReleaseFastGlobalLock();

        return ( ERROR_NOT_READY );
        }

    SymInterlockedIncrement ( &lControlledInitUsage );

    SymReleaseFastGlobalLock();

                                        // Make sure that any events we
                                        // generate from now on are ignored
                                        // by NAVAP.

    if ( AddCurrentProcessToPEL2() )
        {
                                        // Swap stack.

        if ( CallOnNAVAPStack ( UserStackReloadConfig,
                                prConfig,
                                &bResult ) )
            {
                                        // Enable NAVAP.

            DeleteCurrentProcessFromPEL2();

                                        // Update the usage count.

            SymInterlockedDecrement ( &lControlledInitUsage );

                                        // Return the result of processing.

            return ( bResult ? ERROR_SUCCESS : ERROR_INVALID_FUNCTION );
            }

        DeleteCurrentProcessFromPEL2();
        }
    else
        {
                                        // Report out of memory error.

        UIOutOfMemoryError();
        }

                                        // Update the usage count.

    SymInterlockedDecrement ( &lControlledInitUsage );

                                        // Return error.

    return ( ERROR_NOT_ENOUGH_MEMORY );
    }

VOID UserStackReloadConfig (
    PNAVAP_CONFIG prConfig,
    PBOOL         pbResult
    )
    {
                                        // Reload the configuration.

    *pbResult = ReloadNAVAPConfiguration ( prConfig );

                                        // We should flush the cache.

    FlushCleanCache();
    }

//---------------------------------------------------------------------------
//
// DWORD EnterLoadAVAPI (
//      PSTR pszPath
//      );
//
// This function loads the NAVAP scan configuration.  EnterLoadAVAPI()
// should be called with only a local instance of Core initialized; it will
// allocate large stack on its own.
//
// Entry:
//      pszPath - Pointer to virus DB.
//
// Exit:
//      ERROR_SUCCESS if NAVAP's scan configuration was loaded,
//      ERROR_NOT_READY if NAVAP is not initialized,
//      ERROR_INVALID_FUNCTION if NAVAP failed to load the configuration,
//      ERROR_NOT_ENOUGH_MEMORY if memory allocation failed.
//
// Syncrhonization:
//      This function is non-reentrant, but is currently lacking protective
//      synchronization.  For now, we will rely on our clients to protect
//      us from reentrancy problems.
//
//---------------------------------------------------------------------------
DWORD EnterLoadAVAPI (
    PSTR pszPath
    )
    {
    auto BOOL bResult;

                                        // Before entering this function,
                                        // make sure that we are initialized
                                        // and EnterControlledInit()
                                        // doesn't mess something up.  When
                                        // this check is done, an usage
                                        // count will be updated.

    SymAcquireFastGlobalLock();

    if ( !bControlledInitComplete )
        {
        SymReleaseFastGlobalLock();

        return ( ERROR_NOT_READY );
        }

    SymInterlockedIncrement ( &lControlledInitUsage );

    SymReleaseFastGlobalLock();

                                        // Make sure that any events we
                                        // generate from now on are ignored
                                        // by NAVAP.

    if ( AddCurrentProcessToPEL2() )
        {
                                        // Swap stack.

        if ( CallOnNAVAPStack ( UserStackLoadAVAPI,
                                pszPath,
                                &bResult ) )
            {
                                        // Enable NAVAP.

            DeleteCurrentProcessFromPEL2();

                                        // Update the usage count.

            SymInterlockedDecrement ( &lControlledInitUsage );

                                        // Return the result of processing.

            return ( bResult ? ERROR_SUCCESS : ERROR_INVALID_FUNCTION );
            }

        DeleteCurrentProcessFromPEL2();
        }
    else
        {
                                        // Report out of memory error.

        UIOutOfMemoryError();
        }

                                        // Update the usage count.

    SymInterlockedDecrement ( &lControlledInitUsage );

                                        // Return error.

    return ( ERROR_NOT_ENOUGH_MEMORY );
    }

VOID UserStackLoadAVAPI (
    PSTR  pszPath,
    PBOOL pbResult
    )
    {
                                        // Load the scan configuration.  No
                                        // need to flush the clean cache.

    *pbResult = LoadNAVAPScanConfiguration ( pszPath );
    }

//---------------------------------------------------------------------------
//
// DWORD EnterUnloadAVAPI (
//      VOID
//      );
//
// This function unloads the NAVAP scan configuration.  EnterUnloadAVAPI()
// should be called with only a local instance of Core initialized; it will
// allocate large stack on its own.
//
// Entry:
//      -
//
// Exit:
//      ERROR_SUCCESS if NAVAP's scan configuration was unloaded,
//      ERROR_NOT_READY if NAVAP is not initialized,
//      ERROR_INVALID_FUNCTION if NAVAP failed to unload the configuration,
//      ERROR_NOT_ENOUGH_MEMORY if memory allocation failed.
//
// Syncrhonization:
//      This function is non-reentrant, but is currently lacking protective
//      synchronization.  For now, we will rely on our clients to protect
//      us from reentrancy problems.
//
//---------------------------------------------------------------------------
DWORD EnterUnloadAVAPI (
    VOID
    )
    {
    auto BOOL bResult;

                                        // Before entering this function,
                                        // make sure that we are initialized
                                        // and EnterControlledInit()
                                        // doesn't mess something up.  When
                                        // this check is done, an usage
                                        // count will be updated.

    SymAcquireFastGlobalLock();

    if ( !bControlledInitComplete )
        {
        SymReleaseFastGlobalLock();

        return ( ERROR_NOT_READY );
        }

    SymInterlockedIncrement ( &lControlledInitUsage );

    SymReleaseFastGlobalLock();

                                        // Make sure that any events we
                                        // generate from now on are ignored
                                        // by NAVAP.

    if ( AddCurrentProcessToPEL2() )
        {
                                        // Swap stack.

        if ( CallOnNAVAPStack ( UserStackUnloadAVAPI,
                                &bResult ) )
            {
                                        // Enable NAVAP.

            DeleteCurrentProcessFromPEL2();

                                        // We should flush the cache.

            FlushCleanCache();

                                        // Update the usage count.

            SymInterlockedDecrement ( &lControlledInitUsage );

                                        // Return the result of processing.

            return ( bResult ? ERROR_SUCCESS : ERROR_INVALID_FUNCTION );
            }

        DeleteCurrentProcessFromPEL2();
        }
    else
        {
                                        // Report out of memory error.

        UIOutOfMemoryError();
        }

                                        // Update the usage count.

    SymInterlockedDecrement ( &lControlledInitUsage );

                                        // Return error.

    return ( ERROR_NOT_ENOUGH_MEMORY );
    }

VOID UserStackUnloadAVAPI (
    PBOOL pbResult
    )
    {
                                        // Unload the scan configuration.

    *pbResult = UnloadNAVAPScanConfiguration();
    }

//---------------------------------------------------------------------------
//
// DWORD EnterControlledInit (
//      PNAVAP_CONFIG prConfig
//      );
//
// This function performs controlled initialization of NAVAP.
// EnterControlledInit() should be called with only a local instance of Core
// initialized; it will allocate large stack on its own.
//
// Entry:
//      prConfig - Pointer to NAVAP configuration structure.
//
// Exit:
//      ERROR_SUCCESS if successful,
//      ERROR_INVALID_FUNCTION if a child function failed,
//      ERROR_NOT_ENOUGH_MEMORY if memory allocation failed.
//
// Syncrhonization:
//      This function is non-reentrant.  Currently there is NO
//      synchronization provided to protect it from simultaneous calls.
//
//---------------------------------------------------------------------------
DWORD EnterControlledInit (
    PNAVAP_CONFIG prConfig
    )
    {
    SYM_ASSERT ( prConfig );

                                        // Don't initialize if we are
                                        // already initialized.

    if ( !bControlledInitComplete )
        {
        if ( CallOnNAVAPStack ( UserStackControlledInit,
                                prConfig,
                                &bControlledInitComplete ) )
            {
            return ( bControlledInitComplete ? ERROR_SUCCESS :
                                               ERROR_INVALID_FUNCTION );
            }

        return ( ERROR_NOT_ENOUGH_MEMORY );
        }

    return ( ERROR_SUCCESS );
    }

VOID UserStackControlledInit (
    PNAVAP_CONFIG prConfig,
    PBOOL         pbResult
    )
    {
                                        // Do the controlled initialization.

    *pbResult = NAVAPControlledInit(prConfig);
    }

//---------------------------------------------------------------------------
//
// DWORD EnterControlledDeInit (
//      VOID
//      );
//
// This function will undo whatever EnterControlledInit() did the last time
// it was called.  EnterControlledDeInit() will behave well if it is called
// without EnterControlledInit() ever being called, or if it was called and
// failed.  Call this function with only a local instance of Core
// initialized; it will allocate large stack on its own.
//
// Entry:
//      -
//
// Exit:
//      ERROR_SUCCESS if successful,
//      ERROR_INVALID_FUNCTION if a child function failed,
//      ERROR_NOT_ENOUGH_MEMORY if memory allocation failed.
//
// Syncrhonization:
//      This function is non-reentrant.  Currently there is NO
//      synchronization provided to protect it from simultaneous calls.
//
//---------------------------------------------------------------------------
DWORD EnterControlledDeInit (
    VOID
    )
    {
    auto BOOL bResult;

                                        // Try this only if
                                        // EnterControlledInit() has
                                        // succeeded.

    SymAcquireFastGlobalLock();

    if ( bControlledInitComplete )
        {
                                        // Set bControlledInitComplete to
                                        // FALSE.  As soon as we do that,
                                        // no new clients will ever start
                                        // using it, and the old ones that
                                        // are currently using it will
                                        // eventually release it.

        bControlledInitComplete = FALSE;

        SymReleaseFastGlobalLock();

                                        // Wait for all clients to release
                                        // the controlled initialization
                                        // resources.

        while ( lControlledInitUsage )
            {
            Sleep ( 100 );
            }

                                        // We should flush the cache.

        FlushCleanCache();

                                        // Continue processing on our own
                                        // stack.

        if ( CallOnNAVAPStack ( UserStackControlledDeInit,
                                &bResult ) )
            {
                                        // Return to client.

            return ( bResult ? ERROR_SUCCESS : ERROR_INVALID_FUNCTION );
            }

        return ( ERROR_NOT_ENOUGH_MEMORY );
        }

    SymReleaseFastGlobalLock();

    return ( ERROR_SUCCESS );
    }

VOID UserStackControlledDeInit (
    PBOOL pbResult
    )
    {
                                        // Do our "controlled" request.

    *pbResult = NAVAPControlledDeInit();
    }

//---------------------------------------------------------------------------
//
// DWORD GetInitializedModulesStatus (
//      VOID
//      );
//
// This function returns a DWORD containing the status of the modules in
// NAVAP.  These flags are the same flags that the dwInitializedModules
// member of NAVAP_STATUSPACKET returns.  Currently supported flags are:
//
//     NAVAP_INITIALIZEDMODULES_AVAPI.
//
// Entry:
//      -
//
// Exit:
//      Returns flags representing current status of NAVAP's modules.
//
//---------------------------------------------------------------------------
DWORD GetInitializedModulesStatus (
    VOID
    )
    {
    auto DWORD     dwFlags;
    auto HVCONTEXT hVContext;

                                        // Set default return value.

    dwFlags = 0;

                                        // Before entering this function,
                                        // make sure that we are initialized
                                        // and EnterControlledInit() and
                                        // EnterControlledDeInit() don't
                                        // mess something up.  When this
                                        // check is done, an usage count
                                        // will be updated.

    //SymAcquireFastGlobalLock();

    if ( bControlledInitComplete )
        {
        SymInterlockedIncrement ( &lControlledInitUsage );

        //SymReleaseFastGlobalLock();

                                        // If our controlled init has
                                        // completed, then we can query the
                                        // status of AVAPI.

        if ( bControlledInitComplete )
            {
            if ( hVContext = LockNAVAPScanConfiguration() )
                {
                dwFlags |= NAVAP_INITIALIZEDMODULES_AVAPI;

                UnlockNAVAPScanConfiguration ( hVContext );
                }
            }

                                        // Update the usage count.

        SymInterlockedDecrement ( &lControlledInitUsage );
        }
    else
        {
        //SymReleaseFastGlobalLock();
        }

                                        // Return status.

    return ( dwFlags );
    }

//---------------------------------------------------------------------------
//
// BOOL WINAPI Int2FChainCallback (
//      PCRS prCRS
//      );
//
// This function services the interrupt 2F callback in NAVAP.VXD.  It
// emulates NAVTSR's API.
//
// Entry:
//      prCRS - pointer to the registers when interrupt 2F was called.
//
// Exit:
//      TRUE  if interrupt was processed, and it should NOT be chained
//      FALSE if interrupt was not processed, and it should be chained
//
// Note:
//      The strings in this function should not be translated.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL WINAPI Int2FChainCallback (
    PCRS prCRS
    )
    {
    auto BOOL bResult;

                                        // Assume the interrupt was
                                        // addressed to the old NAVTSR.

    bResult = TRUE;

                                        // Try to service the interrupt.

    if ( ((Client_Byte_Reg_Struc*)prCRS)->Client_AH == 0xFE &&
         ((Client_Word_Reg_Struc*)prCRS)->Client_SI == 'NA' &&
         ((Client_Word_Reg_Struc*)prCRS)->Client_DI == 'NU'
       )
        {
                                        // Emulate NAVTSR API

        switch ( ((Client_Byte_Reg_Struc*)prCRS)->Client_AL )
            {
            case 0x00:

                                        // Is TSR present and is it enabled?

                ((Client_Byte_Reg_Struc*)prCRS)->Client_AH =
                    GetCurrentProcessPEL2Count() > 0 ? 0 : 1;

                                        // No PSP.

                ((Client_Word_Reg_Struc*)prCRS)->Client_CX = 0;

                                        // Clear carry flag for success.

                prCRS->Client_EFlags &= ~1;

                break;

            case 0x01:

                                        // Enable TSR.

                DeleteCurrentProcessFromPEL2();

                                        // Clear carry flag for success.

                prCRS->Client_EFlags &= ~1;

                break;

            case 0x02:

                                        // Disable TSR.

                AddCurrentProcessToPEL2();

                                        // Clear carry flag for success.

                prCRS->Client_EFlags &= ~1;

                break;

            case 0x03:

                                        // Get TSR pointers.  That one's
                                        // going to blow up...

                SYM_ASSERT ( FALSE );

                break;

            case 0x04:

                                        // Do some network redirector stuff.
                                        // That one's going to blow up too.

                SYM_ASSERT ( FALSE );

                break;

            default:
                                        // The interrupt was not addressed
                                        // to us.

                bResult = FALSE;
            }
        }
    else
        {
                                        // The interrupt was not addressed
                                        // to us.

        bResult = FALSE;
        }

                                        // If interrupt processed, confirm
                                        // it by setting standard return
                                        // values.

    if ( bResult )
        {
        ((Client_Word_Reg_Struc*)prCRS)->Client_SI = 'na';
        ((Client_Word_Reg_Struc*)prCRS)->Client_DI = 'nu';
        }

    return ( bResult );
    }

#endif // #if defined(SYM_VXD)
