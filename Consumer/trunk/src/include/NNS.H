////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************************
//                                  
// $Header:   S:/include/VCS/nns.h_v   1.0   06 Feb 1997 20:51:58   RFULLER  $
//                                  
// Description:                             
//      This include file defines all the values for the NNS files, so
//      we can map an NNS file to an internal settings variable, called
//      'navOptions'.  We can also map in the opposite direction, too.
//                                  
//************************************************************************
// $Log:   S:/include/VCS/nns.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:58   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:46:58   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:20   BARRY
// Initial revision.
// 
//    Rev 1.53   16 Oct 1995 18:20:58   JWORDEN
// Add platform indicator to all ini entries
// Add platform section entries
// 
//    Rev 1.52   10 Oct 1995 11:29:32   DALLEE
// Fix last change: forgot to delete original startup.bProtectFeature line.
// 
//    Rev 1.51   10 Oct 1995 11:19:18   DALLEE
// 1. Removed strange Ctrl-Z at end of file.
// 2. Moved BYTEINI startup.bProtectFeature inside #ifndef CREATE_NAVBIN block.
// 3. Changed #ifdef NAVNETW back to NAVNET. 
//    NAVNET is the standard define for Navnet for both Windows and DOS.
//    DOS modules in the TOOLS project need to include this file.
//************************************************************************

#ifndef _NNS_H
#define _NNS_H

#ifdef NAVNET

#include "options.h"
#include "navutil.h"
                                        // We will put the settings in this
                                        // global variable
extern NAVOPTIONS       navOptions;

                                        // Section Headings for the NNS file
char SZ_SCANNER[]       = "Scanner";
char SZ_TSR[]           = "AutoProtect";
char SZ_ALERTS[]        = "Alerts";
char SZ_ACTIVITY[]      = "ActivityLog";
char SZ_INOCULATION[]   = "Inoculation";
char SZ_PASSWORD[]      = "Password";
char SZ_GENERAL[]       = "General";
char SZ_REGISTRATION[]  = "Registration";
char SZ_INSTALL[]       = "Install/Update";
char SZ_EXCLUSIONS[]    = "Exclusions";
char SZ_LFNEXCLUSIONS[] = "LFNExclusions";
char SZ_NUM_EXCLUSIONS[]= "NumberOfExclusions";
char SZ_PLATFORM[]      = "Platform";

char SZ_EMPTY_STR[]     = "";

#ifdef BANKERS
  char SZ_MONITOR[]     = "Monitor";
#endif  // BANKERS

                                        //-------------------------------
                                        // Byte variables
                                        //-------------------------------
