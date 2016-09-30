// Copyright 1993 Symantec Corporation
//***********************************************************************
//
// $Header:   S:/COMMON/VCS/options.cpv   1.11   31 Jul 1998 10:44:44   tcashin  $
//
// Description:
//      This file contains the source that configures the options for the
//      Norton AntiVirus (both DOS and Windows).
//
// Contains (as exports):
//
// See Also:
//***********************************************************************
// $Log:   S:/COMMON/VCS/options.cpv  $
// 
//    Rev 1.11   31 Jul 1998 10:44:44   tcashin
// #ifdef bIsNT for Windows & non-Windows platforms.
// 
//    Rev 1.10   23 Jul 1998 12:14:52   mdunn
// Added SaveInocPathToRegistry().  The code was moved here from N32OPTS since
// this is the right place to have it.
// 
//    Rev 1.9   24 Jun 1998 15:32:10   jtaylor
// Restored the general tab for OEM.
// 
//    Rev 1.7   21 Nov 1997 17:08:12   jtaylor
// Added code to create a custom subset of the options pages if n32pdll.dll says the product is scan only.
//
//    Rev 1.6   27 Jun 1997 13:17:18   DBUCHES
// Now, properly set index for active panel.
//
//    Rev 1.5   27 Jun 1997 13:13:50   DBUCHES
// Now, properly set index for active panel.
//
//    Rev 1.4   27 May 1997 13:49:00   DBUCHES
// Now, check for changes in quarantine and custom response options
// when we save out NAVOPTS.
//
//    Rev 1.3   16 May 1997 18:59:10   DBUCHES
// Changes
// 	1) Replaced old-style CPL based prop-page stuff with
// 	   new N32OPTS property sheet.
//
// 	  2) Removed some long-dead code - specifically, NAVNET stuff.
//
//    Rev 1.2   21 Apr 1997 14:09:44   JBRENNA
// Removed TalkToVxD calls related to reloading the options files (NT only).
// The NAVAPSVC auto-detects changes to the file and automatically reloads it.
//
//    Rev 1.1   05 Mar 1997 23:52:26   DALLEE
// Port from HYDRA.15.
//
// DOS controls panels increased to 7 to include password once again.
//
//    Rev 1.0   06 Feb 1997 20:50:10   RFULLER
// Initial revision
//
//    Rev 1.20   25 Oct 1996 23:02:36   MKEATIN
// Fixed Comments
//
//    Rev 1.19   25 Oct 1996 22:50:44   MKEATIN
// Replaced ReplaceLastChar() with AddQuestMarkToExtention().  This will now
// work on all platforms and is DBCS enabled.
//
//    Rev 1.18   18 Oct 1996 13:48:32   TIVANOV
// remove the export statment
//
//    Rev 1.17   18 Oct 1996 13:18:06   TIVANOV
// change the defines from NAVNETNT to NAVNET32
//
//    Rev 1.16   18 Oct 1996 09:27:32   TIVANOV
// fix the checkin
//
//    Rev 1.14.1.0   18 Oct 1996 09:17:30   TIVANOV
// fix the previose checkin
//
//    Rev 1.14   14 Oct 1996 16:29:50   JBRENNA
// Change the DBCS exclusions. If the last character of the extension is a
// DBCS character, replace the 2 byte DBCS char w/ the 1 byte '?' char. Used
// to be replaced with "??".
//
//    Rev 1.13   14 Oct 1996 13:50:10   JBRENNA
// Change UpdateExclude() to correctly process DBCS extensions. Previously,
// an extension ending in a DBCS character (2 bytes) would get garbled
// in the exclusion list. This is because the second byte of the DBCS char
// would be replaced with '?' character. Now, the last DBCS character gets
// replaced with "??" for the exclusions.
//
//    Rev 1.12   12 Sep 1996 17:04:22   PVENKAT
// Fixed #68531.  Forcing reload on both Service & VxD for options change.
//
//    Rev 1.11   20 Aug 1996 11:35:22   JBRENNA
// Fix NT-specific for DOS ... used DOSX logic.
//
//    Rev 1.10   20 Aug 1996 11:30:46   JBRENNA
// Port DOSNAV changes: MZAREMB: NT-specific code was causing a problem in DX,
// so I #ifdef'd around it.
//
//    Rev 1.9   17 Jul 1996 08:22:34   JWORDEN
// Add logic for NAVNETNT
//
//    Rev 1.8   02 Jul 1996 02:19:04   DALLEE
// Convert automatic exclusion for new backup extension to OEM before
// passing to ExcludeXXX() functions -- they all take OEM.
//
//    Rev 1.7   28 Jun 1996 11:13:48   YVANG
// Ported changes from Luigi-Plus.
//
//    Rev 1.6   26 Jun 1996 20:13:18   DJESCH
// Will now always update NAVSTART.DAT when modified (STS 58335).
//
//    Rev 1.5   20 Jun 1996 14:35:40   PVENKAT
// Avoided the second 'Dummy' msg being displayed when Options is modified on
// a Read-Only Permission directory.
//
//    Rev 1.4   10 Jun 1996 15:00:04   PVENKAT
// Added code to process the return code of WriteNavBin to display error msg
// if NAVOPTS.DAT is read-only.
//
//    Rev 1.3   28 May 1996 16:41:34   GWESTER
// Added CPLO_NOTIFY flag to options panel, so that we can validate data.
//
//    Rev 1.2   17 May 1996 14:35:42   DSACKIN
// If ANY options change, we need to send a code to the NAVAPSVC to reload opts.
//
//    Rev 1.1   04 Apr 1996 15:38:12   BARRY
// Ported from Kirin
//
//    Rev 1.1   26 Mar 1996 11:28:30   jworden
// Port rev 1.49 from Luigi
//
//    Rev 1.49   09 Mar 1996 22:43:52   MKEATIN
// When saving exclusion changes, call ExcludeCreateCopy() to use the new
// Exclude buffer.  MemCopy() doesn't copy back lpNavOptions->exclude->lpPrivate
//
//    Rev 1.0   01 Mar 1996 18:27:06   BARRY
// Initial revision.
//***********************************************************************

