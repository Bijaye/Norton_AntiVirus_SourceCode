// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/winmisc.cpv   1.14   11 May 1998 16:52:48   DALLEE  $
//
// Description:
//
// Contents:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVXUTIL/VCS/winmisc.cpv  $
// 
//    Rev 1.14   11 May 1998 16:52:48   DALLEE
// Added LoadAutoProtect() - load service and agent for 95 or NT.
// 
//    Rev 1.13   28 Apr 1998 02:00:30   DALLEE
// Added new message for TalkToVxD() to globally enable/disable AP.
// 
//    Rev 1.12   20 Aug 1997 21:30:10   DALLEE
// waugh... Removed multithreaded RPC calls for now and replaced w/ cheese.
// Had problems under 95 w/ the main thread exiting and nuking the threads
// making RPC calls, leaving the RPC subsystem in a funked up state.
// 
// Original reason for making RPC calls in separate threads was to work around
// long (~5 second) delays on W95 if no RPC server was around.
// Now attempts a FindWindow( AP Window ) before trying RPC calls on W95.
// 
//    Rev 1.11   14 Aug 1997 17:02:52   MKEATIN
// Renamed NAVSYSL.DAT to NAVSYSR.DAT.  Avoids conficts with FAT32 boot
// CRC's with older version of NAV.
// 
//    Rev 1.10   14 Aug 1997 14:04:28   DBUCHES
// More work in support of the last Rev.  Calls to the NAVAP VxD regarding
// process protection actually need to be made in the context of the
// *THREAD* we need to enable/disable protection for.  So be it.  Now,
// only the RPC based calls are executed on a seperate thread.
//
//    Rev 1.9   14 Aug 1997 13:39:08   DBUCHES
// Several changes:
//
// 1) Guarded NavRpc* functions with a critical section.
//
// 2) Changes to TalkToVxD() routine.  This routine now spawns
//    a worker thread which handles communicating with the VxD
//    and ApAgent.  This fixes a problem where NAVW will appear to
//    lock up when attempting to communicate with the ApAgent when
//    the ApAgent is not loaded.
//
//    Rev 1.8   12 Aug 1997 16:16:10   MKEATIN
// removed an extra ')'.
//
//    Rev 1.7   12 Aug 1997 16:03:38   MKEATIN
// Added GetNavexInfFile().
//
//    Rev 1.6   18 Jul 1997 15:25:42   DALLEE
// When checking for AutoProtect Agent, switch on platform to detemine
// name of window class to find.
//
//    Rev 1.5   03 Jun 1997 12:07:46   DALLEE
// oops.. put back #ifdef SYM_WIN32 inside TalkToVxD() to stub it for other
// platforms.
//
//    Rev 1.4   02 Jun 1997 15:01:58   DALLEE
// Standardize on NT-style DriverComm communication in TalkToVxD().
// Old direct calls to DeviceIoControl() no longer supported by NAVAP.VXD.
//
//    Rev 1.3   31 May 1997 15:37:32   KSACKIN
// Modified the check to see if the Scheduler is present to now check for an
// executable for NT.  NT now will use NSCHEDNT.EXE.
//
//    Rev 1.2   21 May 1997 13:16:44   BILL
// Remove self test for Alpha
//
//    Rev 1.1   24 Apr 1997 16:32:18   JBRENNA
// Tesla Port:
//  SEDWARD: r1.3:
// Use new IDS_ERR_NAVOP_OPEN_EX string if unable to start the AP service.
//
//  TSmith: r1.27:
// Added function NavRpcSendPacket.
//
//    Rev 1.0   06 Feb 1997 21:05:12   RFULLER
// Initial revision
//
//    Rev 1.29   23 Jan 1997 13:44:12   JBRENNA
// Use DiskGetBootDrive() instead of DiskGetFirstFixedDrive() when looking for
// the boot disk. On NEC machines, the DiskGetFirstFixedDrive can return the
// wrong boot drive letter.
//
//    Rev 1.28   11 Dec 1996 09:35:32   JBRENNA
// GetSysIntegFile(): No longer hard code the integ file to the C: drive. On NEC
// machines, A: is the boot drive (which is mostly the hard drive). Use
// DiskGetFirstFixedDrive() to determine the hard drive for the integ filename.
//
//    Rev 1.27   09 Sep 1996 13:26:22   DALLEE
// Put in do-nothing W95 handler for NAVAPSVC_RELOAD_CONFIG message to get
// rid of an assertion for unknown/invalid message.
//
// And removed some hard-coded tabs (4 space ones at that!)
//
//    Rev 1.26   06 Sep 1996 17:26:30   PVENKAT
// Ported Gryphon changes.
//
//
//          Rev 1.26    15 Aug 1996 12:35:58    PVENKAT
//      Fixed #64653.  if AP is automatic and not running (both servicem & Agent) and
//      on options if you set it to off, then now it will change to manual.
//          Rev 1.24    07 Aug 1996 10:26:36    PVENKAT
//      Fixed #63519
//
//    Rev 1.25   19 Aug 1996 13:44:40   DALLEE
// Removed 2nd #include of svcctrl.h which was outside SYM_WIN32 section.
//
//    Rev 1.24   15 Aug 1996 14:28:14   JBRENNA
// #ifdef out checking that the Auto-Protect NT Service is running for Win95.
// For Win95, the service is running when AP is running. And, the service is
// not running when AP is not running.
//
//    Rev 1.23   06 Aug 1996 10:21:44   PVENKAT
// Fixed # 63081.  We only ask whether start/stop Autoprotect only if necessary
// otherwise we simply provide a message what is going to happen.
//
//
//    Rev 1.22   01 Aug 1996 16:28:56   DSACKIN
// One of the CServiceControl calls was not wrapped correctly around
// the ifdef NAVWNT.
//
//    Rev 1.21   01 Aug 1996 14:25:26   DSACKIN
// Changed calls to ChangeServiceCfg to ChangeServiceConfig.
//
//    Rev 1.20   01 Aug 1996 10:23:02   PVENKAT
// Fixed 62608.  We provide only 'Ok' button on notifying the user of startup
// change of Services.  And if user doesnot want to 'stop/start now', then
// only startup attribute is changed.
//
//
//    Rev 1.19   31 Jul 1996 11:05:50   RPULINT
// Fixed defect 62609: changed message box coresponding to IDS_VXDAUTOLOAD_YES
// from YES_NO_QUESTION to OK_INFORMATION type.
//
//    Rev 1.18   24 Jul 1996 17:02:48   PVENKAT
// Fixed #61716.  Now Service is started and marked Automatic if 'Load at
// startup' is chosen on Autoprotect options and stopped and marked manual
// otherwise.
//
//
//    Rev 1.17   23 Jul 1996 13:38:24   DSACKIN
// Added two new functions for NavRpcSendString, and reworked NavRpcSendCommand
// to take the Protocol, Address and Endpoint for flexibility.
//
//    Rev 1.16   22 Jul 1996 15:22:42   DSACKIN
// Made the NavRpc function generic enough to use for all cases.
//
//    Rev 1.15   19 Jul 1996 13:58:30   DSACKIN
// Added a function for NAVAPSVC to tell NAVW it is starting.
//
//    Rev 1.14   19 Jul 1996 01:35:26   DSACKIN
// Use CNAVAPSVCClient to talk to the NAVAP Service.
//
//    Rev 1.13   28 Jun 1996 12:46:18   MKEATIN
// Added SchedulerIsInstalled()
//
//    Rev 1.12   28 Jun 1996 12:03:08   MKEATIN
// Ported Changes from LuigiPlus
//
//    Rev 1.11   28 May 1996 11:55:28   DSACKIN
// Added a forgotten break.
//
//    Rev 1.10   24 May 1996 08:40:52   jmillar
// change NAVAP command ids from THREAD to PROCESS
//
//    Rev 1.9   22 May 1996 18:11:40   RHAYES
// Round # 2 of Dave Allee's OEM<->Ansi mod.'s in Luigi.
//
//    Rev 1.8   17 May 1996 16:30:46   DSACKIN
// Added a new control code for telling NAVAPSVC to re-load the config.
//
//    Rev 1.7   16 May 1996 11:26:40   DSACKIN
// A control code changed.
//
//    Rev 1.6   01 May 1996 07:57:50   JMILLAR
// TellNAVAPSVCToReload function added in last fix needs to be #ifdef WIN32 -
// it won't build for DOS and won't work for DOS
//
//    Rev 1.5   26 Apr 1996 13:41:44   DSACKIN
// Now, when re-loading the AP configuration, we tell the APService to do it.
//
//    Rev 1.4   23 Apr 1996 21:18:32   DSACKIN
// Changed code to the NT Device Driver to reflect change in APCOMM.H
//
//    Rev 1.3   09 Apr 1996 18:31:26   DSACKING
// Now we will communicate with the NAVAP device driver in NT.
//
//    Rev 1.2   04 Apr 1996 17:30:04   YVANG
// Ported from Kirin.
//
//    Rev 1.2   03 Apr 1996 15:25:32   YVANG
// DBCS enable.
//
//    Rev 1.1   06 Feb 1996 21:20:30   BARRY
// Fix LPBYTE prototype from Unicode changes
//
//    Rev 1.0   30 Jan 1996 15:56:28   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:30   BARRY
// Initial revision.
//
//    Rev 1.29   09 Nov 1995 14:04:18   GWESTER
// Changed GetStartDir() handling of LFNs. If we are running NT and GetStartDir()
// can't lookup an SFN equivalent, it will return the LFN.
//
//    Rev 1.28   03 Nov 1995 20:22:30   GWESTER
// Changed NT-check from compile-time to run-time. We won't have to split this
// DLL until we absolutely have to.
//
//    Rev 1.27   03 Nov 1995 19:55:34   GWESTER
// Changed NT-specific version so that TalkToVxD () does nothing.
//
//
//    Rev 1.26   24 Oct 1995 08:45:06   DALLEE
// Removed last change -- System Inoculation will be removed entirely from
// Gargoyle builds. We should leave the functionality unchanged until changes
// are required.
//
//
//    Rev 1.25   17 Oct 1995 18:21:30   DALLEE
// W32 platform: rename sysinteg database to NAVSYSNT.DAT when running under
// Windows NT.
//
//    Rev 1.24   03 Jul 1995 18:17:50   DALLEE
// Changed OemToAnsi() to NOemToAnsi() cover so DOS doesn't chunk.
//
//    Rev 1.23   01 Jul 1995 16:02:00   Sheathco
// Correction for special character sets, (OEMtoAnsi)
//
//
//    Rev 1.22   20 Jun 1995 21:30:38   MARKL
// Since when does GetModuleFilename() return a string in OEM and not ANSI??
// Got rid of the OemToAnsi() calls immediately following each call to
// GetModuleFilename().  If this is indeed true, there is a TON of source
// to go through in every product and quake.
//
//    Rev 1.21   07 Jun 1995 18:09:52   MARKL
// Now looks for NAVAPW based on its window class since the VxD.
//
//    Rev 1.20   31 May 1995 17:03:20   KEITH
// Allow unload of NAVWAP immediately when turned off in config.
//
//    Rev 1.19   17 May 1995 12:02:02   SZIADEH
// selfcheck code.
//
//    Rev 1.18   11 May 1995 18:46:16   MARKL
// Fixed defect 35187.  Now autolaunches VxD or displays appropriate messages
// if "Load AutoProtect At Startup" is changed.
//
//    Rev 1.17   17 Apr 1995 15:47:16   MFALLEN
// sym_max_path+1
//
//    Rev 1.16   12 Apr 1995 17:15:48   MFALLEN
// Use quake NameShortenFilename() instead of MakePathShorter()
//
//    Rev 1.15   04 Apr 1995 19:38:42   SZIADEH
// changed "NAVSYS.DAT" to "NAVSYSL.DAT"
//
//    Rev 1.14   31 Mar 1995 06:51:42   MFALLEN
// Removed old file validation. The new IsValidLFN or IsValidSFN should be used.
//
//    Rev 1.13   30 Mar 1995 16:42:02   MFALLEN
// GetStartDir() will always return the short name of the startup directory. THis
// is to be consistent with previous releases.
//
//    Rev 1.12   30 Mar 1995 14:01:04   MFALLEN
// Replaced NameGetIllegalChars() with NameReturnIllegalChars()
//
//    Rev 1.11   27 Mar 1995 16:20:38   DALLEE
// Final(?) "fix" to FileIsDOSExecutable().
// Replaced NameWildcardMatch() code which didn't understand full paths, with
// NameReturnExtension() and STRICMP().  Simplistic, maybe.  Working, yes.
//
//    Rev 1.10   22 Mar 1995 15:58:22   MARKL
// Added TalkToVxD.  This routine use to be in NAVW.CPP
//
//    Rev 1.9   16 Mar 1995 14:32:24   DALLEE
// Moved szOptions buffer here where it's required.
// (TOOLS programs weren't linking)
//
//    Rev 1.8   09 Mar 1995 15:48:52   DALLEE
// Removed "windowsx.h" - we pick it up from platform.h with <> brackets.
// Since this comes right from the SDK, we don't want to pick it up
// as a dependency w/ MKDEPS.
//
//    Rev 1.7   03 Mar 1995 16:23:08   DALLEE
// For NAVBOOT (DOS) - Return szOptionsDir rather than program dir from
// GetProgramDir().
//
//    Rev 1.6   10 Feb 1995 10:47:52   MFALLEN
// Fixed for W16
//
//    Rev 1.5   23 Jan 1995 17:44:18   DALLEE
// Added routines to get name of NAVSTART.DAT
//
//    Rev 1.4   20 Jan 1995 19:09:44   MFALLEN
// All the DOS and VXD strings are accessed using the LoadString() functions
// that emulates it's Windows counterpart. All the string ID's are stored in the
// stbexts.h file that identical for all the platforms. DOS and VXD strings are
// contained in the UTILSTR.STR file.
//
//    Rev 1.3   16 Jan 1995 13:16:44   MFALLEN
// Was not compiling for DOS platform.
//
//    Rev 1.2   27 Dec 1994 16:58:12   MFALLEN
// Fixed the FileIsDosExecutable function so it behaves correctly under W32.
//
//    Rev 1.1   22 Dec 1994 09:08:50   MFALLEN
// Patched NavSelfTest while it's under construction.
//
//    Rev 1.0   16 Dec 1994 10:36:48   MFALLEN
// Initial revision.
//
//    Rev 1.14   15 Dec 1994 20:14:12   MFALLEN
// From NIRVANA
//
//    Rev 1.13   16 Oct 1994 22:56:22   DALLEE
// Added call to VirusScanSaveMemoryInfo() on failed self-test.
//
//    Rev 1.12   17 Nov 1993 17:57:20   MFALLEN
// A lot of fixes for the inline.
//
//    Rev 1.11   26 Oct 1993 16:03:32   DALLEE
// Fix to FileIsDOSExecutable().
//
//    Rev 1.10   20 Oct 1993 19:16:06   DALLEE
// Added FileIsDOSExecutable().
//
//    Rev 1.9   14 Oct 1993 16:31:54   MFALLEN
// Fixed casting bug.
//
//    Rev 1.8   07 Oct 1993 17:52:34   MFALLEN
// Make sure we internally deal only with Ansi strings
//
//    Rev 1.7   20 Sep 1993 05:01:30   BRAD
// If value coming in was too large, wouldn't work.
//
//    Rev 1.6   16 Sep 1993 04:39:52   BRAD
// Changed ZIP_SEPARATOR.
//
//    Rev 1.5   23 Aug 1993 00:39:28   BARRY
// Added MODULE_NAME and fixed MemUnlock arguments for VMM debugging
//
//    Rev 1.4   17 Aug 1993 23:25:52   MFALLEN
// New function to shorten path
//
//    Rev 1.3   16 Aug 1993 23:12:22   MFALLEN
// Fix
//
//    Rev 1.2   16 Aug 1993 21:54:04   MFALLEN
// GetSysIntegFile()
//
//    Rev 1.1   16 Aug 1993 14:10:26   MFALLEN
// New parameters to GetSysIntegFile()
//
//    Rev 1.0   12 Aug 1993 11:57:58   MFALLEN
// Initial Revision
//
//    Rev 1.26   27 Jul 1993 19:29:32   MFALLEN
// Removed GetTsrFlag()
//************************************************************************


