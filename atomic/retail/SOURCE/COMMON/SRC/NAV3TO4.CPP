// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/common/VCS/nav3to4.cpv   1.0   06 Feb 1997 20:50:08   RFULLER  $
//
// Description:
//      Converts old NAVOPTS data to v4.0 format.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/common/VCS/nav3to4.cpv  $
// 
//    Rev 1.0   06 Feb 1997 20:50:08   RFULLER
// Initial revision
// 
//    Rev 1.0   01 Mar 1996 18:26:46   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 09:55:32   BARRY
// Initial revision.
// 
//    Rev 1.2   14 Nov 1994 18:02:50   MFALLEN
// assert
// 
//    Rev 1.1   18 Jul 1994 17:51:30   DALLEE
// Set CREATE_NAVBIN since these routines deal with the NAVOPTS structure
// as it's written to disk.
//************************************************************************

#define CREATE_NAVBIN                   // Defined since we're dealing with
                                        // NAVOPTS as written to disk.

#include "platform.h"
#include "navbnhdr.h"
#include "options.h"
#include "oldnavop.h"
#include "navconv.h"

// ---------------------------------------------------------------------------
//                          LOCAL FUNCTION PROTOS
// ---------------------------------------------------------------------------

#ifdef SYM_WIN
  VOID LOCAL NavOptsOemToAnsi (LPNAVOPTIONS lpNavOptions);
  VOID LOCAL NavOptsAnsiToOem (LPNAVOPTIONS lpNavOptions);
#else
  #define NavOptsOemToAnsi(x)
#endif


STATUS LOCAL ConvertNavOptions30to40 (  // Convert NAVOPTIONS information
    LPNAVOPTIONS lpNav,                 // [in] new NAVOPTIONS format
    LPSTR        lpBuffer );            // [in] data read from file

STATUS LOCAL ConvertUserOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav );          // [in] NAV version 3.0 structure

STATUS LOCAL ConvertTsrOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav );          // [in] NAV version 3.0 structure

STATUS LOCAL ConvertActivityOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav );          // [in] NAV version 3.0 structure

STATUS LOCAL ConvertAlertOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav );          // [in] NAV version 3.0 structure

STATUS LOCAL ConvertInocOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav );          // [in] NAV version 3.0 structure

STATUS LOCAL ConvertScanOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav );          // [in] NAV version 3.0 structure

STATUS LOCAL ConvertPasswordOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav );          // [in] NAV version 3.0 structure

STATUS LOCAL ConvertGeneralOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav );          // [in] NAV version 3.0 structure



// --------------------------------------------------------------------------
// Copyright Symantec Corporation, Peter Norton Group.
// --------------------------------------------------------------------------
//
//  Function: ConvertNavOptions
//
//  Author:   Martin Fallenstedt
//
//  Returns:  NOERR if lpNavOptions was initialized
//            ERR if error occured.
//
// --------------------------------------------------------------------------

STATUS WINAPI ConvertNavOptions (       // Initialize NAV options structure
    DWORD dwVersion,                    // [in] depending on version number
    LPNAVOPTIONS lpNavOptions,          // [in] actual version structure
    LPSTR lpBuffer )                    // [in] buffer as read from disk
{
    auto    STATUS          Status = ERR;

    if ( dwVersion == NAV_VERSION_30 )
        {
                                        // -----------------------------------
                                        // Convert from 3.0 format to 3.5
                                        // -----------------------------------
        Status = ConvertNavOptions30to40 ( lpNavOptions, lpBuffer );
        }
    else if ( dwVersion == NAV_VERSION_40 )
        {
                                        // -----------------------------------
                                        // The buffer contains the current
                                        // version of NAVOPTIONS, so just copy
                                        // -----------------------------------
        MEMCPY ( lpNavOptions, lpBuffer, sizeof(NAVOPTIONS) );
        Status = NOERR;
        }
    
    if ( Status == NOERR )
        NavOptsOemToAnsi ( lpNavOptions );

    return ( Status );
}
                        
// --------------------------------------------------------------------------
// Copyright Symantec Corporation, Peter Norton Group.
// --------------------------------------------------------------------------
//
//  Function: ConvertNavOptions30to40
//
//  Author:   Martin Fallenstedt
//
//  Returns:  NOERR if lpNavOptions was initialized
//            ERR if error occured.
//
// --------------------------------------------------------------------------

