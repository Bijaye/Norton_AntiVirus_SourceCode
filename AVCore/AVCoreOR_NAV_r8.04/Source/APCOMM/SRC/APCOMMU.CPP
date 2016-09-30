// Copyright 1997 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APCOMM/VCS/APCOMMU.CPv   1.1   17 Feb 1998 14:27:52   RStanev  $
//
// Description:
//      This file defines user-mode functions for communicating with the
//      NAVAP driver.  These are internal NAV functions which will be able
//      to communicate only with the driver shipped in the same product
//      where the module using these functions is.
//
//      External NAVAP clients should use the exports of NAVAP32.DLL.
//
// Contains:
//      NAVAPiGetEnabledDisabled()
//      NAVAPiSetEnabledDisabled()
//      NAVAPiProtectProcess()
//      NAVAPiUnprotectProcess()
//      NAVAPiDecodeUserInfoSubPacket()
//
//      NAVAPiStart()
//      NAVAPiStop()
//      NAVAPiControlledInit()
//      NAVAPiControlledDeInit()
//      NAVAPiReloadConfiguration()
//      NAVAPiLoadDefinitions()
//      NAVAPiUnloadDefinitions()
//      NAVAPiGetStatus()
//
//      LoadNAVAPEL()
//
// See Also:
//      apcomm.h
//
// Note:
//
//***************************************************************************
// $Log:   S:/APCOMM/VCS/APCOMMU.CPv  $
//
//    Rev 1.1   17 Feb 1998 14:27:52   RStanev
// Updated copyright.
//
//    Rev 1.0   16 Apr 1997 10:43:16   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "drvcomm.h"
#include "apcomm.h"

// Prototype local functions.

static BOOL LoadNAVAPEL (
    VOID
    );