#include "platform.h"                   // Platform independence
#include "xapi.h"
#include <shellapi.h>
#include <ctype.h>                      // isdigit()
#include <stdlib.h>
#include "file.h"
#include "syminteg.h"
#include "navutil.h"
#include "navalog.h"

#ifdef SYM_WIN
 #include "n32behav.h"
 #include <windowsx.h>                  // Windows control functions
 #include <winbase.h>
 #include "ctrlpanl.h"                  // Control Panel
 #include "navw.h"
 #include "navwstb.h"
 #include "navutil.h"
 #include "navsheet.h"			// New MFC based option panels
#else
 #include "stddos.h"
 #include "stddlg.h"
#endif

#include "inoc.h"
#include "navbin.h"
#include "common.h"
#include "exclude.h"
#include "logio.h"
#include "nonav.h"
#include "navnet.h"
#include "nnetstb.h"
#include "dbcs.h"


//************************************************************************
// EXTERNS
//************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

 extern      NAVOPTIONS      navOptions;

 #ifdef SYM_DOS

  extern      DIALOG          dlOptions;         // CP background dialog in optstr.cpp
  extern      char            szOptListLabel[];  // CP list title in optstr.cpp
  extern      CPL             cpOptList[];       // Array of CPL entries in optstr.cpp
  extern      LPNAVOPTIONS    lpNavOptions;

 #endif  // SYM_DOS

#ifdef __cplusplus
}
#endif


MODULE_NAME;

//************************************************************************
// LOCAL DEFINES
//************************************************************************


//#define TRACE   ERR_TRACE_ID (_NAVWOPTS_C)

#define OPTIONS_CPL_WIDTH       17

#ifdef __cplusplus
extern "C" {
#endif

WORD        wCPLOptions = 7;            // Number of options panels

#ifdef __cplusplus
}
#endif


//************************************************************************
// GLOBAL VARIABLES
//************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

char SZ_COMMA[]                 = ",";

#ifdef __cplusplus
}
#endif

static BOOL gbDirectoryExists,
            gbFilePathsAreDifferent;

static HINSTANCE ghInst;

//************************************************************************
// External Global Variables
//************************************************************************

extern WORD gwNAVPresent,
            gwPlatform[PLATFORM_COUNT];



//************************************************************************
// LOCAL PROTOTYPES - common
//************************************************************************