STATUS LOCAL ConvertNavOptions30to40 (  // Convert NAVOPTIONS information
    LPNAVOPTIONS lpNav,                 // [in] new NAVOPTIONS format
    LPSTR        lpBuffer )             // [in] data read from file
{
    auto    STATUS          Status = ERR;
    auto    LPNAV30OPTIONS  lpOldNav = (LPNAV30OPTIONS)lpBuffer;

    SYM_ASSERT ( lpNav && lpBuffer );

    if ( ConvertUserOptions30to40     ( lpNav, lpOldNav ) == NOERR &&
         ConvertTsrOptions30to40      ( lpNav, lpOldNav ) == NOERR &&
         ConvertActivityOptions30to40 ( lpNav, lpOldNav ) == NOERR &&
         ConvertAlertOptions30to40    ( lpNav, lpOldNav ) == NOERR &&
         ConvertInocOptions30to40     ( lpNav, lpOldNav ) == NOERR &&
         ConvertScanOptions30to40     ( lpNav, lpOldNav ) == NOERR &&
         ConvertPasswordOptions30to40 ( lpNav, lpOldNav ) == NOERR &&
         ConvertGeneralOptions30to40  ( lpNav, lpOldNav ) == NOERR )
        {
        Status = NOERR;
        }

    return ( Status );
}

// --------------------------------------------------------------------------
// Copyright Symantec Corporation, Peter Norton Group.
// --------------------------------------------------------------------------
//
//  Function: ConvertUserOptions30to40
//
//  Author:   Martin Fallenstedt
//
//  Returns:  NOERR if lpNavOptions was initialized
//            ERR if error occured.
//
// --------------------------------------------------------------------------

STATUS LOCAL ConvertUserOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav )           // [in] NAV version 3.0 structure
{
    SYM_ASSERT ( sizeof(lpNav->user.szName)    >= sizeof(lpOldNav->user.szName) );
    SYM_ASSERT ( sizeof(lpNav->user.szCompany) >= sizeof(lpOldNav->user.szCompany) );

    STRCPY ( lpNav->user.szName,    lpOldNav->user.szName );
    STRCPY ( lpNav->user.szCompany, lpOldNav->user.szCompany);

    return ( NOERR );
}

// --------------------------------------------------------------------------
// Copyright Symantec Corporation, Peter Norton Group.
// --------------------------------------------------------------------------
//
//  Function: ConvertTsrOptions30to40
//
//  Author:   Martin Fallenstedt
//
//  Returns:  NOERR if lpNavOptions was initialized
//            ERR if error occured.
//
// --------------------------------------------------------------------------

STATUS LOCAL ConvertTsrOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav )           // [in] NAV version 3.0 structure
{
                                        // ----------------------------------
                                        // The TSR options do not have
                                        // embeded strings.
                                        // ----------------------------------
    SYM_ASSERT ( sizeof(lpNav->tsr) == sizeof(lpOldNav->tsr) );

    MEMCPY ( &lpNav->tsr, &lpOldNav->tsr, sizeof(lpNav->tsr) );

    return ( NOERR );
}

// --------------------------------------------------------------------------
// Copyright Symantec Corporation, Peter Norton Group.
// --------------------------------------------------------------------------
//
//  Function: ConvertActivityOptions30to40
//
//  Author:   Martin Fallenstedt
//
//  Returns:  NOERR if lpNavOptions was initialized
//            ERR if error occured.
//
// --------------------------------------------------------------------------

STATUS LOCAL ConvertActivityOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav )           // [in] NAV version 3.0 structure
{
    SYM_ASSERT ( sizeof(lpNav->activity.szLogFile) >=
             sizeof(lpOldNav->activity.szLogFile) );

    STRCPY ( lpNav->activity.szLogFile, lpOldNav->activity.szLogFile );

    lpNav->activity.bKnown      = lpOldNav->activity.bKnown;
    lpNav->activity.bUnknown    = lpOldNav->activity.bUnknown;
    lpNav->activity.bInocChange = lpOldNav->activity.bInocChange;
    lpNav->activity.bVirusLike  = lpOldNav->activity.bVirusLike;
    lpNav->activity.bStartEnd   = lpOldNav->activity.bStartEnd;
    lpNav->activity.bVirusList  = lpOldNav->activity.bVirusList;
    lpNav->activity.bKeepLast   = lpOldNav->activity.bKeepLast;
    lpNav->activity.uEntries    = lpOldNav->activity.uEntries;
    lpNav->activity.bKeepUpTo   = lpOldNav->activity.bKeepUpTo;
    lpNav->activity.uKiloBytes  = lpOldNav->activity.uKiloBytes;

    return ( NOERR );
}

