#ifndef _NAV_30_OPTIONS_
#define _NAV_30_OPTIONS_

// *************************************************** //
// Constants
// *************************************************** //

#define N30_SIZE_EXTENSION_LIST     100
#define SIZE_EXTENSION          3
#define SIZE_ALERT_MSG          76

// ---------------------------------------
// DO NOT USE: THESE ARE OLD QUACK DEFINES
// ---------------------------------------

#define MAX_USER_INFO           40
#define MAX_NET_PATH_SIZE       256
#define MAX_PATH_SIZE           144

// *************************************************** //
// *************************************************** //

                                        // User Registration Info
typedef struct tagOLDUSEROPT
    {
    char        szName   [MAX_USER_INFO+1];
    char        szCompany[MAX_USER_INFO+1];
    } OLDUSEROPT, FAR *LPOLDUSEROPT;


                                        // Install/Update CPL
typedef struct tagOLDADMINOPT
    {
    char        szNetworkDir [ MAX_NET_PATH_SIZE+1 ];
    char        szLocalDir   [ MAX_PATH_SIZE+1     ];
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
    } OLDADMINOPT, FAR *LPOLDADMINOPT;

                                        // Actions CPL
typedef struct tagOLDACTOPT
{
   BYTE         bKnown;
   BYTE         bUnknown;
   BYTE         bInocChange;
   BYTE         bVirusLike;
   BYTE         bStartEnd;
   BYTE         bVirusList;
   BYTE         bKeepLast;
   WORD         uEntries;
   BYTE         bKeepUpTo;
   WORD         uKiloBytes;
   char         szLogFile [MAX_PATH_SIZE+1];
#ifdef BANKERS
   BYTE         bRegChange;
   BYTE         bConfigChange;
#endif  // BANKERS
} OLDACTOPT, FAR *LPOLDACTOPT;


                                        // Alerts CPL
typedef struct tagOLDALERTOPT
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
#ifdef BANKERS
   BYTE         bRegChange;
#endif  // BANKERS
} OLDALERTOPT, FAR *LPOLDALERTOPT;

                                        // General CPL
typedef struct tagOLDGENOPT
{
   BYTE         bBackupRep;
   char         szBackExt [SIZE_EXTENSION+1];
   BYTE         bScanZipFiles;
   BYTE         bScanHighMemory;
   char         szExt [N30_SIZE_EXTENSION_LIST+1];
   char         szDef [N30_SIZE_EXTENSION_LIST+1];
} OLDGENOPT, FAR *LPOLDGENOPT;

                                        // Inoculation CPL
typedef struct tagOLDINOCOPT
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
} OLDINOCOPT, FAR *LPOLDINOCOPT;

                                        // Password CPL
typedef struct tagOLDPASWOPT
{
   BYTE         bProtect;
   BYTE         bMaxProtection;
   DWORD        dwPassword;             // CRC of password
   BYTE         bProtectFeature[MAX_PROTECT_FEATURE];
} OLDPASWOPT, FAR *LPOLDPASWOPT;

                                        // Scanner CPL
typedef struct tagOLDSCANOPT
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

   BYTE         bAllowNetScan;
   BYTE         bAllowScanStop;
   BYTE         bPrestoNotify;
   BYTE         bPresFlop;
   BYTE         bPresHard;
   BYTE         bPresNet;

} OLDSCANOPT, FAR *LPOLDSCANOPT;

                                        // System Monitor CPL
typedef struct tagOLDTSROPT
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

} OLDTSROPT, FAR *LPOLDTSROPT;


typedef struct tagOLDNAVMON
{
    BYTE        bUseReg;
    BYTE        uActionNewReg;
    BYTE        uActionRegChg;
    BYTE        bDispReg;
    BYTE        bDispCont;
    BYTE        bDispStop;
    BYTE        bDispExcl;
} OLDNAVMON, FAR * LPOLDNAVMON;


typedef struct tagNAV30OPTIONS
{
    OLDUSEROPT     user;
    OLDTSROPT      tsr;
    OLDACTOPT      activity;
    OLDALERTOPT    alert;
    OLDINOCOPT     inoc;
    OLDSCANOPT     scanner;
    OLDPASWOPT     password;
    OLDGENOPT      general;
} NAV30OPTIONS, FAR *LPNAV30OPTIONS;


#endif