#include "platform.h"
#include "xapi.h"
#include "navopts.h"

#ifdef SYM_WIN16
    #include "cts4.h"
#else
    #include "ctsn.h"
#endif

#include "navutil.h"
#include "disk.h"
#include "virscan.h"
#include "navapcom.h"

#ifdef SYM_WIN
    #include "ngui.h"
    #include "msgdisp.h"
    #define  TRACE_ID   627
#endif

#ifdef SYM_WIN32
    #include "navwcomm.h"
    #include "drvcomm.h"                // Generic Driver communications

    #include "svcctrl.h"                // For controlling NAVAPSVC
    #include "apscomm.h"                // For communicating with NAVAPSVC
    #include "linkval.h"                // For LINK_ERROR
    #include "apscli.h"                 // For communicating with NAVAPSVC
    #include "navrpc.h"                 // For NAVW RPC endpoints
    #include "navw.h"

    #include "apcomm.h"                 // NAVAP specific stuff.
    #include "navver.h"
#endif

#include "stbexts.h"
#include "dbcs.h"

MODULE_NAME;

//***************************************************************************
// Local defines.
//***************************************************************************
#define     MAX_SERVICE_NAME            (256)

//***************************************************************************
// Statics
//***************************************************************************
static	UINT	uCustomVersion = VERSION_STANDARD;

