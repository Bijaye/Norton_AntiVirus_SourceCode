// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APKERNEL/VCS/FMOVEREN.CPv   1.8   08 Jun 1998 21:10:10   RStanev  $
//
// Description:
//      This file defines Kernel functions for moving and renaming
//      infected files.
//
// Contains:
//      MoveInfectedFile()
//      RenameInfectedFile()
//      CreateTargetDirectory()             (local function)
//
// See Also:
//      taskscan.cpp
//
// Note:
//      Kernel module
//
//***************************************************************************
// $Log:   S:/APKERNEL/VCS/FMOVEREN.CPv  $
// 
//    Rev 1.8   08 Jun 1998 21:10:10   RStanev
// Removed references to the backup extensions used by NAV.
//
//    Rev 1.7   18 Feb 1998 21:17:58   RStanev
// Removed unknown virus detection.
//
//    Rev 1.6   17 Feb 1998 20:25:06   RStanev
// Updated copyright.
//
//    Rev 1.5   28 May 1997 20:58:00   RStanev
// Added CreateTargetDirectory().
//
//    Rev 1.4   28 May 1997 12:54:18   RStanev
// Forgot to add MoveChangedFile() to the header of this file.
//
//    Rev 1.3   28 May 1997 12:52:54   RStanev
// Added MoveChangedFile().
//
//    Rev 1.2   15 May 1997 01:51:38   RStanev
// Fixed compilation breakage due to change in prototype of VLMoveFile().
//
//    Rev 1.1   29 Apr 1997 12:04:06   RStanev
// Fixed compilation errors introduced by AVENGE changes.
//
//    Rev 1.0   26 Feb 1997 16:39:42   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "xapi.h"
#include "navap.h"
#include "apkernel.h"

//---------------------------------------------------------------------------
//
// Prototype local functions.
//
//---------------------------------------------------------------------------
VOID CreateTargetDirectory (
    PCTSTR pszDirectory
    );

