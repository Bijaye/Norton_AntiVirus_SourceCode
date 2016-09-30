// Copyright 1995 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APUI/VCS/UIMISC.CPv   1.6   17 Feb 1998 20:53:56   RStanev  $
//
// Description:
//      This file defines miscellaneous UI functions.
//
// Contains:
//      UIInitError()
//      UIFastError()               (SYM_VXD only, local function)
//      ParseTextLine()             (SYM_VXD only, local function)
//
//      UIControlledInitMessage()
//
//      UIOutOfMemoryError()
//
//      UIShutdownError()
//
//      UIShowLongCustomAlert()
//
//      UIGetVirusName()
//      UIGetUserInfo()
//      UIGetCurrentUserSID()       (SYM_NTK only)
//
// See Also:
//      uiapi.h
//
// Note:
//      UI module
//
//***************************************************************************
// $Log:   S:/APUI/VCS/UIMISC.CPv  $
//
//    Rev 1.6   17 Feb 1998 20:53:56   RStanev
// Updated copyright.
//
//    Rev 1.5   02 Jun 1997 10:17:20   RStanev
// Added timeout support to UIShowLongCustomAlert().
//
//    Rev 1.4   30 May 1997 19:55:22   RStanev
// UIShowLongCustomAlert() will now display alerts only if the
// pszLongCustomAlert member of hEventPacket's configuration is
// not NULL.
//
//    Rev 1.3   28 May 1997 13:00:56   RStanev
// Added UIShowLongCustomAlert().
//
//    Rev 1.2   15 May 1997 02:28:34   RStanev
// Too many changes to describe.
//
//    Rev 1.1   29 Apr 1997 13:18:34   RStanev
// Fixed compilation errors introduced by AVENGE changes.
//
//    Rev 1.0   09 Apr 1997 12:16:02   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "dbcs.h"
#include "symsync.h"
#include "apcomm.h"
#include "navap.h"
#include "apui.h"

//---------------------------------------------------------------------------
//
// lDisplayOutOfMemoryError is a flag for UIOutOfMemoryError() which is
// used to disable the function after the first time it is called.
//
//---------------------------------------------------------------------------
#define DRIVER_NTK_LOCKED_DATA_SEGMENT
#include "drvseg.h"

LONG lDisplayOutOfMemoryError = 1;

#define DRIVER_DEFAULT_DATA_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// Prototype local functions.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

VOID UIFastError (
    PSTR pszMessage
    );

VOID ParseTextLine (
    PTSTR *ppszNextLine,
    PTSTR *ppszLineStart,
    PUINT puLineSize,
    UINT  uMaxLineSize
    );

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// The code for UIInitError(), UIFastError(), and ParseTextLine() should be
// discarded after initialization.
//
//---------------------------------------------------------------------------
#define DRIVER_INIT_CODE_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// VOID UIInitError (
//      UIINITERROR eError
//      );
//
// This function reports errors during global initialization of NAVAP.  It
// is available only during initialization; its code is discarded after
// that.  In SYM_VXD, this function will display the errors using the local
// NAVAP UI functions.  In SYM_NTK, this function will log the errors to the
// NT Event Log.
//
// Entry:
//      eError - a member of UIINITERROR.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID UIInitError (
    UIINITERROR eError
    )
    {
#if defined(SYM_VXD)

    auto PTSTR pszText;
    auto void  *apvParameters[_UI_PARAMETER_TABLE_SIZE];

                                        // Prepare strings and parameters
                                        // for BuildText().

    apvParameters[_UI_INIT_ERRORS] = &eError;

    pszText = (PTSTR) MemAllocPtr ( GHND, GetBuildTextLength ( apvInitError,
                                                               apvParameters )
                                  );

    if ( pszText )
        {
                                        // Build text and show it through
                                        // UIFastError().

        BuildText ( apvInitError, apvParameters, pszText );

        UIFastError ( pszText );

                                        // Release allocated memory.

        MemFreePtr ( pszText );
        }
    else
        {
                                        // Report error.

        UIOutOfMemoryError();
        }

#elif defined(SYM_NTK)

                                        // Make sure eError has a valid NT
                                        // Event Log ID equivalent.

    SYM_ASSERT ( aUIINITERRORTranslator[eError] );

                                        // Translate eError into NT Event
                                        // Log ID and write it.

    NAVAPNTWriteStandardEventLog ( aUIINITERRORTranslator[eError] );

#endif
    }