//***************************************************************************
//***************************************************************************

#ifdef SYM_WIN

//***************************************************************************
// DisableDialog()
//
// Description:
//      This routine disables the child controls of the dialogs
//      comprising the Vishnu control panel.
//
// Parameters:
//      hWnd                    [in] Dialog Handle
//      UINT FAR *              [in] Array of Child window IDs not to disable
//
// Return Value:
//      none
//
//***************************************************************************
// 03/10/1993 SGULLEY Function Created.
// 04/15/1993 Martin  Function Modified to be more generic
//***************************************************************************
VOID SYM_EXPORT WINAPI DisableDialog(       // Disables child controls
    HWND hDlg,                          // [in] Parent dialog handle
    BOOL bDisable,                      // [in] disable children
    UINT FAR *lpSaveChildren)           // [in] Childrens to save
{
   BOOL SYM_EXPORT CALLBACK DisableChildWindow (HWND hChild, LPARAM lParam);
   BOOL SYM_EXPORT CALLBACK EnableChildWindow (HWND hChild, LPARAM lParam);
   extern HINSTANCE hInstance;
   FARPROC lpfn;

   if (bDisable == TRUE)
      lpfn = MakeProcInstance((FARPROC)DisableChildWindow, hInstance);
   else
      lpfn = MakeProcInstance((FARPROC)EnableChildWindow, hInstance);

   if (lpfn)
      {
      EnumChildWindows(hDlg, (WNDENUMPROC)lpfn, (LPARAM)lpSaveChildren);
      FreeProcInstance(lpfn);
      }
}

BOOL SYM_EXPORT CALLBACK DisableChildWindow (HWND hChild, LPARAM lParam)
{
   #define CLASSNAMELEN 25

#ifdef SYM_WIN32
    auto    DWORD  wChildStyle;
#else
    auto    WORD   wChildStyle;
#endif

   auto  BOOL  bFound;
   auto  UINT  FAR *lpSaveChildren;
   auto  UINT  uChildID, i;
   auto  char  szClassName[CLASSNAMELEN + 1];

   lpSaveChildren = (UINT FAR*)lParam;

   if (lpSaveChildren && (uChildID = GetDlgCtrlID(hChild)))
      {
      for (i = 0, bFound = FALSE; lpSaveChildren[i]; i++)
         if (lpSaveChildren[i] == uChildID)
            {
            bFound = TRUE;
            break;
            }

      if (bFound == FALSE)
         {
         EnableWindow(hChild, FALSE);
         GetClassName(hChild, szClassName, CLASSNAMELEN);

         if (!STRCMP((LPSTR)szClassName,"Button"))
            {
#ifdef SYM_WIN32
            wChildStyle = GetClassLong(hChild, GCL_STYLE);
#else
            wChildStyle = GetClassWord(hChild, GCW_STYLE);
#endif
            if (wChildStyle & BS_CHECKBOX              ||
                wChildStyle & BS_AUTOCHECKBOX)
               {
               Button_SetCheck(hChild,FALSE);
               }
            else if (wChildStyle & BS_RADIOBUTTON      ||
                     wChildStyle & BS_AUTORADIOBUTTON)
               {
               Button_SetState(hChild,FALSE);
               }
            }

         if (!STRCMP((LPSTR)szClassName,"ListBox"))
            {
            ListBox_ResetContent(hChild);
            }

         if (!STRCMP((LPSTR)szClassName,"ComboBox"))
            {
            ComboBox_SetText(hChild,"");
            }

         if (!STRCMP((LPSTR)szClassName,"Edit"))
            {
            Edit_SetText(hChild,"");
            }
/*
         if (!STRCMP((LPSTR)szClassName,"Static"))
            {
                                     // make sure were not erasing
                                     // required text
            if (GetDlgCtrlID(hChild) != -1)
               {
               Static_SetText(hChild,"");
               }
            }
*/
         if (!STRCMP((LPSTR)szClassName,"NGui_Tunnel"))
            {
            Tunnel_SetText(hChild,"");
            }
         }
      }

   return (TRUE);
}

