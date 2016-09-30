// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header: /Atomic/Shared/Include/navutil.h 2     12/30/98 10:38a Cbenson $
//
// Description:
//      Constants, typedefs, and prototypes for NAV WIN and DOS utilities.
//
// Contains:
//************************************************************************
// $Log: /Atomic/Shared/Include/navutil.h $
// 
// 2     12/30/98 10:38a Cbenson
// updated in 33g
// 
//    Rev 1.20   22 Sep 1998 22:13:10   tcashin
// Added log events for scan start, dir access denied, and file access denied.
// 
//    Rev 1.19   17 Sep 1998 15:51:20   tcashin
// Fixed LPNONAV in LOGSTRUCTEX to LPVOID.
// 
//    Rev 1.18   17 Sep 1998 14:31:34   tcashin
// Added new LogEventEx(), LOGSTRUCTEX, and RegistryCheckLastScanUpdateKey().
// 
//    Rev 1.17   18 May 1998 20:12:46   DALLEE
// ifdef SYM_WIN32 around objbase.h and IQuarantine include.
// 
//    Rev 1.16   18 May 1998 21:24:02   tcashin
// Changed the prototypes for CreateBackup() and RestoreFile() to pass
// a pointer to a REPAIRFILESTRUCT. Added a pointer to a IQuarantineItem
// in the REPAIRFILESTRUCT.
// 
//    Rev 1.15   11 May 1998 16:52:22   DALLEE
// LoadAutoProtect() prototype. Starts Agent and Service, 95 and NT.
// 
//    Rev 1.14   28 Apr 1998 02:00:00   DALLEE
// Added new messages for TalkToVxD() to globally enable/disable driver.
// 
//    Rev 1.13   18 Mar 1998 16:03:34   DALLEE
// Removed UpdateQuarantineFile() prototype. This was a leftover from
// an early aborted implementation of quarantine.
// 
//    Rev 1.12   05 Jan 1998 17:56:46   MDUNN
// Added GetLastAccessDateNT() as part of the fix to preserve the last-accessed
// date when scanning w/ NAVW on NT.
// 
//    Rev 1.11   14 Aug 1997 17:45:10   MDUNN
// Added prototype for NavNameIsReservedName().
// 
//    Rev 1.10   13 Aug 1997 15:56:34   DBUCHES
// Changed function definition for RegistryStampLastScanTime() function.
//
//    Rev 1.9   12 Aug 1997 16:02:10   MKEATIN
// Fixed comment.
//
//    Rev 1.8   12 Aug 1997 16:00:52   MKEATIN
// Added prototype for GetNavexInfFile()
//
//    Rev 1.7   24 Jun 1997 15:58:10   TIVANOV
// fix the multiple aperance of the long custom msg
//
//    Rev 1.6   18 Jun 1997 15:20:14   MKEATIN
// Added the prototype for InitCallBacks().
//
//    Rev 1.5   21 May 1997 16:44:48   TIVANOV
// new function prototype
//
//    Rev 1.4   19 May 1997 15:33:36   TIVANOV
// function prototype for the quarantine log file
//
//    Rev 1.3   02 May 1997 15:15:56   TIVANOV
// new entry in the possible events enum list
//
//    Rev 1.2   01 May 1997 09:41:54   TIVANOV
// new process used in UpdateFileStatus
//
//    Rev 1.1   24 Apr 1997 18:26:02   JBRENNA
// Tesla port: TSmith r1.13:
// Added NavRpcSendPacket.
//
//    Rev 1.0   06 Feb 1997 20:51:24   RFULLER
// Initial revision
//
//    Rev 1.18   08 Nov 1996 17:30:26   JBRENNA
// Add NavNameShortenToWindow() and NavNameFormat() prototypes.
//
//    Rev 1.17   10 Oct 1996 14:41:44   JBRENNA
// Add NavAnsiToOemUpper() function prototype.
//
//    Rev 1.16   23 Sep 1996 15:21:36   GWESTER
// Reversed changes, since they weren't helping.
//
//    Rev 1.15   23 Sep 1996 15:01:06   GWESTER
// Fixed build problem.
//
//    Rev 1.14   23 Sep 1996 14:45:36   GWESTER
// Switched order of includes to fix a compiler error.
//
//    Rev 1.13   22 Aug 1996 13:33:34   JBRENNA
// Wrapper the Nav Ansi / Oem functions with a extern "C" so that they are
// available in both C and C++ files.
//
//    Rev 1.12   23 Jul 1996 13:39:06   DSACKIN
// Added NavRpcSendString and NavRpcSendStringW. Changed the prototype for
// NavRpcSendCommand to include Protocol, Address, and Endpoint.
//
//    Rev 1.11   22 Jul 1996 15:23:12   DSACKIN
// Removed TellNAVAPSVCToReload and replaced with NavRpcSendCommand.
//
//    Rev 1.10   19 Jul 1996 13:59:54   DSACKIN
// Added a prototype for SendNAVWCommand.
//
//    Rev 1.9   28 Jun 1996 11:42:20   DALLEE
// LuigiPlus port:
// Added custom version defines and prototypes.
//
//    Rev 1.8   24 Jun 1996 19:55:36   RHAYES
// Prototypes for new OEM<->Ansi functions from Luigi.
//
//    Rev 1.7   17 May 1996 14:36:22   DSACKIN
// Added a new control code for telling NAVAPSVC to reload its configuration.
//
//    Rev 1.6   06 May 1996 16:06:00   RSTANEV
// BackupInfectedFile() is prototyped and defined in NEWVIR.
//
//    Rev 1.5   06 May 1996 15:51:58   RSTANEV
// Merged Ron's Unicode changes.
//
//    Rev 1.4   02 May 1996 17:40:42   DSACKIN
// Removed two function prototypes because there are functions in Quake for them.
//
//    Rev 1.3   02 May 1996 16:58:28   DSACKIN
// Added functions for the wininfo.cpp file.
//
//    Rev 1.2   16 Feb 1996 20:28:12   YVANG
// Changed #include "nstdhelp.h" to "stdhelp.h".
//
//    Rev 1.1   13 Feb 1996 17:51:14   JWORDEN
// Add szFullOemPath to REPAIRFILESTRUCT
//
//    Rev 1.0   30 Jan 1996 15:47:06   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 10:11:30   BARRY
// Initial revision.
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