BYTEINI byteSettings[] =
{
   {SZ_ACTIVITY, "LogKnownVirus",      (LPVOID)&navOptions.activity.bKnown,     TRUE, PLATFORM_ALL},
   {SZ_ACTIVITY, "LogUnknownVirus",    (LPVOID)&navOptions.activity.bUnknown,   FALSE, PLATFORM_ALL},
   {SZ_ACTIVITY, "LogInocChange",      (LPVOID)&navOptions.activity.bInocChange,FALSE, PLATFORM_ALL},
   {SZ_ACTIVITY, "LogVirusActivity",   (LPVOID)&navOptions.activity.bVirusLike, FALSE, PLATFORM_ALL},
   {SZ_ACTIVITY, "LogStartEnd",        (LPVOID)&navOptions.activity.bStartEnd,  FALSE, PLATFORM_ALL},
   {SZ_ACTIVITY, "LogVirusListChanges",(LPVOID)&navOptions.activity.bVirusList, FALSE, PLATFORM_ALL},
#ifdef BANKERS
   {SZ_ACTIVITY, "LogRegChange",       (LPVOID)&navOptions.activity.bRegChange, TRUE, PLATFORM_ALL},
   {SZ_ACTIVITY, "LogConfigurationChange",(LPVOID)&navOptions.activity.bConfigChange, TRUE, PLATFORM_ALL},
#endif  // BANKERS
   {SZ_ACTIVITY, "LimitLogSize",       (LPVOID)&navOptions.activity.bKeepUpTo,  TRUE, PLATFORM_ALL},

   {SZ_ALERTS, "DisplayAlertMessage",    (LPVOID)&navOptions.alert.bDispAlertMsg, FALSE, PLATFORM_ALL},
   {SZ_ALERTS, "RemoveAlert",            (LPVOID)&navOptions.alert.bRemoveAfter,  FALSE, PLATFORM_ALL},
   {SZ_ALERTS, "RemoveAlertSeconds",     (LPVOID)&navOptions.alert.uSeconds,      0, PLATFORM_ALL},
   {SZ_ALERTS, "AudibleAlert",           (LPVOID)&navOptions.alert.bBeep,         TRUE, PLATFORM_ALL},
   {SZ_ALERTS, "AlertNetworkUsers",      (LPVOID)&navOptions.alert.bAlertNetUsr,  FALSE, PLATFORM_ALL},
   {SZ_ALERTS, "AlertNetworkConsole",    (LPVOID)&navOptions.alert.bAlertNetCon,  FALSE, PLATFORM_ALL},
   {SZ_ALERTS, "AlertNLM",               (LPVOID)&navOptions.alert.bAlertNLM,     TRUE, PLATFORM_ALL},
   {SZ_ALERTS, "AlertOthersKnown",       (LPVOID)&navOptions.alert.bKnown,        TRUE, PLATFORM_ALL},
   {SZ_ALERTS, "AlertOthersUnknown",     (LPVOID)&navOptions.alert.bUnknown,      FALSE, PLATFORM_ALL},
   {SZ_ALERTS, "AlertOthersInoculation", (LPVOID)&navOptions.alert.bInoculation,  FALSE, PLATFORM_ALL},
   {SZ_ALERTS, "AlertOthersVirusActivity",(LPVOID)&navOptions.alert.bVirusLike,   FALSE, PLATFORM_ALL},
   {SZ_ALERTS, "AlertOthersStartEnd",    (LPVOID)&navOptions.alert.bStartEnd,     FALSE, PLATFORM_ALL},
   {SZ_ALERTS, "AlertOthersVirusListChange",(LPVOID)&navOptions.alert.bVirListChange,FALSE, PLATFORM_ALL},
#ifdef BANKERS
   {SZ_ALERTS, "AlertOthersRegistration",(LPVOID)&navOptions.alert.bRegChange,    FALSE, PLATFORM_ALL},
#endif  // BANKERS

   {SZ_INOCULATION, "InocSystemFiles",(LPVOID)&navOptions.inoc.bUseSystemInoc,TRUE, PLATFORM_ALL},
   {SZ_INOCULATION, "InocFiles",      (LPVOID)&navOptions.inoc.bUseInoc,      FALSE, PLATFORM_ALL},
   {SZ_INOCULATION, "InocFloppies",   (LPVOID)&navOptions.inoc.bInocFloppies, FALSE, PLATFORM_ALL},
   {SZ_INOCULATION, "NewInocAction",  (LPVOID)&navOptions.inoc.uActionNewInoc,INOC_AUTO, PLATFORM_ALL},
   {SZ_INOCULATION, "ReinocAction",   (LPVOID)&navOptions.inoc.uActionInocChg,INOC_PROMPT, PLATFORM_ALL},
   {SZ_INOCULATION, "ButtonRepair",   (LPVOID)&navOptions.inoc.bDispRepair,   TRUE, PLATFORM_ALL},
   {SZ_INOCULATION, "ButtonDelete",   (LPVOID)&navOptions.inoc.bDispDelete,   FALSE, PLATFORM_ALL},
   {SZ_INOCULATION, "ButtonInoc",     (LPVOID)&navOptions.inoc.bDispInoc,     TRUE, PLATFORM_ALL},
   {SZ_INOCULATION, "ButtonContinue", (LPVOID)&navOptions.inoc.bDispCont,     TRUE, PLATFORM_ALL},
   {SZ_INOCULATION, "ButtonStop",     (LPVOID)&navOptions.inoc.bDispStop,     TRUE, PLATFORM_ALL},
   {SZ_INOCULATION, "ButtonExclude",  (LPVOID)&navOptions.inoc.bDispExcl,     FALSE, PLATFORM_ALL},

   {SZ_TSR, "ScanRun",       (LPVOID)&navOptions.tsr.bScanRun,      TRUE, PLATFORM_ALL},
   {SZ_TSR, "ScanOpen",      (LPVOID)&navOptions.tsr.bScanOpen,     FALSE, PLATFORM_ALL},
   {SZ_TSR, "ScanCreate",    (LPVOID)&navOptions.tsr.bScanCreate,   FALSE, PLATFORM_ALL},
   {SZ_TSR, "ScanAll",       (LPVOID)&navOptions.tsr.bScanAll,      FALSE, PLATFORM_ALL},
   {SZ_TSR, "KnownAction",   (LPVOID)&navOptions.tsr.uActionKnown,  TSR_PROMPT, PLATFORM_ALL},
   {SZ_TSR, "ButtonRepair",  (LPVOID)&navOptions.tsr.bDispRepair,   TRUE, PLATFORM_ALL},
   {SZ_TSR, "ButtonDelete",  (LPVOID)&navOptions.tsr.bDispDelete,   TRUE, PLATFORM_ALL},
   {SZ_TSR, "ButtonContinue",(LPVOID)&navOptions.tsr.bDispContinue, FALSE, PLATFORM_ALL},
   {SZ_TSR, "ButtonStop",    (LPVOID)&navOptions.tsr.bDispStop,     TRUE, PLATFORM_ALL},
   {SZ_TSR, "ButtonExclude", (LPVOID)&navOptions.tsr.bDispExclude,  FALSE, PLATFORM_ALL},

   {SZ_TSR, "ScanUnknown",   (LPVOID)&navOptions.tsr.bScanUnknown,  FALSE, PLATFORM_ALL},
   {SZ_TSR, "UnknownAction", (LPVOID)&navOptions.tsr.uActionUnknown,TSR_PROMPT, PLATFORM_ALL},
   {SZ_TSR, "ButtonUnknownRepair",  (LPVOID)&navOptions.tsr.bDispUnknownRepair,   TRUE, PLATFORM_ALL},
   {SZ_TSR, "ButtonUnknownDelete",  (LPVOID)&navOptions.tsr.bDispUnknownDelete,   TRUE, PLATFORM_ALL},
   {SZ_TSR, "ButtonUnknownContinue",(LPVOID)&navOptions.tsr.bDispUnknownContinue, TRUE, PLATFORM_ALL},
   {SZ_TSR, "ButtonUnknownExclude", (LPVOID)&navOptions.tsr.bDispUnknownExclude,  FALSE, PLATFORM_ALL},

   {SZ_TSR, "ScanMemory",     (LPVOID)&navOptions.tsr.bScanMem,      TRUE, PLATFORM_ALL},
   {SZ_TSR, "ScanMasterBoot", (LPVOID)&navOptions.tsr.bScanMBoot,    TRUE, PLATFORM_ALL},
   {SZ_TSR, "ScanBootRecs",   (LPVOID)&navOptions.tsr.bScanBootRecs, TRUE, PLATFORM_ALL},
   {SZ_TSR, "UseHMA",         (LPVOID)&navOptions.tsr.bUseHMA,       TRUE, PLATFORM_ALL},
   {SZ_TSR, "UseUMB",         (LPVOID)&navOptions.tsr.bUseUMB,       FALSE, PLATFORM_ALL},
   {SZ_TSR, "UseEMS",         (LPVOID)&navOptions.tsr.bUseEMS,       TRUE, PLATFORM_ALL},
   {SZ_TSR, "BypassKey",      (LPVOID)&navOptions.tsr.uBypassKey,    KEYS_ALT, PLATFORM_ALL},
   {SZ_TSR, "Unloadable",     (LPVOID)&navOptions.tsr.bUnloadable,   TRUE, PLATFORM_ALL},
   {SZ_TSR, "HideIcon",       (LPVOID)&navOptions.tsr.bHideIcon,     FALSE, PLATFORM_ALL},
   {SZ_TSR, "LowLevelFormat",     (LPVOID)&navOptions.tsr.uFormat,        TSR_ADV_ALLOW, PLATFORM_ALL},
   {SZ_TSR, "WriteHardBoot",      (LPVOID)&navOptions.tsr.uWriteHard,     TSR_ADV_ALLOW, PLATFORM_ALL},
   {SZ_TSR, "WriteFloppyBoot",    (LPVOID)&navOptions.tsr.uWriteFlop,     TSR_ADV_ALLOW, PLATFORM_ALL},
   {SZ_TSR, "WriteProgramFiles",  (LPVOID)&navOptions.tsr.uWriteExe,      TSR_ADV_ALLOW, PLATFORM_ALL},
   {SZ_TSR, "ReadOnlyAttribute",  (LPVOID)&navOptions.tsr.uROAttrib,      TSR_ADV_ALLOW, PLATFORM_ALL},
   {SZ_TSR, "CheckFloppyOnAccess",(LPVOID)&navOptions.tsr.bChkFlopAccess, TRUE, PLATFORM_ALL},
   {SZ_TSR, "CheckFloppyOnBoot",  (LPVOID)&navOptions.tsr.bChkFlopOnBoot, TRUE, PLATFORM_ALL},
   {SZ_TSR, "CheckBothFloppies",  (LPVOID)&navOptions.tsr.bChkBothFlops,  FALSE, PLATFORM_ALL},

   {SZ_SCANNER, "ScanMemory",     (LPVOID)&navOptions.scanner.bMemory,        TRUE, PLATFORM_ALL},
   {SZ_SCANNER, "ScanMasterBoot", (LPVOID)&navOptions.scanner.bMasterBootRec, TRUE, PLATFORM_ALL},
   {SZ_SCANNER, "ScanBootRecs",   (LPVOID)&navOptions.scanner.bBootRecs,      TRUE, PLATFORM_ALL},
   {SZ_SCANNER, "ScanAll",        (LPVOID)&navOptions.scanner.bScanAll,       FALSE, PLATFORM_ALL},
   {SZ_SCANNER, "KnownAction",    (LPVOID)&navOptions.scanner.uAction,        SCAN_PROMPT, PLATFORM_ALL},
   {SZ_SCANNER, "ButtonRepair",   (LPVOID)&navOptions.scanner.bDispRepair,    TRUE, PLATFORM_ALL},
   {SZ_SCANNER, "ButtonDelete",   (LPVOID)&navOptions.scanner.bDispDelete,    TRUE, PLATFORM_ALL},
   {SZ_SCANNER, "ButtonContinue", (LPVOID)&navOptions.scanner.bDispCont,      TRUE, PLATFORM_ALL},
   {SZ_SCANNER, "ButtonExclude",  (LPVOID)&navOptions.scanner.bDispExcl,      FALSE, PLATFORM_ALL},
   {SZ_SCANNER, "AllowNetworkScan",(LPVOID)&navOptions.scanner.bAllowNetScan, FALSE, PLATFORM_ALL},
   {SZ_SCANNER, "AllowScanStop",    (LPVOID)&navOptions.scanner.bAllowScanStop,TRUE, PLATFORM_ALL},
   {SZ_SCANNER, "ImmediateNotify",  (LPVOID)&navOptions.scanner.bPrestoNotify, FALSE, PLATFORM_ALL},
   {SZ_SCANNER, "PreselectFloppies",(LPVOID)&navOptions.scanner.bPresFlop,     FALSE, PLATFORM_ALL},
   {SZ_SCANNER, "PreselectLocalDrives", (LPVOID)&navOptions.scanner.bPresHard, TRUE, PLATFORM_ALL},
   {SZ_SCANNER, "PreselectNetworkDrives",(LPVOID)&navOptions.scanner.bPresNet, FALSE, PLATFORM_ALL},

   {SZ_GENERAL, "BackupFile",    (LPVOID)&navOptions.general.bBackupRep,        TRUE, PLATFORM_ALL},
   {SZ_GENERAL, "ScanCompressedFiles",  (LPVOID)&navOptions.general.bScanZipFiles,     FALSE, PLATFORM_ALL},
   {SZ_GENERAL, "ScanHighMemory",(LPVOID)&navOptions.general.bScanHighMemory,   FALSE, PLATFORM_ALL},

   {SZ_PASSWORD, "Protect",          (LPVOID)&navOptions.password.bProtect,       FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "MaxProtection",    (LPVOID)&navOptions.password.bMaxProtection, TRUE, PLATFORM_ALL},
   {SZ_PASSWORD, "ViewReports",      (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_VIEWACTIVITY],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "ClearLog",         (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_CLEARLOG],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "ViewVirusList",    (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_VIEWVIRUSLIST],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "ModifyVirusList",  (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_MODIVIRUSLIST],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "InocFiles",        (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_INOCFILES],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "UninocFiles",      (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_UNINOCFILES],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "ViewScanOptions",  (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_VIEWSCANOPT],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "ViewAutoProtectOptions",(LPVOID)&navOptions.password.bProtectFeature[PASSWORD_VIEWTSROPT],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "ViewAlertOptions", (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_VIEWALERTOPT],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "ViewExclusions",   (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_VIEWEXCLOPT],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "ModifyExclusions", (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_MODIEXCLUSIONS],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "ViewLogOptions",   (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_VIEWACTIVITYOPT],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "ViewInocOptions",  (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_VIEWINOCOPT],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "ViewGeneralOptions", (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_VIEWGENOPT],  FALSE, PLATFORM_ALL},
#ifdef BANKERS
   {SZ_PASSWORD, "ViewMonitorOptions",(LPVOID)&navOptions.password.bProtectFeature[PASSWORD_VIEWMONOPT],  FALSE, PLATFORM_ALL},
   {SZ_PASSWORD, "RegisterFile",     (LPVOID)&navOptions.password.bProtectFeature[PASSWORD_REGFILES],  FALSE, PLATFORM_ALL},
#endif  // BANKERS

#ifdef BANKERS
   {SZ_MONITOR, "MonitorFiles",         (LPVOID)&navOptions.monitor.bUseReg,        TRUE, PLATFORM_ALL},
   {SZ_MONITOR, "NewRegAction",         (LPVOID)&navOptions.monitor.uActionNewReg,  MONITOR_AUTO, PLATFORM_ALL},
   {SZ_MONITOR, "RegChangeAction",      (LPVOID)&navOptions.monitor.uActionRegChg,  MONITOR_PROMPT, PLATFORM_ALL},
   {SZ_MONITOR, "ButtonRegister",       (LPVOID)&navOptions.monitor.bDispReg,       TRUE, PLATFORM_ALL},
   {SZ_MONITOR, "ButtonContinue",       (LPVOID)&navOptions.monitor.bDispCont,      TRUE, PLATFORM_ALL},
   {SZ_MONITOR, "ButtonStop",           (LPVOID)&navOptions.monitor.bDispStop,      TRUE, PLATFORM_ALL},
   {SZ_MONITOR, "ButtonExclude",        (LPVOID)&navOptions.monitor.bDispExcl,      FALSE, PLATFORM_ALL},
#endif  // BANKERS

#ifndef CREATE_NAVBIN
   {SZ_PASSWORD, "ViewStartupOptions", (LPVOID)&navOptions.startup.bProtectFeature,  FALSE, PLATFORM_ALL},

   {SZ_INSTALL, "PromptForLocalDir",    (LPVOID) &navOptions.admin.bPromptForLocalDir,   TRUE, PLATFORM_ALL},
   {SZ_INSTALL, "UsePreviousLocalDir",  (LPVOID) &navOptions.admin.bUsePreviousLocalDir, TRUE, PLATFORM_NAV_3},
   {SZ_INSTALL, "InstallAutoProtect",   (LPVOID) &navOptions.admin.uSystemMonitor, SYS_MON_IN_CONFIGSYS, PLATFORM_NAV_3},
   {SZ_INSTALL, "RunNAV",               (LPVOID) &navOptions.admin.bRunNAV, TRUE, PLATFORM_NAV_3},
   {SZ_INSTALL, "AddToPath",            (LPVOID) &navOptions.admin.bAddToPath, TRUE, PLATFORM_NAV_3},
   {SZ_INSTALL, "RetainSettings",       (LPVOID) &navOptions.admin.bRetainSettings, TRUE, PLATFORM_ALL},
   {SZ_INSTALL, "LogoffIfNoAutoProtect",(LPVOID) &navOptions.admin.bLogoffIfNoMonitor, FALSE, PLATFORM_NAV_3},
   {SZ_INSTALL, "ConfirmAllPrompts",    (LPVOID) &navOptions.admin.bConfirmAllPrompts, FALSE, PLATFORM_NAV_3},
   {SZ_INSTALL, "InstallWindowsNAV",    (LPVOID) &navOptions.admin.bInstallWindowsNAV, TRUE, PLATFORM_NAV_3},
   {SZ_INSTALL, "LoadWindowsDrivers",   (LPVOID) &navOptions.admin.bLoadWindowsDrivers, TRUE, PLATFORM_NAV_3},
   {SZ_INSTALL, "CreateWindowsGroup",   (LPVOID) &navOptions.admin.bCreateWindowsGroup, TRUE, PLATFORM_NAV_3},
   {SZ_INSTALL, "PromptForCompany",     (LPVOID) &navOptions.admin.bPromptForCompany, TRUE, PLATFORM_ALL},
   {SZ_INSTALL, "PromptForName",        (LPVOID) &navOptions.admin.bPromptForName, TRUE, PLATFORM_ALL},
   {SZ_INSTALL, "ScanMemory",           (LPVOID) &navOptions.admin.bScanMemory, TRUE, PLATFORM_ALL},
   {SZ_INSTALL, "ScanBootRecs",         (LPVOID) &navOptions.admin.bScanHardDisk, TRUE, PLATFORM_ALL},
   {SZ_INSTALL, "ScanProgramFiles",     (LPVOID) &navOptions.admin.bScanProgramFiles, TRUE, PLATFORM_ALL},
   {SZ_INSTALL, "ActionAfterInstalling",(LPVOID) &navOptions.admin.uActionAfterInstalling, PROMPT_TO_REBOOT, PLATFORM_ALL},
   {SZ_INSTALL, "UpgradeOnly",          (LPVOID) &navOptions.admin.bUpgradeOnly, FALSE, PLATFORM_ALL},
   {SZ_INSTALL, "ScanAlways",           (LPVOID) &navOptions.admin.bScanAlways, FALSE, PLATFORM_ALL},
   {SZ_INSTALL, "BeforeCommandAlways",  (LPVOID) &navOptions.admin.bBeforeCommandAlways, FALSE, PLATFORM_ALL},
   {SZ_INSTALL, "AfterCommandAlways",   (LPVOID) &navOptions.admin.bAfterCommandAlways, FALSE, PLATFORM_ALL},
   {SZ_INSTALL, "ModifyAutoexec",       (LPVOID) &navOptions.admin.bAutoexecAction, AUTOEXEC_AUTO_MODIFY, PLATFORM_NAV_95},
   {SZ_INSTALL, "InstallAutoProtect",   (LPVOID) &navOptions.admin.bInstallAutoProtect, TRUE, PLATFORM_NAV_95},
   {SZ_INSTALL, "InstallStartupProtect",(LPVOID) &navOptions.admin.bInstallStartup, TRUE, PLATFORM_NAV_95},
   {SZ_INSTALL, "InstallTools",         (LPVOID) &navOptions.admin.bInstallTools, TRUE, PLATFORM_NAV_95},
   {SZ_INSTALL, "RescueDisk",           (LPVOID) &navOptions.admin.bCreateRescue, RESCUE_CREATE, PLATFORM_NAV_95},
   #endif

   {NULL,       NULL,            NULL,                         0, PLATFORM_ALL}
};


                                        //-------------------------------
                                        // Word variables
                                        //-------------------------------
