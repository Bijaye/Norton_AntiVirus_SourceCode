// Copyright 1992-1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/navopts.h_v   1.23   30 Apr 1998 01:52:14   tcashin  $
//
// Description:
//      This include file defines all the option structures used for
//      NAV.
// Copyright 1993-1994 Symantec, Peter Norton Product Group
//************************************************************************
// $Log:   S:/INCLUDE/VCS/navopts.h_v  $
// 
//    Rev 1.23   30 Apr 1998 01:52:14   tcashin
// Cleaned up the #defines for scanopts uaction member.
// 
//    Rev 1.22   19 Apr 1998 14:35:48   tcashin
// Add bDispQuarantine to TSR options.
// 
//    Rev 1.21   13 Apr 1998 18:20:00   MKEATIN
// Fixed comments.
// 
//    Rev 1.20   13 Apr 1998 18:18:08   MKEATIN
// Added heuristic options to the tsr struct.
// 
//    Rev 1.19   12 Apr 1998 17:35:56   tcashin
// Added bQuarantine to the Alert options.
// 
//    Rev 1.18   12 Apr 1998 16:49:08   tcashin
// Added bDispQuarantine to Scanner options.
// 
//    Rev 1.17   01 Apr 1998 16:36:44   DALLEE
// Added TSR_QUAR_CANT_REP define.
// 
//    Rev 1.16   30 Mar 1998 15:02:34   tcashin
// Added more #defines for Quarantine.
// 
//    Rev 1.15   22 Feb 1998 20:05:06   RStanev
// New sets of actions for NAVAP.
//
//    Rev 1.14   19 Feb 1998 17:53:12   RStanev
// Added TSR_QUARANTINEQUIET.
//
//    Rev 1.13   15 Jun 1997 19:43:30   DBUCHES
// Added members to SCANOPT struct to support heuristic scanner.
//
//    Rev 1.12   10 Jun 1997 14:54:46   DBUCHES
// Added startup password protection.
//
//    Rev 1.11   10 Jun 1997 11:02:48   DBUCHES
// Oops.. wrong pragma.
//
//    Rev 1.10   10 Jun 1997 10:59:26   DBUCHES
// Added 1 byte structure packing to this file.
//
//    Rev 1.9   20 May 1997 21:58:54   DBUCHES
// Added custom response actions for SCANOPTS and TSROPTS
//
//    Rev 1.8   13 May 1997 16:46:10   TIVANOV
// new member to ACTOPT - to support filtering on quarantine events
//
//    Rev 1.7   13 May 1997 11:15:26   TIVANOV
// new quarantine defines for INOC_ and INOC_CHANGE_
//
//    Rev 1.6   02 May 1997 12:06:44   TIVANOV
// new define SCAN_MOVE
//
//    Rev 1.5   07 Mar 1997 14:21:22   RSTANEV
// Added TSR_MOVEQUIET and TSR_RENAMEQUIET.
//
//    Rev 1.4   05 Aug 1996 16:28:28   JBRENNA
// LuigiPlus Port: DALLEE: Sorry! Added an internal #define for a web-browser
// response for known virus found. New SCANOPTS.uAction is SCAN_DOWNLOADPROMPT.
//
//    Rev 1.3   11 Apr 1996 12:08:20   JMILLAR
// change back to char - we are keeping the file in DBCS for now, and
// the NT drivers build with the Unicode flag on, which means the strings
// actually build as Unicode, which isn't what we want. We will reevaluate
// this for Firebird.
//
//    Rev 1.2   11 Apr 1996 13:25:48   jworden
// Convert char to TCHAR for DBCS
//
//    Rev 1.1   08 Apr 1996 19:00:32   RSTANEV
// Added TSR_...QUIET defines.
//
//    Rev 1.0   26 Jan 1996 20:13:48   RJACKSO
// Initial revision.
//
//    Rev 1.16   16 Oct 1995 18:07:14   JWORDEN
// Add Luigi/Net install options
//
//    Rev 1.15   21 Sep 1995 12:38:38   YVANG
// Added a password protect feature option for 'View Startup Settings',
// PASSWORD_VIEWSTARTUPOPT, which is equated to MAX_PROTECT_FEATURE+1.
// This protect feature option is stored in the 'bPrectectFeature' entry
// of the tagSTARTOPT structure, instead of the tagPASWOPT structure.
//
//    Rev 1.14   06 Dec 1994 14:41:58   MFALLEN
// Reverted back to Quake 2 equivalent in order to maintain backward compatibility.
//
//************************************************************************

#ifndef _INC_NAVOPTS
#define _INC_NAVOPTS