#include "ctsn.h"
#include "options.h"
#include "npt.h"
#include "virlist.h"
#include "xapi.h"                       // Needed for NAnsiToOem()
#include "navcb.h"

#include "nonav.h"

#ifndef INCL_NCFG
   #include "symcfg.h"
#endif

#ifndef _SYMNET_H
   #include "symnet.h"
#endif

#ifndef _SYMALOG_H
   #include "symalog.h"
#endif

#ifndef _DRIVE_ERROR_
    #include "drvprblm.h"
#endif

#ifdef SYM_WIN32
    #include "objbase.h"
    #include "iquaran.h"
#endif

#include "stdhelp.h"


                                        // ------------------------------
                                        // Character separating zip file
                                        // name from file within zip
                                        // file.
                                        // ------------------------------
#define ZIP_SEPARATOR           ('|')


#define	VERSION_STANDARD	  0			// Standard Norton version.
#define	VERSION_NETSCAPE	  1			// Netscape version.

// ---------------------------------------------------------------------------


// ********************************************** //
// Defines for the virus database file processing //
// ********************************************** //

#define N30_NEXT 0xFFFF
#define SIZE_COPYRIGHT  128
#define SIZE_INF_HEADER SIZE_COPYRIGHT + 8

#define SIZE_WORK    256

#define DLGREC_SELECTOROF      "NavDlgRec_Selector"
#define DLGREC_OFFSETOF        "NavDlgRec_Offset"
#define NAV_SELECTOROF         "NavProp_SELECTOROF"
#define NAV_OFFSETOF           "NavProp_OFFSETOF"
#define NAV_WORD               "NavWord"
                                        // ------------------------------------
                                        // Return values for the GetxxxxIni
                                        // functions
                                        // ------------------------------------

#define INI_OK                 0
#define INI_DEFAULTED_SUFFIX   1
#define INI_DEFAULTED          2
#define INI_FAILED             3

                                        // ------------------------------------
                                        // The PROCESS_xx are used in the
                                        // UpdateFileStatus routine to indicate
                                        // the process the completion status
                                        // belongs to.
                                        // ------------------------------------
enum
    {
    PROCESS_REPAIR = 1,
    PROCESS_DELETE,
    PROCESS_INOCULATE,
    PROCESS_EXCLUDE,
    PROCESS_QUARANTINE
    };

                                        // -----------------------------------
                                        // Actions to the TalkToVxd Function
                                        // -----------------------------------
typedef enum
    {
    VXDACTION_DISABLE = 0,              // Unprotect current process
    VXDACTION_ENABLE,                   // Protect current process
    VXDACTION_RELOAD_CONFIG,            // Reload NAVAP driver configuration
    NAVAPSVC_RELOAD_CONFIG,             // Reload AP Service configuration
    NAVAPSVC_ENABLE_DRIVER,             // Globally enable NAVAP driver
    NAVAPSVC_DISABLE_DRIVER             // Globally disable NAVAP driver
    } VXDACTION;

// **************************************************************** //
// ****            Possible events to log within NAV           **** //
// **************************************************************** //

