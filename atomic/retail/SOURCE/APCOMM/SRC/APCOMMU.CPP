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
        if ( pNAVAP->Open ( NAVAP_COMMUNICATION_NAME ) &&
             pNAVAP->Send ( NAVAP_COMM_PROTECTPROCESS ) )
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
        if ( pNAVAP->Open ( NAVAP_COMMUNICATION_NAME ) &&
             pNAVAP->Send ( NAVAP_COMM_UNPROTECTPROCESS ) )
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