//---------------------------------------------------------------------------
//
// VOID UIFastError (
//      PSTR pszMessage
//      );
//
// This function is called when NAVAP needs to display a message, but
// the NAVAPMessage() service is not available.  It displays the message at
// pszMessage and waits for a key to be pressed.
//
// UIFastError() is available only during driver initialization.
//
// Entry:
//      pszMessage - pointer to a NULL terminated string
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

VOID UIFastError (
    PSTR pszMessage
    )
    {
    auto UINT uScreenSizeX;
    auto UINT uScreenSizeY;
    auto PSTR pszCurrentLine;
    auto UINT uCurrentLineSize;
    auto UINT uCurrentLine;
    auto WORD wKeyPressed;
    auto UINT uKeyboardOwner, uMouseOwner;

                                        // Validate input parameters.

    SYM_ASSERT ( pszMessage );

                                        // Prepare devices.

    Begin_Critical_Section(0);

    uKeyboardOwner = _VKD_Get_Kbd_Owner();
    uMouseOwner    = _VMD_Get_Mouse_Owner();

    _VKD_Set_Kbd_Owner ( Get_Cur_VM_Handle() );

    _Begin_Message_Mode();

                                        // Clear screen and set colors.

    if ( DBCSInstalled() )
        {
                                        // Clear to colored bkgnd first to
                                        // work around bug in graphic
                                        // VDD_Msg_ClrScrn.

        _VDD_Msg_ClrScrn ( 1, &uScreenSizeX, &uScreenSizeY );
        }

    _VDD_Msg_ClrScrn ( 0, &uScreenSizeX, &uScreenSizeY );
    _VDD_Msg_BakColor ( 0 );
    _VDD_Msg_ForColor ( 7 );

                                        // Display text.

    uCurrentLine = 0;

    while ( *pszMessage != '\0' )
        {
        ParseTextLine ( &pszMessage,
                        &pszCurrentLine,
                        &uCurrentLineSize,
                        uScreenSizeX );

        if ( uCurrentLineSize && uCurrentLine < uScreenSizeY )
            {
            _VDD_Msg_TextOut ( pszCurrentLine,
                               uCurrentLineSize,
                               0,
                               uCurrentLine );

                                        // VDD_Msg_SetCursPos is broken for
                                        // graphical message mode.

            if ( !DBCSInstalled() )
                {
                _VDD_Msg_SetCursPos ( uCurrentLineSize, uCurrentLine );
                }
            }

        uCurrentLine++;
        }

                                        // Wait for a key to be pressed.

    ClearKeyboardQueue();

    while ( !GetKeyPressed ( &wKeyPressed ) );

                                        // Restore devices.

    _End_Message_Mode();

    _VKD_Set_Kbd_Owner ( uKeyboardOwner );

    _VMD_Set_Mouse_Owner ( uMouseOwner );

    End_Critical_Section();
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
//  VOID ParseTextLine (
//      PSTR  *ppszNextLine,
//      PSTR  *ppszLineStart,
//      PUINT puLineSize,
//      UINT  uMaxLineSize
//      );
//
// This function parses a text line.  It is available only during driver
// initialization, after this the code is discarded.
//
// Entry:
//      ppszNextLine  - a pointer to a variable with start of string
//      ppszLineStart - a pointer to a variable to store start of line
//      puLineSize    - pointer to a variable to store line size
//      uMaxLineSize  - maximum number of characters per line
//
// Exit:
//      *ppszNextLine  - pointer to the next line to be parsed
//      *ppszLineStart - start of line to be displayed
//      *puLineSize    - number of BYTES (note, not characters!) in the
//                       current line
//
// Note:
//      This function has several limitations (not bugs!) and because
//      of this the strings will not always be optimally parsed.  This
//      shouldn't really matter, because this function is used to parse
//      very few strings, and they will probably never show up.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

VOID ParseTextLine (
    PSTR  *ppszNextLine,
    PSTR  *ppszLineStart,
    PUINT puLineSize,
    UINT  uMaxLineSize
    )
    {
    auto PSTR pszPreWork;
    auto PSTR pszWork;
    auto PSTR pszLastWhiteSpace;
    auto UINT uLineSize;

                                        // Validate input parameters.

    SYM_ASSERT ( ppszNextLine );
    SYM_ASSERT ( *ppszNextLine );
    SYM_ASSERT ( ppszLineStart );
    SYM_ASSERT ( puLineSize );

                                        // Guarantee width of DB char.

    SYM_ASSERT ( uMaxLineSize > 2 );

                                        // Set the main work variable...

    pszWork = *ppszNextLine;

                                        // Skip white spaces, and this will
                                        // be the start of the current line.

    while ( *pszWork == ' ' )
        {
        pszWork = AnsiNext ( pszWork );
        }

    *ppszLineStart = pszWork;

                                        // Find line end and calculate size.

    uMaxLineSize--;
    uLineSize = 0;
    pszLastWhiteSpace = NULL;

    while ( *pszWork != '\0' &&
            *pszWork != '\n' &&
            uLineSize < uMaxLineSize )
        {

        if ( *pszWork == ' ' )
            {
            pszLastWhiteSpace = pszWork;
            }

        pszPreWork = pszWork;
        pszWork = AnsiNext ( pszWork );

        uLineSize += pszWork - pszPreWork;
        }

                                        // Backup if we hit the limit.  Make
                                        // sure we don't split a DB char.

    if ( pszLastWhiteSpace && uLineSize == uMaxLineSize  )
        {
        pszWork = pszLastWhiteSpace;
        }
    else if ( uLineSize > uMaxLineSize )
        {
        pszWork = pszPreWork;
        }

    *puLineSize = (UINT)pszWork - (UINT)*ppszLineStart;

                                        // Skip CR/LF

    if ( *pszWork == '\n' )
        {
        pszWork = AnsiNext ( pszWork );
        }

                                        // Set next line start

    *ppszNextLine = pszWork;
    }

#endif // #if defined(SYM_VXD)

//---------------------------------------------------------------------------
//
// All code from now on has to stay in memory for as long as the driver is
// loaded.
//
//---------------------------------------------------------------------------
#define DRIVER_DEFAULT_CODE_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// VOID UIControlledInitMessage (
//      UICONTROLLEDINITMESSAGE eMessage
//      );
//
// This function displays messages during controlled initialization of
// NAVAP.  In SYM_VXD, this function will display the messages using the
// NAVAPMessage() API.  In SYM_NTK, this function will log the messages to
// the NT Event Log.
//
// Entry:
//      eMessage - a member of UICONTROLLEDINITMESSAGE.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID UIControlledInitMessage (
    UICONTROLLEDINITMESSAGE eMessage
    )
    {
#if defined(SYM_VXD)

    auto PTSTR pszText, pszButtons;
    auto void  *apvParameters[_UI_PARAMETER_TABLE_SIZE];

                                        // Prepare string buffers and
                                        // parameters for building a
                                        // message.

    apvParameters[_UI_CONTROLLEDINIT_MESSAGES] = &eMessage;

    pszText = (PTSTR) MemAllocPtr (
                          GHND,
                          GetBuildTextLength ( apvControlledInitMessage,
                                               apvParameters ) );

    pszButtons = (PTSTR) MemAllocPtr (
                             GHND,
                             GetBuildButtonsLength ( fPROMPTACTION_OK ) );

                                        // Build and display message.

    if ( pszText && pszButtons )
        {
        BuildText ( apvControlledInitMessage, apvParameters, pszText );

        BuildButtons ( pszButtons, fPROMPTACTION_OK );

        NAVAPMessage ( &rControlledInitMessageBoxRect,
                       0,
                       AVACTION_OK,
                       AMB_COLOR_RED,
                       szControlledInitMessageTitle,
                       pszText,
                       NULL,
                       pszButtons );
        }
    else
        {
        UIOutOfMemoryError();
        }

                                        // Release allocated memory.

    if ( pszText )
        {
        MemFreePtr ( pszText );
        }

    if ( pszButtons )
        {
        MemFreePtr ( pszButtons );
        }

#elif defined(SYM_NTK)

                                        // Make sure eMessage has a valid NT
                                        // Event Log ID equivalent.

    SYM_ASSERT ( aUICONTROLLEDINITMESSAGETranslator[eMessage] );

                                        // Translate eMessage into NT Event
                                        // Log ID and write it.

    NAVAPNTWriteStandardEventLog (
        aUICONTROLLEDINITMESSAGETranslator[eMessage] );

#endif
    }