BOOL SYM_EXPORT CALLBACK EnableChildWindow (HWND hChild, LPARAM lParam)
{
   #define CLASSNAMELEN 25

#ifdef SYM_WIN32
    auto    DWORD   wChildStyle;
#else
    auto    WORD    wChildStyle;
#endif

   auto  UINT  uChildID;
   auto  char  szClassName[CLASSNAMELEN + 1];

   if (uChildID = GetDlgCtrlID(hChild))
      {
      EnableWindow(hChild, TRUE);
      GetClassName(hChild, szClassName, CLASSNAMELEN);

      if (!STRCMP((LPSTR)szClassName,"Button"))
         {
#ifdef SYM_WIN32
         wChildStyle = GetClassLong(hChild, GCL_STYLE);
#else
         wChildStyle = GetClassWord(hChild, GCW_STYLE);
#endif
         if (wChildStyle & BS_CHECKBOX              ||
             wChildStyle & BS_AUTOCHECKBOX)
            {
            Button_SetCheck(hChild, FALSE);
            }
         else if (wChildStyle & BS_RADIOBUTTON      ||
                  wChildStyle & BS_AUTORADIOBUTTON)
            {
            Button_SetState(hChild, TRUE);
            }
         }
      }

   return (TRUE);
}


#endif                                       // This is an #endif for SYM_WIN //


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID SYM_EXPORT WINAPI HMS_Time(DWORD dwTime, HMS FAR *Hms)
{
   MEMSET(Hms, 0, sizeof(HMS));

   if (dwTime >= 3600)
      {
      Hms->uHour = (UINT)(dwTime / 3600);

      dwTime -= ((DWORD)Hms->uHour * 3600);

      if ( dwTime >= 3600 )
          dwTime = (3600 - 1);
      }

   if (dwTime >= 60)
      {
      Hms->uMinute = (UINT)(dwTime / 60);
      dwTime -= (Hms->uMinute * 60);
      }

   Hms->uSecond = (UINT)dwTime;
}


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****     Returns directory where NAV is located.                    **** //
// ****     Used to determine locations of options files.              **** //
// ****                                                                **** //
// ****     For NAVBOOT - we return the options directory the user     **** //
// ****     selected at startup, not the program directory.            **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

#ifdef SYM_DOS

 #ifdef __cplusplus
 extern "C" {
 #endif

 char    szOptionsDir [SYM_MAX_PATH+1];

 #ifdef __cplusplus
 }
 #endif

#endif