#ifdef SYM_WIN
#pragma pack(push, 1)
#endif
// *************************************************** //
// Constants
// *************************************************** //

					//---------------------------------
					// These constants are required to
					// maintain backward compatibility.
					//---------------------------------
#define MAX_USER_INFO       40
#define MAX_NET_PATH_SIZE   256
#define MAX_PATH_SIZE       144

					//---------------------------------
					//---------------------------------

#define SIZE_EXTENSION_LIST     100
#define SIZE_EXTENSION          3
#define SIZE_ALERT_MSG          76

// *************************************************** //
// *************************************************** //

					// User Registration Info
typedef struct tagUSEROPT
    {
    char        szName[MAX_USER_INFO+1];
    char        szCompany[MAX_USER_INFO+1];
    } USEROPT, FAR *LPUSEROPT;


					//---------------------------------
					// Install System Monitor radio
					// button states.
					//---------------------------------
#define SYS_MON_DONT_INSTALL    0
#define SYS_MON_IN_CONFIGSYS    1
#define SYS_MON_IN_AUTOEXECBAT  2

					//---------------------------------
					// Autoexec modification radio
					// button states.
					//---------------------------------
#define AUTOEXEC_AUTO_MODIFY    0
#define AUTOEXEC_SAVE_CHANGES   1
#define AUTOEXEC_DO_NOT_CHANGE  2

					//---------------------------------
					// Rescue disk radio button states.
					//---------------------------------
#define RESCUE_DO_NOT_CREATE    0
#define RESCUE_CREATE           1

					// Action after installing
#define DONT_REBOOT             0
#define REBOOT                  1
#define PROMPT_TO_REBOOT        2

					// Install/Update CPL
typedef struct tagADMINOPT
    {
    char        szNetworkDir[MAX_NET_PATH_SIZE+1];
    char        szLocalDir[MAX_PATH_SIZE+1];
    BYTE        bPromptForLocalDir;
    BYTE        bUsePreviousLocalDir;
    BYTE        uSystemMonitor;         // How should System Monitor be installed
    BYTE        bRunNAV;
    BYTE        bAddToPath;
    BYTE        bRetainSettings;
    BYTE        bLogoffIfNoMonitor;
    BYTE        bConfirmAllPrompts;
    BYTE        bInstallWindowsNAV;
    BYTE        bLoadWindowsDrivers;
    BYTE        bCreateWindowsGroup;
    BYTE        bPromptForCompany;
    char        szCompany[MAX_USER_INFO+1];
    BYTE        bPromptForName;
    BYTE        bScanMemory;
    BYTE        bScanHardDisk;
    BYTE        bScanProgramFiles;
    BYTE        bScanAlways;
    BYTE        uActionAfterInstalling;
    BYTE        bUpgradeOnly;
    char        szBeforeCommand[MAX_PATH_SIZE+1];
    BYTE        bBeforeCommandAlways;
    char        szAfterCommand[MAX_PATH_SIZE+1];
    BYTE        bAfterCommandAlways;
    char        szLogoffCommand[MAX_PATH_SIZE+1];
    char        szScannerCmdLine[MAX_PATH_SIZE+1];
    char        szSystemMonitorCmdLine[MAX_PATH_SIZE+1];
    char        szDevice[MAX_PATH_SIZE+1];
    BYTE        bAutoexecAction;
    char        szSaveAutoexecTo[MAX_PATH_SIZE+1];
    char        szSymantecFiles[MAX_PATH_SIZE+1];
    BYTE        bInstallAutoProtect;
    BYTE        bInstallStartup;
    BYTE        bInstallTools;
    BYTE        bCreateRescue;
    } ADMINOPT, FAR *LPADMINOPT;

					// Actions CPL
typedef struct tagACTOPT
{
   BYTE         bKnown;
   BYTE         bUnknown;
   BYTE         bInocChange;
   BYTE         bVirusLike;
   BYTE         bStartEnd;
   BYTE         bVirusList;
   BYTE         bKeepLast;
   BYTE         bQuarantine;
   WORD         uEntries;
   BYTE         bKeepUpTo;
   WORD         uKiloBytes;
   char         szLogFile [MAX_PATH_SIZE+1];
#ifdef BANKERS
   BYTE         bRegChange;
   BYTE         bConfigChange;
#endif  // BANKERS
} ACTOPT, FAR *LPACTOPT;


					// Alerts CPL
