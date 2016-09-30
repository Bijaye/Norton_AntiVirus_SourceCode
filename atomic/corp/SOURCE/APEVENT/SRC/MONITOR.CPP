// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APEVENT/VCS/MONITOR.CPv   1.4   18 Feb 1998 22:13:28   RStanev  $
//
// Description:
//      This file defines file monitor functions.
//
// Contains:
//      FileMonitorInit()
//      FileMonitorDeInit()
//      FileMonitorEnable()
//      FileMonitorDisable()
//
//      SYMEvent_GetOpenFileDataPtrEx()     (macro or inline function)
//
//      FileMonitorGetAttributes()
//      FileMonitorSetAttributes()
//      FileMonitorClearAttributes()
//
//      FileMonitorSaveCurrentUserSID()     (SYM_NTK only)
//      FileMonitorQuerySavedUserSID()      (SYM_NTK only)
//
//      FileMonitorGetFileID()              (SYM_NTK only)
//      FileMonitorSetFileID()              (SYM_NTK only)
//
//      FileMonitorTransferData()
//
// See Also:
//      monitor.h
//
// Note:
//      Event Manager module.
//
//***************************************************************************
// $Log:   S:/APEVENT/VCS/MONITOR.CPv  $
// 
//    Rev 1.4   18 Feb 1998 22:13:28   RStanev
// Removed unknown virus detection.
//
//    Rev 1.3   17 Feb 1998 16:55:50   RStanev
// Updated copyright.
//
//    Rev 1.2   16 May 1997 01:57:44   RStanev
// Changed the prototype of FileMonitorExchangeInocInfoPtr().
//
//    Rev 1.1   15 May 1997 01:40:16   RStanev
// Too many changes to describe.
//
//    Rev 1.0   26 Feb 1997 04:34:20   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "symsync.h"
#include "navap.h"
#include "monitor.h"

//---------------------------------------------------------------------------
//
// File Monitor data structure.  This structure is allocated for each
// monitored file.
//
// If this structure, or any of the information it refers to changes, then
// a new unique SEPDID should be assigned for allocating persistent data.
//
// Note: For Atomic we are removing unknown virus detection, which makes the
//       lpFileProtect field obsolete.  I don't have time to change the
//       SEPDID for Atomic, so this field will be left dead here...  If
//       someone feels like doing the right thing, please be my guest.
//
//---------------------------------------------------------------------------
typedef struct
    {
    UINT    uAttributes;                // Monitoring flags.
    LPFP    lpFileProtect;              // Pointer to File protect info.
#if defined(SYM_NTK)
    __int64 i64FileID;                  // File ID.
    DWORD   dwSIDLength;                // Length of SID at abySID (0 if no SID).
    BYTE    abySID[1];                  // SID of whoever opened the file.
#endif
    } FILEMONITORDATA, *PFILEMONITORDATA;

//---------------------------------------------------------------------------
//
// hOFData is a handle used by the file monitor, and it allows data to be
// be saved on per open file basis.  This handle is allocated by calling
// SYMEvent, and is maintained by SYMEvent.
//
//---------------------------------------------------------------------------
HOFDATA hOFData = NULL;

UINT uFileMonitorDataLength = 0;

//---------------------------------------------------------------------------
//
// bResourceSensitiveAPIsAreEnabled is a flag used to control the resource
// sensitive APIs.  It is protected by SymAcquireFastGlobalLock().
//
//---------------------------------------------------------------------------
BOOL bResourceSensitiveAPIsAreEnabled = FALSE;

//---------------------------------------------------------------------------
//
// BOOL FileMonitorInit(
//      VOID
//      );
//
// This function initializes the file monitor.  It checks NAVAP's persistent
// data area if there is already a per open file data allocated, and uses
// its handle if it does.  If there isn't, then a new handle is allocated
// and saved in NAVAP's persistent data area.
//
// Call FileMonitorInit() before using any other file monitor functions.
//
// NAVAP persistent data area must be allocated and available in
// prNAVAPPersistentData before calling FileMonitorInit().
//
// Entry:
//      -
//
// Exit:
//      Returns TRUE if successful, FALSE if per open file data allocation
//      failed.
//
// Note:
//      SYM_NTK: The caller must ensure that if per open file data was
//      allocated and saved in NAVAP's persistent data area prior to calling
//      FileMonitorInit(), the length of the SID allocated for that data is
//      longer or equal to the length of the SID that this session of NAVAP
//      will use (rNAVAPInternalCfg.dwSIDLength).
//
// Synchronization:
//      This function is non-reentrant and should not be called more than
//      once.
//
//---------------------------------------------------------------------------
#define DRIVER_INIT_CODE_SEGMENT
#include "drvseg.h"