VOID SYM_EXPORT WINAPI GetStartDir (HINSTANCE hInstance, LPSTR lpPath, UINT uSize)
{
#ifndef SYM_DOS
    auto    char    szPath [ SYM_MAX_PATH + 1 ];

    GetModuleFileName ( hInstance, szPath, sizeof(szPath) );
    NameStripFile ( szPath );

    STATUS ShortNameReturnValue = NameReturnShortName ( szPath, lpPath );

    if  (
        (ShortNameReturnValue != NOERR) &&
        (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        )
        {
                                        // If we are running NT, and it
                                        // can't convert the LFN to a
                                        // SFN, return the LFN.
        STRCPY ( lpPath , szPath );
        }
#else
    if (EOS != szOptionsDir[0])
        {
        STRCPY(lpPath, szOptionsDir);
        }
    else
        {
        DOSGetProgramName(lpPath);
        NameStripFile(lpPath);
        }
#endif
}

VOID SYM_EXPORT WINAPI GetSysIntegFile (LPSTR lpPath, UINT uSize, LPINOCOPT lpInoc)
{
    auto    BYTE        BootDrive = DiskGetBootDrive ();
    auto    char        szFilename [ SYM_MAX_PATH+1 ];

    if (BootDrive == 0)
        BootDrive = 'C';

    if (*lpInoc->szNetInocPath && lpInoc->szNetInocPath[1] == ':')
        {
        STRCPY (lpPath, lpInoc->szNetInocPath);
        *lpPath = BootDrive;
        }
    else
        {
        lpPath[0] = BootDrive;
        lpPath[1] = ':';
        lpPath[2] = EOS;
        NameAppendFile(lpPath, lpInoc->szNetInocPath);
        }

    NameAppendFile(lpPath, GetSysIntegFilename(szFilename));
}

// ************************************************************************ //
// ************************************************************************ //
VOID SYM_EXPORT WINAPI GetNavBin(HINSTANCE hInstance, LPSTR lpPath, UINT uSize)
{
    auto        char            szFilename [ SYM_MAX_PATH+1 ];


    GetStartDir (hInstance, lpPath, uSize);
    NameAppendFile(lpPath, GetNavBinFilename(szFilename));
}

// ************************************************************************ //
// ************************************************************************ //
LPSTR SYM_EXPORT WINAPI GetNavBinFilename(LPSTR lpFilename)
{
    return(STRCPY(lpFilename, "NAVOPTS.DAT"));
}


/*@API:**********************************************************************
@Declaration:
VOID SYM_EXPORT WINAPI GetNavStart(HINSTANCE hInstance,
                                   LPSTR lpPath,
                                   UINT uSize)

@Description:
This function returns the full path to NAVSTART.DAT located in the program
directory.

@Parameters:
$hInstance$     Instance of .EXE for program directory.
$lpPath$        Buffer to place path.
$uSize$         size of buffer.

@Returns:       Nothing.
@See:
@Include:       navutil.h
@Compatibility:
****************************************************************************/
// 12/16/94 DALLEE, created.

VOID SYM_EXPORT WINAPI GetNavStart(HINSTANCE hInstance, LPSTR lpPath, UINT uSize)
{
    auto        char            szFilename [SYM_MAX_PATH+1];

    GetStartDir (hInstance, lpPath, uSize);
    NameAppendFile(lpPath, GetNavStartFilename(szFilename));
}

/*@API:**********************************************************************
@Declaration:
VOID SYM_EXPORT WINAPI GetNavexInfFile(HINSTANCE hInstance,
                                       LPSTR lpPath,
                                       UINT uSize)

@Description:
This function returns the full path to NAVEX15.INF located in the program
directory.

@Parameters:
$hInstance$     Instance of .EXE for program directory.
$lpPath$        Buffer to place path.
$uSize$         size of buffer.

@Returns:       Nothing.
@See:
@Include:       navutil.h
@Compatibility:
****************************************************************************/
// 12/16/94 DALLEE, created.

VOID SYM_EXPORT WINAPI GetNavexInfFile(HINSTANCE hInstance, LPSTR lpPath, UINT uSize)
{
    GetStartDir (hInstance, lpPath, uSize);
    NameAppendFile(lpPath, "NAVEX15.INF");
}

/*@API:**********************************************************************
@Declaration:
LPSTR SYM_EXPORT WINAPI GetNavStartFilename (LPSTR lpFilename)

@Description:
This function returns the name of the NAVSTART.DAT startup options file.

@Parameters:
$lpFilename$    Buffer to place name.

@Returns:   Address of filename (lpFilename)
@See:
@Include:   navutil.h
@Compatibility:
****************************************************************************/
// 12/16/94 DALLEE, created.

LPSTR SYM_EXPORT WINAPI GetNavStartFilename(LPSTR lpFilename)
{
    return(STRCPY(lpFilename, "NAVSTART.DAT"));
}


LPSTR SYM_EXPORT WINAPI GetSysIntegFilename(LPSTR lpFilename)
{
    return(STRCPY(lpFilename, "NAVSYSR.DAT"));
}

//************************************************************************
// NavSelfTest()
//
// This routine verifies the program has not changed by checking
// the self-test CRC.
//
// Parameters:
//      HINSTANCE   hInstance           WIN - program instance
//                                      DOS - garbage
//
// Returns:
//      SELF_TEST_SUCCESS    (0)        Passed self-test
//      SELF_TEST_FAIL       (2)        Bad CRC
//      SELF_TEST_FILE_ERROR (3)        Could not open/read executable.
//      1                               Program never CRC stamped.
//      ERR (-1)                        Memory allocation error.
//************************************************************************
// 6/14/93 DALLEE, Function created.
//10/16/94 DALLEE, call VirusScanSaveMemoryInfo() on fail.
//************************************************************************

WORD SYM_EXPORT WINAPI NavSelfTest (HINSTANCE hInstance)
{
#ifdef _ALPHA_
	// WES We don't have certlibn available, so succeed for now.
	return SELF_TEST_SUCCESS;
#else
    auto    LPBYTE      lpBuffer;
    auto    WORD        wResult = ERR;
    auto    char        szProgramName [SYM_MAX_PATH + 1];

#ifdef SYM_WIN
    GetModuleFileName(hInstance, szProgramName, SYM_MAX_PATH);
#else
    DOSGetProgramName(szProgramName);
#endif


    lpBuffer = (LPBYTE) MemAllocPtr (GHND, SIZE_WORK_BUFFER );

    if (lpBuffer != NULL )
        {
        wResult = FileSelfTest(szProgramName, lpBuffer);

        if (SELF_TEST_FAIL == wResult)
            {
            GetInfectionData (lpBuffer,NULL, 0);
            }

        MemFreePtr(lpBuffer);
        }

    return (wResult);

#endif
} // End NavSelfTest()


//************************************************************************
// ParseZipFileName()
//
// Separates a zip file and path (C:\WORK\STUFF.ZIP#FILE.NAM) into
// zip path (C:\WORK\STUFF.ZIP) and file name inside zip (FILE.NAM).
//
// Parameters:
//      LPSTR lpFullPath                Full path to zip, plus name of
//                                          compressed file in zip.
//      LPSTR lpZipPath                 Return buffer for full path to zip.
//      LPSTR lpFileName                Return buffer for compressed filename.
//
// Returns:
//      Nothing
//************************************************************************
// 7/12/93 DALLEE, This header and moved out of #ifdef SYM_WIN section.
//************************************************************************

VOID SYM_EXPORT WINAPI ParseZipFileName (LPSTR lpFullPath, LPSTR lpZipPath,
                                     LPSTR lpFileName)
{
    auto    UINT    i;
    auto    LPSTR   lpStr;

    lpStr = SYMstrrchr ((LPCSTR)lpFullPath, (int)ZIP_SEPARATOR);
    if (NULL != lpStr)
        i = lpStr - lpFullPath;
    else
        i = 0;

    if (lpFullPath[i] == ZIP_SEPARATOR)
        {
        STRNCPY (lpZipPath, lpFullPath, i);
        lpZipPath[i] = EOS;
        STRCPY(lpFileName, &lpFullPath[i+1]);
        }
    else
        {
        *lpZipPath  = EOS;
        *lpFileName = EOS;
        }
}

#ifdef SYM_WIN16

// ---------------------------------------------------------------------------
//  We don't want this function for W32, The quack NameShortenFilename() should
//  be used.
// ---------------------------------------------------------------------------

VOID SYM_EXPORT WINAPI MakePathShorter (LPSTR lpPath, UINT uLevels)
{
    auto    char    szFullPath[SYM_MAX_PATH+1];
    auto    UINT    uSlash = 0;
    auto    UINT    i;

    MEMSET (szFullPath, EOS, sizeof(szFullPath));

    for (i=STRLEN(lpPath); i; i--)
        if (lpPath[i] == '\\')
            uSlash++;

    if (uSlash > uLevels)
        {
        for (i=0, uSlash = 0; uSlash != (uLevels-1); i++)
            {
            if (lpPath[i] == '\\')
                uSlash++;

            szFullPath[i] = lpPath[i];
            }

        STRCAT (szFullPath, "...");

        for (i=STRLEN(lpPath), uSlash=0; uSlash != 1; i--)
            {
            if (lpPath[i] == '\\')
                uSlash++;
            }
        STRCAT (szFullPath, &lpPath[i+1]);

        STRCPY (lpPath, szFullPath);
        }
}

#endif // #ifdef SYM_WIN16

//************************************************************************
// FileIsDOSExecutable()
//
// Returns whether the file extension matches the list of standard DOS
// executables.
//
// Parameters:
//      LPCSTR  lpszFile                Filename to check
//
// Returns:
//      TRUE                            Extension matches std DOS executable
//      FALSE                           Does not match.
//************************************************************************
// 10/19/93 DALLEE, Function created.
//************************************************************************

BOOL SYM_EXPORT WINAPI FileIsDOSExecutable (
    LPSTR   lpszFile )
{
    static  LPSTR   lpExeList[] =
        {
        "COM",
        "EXE",
        NULL
        };

    auto    BOOL    bReturn = FALSE;
    auto    char    szExt [SYM_MAX_PATH+1];

    for ( UINT uIndex = 0; lpExeList[uIndex] && bReturn == FALSE; uIndex++ )
        {
        NameReturnExtensionEx(lpszFile, szExt, sizeof(szExt));

        bReturn = (0 == STRICMP(szExt,  lpExeList[uIndex]));
        }

    return ( bReturn );
}

/* **************************************************************************
@Name:
DWORD SYM_EXPORT WINAPI NavRpcSendPacket( VOID )

@Description:
   Send a block of data referenced by parameter 4, to the endpoint. First
   DWORD of data block must be its size.

@Parameters:
   LPSTR lpszProtocol   - Protocol to use to send the command.
   LPSTR lpszAddress    - Address to contact.
   LPSTR lpszEndpoint   - Endpoint to look for at the Address.
   LPVOID lpPacket      - Pointer to data block to transmit.

@Returns:
   DWORD - Whatever the remote function returns.

@Include: navrpc.h
************************************************************************** */
DWORD SYM_EXPORT WINAPI NavRpcSendPacket(
    LPSTR    lpszProtocol,              // [in] - Protocol to use
    LPSTR    lpszAddress,               // [in] - Address to contact
    LPSTR    lpszEndpoint,              // [in] - Endpoint to look for.
    LPVOID   lpPacket                   // [in] - Data block to send to endpoint
    )
    {
#if defined( SYM_WIN32 )

    auto  DWORD         dwReturn = LINK_ERROR;
    auto  CLinkClient   client;

    client.SetProtocol( lpszProtocol,
                        lpszAddress,
                        lpszEndpoint );

    if ( client.Connect( ) )
        {
        dwReturn = client.SendPacket( lpPacket );

        client.DisConnect( );
        }

    return( dwReturn );

#else

    return( 0 );

#endif
    }


    /* **************************************************************************
@Name:
DWORD SYM_EXPORT WINAPI NavRpcSendCommand (VOID);  // Tell them to exclude their process

@Description:
   This function sends a command to the specified endpoint

@Parameters:
   LPSTR - lpszProtocol - Protocol to use to send the command
   LPSTR - lpszAddress  - Address to contact
   LPSTR - lpszEndpoint - Endpoint to look for at the Address
   DWORD - dwCmd        - Command to send to the Address.

@Returns:
   DWORD - Whatever the remote function returns.

@Include: navrpc.h
************************************************************************** */

DWORD SYM_EXPORT WINAPI NavRpcSendCommand (
   LPSTR    lpszProtocol,               // [in] - Protocol to use
   LPSTR    lpszAddress,                // [in] - Address to contact
   LPSTR    lpszEndpoint,               // [in] - Endpoint to look for.
   DWORD    dwCmd)                      // [in] - Code to send to the endpoint
{
#ifdef SYM_WIN32

   auto  DWORD         dwReturn = LINK_ERROR;
   auto  CLinkClient   client;

   client.SetProtocol(
      lpszProtocol,
      lpszAddress,
      lpszEndpoint);

   if (client.Connect())
   {
      dwReturn = client.SendCommand( dwCmd );

      client.DisConnect();
   }

   return dwReturn;

#else

   return 0;

#endif

}

/* **************************************************************************
@Name:
DWORD SYM_EXPORT WINAPI NavRpcSendString (VOID);

@Description:
   This function sends a command to the specified endpoint

@Parameters:
   LPSTR - lpszProtocol - Protocol to use to send the command
   LPSTR - lpszAddress  - Address to contact
   LPSTR - lpszEndpoint - Endpoint to look for at the Address
   LPSTR - lpszString   - String to send to the Address.

@Returns:
   DWORD - Whatever the remote function returns.

@Include: navrpc.h
************************************************************************** */

DWORD SYM_EXPORT WINAPI NavRpcSendString (
   LPSTR    lpszProtocol,               // [in] - Protocol to use
   LPSTR    lpszAddress,                // [in] - Address to contact
   LPSTR    lpszEndpoint,               // [in] - Endpoint to look for.
   LPSTR    lpszString)                 // [in] - String to send to the endpoint
{
#ifdef SYM_WIN32

   auto  DWORD         dwReturn = LINK_ERROR;
   auto  CLinkClient   client;

   client.SetProtocol(
      lpszProtocol,
      lpszAddress,
      lpszEndpoint);

   if (client.Connect())
   {
      dwReturn = client.SendString( lpszString );

      client.DisConnect();
   }

   return dwReturn;

#else

   return 0;

#endif

}

/* **************************************************************************
@Name:
DWORD SYM_EXPORT WINAPI NavRpcSendStringW (VOID);

@Description:
   This function sends a command to the specified endpoint

@Parameters:
   LPSTR -  lpszProtocol - Protocol to use to send the command
   LPSTR -  lpszAddress  - Address to contact
   LPSTR -  lpszEndpoint - Endpoint to look for at the Address
   LPWSTR - lpszStringW  - UNICODE String to send to the Address.

@Returns:
   DWORD - Whatever the remote function returns.

@Include: navrpc.h
************************************************************************** */

DWORD SYM_EXPORT WINAPI NavRpcSendStringW (
   LPSTR    lpszProtocol,               // [in] - Protocol to use
   LPSTR    lpszAddress,                // [in] - Address to contact
   LPSTR    lpszEndpoint,               // [in] - Endpoint to look for.
   LPWSTR   lpszStringW)                // [in] - UNICODE String to send
{
#ifdef SYM_WIN32

   auto  DWORD         dwReturn = LINK_ERROR;
   auto  CLinkClient   client;

   client.SetProtocol(
      lpszProtocol,
      lpszAddress,
      lpszEndpoint);

   if (client.Connect())
   {
      dwReturn = client.SendString( lpszStringW );

      client.DisConnect();
   }

   return dwReturn;

#else

   return 0;

#endif

}

/* **************************************************************************
@Name:
VOID SYM_EXPORT WINAPI TalkToVxD (      // Communicate with VXD
    VXDACTION   wVxDAction              // [in] action to perform
    );

@Description:
This function communicates with the NAV AP VXD.

@Parameters:
$wVxDAction$  Action indicator specifying to message to send.
              At present time it can any of the following:

              VXDACTION_ENABLE:         To enable the VXD for this thread
              VXDACTION_DISABLE:        To disable the VXD for this thread
              VXDACTION_RELOAD_CONFIG:  To have VXD reload options and defs.
              NAVAPSVC_RELOAD_CONFIG:   To have AP service reload options.
              NAVAPSVC_ENABLE_DRIVER,   To globally enable NAVAP driver
              NAVAPSVC_DISABLE_DRIVER   To globally disable NAVAP driver

@Include: navapcom.h
************************************************************************** */

VOID SYM_EXPORT WINAPI TalkToVxD (      // Communicate with VxD
    VXDACTION   wVxDAction              // [in] Action to tell VxD to do
    )
{
// Win32 only code. Called as stub function from other platforms, though.

#ifdef SYM_WIN32
    auto    DriverComm    commNAVAP;

                                    // Tell the Auto-Protect what we want it
                                    // to do.
    switch ( wVxDAction )
        {
        case VXDACTION_ENABLE:
            if ( commNAVAP.Open( NAVAP_COMMUNICATION_NAME ) )
                commNAVAP.Send( NAVAP_COMM_PROTECTPROCESS );
            break;

        case VXDACTION_DISABLE:
            if ( commNAVAP.Open( NAVAP_COMMUNICATION_NAME ) )
                commNAVAP.Send( NAVAP_COMM_UNPROTECTPROCESS );
            break;

        case VXDACTION_RELOAD_CONFIG:
                                        // Prevent obscene ( ~5 second )
                                        // timeout on RPC calls to missing
                                        // agent on Win 95.
            if ( ( SYM_SYSTEM_WIN_95 == SystemGetWindowsType() ) &&
                 ( NULL == FindWindow( NAVAP_95_WND_CLASS, NULL ) ) )
                {
                break;
                }
                                        // Use our RPC call to send commands.
            NavRpcSendCommand(
              NAVAPSVC_PROTOCOL,
              NAVAPSVC_ADDRESS,
              NAVAPSVC_ENDPOINT,
              NAVAPSVC_RELOADCONFIG);
            break;

        case NAVAPSVC_RELOAD_CONFIG:
                                        // Prevent obscene ( ~5 second )
                                        // timeout on RPC calls to missing
                                        // agent on Win 95.
            if ( ( SYM_SYSTEM_WIN_95 == SystemGetWindowsType() ) &&
                 ( NULL == FindWindow( NAVAP_95_WND_CLASS, NULL ) ) )
                {
                break;
                }
                                        // Use our RPC call to send commands.
            NavRpcSendCommand(
              NAVAPSVC_PROTOCOL,
              NAVAPSVC_ADDRESS,
              NAVAPSVC_ENDPOINT,
              NAVAPSVC_RELOADOPTIONS);
            break;

        case NAVAPSVC_ENABLE_DRIVER:
                                        // Prevent obscene ( ~5 second )
                                        // timeout on RPC calls to missing
                                        // agent on Win 95.
            if ( ( SYM_SYSTEM_WIN_95 == SystemGetWindowsType() ) &&
                 ( NULL == FindWindow( NAVAP_95_WND_CLASS, NULL ) ) )
                {
                break;
                }
                                        // Use our RPC call to send commands.
            NavRpcSendCommand(
              NAVAPSVC_PROTOCOL,
              NAVAPSVC_ADDRESS,
              NAVAPSVC_ENDPOINT,
              NAVAPSVC_AP_ENABLE);
            break;

        case NAVAPSVC_DISABLE_DRIVER:
                                        // Prevent obscene ( ~5 second )
                                        // timeout on RPC calls to missing
                                        // agent on Win 95.
            if ( ( SYM_SYSTEM_WIN_95 == SystemGetWindowsType() ) &&
                 ( NULL == FindWindow( NAVAP_95_WND_CLASS, NULL ) ) )
                {
                break;
                }
                                        // Use our RPC call to send commands.
            NavRpcSendCommand(
              NAVAPSVC_PROTOCOL,
              NAVAPSVC_ADDRESS,
              NAVAPSVC_ENDPOINT,
              NAVAPSVC_AP_DISABLE);
            break;

        default:
            SYM_ASSERT ( 0 );
            break;
        }
#endif
}



/* **************************************************************************
@Name:
VOID SYM_EXPORT WINAPI NavOptsLoadVxDChanged (
    HWND        hWnd,                   // [in] Handle of parent window
    BOOL        bNewVxDLoad);           // [in] New state of VxD load with WIN

@Description:
This function handles the case where the user has changed whether or not
the VxD is automatically loaded with Windows.

@Parameters:
$hWnd$          [in] Handle of parent window.
$bNewVxDLoad$   [in] The new setting for whether or not the VxD should be
                     automatically loaded with windows.

@Include: navutil.h
************************************************************************** */

VOID SYM_EXPORT WINAPI NavOptsLoadVxDChanged (
    HWND        hWnd,                   // [in] Handle of parent window
    BOOL        bNewVxDLoad)            // [in] New state of VxD load with WIN
{
#ifdef SYM_WIN32
    auto   BOOL   bVxDCurrentlyRunning  = FALSE;
    auto   BOOL   bApAgentRunning       = FALSE;
    auto   WORD   wLoadVxD              = IDNO;
    auto   char   szCaption [80];
    extern HINSTANCE hInstance;
    auto   DWORD  dwState = 0L;
    auto   LPCTSTR lpszNavapWndClass;

    lpszNavapWndClass = SYM_SYSTEM_WIN_95 == SystemGetWindowsType() ?
                                NAVAP_95_WND_CLASS : NAVAP_WND_CLASS;

#ifdef  NAVWNT
    TCHAR           szServiceName[MAX_SERVICE_NAME + 1];    // NAVAP Service
    CServiceControl svc;
    TCHAR           szFmt[80];
    TCHAR           szErrMsg[256];

                                        // Load our caption.
    LoadString (hInstance,
                    IDS_NAVAP_SERVICE_NAME,
                    szServiceName,
                    sizeof (szServiceName));


    if (svc.Open (szServiceName) != ERROR_SUCCESS)
    {
        LoadString (hInstance,
                    IDS_ERR_NAVAP_OPEN_EX,
                    szErrMsg,
                    sizeof (szErrMsg));
        MessageBox (hWnd, szErrMsg, szServiceName, MB_OK);
		return;
    }
#endif

    LoadString (hInstance, IDS_VXDAUTOLOAD_CAPTION,
                szCaption, sizeof(szCaption));

                                        // See if the AP Agent is currently
                                        // running.
    if (FindWindow ( lpszNavapWndClass, NULL ) != NULL)
        bApAgentRunning = TRUE;

#ifdef NAVWNT
    // See whether the AP Service is running now.
    svc.QueryServiceStatus (&dwState);
    if (dwState == SERVICE_RUNNING)
        bVxDCurrentlyRunning = TRUE;
#else
    // Win95: In order for the "if" to correctly process, we need
    // bVxDCurrentlyRunning to match bApAgentRunning.
    bVxDCurrentlyRunning = bApAgentRunning;
#endif

                                        // Prompt the user appropriately.
    if ( bNewVxDLoad )
    {
                                        // If the user now wants to have the
                                        // VxD automatically loaded with
                                        // Windows, give the appropriate
                                        // prompt.
        if ( bVxDCurrentlyRunning && bApAgentRunning)
            {
            NMsgBox (hInstance,
                     hWnd,
                     ERR_SEV_NUM (ERR_SEV_WARNING, IDS_VXDAUTOLOAD_YES),
                     TRACE_ID,
                     szCaption,
                     MB_OK | MB_ICONINFORMATION,
                     NULL);
            wLoadVxD = IDYES;
            }
        else
            wLoadVxD = NMsgBox (hInstance,
                                hWnd,
                                ERR_SEV_NUM (ERR_SEV_WARNING, IDS_VXDAUTOLOAD_YES_ASKLOADNOW),
                                TRACE_ID,
                                szCaption,
                                MB_YESNO | MB_ICONQUESTION,
                                NULL);

        if (wLoadVxD == IDYES)
        {
            // Start the AP Agent.
            if (!bApAgentRunning)
            {

                                            // Compute the path to auto-protect
                                            // and launch it!
                auto    char    szFullPath [SYM_MAX_PATH];
                auto    char    szAutoProtect [SYM_MAX_PATH];

                GetStartDir (hInstance, szFullPath, sizeof(szFullPath));
                LoadString (hInstance, IDS_AUTOPROTECT, szAutoProtect, sizeof(szAutoProtect));
                NameAppendFile (szFullPath, szAutoProtect);
                SetCursor (LoadCursor(NULL, IDC_WAIT));
                WinExec (szFullPath, SW_SHOW);
            }
#ifdef  NAVWNT
            if (!bVxDCurrentlyRunning)
            {
                if (svc.Start () != ERROR_SUCCESS)
                {
                    LoadString (hInstance,
                                IDS_ERR_NAVAP_START,
                                szFmt,
                                sizeof (szFmt));
                    SPRINTF (szErrMsg, szFmt, szServiceName);
                    MessageBox (hWnd, szErrMsg, szCaption, MB_OK);
                }
            }
#endif      // NAVWNT
        }
#ifdef  NAVWNT
        // What ever the user selection we need to change the
        // startup type.
        svc.ChangeServiceConfig_StartType (SERVICE_AUTO_START);
#endif
        SetCursor (LoadCursor(NULL, IDC_ARROW));
    }
    else
    {
                                        // If the user now doesn't want the
                                        // VxD automatically loaded with
                                        // Windows, give the appropriate
                                        // prompt.
        if ( bVxDCurrentlyRunning || bApAgentRunning)
        {

            wLoadVxD = NMsgBox (hInstance,
                                hWnd,
                                ERR_SEV_NUM (ERR_SEV_WARNING, IDS_VXDAUTOLOAD_NO_NOWLOADED),
                                TRACE_ID,
                                szCaption,
                                MB_YESNO | MB_ICONQUESTION,
                                NULL);

            if ( wLoadVxD == IDYES )
            {
                                        // Unload Autoprotect and VxD.
                HWND    hNavapW        = FindWindow ( lpszNavapWndClass, NULL );
                UINT    uNavApWMessage = 0L;

#ifdef  NAVWNT
                if (bVxDCurrentlyRunning)
                {
                    if (svc.Stop () != ERROR_SUCCESS)
                    {
                        LoadString (hInstance,
                                IDS_ERR_NAVAP_STOP,
                                szFmt,
                                sizeof (szFmt));
                        SPRINTF (szErrMsg, szFmt, szServiceName);
                        MessageBox (hWnd, szErrMsg, szCaption, MB_OK);
                    }
                }
#endif
                // Kill the AP Agent only if it is running.
                if (hNavapW)
                {
                    uNavApWMessage = RegisterWindowMessage (NAVW_NAVAPW_COMM);

                    if ( uNavApWMessage != NULL )
                        PostMessage ( hNavapW,
                                  uNavApWMessage,
                                  NAVW_CMD_UNLOADNAVAPW,
                                  0L);
                }
            }
        }
        else
        {
            NMsgBox (hInstance,
                     hWnd,
                     ERR_SEV_NUM (ERR_SEV_WARNING, IDS_VXDAUTOLOAD_NO),
                     TRACE_ID,
                     szCaption,
                     MB_OK | MB_ICONINFORMATION,
                     NULL);
        }
        // What ever the user selection we need to change the
        // startup type.
#ifdef  NAVWNT
        svc.ChangeServiceConfig_StartType (SERVICE_DEMAND_START);
#endif
    }
#ifdef  NAVWNT
    svc.Close ();
#endif
#endif      // SYM_WIN32
}


#ifdef SYM_WIN32

//*************************************************************************
// WINAPI LoadAutoProtect()
//
// WINAPI LoadAutoProtect( )
//
// This routine loads the auto-protect service and agent.
// Works for both NT and 95.
//
// Returns:
//      TRUE on success. Otherwise, FALSE.
//*************************************************************************
// 5/11/98 DALLEE, created - header added.
//*************************************************************************

BOOL SYM_EXPORT WINAPI LoadAutoProtect()
{
    auto    LPCTSTR         lpszNavapWndClass;

    auto    TCHAR           szServiceName [ MAX_SERVICE_NAME + 1 ];
    auto    DWORD           dwServiceState;
    auto    CServiceControl svc;

    auto    TCHAR           szAgentName [ SYM_MAX_PATH ];
    auto    TCHAR           szAgentPath [ SYM_MAX_PATH ];

    auto    BOOL            bDriverRunning;
    auto    BOOL            bAgentRunning;
    auto    HCURSOR         hOldCursor;

    extern  HINSTANCE       hInstance;

    bDriverRunning = FALSE;
    bAgentRunning  = FALSE;

    // Wait cursor.

    hOldCursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );

    // Start Agent.

    lpszNavapWndClass = ( SYM_SYSTEM_WIN_95 == SystemGetWindowsType() ?
                                NAVAP_95_WND_CLASS : NAVAP_WND_CLASS );

    bAgentRunning = ( NULL != FindWindow( lpszNavapWndClass, NULL ) );

    if ( !bAgentRunning )
    {
        if ( LoadString( hInstance,
                         IDS_AUTOPROTECT,
                         szAgentName,
                         sizeof( szAgentName ) / sizeof( TCHAR ) - 1 ) )
        {
            GetStartDir( hInstance, szAgentPath, sizeof( szAgentPath ) / sizeof( TCHAR ) );

            NameAppendFile( szAgentPath, szAgentName );
            bAgentRunning = ( HINSTANCE_ERROR < ( UINT ) ShellExecute( NULL,
                                                                       _T("open"),
                                                                       szAgentPath,
                                                                       NULL,
                                                                       NULL,
                                                                       SW_SHOWNORMAL ) );
        }
    }

    // On NT, need to start services. 95 only starts single agent.

    if ( SYM_SYSTEM_WIN_95 == SystemGetWindowsType() )
    {
        bDriverRunning = bAgentRunning;
    }
    else
    {
        if ( LoadString( hInstance,
                         IDS_NAVAP_SERVICE_NAME, 
                         szServiceName, 
                         sizeof( szServiceName ) / sizeof( TCHAR ) - 1 ) )
        {
            if ( ERROR_SUCCESS == svc.Open( szServiceName ) )
            {
                svc.QueryServiceStatus( &dwServiceState );

                bDriverRunning = ( SERVICE_RUNNING == dwServiceState );

                if ( !bDriverRunning )
                {
                    bDriverRunning = ( ERROR_SUCCESS == svc.Start() );
                }

                svc.Close();
            }
        }
    }

    // Restore cursor.

    SetCursor( hOldCursor );

    return ( bAgentRunning && bDriverRunning );
} // LoadAutoProtect()