enum {
     LOGEV_HALTCPU = 1,
     LOGEV_SCANEND,
     LOGEV_SCANINTERRUPTED,
     LOGEV_VIRLISTCHANGE,
     LOGEV_INOCULATED_NEW,
     LOGEV_INOCULATEFAILED_NEW,
     LOGEV_INOCULATED_CHANGED,
     LOGEV_INOCULATEFAILED_CHANGED,
     LOGEV_UNINOCULATED,
     LOGEV_UNINOCULATEFAILED,
     LOGEV_INTEG_CREATEFAILED,
     LOGEV_INTEG_REINOCULATED,
     LOGEV_INTEG_REINOCULATEFAILED,
     LOGEV_INTEG_MBREPAIRED,
     LOGEV_INTEG_MBREPAIRFAILED,
     LOGEV_INTEG_BOOTREPAIRED,
     LOGEV_INTEG_BOOTREPAIRFAILED,
     LOGEV_INTEG_SYSFILEREPAIRED,
     LOGEV_INTEG_SYSFILEREPAIRFAILED,
     LOGEV_INTEG_MBCHANGED,
     LOGEV_INTEG_BOOTCHANGED,
     LOGEV_INTEG_SYSFILECHANGED,
     LOGEV_INTEG_NOTEXIST,
     LOGEV_QUARANTINED,
     LOGEV_SYSERROR_ACCESSDENIED_DIR,
     LOGEV_SYSERROR_ACCESSDENIED_FILE,
     LOGEV_SCANSTART
	 ,LOGEV_SKIPDRIVE
     };

typedef struct tagREPAIRFILESTRUCT
{
    UINT                uVirusSigIndex;
    char                szFullPath [ SYM_MAX_PATH+1 ];
    char                szFullOemPath [ SYM_MAX_PATH+1 ];
    char                szBackExt  [ SYM_MAX_PATH+1 ];
    BOOL                bUseInoc;
    BOOL                bCreateBackup;
#ifdef SYM_WIN32
    IQuarantineItem *   pQuarBackupItem;
#endif
} REPAIRFILESTRUCT, FAR *LPREPAIRFILESTRUCT;

// *********************************************** //
// structure used to convert a dword into HH/MM/SS //
// *********************************************** //

typedef struct tagHMS
{
   UINT uHour;
   UINT uMinute;
   UINT uSecond;
} HMS, FAR * LPHMS;

// ********************************************************* //
// structure required to log events to the activity log file //
// ********************************************************* //

typedef struct tagLOGSTRUCT
{
   UINT        uEvent;        // Type of event
   LPSTR       lpFileName;    // the name of the file in question
   LPSTR       lpVirusName;   // and the virus name, if applicable
   LPACTOPT    lpAct;         // Activity log options
} LOGSTRUCT, FAR * LPLOGSTRUCT;

typedef struct tagLOGSTRUCTEX
{
   LPVOID      lpNoNav;       // NONAV structure of extended info  
   UINT        uEvent;        // Type of event
   LPSTR       lpFileName;    // the name of the file in question
   LPSTR       lpVirusName;   // and the virus name, if applicable
   LPACTOPT    lpAct;         // Activity log options
} LOGSTRUCTEX, FAR * LPLOGSTRUCTEX;

// ********************************************** //
// Structure Defines to hold INI data information //
// ********************************************** //

typedef struct tagBYTEINI
{
   LPSTR        lpSection;
   LPSTR        lpEntry;
   LPVOID       lpVar;
   BYTE         byDefault;
   WORD         wPlatform;
} BYTEINI, FAR *LPBYTEINI;


typedef struct tagWORDINI
{
   LPSTR        lpSection;
   LPSTR        lpEntry;
   LPVOID       lpVar;
   WORD         wDefault;
   WORD         wPlatform;
} WORDINI, FAR *LPWORDINI;

typedef struct tagDWORDINI
{
   LPSTR        lpSection;
   LPSTR        lpEntry;
   LPVOID       lpVar;
   DWORD        dwDefault;
   WORD         wPlatform;
} DWORDINI, FAR *LPDWORDINI;

typedef struct tagSTRINGINI
{
   LPCSTR       lpSection;
   LPCSTR       lpEntry;
   LPVOID       lpVar;
   UINT         uSize;
   LPCSTR       lpDefault;
   WORD         wPlatform;
} STRINGINI, FAR *LPSTRINGINI;

// ********************************************** //
// Notify structure                               //
// ********************************************** //

typedef struct tagNOTIFYSTRUCT
{
    UINT            uRemoveAfter;       // Number of seconds to keep msg in view
    UINT            uMsgID;             // Message ID to display
    UINT            uTitleID;           // Title ID to display
    LPSTR           lpZipFileName;      // Name of the .ZIP file, if any
    LPSTR           lpFileName;         // Filename to parse into message
    LPSTR           lpVirusName;        // Virus name to parse into message
    LPSTR           lpAlertMsg;         // Alert message to display at the bottom
    HWND            hParent;            // Parent Window handle
    HINSTANCE       hInstance;          // Instance handler of calling program

} NOTIFYSTRUCT, FAR * LPNOTIFYSTRUCT;

typedef struct tagYESNOSTRUCT
{
    HWND        hParent;
    UINT        uQuestionID;
    UINT        uYesButtonID;
    UINT        uNoButtonID;
    UINT        uTitleID;
    HINSTANCE   hInstance;
    BOOL        bYesDisabled;           // Setup so FALSE is default
    BOOL        bNoDisabled;

    LPDWORD     lpdwWhatsThisArray;
    UINT        uHelpBtnID;

} YESNOSTRUCT, FAR * LPYESNOSTRUCT;