//---------------------------------------------------------------------------
//
// VOID UIOutOfMemoryError (
//      VOID
//      );
//
// This function should be called when the system runs out of memory.  It
// reports that NAVAP may not be able to function properly the first time
// it is called.  Consecutive calls will not report anything.
//
// Entry:
//      -
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID UIOutOfMemoryError (
    VOID
    )
    {
    if ( SymInterlockedExchange ( &lDisplayOutOfMemoryError, 0 ) )
        {
#if defined(SYM_VXD)

                                        // Display a system modal message.

        SHELL_SYSMODAL_Message (
            Get_Cur_VM_Handle(),
            MB_OK | MB_DEFBUTTON1 | MB_NOWINDOW | MB_SYSTEMMODAL,
            szOutOfMemoryError,
            szOutOfMemoryErrorTitle );

#elif defined(SYM_NTK)

                                        // Write a message into NT's Event
                                        // Log.

            NAVAPNTWriteStandardEventLog (
                aOutOfMemoryErrorMessageTranslator[0] );

#endif
        }
    }

//---------------------------------------------------------------------------
//
// VOID UIShutdownError (
//      UISHUTDOWNERROR eError
//      );
//
// This function is called when an error occured during system shutdown.
//
// Entry:
//      eError - MSG_Unable_To_Initiate_Shutdown or MSG_Shutdown_Aborted,
//               depending on the situation.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID UIShutdownError (
    UISHUTDOWNERROR eError
    )
    {
#if defined(SYM_VXD)

    auto void  **apvMessage;
    auto PTSTR pszText, pszButtons;

    switch ( eError )
        {
        case MSG_Unable_To_Initiate_Shutdown:

            apvMessage = apvUnableToInitiateShutdown;

            break;

        case MSG_Shutdown_Aborted:

            apvMessage = apvShutdownAborted;

            break;

        default:

            SYM_ASSERT ( FALSE );
        }

                                        // Prepare string buffers for
                                        // building a message.

    pszText = (PSTR) MemAllocPtr (
                        GHND,
                        GetBuildTextLength ( apvMessage, NULL ) );

    pszButtons = (PSTR) MemAllocPtr (
                            GHND,
                            GetBuildButtonsLength ( fPROMPTACTION_OK ) );

                                        // Build and display message.

    if ( pszText && pszButtons )
        {
        BuildText ( apvMessage, NULL, pszText );

        BuildButtons ( pszButtons, fPROMPTACTION_OK );

                                        // Display message.

        NAVAPMessage ( &rShutdownErrorMessageBoxRect,
                       0,
                       AVACTION_OK,
                       AMB_COLOR_RED,
                       szShutdownErrorMessageTitle,
                       pszText,
                       NULL,
                       pszButtons );
        }
    else
        {
        UIOutOfMemoryError();
        }

                                        // Release allocated memory.

    if ( pszText )
        {
        MemFreePtr ( pszText );
        }

    if ( pszButtons )
        {
        MemFreePtr ( pszButtons );
        }

#else

    SYM_ASSERT ( FALSE );

#endif
    }

