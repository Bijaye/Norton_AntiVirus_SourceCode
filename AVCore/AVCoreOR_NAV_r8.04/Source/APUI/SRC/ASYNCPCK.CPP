// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APUI/VCS/ASYNCPCK.CPv   1.9   19 Feb 1998 16:38:56   RStanev  $
//
// Description:
//      This file defines UI functions for sending asynchronous information
//      packets to user mode.
//
// Contains:
//      UISendAsyncInformation()
//
// See Also:
//      apui.h
//
// Note:
//      UI module
//
//***************************************************************************

#include "platform.h"
#include "drvcomm.h"
#include "apcomm.h"
#include "navap.h"
#include "apui.h"

//---------------------------------------------------------------------------
//
// VOID UISendAsyncInformation (
//      HEVENTPACKET hEventPacket,
//      AVCONTEXT    uAVContext,
//      AVACTION     uLastAVAction,
//      BOOL         bLastActionCompleted,
//      BOOL         bFinalActionInSequence,
//      HVLVIRUS     hVLVirus
//      );
//
// This function sends an action information packet to NAVAP's user mode
// counterpart.
//
// Entry:
//      hEventPacket           - handle to current event packet
//      uAVContext             - context of the action taken by the kernel
//      uLastAVAction          - action ID
//      bLastActionCompleted   - result of the action
//      bFinalActionInSequence - TRUE if this was the last action for
//                               uAVContext, FALSE if not.
//      hVLVirus               - handle to the virus (if virus-related event)
//                               Non-virus related clients must pass NULL.
//
// Exit:
//      No status is returned.
//
//---------------------------------------------------------------------------
VOID UISendAsyncInformation (
    HEVENTPACKET hEventPacket,
    AVCONTEXT    uAVContext,
    AVACTION     uLastAVAction,
    BOOL         bLastActionCompleted,
    BOOL         bFinalActionInSequence,
    HVLVIRUS     hVLVirus
    )
    {
    auto PEDEXTRA                         prEDExtra;
    auto LONG                             lVirus, lFile, lDevice, lOwner,
                                          lMovedRenamed;
    auto LONG                             lPacketLength;
    auto PNAVAP_EVENTPROGRESSREPORTPACKET prPacket;

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );

                                        // We don't need to send information
                                        // packages if the configuration says
                                        // not to, or before we have taken
                                        // any actions.

    if ( !rNAVAPInternalCfg.bSendInfoPackets ||
         uLastAVAction == AVACTION_FIRST )
        {
        return;
        }

                                        // Initialize local variables.

    prEDExtra = hEventPacket->prEDExtra;

                                        // Determine what dynamic strings we
                                        // need to send with the packet and
                                        // calculate their lengths (not
                                        // including terminating 0s).

    lVirus = hVLVirus ? SIZE_VIRUSNAME : 0;

    lFile = lDevice = 0;

    switch ( uAVContext )
        {
        case AVCONTEXT_SCANFILE:
        case AVCONTEXT_WRITETOFILE:
        case AVCONTEXT_FILEROATTRCHANGE:

            lFile = STRLEN ( EDXpszOEMFile ( prEDExtra ) );

            break;

        case AVCONTEXT_HDFORMAT:
        case AVCONTEXT_HDMBRWRITE:
        case AVCONTEXT_HDBOOTWRITE:
        case AVCONTEXT_FDBOOTWRITE:
        case AVCONTEXT_SCANBOOT:
        case AVCONTEXT_SCANMBR:

            lDevice = STRLEN ( EDXpszOEMDevice ( prEDExtra ) );

        }

    lOwner = lMovedRenamed = 0;

    switch ( uAVContext )
        {
        case AVCONTEXT_HDFORMAT:
        case AVCONTEXT_HDMBRWRITE:
        case AVCONTEXT_HDBOOTWRITE:
        case AVCONTEXT_FDBOOTWRITE:
        case AVCONTEXT_WRITETOFILE:
        case AVCONTEXT_FILEROATTRCHANGE:

            lOwner = STRLEN ( EDXpszOEMOwner ( prEDExtra ) );

            break;

        case AVCONTEXT_SCANFILE:

            lMovedRenamed = STRLEN ( prEDExtra->pszBackupFile );

        }

                                        // Enabling the following block may
                                        // fix some memory fragmentation
                                        // issues.  This will work only when
                                        // the files used are less than
                                        // SYM_MAX_PATH.