BOOL FileMonitorInit (
    VOID
    )
    {
                                        // Make sure we are called properly.

    SYM_ASSERT ( prNAVAPPersistentData );
    SYM_ASSERT ( !hOFData );

#if defined(SYM_NTK)

                                        // Calculate the total length of
                                        // open file data needed for the
                                        // file monitor.

    uFileMonitorDataLength = sizeof ( FILEMONITORDATA ) +
                             rNAVAPInternalCfg.dwSIDLength;

                                        // If we haven't yet allocated data
                                        // per open file handle, then do it
                                        // now.

    if ( !prNAVAPPersistentData->hOFData )
        {
        if ( prNAVAPPersistentData->hOFData = SYMEvent_AllocOpenFileData (
                                                  uFileMonitorDataLength )
           )
            {
            prNAVAPPersistentData->dwOFDataSIDLength =
                rNAVAPInternalCfg.dwSIDLength;
            }
        }
    else
        {
                                        // Assert that the SID used by the
                                        // current NAVAP session is going to
                                        // fit in the SID buffer reserved in
                                        // our pre-allocated open file data
                                        // area.  If this assert ever fails,
                                        // this means that the caller is not
                                        // following the instructions for
                                        // calling FileMonitorInit().

        SYM_ASSERT ( prNAVAPPersistentData->dwOFDataSIDLength >=
                     rNAVAPInternalCfg.dwSIDLength );
        }

#else

                                        // Calculate the total length of
                                        // open file data needed for the
                                        // file monitor.

    uFileMonitorDataLength = sizeof ( FILEMONITORDATA );

                                        // If we haven't yet allocated data
                                        // per open file handle, then do it
                                        // now.

    if ( !prNAVAPPersistentData->hOFData )
        {
        prNAVAPPersistentData->hOFData = SYMEvent_AllocOpenFileData (
                                        uFileMonitorDataLength );
        }

#endif

                                        // Return the status of processing.

    return ( ( hOFData = prNAVAPPersistentData->hOFData ) ? TRUE : FALSE );
    }

#define DRIVER_DEFAULT_CODE_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// BOOL FileMonitorDeInit (
//      VOID
//      );
//
// This function uninitializes the file monitor module.  Before calling
// FileMonitorDeInit(), you must make sure FileMonitorDisable() is called to
// release all file monitor resources allocated at run-time.
//
// Entry:
//      -
//
// Exit:
//      Returns always TRUE.
//
// Synchronization:
//      This function is non-reentrant.
//
//---------------------------------------------------------------------------
BOOL FileMonitorDeInit (
    VOID
    )
    {
                                        // Don't really need to do anything
                                        // important.  Just report success.

    return ( TRUE );
    }

//---------------------------------------------------------------------------
//
// VOID FileMonitorEnable (
//      VOID
//      );
//
// This function enables the file monitor module.  No "real" file monitor
// functions should be called before this function completes (which means
// it should be called before any event hooks are installed).
//
// FileMonitorEnable() cleans up the open file data areas of all currently
// open files.  This ensures that when the event hooks get installed and
// enabled, all monitoring starts from scratch and there is no false junk
// laying around.
//
// This function serves a very important role in keeping track of "resource
// sensitive" data saved in OFD areas and syncronizing the resource
// sensitive APIs with FileMonitorDisable().  The resource sensitive APIs
// save data in the OFD area, and the data need to be specifically release
// (in other words, just clearing of the OFD areas will create resource
// leaks).  Such data is released by FileMonitorDisable().
//
// Entry:
//      -
//
// Exit:
//      -
//
// Note:
//      The only resource sensitive API was FileMonitorExchangeInocInfoPtr()
//      until it was removed for Atomic.
//
// Synchronization:
//      This function is non-reentrant.
//
//---------------------------------------------------------------------------
VOID FileMonitorEnable (
    VOID
    )
    {
    auto PVOID prFileMonitorData;

                                        // Clear all open file data areas.

    prFileMonitorData = SYMEvent_GetOpenFileFirstNext ( hOFData, NULL );

    while ( prFileMonitorData )
        {
        memset ( prFileMonitorData, 0, sizeof(FILEMONITORDATA) );

        prFileMonitorData = SYMEvent_GetOpenFileFirstNext (
                                hOFData,
                                prFileMonitorData );
        }

                                        // Enable the resource sensitive
                                        // file monitor APIs.  Note, that we
                                        // don't need to acquire the
                                        // protection of the BOOL variable,
                                        // because there is nobody currently
                                        // using the APIs (it's part of this
                                        // function's specification).

    bResourceSensitiveAPIsAreEnabled = TRUE;
    }