//---------------------------------------------------------------------------
//
// BOOL MoveInfectedFile (
//     HEVENTPACKET hEventPacket,
//     PHVLVIRUS    phVLVirus
//     );
//
// This function moves an infected file.  If the function succeeds,
// hEventPacket->prEDExtra->pszBackupFile will contain the name of the
// moved file, otherwise it will be left as an empty string.
//
// Entry:
//      hEventPacket - Event Packet handle
//      phVLVirus    - Pointer to a variable containing the handle to
//                     the virus detected in the file described by
//                     hEventPacket.
//
// Exit:
//      TRUE if operation successful, or FALSE if error.
//
//---------------------------------------------------------------------------
BOOL MoveInfectedFile (
    HEVENTPACKET hEventPacket,
    PHVLVIRUS    phVLVirus
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( prAPCfg(hEventPacket)->pszMoveLocation );
    SYM_ASSERT ( hEventPacket->prEDExtra->pszBackupFile );
    SYM_ASSERT ( !*hEventPacket->prEDExtra->pszBackupFile );
    SYM_ASSERT ( phVLVirus );
    SYM_ASSERT ( *phVLVirus );

                                        // Ensure the target directory
                                        // exists.

    CreateTargetDirectory ( prAPCfg(hEventPacket)->pszMoveLocation );

                                        // Move the infected file.  Note,
                                        // that since the backup extension
                                        // has been moved out of the config
                                        // file, we will hard code it here!

    if ( VLMoveFile ( *phVLVirus,
                      prAPCfg(hEventPacket)->pszMoveLocation,
                      _T("RAD"),
                      hEventPacket->prEDExtra->pszBackupFile ) == VS_OK )
        {
        return ( TRUE );
        }

    *hEventPacket->prEDExtra->pszBackupFile = EOS;

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL RenameInfectedFile (
//     HEVENTPACKET hEventPacket,
//     PHVLVIRUS    phVLVirus
//     );
//
// This function renames an infected file.  If the function succeeds,
// hEventPacket->prEDExtra->pszBackupFile will contain the name of the
// renamed file, otherwise it will be left as an empty string.
//
// Entry:
//      hEventPacket - Event Packet handle
//      phVLVirus    - Pointer to a variable containing the handle to
//                     the virus detected in the file described by
//                     hEventPacket.
//
// Exit:
//      TRUE if operation successful, or FALSE if error.
//
//---------------------------------------------------------------------------
BOOL RenameInfectedFile (
    HEVENTPACKET hEventPacket,
    PHVLVIRUS    phVLVirus
    )
    {
    auto int   iExtensionLength;
    auto TCHAR szTargetFile[SYM_MAX_PATH];

                                        // Validate input parameters.

    SYM_ASSERT ( hEventPacket );
    SYM_ASSERT ( hEventPacket->prEDExtra );
    SYM_ASSERT ( hEventPacket->prEDExtra->pszBackupFile );
    SYM_ASSERT ( !*hEventPacket->prEDExtra->pszBackupFile );
    SYM_ASSERT ( phVLVirus );
    SYM_ASSERT ( *phVLVirus );

                                        // First, create a local copy of the
                                        // infected filename.

    STRCPY ( szTargetFile, EDXpszOEMFile(hEventPacket->prEDExtra) );

                                        // Then, strip the extension.

    NameStripExtension ( szTargetFile );

                                        // Check if appending the rename
                                        // extension will not overflow our
                                        // buffer.

    iExtensionLength = STRLEN ( prAPCfg(hEventPacket)->pszRenameExtension );

    if ( STRLEN ( szTargetFile ) +
         ( iExtensionLength ? iExtensionLength +
                              STRLEN ( _T(".") ) : 0 ) >=
         SYM_MAX_PATH )
        {
        *hEventPacket->prEDExtra->pszBackupFile = EOS;

        return ( FALSE );
        }

                                        // Append the rename extension.

    NameAppendExtension ( szTargetFile,
                          prAPCfg(hEventPacket)->pszRenameExtension );

                                        // Now rename the infected file.

    if ( VLRenameFile ( *phVLVirus,
                        szTargetFile,
                        hEventPacket->prEDExtra->pszBackupFile ) == VS_OK )
        {
        return ( TRUE );
        }

    *hEventPacket->prEDExtra->pszBackupFile = EOS;

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// VOID CreateTargetDirectory (
//      PCTSTR pszDirectory
//      );
//
// This function does its best to ensures that the directory specification
// at pszDirectory exists.
//
// Entry:
//      pszDirectory - fully qualified path name.  SYM_VXD: in OEM.
//
// Exit:
//      -
//
//---------------------------------------------------------------------------
VOID CreateTargetDirectory (
    PCTSTR pszDirectory
    )
    {
    auto PTSTR pszPath, pszIndex;
    auto BOOL  bCharReplaced;

                                        // Validate input parameters.

    SYM_ASSERT ( pszDirectory );
    SYM_VERIFY_STRING ( pszDirectory );

    if ( ! NameIsUNC ( pszDirectory ) &&
         ! ( NameHasDriveSpec ( pszDirectory ) &&
             pszDirectory[2] == _T('\\') )
       )
        {
        SYM_ASSERT ( FALSE );

        return;
        }

                                        // Allocate memory for converting
                                        // VxD strings.

#if defined(SYM_VXD)

    auto PTSTR pszOSPath;

    if ( ! ( pszOSPath = (PTSTR)
             MemAllocPtr ( GMEM_MOVEABLE, sizeof(WCHAR) *
                                          ( STRLEN ( pszDirectory ) + 1 ) ) )
       )
        {
        UIOutOfMemoryError();

        return;
        }

#else

    #define pszOSPath pszPath

#endif
                                        // Allocate memory for parsing
                                        // buffer and create name for
                                        // parsing.

    if ( pszPath = (PTSTR)
         MemAllocPtr ( GMEM_MOVEABLE, sizeof(TCHAR) *
                                      ( STRLEN ( pszDirectory ) + 1 ) )
       )
        {
        STRCPY ( pszPath, pszDirectory );

                                        // Determine the beginning of the
                                        // string we will parse.

        if ( NameIsUNC ( pszPath ) )
            {
            for ( pszIndex = pszPath + 2;
                  *pszIndex && *pszIndex != _T('\\');
                  pszIndex = CharNext ( pszIndex ) );

            if ( *pszIndex )
                {
                pszIndex = CharNext ( pszIndex );
                }
            }
        else
            {
            pszIndex = pszPath + 3;
            }

                                        // Parse the string, creating
                                        // directory for each segment.
                                        // Ignore any error codes generated
                                        // by DirMake(), because chances are
                                        // the directory already exists.

        do
            {
            for ( ;
                  *pszIndex && *pszIndex != _T('\\');
                  pszIndex = CharNext ( pszIndex ) );

            if ( bCharReplaced = *pszIndex )
                {
                *pszIndex = _T('\0');
                }

#if defined(SYM_VXD)

            VxDGenericStringConvert ( pszPath, CHARSET_OEM,
                                      pszOSPath, CHARSET_CURRENT );

#endif // #if defined(SYM_VXD)

            DirMake ( pszOSPath );

            if ( bCharReplaced )
                {
                *pszIndex = _T('\\');

                pszIndex = CharNext ( pszIndex );
                }

            } while ( *pszIndex );

                                        // Release allocated memory.

        MemFreePtr ( pszPath );
        }
    else
        {
        UIOutOfMemoryError();
        }

#if defined(SYM_VXD)

    MemFreePtr ( pszOSPath );

#else

    #undef pszOSPath

#endif
    }