//---------------------------------------------------------------------------
//
// VOID UIShowLongCustomAlert (
//      HEVENTPACKET hEventPacket,
//      SE_AMB_COLOR eColor
//      );
//
// Displays a long custom alert dialog if it exists.
//
// Entry:
//      hEventPacket - handle of an event packet, from which this function
//                     only uses the configuration.
//      eColor       - color to display the alert in.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID UIShowLongCustomAlert (
    HEVENTPACKET hEventPacket,
    SE_AMB_COLOR eColor
    )
    {
#if defined(SYM_VXD)

    auto PTSTR pszText, pszButtons;
    auto void  *apvMessage[] = { NULL, UI_END_OF_TABLE };

                                        // Do we really need to display
                                        // the alert?

    if ( prAPCfg(hEventPacket)->pszLongCustomAlert )
        {
                                        // Prepare string buffers for
                                        // building a message.

        apvMessage[0] = prAPCfg(hEventPacket)->pszLongCustomAlert;

        pszText = (PSTR) MemAllocPtr (
                             GHND,
                             GetBuildTextLength ( apvMessage, NULL ) );

        pszButtons = (PSTR) MemAllocPtr (
                                GHND,
                                GetBuildButtonsLength ( fPROMPTACTION_CONTINUE ) );

                                        // Build and display message.

        if ( pszText && pszButtons )
            {
            BuildText ( apvMessage, NULL, pszText );

            BuildButtons ( pszButtons, fPROMPTACTION_CONTINUE );

                                        // Display message.

            NAVAPMessage ( &rLongCustomAlertMessageBoxRect,
                           prAPCfg(hEventPacket)->uAlertTimeOut,
                           AVACTION_CONTINUE,
                           eColor,
                           szLongCustomAlertMessageTitle,
                           pszText,
                           NULL,
                           pszButtons );
            }
        else
            {
            UIOutOfMemoryError();
            }

                                        // Release allocated memory.

        if ( pszText )
            {
            MemFreePtr ( pszText );
            }

        if ( pszButtons )
            {
            MemFreePtr ( pszButtons );
            }
        }
#endif
    }