#endif

#ifdef SYM_WIN

/* **************************************************************************
@Name: BOOL SYM_EXPORT WINAPI SetCustomVersion ( VOID )

@Description:
This function checks the Registry to determine if this is a special custom version
of the AntiVirus. The static uCustomVersion is set as a result.

@Parameters:
    NONE
************************************************************************** */
BOOL SYM_EXPORT WINAPI SetCustomVersion ( VOID )
{
										// By default :
	uCustomVersion = VERSION_STANDARD;

										// Check for the Netscape custom version :
	char szRegValue [SYM_MAX_PATH];

	CFGREGKEY cfg;
	cfg.hkey = HKEY_LOCAL_MACHINE;
	cfg.lpszProduct = NAV_INSTALLKEY_PRODUCT;
	cfg.lpszVersion = NAV_INSTALLKEY_FUNC;
	cfg.lpszProject = NAV_INSTALLKEY_VER;
	cfg.lpszSection = NULL;
	cfg.lpszEntry = NULL;
	cfg.lpszName = NAV_INSTALLKEY_CUSTOMVERSION;
	cfg.lpszSwitch = NULL;

	auto LONG  lRegResult;
	auto DWORD dwType, dwValueSize;
	dwType = REG_SZ;
	dwValueSize = sizeof(szRegValue);

	lRegResult =
		ConfigRegQueryValue ( &cfg, NULL, &dwType, (LPBYTE)szRegValue, &dwValueSize );

	if ((lRegResult == ERROR_SUCCESS) && (dwType == REG_SZ))
		{
		if (STRICMP ( szRegValue , NAV_INSTALLKEY_NETSCAPE ) == 0)
			{
										// Netscape value was found:
			uCustomVersion = VERSION_NETSCAPE;
			}
		}

return TRUE;
}                                       // End SetCustomVersion()



