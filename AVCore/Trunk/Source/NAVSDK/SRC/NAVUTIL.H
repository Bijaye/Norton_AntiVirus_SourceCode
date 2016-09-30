// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVSDK/VCS/NAVUTIL.H_v   1.0   02 Dec 1997 14:23:38   DALLEE  $
//
// Description:
//      Constants, typedefs, and prototypes for NAV WIN and DOS utilities.
//
// Contains:
//************************************************************************
// $Log:   S:/NAVSDK/VCS/NAVUTIL.H_v  $
// 
//    Rev 1.0   02 Dec 1997 14:23:38   DALLEE
// Initial revision.
// 
//    Rev 1.1   26 Jun 1996 10:07:02   TCASHIN
// Changes for talking to the AP service under NT.
// 
//    Rev 1.0   09 Apr 1996 19:27:22   TCASHIN
// Initial revision.
// 
//    Rev 1.164.1.0   04 Mar 1996 16:58:28   RJACKSO
// Branch base for version LUG3
// 
//    Rev 1.164   06 Feb 1996 19:57:40   GWESTER
// Moved SchedulerIsInstalled() from .EXE to here .
// 
//    Rev 1.163   31 Jan 1996 14:43:58   GWESTER
// Moved GetCustomVersion(), SetCustomVersion() from N32SCANW.EXE to NNTWORK.DLL .
// 
//    Rev 1.162   14 Jan 1996 16:01:46   DALLEE
// Changes NAVWNT #defines to NAVSCANW.
// 
//    Rev 1.161   12 Dec 1995 19:24:04   GWESTER
// Added RegistryStampLastScanTime(), which stores the time when all of the
// systems's local drives were last scanned in the Registry. This is only for
// the NT platform, but it can easily be ported into W95.
// 
//    Rev 1.160   30 Nov 1995 22:27:42   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
// 
//    Rev 1.159   16 Oct 1995 18:22:48   JWORDEN
// Add wPlatform field to each INI structure
// 
//    Rev 1.158   14 Sep 1995 09:30:26   JWORDEN
// Remove typedef for SECTIONSUFFIX structure - moved to options.h
// 
//    Rev 1.157   13 Sep 1995 10:43:38   JWORDEN
// Add structure typedef for SECTION SUFFIX
// Alter prototypes for GetxxxIni to pass SectionSuffix info
// Add prototypes for ReadConfigValue and ReadConfigString
// 
//    Rev 1.156   10 Jul 1995 14:14:16   MARKL
// NavHaltSystem() now accessable to VxD.
// 
//    Rev 1.155   07 Jul 1995 11:17:46   MARKL
// Added NavHaltSystem()
// 
//    Rev 1.154   01 Jul 1995 14:24:26   KEITH
// Changed ZIP_SEPARATOR from a plus (+) to a pipeline (|) 
// character since + is allowed in an LFN in Win95.
// 
//    Rev 1.153   23 May 1995 11:26:32   MARKL
// Modified prototype such that SYM_VXD platform gets it as well.
// 
//    Rev 1.152   22 May 1995 21:16:36   MARKL
// Added RegistryAddVirusDefDate()
// 
//    Rev 1.151   18 May 1995 10:17:34   KEITH
// Remove duplicate prototype for DriveAccessProblem.
// 
//    Rev 1.150   11 May 1995 18:45:30   MARKL
// Fixed defect 35187.  Now autolaunches VxD or displays appropriate messages
// if "Load AutoProtect At Startup" is changed.
// 
//    Rev 1.149   24 Apr 1995 14:53:30   MFALLEN
// Added auto-protect registry functions. Not debugged yet due to build system
// limitations.
// 
//    Rev 1.148   19 Apr 1995 15:56:30   MFALLEN
// Added w95stuff
// 
//    Rev 1.147   12 Apr 1995 17:15:56   MFALLEN
// Use MakePathShorter() only for Win16 platform.
// 
//    Rev 1.146   05 Apr 1995 11:21:00   MFALLEN
// Added NavReturnIllegalChars()
// 
//    Rev 1.145   31 Mar 1995 06:52:20   MFALLEN
// Removed old validation code. New IsValidSFN or IsValidLFN should be used.
// 
//    Rev 1.144   24 Mar 1995 17:49:42   MFALLEN
// Added help to confirmation Yes/No dialog box.
// 
//    Rev 1.143   22 Mar 1995 15:58:00   MARKL
// Added TalkToVxD for windows.  Moved from original location in NAVW.CPP
// 
//    Rev 1.142   21 Mar 1995 12:02:20   MFALLEN
// 
//    Rev 1.141   16 Mar 1995 18:51:56   AMATIC
// 
//    Rev 1.140   16 Mar 1995 17:44:04   AMATIC
// 
//    Rev 1.139   12 Mar 1995 16:59:20   AMATIC
// Sync. with new Stdhelp.
// navutil.h -> S:\include\vcs\navutil.h_v
// Enter description of changes made.
// 
//    Rev 1.138   10 Mar 1995 17:17:30   AMATIC
// 
//    Rev 1.137   10 Mar 1995 16:26:02   AMATIC
// 
//    Rev 1.136   08 Mar 1995 16:10:40   DALLEE
// Moved DOS_STRINGS struct here for DOS LoadString() cover.
// 
//    Rev 1.135   21 Feb 1995 09:48:38   AMATIC
// Removed Virlist Dlg support.
// 
//    Rev 1.134   20 Feb 1995 17:46:32   AMATIC
// Added Prototype for SettingHelpId for VirList Dialog.
// 
//    Rev 1.133   15 Feb 1995 15:30:02   AMATIC
// Added declaration for commom printer dialog context sensitive help support.
// 
// 
//    Rev 1.132   10 Feb 1995 14:18:50   AMATIC
// Added function prototype for context help function.
// 
//    Rev 1.131   06 Feb 1995 15:55:44   DALLEE
// UpdateTSR() now takes lpNavOptions parameter to build custom TSR's.
// Still limited to 5k TSR beneath this level in InsertSmallOVL().
// 
//    Rev 1.130   01 Feb 1995 17:31:46   RSTANEV
// ReadNavStartFromOpenFile() now returns BOOL.
//
//    Rev 1.129   01 Feb 1995 15:50:52   RSTANEV
// Sleepy change...
//
//    Rev 1.128   01 Feb 1995 15:47:38   RSTANEV
// Added prototype for ReadNavStartFromOpenFile().
//
//    Rev 1.127   23 Jan 1995 17:39:40   DALLEE
// Prototypes for read/write NAVSTART.DAT
//
//    Rev 1.126   20 Jan 1995 19:12:04   MFALLEN
// Added LoadString() function for DOS platform.
//
//    Rev 1.125   19 Jan 1995 18:09:44   RSTANEV
// Added prototype of ReadNavBinFromOpenedFile().
//
//    Rev 1.124   29 Dec 1994 09:14:30   MARKL
// Updated to reflect Brad's changes.  Now includes ctsn.h.
//
//    Rev 1.123   16 Dec 1994 13:59:36   MFALLEN
// All the drive problem structures, defines and other stuff is in drvprblm.h
//
//    Rev 1.122   16 Dec 1994 13:50:58   MFALLEN
// RestoreFile function is exported.
//
//    Rev 1.121   16 Dec 1994 10:44:22   RSTANEV
// Added prototype of BackupInfectedFile().
//
//    Rev 1.120   16 Dec 1994 10:24:16   MFALLEN
// Use ctsn for W32
//
//    Rev 1.119   15 Dec 1994 20:14:40   MFALLEN
// From nirvana
//
//    Rev 1.75   23 Mar 1994 10:59:26   DALLEE
// Added LOGEV_CONFIGCHANGE for Bankers Trust version.
//
//    Rev 1.74   08 Mar 1994 11:12:46   DALLEE
// Added GetRegistrationInfo() prototype for BANKERS.
//
//    Rev 1.73   01 Mar 1994 12:09:58   DALLEE
// Added new PROCESS_REGISTER #define for BANKERS.
//
//    Rev 1.72   17 Nov 1993 17:52:02   MFALLEN
// A lot of fixes for the inline.
//
//    Rev 1.71   30 Oct 1993 01:57:44   BARRY
// Add bYesDisabled/bNoDisabled flags to YESNOSTRUCT
//
//    Rev 1.70   20 Oct 1993 19:10:58   DALLEE
//
//    Rev 1.69   15 Oct 1993 12:28:20   MFALLEN
// Added ComputeKilobytesScanned()
//
//    Rev 1.68   14 Oct 1993 20:04:00   DALLEE
// Make DOS cover for NetGetCurrentUserName() global -- it's used
// by bldlog.c as well as netalert.c
//
//    Rev 1.67   16 Sep 1993 04:32:52   BRAD
// Added ZIP_SEPARATOR.
//
//    Rev 1.66   15 Sep 1993 13:56:44   REESE
// Added DEFAULT_NAV_BIN_HEADER definition.
//
//    Rev 1.65   10 Sep 1993 17:59:44   MFALLEN
// Added WORD_GET, _SET, _FREE functions.
//
//    Rev 1.64   05 Sep 1993 20:17:20   DALLEE
// Added error return codes for logging functions.
//
//    Rev 1.63   03 Sep 1993 19:08:42   MFALLEN
// CAREY's new function was added that figures out the size of the items to scan.
//
//    Rev 1.62   02 Sep 1993 22:50:42   MFALLEN
//
//    Rev 1.61   02 Sep 1993 22:41:58   MFALLEN
// Moved callback pointer to symalog.h
//
//    Rev 1.60   01 Sep 1993 21:42:56   MFALLEN
// Made NAVW look and act as NAV when a disk cant be accessed.
// navutil.h -> S:\source\include\vcs\navutil.h_v
// Enter description of changes made.
//
//    Rev 1.59   25 Aug 1993 17:31:26   MFALLEN
// Added scan interrupted activity loging.
//
//    Rev 1.58   19 Aug 1993 14:10:26   MFALLEN
// Added yesno()
//
//    Rev 1.57   17 Aug 1993 23:26:16   MFALLEN
// New proto
//
//    Rev 1.56   16 Aug 1993 18:10:38   MFALLEN
// Removed askzap.h since it's not being used.
//
//    Rev 1.55   16 Aug 1993 14:10:02   MFALLEN
// New parameters to GetSysIntegFile()
//
//    Rev 1.54   12 Aug 1993 21:50:46   MFALLEN
// Removed tsr update structure. Now the code resides in NAVUTIL.DLL
// navutil.h -> S:\source\include\vcs\navutil.h_v
// Enter description of changes made.
//
//    Rev 1.53   11 Aug 1993 19:12:04   DALLEE
// NAVDWriteLog prototype  to NAVDPROT.H
//
//    Rev 1.52   11 Aug 1993 18:12:24   DALLEE
// Prototype for NAVDWriteLog().
//
//    Rev 1.51   10 Aug 1993 21:29:10   MFALLEN
// Log with UI
//
//    Rev 1.50   06 Aug 1993 23:17:18   MFALLEN
// Halt CPU activity loging.
//************************************************************************


#ifndef _NAVWUTIL_H
#define _NAVWUTIL_H

// ---------------------------------------------------------------------------


                                        // -----------------------------------
                                        // Actions to the TalkToVxd Function
                                        // -----------------------------------
typedef enum
    {
    VXDACTION_DISABLE = 0,
    VXDACTION_ENABLE,
    VXDACTION_RELOAD_CONFIG
#ifdef SYM_WIN32
    ,NAVAPSVC_RELOAD_CONFIG
#endif
        } VXDACTION;

// ******************************* //
// Miscellaneous windows functions //
// ******************************* //

VOID SYM_EXPORT WINAPI TalkToVxD (      // Communicate with VxD
    VXDACTION   wVxdAction              // [in] Action to tell VxD to do
    );

#endif