//---------------------------------------------------------------------------
//
// VOID UIGetVirusName (
//      HVLVIRUS hVLVirus,
//      PTSTR    pszVirusName
//      );
//
// Returns the name of the virus in hVLVirus.
//
// Entry:
//      hVLVirus     - a handle to virus
//      pszVirusName - buffer for virus name (must be SIZE_VIRUSNAME + 1
//                     characters long)
//
// Exit:
//      Returns virus name in pszVirusName.
//
//---------------------------------------------------------------------------
VOID UIGetVirusName (
    HVLVIRUS hVLVirus,
    PTSTR    pszVirusName
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( hVLVirus );
    SYM_ASSERT ( pszVirusName );

                                        // Get virus name.

    if ( !VLVirusInfo ( hVLVirus, 0, VI_VIRUS_NAME, pszVirusName ) )
        {
        STRCPY ( pszVirusName, szVirusNameError );
        }

    SYM_ASSERT ( STRLEN (pszVirusName) <= SIZE_VIRUSNAME );
    }

//---------------------------------------------------------------------------
//
// BOOL UIGetUserInfo (
//      LPED                     prEventData,
//      PNAVAP_USERINFOSUBPACKET prUserInfo
//      );
//
// This function places information for the current user in prUserInfo.
//
// Entry:
//      prEventData - pointer to an event data structure.  If this value
//                    is not NULL, and the event data structure is a valid
//                    NAVAP SE_FS_ event, then the OFD of the file object
//                    associated with prEventData is examined for SID.
//      prUserInfo  - pointer to a packet to which this function will copy
//                    user's info.  If the current platform supports SIDs,
//                    and if the current user has a SID, then its value will
//                    be returned.  Otherwise, a user name will be returned.
//
// Exit:
//      TRUE  user info was found and copied to prUserInfo.
//      FALSE user info was not found or there was an error while
//            retrieving.
//            prUserInfo contains empty user ID.
//
//---------------------------------------------------------------------------
BOOL UIGetUserInfo (
    LPED                     prEventData,
    PNAVAP_USERINFOSUBPACKET prUserInfo
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( prUserInfo );
    SYM_VERIFY_BUFFER ( prUserInfo, sizeof(*prUserInfo) );

                                        // Reset all members at *prUserInfo.

    memset ( prUserInfo, 0, sizeof(*prUserInfo) );

#if defined(SYM_NTK)

                                        // Return success if we have a
                                        // valid event data structure,

    if ( prEventData &&

                                        // ... which is a member of the SE_FS
                                        // events which NAVAP processes,

         ( prEventData->SE_ED_etEventType == SE_FS_FILE_OPEN ||
           prEventData->SE_ED_etEventType == SE_FS_FILE_CREATE ||
           prEventData->SE_ED_etEventType == SE_FS_FILE_CLOSE ) &&

                                        // ... and we successfully extract
                                        // a SID out of it.

         FileMonitorQuerySavedUserSID ( prEventData, &prUserInfo->rUserInfo.Sid )
       )
        {
        return ( prUserInfo->bUseAsSIDInsteadOfString = TRUE );
        }

                                        // Return success if we can acquire
                                        // current user's SID.

    memset ( prUserInfo, 0, sizeof(*prUserInfo) );

    if ( UIGetCurrentUserSID ( &prUserInfo->rUserInfo.Sid,
                               sizeof(prUserInfo->rUserInfo) ) != 0 )
        {
        return ( prUserInfo->bUseAsSIDInsteadOfString = TRUE );
        }

                                        // Clean up the target structure,
                                        // thus invalidating the user ID.

    memset ( prUserInfo, 0, sizeof(*prUserInfo) );

    return ( FALSE );

#elif defined(SYM_VXD)

    auto BOOL  bResult;
    auto DWORD /* HKEY */ hKey;
    auto DWORD dwRegKeyType;
    auto ULONG ulOutBufferLength;
    auto char  szBuffer[sizeof(prUserInfo->rUserInfo) / sizeof(WCHAR)];

                                        // Initialize local variables.

    bResult = FALSE;

    dwRegKeyType = REG_SZ;

    ulOutBufferLength = sizeof(szBuffer);

                                        // Read the registry entry for
                                        // current user.

    if ( RegOpenKey ( HKEY_LOCAL_MACHINE,
                      szCurrentUserKey,
                      &hKey ) == ERROR_SUCCESS )
        {
        if ( RegQueryValueEx ( hKey,
                               szCurrentUserKeyValueName,
                               NULL,
                               &dwRegKeyType,
                               szBuffer,
                               &ulOutBufferLength ) == ERROR_SUCCESS )
            {
                                        // If there wasn't buffer overflow,
                                        // then convert the name to Unicode.

            SYM_VERIFY_STRING ( szBuffer );

            if ( strlen ( szBuffer ) < sizeof(szBuffer) )
                {
                if ( OemToCharW ( szBuffer, prUserInfo->rUserInfo.szUserID ) )
                    {
                    bResult = TRUE;
                    }
                else
                    {
                    SYM_ASSERT ( FALSE );
                    }
                }
            else
                {
                SYM_ASSERT ( FALSE );
                }
            }

        if ( RegCloseKey ( hKey ) != ERROR_SUCCESS )
            {
            SYM_ASSERT ( FALSE );
            }
        }

                                        // Return to caller.

    return ( bResult );

#endif
    }