typedef struct tagALERTOPT
{
   BYTE         bDispAlertMsg;
   char         szAlertMsg [SIZE_ALERT_MSG+1];
   BYTE         bRemoveAfter;
   BYTE         uSeconds;
   BYTE         bBeep;
   BYTE         bAlertNetUsr;
   BYTE         bAlertNetCon;
   BYTE         bAlertNLM;
   BYTE         bKnown;
   BYTE         bUnknown;
   BYTE         bInoculation;
   BYTE         bVirusLike;
   BYTE         bStartEnd;
   BYTE         bVirListChange;
   BYTE         bQuarantine;
#ifdef BANKERS
   BYTE         bRegChange;
#endif  // BANKERS
} ALERTOPT, FAR *LPALERTOPT;

					// General CPL
typedef struct tagGENOPT
{
   BYTE         bBackupRep;
   char         szBackExt [SIZE_EXTENSION+1];
   BYTE         bScanZipFiles;
   BYTE         bScanHighMemory;
   char         szExt [SIZE_EXTENSION_LIST+1];
   char         szDef [SIZE_EXTENSION_LIST+1];
} GENOPT, FAR *LPGENOPT;

// ******************************** //
// Defines for uActionNewInoc field //
// ******************************** //

#define INOC_PROMPT     1
#define INOC_AUTO       2
#define INOC_REPORT     3
#define INOC_DENY       4
#define INOC_DENYQUIET  5
#define INOC_QUARANTINE 6
					// ---------------------------------
					// This is an internal options only
					// used when scanning from Tools->
					// Options. Used by NAV and NAVW
					// ---------------------------------
#define INOC_AUTO_FILEONLY  9

// ******************************** //
// Defines for uActionInocChg field //
// ******************************** //

#define INOC_CHANGE_PROMPT      1
#define INOC_CHANGE_REPORT      3
#define INOC_CHANGE_DENY        4
#define INOC_CHANGE_DENYQUIET   5
#define INOC_CHANGE_QUARANTINE  6

					// Inoculation CPL
typedef struct tagINOCOPT
{
   BYTE         bUseSystemInoc;
   BYTE         bUseInoc;
   BYTE         bInocFloppies;
   BYTE         bInocFast;
   BYTE         uActionNewInoc;
   BYTE         uActionInocChg;
   BYTE         bDispRepair;
   BYTE         bDispDelete;
   BYTE         bDispInoc;
   BYTE         bDispCont;
   BYTE         bDispStop;
   BYTE         bDispExcl;
   char         szNetInocPath [MAX_PATH_SIZE+1];
} INOCOPT, FAR *LPINOCOPT;

					// Different areas we protect
enum PASSWORD_PROTECT
    {
    PASSWORD_VIEWACTIVITY,
    PASSWORD_CLEARLOG,
    PASSWORD_VIEWVIRUSLIST,
    PASSWORD_MODIVIRUSLIST,
    PASSWORD_INOCFILES,
    PASSWORD_UNINOCFILES,
    PASSWORD_VIEWSCANOPT,
    PASSWORD_VIEWTSROPT,
    PASSWORD_VIEWALERTOPT,
    PASSWORD_VIEWEXCLOPT,
    PASSWORD_MODIEXCLUSIONS,
    PASSWORD_VIEWACTIVITYOPT,
    PASSWORD_VIEWINOCOPT,
    PASSWORD_VIEWGENOPT,
    PASSWORD_VIEWSTARTUPOPT,
#ifdef BANKERS
    PASSWORD_VIEWMONOPT,
    PASSWORD_REGFILES,
#endif
    MAX_PROTECT_FEATURE
    };


#define PASSWORD_ALWAYS         (0xFFFF)

					// Password CPL
typedef struct tagPASWOPT
{
   BYTE         bProtect;
   BYTE         bMaxProtection;
   DWORD        dwPassword;             // CRC of password
   BYTE         bProtectFeature[MAX_PROTECT_FEATURE];
} PASWOPT, FAR *LPPASWOPT;


// ************************************************* //
// The followinf define's are for the uAction member //
// ************************************************* //
#define SCAN_PROMPT             1
#define SCAN_REPORT             2
#define SCAN_REPAUTO            3
#define SCAN_DELAUTO            4
#define SCAN_CUSTOM_RESPONSE    5
#define SCAN_QUARAUTO           6
#define SCAN_QUAR_CANT_REP      7
#define SCAN_HALTCPU            8
#define SCAN_QUARANTINE         9
#define SCAN_MOVE               10

// ************************************************* //
// These are the defines for the heuristic scanner.
// uHeuristicLevel
// ************************************************* //
#define SCAN_HEURISTIC_LOW              0
#define SCAN_HEURISTIC_MEDIUM   1
#define SCAN_HEURISTIC_HIGH             2

					// The next uAction is an internal
					// option used to indicate web-browser
					// download handling. It is never
					// saved to disk.
#define SCAN_DOWNLOADPROMPT 9

					// Scanner CPL