VOID LOCAL UpdateExclude
(
    LPEXCLUDE   lpExclude,
    LPSTR       lpOldExt,
    LPSTR       lpNewExt
);

VOID LOCAL StoreChangesExceptExclude
(
    LPNAVOPTIONS    lpTarget,
    LPNAVOPTIONS    lpSource
);

#ifdef SYM_WIN32
void SaveInocPathToRegistry ( LPCTSTR );
#endif

//************************************************************************
// Ye Actual Code
//************************************************************************

//************************************************************************
// NavOptsControlPanel ()
//
// This routine calls up the control panel for the user after allocating
// enough memory for the control panel record and initializing it with
// all of the templates.  Only the panel(s) desired are constructed.
//
// INPUTS:
//    HWND         hWnd,                // Handle of parent window
//    HINSTANCE    hInstance,           // Handle of Instance
//
// RETURNS:
//      TRUE                    TSR options modified, update TSR
//      FALSE                   TSR options are NOT modified
//
//************************************************************************
// 01/05/93 MSL Function Created
// 07/09/93 DALLEE Moved code to disable TSR to NAVUTIL and EXCLUDE projects.
// 07/30/93 Martin Made sure we're processing memory allocation errors
//                 correctly.
// 06/14/95 MSL If this only resembled my original function...Good grief,
//              now tells VxD in Win95 to reload its options if they have
//              changed.
//************************************************************************