//    if ( lFile )
//        {
//        lFile = max ( lFile, SYM_MAX_PATH );
//        }
//
//    if ( lDevice )
//        {
//        lDevice = max ( lDevice, SYM_MAX_PATH );
//        }
//
//    if ( lOwner )
//        {
//        lOwner = max ( lOwner, SYM_MAX_PATH );
//        }
//
//    if ( lMovedRenamed )
//        {
//        lMovedRenamed= max ( lMovedRenamed, SYM_MAX_PATH );
//        }

                                        // Allocate packet.  Note, that we
                                        // need to allocate one less WCHAR
                                        // for the strings, since this WCHAR
                                        // is taken out of szStringArray's
                                        // definition.

    SYM_ASSERT ( STRLEN ( prAPCfg(prEDExtra)->pszSpoolFileLocation ) );
    SYM_VERIFY_STRING ( prAPCfg(prEDExtra)->pszSpoolFileLocation );

    lPacketLength = sizeof(*prPacket) +
                    sizeof(WCHAR) * ( STRLEN ( prAPCfg(prEDExtra)->pszSpoolFileLocation ) + 1 +
                                      lVirus + 1 +
                                      lFile + lDevice + 1 +
                                      lOwner + lMovedRenamed );


    if ( prPacket = (PNAVAP_EVENTPROGRESSREPORTPACKET)
                        MemAllocPtr ( GHND, lPacketLength ) )
        {
        auto SYSTEMTIME rSystemTime;

                                        // Set time of packet.

        GetSystemTime ( &rSystemTime );

        prPacket->rSystemTime.wYear         = rSystemTime.wYear;
        prPacket->rSystemTime.wMonth        = rSystemTime.wMonth;
        prPacket->rSystemTime.wDayOfWeek    = rSystemTime.wDayOfWeek;
        prPacket->rSystemTime.wDay          = rSystemTime.wDay;
        prPacket->rSystemTime.wHour         = rSystemTime.wHour;
        prPacket->rSystemTime.wMinute       = rSystemTime.wMinute;
        prPacket->rSystemTime.wSecond       = rSystemTime.wSecond;
        prPacket->rSystemTime.wMilliseconds = rSystemTime.wMilliseconds;

                                        // Set user information.

        UIGetUserInfo ( prEDExtra->prSEEventData,
                        &prPacket->rUser );

                                        // Set packet sequence recognizer.

#if defined(SYM_NTK)

        prPacket->dwSequenceRecognizer = (DWORD)PsGetCurrentThread();

#elif defined(SYM_VXD)

        prPacket->dwSequenceRecognizer = (DWORD)Get_Cur_Thread_Handle();

#else

#error Don't know how to create sequence recognizer for target platform.

#endif

        prPacket->dwLastSequencePacket = bFinalActionInSequence;

                                        // Set action information.

        prPacket->dwAVContext       = uAVContext;
        prPacket->dwAction          = uLastAVAction;
        prPacket->dwActionCompleted = bLastActionCompleted;

                                        // Create the array of strings at the
                                        // end of the packet.

        auto LONG lIndex;

                                        // Set the name of the configuraiton
                                        // directory. Note, that this code
                                        // is optimized because it's index
                                        // in the array is 0!

#if !defined(UNICODE)

        if ( !OemToCharW ( prAPCfg(prEDExtra)->pszSpoolFileLocation,
                           prPacket->szStringArray ) )
            {
            *prPacket->szStringArray = '\0';

            SYM_ASSERT ( FALSE );
            }

#else
            wcscpy ( prPacket->szStringArray,
                     prAPCfg(prEDExtra)->pszSpoolFileLocation );
#endif

        lIndex = wcslen ( prPacket->szStringArray ) + 1;

                                        // Set name and type of virus.

        if ( lVirus )
            {
#if !defined(UNICODE)

            auto char szBuffer[SIZE_VIRUSNAME + 1];

            UIGetVirusName ( hVLVirus, szBuffer );

            if ( !OemToCharW ( szBuffer, &prPacket->szStringArray[lIndex] ) )
                {
                prPacket->szStringArray[lIndex] = '\0';

                SYM_ASSERT ( FALSE );
                }

#else
            UIGetVirusName ( hVLVirus, &prPacket->szStringArray[lIndex] );
#endif

            SYM_ASSERT ( wcslen ( &prPacket->szStringArray[lIndex] ) <= lVirus );

            prPacket->dwMacroVirus = VLVirusInfo ( hVLVirus, 0, VI_BOOL_MACRO_VIRUS, NULL ) ? 1 : 0;

            prPacket->dwVirusID = hVLVirus->stVirusInfoBlock.wVirID;

            }

        prPacket->dwVirusNameIndex = lIndex;

        lIndex += wcslen ( &prPacket->szStringArray[lIndex] ) + 1;

                                        // Set name of file or device in
                                        // trouble.

        if ( lFile )
            {
#if !defined(UNICODE)

            if ( !OemToCharW ( EDXpszOEMFile ( prEDExtra ),
                               &prPacket->szStringArray[lIndex] ) )
                {
                prPacket->szStringArray[lIndex] = '\0';

                SYM_ASSERT ( FALSE );
                }

#else
            wcscpy ( &prPacket->szStringArray[lIndex],
                     EDXpszOEMFile ( prEDExtra ) );
#endif

            SYM_ASSERT ( wcslen ( &prPacket->szStringArray[lIndex] ) <= lFile );
            }

        if ( lDevice )
            {
#if !defined(UNICODE)

            if ( !OemToCharW ( EDXpszOEMDevice ( prEDExtra ),
                               &prPacket->szStringArray[lIndex] ) )
                {
                prPacket->szStringArray[lIndex] = '\0';

                SYM_ASSERT ( FALSE );
                }

#else
            wcscpy ( &prPacket->szStringArray[lIndex],
                     EDXpszOEMDevice ( prEDExtra ) );
#endif

            SYM_ASSERT ( wcslen ( &prPacket->szStringArray[lIndex] ) <= lDevice );
            }

        prPacket->dwFileDeviceInTroubleIndex = lIndex;

        lIndex += wcslen ( &prPacket->szStringArray[lIndex] ) + 1;

                                        // Set name of file causing trouble.

        if ( lOwner )
            {
#if !defined(UNICODE)

            if ( !OemToCharW ( EDXpszOEMOwner ( prEDExtra ),
                               &prPacket->szStringArray[lIndex] ) )
                {
                prPacket->szStringArray[lIndex] = '\0';

                SYM_ASSERT ( FALSE );
                }

#else
            wcscpy ( &prPacket->szStringArray[lIndex],
                     EDXpszOEMOwner ( prEDExtra ) );
#endif

            SYM_ASSERT ( wcslen ( &prPacket->szStringArray[lIndex] ) <= lOwner );
            }

                                        // Set name of moved/renamed file.

        if ( lMovedRenamed )
            {
#if defined(UNICODE)
            wcscpy ( &prPacket->szStringArray[lIndex],
                     prEDExtra->pszBackupFile );
#elif defined(SYM_VXD)

            if ( !OemToCharW ( prEDExtra->pszBackupFile,
                               &prPacket->szStringArray[lIndex] ) )
                {
                prPacket->szStringArray[lIndex] = '\0';

                SYM_ASSERT ( FALSE );
                }

#else

                                        // ~~~TBD~~~ Convert
                                        // prEDExtra->pszBackupFile from
                                        // platform's native character set to
                                        // Unicode.

            SYM_ASSERT ( FALSE );

#endif

            SYM_ASSERT ( wcslen ( &prPacket->szStringArray[lIndex] ) <= lMovedRenamed );
            }

        SYM_ASSERT ( &prPacket->dwFileCausingTroubleIndex ==
                     &prPacket->dwMovedRenamedFileIndex );

        prPacket->dwFileCausingTroubleIndex = lIndex;

                                        // Send packet.

        if ( DriverCommAsyncSend ( NAVAP_COMM_EVENTPROGRESSREPORT,
                                   prPacket,
                                   lPacketLength ) != ERROR_DRVCOMM_SUCCESS )
            {
            SYM_ASSERT ( FALSE );
            }

                                        // Release memory allocated for
                                        // packet.

        MemFreePtr ( prPacket );
        }
    }