//---------------------------------------------------------------------------
//
// VOID FileMonitorDisable (
//      VOID
//      );
//
// This function disables the file monitor and at the same time releases
// all "resource sensitive" data saved in OFD areas.
//
// Entry:
//      -
//
// Exit:
//      -
//
// Synchronization:
//      This function is non-reentrant.
//
//---------------------------------------------------------------------------
VOID FileMonitorDisable (
    VOID
    )
    {
    auto PFILEMONITORDATA prFileMonitorData;

    SymAcquireFastGlobalLock();

                                        // Free the resource sensitive data
                                        // in all OFD areas.

    prFileMonitorData = (PFILEMONITORDATA)
        SYMEvent_GetOpenFileFirstNext ( hOFData, NULL );

    while ( prFileMonitorData )
        {
                                        // >>> PUT RELEASE CODE HERE <<<

        prFileMonitorData = (PFILEMONITORDATA)
            SYMEvent_GetOpenFileFirstNext ( hOFData, prFileMonitorData );
        }

                                        // Disable the resource sensitive
                                        // file monitor APIs.

    bResourceSensitiveAPIsAreEnabled = FALSE;

    SymReleaseFastGlobalLock();
    }

//---------------------------------------------------------------------------
//
// LPVOID inline SYMEvent_GetOpenFileDataPtrEx (
//      HOFDATA hData,
//      LPED    lpED
//      );
//
// This function should be used to query the OFD whenever the client may be
// using a custom created event data packet.
//
// Entry:
//      hData - handle to per open file data.
//      lpED  - pointer to an event data structure.  If custom-created
//              structure, read the FileMonitorTransferData() notes for
//              more information.
//
// Exit:
//      Returns a pointer to OFD associated with lpED.
//
// Note:
//      Currently FileMonitorTransferData() is not used, therefore this
//      inline function is replaced by a macro which translates directly
//      to SYMEvent_GetOpenFileDataPtr().  If FileMonitorTransferData()
//      is eventually going to be used, comment out the #define line below,
//      but make sure that FileMonitorEnable() and FileMonitorDisable()
//      perform proper cleanup!
//
//---------------------------------------------------------------------------
#define SYMEvent_GetOpenFileDataPtrEx SYMEvent_GetOpenFileDataPtr

#if !defined(SYMEvent_GetOpenFileDataPtrEx)

LPVOID inline SYMEvent_GetOpenFileDataPtrEx (
    HOFDATA hData,
    LPED    lpED
    )
    {
    SYM_ASSERT ( hData );
    SYM_ASSERT ( hData == hOFData );
    SYM_ASSERT ( lpED );

    if ( lpED->SE_ED_efEventFlags & SE_FLAGS_SYNC )
        {
        return ( SYMEvent_GetOpenFileDataPtr ( hData, lpED ) );
        }
    else
        {
        SYM_ASSERT ( lpED->SE_ED_lpDTA );

        return ( lpED->SE_ED_lpDTA );
        }
    }

#endif // #if !defined(SYMEvent_GetOpenFileDataPtrEx)