WORD WINAPI NavOptsControlPanel (       // Calls up Options Control Panel
    HWND         hWnd,                  // Handle of parent window
    HINSTANCE    hInstance,             // Handle of Instance
    WORD         wSetIndex              // Force this panel on to if not 0xFFFF
)
{
    auto        BOOL            bMemAllocOk = TRUE;
    auto        UINT            uStatus     = TRUE;

#ifdef SYM_WIN
    static      WORD            wOldIndex_95 = 0,
                                wOldIndex_3 = 0;
    auto        LPSTR           lpszPath = NULL;
    auto        LPNAVOPTIONS    lpNavOptions;
    auto        BOOL            bIsNT = GetVersion() < 0x80000000;
#else
    auto        BOOL            bIsNT = FALSE;
#endif

    auto        HGLOBAL         hNavOptions     = 0;
    auto        HGLOBAL         hExcludeItem    = 0;
    auto        HGLOBAL         hNetUserItem    = 0;
    auto        NETUSER         tempNetUser;
    auto        BOOL            bUseOldOne = TRUE;
    auto        WORD            wOptionsChanged = 0;
    auto        WORD            wNNSDefault = INI_OK;

#if !defined(NAVSCAN)
    auto        char            szOldExt  [ SYM_MAX_PATH + 1 ];
    auto        char            szLogFile [ SYM_MAX_PATH + 1 ];
    auto        BOOL            bOldLoadVxD;
#endif


                                        //-------------------------------
                                        // Take care of a few details
                                        // before we get started
                                        //-------------------------------
    gbDirectoryExists = FALSE;
    gbFilePathsAreDifferent = FALSE;
    ghInst = hInstance;

                                        //-------------------------------
                                        // Allocate space for copy of
                                        // 'navOptions'.
                                        //-------------------------------
    hNavOptions = MemAlloc(GHND, sizeof(NAVOPTIONS));

    if ( hNavOptions )
        {
        lpNavOptions = (LPNAVOPTIONS) MemLock(hNavOptions);

        if ( lpNavOptions == NULL )
            bMemAllocOk = FALSE;
        else
            {
                                        //---------------------------------
                                        // Make a copy of the 'navOptions'
                                        //---------------------------------
            *lpNavOptions = navOptions;
            }
        }
    else
        bMemAllocOk = FALSE;
                                        //---------------------------------
                                        // Make a copy of the Exclude buffer
                                        // For the exclusions buffer, we
                                        // always allocate + 1 (makes life
                                        // easier).
                                        //---------------------------------
    if ( bMemAllocOk == TRUE )
        {
        bMemAllocOk = ExcludeCreateCopy (
                                    &navOptions.exclude,
                                    &lpNavOptions->exclude
                                    );
        }
                                        //---------------------------------
                                        // Make a copy of the NetUser buffer
                                        // For the netuser buffer, we
                                        // always allocate + 1 (makes life
                                        // easier).
                                        //---------------------------------
    if ( bMemAllocOk == TRUE )
        {
        tempNetUser.uEntries = navOptions.netUser.uEntries;
        tempNetUser.hNetUserItem = MemAlloc(GHND, (navOptions.netUser.uEntries+1) *
                                              sizeof(NETUSERITEM));
        if ( tempNetUser.hNetUserItem )
            {
            tempNetUser.lpNetUserItem = (LPNETUSERITEM) MemLock(
                                                      tempNetUser.hNetUserItem);

            if ( tempNetUser.lpNetUserItem )
                {
                MEMCPY (tempNetUser.lpNetUserItem,
                        navOptions.netUser.lpNetUserItem,
                        navOptions.netUser.uEntries * sizeof(NETUSERITEM));
                }
            else
                bMemAllocOk = FALSE;
            }
        else
            bMemAllocOk = FALSE;
        }

    if ( bMemAllocOk == TRUE )
        {
                                        // Use the copies
        lpNavOptions->netUser = tempNetUser;

#ifdef SYM_WIN
        /*


         ******* New stuff should be inserted here.

        */
    int aiPages[N32OPTS_MAXPAGES];
    int iCount;

    // If this is the SCAN_ONLY version, customize the options panels
    if( NAV_IS_SCANONLY() )
        {
        // Allocate the pages for options.  In the SCAN_ONLY version.
        // the same options panels are diplayed for NT and 95.
        aiPages[0] = N32OPTS_SCANNER_PAGE;
        aiPages[1] = N32OPTS_ALERT_PAGE;
        aiPages[2] = N32OPTS_LOG_PAGE;
        aiPages[3] = N32OPTS_EXCLUDE_PAGE;
        aiPages[4] = N32OPTS_GENERAL_PAGE;

        iCount = 5;

        // Create the custom set of options panels
        uStatus =  (NAVOptionsSheetEx( hWnd, lpNavOptions, aiPages, iCount, wSetIndex != 0xFFFF ? wSetIndex : 0, bIsNT ) == ERROR_SUCCESS);
        }
    // Otherwise, create the standard options panels.
    else
        uStatus = (NAVOptionsSheet(hWnd, lpNavOptions, wSetIndex != 0xFFFF ? wSetIndex : 0) == ERROR_SUCCESS);

#else   // SYM_WIN
                                        // Open the CP dialog and let
                                        // the user set the options.
        if (DialogCPLBox (&dlOptions,        // Control Panel background dialog
                          (LPBYTE)szOptListLabel,    // CP list title
                          OPTIONS_CPL_WIDTH, // CP list width
                          wCPLOptions,       // Number of CP list entries
                          cpOptList))        // CP list entries
            {
            uStatus = FALSE;
            }

#endif
                                        // Save these back
        tempNetUser = lpNavOptions->netUser;
                                        // Save the backup extension so we
                                        // can compare if it has changed
                                        // after the CPL call.  Do the same
                                        // for if autoprotect is to be
                                        // automatically loaded with windows.

#if !defined(NAVSCAN)

        STRCPY (szOldExt, navOptions.general.szBackExt);
        bOldLoadVxD = navOptions.startup.bLoadVxD;

#endif
                                        // If user pressed OK and there is
                                        // a change, then write the new values
                                        // out.
        if ( uStatus )
            {
                                        // Make them match
            lpNavOptions->netUser = navOptions.netUser;

            if ( MEMCMP (lpNavOptions, &navOptions, sizeof (NAVOPTIONS))
            ||   gbFilePathsAreDifferent )
                {
                #ifdef SYM_WIN32
                if ( !NAV_IS_SCANONLY() && !bIsNT )
                    {
                    SaveInocPathToRegistry ( lpNavOptions->inoc.szNetInocPath );
                    }
                #endif

                #ifndef NAVSCAN

                                        // ------------------------------------
                                        // If exclusions changed we don't need
                                        // to update the NAVOPTS.DAT file.
                                        // After NAVOPTS.DAT has been written
                                        // the OPTIONS_UPDATETSR and the
                                        // OPTIONS_CHANGED flags will be set.
                                        // ------------------------------------

                    if ( !ExcludeObjectsIdentical (&lpNavOptions->exclude,
                                                  &navOptions.exclude))
                        {
                        wOptionsChanged |= ( OPTIONS_EXCLUDECHANGED | OPTIONS_UPDATETSR);
                        }

                                            // Check if TSR options should be
                                            // updated.
                    if (MEMCMP (&lpNavOptions->tsr, &navOptions.tsr, sizeof(navOptions.tsr))
                    ||  MEMCMP (&lpNavOptions->alert, &navOptions.alert, sizeof(navOptions.alert))
                    ||  MEMCMP (&lpNavOptions->activity, &navOptions.activity, sizeof(navOptions.activity))
                    ||  MEMCMP (&lpNavOptions->inoc, &navOptions.inoc, sizeof(navOptions.inoc))
                    ||  MEMCMP (&lpNavOptions->startup, &navOptions.startup, sizeof(navOptions.startup))
                    ||  MEMCMP (&lpNavOptions->customAP, &navOptions.customAP, sizeof(navOptions.customAP))
                    ||  MEMCMP (&lpNavOptions->quarantineset, &navOptions.quarantineset, sizeof(navOptions.quarantineset))
                    ||  MEMCMP (&lpNavOptions->password, &navOptions.password, sizeof(navOptions.password)))
                        {
                        wOptionsChanged |= (OPTIONS_UPDATETSR | OPTIONS_CHANGED);

                        #ifdef SYM_WIN
                                            // ------------------------------------
                                            // If show/hide vxd icon option changed
                                            // indicate action to take by NAVW32
                                            // ------------------------------------

                            if ( lpNavOptions->tsr.bHideIcon != navOptions.tsr.bHideIcon )
                                {
                                if ( lpNavOptions->tsr.bHideIcon == TRUE )
                                    wOptionsChanged |= OPTIONS_HIDEVXDICON;
                                else
                                    wOptionsChanged |= OPTIONS_SHOWVXDICON;
                                }

                            if ( lpNavOptions->tsr.bUnloadable !=
                                 navOptions.tsr.bUnloadable )
                                {
                                if ( lpNavOptions->tsr.bUnloadable == TRUE )
                                    wOptionsChanged |= OPTIONS_VXDUNLOADABLE;
                                else
                                    wOptionsChanged |= OPTIONS_VXDNOTUNLOADABLE;
                                }

                        #endif

                        }

                                            // ------------------------------------
                                            // If scanner, general
                                            // options changed indicate that file
                                            // should be updated.
                                            // ------------------------------------

                    if ( MEMCMP (&lpNavOptions->scanner, &navOptions.scanner,
                                sizeof(navOptions.scanner) )
                    ||   MEMCMP (&lpNavOptions->customScan, &navOptions.customScan,
                                sizeof(navOptions.customScan) )
                    ||   MEMCMP (&lpNavOptions->general, &navOptions.general,
                                sizeof(navOptions.general) ) )
                        {
                        wOptionsChanged |= OPTIONS_CHANGED;
                        }

                                            // ------------------------------------
                                            // If the log file size has changed or
                                            // the user decided not to limit the
                                            // size of the log file, write new info
                                            // ------------------------------------

                        if (lpNavOptions->activity.uKiloBytes != navOptions.activity.uKiloBytes
                        ||  lpNavOptions->activity.bKeepUpTo != navOptions.activity.bKeepUpTo )
                            {
                            BuildLogFilename (szLogFile, lpNavOptions->activity.szLogFile);

                            if ( lpNavOptions->activity.bKeepUpTo )
                                {
                                SetLogFileSize (szLogFile,
                                               ((DWORD)lpNavOptions->activity.uKiloBytes *
                                               1024));
                                }
                            else
                                {
                                lpNavOptions->activity.uKiloBytes = 0;
                                SetLogFileSize (szLogFile, 0);
                                }

                            wOptionsChanged |= OPTIONS_CHANGED;
                            }

                        if (STRCMP(szOldExt, lpNavOptions->general.szBackExt))
                            {
                            UpdateExclude(
                                        &lpNavOptions->exclude,
                                        szOldExt,
                                        lpNavOptions->general.szBackExt
                                        );

                            wOptionsChanged |= (OPTIONS_UPDATETSR | OPTIONS_CHANGED);
                            }

                                            //------------------------------
                                            // Update the navOptions structure
                                            // with the updated info
                                            //------------------------------

                    StoreChangesExceptExclude ( &navOptions, lpNavOptions );

                                            //------------------------------------
                                            // Only save options if something
                                            // changed.
                                            //------------------------------------

                    if ( wOptionsChanged & OPTIONS_CHANGED )
                        {
                            #if defined(SYM_DOS) || defined(SYM_DOSX)

                            WriteNavBin   ( &navOptions );

                            #else

                            if (WriteNavBin   ( &navOptions ) )
                                {
                                NOTIFYSTRUCT    Notify  = {0};

                                Notify.hParent   = hWnd;
                                Notify.uMsgID    = IDS_NAVOPTS_UPDATE_FAILED;
                                #ifdef NAVNETW
                                    Notify.uTitleID  = IDS_NAVNETW;
                                #else
                                    Notify.uTitleID  = IDS_NAVW;
                                #endif
                                Notify.hInstance = hInstance;

                                NotifyUser (&Notify);

                                }
                            #endif

                            #ifdef SYM_WIN                          // Only write NAVSTART in NAVW32
                            if ( WriteNavStart ( &navOptions ) == ERR )
                                {
                                NOTIFYSTRUCT    Notify  = {0};

                                Notify.hParent   = hWnd;
                                Notify.uMsgID    = IDS_NAVOPTS_CPL_NAVSTART_ERR;
                                #ifdef NAVNETW
                                    Notify.uTitleID  = IDS_NAVNETW;
                                #else
                                    Notify.uTitleID  = IDS_NAVW;
                                #endif
                                Notify.hInstance = hInstance;

                                NotifyUser (&Notify);
                                }
                            #endif

                            #if defined SYM_WIN32
                                BOOL bLoadVxdStartup = RegistryIsVxDStartup();

                                if ( navOptions.startup.bLoadVxD == TRUE )
                                    {
                                    if ( bLoadVxdStartup == FALSE )
                                        {
                                        if ( RegistryAddVxD() == FALSE )
                                            {
                                            wOptionsChanged |= OPTIONS_ADDVXD_ERROR;
                                            }
                                        }
                                    }
                                else
                                    {
                                    if ( bLoadVxdStartup == TRUE )
                                        {
                                        if ( RegistryRemoveVxD() == FALSE )
                                            {
                                            wOptionsChanged |= OPTIONS_REMOVEVXD_ERROR;
                                            }
                                        }
                                    }

                                                    // If we changed the autoloading
                                                    // of the VxD with Windows, there is
                                                    // some prompting to do.
                                if (bOldLoadVxD != navOptions.startup.bLoadVxD)
                                    {
                                    NavOptsLoadVxDChanged (hWnd,
                                                            navOptions.startup.bLoadVxD);
                                    }

                                if (SystemGetWindowsType() == SYM_SYSTEM_WIN_95)
                                {
                                        // If any options have changed, we
                                        // want to send a control code to the
                                        // service to re-load the options.
                                    TalkToVxD (VXDACTION_RELOAD_CONFIG);
                                }
                            #endif // #if defined SYM_WIN32

                        }

                                                //------------------------------------
                                                // If exclusions changed, clear that
                                                // flag and indicate that options
                                                // changed and the tsr should be
                                                // updated. Note that this must be done
                                                // after NAVOPTS.DAT has been written.
                                                //------------------------------------

                                if ( wOptionsChanged & OPTIONS_EXCLUDECHANGED )
                                    {
                                    wOptionsChanged &= ~OPTIONS_EXCLUDECHANGED;
                                    wOptionsChanged |= (OPTIONS_UPDATETSR | OPTIONS_CHANGED);
                                    }

                                RememberInocFloppy(navOptions.inoc.bInocFloppies);

                #else   // #ifndef NAVSCAN

                                        // Return TRUE for NAVSCAN if
                                        // any options have changed.

                    wOptionsChanged |= OPTIONS_CHANGED;

                #endif  // #else, #ifndef NAVSCAN
                }
                                        //------------------------------
                                        // See if exclusions changed
                                        //------------------------------


            if (  gbFilePathsAreDifferent ||
                  ExcludeObjectsIdentical (&lpNavOptions->exclude,
                                         &navOptions.exclude ) == FALSE )
                {
                #if !defined NAVSCAN

                bUseOldOne = FALSE;

                                        // Update EXCLUDE.BIN, but don't
                                        // deallocate memory

                ExcludeTerminate ( &lpNavOptions->exclude, FALSE );
                #endif

                wOptionsChanged |= (OPTIONS_UPDATETSR | OPTIONS_CHANGED);

                }
                                        //------------------------------
                                        // We don't need this for now
                                        //------------------------------

                                        //------------------------------
                                        // See if NetUser's changed
                                        //------------------------------
                if ( tempNetUser.uEntries != navOptions.netUser.uEntries ||
                     MEMCMP (tempNetUser.lpNetUserItem,
                            navOptions.netUser.lpNetUserItem,
                            navOptions.netUser.uEntries * sizeof(NETUSERITEM)))
                    {
#ifndef NAVSCAN
                    bUseOldOne = FALSE;
                                            // Update NETUSER.BIN, but don't
                                            // deallocate memory

                    UpdateNetUserList(&tempNetUser, FALSE);
#else   // #ifndef NAVSCAN
                                            // Return TRUE for NAVSCAN if any
                                            // options have changed.
                    wOptionsChanged |= OPTIONS_CHANGED;
#endif  // #else, #ifndef NAVSCAN
                    }

                #ifdef SYM_WIN32
                                            // Tell the VxD and agent to
                                            // reload the configuration file
                                            // since something changed.
                    if ((wOptionsChanged & OPTIONS_UPDATETSR) &&
                        SystemGetWindowsType() == SYM_SYSTEM_WIN_95)
                        {
                        TalkToVxD (VXDACTION_RELOAD_CONFIG);
                        }
                #endif
            }


                if ( bUseOldOne == TRUE )
                    {
                    hNetUserItem    = tempNetUser.hNetUserItem;

                    ExcludeKillCopy ( &lpNavOptions->exclude );
                    }
                else
                    {
                    hNetUserItem = navOptions.netUser.hNetUserItem;

                                            //------------------------------
                                            // And use new NetUser Buffer
                                            //------------------------------

                    navOptions.netUser = tempNetUser;

                                            //------------------------------
                                            // And use new Exclude Buffer
                                            //------------------------------

                    ExcludeKillCopy ( &navOptions.exclude );
                    ExcludeCreateCopy ( &lpNavOptions->exclude, &navOptions.exclude );
                    }

        }
                                        //------------------------------
                                        // Deallocate hNetUserItem
                                        //------------------------------
    if ( hNetUserItem )
        {
        MemRelease (hNetUserItem);
        MemFree (hNetUserItem);
        }
                                        //------------------------------
                                        // Deallocate navOptions
                                        //------------------------------
    if ( hNavOptions )
        {
        MemRelease (hNavOptions);
        MemFree (hNavOptions);
        }

    return (wOptionsChanged);
} // NavOptsControlPanel()