//---------------------------------------------------------------------------
//
// BOOL NAVAPiGetEnabledDisabled (
//      PDWORD pdwStatus
//      );
//
// This function returns the status of NAVAP (whether it is enabled or
// disabled).
//
// Entry:
//      pdwStatus - pointer to a DWORD to receive the status of NAVAP.
//
// Exit:
//      TRUE if NAVAP was loaded and its status was placed in pdwStatus.
//      FALSE if error.
//
//      Upon successful return, *pdwStatus will be set to 0 if NAVAP is
//      disabled and set to non-0 if NAVAP is enabled.
//
//---------------------------------------------------------------------------
BOOL NAVAPiGetEnabledDisabled (
    PDWORD pdwStatus
    )
    {
    auto DriverComm *pNAVAP;
    auto DWORD      dwStatus, dwBytesReturned;

    SYM_ASSERT ( pdwStatus );
    SYM_VERIFY_BUFFER ( pdwStatus, sizeof(*pdwStatus) );

    if ( pNAVAP = new DriverComm )
        {
        if ( pNAVAP->Open ( NAVAP_COMMUNICATION_NAME ) &&
             pNAVAP->Send ( NAVAP_COMM_GETENABLEDDISABLED,
                            NULL,
                            0,
                            &dwStatus,
                            sizeof(dwStatus),
                            &dwBytesReturned ) &&
             dwBytesReturned == sizeof(dwStatus) )
            {
            delete pNAVAP;

            *pdwStatus = dwStatus;

            return ( TRUE );
            }

        delete pNAVAP;
        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPiSetEnabledDisabled (
//      DWORD dwStatus
//      );
//
// This function enables or disables NAVAP.
//
// Entry:
//      dwStatus - new status of NAVAP.  0 if client wants to globally
//                 disable NAVAP and non-0 if client wants to globally
//                 enable NAVAP.
//
// Exit:
//      TRUE if NAVAP was loaded and its status was set according to
//      dwStatus.
//      FALSE if error.
//
//---------------------------------------------------------------------------
BOOL NAVAPiSetEnabledDisabled (
    DWORD dwStatus
    )
    {
    auto DriverComm *pNAVAP;

    if ( pNAVAP = new DriverComm )
        {
        if ( pNAVAP->Open ( NAVAP_COMMUNICATION_NAME ) &&
             pNAVAP->Send ( NAVAP_COMM_SETENABLEDDISABLED,
                            &dwStatus,
                            sizeof(dwStatus) ) )
            {
            delete pNAVAP;

            return ( TRUE );
            }

        delete pNAVAP;
        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPiProtectProcess (
//      VOID
//      );
//
// This function decrements the protection counter for the current process.
//
// By default all processes' protection counters are 0.  If a process wishes
// to tell NAVAP that it wants to perform actions which would be considered
// by NAVAP as harmful, then it should call NAVAPUnproctectProcess() before
// performing those actions.  This will increment this process' protection
// counter, making it a positive value, thus telling NAVAP to stop protecting
// this process.  After all "harm" is done, the process should call
// NAVAPProtectProcess() to decrement its counter.  NAVAP will never allow
// the counter to become a negative value, even though such condition can
// occur when NAVAP is loaded/unloaded.
//
// Entry:
//      -
//
// Exit:
//      TRUE if NAVAP was loaded and the request was serviced.
//      FALSE if error.
//
//---------------------------------------------------------------------------
BOOL NAVAPiProtectProcess (
    VOID
    )
{
    auto DriverComm *pNAVAP;

    if ( pNAVAP = new DriverComm )
    {
        if ( !pNAVAP->Open ( NAVAPEL_COMMUNICATION_NAME ) )
        {
            delete pNAVAP;

            if ( !LoadNAVAPEL() || !( pNAVAP = new DriverComm ) )
            {
                return ( FALSE );
            }

            if ( !pNAVAP->Open ( NAVAPEL_COMMUNICATION_NAME ) )
            {
                delete pNAVAP;

                return ( FALSE );
            }
        }

        if ( pNAVAP->Send ( NAVAPEL_COMM_PROTECTPROCESS ) )
        {
            delete pNAVAP;

            return ( TRUE );
        }

        delete pNAVAP;
    }

    return ( FALSE );
}

//---------------------------------------------------------------------------
//
// BOOL NAVAPiUnprotectProcess (
//      VOID
//      );
//
// This function increments the protection counter for the current process.
//
// By default all processes' protection counters are 0.  If a process wishes
// to tell NAVAP that it wants to perform actions which would be considered
// by NAVAP as harmful, then it should call NAVAPUnproctectProcess() before
// performing those actions.  This will increment this process' protection
// counter, making it a positive value, thus telling NAVAP to stop protecting
// this process.  After all "harm" is done, the process should call
// NAVAPProtectProcess() to decrement its counter.  NAVAP will never allow
// the counter to become a negative value, even though such condition can
// occur when NAVAP is loaded/unloaded.
//
// Entry:
//      -
//
// Exit:
//      TRUE if NAVAP was loaded and the request was serviced.
//      FALSE if error.
//
//---------------------------------------------------------------------------
BOOL NAVAPiUnprotectProcess (
    VOID
    )
    {
    auto DriverComm *pNAVAP;

    if ( pNAVAP = new DriverComm )
    {
        if ( !pNAVAP->Open ( NAVAPEL_COMMUNICATION_NAME ) )
        {
            delete pNAVAP;

            if ( !LoadNAVAPEL() || !( pNAVAP = new DriverComm ) )
            {
                return ( FALSE );
            }

            if ( !pNAVAP->Open ( NAVAPEL_COMMUNICATION_NAME ) )
            {
                delete pNAVAP;

                return ( FALSE );
            }
        }

        if ( pNAVAP->Send ( NAVAPEL_COMM_UNPROTECTPROCESS ) )
        {
            delete pNAVAP;

            return ( TRUE );
        }

        delete pNAVAP;
    }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL NAVAPiDecodeUserInfoSubPacket (
//      PNAVAP_USERINFOSUBPACKET prPacket,
//      PTSTR                    pszUser,
//      PDWORD                   pcchUser,
//      PTSTR                    pszDomain,
//      PDWORD                   pcchDomain
//      );
//
// This function decodes a NAVAP user info subpacket.  This "subpacket" is
// returned as part of some of NAVAP's packets when user information is
// required for their processing.  Please refer to APCOMM.H on what the
// subpacket exactly contains.
//
// Entry:
//      prPacket   - pointer to a subpacket, as sent by NAVAP.
//      pszUser    - pointer to a buffer to receive the name of the user
//                   associated with prPacket.  This parameter can be set to
//                   NULL if the client does not need to retrieve that name.
//      pcchUser   - pointer to DWORD containing the size (in characters) of
//                   the memory block pointed to by pszUser.  This parameter
//                   is ignored if pszUser is NULL.  If pcchUser is NULL,
//                   or *pcchUser is 0, then nothing will be copied to
//                   pszUser, regardless of what pszUser points to.
//      pszDomain  - pointer to a buffer to receive the name of the
//                   computer or domain associated with prPacket.  This
//                   parameter can be set to NULL if the client does not
//                   need to retrieve the domain/computer name.
//      pcchDomain - pointer to DWORD containing the size (in characters) of
//                   the memory block pointed to by pszDomain.  This
//                   parameter is ignored if pszDomain is NULL.  If
//                   pcchDomain is NULL, or *pcchDomain is 0, then nothing
//                   will be copied to pszDomain, regardless of what
//                   pszDomain points to.
//
// Exit:
//      Returns TRUE if successfull, FALSE otherwise.  Use GetLastError()
//      to determine the reason for the failure.  If GetLastError() returned
//      ERROR_INSUFFICIENT_BUFFER, *pcchUser OR *pcchDomain (whichever is
//      greater than its original value) will contain the number of
//      characters that need to be copied into that buffer.
//
//      Upon successful completion of this function, *pcchUser (if
//      applicable) will be set to the number of characters copied to
//      pszUser, and *pcchDomain (if applicable) will be set to the number
//      of characters copied to pszDomain.
//
//      If this function fails, the status of the return buffers is
//      undefined.
//
// Note:
//      prPacket will be decoded properly only on the computer where NAVAP
//      originally built the packet.
//
//---------------------------------------------------------------------------
BOOL NAVAPiDecodeUserInfoSubPacket (
    PNAVAP_USERINFOSUBPACKET prPacket,
    PTSTR                    pszUser,
    PDWORD                   pcchUser,
    PTSTR                    pszDomain,
    PDWORD                   pcchDomain
    )
    {
    auto DWORD cchTempUser, cchTempDomain;

                                        // Validate input parameters.

    SYM_ASSERT ( prPacket );
    SYM_VERIFY_BUFFER ( prPacket, sizeof(*prPacket) );

                                        // Adjust parameters to make it easy
                                        // on us when handling all different
                                        // cases of present and not-present
                                        // arguments.  At the same time
                                        // initialize local variables.

    if ( !pszUser || !pcchUser || !*pcchUser )
        {
        pszUser = NULL;

        pcchUser && ( *pcchUser = 0 );

        pcchUser = &cchTempUser;

        cchTempUser = 0;
        }
    else
        {
        cchTempUser = *pcchUser;

        SYM_VERIFY_BUFFER ( pszUser, cchTempUser * sizeof(TCHAR) );
        }

    if ( !pszDomain || !pcchDomain || !*pcchDomain )
        {
        pszDomain = NULL;

        pcchDomain && ( *pcchDomain = 0 );

        pcchDomain = &cchTempDomain;

        cchTempDomain = 0;
        }
    else
        {
        cchTempDomain = *pcchDomain;

        SYM_VERIFY_BUFFER ( pszDomain, cchTempDomain * sizeof(TCHAR) );
        }

                                        // In order for us to cut down on
                                        // calls to LookupAccountSid(), we
                                        // will make sure the client has
                                        // requested at least one user data
                                        // string.

    if ( !pszUser && !pszDomain )
        {
        return ( TRUE );
        }

                                        // See what kind of packet we have
                                        // here.

    if ( prPacket->bUseAsSIDInsteadOfString )
        {
                                        // We have a SID.

        auto SID_NAME_USE eUse;

                                        // Calculate how much memory we need
                                        // for SID conversion...  If this
                                        // initial conversion succeeds, this
                                        // means that the user requested
                                        // both user and computer/domain
                                        // names and the supplied buffers
                                        // were large enough to hold them.

        if ( LookupAccountSid ( NULL,
                                &prPacket->rUserInfo.Sid,
                                pszUser,
                                pcchUser,
                                pszDomain,
                                pcchDomain,
                                &eUse ) )
            {
            return ( TRUE );
            }
        else
            {
            auto BOOL bResult;

                                        // Prepare to bail out at any time

            bResult = FALSE;

                                        // Make sure the reason for the
                                        // failure was a missing buffer.

            if ( GetLastError() == ERROR_INSUFFICIENT_BUFFER &&

                                        // Check if the user supplied buffers
                                        // are large enough for the SID
                                        // conversion.

                 ( !pszUser || ( cchTempUser >= *pcchUser ) ) &&
                 ( !pszDomain || ( cchTempDomain >= *pcchDomain ) ) &&

                                        // Allocate necessary memory

                 ( pszUser || ( pszUser = new TCHAR[cchTempUser] ) ) &&
                 ( pszDomain || ( pszDomain = new TCHAR[cchTempDomain] ) )
               )
                {
                                        // Convert SID

                if ( LookupAccountSid ( NULL,
                                        &prPacket->rUserInfo.Sid,
                                        pszUser,
                                        pcchUser,
                                        pszDomain,
                                        pcchDomain,
                                        &eUse ) )
                    {
                    bResult = TRUE;
                    }

                                        // Release allocated memory, without
                                        // destroying the last error code.

                auto DWORD dwSaveLastError;

                dwSaveLastError = GetLastError();

                if ( pcchUser == &cchTempUser )
                    {
                    delete []pszUser;
                    }

                if ( pcchDomain == &cchTempDomain )
                    {
                    delete []pszDomain;
                    }

                SetLastError ( dwSaveLastError );
                }

            return ( bResult );
            }
        }

                                        // We have a UNC-style string.

    auto PWSTR pszStart, pszEnd, pszLastSeparator;
    auto DWORD cchMisc;

                                        // Get the status of the separators
                                        // in the string.

    for ( pszStart = pszEnd = prPacket->rUserInfo.szUserID, cchMisc = 0;
          *pszEnd;
          pszEnd++ )
        {
        if ( *pszEnd == '\\' )
            {
            pszLastSeparator = pszEnd;

            cchMisc++;
            }
        }

                                        // Make sure we have a valid string.
                                        // That is "\\domain\user" or "user".
                                        // Note, that we will allow empty
                                        // strings for domain or user names.

    if ( cchMisc &&
         ( cchMisc != 3 || pszStart[0] != '\\' || pszStart[1] != '\\' )
       )
        {
        SetLastError ( ERROR_INVALID_DATA );

        return ( FALSE );
        }

#if defined(UNICODE)

    auto DWORD cchLength;

                                        // Copy the user name.

    if ( pszUser )
        {
        cchLength = pszEnd - ( cchMisc ? pszLastSeparator + 1 :
                                         pszStart );

        *pcchUser = cchLength + 1;

        if ( cchLength < cchTempUser )
            {
            memcpy ( pszUser,
                     cchMisc ? pszLastSeparator + 1 : pszStart,
                     cchLength * sizeof(WCHAR) );

            pszUser[cchLength] = '\0';
            }
        else
            {
            SetLastError ( ERROR_INSUFFICIENT_BUFFER );

            return ( FALSE );
            }
        }

                                        // Copy the domain/server name

    if ( pszDomain )
        {
        if ( cchMisc )
            {
            pszStart += 2;

            cchLength = pszLastSeparator - pszStart;

            *pcchDomain = cchLength + 1;

            if ( cchLength < cchTempDomain )
                {
                memcpy ( pszDomain,
                         pszStart,
                         cchLength * sizeof(WCHAR) );

                pszDomain[cchLength] = '\0';
                }
            else
                {
                SetLastError ( ERROR_INSUFFICIENT_BUFFER );

                return ( FALSE );
                }
            }
        else
            {
            *pszDomain = '\0';

            *pcchDomain = 1;
            }
        }

    return ( TRUE );

#else

    SetLastError ( ERROR_INVALID_FUNCTION );

    return ( FALSE );

#endif
    }

// *** START PATCH ***

static BOOL bInitialized = FALSE;
static BOOL bRunningOnNT = FALSE;
static HANDLE hPacketEvent = NULL;
static CRITICAL_SECTION rPacketLock;
static BOOL bPacketWaitersBlocked = FALSE;

typedef struct tagNAVAPQUEUEDPACKET
{
    tagNAVAPQUEUEDPACKET * pNext;
    DWORD dwServiceCode;
    DWORD dwPacketSize;
} NAVAPQUEUEDPACKET, *PNAVAPQUEUEDPACKET;

static PNAVAPQUEUEDPACKET pPacketHead = NULL;
static PNAVAPQUEUEDPACKET pPacketTail = NULL;

static NT_SERVICE_START pServiceStart = NULL;
static NT_SERVICE_STOP pServiceStop = NULL;

BOOL NAVAPiInit (
    NT_SERVICE_START pfnServiceStart,
    NT_SERVICE_STOP pfnServiceStop
    )
{
    if ( bInitialized )
        return FALSE;

    auto OSVERSIONINFO rInfo;

    rInfo.dwOSVersionInfoSize = sizeof(rInfo);

    if ( !GetVersionEx ( &rInfo ) )
        return FALSE;

    if ( rInfo.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS && rInfo.dwPlatformId != VER_PLATFORM_WIN32_NT )
        return FALSE;

    bRunningOnNT = ( rInfo.dwPlatformId == VER_PLATFORM_WIN32_NT );

    if ( hPacketEvent = CreateEvent ( NULL, TRUE, FALSE, NULL ) )
    {
        InitializeCriticalSection ( &rPacketLock );

        bPacketWaitersBlocked = TRUE;

        pServiceStart = pfnServiceStart;
        pServiceStop = pfnServiceStop;

        return bInitialized = TRUE;
    }

    return bInitialized = FALSE;
}

void NAVAPiDeInit()
{
    if ( bInitialized )
    {
        EnterCriticalSection ( &rPacketLock );
        for ( auto PNAVAPQUEUEDPACKET pPacket = pPacketHead; pPacket; )
        {
            pPacketTail = pPacket->pNext;
            delete[] (char *)pPacket;
            pPacket = pPacketTail;
        }
        bPacketWaitersBlocked = FALSE;
        LeaveCriticalSection ( &rPacketLock );
        DeleteCriticalSection ( &rPacketLock );
        CloseHandle ( hPacketEvent );
    }
}

static VOID NAVAPServiceRequests ( DWORD  dwServiceCode,
                                   PVOID  pvInBuffer,
                                   DWORD  dwInBufferSize,
                                   PVOID  pvOutBuffer,
                                   DWORD  dwOutBufferSize,
                                   PDWORD pdwBytesReturned )
{
    EnterCriticalSection ( &rPacketLock );

    auto PNAVAPQUEUEDPACKET pPacket = (PNAVAPQUEUEDPACKET) new char[sizeof(*pPacket) + dwInBufferSize];

    if ( pPacket )
    {
        pPacket->pNext = NULL;
        pPacket->dwServiceCode = dwServiceCode;
        if ( pPacket->dwPacketSize = dwInBufferSize )
            MEMCPY ( &pPacket[1], pvInBuffer, dwInBufferSize );

        if ( pPacketTail )
        {
            pPacketTail->pNext = pPacket;
            pPacketTail = pPacket;
        }
        else
        {
            pPacketHead = pPacketTail = pPacket;
            SetEvent ( hPacketEvent );
        }
    }

    LeaveCriticalSection ( &rPacketLock );
}

BOOL NAVAPiWaitForPacket (
    PVOID pvData,
    PDWORD pdwData
    )
{
    if ( !bPacketWaitersBlocked )
        return FALSE;

    EnterCriticalSection ( &rPacketLock );

    while ( bPacketWaitersBlocked )
    {
        LeaveCriticalSection ( &rPacketLock );

        if ( WaitForSingleObject ( hPacketEvent, INFINITE ) != WAIT_OBJECT_0 )
            return FALSE;

        EnterCriticalSection ( &rPacketLock );

        if ( !bPacketWaitersBlocked )
            break;

        if ( pPacketHead )
        {
            if ( pPacketHead->dwPacketSize + sizeof(DWORD) > *pdwData )
            {
                *pdwData = pPacketHead->dwPacketSize + sizeof(DWORD);
                LeaveCriticalSection ( &rPacketLock );
                return TRUE;
            }

            auto PNAVAPQUEUEDPACKET pPacket = pPacketHead;

            if ( ! ( pPacketHead = pPacket->pNext ) )
            {
                pPacketTail = NULL;
                ResetEvent ( hPacketEvent );
            }

            *((PDWORD)pvData) = pPacket->dwServiceCode;
            MEMCPY ( &((PDWORD)pvData)[1], &pPacket[1], pPacket->dwPacketSize );
            *pdwData = pPacket->dwPacketSize + sizeof(DWORD);

            LeaveCriticalSection ( &rPacketLock );

            delete[] (char *)pPacket;

            return TRUE;
        }
    }

    LeaveCriticalSection ( &rPacketLock );

    return FALSE;
}

BOOL NAVAPiReleaseWaiters()
{
    EnterCriticalSection ( &rPacketLock );

    bPacketWaitersBlocked = FALSE;
    SetEvent ( hPacketEvent );

    LeaveCriticalSection ( &rPacketLock );
    return TRUE;
}

BOOL NAVAPiBlockWaiters()
{
    EnterCriticalSection ( &rPacketLock );

    bPacketWaitersBlocked = TRUE;
    ResetEvent ( hPacketEvent );

    LeaveCriticalSection ( &rPacketLock );
    return TRUE;
}

static DriverComm * pNAVAPHandle = NULL;

HANDLE NAVAPiStart (
    PSTR pszLocation
    )
{
    if ( !bInitialized || pNAVAPHandle )
        return NULL;

    auto DriverComm * pDriver = new DriverComm;

    if ( pDriver )
    {
        // Make sure we have NAVAPEL.

        if ( !pDriver->Open ( NAVAPEL_COMMUNICATION_NAME ) )
        {
            delete pDriver;

            pDriver = new DriverComm;

            if ( !pDriver )
                return NULL;

            if ( !LoadNAVAPEL() || !pDriver->Open ( NAVAPEL_COMMUNICATION_NAME ) )
            {
                delete pDriver;
                return NULL;
            }
        }

        delete pDriver;

        pDriver = new DriverComm;

        if ( pDriver )
        {
            if ( bRunningOnNT )
            {
                if ( pServiceStart ( "SYMEVENT", NULL ) && pServiceStart ( "NAVAP", pszLocation ) )
                {
                    if ( pDriver->Open ( _T("NAVAP") ) &&
                         pDriver->RegisterServiceCallback ( NAVAPServiceRequests ) )
                        return (HANDLE) ( pNAVAPHandle = pDriver );

                    pServiceStop ( "NAVAP" );
                }
            }
            else
            {
                if ( pDriver->SetVxDLocation ( pszLocation ) &&
                     pDriver->LoadAndOpen ( _T("NAVAP") ) &&
                     pDriver->RegisterServiceCallback ( NAVAPServiceRequests ) )
                    return (HANDLE) ( pNAVAPHandle = pDriver );
            }

            delete pDriver;
        }
    }

    return NULL;
}

BOOL NAVAPiStop (
    HANDLE hDriver
    )
{
    auto DriverComm * pDriver= (DriverComm *) hDriver;

    if ( !pDriver || pDriver != pNAVAPHandle )
        return FALSE;

    delete pDriver;

    pNAVAPHandle = NULL;

    if ( bRunningOnNT )
        pServiceStop ( "NAVAP" );

    return TRUE;
}

BOOL NAVAPiControlledInit (
    HANDLE hDriver,
    PNAVAP_CONFIG prConfig
    )
{
    if ( !bInitialized || !hDriver )
        return FALSE;

    auto DriverComm * pDriver = (DriverComm *) hDriver;

    DWORD dwStatus, dwWritten;

    if ( pDriver->Send ( NAVAP_COMM_CONTROLLEDINITEX, prConfig, 10, &dwStatus, sizeof(dwStatus), &dwWritten ) )
    {
        if ( dwStatus == ERROR_SUCCESS )
            return TRUE;
    }

    return FALSE;
}

BOOL NAVAPiControlledDeInit (
    HANDLE hDriver
    )
{
    if ( !bInitialized || !hDriver )
        return FALSE;

    auto DriverComm * pDriver = (DriverComm *) hDriver;

    DWORD dwStatus, dwWritten;

    if ( pDriver->Send ( NAVAP_COMM_CONTROLLEDDEINIT, NULL, 0, &dwStatus, sizeof(dwStatus), &dwWritten ) )
    {
        if ( dwStatus == ERROR_SUCCESS )
            return TRUE;
    }

    return FALSE;
}

BOOL NAVAPiReloadConfiguration (
    HANDLE hDriver,
    PNAVAP_CONFIG prConfig
    )
{
    if ( !bInitialized || !hDriver )
        return FALSE;

    auto DriverComm * pDriver = (DriverComm *) hDriver;

    DWORD dwStatus, dwWritten;

    if ( pDriver->Send ( NAVAP_COMM_RELOADCONFIGEX, prConfig, 10, &dwStatus, sizeof(dwStatus), &dwWritten ) )
    {
        if ( dwStatus == ERROR_SUCCESS )
            return TRUE;
    }

    return FALSE;
}

BOOL NAVAPiLoadDefinitions (
    HANDLE hDriver,
    PTSTR pszLocation
    )
{
    if ( !bInitialized || !hDriver )
        return FALSE;

    auto DriverComm * pDriver = (DriverComm *) hDriver;

    if ( !bRunningOnNT ||
         ( pServiceStart ( "NAVEX15", pszLocation ) && pServiceStart ( "NAVENG", pszLocation ) )
       )
    {
        DWORD dwStatus, dwWritten;
        char szOemBuffer[260];
        // The driver expects an OEM path and pszLocation is ANSI
        CharToOem(pszLocation, szOemBuffer);

        if ( pDriver->Send ( NAVAP_COMM_LOADAVAPIEX, szOemBuffer, STRLEN ( szOemBuffer ), &dwStatus, sizeof(dwStatus), &dwWritten ) )
        {
            if ( dwStatus == ERROR_SUCCESS )
                return TRUE;
        }
    }

    if ( bRunningOnNT )
    {
        pServiceStop ( "NAVENG" );
        pServiceStop ( "NAVEX15" );
    }

    return FALSE;
}

BOOL NAVAPiUnloadDefinitions (
    HANDLE hDriver
    )
{
    if ( !bInitialized || !hDriver )
        return FALSE;

    auto DriverComm * pDriver = (DriverComm *) hDriver;

    DWORD dwStatus, dwWritten;

    if ( pDriver->Send ( NAVAP_COMM_UNLOADAVAPI, NULL, 0, &dwStatus, sizeof(dwStatus), &dwWritten ) )
    {
        if ( bRunningOnNT )
        {
            pServiceStop ( "NAVENG" );
            pServiceStop ( "NAVEX15" );
        }

        if ( dwStatus == ERROR_SUCCESS )
            return TRUE;
    }

    return FALSE;
}

BOOL NAVAPiGetStatus (
    HANDLE hDriver,
    PNAVAP_STATUSPACKET prPacket
    )
{
    if ( !bInitialized || !hDriver )
        return FALSE;

    auto DriverComm * pDriver = (DriverComm *) hDriver;

    auto DWORD dwReturned;

    if ( pDriver->Send ( NAVAP_COMM_GETSTATUS, NULL, 0, prPacket, prPacket->dwSize, &dwReturned ) &&
         ( prPacket->dwErrorStatus == ERROR_SUCCESS ||
           prPacket->dwErrorStatus == ERROR_NOT_ENOUGH_MEMORY ) )
    {
        prPacket->dwErrorStatus = ERROR_SUCCESS;
        return TRUE;
    }

    return FALSE;
}

// LoadNAVAPEL() makes sure that NAVAPEL_COMMUNICATION_NAME is loaded and started.
// The driver location is retrieved from \\HKEY_LOCAL_MACHINE\SOFTWARE\Symantec\InstalledApps
// under the NAVNT (for WinNT) or NAV95 (for Win9x) values.
//
// Entry:
//      -
//
// Exit:
//      TRUE if successful, FALSE if error.

typedef SC_HANDLE (WINAPI *fnOpenSCManager)       (LPCTSTR,LPCTSTR,DWORD);
typedef SC_HANDLE (WINAPI *fnOpenService)         (SC_HANDLE,LPCTSTR,DWORD);
typedef SC_HANDLE (WINAPI *fnCreateService)       (SC_HANDLE,LPCTSTR,LPCTSTR,DWORD,DWORD,DWORD,DWORD,LPCTSTR,LPCTSTR,LPDWORD,LPCTSTR,LPCTSTR,LPCTSTR);
typedef BOOL      (WINAPI *fnStartService)        (SC_HANDLE,DWORD,LPCTSTR *);
typedef BOOL      (WINAPI *fnChangeServiceConfig) (SC_HANDLE,DWORD,DWORD,DWORD,LPCTSTR,LPCTSTR,LPDWORD,LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR);
typedef BOOL      (WINAPI *fnCloseServiceHandle)  (SC_HANDLE);

BOOL LoadNAVAPEL (
    VOID
    )
{
    // Get the OS version and make sure it is either 9x or NT.

    auto OSVERSIONINFO rInfo;

    rInfo.dwOSVersionInfoSize = sizeof(rInfo);

    if ( !GetVersionEx ( &rInfo ) )
        return FALSE;

    if ( rInfo.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS && rInfo.dwPlatformId != VER_PLATFORM_WIN32_NT )
        return FALSE;

    // Get the location of NAV.

    auto HKEY hKey;

    if ( RegOpenKeyEx ( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Symantec\\InstalledApps"), 0, KEY_READ, &hKey ) != ERROR_SUCCESS )
        return FALSE;

    auto DWORD dwType = REG_SZ;
    auto TCHAR szPath[260];
    auto DWORD cbPathSize = sizeof(szPath);

    if ( RegQueryValueEx ( hKey, rInfo.dwPlatformId == VER_PLATFORM_WIN32_NT ? _T("NAVNT") : _T("NAV95"), NULL, &dwType, (LPBYTE) szPath, &cbPathSize ) != ERROR_SUCCESS ||
         cbPathSize < sizeof(TCHAR) )
    {
        CloseHandle ( hKey );

        return FALSE;
    }

    CloseHandle ( hKey );

    // Create a global mutex to synchronize execution of this function.

    auto HANDLE hMutex = CreateMutex ( NULL, FALSE, _T("LoadNAVAPELGlobalMutex") );

    if ( !hMutex )
        return FALSE;

    if ( WaitForSingleObject ( hMutex, INFINITE ) != WAIT_OBJECT_0 )
    {
        CloseHandle ( hMutex );

        return FALSE;
    }

    // Load the NAVAPEL driver.

    if ( rInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
    {
        HINSTANCE hLibrary = LoadLibrary ( _T("ADVAPI32") );

        if ( hLibrary )
        {
            // Get the addresses of the necessary service control manager functions.

#if defined(UNICODE)
            static TCHAR szOpenSCManager[]       = _T("OpenSCManagerW");
            static TCHAR szOpenService[]         = _T("OpenServiceW");
            static TCHAR szCreateService[]       = _T("CreateServiceW");
            static TCHAR szStartService[]        = _T("StartServiceW");
            static TCHAR szChangeServiceConfig[] = _T("ChangeServiceConfigW");
#else
            static TCHAR szOpenSCManager[]       = _T("OpenSCManagerA");
            static TCHAR szOpenService[]         = _T("OpenServiceA");
            static TCHAR szCreateService[]       = _T("CreateServiceA");
            static TCHAR szStartService[]        = _T("StartServiceA");
            static TCHAR szChangeServiceConfig[] = _T("ChangeServiceConfigA");
#endif
            static TCHAR szCloseServiceHandle[]  = _T("CloseServiceHandle");

            auto fnOpenSCManager       pOpenSCManager       = (fnOpenSCManager)       GetProcAddress ( hLibrary, szOpenSCManager );
            auto fnOpenService         pOpenService         = (fnOpenService)         GetProcAddress ( hLibrary, szOpenService );
            auto fnCreateService       pCreateService       = (fnCreateService)       GetProcAddress ( hLibrary, szCreateService );
            auto fnStartService        pStartService        = (fnStartService)        GetProcAddress ( hLibrary, szStartService );
            auto fnChangeServiceConfig pChangeServiceConfig = (fnChangeServiceConfig) GetProcAddress ( hLibrary, szChangeServiceConfig );
            auto fnCloseServiceHandle  pCloseServiceHandle  = (fnCloseServiceHandle)  GetProcAddress ( hLibrary, szCloseServiceHandle );

            if ( pOpenSCManager && pOpenService && pCreateService && pStartService && pChangeServiceConfig && pCloseServiceHandle )
            {
                // Get the full path name to our driver.

                _tcscat ( szPath, _T("\\") );
                _tcscat ( szPath, NAVAPEL_COMMUNICATION_NAME );
                _tcscat ( szPath, _T(".SYS") );

                // Open a handle to the service control manager.

                auto SC_HANDLE hScm = pOpenSCManager ( NULL, NULL, SC_MANAGER_ALL_ACCESS );

                if ( hScm )
                {
                    // Try to open a handle to the NAVAPEL service.

                    auto SC_HANDLE hSvc = pOpenService ( hScm, NAVAPEL_COMMUNICATION_NAME, SERVICE_QUERY_STATUS );

                    if ( hSvc )
                    {
                        // The open has succeeded, which means that the service has already been created.
                        // In this case, we only need to update its configuration.

                        pCloseServiceHandle ( hSvc );

                        hSvc = pOpenService ( hScm, NAVAPEL_COMMUNICATION_NAME, SERVICE_ALL_ACCESS );

                        if ( hSvc )
                        {
                            if ( !pChangeServiceConfig ( hSvc,
                                                         SERVICE_KERNEL_DRIVER,
                                                         SERVICE_DEMAND_START,
                                                         SERVICE_ERROR_NORMAL,
                                                         szPath,
                                                         NULL,
                                                         NULL,
                                                         NULL,
                                                         NULL,
                                                         NULL,
                                                         NULL ) )
                            {
                                pCloseServiceHandle ( hSvc );

                                hSvc = NULL;
                            }
                        }
                    }
                    else
                    {
                        // The open has failed, which means that the service has not been created.  In this
                        // case, we need to create the service.

                        hSvc = pCreateService ( hScm,
                                                NAVAPEL_COMMUNICATION_NAME,
                                                NAVAPEL_COMMUNICATION_NAME,
                                                SERVICE_ALL_ACCESS,
                                                SERVICE_KERNEL_DRIVER,
                                                SERVICE_DEMAND_START,
                                                SERVICE_ERROR_NORMAL,
                                                szPath,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL );
                    }

                    // If at this point we have a handle to the service, then we only need to start it.

                    if ( hSvc )
                    {
                        if ( pStartService ( hSvc, 0, NULL ) ||
                             GetLastError() == ERROR_SERVICE_ALREADY_RUNNING )
                        {
                            pCloseServiceHandle ( hSvc );
                            pCloseServiceHandle ( hScm );

                            FreeLibrary ( hLibrary );

                            ReleaseMutex ( hMutex );
                            CloseHandle ( hMutex );

                            return TRUE;
                        }
                    }

                    pCloseServiceHandle ( hScm );
                }
            }

            FreeLibrary ( hLibrary );
        }
    }
    else
    {
        auto DriverComm * pDriver = new DriverComm;

        if ( pDriver )
        {
            if ( pDriver->SetVxDLocation ( szPath ) &&
                 pDriver->LoadAndOpen ( NAVAPEL_COMMUNICATION_NAME, FALSE ) )
            {
                delete pDriver;

                ReleaseMutex ( hMutex );
                CloseHandle ( hMutex );

                return TRUE;
            }

        delete pDriver;
        }
    }

    ReleaseMutex ( hMutex );
    CloseHandle ( hMutex );

    return FALSE;
}