//---------------------------------------------------------------------------
//
// BOOL FileMonitorGetAttributes (
//      LPED   lpEventData,
//      LPUINT lpuAttributes
//      );
//
// This function returns the File Monitor attributes for a file.  If the
// file isn't being monitored, it will return 0 in *lpuAttributes.
//
// Entry:
//      lpEventData   - pointer to event data structure
//      lpuAttributes - pointer to variable to receive the monitoring
//                      attributes
//
// Exit:
//      Returns TRUE if successful, else FALSE.  If TRUE, then
//      *lpuAttributes is set to the monitoring attributes.
//
//---------------------------------------------------------------------------
BOOL FileMonitorGetAttributes (
    LPED   lpEventData,
    LPUINT lpuAttributes
    )
    {
    auto PFILEMONITORDATA prFileMonitorData;

                                        // Validate input parameters.

    SYM_ASSERT ( hOFData );
    SYM_ASSERT ( lpEventData );
    SYM_ASSERT ( lpuAttributes );

                                        // Prepare result in case we fail.

    *lpuAttributes = 0;

                                        // Get attributes.

    prFileMonitorData = (PFILEMONITORDATA)
        SYMEvent_GetOpenFileDataPtrEx ( hOFData, lpEventData );

    SYM_ASSERT ( prFileMonitorData );

    if ( prFileMonitorData )
        {
        *lpuAttributes = prFileMonitorData->uAttributes;

        return ( TRUE );
        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL FileMonitorSetAttributes (
//      LPED lpEventData,
//      UINT uAttributes
//      );
//
// This function sets specific bits of the file monitoring attributes.
//
// Entry:
//      lpEventData - pointer to event data structure
//      uAttributes - bits to set
//
// Exit:
//      Returns TRUE if successful, else FALSE.
//
//---------------------------------------------------------------------------
BOOL FileMonitorSetAttributes (
    LPED lpEventData,
    UINT uAttributes
    )
    {
    auto PFILEMONITORDATA prFileMonitorData;

                                        // Validate input parameters.

    SYM_ASSERT ( hOFData );
    SYM_ASSERT ( lpEventData );

                                        // Set attributes.

    prFileMonitorData = (PFILEMONITORDATA)
        SYMEvent_GetOpenFileDataPtrEx ( hOFData, lpEventData );

    SYM_ASSERT ( prFileMonitorData );

    if ( prFileMonitorData )
        {
        SymAcquireFastGlobalLock();

        prFileMonitorData->uAttributes |= uAttributes;

        SymReleaseFastGlobalLock();

        return ( TRUE );
        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL FileMonitorClearAttributes (
//      LPED lpEventData,
//      UINT uAttributes
//      );
//
// This function clears specific bits of the file monitoring attributes.
//
// Entry:
//      lpEventData - pointer to event data structure
//      uAttributes - bits to clear
//
// Exit:
//      Returns TRUE if successful, else FALSE.
//
//---------------------------------------------------------------------------
BOOL FileMonitorClearAttributes (
    LPED lpEventData,
    UINT uAttributes
    )
    {
    auto PFILEMONITORDATA prFileMonitorData;

                                        // Validate input parameters.

    SYM_ASSERT ( hOFData );
    SYM_ASSERT ( lpEventData );

                                        // Set attributes.

    prFileMonitorData = (PFILEMONITORDATA)
        SYMEvent_GetOpenFileDataPtrEx ( hOFData, lpEventData );

    SYM_ASSERT ( prFileMonitorData );

    if ( prFileMonitorData )
        {
        SymAcquireFastGlobalLock();

        prFileMonitorData->uAttributes &= ~uAttributes;

        SymReleaseFastGlobalLock();

        return ( TRUE );
        }

    return ( FALSE );
    }

//---------------------------------------------------------------------------
//
// BOOL FileMonitorSaveCurrentUserSID (
//      LPED lpEventData
//      );
//
// This function saves the SID of the current user (the user who is being
// impersonated by the current thread or process) in the per open file data
// area for the file at lpEventData.
//
// Entry:
//      lpEventData - pointer to an event data structure (of an open/create
//                    event) which will be used to address the open-file
//                    data of a file object.  The current user's SID will be
//                    associated with that file object.
//
// Exit:
//      Returns TRUE if successful, else FALSE.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

BOOL FileMonitorSaveCurrentUserSID (
    LPED lpEventData
    )
    {
    auto PFILEMONITORDATA prFileMonitorData;

                                        // Validate input parameters.

    SYM_ASSERT ( hOFData );
    SYM_ASSERT ( lpEventData );
    SYM_ASSERT ( lpEventData->SE_ED_etEventType == SE_FS_FILE_OPEN ||
                 lpEventData->SE_ED_etEventType == SE_FS_FILE_CREATE );

                                        // Get pointer to the current file
                                        // data.

    prFileMonitorData = (PFILEMONITORDATA)
        SYMEvent_GetOpenFileDataPtrEx ( hOFData, lpEventData );

    SYM_ASSERT ( prFileMonitorData );

                                        // Return success if we have a valid
                                        // per open file data,

    if ( prFileMonitorData &&

                                        // ... and if we successfully acquire
                                        // the SID.

        ( prFileMonitorData->dwSIDLength =
              UIGetCurrentUserSID ( (PSID)prFileMonitorData->abySID,
                                    rNAVAPInternalCfg.dwSIDLength )
        )
       )
        {
        return ( TRUE );
        }

    return ( FALSE );
    }

#endif

//---------------------------------------------------------------------------
//
// DWORD FileMonitorQuerySavedUserSID (
//      LPED lpEventData,
//      PSID pSID
//      );
//
// This function returns the SID stored by FileMonitorSaveCurrentUserSID()
// at the per open file data addressable through lpEventData.
//
// Entry:
//      lpEventData - pointer to an event data structure generated upon a
//                    file system operation on the file who's per open file
//                    data contains the SID.
//      pSID        - pointer to a buffer to receive SID
//                    (at least rNAVAPInternalCfg.dwSIDLength bytes long)
//
// Exit:
//      Returns length of SID written to pSID.  0 if error or if SID not
//      present.
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

DWORD FileMonitorQuerySavedUserSID (
    LPED lpEventData,
    PSID pSID
    )
    {
    auto PFILEMONITORDATA prFileMonitorData;

                                        // Validate input parameters.

    SYM_ASSERT ( hOFData );
    SYM_ASSERT ( lpEventData );
    SYM_ASSERT ( lpEventData->SE_ED_etEventType == SE_FS_FILE_OPEN ||
                 lpEventData->SE_ED_etEventType == SE_FS_FILE_CREATE ||
                 lpEventData->SE_ED_etEventType == SE_FS_FILE_CLOSE );
    SYM_ASSERT ( pSID );
    SYM_VERIFY_BUFFER ( pSID, rNAVAPInternalCfg.dwSIDLength );

                                        // Get pointer to the current file
                                        // data.

    prFileMonitorData = (PFILEMONITORDATA)
        SYMEvent_GetOpenFileDataPtrEx ( hOFData, lpEventData );

    SYM_ASSERT ( prFileMonitorData );

                                        // Copy the SID if we have a valid
                                        // per open file data with a SID.

    if ( prFileMonitorData && prFileMonitorData->dwSIDLength )
        {
        SYM_ASSERT ( rNAVAPInternalCfg.dwSIDLength >=
                     prFileMonitorData->dwSIDLength );

        memcpy ( pSID,
                 prFileMonitorData->abySID,
                 prFileMonitorData->dwSIDLength );

        return ( prFileMonitorData->dwSIDLength );
        }

    return ( 0 );
    }

#endif

//---------------------------------------------------------------------------
//
// BOOL FileMonitorGetFileID (
//      LPED    lpEventData,
//      __int64 *pi64FileID
//      );
//
// This function returns the file ID saved previously by a call to
// FileMonitorSetFileID().
//
// Entry:
//      lpEventData - pointer to event data structure
//      pi64FileID  - pointer to a variable to receive the saved file ID.
//
// Exit:
//      Returns TRUE if successful and *pi64FileID contains a valid file
//      ID, otherwise FALSE.
//
// Synchronization:
//      This function is non-reentrant and should not be called
//      simultaneously with FileMonitorSetFileID().
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

BOOL FileMonitorGetFileID (
    LPED    lpEventData,
    __int64 *pi64FileID
    )
    {
    auto PFILEMONITORDATA prFileMonitorData;

                                        // Validate input parameters.

    SYM_ASSERT ( hOFData );
    SYM_ASSERT ( lpEventData );
    SYM_ASSERT ( pi64FileID );

                                        // Get synchronization object.

    prFileMonitorData = (PFILEMONITORDATA)
        SYMEvent_GetOpenFileDataPtrEx ( hOFData, lpEventData );

    SYM_ASSERT ( prFileMonitorData );

    if ( prFileMonitorData )
        {
                                        // If we have a file ID (non-0), then
                                        // this function will complete
                                        // successfully, otherwise it will
                                        // fail.

        return ( ( *pi64FileID = prFileMonitorData->i64FileID ) ?
                 TRUE : FALSE );
        }

                                        // Fail the function.

    *pi64FileID = 0;

    return ( FALSE );
    }

#endif

//---------------------------------------------------------------------------
//
// BOOL FileMonitorSetFileID (
//      LPED    lpEventData,
//      __int64 i64FileID
//      );
//
// This function saves the ID of a file at i64FileID at the open file data
// area at lpEventData.
//
// Entry:
//      lpEventData - pointer to event data structure
//      i64FileID   - a valid (non-0) file ID
//
// Exit:
//      Returns TRUE if successful, else FALSE.
//
// Synchronization:
//      This function is non-reentrant and should not be called
//      simultaneously with FileMonitorGetFileID().
//
//---------------------------------------------------------------------------
#if defined(SYM_NTK)

BOOL FileMonitorSetFileID (
    LPED    lpEventData,
    __int64 i64FileID
    )
    {
    auto PFILEMONITORDATA prFileMonitorData;

                                        // Validate input parameters.

    SYM_ASSERT ( hOFData );
    SYM_ASSERT ( lpEventData );
    SYM_ASSERT ( i64FileID );

                                        // Set attributes.

    prFileMonitorData = (PFILEMONITORDATA)
        SYMEvent_GetOpenFileDataPtrEx ( hOFData, lpEventData );

    SYM_ASSERT ( prFileMonitorData );

    if ( prFileMonitorData && i64FileID )
        {
        prFileMonitorData->i64FileID = i64FileID;

        return ( TRUE );
        }

    return ( FALSE );
    }

#endif

//---------------------------------------------------------------------------
//
// BOOL FileMonitorTransferData (
//      LPED  lpEventData,
//      PVOID pvDestination
//      );
//
// This function transfers all file monitor data from lpEventData to the
// buffer at pvDestination.  This function should be used if a client needs
// to create an event data structure and transfer the file monitor data
// from lpEventData to it.  A custom built event data structure should have
// its SE_ED_lpDTA member set to point to pvDestination, and SE_FLAGS_SYNC
// flag set at SE_ED_efEventFlags in order for the file monitor APIs to
// access the correct file monitor data.
//
// Entry:
//      lpEventData   - event data structure
//      pvDestination - buffer to store file monitor data
//                      (uFileMonitorDataLength bytes in length)
//
// Exit:
//      Returns TRUE if successful, else FALSE.  Upon successful completion,
//      the buffer at pvDestination contains a full copy of the file monitor
//      data at lpEventData, and the original file monitor data at
//      lpEventData is cleared.
//
//---------------------------------------------------------------------------
BOOL FileMonitorTransferData (
    LPED  lpEventData,
    PVOID pvDestination
    )
    {
    auto PFILEMONITORDATA prFileMonitorData;

                                        // Validate input parameters.

    SYM_ASSERT ( hOFData );
    SYM_ASSERT ( uFileMonitorDataLength );
    SYM_ASSERT ( lpEventData );
    SYM_ASSERT ( pvDestination );
    SYM_VERIFY_BUFFER ( pvDestination, uFileMonitorDataLength );

                                        // Get a pointer to the file monitor
                                        // data and copy it into the
                                        // destination buffer.  After this,
                                        // clear the data in the original
                                        // event data structure.

                                        // Note, that we don't use the Ex
                                        // version of
                                        // SYMEvent_GetOpenFileDataPtr()
                                        // because we are always called with
                                        // lpEventData created by SYMEvent.

    prFileMonitorData = (PFILEMONITORDATA)
        SYMEvent_GetOpenFileDataPtr ( hOFData, lpEventData );

    if ( prFileMonitorData )
        {
        SymAcquireFastGlobalLock();

        memcpy ( pvDestination, prFileMonitorData, uFileMonitorDataLength );
        memset ( prFileMonitorData, 0, uFileMonitorDataLength );

        SymReleaseFastGlobalLock();

        return ( TRUE );
        }

    return ( FALSE );
    }