void SYM_EXPORT WINAPI LongCustomMessage(HWND hDlg);
void SYM_EXPORT WINAPI SetShowMsg();
BOOL SYM_EXPORT WINAPI GetShowMsg();

// ***************************************************** //
// Defines and prototypes for N30.DAT data file routines //
// ***************************************************** //

#ifdef __cplusplus
extern "C" {
#endif

BOOL    SYM_EXPORT WINAPI AskYesNo       (LPYESNOSTRUCT lpYesNo);
VOID    SYM_EXPORT WINAPI NavutilEnd     (VOID);
VOID    SYM_EXPORT WINAPI NavutilInit    (VOID);

VOID    SYM_EXPORT WINAPI NotifyUser     (LPNOTIFYSTRUCT lpNotify);
VOID    SYM_EXPORT WINAPI NavExtensions  (HWND, LPSTR, LPSTR);
UINT    SYM_EXPORT WINAPI N30_Append     (UINT, N30 FAR *);
UINT    SYM_EXPORT WINAPI N30_FindVirus  (UINT, WORD, N30 FAR *);
UINT    SYM_EXPORT WINAPI N30_Open       (HINSTANCE);
UINT    SYM_EXPORT WINAPI N30_Read       (UINT, UINT, N30 FAR *);
UINT    SYM_EXPORT WINAPI N30_Write      (UINT, UINT, N30 FAR *);
BOOL    SYM_EXPORT WINAPI N30_Delete     (UINT, UINT);
UINT    SYM_EXPORT WINAPI N30_NumRecs    (UINT);
VOID    SYM_EXPORT WINAPI N30_Close      (UINT);
BOOL    SYM_EXPORT WINAPI N30_GetComment (LPSTR, UINT, LPSTR, LPSTR);
DWORD   SYM_EXPORT WINAPI N30_AddComment (HINSTANCE, LPCSTR, LPCSTR);
BOOL    SYM_EXPORT WINAPI N30_CreateBak  (HINSTANCE);
VOID    SYM_EXPORT WINAPI N30_GetPath    (HINSTANCE, LPSTR, UINT);

VOID    SYM_EXPORT WINAPI HMS_Time       (DWORD, HMS FAR *);

UINT SYM_EXPORT WINAPI UpdateFileStatus (
    UINT    uPrevFileStatus,
    UINT    uProcess,
    BOOL    bCompletionStatus
    );

// ******************************* //
// Miscellaneous windows functions //
// ******************************* //

VOID SYM_EXPORT WINAPI GetStartDir          (HINSTANCE, LPSTR, UINT);
VOID SYM_EXPORT WINAPI GetNavBin            (HINSTANCE, LPSTR, UINT);
VOID SYM_EXPORT WINAPI GetNavStart          (HINSTANCE, LPSTR, UINT);
VOID SYM_EXPORT WINAPI GetNavexInfFile      (HINSTANCE, LPSTR, UINT);
VOID SYM_EXPORT WINAPI GetSysIntegFile      (LPSTR lpPath, UINT uSize,
                                        LPINOCOPT lpInoc);
LPSTR SYM_EXPORT WINAPI GetNavBinFilename   (LPSTR lpFilename);
LPSTR SYM_EXPORT WINAPI GetNavStartFilename (LPSTR lpFilename);
LPSTR SYM_EXPORT WINAPI GetSysIntegFilename (LPSTR lpFilename);

WORD SYM_EXPORT WINAPI NavSelfTest (
    HINSTANCE
    );

VOID SYM_EXPORT WINAPI DisableDialog(       // Disables child controls
    HWND    hDlg,
    BOOL    bDisable,
    LPUINT  lpSaveChildren
    );

VOID SYM_EXPORT WINAPI TalkToVxD (      // Communicate with VxD
    VXDACTION   wVxdAction              // [in] Action to tell VxD to do
    );

DWORD SYM_EXPORT WINAPI NavRpcSendPacket(
    LPSTR    lpszProtocol,              // [in] - Protocol to use
    LPSTR    lpszAddress,               // [in] - Address to contact
    LPSTR    lpszEndpoint,              // [in] - Endpoint to look for.
    LPVOID   lpPacket                   // [in] - Data block to send to endpoint
    );

DWORD SYM_EXPORT WINAPI NavRpcSendCommand (
   LPSTR    lpszProtocol,               // [in] - Protocol to use
   LPSTR    lpszAddress,                // [in] - Address to contact
   LPSTR    lpszEndpoint,               // [in] - Endpoint to look for.
   DWORD    dwCmd);                     // [in] - Code to send to the endpoint

DWORD SYM_EXPORT WINAPI NavRpcSendString (
   LPSTR    lpszProtocol,               // [in] - Protocol to use
   LPSTR    lpszAddress,                // [in] - Address to contact
   LPSTR    lpszEndpoint,               // [in] - Endpoint to look for.
   LPSTR    lpszString);                // [in] - String to send to the endpoint

DWORD SYM_EXPORT WINAPI NavRpcSendStringW (
   LPSTR    lpszProtocol,               // [in] - Protocol to use
   LPSTR    lpszAddress,                // [in] - Address to contact
   LPSTR    lpszEndpoint,               // [in] - Endpoint to look for.
   LPWSTR   lpszStringW);               // [in] - UNICODE String to send

BOOL SYM_EXPORT WINAPI SetCustomVersion ( VOID );
										// (VERSION_STANDARD, VERSION_NETSCAPE, etc.)
UINT SYM_EXPORT WINAPI GetCustomVersion ( VOID );

BOOL SYM_EXPORT WINAPI SchedulerIsInstalled ( VOID );

VOID SYM_EXPORT WINAPI InitCallBacks (
   LPAVGLOBALCALLBACKS lpstNAVCallBacks,
   DWORD dwFlags);

// ************************** //
// Miscellaneous DOS fuctions //
// ************************** //

#ifndef SYM_WIN

LPSTR PASCAL CutString (
    LPSTR       szDest,
    LPSTR       szSource,
    WORD        wLength
    );

#define NetGetCurrentUserName(a,b,c,d)  { DOSNetGetCurrentUserName(b,c,d); }

WORD PASCAL DOSNetGetCurrentUserName (LPSTR  lpszServer,
                                      LPSTR  lpszUser,
                                      WORD   wBufferSize);

typedef struct tagNAVXUTIL_DOS_STRING   // Fake-o DOS string table struct.
    {
    UINT       uID;
    LPSTR      lpszString;
    } DOS_STRINGS;

int PASCAL LoadString (                 // Emulates Windows' LoadString()
    HINSTANCE   hInstance,              // [IN] Not used
    UINT        uID,                    // [IN] Id of string to use
    LPSTR       lpszBuffer,             // [OUT] Buffer to receive string
    int         nBufferMax);            // [IN] Max size of buffer

#endif  // !SYM_WIN

#if !defined(SYM_NTK)
// backup.cpp //
// or for SYM_NTK is in AVAPI //
BOOL SYM_EXPORT WINAPI BackupInfectedFile (LPSTR, LPSTR, LPSTR);
#endif

// pointer.c //
LPVOID SYM_EXPORT WINAPI NAV_GETPTR   (HWND);
LPVOID SYM_EXPORT WINAPI NAV_ALLOCPTR (HWND, DWORD);
VOID   SYM_EXPORT WINAPI NAV_FREEPTR  (HWND);

LPVOID SYM_EXPORT WINAPI LP_GET      (HWND);
VOID   SYM_EXPORT WINAPI LP_SET      (HWND, LPVOID);
VOID   SYM_EXPORT WINAPI LP_FREE     (HWND);
VOID   SYM_EXPORT WINAPI WORD_FREE   (HWND hwnd);
VOID   SYM_EXPORT WINAPI WORD_SET    (HWND hwnd, WORD wValue);
WORD   SYM_EXPORT WINAPI WORD_GET    (HWND hwnd);

// inistuff.c //

WORD SYM_EXPORT WINAPI GetByteIni   (LPCFGFILE, LPBYTEINI, LPSECTIONSUFFIX);
VOID SYM_EXPORT WINAPI SetByteIni   (LPCFGFILE, LPBYTEINI, LPSECTIONSUFFIX);
WORD SYM_EXPORT WINAPI GetWordIni   (LPCFGFILE, LPWORDINI, LPSECTIONSUFFIX);
VOID SYM_EXPORT WINAPI SetWordIni   (LPCFGFILE, LPWORDINI, LPSECTIONSUFFIX);
WORD SYM_EXPORT WINAPI GetDWordIni  (LPCFGFILE, LPDWORDINI, LPSECTIONSUFFIX);
VOID SYM_EXPORT WINAPI SetDWordIni  (LPCFGFILE, LPDWORDINI, LPSECTIONSUFFIX);
WORD SYM_EXPORT WINAPI GetStringIni (LPCFGFILE, LPSTRINGINI, LPSECTIONSUFFIX);
VOID SYM_EXPORT WINAPI SetStringIni (LPCFGFILE, LPSTRINGINI, LPSECTIONSUFFIX);

#define DEFAULT_NAV_BIN_HEADER \
    { \
    "NAVOPTS.DAT\x1A",                  /* Signature */ \
    0,                                  /* Version # */ \
    0                                   /* Filler */ \
    }
BOOL WINAPI ReadNavBinFromOpenedFile (
    HFILE        hFile,
    LPNAVOPTIONS lpNavOptions
    );

STATUS SYM_EXPORT WINAPI ReadNavBin (LPNAVOPTIONS lpNavOptions);
STATUS SYM_EXPORT WINAPI ReadNavBinFromDir (LPSTR lpszDir,
                                            LPNAVOPTIONS lpNavOptions);
STATUS SYM_EXPORT WINAPI ReadNavStart (LPNAVOPTIONS lpNavOptions);
STATUS SYM_EXPORT WINAPI ReadNavStartFromDir (LPSTR lpszDir,
                                              LPNAVOPTIONS lpNavOptions);
BOOL   SYM_EXPORT WINAPI ReadNavStartFromOpenFile (HFILE hFile,
                                                   LPNAVOPTIONS lpNavOptions);
STATUS SYM_EXPORT WINAPI WriteNavBin (LPNAVOPTIONS lpNavOptions);
STATUS SYM_EXPORT WINAPI WriteNavBinToDir (LPSTR lpszDir,
                                           LPNAVOPTIONS lpNavOptions);
STATUS SYM_EXPORT WINAPI WriteNavStart (LPNAVOPTIONS lpNavOptions);
STATUS SYM_EXPORT WINAPI WriteNavStartToDir (LPSTR lpszDir,
                                             LPNAVOPTIONS lpNavOptions);

UINT SYM_EXPORT WINAPI LogEvent (LPLOGSTRUCT lpLog);
UINT SYM_EXPORT WINAPI LogEventEx (LPLOGSTRUCTEX lpLog);

WORD SYM_EXPORT WINAPI InitIni (            // Initailizes INI file
    LPCSTR      lpszFileName,           // [in] INI file to use
    LPCFGFILE   lpCfgFIle);             // [out] Buffer to receive cfg spec

WORD SYM_EXPORT WINAPI InitConfig (         // Initailizes configuration file
    LPCSTR      lpszFileName,           // [in] Configuration file to use
    LPCFGFILE   lpCfgFile);             // [in/out] Buffer to update

WORD SYM_EXPORT WINAPI TerminateConfig (    // Terminates config session
    LPCFGFILE   lpCfgFile);             // [in] Configuration file info

// creaback.c //
BOOL SYM_EXPORT WINAPI CreateBackup (
    LPREPAIRFILESTRUCT lpRepair);       // Pointer to file repair structure

VOID SYM_EXPORT WINAPI RestoreFile (    // Restores a backuped file
    LPREPAIRFILESTRUCT lpRepair);       // Pointer to file repair structure

int SYM_EXPORT WINAPI NavLoadString (       // Calls LoadString and parses '|'
    HINSTANCE   hInst,                  // Instance to load resource from
    UINT        idResource,             // ID of resource to load
    LPSTR       lpszBuffer,             // Buffer to copy loaded/parsed string
    int         cbBuffer);              // Size of buffer

VOID SYM_EXPORT WINAPI NavComboBoxLoad (    // Initializes combo boxes
    HWND        hWnd,                   // [in] Handle combo box
    HINSTANCE   hInstRes,               // [in] Resource Instance handle
    WORD        wStringID,              // [in] Identifies combobox
    WORD        wIndexSelect);          // [in] Default selection

// Declare the following functions to be C functions so that C++ does
// not name mangle them. This makes the functions available for C files.
#ifdef __cplusplus
extern "C" {
#endif

#ifdef SYM_WIN

LPSTR SYM_EXPORT WINAPI NavStrUpr (
    LPSTR       lpszString              // [in] String to be converted (ANSI)
);

LPTSTR SYM_EXPORT WINAPI NavOemStrUpr (
    LPSTR       lpszString             // [in] String to be converted (OEM)
);

BOOL SYM_EXPORT WINAPI NavTestOemToAnsi (
    LPSTR       lpszString              // [in] String to be tested (OEM)
);

BOOL SYM_EXPORT WINAPI NavTestAnsiToOem (
    LPSTR       lpszString              // [in] ANSI string to be tested
);

LPSTR SYM_EXPORT WINAPI NavOemToAnsi (
    LPSTR lpszOemString,                // [in]  String to be converted (OEM)
    LPSTR lpszAnsiString                // [out] Converted string (ANSI)
);

LPSTR SYM_EXPORT WINAPI NavAnsiToOem (
    LPSTR lpszAnsiString,               // [in]  String to be converted (ANSI)
    LPSTR lpszOemString                 // [out] Converted string (OEM)
);

#else

#define NavStrUpr(x)           STRUPR(x)
#define NavOemStrUpr(x)        STRUPR(x)

BOOL SYM_EXPORT WINAPI NavTestOemToAnsi (
    LPSTR       lpszString              // [in] String to be tested (OEM)
);

BOOL SYM_EXPORT WINAPI NavTestAnsiToOem (
    LPSTR       lpszString              // [in] ANSI string to be tested
);

LPSTR SYM_EXPORT WINAPI NavOemToAnsi (
    LPSTR lpszOemString,                // [in]  String to be converted (OEM)
    LPSTR lpszAnsiString                // [out] Converted string (ANSI)
);

LPSTR SYM_EXPORT WINAPI NavAnsiToOem (
    LPSTR lpszAnsiString,               // [in]  String to be converted (ANSI)
    LPSTR lpszOemString                 // [out] Converted string (OEM)
);

#endif


#if defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)

BOOL SYM_EXPORT GetLastAccessDateNT ( LPCTSTR szFileName,
                                      FILETIME *lpFileTime );
#endif


#ifdef SYM_WIN
//************************************************************************
// NavAnsiToOemUpper()
//
// This routine translates the specified Ansi string to OEM. Unlike
// NavAnsiToOem(), characters that do not translate to OEM are not preserved
// in the string. Also, like NAnsiToOem(), this routine upper cases the
// characters. However, DBCS characters are not upper cased.
//
// This function is meant as a replacement for NAnsiToOem() calls. The
// difference is that NAnsiToOem() upper cases all characters. This does
// not upper case DBCS characters.
//
// Remember that lpszAnsiStr may be the same pointer as lpszOemStr.
//
// Parameters:
//      lpszAnsiStr    [in]  Ansi string to convert to OEM.
//      lpszOemStr     [out] The OEM version of lpszAnsiStr.
//************************************************************************
void SYM_EXPORT WINAPI NavAnsiToOemUpper (LPCSTR lpszAnsiStr,
                                          LPSTR  lpszOemStr);
#else
                                    // If DOS, simply use NAnsi...
#define NavAnsiToOemUpper     NAnsiToOem
#endif // SYM_WIN

#ifdef __cplusplus
}
#endif


VOID SYM_EXPORT WINAPI BuildLogFilename (
    LPSTR       lpLogFilename,
    LPCSTR      lpPartialFilename
    );

UINT SYM_EXPORT WINAPI WriteAllFileLogRecords (
    LPACTOPT    lpAct,
    LPVOID      lpVoid,
    LOGCALLBACK lpfnCallback,
    DWORD       dwUserData
    );

VOID SYM_EXPORT WINAPI ParseZipFileName (LPSTR lpFullPath, LPSTR lpZipPath,
                                     LPSTR lpFileName);

VOID SYM_EXPORT WINAPI BuildWhatScanned (
    LPSTR   lpScanList,
    LPSTR   lpScanned,
    UINT    uSize
    );

BOOL SYM_EXPORT WINAPI RepairFile (
    LPREPAIRFILESTRUCT lpRepair
    );

BOOL SYM_EXPORT WINAPI IsValidExtension (
    LPSTR   lpExt
    );

BOOL SYM_EXPORT WINAPI FileIsDOSExecutable (
    LPSTR   lpszFile
    );

VOID SYM_EXPORT WINAPI UpdateTsrDlg(    // Update TSR UI
    HWND    hDlg                        // Mommy Window
    );

UINT WINAPI UpdateTSR (
    LPSTR   lpszDir,
    LPNAVOPTIONS lpNavOptions
    );


// This is defined in drvprblm.h included in this file.
// UINT SYM_EXPORT WINAPI DriveAccessProblem (
//    LPDRIVEPROBLEMSTRUCT lpDP
//    );

DWORD SYM_EXPORT WINAPI ComputeKilobytesScanned (
    DWORD   dwFileSize,                 // size of the file
    DWORD   dwClusterSize               // size of one cluster
    );

BOOL SYM_EXPORT WINAPI IsUNCPath (      // Checks path to be valid UNC path
    LPSTR   lpPath                      // [in] path to check
    );

BOOL SYM_EXPORT NavNameIsReservedName ( LPCTSTR szPath );

BOOL SYM_EXPORT WINAPI IsValidFilename ( // Validates a filename
    LPSTR   lpItem,                     // [in] item to validate
    BOOL    bItemIsLFN                  // [in] TRUE if item an LFN
    );

#define IsValidLFN(lp) (IsValidFilename(lp,TRUE))
#define IsValidSFN(lp) (IsValidFilename(lp,FALSE))

UINT SYM_EXPORT WINAPI NavReturnIllegalChars ( // Get illegal characters
    WORD        wOsFlag,                // [in] use any SYM_ILLEGAL_xxx
    LPCTSTR     lpVolume,               // [in] pointer to volume, for LFN
    LPTSTR      lpIllegals,             // [out] illegal chars placed here
    UINT        uSizeIllegals           // [in] max size of lpIllegals
    );

#ifdef SYM_WIN32

VOID SYM_EXPORT WINAPI NavDlgOnContextHelp (
    HWND    hDlg,
    UINT    uMsg,
    WPARAM  wParam,
    LPARAM  lParam,
    DWORD  *pdwarrWhatsThisMap,
    DWORD  *pdwarrHowToMap,
    DWORD  *pdwarrLBItemsHelpStrings,
    LB_ITEMSELECTIONPROC lpfnGetSelItem
    );

VOID SYM_EXPORT NavPrnDlgSetContextHelpIds (
    LPPRINTDLGSTRUCT    pPrintDlgStruct
    );

VOID SYM_EXPORT WINAPI IfSpecialFileGetTrueFile (// If provided file is LNK then get
    LPSTR   lpFile                      // [in][out] true filename.
    );

BOOL SYM_EXPORT WINAPI ResolveLnkFile ( // Finds what LNK file pointing to
    LPCSTR  pszShortcutFile,            // [in] pointer to LNK file
    LPSTR   pszPath                     // [out] pointer to target file.
    );

BOOL SYM_EXPORT WINAPI ResolvePifFile ( // Finds what PIF file pointing to
    LPCSTR  pszPifFile,                 // [in] pointer to PIF file
    LPSTR   pszPath                     // [out] pointer to target file.
    );

BOOL SYM_EXPORT WINAPI RegistryAddVxD (
    VOID
    );

BOOL SYM_EXPORT WINAPI RegistryRemoveVxD (
    VOID
    );

BOOL SYM_EXPORT WINAPI RegistryIsVxDStartup (
    VOID
    );

VOID SYM_EXPORT WINAPI NavOptsLoadVxDChanged (
    HWND        hWnd,                   // [in] Handle of parent window
    BOOL        bNewVxDLoad);           // [in] New state of VxD load with WIN
 
BOOL SYM_EXPORT WINAPI LoadAutoProtect (
    VOID
    );
        
UINT SYM_EXPORT WINAPI ReadConfigValue
(
    LPCFGFILE           lpFile,
    LPCFGTEXTREC        lpConfig,
    UINT FAR            *lpwValue
);

UINT SYM_EXPORT WINAPI ReadConfigString
(
    LPCFGFILE           lpFile,
    LPCFGTEXTREC        lpConfig,
    LPSTR               lpszValue,
    WORD                wSize
);

#else

VOID SYM_EXPORT WINAPI MakePathShorter (
    LPSTR       lpPath,
    UINT        uLevels
    );

#endif

#if defined(SYM_VXD) || defined(SYM_WIN32)

BOOL SYM_EXPORT WINAPI RegistryAddVirusDefDate ( // Add date of vir def file
    HVCONTEXT hVirContext);                      // to the registry.

VOID SYM_EXPORT WINAPI NavHaltSystem ( VOID );

#endif

#ifdef SYM_WIN32

BOOL SYM_EXPORT WINAPI RegistryStampLastScanInfo (
    LPVOID lpvNav,
    LPSTR szDrives,
    LPSYSTEMTIME TimeStamp);

BOOL SYM_EXPORT WINAPI RegistryCheckLastScanUpdateKey (void);

#endif                                  // NAVSCANW


/////////////////////////////////////////////////////////////////////
// WinInfo.cpp Prototypes

#if defined(SYM_WIN32)

                                        // Version information
BOOL SYM_EXPORT WINAPI WinInfoIsNewShell ( VOID );


#endif // defined(SYM_WIN32)

#ifdef SYM_WIN
//****************************************************************************
// NavNameShortenToWindow()
//
// Description:
//   Use the NameShortenFileName() to shorten the filename. The difference is
//   that this function calculates the maximum possible number of characters
//   given the actually filename and the current font. Unforunately, this
//   algorithm is expensive. Without rewriting NameShortenFileName(), there is
//   no easy way to accurately add the "..." given the filename.
//
// Parameters:
//   hWnd           [in]   Control that determines the font and width info.
//   lpszName       [in]   Original long version of the filename.
//   lpszShortName  [out]  Shorten version of the filename.
//   nLines         [in]   Number of lines that the control is allowing for
//                         the filename.
//   nUsedPixels    [in]   Number of pixels for the line that are already
//                         consumed by prefix text. Use AlignWndGetTextSize
//                         to calculate text size. Usually this is 0.
//****************************************************************************
void SYM_EXPORT NavNameShortenToWindow (HWND hWnd, LPCSTR lpszName,
                                        LPSTR lpszShortName, int nLines,
                                        int nUsedPixels);
#endif // #ifdef SYM_WIN

#ifdef SYM_WIN
//****************************************************************************
// NavNameFormat()
//
// Description:
//   Walk through lpFormatStr and add CRLF when the control is going to wrap
//   the text. This routine does not CRLF in the middle of words contained in
//   lpFormatStr. When a %s appears in lpFormatStr, this routine replaces the
//   %s with lpFileName. Note that when putting lpFileName into lpOutputStr,
//   CRLFs are added in the middle of lpFileName. This causes the file name
//   to wrap in the middle of the string.
//
//   Note, this function only add CRLFs to the text that occurs before the %s
//   string. All text after the %s is copied unchanged.
//
//   This function is necessary because windows clips strings that are longer
//   than one line in the control.
//
// Parameters:
//   hWnd           [in]   Control that determines the font and width info.
//   lpszFormat     [in]   Format string with the "%s" for lpszFileName.
//   lpszFileName   [in]   File name to replace the first "%s" in lpszFormat.
//   lpszOutput     [out]  Completely formated string, with CRLFs added.
//****************************************************************************
void SYM_EXPORT NavNameFormat (HWND   hWnd,         LPCSTR lpszFormat,
                               LPCSTR lpszFileName, LPSTR  lpszOutput);
#endif // #ifdef SYM_WIN

#ifdef __cplusplus
}
#endif

#endif // _NAVWUTIL_H