WORDINI wordSettings[] =
{
   {SZ_ACTIVITY, "MaxLogSize",         (LPVOID)&navOptions.activity.uKiloBytes, 10, PLATFORM_ALL},

   {NULL,       NULL,            NULL,                         0, PLATFORM_ALL}
};

                                        //-------------------------------
                                        // DWord variables
                                        //-------------------------------
DWORDINI dwordSettings[] =
{
   {SZ_PASSWORD, "Password",     (LPVOID)&navOptions.password.dwPassword, 0, PLATFORM_ALL},

   {NULL,       NULL,            NULL,                         0, PLATFORM_ALL}
};

                                        // String variables
STRINGINI stringSettings[] =
   {
   // ------------------------------------------------------------------- //
   {SZ_ACTIVITY,"LogFile",      
                        (LPVOID) navOptions.activity.szLogFile, 
                        sizeof(navOptions.activity.szLogFile), "ACTIVITY.LOG", PLATFORM_ALL},
   {SZ_ALERTS,  "AlertMessage", 
                        (LPVOID)navOptions.alert.szAlertMsg, 
                        sizeof(navOptions.alert.szAlertMsg), SZ_EMPTY_STR, PLATFORM_ALL},
   {SZ_INOCULATION, "NetworkInocPath", 
                        (LPVOID)navOptions.inoc.szNetInocPath, 
                        sizeof(navOptions.inoc.szNetInocPath), "\\NCDTREE", PLATFORM_ALL},
   {SZ_GENERAL, "BackupExtension",  
                        (LPVOID)navOptions.general.szBackExt,
                        sizeof(navOptions.general.szBackExt), "VIR", PLATFORM_ALL},
   {SZ_GENERAL, "ProgramExtensions",  
                        (LPVOID)navOptions.general.szExt,
                        sizeof(navOptions.general.szExt), 
                        "386 BIN COM CPL DLL DRV EXE NAV NCP NED NNL OCX OV? SCR SYS VBX VXD", PLATFORM_ALL},
   {SZ_GENERAL, "DefaultExtensions",  
                        (LPVOID)navOptions.general.szDef,
                        sizeof(navOptions.general.szDef), 
                        "386 BIN COM CPL DLL DRV EXE NAV NCP NED NNL OCX OV? SCR SYS VBX VXD", PLATFORM_ALL},
   {SZ_REGISTRATION, "Name",    
                        (LPVOID)navOptions.user.szName,
                        sizeof(navOptions.user.szName), SZ_EMPTY_STR, PLATFORM_ALL},
   {SZ_REGISTRATION, "Company", 
                        (LPVOID)navOptions.user.szCompany,
                        sizeof(navOptions.user.szCompany), SZ_EMPTY_STR, PLATFORM_ALL},

#ifndef CREATE_NAVBIN
   {SZ_INSTALL, "NetworkDir",
                       (LPVOID) navOptions.admin.szNetworkDir,
                       sizeof(navOptions.admin.szNetworkDir), SZ_EMPTY_STR, PLATFORM_ALL},
   {SZ_INSTALL, "LocalDir",
                       (LPVOID) navOptions.admin.szLocalDir,
                       sizeof(navOptions.admin.szLocalDir), "C:\\NAV", PLATFORM_ALL},
   {SZ_INSTALL, "Company",
                       (LPVOID) navOptions.admin.szCompany,
                       sizeof(navOptions.admin.szCompany), SZ_EMPTY_STR, PLATFORM_ALL},
   {SZ_INSTALL, "BeforeCommand",
                       (LPVOID) navOptions.admin.szBeforeCommand,
                       sizeof(navOptions.admin.szBeforeCommand), SZ_EMPTY_STR, PLATFORM_ALL},
   {SZ_INSTALL, "AfterCommand",
                       (LPVOID) navOptions.admin.szAfterCommand,
                       sizeof(navOptions.admin.szAfterCommand), SZ_EMPTY_STR, PLATFORM_ALL},
   {SZ_INSTALL, "LogoffCommand",
                       (LPVOID) navOptions.admin.szLogoffCommand,
                       sizeof(navOptions.admin.szLogoffCommand), SZ_EMPTY_STR, PLATFORM_NAV_3},
   {SZ_INSTALL, "ScannerCmdLine",
                       (LPVOID) navOptions.admin.szScannerCmdLine,
                       sizeof(navOptions.admin.szScannerCmdLine), "/L", PLATFORM_ALL},
   {SZ_INSTALL, "AutoProtectCmdLine",
                       (LPVOID) navOptions.admin.szSystemMonitorCmdLine,
                       sizeof(navOptions.admin.szSystemMonitorCmdLine), SZ_EMPTY_STR, PLATFORM_NAV_3},
   {SZ_INSTALL, "Device",
                       (LPVOID) navOptions.admin.szDevice,
                       sizeof(navOptions.admin.szDevice), "DEVICE=", PLATFORM_NAV_3},
   {SZ_INSTALL, "SaveAutoexecChangesTo",
                       (LPVOID) navOptions.admin.szSaveAutoexecTo,
                       sizeof(navOptions.admin.szSaveAutoexecTo), SZ_EMPTY_STR, PLATFORM_NAV_95},
   {SZ_INSTALL, "SymantecSharedFiles",
                       (LPVOID) navOptions.admin.szSymantecFiles,
                       sizeof(navOptions.admin.szSymantecFiles), SZ_EMPTY_STR, PLATFORM_NAV_95},
#endif

   {NULL, NULL,  NULL, 0, NULL, PLATFORM_ALL}
   };


#ifndef SYM_DOS

// ------------------------------------------------------------------- //
// Note: The position of the entries in this array must remain         //
//       relative to the PLATFORM_ values. There must be one entry     //
//       per platform.
// ------------------------------------------------------------------- //



BYTEINI PlatformSettings[] =
   {
   // ------------------------------------------------------------------- //
   {SZ_PLATFORM, "Nav95",      NULL,     FALSE, PLATFORM_ALL},
   {SZ_PLATFORM, "Nav3",       NULL,     TRUE,  PLATFORM_ALL},
   {NULL,        NULL,         NULL,     0,     0}
   };

#endif

#endif

#endif