// --------------------------------------------------------------------------
// Copyright Symantec Corporation, Peter Norton Group.
// --------------------------------------------------------------------------
//
//  Function: ConvertAlertOptions30to40
//
//  Author:   Martin Fallenstedt
//
//  Returns:  NOERR if lpNavOptions was initialized
//            ERR if error occured.
//
// --------------------------------------------------------------------------

STATUS LOCAL ConvertAlertOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav )           // [in] NAV version 3.0 structure
{
    SYM_ASSERT ( sizeof(lpNav->alert.szAlertMsg) >=
             sizeof(lpOldNav->alert.szAlertMsg) );

    STRCPY ( lpNav->alert.szAlertMsg, lpOldNav->alert.szAlertMsg );

    lpNav->alert.bDispAlertMsg  = lpOldNav->alert.bDispAlertMsg;
    lpNav->alert.bRemoveAfter   = lpOldNav->alert.bRemoveAfter;
    lpNav->alert.uSeconds       = lpOldNav->alert.uSeconds;
    lpNav->alert.bBeep          = lpOldNav->alert.bBeep;
    lpNav->alert.bAlertNetUsr   = lpOldNav->alert.bAlertNetUsr;
    lpNav->alert.bAlertNetCon   = lpOldNav->alert.bAlertNetCon;
    lpNav->alert.bAlertNLM      = lpOldNav->alert.bAlertNLM;
    lpNav->alert.bKnown         = lpOldNav->alert.bKnown;
    lpNav->alert.bUnknown       = lpOldNav->alert.bUnknown;
    lpNav->alert.bInoculation   = lpOldNav->alert.bInoculation;
    lpNav->alert.bVirusLike     = lpOldNav->alert.bVirusLike;
    lpNav->alert.bStartEnd      = lpOldNav->alert.bStartEnd;
    lpNav->alert.bVirListChange = lpOldNav->alert.bVirListChange;

    return ( NOERR );
}

// --------------------------------------------------------------------------
// Copyright Symantec Corporation, Peter Norton Group.
// --------------------------------------------------------------------------
//
//  Function: ConvertInocOptions30to40
//
//  Author:   Martin Fallenstedt
//
//  Returns:  NOERR if lpNavOptions was initialized
//            ERR if error occured.
//
// --------------------------------------------------------------------------

STATUS LOCAL ConvertInocOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav )           // [in] NAV version 3.0 structure
{
    SYM_ASSERT ( sizeof(lpNav->inoc.szNetInocPath) >=
             sizeof(lpOldNav->inoc.szNetInocPath) );

    STRCPY ( lpNav->inoc.szNetInocPath, lpOldNav->inoc.szNetInocPath );

    lpNav->inoc.bUseSystemInoc = lpOldNav->inoc.bUseSystemInoc;
    lpNav->inoc.bUseInoc       = lpOldNav->inoc.bUseInoc;
    lpNav->inoc.bInocFloppies  = lpOldNav->inoc.bInocFloppies;
    lpNav->inoc.bInocFast      = lpOldNav->inoc.bInocFast;
    lpNav->inoc.uActionNewInoc = lpOldNav->inoc.uActionNewInoc;
    lpNav->inoc.uActionInocChg = lpOldNav->inoc.uActionInocChg;
    lpNav->inoc.bDispRepair    = lpOldNav->inoc.bDispRepair;
    lpNav->inoc.bDispDelete    = lpOldNav->inoc.bDispDelete;
    lpNav->inoc.bDispInoc      = lpOldNav->inoc.bDispInoc;
    lpNav->inoc.bDispCont      = lpOldNav->inoc.bDispCont;
    lpNav->inoc.bDispStop      = lpOldNav->inoc.bDispStop;
    lpNav->inoc.bDispExcl      = lpOldNav->inoc.bDispExcl;

    return ( NOERR );
}

// --------------------------------------------------------------------------
// Copyright Symantec Corporation, Peter Norton Group.
// --------------------------------------------------------------------------
//
//  Function: ConvertScanOptions30to40
//
//  Author:   Martin Fallenstedt
//
//  Returns:  NOERR if lpNavOptions was initialized
//            ERR if error occured.
//
// --------------------------------------------------------------------------