//---------------------------------------------------------------------------
//
// DWORD UIGetCurrentUserSID (
//      PSID  pSID,
//      DWORD cbSID
//      );
//
// This function returns the SID of the user that is being impersonated by
// the current thread/process.
//
// Entry:
//      pSID  - buffer to receive SID.
//      cbSID - length of buffer pointed to by pSID.
//
// Exit:
//      Returns the number of bytes copied to pSID.  If the function failed
//      due to missing SID, or due to too small buffer, the return value
//      will be 0.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

DWORD UIGetCurrentUserSID (
    PSID  pSID,
    DWORD cbSID
    )
    {
    auto DWORD       cbResult;
    auto HANDLE      hToken;
    auto DWORD       cbLength;
    auto PTOKEN_USER prTokenUser;

                                        // Validate input parameters

    SYM_ASSERT ( pSID );
    SYM_ASSERT ( cbSID );
    SYM_VERIFY_BUFFER ( pSID, cbSID );

                                        // Initialize local variables

    cbResult = 0;

                                        // Open handle to the token for the
                                        // current thread or process.

    if ( NT_SUCCESS ( ZvOpenThreadToken ( NtCurrentThread(),
                                          TOKEN_QUERY,
                                          TRUE,
                                          &hToken ) ) ||

         NT_SUCCESS ( ZvOpenProcessToken ( NtCurrentProcess(),
                                           TOKEN_QUERY,
                                           &hToken ) )
       )
        {
                                        // Get the size of token user
                                        // information.

        cbLength = 0;

        if ( ZvQueryInformationToken ( hToken,
                                       TokenUser,
                                       NULL,
                                       0,
                                       &cbLength ) == STATUS_BUFFER_TOO_SMALL &&

                                        // Make sure the returned value is
                                        // valid.

             cbLength &&

                                        // Allocate memory for the user
                                        // information.

             ( prTokenUser = (PTOKEN_USER) MemAllocPtr ( GHND, cbLength ) )
           )
            {
                                        // Get token user information.

            if ( ZvQueryInformationToken ( hToken,
                                           TokenUser,
                                           prTokenUser,
                                           cbLength,
                                           &cbLength ) == STATUS_SUCCESS )
                {
                                        // Now that we have the token user
                                        // information in our buffer, extract
                                        // the SID out of it.  We assume that
                                        // Microsoft keeps the format of the
                                        // token user information unchanged
                                        // from one rev of NTOS to another.
                                        // Under normal circumstances, we
                                        // would not rely on that, but since
                                        // the same structure is exported to
                                        // all Win32 applications, we can
                                        // give ourselves some slack.  With
                                        // the simple check below we will
                                        // ensure that the OS put the SID
                                        // right after the TOKEN_USER
                                        // structure, and that the pointer to
                                        // it is really valid.

                if ( (ULONG)prTokenUser->User.Sid == (ULONG)prTokenUser + sizeof(TOKEN_USER) &&
                     (ULONG)prTokenUser->User.Sid < (ULONG)prTokenUser + cbLength )
                    {
                    if ( cbSID >= cbLength - sizeof(TOKEN_USER) )
                        {
                                        // Copy the SID into caller's
                                        // buffer.

                        cbResult = cbLength - sizeof(TOKEN_USER);

                        memcpy ( pSID, prTokenUser->User.Sid, cbResult );
                        }
                    }
                else
                    {
                    SYM_ASSERT ( FALSE );
                    }
                }

            MemFreePtr ( prTokenUser );
            }
        else
            {
            SYM_ASSERT ( FALSE );
            }

        ZvClose ( hToken );
        }

    return ( cbResult );
    }

#endif // #if defined(SYM_NTK)