/* **************************************************************************
@Name: UINT SYM_EXPORT WINAPI GetCustomVersion ( VOID )

@Description:
Any module can call this function to find out at runtime whether this
application is a custom version of the AntiVirus.

@Parameters:
    NONE

@Return:
	VERSION_STANDARD					// Standard Norton version
	VERSION_NETSCAPE					// Netscape version

************************************************************************** */
UINT SYM_EXPORT WINAPI GetCustomVersion ( VOID )
{
	return uCustomVersion;
}                                       // End GetCustomVersion()


/* **************************************************************************
@Name: BOOL SYM_EXPORT WINAPI SchedulerIsInstalled ( VOID )

@Description:
This function returns TRUE if the Scheduler is installed. Under W95,
it returns TRUE if NSCHED32.EXE is present in the install directory;
under NT, it returns TRUE if NAVNTSCH.DLL is present.

@Parameters:
    NONE
************************************************************************** */
BOOL SYM_EXPORT WINAPI SchedulerIsInstalled ( VOID )
{
    extern          HINSTANCE           hInstance;
    auto            char                szWork [ SYM_MAX_PATH+1 ];
    auto            char                szFile [ SYM_MAX_PATH+1 ];

    GetStartDir (hInstance, szFile, sizeof(szFile));

    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_95)
        {
                                        // Running under Windows 95
        LoadString (hInstance, IDS_SCHEDULER, szWork, sizeof(szWork));
        }
    else
        {
                                        // Running under NT
        LoadString (hInstance, IDS_SCHEDULERNT, szWork, sizeof(szWork));
        }

    NameAppendFile(szFile, szWork);

                                        // Determine if szFile present:

    FINDDATA UnusedRec;                 // Required for Quake call
    return ((FileFindFirst(szFile, &UnusedRec)) != INVALID_HANDLE_VALUE);

}                                       // End SchedulerIsInstalled()

#endif // SYM_WIN