STATUS LOCAL ConvertScanOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav )           // [in] NAV version 3.0 structure
{
    SYM_ASSERT ( sizeof(lpNav->scanner) == sizeof(lpOldNav->scanner) );

    MEMCPY ( &lpNav->scanner, &lpOldNav->scanner, sizeof (lpNav->scanner) );

    return ( NOERR );
}

// --------------------------------------------------------------------------
// Copyright Symantec Corporation, Peter Norton Group.
// --------------------------------------------------------------------------
//
//  Function: ConvertPasswordOptions30to40
//
//  Author:   Martin Fallenstedt
//
//  Returns:  NOERR if lpNavOptions was initialized
//            ERR if error occured.
//
// --------------------------------------------------------------------------

STATUS LOCAL ConvertPasswordOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav )           // [in] NAV version 3.0 structure
{
    SYM_ASSERT ( sizeof(lpNav->password) == sizeof(lpOldNav->password) );

    MEMCPY ( &lpNav->password, &lpOldNav->password, sizeof(lpNav->password) );

    return ( NOERR );
}

// --------------------------------------------------------------------------
// Copyright Symantec Corporation, Peter Norton Group.
// --------------------------------------------------------------------------
//
//  Function: ConvertGeneralOptions30to40
//
//  Author:   Martin Fallenstedt
//
//  Returns:  NOERR if lpNavOptions was initialized
//            ERR if error occured.
//
// --------------------------------------------------------------------------

STATUS LOCAL ConvertGeneralOptions30to40 (
    LPNAVOPTIONS lpNav,                 // [in] New NAV options structure
    LPNAV30OPTIONS lpOldNav )           // [in] NAV version 3.0 structure
{
    SYM_ASSERT ( sizeof(lpNav->general.szBackExt) >=
             sizeof(lpOldNav->general.szBackExt) );

    SYM_ASSERT ( sizeof(lpNav->general.szExt) >=
             sizeof(lpOldNav->general.szExt) );

    SYM_ASSERT ( sizeof(lpNav->general.szDef) >=
             sizeof(lpOldNav->general.szDef) );

    STRCPY ( lpNav->general.szBackExt, lpOldNav->general.szBackExt);
    STRCPY ( lpNav->general.szExt,     lpOldNav->general.szExt);
    STRCPY ( lpNav->general.szDef,     lpOldNav->general.szDef);

    lpNav->general.bScanHighMemory = lpOldNav->general.bScanHighMemory;
    lpNav->general.bScanZipFiles   = lpOldNav->general.bScanZipFiles;
    lpNav->general.bBackupRep      = lpOldNav->general.bBackupRep;

    return ( NOERR );
}

#ifdef SYM_WIN

//************************************************************************
// NavOptsOemToAnsi()
//
// This routine converts all the NAV text options from Oem to Ansi strings.
//
// *** WARNING! *** WARNING! *** WARNING! *** WARNING! *** WARNING! ***
//
// This function is #define'd out of DOS compiles.  If changes are made
// here that cause the navoptions to be written to different buffers or
// that could break DOS in other fashions, please cover the routine for
// DOS.
//
// Parameters:
//      LPNAVOPTIONS lpNavOptions       'nuff said
//
// Returns:
//      Nothing
//************************************************************************
// 7/22/93 DALLEE, #ifdef for Windows only.
//************************************************************************

VOID LOCAL NavOptsOemToAnsi (LPNAVOPTIONS lpNavOptions)
{
    OemToAnsi (lpNavOptions->user.szName,
               lpNavOptions->user.szName);
    OemToAnsi (lpNavOptions->user.szCompany,
               lpNavOptions->user.szCompany);
    OemToAnsi (lpNavOptions->activity.szLogFile,
               lpNavOptions->activity.szLogFile);
    OemToAnsi (lpNavOptions->alert.szAlertMsg,
               lpNavOptions->alert.szAlertMsg);
    OemToAnsi (lpNavOptions->general.szBackExt,
               lpNavOptions->general.szBackExt);
    OemToAnsi (lpNavOptions->general.szExt,
               lpNavOptions->general.szExt);
    OemToAnsi (lpNavOptions->general.szDef,
               lpNavOptions->general.szDef);
    OemToAnsi (lpNavOptions->inoc.szNetInocPath,
               lpNavOptions->inoc.szNetInocPath);

} // End NavOptsOemToAnsi()

#endif  // SYM_WIN