typedef struct tagSCANOPT
{
   BYTE         bMemory;
   BYTE         bMasterBootRec;
   BYTE         bBootRecs;
   BYTE         bScanAll;
   BYTE         uAction;
   BYTE         bDispRepair;
   BYTE         bDispDelete;
   BYTE         bDispCont;
   BYTE         bDispExcl;
   BYTE         bDispQuarantine;

   BYTE         bAllowNetScan;
   BYTE         bAllowScanStop;
   BYTE         bPrestoNotify;
   BYTE         bPresFlop;
   BYTE         bPresHard;
   BYTE         bPresNet;

   BYTE                 bEnableHeuristicScan;
   BYTE                 uHeuristicLevel;

} SCANOPT, FAR *LPSCANOPT;


// ************************************************* //
// The followinf define's are for the uAction member //
// ************************************************* //

// New defines. NAVAP will support these properly
// as long as old defines and new defines are not
// mixed in the same configuration file!!!)

#define TSR_PROMPT                  1
#define TSR_DENYACCESS_NOTIFY       2
#define TSR_REPAIR_QUIET            3
#define TSR_DELETE_QUIET            4
#define TSR_SHUTDOWN_NOTIFY         5
#define TSR_DENYACCESS_QUIET        6
#define TSR_QUARANTINE_QUIET        7
#define TSR_RENAME_QUIET            8
#define TSR_CUSTOM_RESPONSE         9
#define TSR_MOVE_QUIET              10
#define TSR_REPAIR_NOTIFY           11
#define TSR_DELETE_NOTIFY           12
#define TSR_QUARANTINE_NOTIFY       13
#define TSR_RENAME_NOTIFY           14
#define TSR_MOVE_NOTIFY             15
#define TSR_QUAR_CANT_REP           16

// Old defines.  Don't add defines which conflict
// with the new defines above, or NAVAP will break!!!

// duplicate: #define TSR_PROMPT              1
#define TSR_DENY                TSR_DENYACCESS_NOTIFY
#define TSR_REPAIR              TSR_REPAIR_QUIET
#define TSR_DELETE              TSR_DELETE_QUIET
#define TSR_HALTCPU             TSR_SHUTDOWN_NOTIFY
#define TSR_DENYQUIET           TSR_DENYACCESS_QUIET
#define TSR_QUARANTINEQUIET     TSR_QUARANTINE_QUIET
#define TSR_RENAMEQUIET         TSR_RENAME_QUIET
// duplicate: #define TSR_CUSTOM_RESPONSE     9
#define TSR_MOVEQUIET           TSR_MOVE_QUIET

// ******************************* //
// Bypass key define's for the Tsr //
// ******************************* //

#define KEYS_NONE    0
#define KEYS_SHIFT   1
#define KEYS_ALT     2
#define KEYS_CTRL    3

// ************************************** //
// Tsr advanced actions define statements //
// ************************************** //


#define TSR_ADV_ALLOW   0
#define TSR_ADV_PROMPT  1
#define TSR_ADV_DONT    2
#define TSR_ADV_DONTQUIET 3

					// System Monitor CPL
typedef struct tagTSROPT
{
   BYTE         bScanUnknown;
   BYTE         bScanRun;
   BYTE         bScanOpen;
   BYTE         bScanCreate;
   BYTE         bScanAll;
   BYTE         uActionKnown;
   BYTE         uActionUnknown;
   BYTE         bDispRepair;
   BYTE         bDispDelete;
   BYTE         bDispContinue;
   BYTE         bDispStop;
   BYTE         bDispExclude;
   BYTE         bDispQuarantine;

   BYTE         bDispUnknownRepair;
   BYTE         bDispUnknownDelete;
   BYTE         bDispUnknownContinue;
   BYTE         bDispUnknownExclude;

   BYTE         bScanMem;
   BYTE         bScanMBoot;
   BYTE         bScanBootRecs;
   BYTE         bUseHMA;
   BYTE         bUseUMB;
   BYTE         bUseEMS;
   BYTE         bUseXMS;
   BYTE         uBypassKey;
   BYTE         bUnloadable;
   BYTE         bHideIcon;

   BYTE         uFormat;
   BYTE         uWriteHard;
   BYTE         uWriteFlop;
   BYTE         uWriteExe;
   BYTE         uROAttrib;
   BYTE         bChkFlopAccess;
   BYTE         bChkFlopOnBoot;
   BYTE         bChkBothFlops;

   BYTE         bEnableHeuristicScan;
   BYTE         uHeuristicLevel;

} TSROPT, FAR *LPTSROPT;


#ifdef SYM_WIN
#pragma pack(pop)
#endif

#endif