//************************************************************************
// StoreChangesExceptExclude()
//
// Stores all options except the exclude options that are handles differently.
//
// Parameters:
//    LPNAVOPTIONS    lpTarget,
//    LPNAVOPTIONS    lpSource
//
// Returns:
//  nothing
//************************************************************************

VOID LOCAL StoreChangesExceptExclude (
    LPNAVOPTIONS    lpTarget,
    LPNAVOPTIONS    lpSource
    )
{
    auto    EXCLUDE     Exclude = { 0 };

    MEMCPY ( &Exclude, &lpTarget->exclude, sizeof(Exclude) );
    MEMCPY ( lpTarget, lpSource, sizeof(NAVOPTIONS) );
    MEMCPY ( &lpTarget->exclude, &Exclude, sizeof(Exclude) );
}



//****************************************************************************
// AddQuestMarkToExtention()
//
// Append a '?' to the end of an extention. If the string is longer than
// 3 bytes it will truncated to 3 bytes.
//
// Parameters:
//   lpszExtention [in/out]  extention buffer - must at least 4 bytes in size.
//****************************************************************************
// 10/25/96 MKEATIN - created
//****************************************************************************
static void AddQuestMarkToExtention (LPSTR lpszExtention)
{
    const WORD wStrLen = STRLEN (lpszExtention);

    if (!wStrLen)                       // Empty string
        return;
    else if (wStrLen < 3)               // Just append a '?'
        STRCAT (lpszExtention, "?");
    else                                // Replace the 3rd byte with a '?'
    {                                   // If it's a Double byte char, then
                                        // replace the 2nd byte.

        if (DBCSGetByteType(&lpszExtention[0],
                            &lpszExtention[2]) == _MBC_TRAIL)
        {
            lpszExtention[1] = '?';
            lpszExtention[2] = EOS;
        }
        else
        {
            lpszExtention[2] = '?';
            lpszExtention[3] = EOS;
        }
    }
}


//************************************************************************
// UpdateExclude()
//
// This routine deletes the old backup extension from the list of
// exclusions and adds the new backup extension.
// Expects extensions in ANSI form.  Will convert to OEM before passing
// to ExcludeXXX() functions.
//
// Parameters:
//      LPEXCLUDE   lpExclude           Record with exclude items.
//      LPSTR       lpOldExt            Old extension to remove.
//      LPSTR       lpNexExt            Nex extension to add.
//
// Returns:
//      Nothing
//************************************************************************
// 5/??/93 MFALLEN, created.
// 6/27/95 KEITH, fix missing "*." on new extension.
//************************************************************************

VOID LOCAL UpdateExclude (
    LPEXCLUDE   lpExclude,
    LPSTR       lpOldExt,
    LPSTR       lpNewExt )
{
    auto    EXCLUDEITEM     FindItem = {0};
    auto    EXCLUDEITEM     ReplaceWith = {0};
    auto    TCHAR           szNewExtCopy[4] = {0};
    auto    TCHAR           szOldExtCopy[4] = {0};

    if ( STRLEN (lpNewExt) < 4 )
        STRCPY( szNewExtCopy, lpNewExt );
    AddQuestMarkToExtention ( szNewExtCopy );
    STRCPY ( ReplaceWith.szText, "*." );
    STRCAT ( ReplaceWith.szText, szNewExtCopy );

    ReplaceWith.wBits           = excVIRUS_FOUND;
    ReplaceWith.bSubDirectory   = TRUE;

                                        // Must be in OEM for exclude functions.
#ifdef SYM_WIN
    NavAnsiToOem(ReplaceWith.szText, ReplaceWith.szText);
#endif

    if ( STRLEN (lpOldExt) < 4 )
        STRCPY( szOldExtCopy, lpOldExt );
    AddQuestMarkToExtention ( szOldExtCopy );
    STRCPY ( FindItem.szText, "*." );
    STRCAT ( FindItem.szText, szOldExtCopy );
                                        // Must be in OEM for exclude functions.
#ifdef SYM_WIN
    NavAnsiToOem(FindItem.szText, FindItem.szText);
#endif

    BOOL bStatus = ExcludeEdit (
                            lpExclude,
                            &FindItem,
                            &ReplaceWith
                            );

    if ( bStatus == FALSE )
        {
        ExcludeAdd ( lpExclude, &ReplaceWith );
        }
}


#ifdef SYM_WIN32
//////////////////////////////////////////////////////////////////////////
//
// Function:    SaveInocPathToRegistry
//
// Description:
//  Stores the inoc data path in the registry for Microsoft's use.
//  See defect #112166 for more info.
//
// Input:
//  szPath: [in] Path to store.
//
// Returns:
//  Nothing.
//
// Note:
//  Strings in function should not be translated.
//
//////////////////////////////////////////////////////////////////////////
// 7/21/98  Mike Dunn -- function created
//////////////////////////////////////////////////////////////////////////

void SaveInocPathToRegistry ( LPCTSTR szPath )
{
HKEY hkey;

    if ( ERROR_SUCCESS == RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                                         _T("Software\\Symantec\\Norton AntiVirus"),
                                         0, KEY_SET_VALUE, &hkey ) )
        {
                                        // Store the inoc path.

        RegSetValueEx ( hkey, _T("InocDir"), 0, REG_SZ,
                        (LPBYTE) szPath,
                        sizeof(TCHAR) * (1 + lstrlen (szPath)) );
        
        RegCloseKey ( hkey );
        }
}
#endif  // def SYM_WIN32

