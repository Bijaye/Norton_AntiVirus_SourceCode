////////////////////////////////////////////////////////////////////////////////
//
// File: setup.h
//
////////////////////////////////////////////////////////////////////////////////


// global define statements (do not translate)
#define CONFIG_SECTION_NAME_NAV             "NAV"
#define CUSTOM_DLLNAME                      "NAVINSTNT.DLL"
#define PERSONALITY_DLLNAME                 "N32PDLL.DLL"
#define QUAKE_DLL                           "S32NAVN.DLL"
#define COMCTL32_UPDATER                    "40COMUPD.EXE"
#define DEFAULT_SKU_NUMBER                  "00-00-0000"
#define EXTRA_TARGETDISK_MEGS               5
#define EXTRA_WINDISK_MEGS                  5
#define NAV_EXTERNAL_VERSION                "5.02"
#define NAV_INTERNAL_VERSION                "5.32"
#define NAV_SCAN_WIN_CLASS_NAME             "ScanWindow"
#define NAV_WIN_CLASS_NAME                  "nav"
#define NORTON_SCHEDULER_EXE_NAME           "nschednt.exe"
#define NSPLUGIN_EXE                        "nsplugin.exe"
#define NAV_LU_EXE_NAME                     "navlu32.exe"
#define NAV_LU_CPL_NAME                     "s32lucp1.cpl"
#define NSPLUGIN_INSTALL_SWITCHES           "/i /n"
#define PARTNUMER_SECTION_NAME              "PartNumber"
#define SCRIPT_CUSTOM_DLLNAME               NAVINSTNT
#define SETUP_REVIEW_TAB                    "    "
#define ULOGFILE_NAME                       "_ISNAVNT.ULG"
#define INSTALL_OPTIONS_FILE                "INSTOPTS.INI"
#define PRESERVE_GROUP_ITEMS                "Preserve Group Items"
#define NETSCAPE_INTEGRATION_SECTION        "Netscape Integration"
#define NETSCAPE_PLUGIN_SWITCH              "InstallAsPlugin"
#define INSTALL_OPTIONS_SECTION             "Install Options"
#define OVERWRITE_SCHEDULE_DAT_SWITCH       "OverwriteScheduleDat"
#define CORPORATE_VERSION_SWITCH            "CorporateFeatures"
#define DISABLE_QCONSOLE_SWITCH             "DisableQuarantineConsole"
#define LIVEUPDATE_SCHEDULE_DLG_SECTION	    "LUScheduleDlg"
#define LIVEUPDATE_SCHEDULE_DLG_SWITCH      "DisableLUSchedule"
#define SUPPRESS_LOCKED_DRIVE_MSG_SECTION   "Skip Locked Drive Message"
#define SUPPRESS_LOCKED_DRIVE_MSG_SWITCH    "SkipLockedDrvMsg"
#define SKIP_SHUTDOWN_SCAN_SECTION          "Skip Shutdown Scan"
#define SKIP_SHUTDOWN_SCAN_SWITCH	    "SkipShutdownScan"
#define DEFANNUITY_SCRIPT_DLL               DefAnnty
#define DEFANNUITY_DLL                      "DefAnnty.dll"
#define DEFANNUITY_SECTION                  "DefAnnuity"
#define DEFANNUITY_ENABLED_SWITCH           "Enabled"
#define DEFANNUITY_CONFIRMATION_SWITCH      "ConfirmationNumber"
#define DEFANNUITY_FREELENGTH_SWITCH        "FreeLength"
#define DEFANNUITY_SUBLENGTH_SWITCH         "SubscriptionLength"
#define DEFANNUITY_FREEENDWARNING_SWITCH    "FreeEndWarning"
#define DEFANNUITY_SUBENDWARNING_SWITCH     "SubscriptionEndWarning"
#define NAV_MUTEX_NAME                      "NAV_SETUP_MUTEX_OOGACHAKA"
#define SYSTEMWORKS_SECTION                  "SystemWorks"
#define SYSTEMWORKS_ENABLED_SWITCH           "EnableSysWorks"
#define MICRO_DEFS_DIR						 "MicroDef"
#define HUB_DEFS_DIR						 "HubDef"
#define NAV_EXTRA_FILES_DIR					 "Tonav"
#define LU_EXTRA_FILES_DIR					 "Tolu"
#define NAV_FILE_LIST1                       "mknpschd.exe|alertsvc.exe|BackLog.exe|32scanw.exe|navwnt.exe|NPSCheck.EXE|NPSSVC.EXE|NSCHEDNT.EXE|NSPlugIn.exe|NTaskMgr.exe|navapsvc.exe|navapw32.exe|NavLu32.exe|NavRunr.exe|NavUStub.exe|QCONSOLE.EXE|NAVABOUT.DLL|AlertUI.DLL|APWCMD.DLL|chan32I.dll"
#define NAV_FILE_LIST2                       "dec2.dll|dec2amg.dll|dec2arj.dll|dec2cab.dll|dec2id.dll|dec2lha.dll|S32INTEG.DLL|S32NAVN.DLL|scandres.dll|SDFLT32I.DLL|SDPCK32I.DLL|SDSND32I.DLL|SDSOK32I.DLL|SDSTP32I.DLL|SFSTR32I.DLL|SMSTR32I.DLL|symamg32.dll|symlha.dll|V32SCAN.DLL|dec2lz.dll"
#define NAV_FILE_LIST3                       "dec2mime.dll|dec2zip.dll|DefAnnty.dll|fslink.dll|N32ALERT.DLL|n32call.dll|n32secur.dll|NetBrExt.dll|patch32I.dll|S32ALOGN.DLL|XNTALERT.DLL|xntalog.dll|xntexclu.dll|xntinoc.dll|XNTOPTS.DLL|XNTSERVE.DLL|xntwork.dll|xntxutil.dll|N32VLIST.DLL|n32zip.dll"
#define NAV_FILE_LIST4                       "navap32.dll|NAVInstNT.dll|navntutl.dll|NavRPC.dll|navshell.dll|aboutsw.dll|quar32.dll|scandlvr.dll|N32PDLL.DLL|qconres.dll"
#define LU_FILE_LIST						 "S32LUIS1.DLL|S32LUUZ1.DLL|S32LUHM1.DLL|S32LUHF1.DLL|S32LIVE1.DLL|UNINST.EXE"



// Version numbers
#define THIS_PRODUCT_MAJOR                  5
#define THIS_PRODUCT_MINOR1                 3
#define THIS_PRODUCT_MINOR2                 2

#define GRYPHNEC_MAJOR                      2
#define GRYPHNEC_MINOR1                     0
#define GRYPHNEC_MINOR2                     4

#define ROSWELL_TESLA_NEC_MAJOR                5
#define ROSWELL_TESLA_NEC_MINOR1               0
#define ROSWELL_TESLA_NEC_MINOR2               2

// service types (for NT, from "winnt.h")
#define SERVICE_KERNEL_DRIVER          0x00000001
#define SERVICE_FILE_SYSTEM_DRIVER     0x00000002
#define SERVICE_ADAPTER                0x00000004
#define SERVICE_RECOGNIZER_DRIVER      0x00000008
#define SERVICE_DRIVER                 (SERVICE_KERNEL_DRIVER | SERVICE_FILE_SYSTEM_DRIVER | SERVICE_RECOGNIZER_DRIVER)
#define SERVICE_WIN32_OWN_PROCESS      0x00000010
#define SERVICE_WIN32_SHARE_PROCESS    0x00000020
#define SERVICE_WIN32                  (SERVICE_WIN32_OWN_PROCESS | SERVICE_WIN32_SHARE_PROCESS)
#define SERVICE_INTERACTIVE_PROCESS    0x00000100
#define SERVICE_TYPE_ALL               (SERVICE_WIN32  | SERVICE_ADAPTER | SERVICE_DRIVER  | SERVICE_INTERACTIVE_PROCESS)
#define SERVICE_LOGFILE_SEPARATOR      "="


// service-agent names
#ifdef  DEC_ALPHA_BUILD
#define NAVEX_SERVICE_BINARY_NAME           "navexda.sys"
#else
#define NAVEX_SERVICE_BINARY_NAME           "navex15.sys"
#endif

#define NAV_SCANNER_DLL                     "S32alogn.dll"
#define NAVALERT_SERVICE_BINARY_NAME        "alertsvc.exe"
#define NAVALERT_SERVICE_NAME               "NAV Alert"
#define NAVAP_AGENT_BINARY_NAME             "navapw32.exe"
#define NAVAP_SERVICE_BINARY_NAME           "navapsvc.exe"
#define NAVAP_SERVICE_NAME                  "NAV Auto-Protect"
#define NAVAP_SYS_BINARY_NAME               "navap.sys"
#define NAVAP_SYS_NAME                      "NAVAP"
#define NAVENG_SERVICE_NAME                 "NAVENG"
#define NAVENG_SERVICE_BINARY_NAME          "naveng.sys"
#define NAVEX_SERVICE_NAME                  "NAVEX15"
#define PROGRAM_SCHEDULER_BINARY_NAME       "npssvc.exe"
#define PROGRAM_SCHEDULER_SERVICE_NAME      "Norton Program Scheduler"
#define SYMEVENT_SERVICE_BINARY_NAME        "symevent.sys"
#define SYMEVENT_SERVICE_NAME               "SymEvent"


// defines for the SYMREG COM object (online registration)
#ifdef  DEC_ALPHA_BUILD
#define SYMREG_DLL_NAME                     "ARAREG.DLL"
#define SYMREG_PROTOTYPE_DLL_NAME           ARAREG
#else
#define SYMREG_DLL_NAME                     "IRAREG.DLL"
#define SYMREG_PROTOTYPE_DLL_NAME           IRAREG
#endif

#define SYMREG_INSTALLATION_TYPE_CORP       "/machine"
#define SYMREG_INSTALLATION_TYPE_RETL       "/user"
#define SYMREG_PRODUCT_COMPANY              "/co:Symantec"
#define SYMREG_PRODUCT_LANGUAGE             "/lang:English"
#define SYMREG_PRODUCT_NAME_OEM             "/name:Norton AntiVirus (OEM)"
#define SYMREG_PRODUCT_NAME                 "/name:Norton AntiVirus"
#define SYMREG_PRODUCT_SKU                  "/sku:07-00-01906"
#define SYMREG_PRODUCT_VERSION              "5.02"
                                        // SYMREG_PRODUCT_VERSION is only
                                        // used as a default for GetProfString(),
                                        // but BE SURE it matches the
                                        // PublicVersion in VERSION.DAT!!

#define LU_DEFAULT_PUBLIC_REV               "5.02.06"
                                        // This is also used only as a default.
                                        // BE SURE it matches PublicRevision
                                        // in VERSION.DAT!!


#define SYMTRIAL_PRODUCT_COMPANY            "Symantec"
#define SYMTRIAL_PRODUCT_NAME               "Norton AntiVirus"
#define SYMTRIAL_PRODUCT_VERSION_MAJOR      "5"
#define SYMTRIAL_PRODUCT_VERSION_MINOR      "3"

#ifdef  DEC_ALPHA_BUILD
#define SYMTRIAL_DLL_NAME                   "ARATRIAL.DLL"
#define SYMTRIAL_PROTOTYPE_DLL_NAME         ARATRIAL
#else
#define SYMTRIAL_DLL_NAME                   "IRATRIAL.DLL"
#define SYMTRIAL_PROTOTYPE_DLL_NAME         IRATRIAL
#endif


// IDs for custom dialogs (_isuser.dll)
#define IDC_RESUME_BUTTON                       1
#define IDC_EXIT_BUTTON                         2
#define IDD_EXIT_DIALOG                         102
#define IDD_ADVOTEXT_1                          13037
#define IDD_ADVOTEXT_1_NEC                      13038
#define IDD_ADVOTEXT_2                          13036
#define IDC_AUTO_PROTECT_CHECKBOX               1013
#define IDC_BUTTON_NETSCAPE_ADVANCED            1015
#define IDC_KEEP_SETTINGS_NO                    1010
#define IDC_KEEP_SETTINGS_YES                   1009
#define IDC_NETSCAPE_CHECKBOX                   1003
#define IDC_RUN_LIVE_UPDATE_CHECKBOX            1016
#define IDC_RUN_NAV_CHECKBOX                    1017
#define IDC_SCHEDULE_SCAN_CHECKBOX              1012
#define IDD_INITIAL_SETTINGS                    13033
#define IDD_KEEP_SETTINGS                       13032
#define IDD_SCAN_FOR_VIRUSES                    13030
#define IDS_ADVO1                               "DlgAdvo1"
#define IDS_ADVO2                               "DlgAdvo2"
#define IDS_EXIT_DIALOG                         "DlgExitHandler"
#define IDS_INITIAL_SETTINGS                    "DlgInitialSettings"
#define IDS_KEEP_SETTINGS                       "DlgKeepSettings"
#define IDS_SCAN_FOR_VIRUSES                    "DlgScanForViruses"


// define values for custom dialogs during silent installs
#define SILENT_SECTION_ADVO1                    "DlgAdvo1-0"
#define SILENT_SECTION_ADVO2                    "DlgAdvo2-0"
#define SILENT_SECTION_INITIAL_SETTINGS         "DlgInitialSettings-0"
#define SILENT_SECTION_NETSCAPE_PLUGINS         "DlgNetscapePlugins-0"
#define SILENT_SECTION_SCAN_FOR_VIRUSES         "DlgScanForViruses-0"
#define SILENT_VALUE_NAME_NETSCAPE_PLUGINS      "NetscapePlugins"
#define SILENT_VALUE_NAME_RESULT                "Result"
#define SILENT_VALUE_NAME_RUN_LIVEUPDATE        "RunLiveUpdateAfterInstall"
#define SILENT_VALUE_NAME_RUN_NAVSCAN           "RunScanAfterInstall"
#define SILENT_VALUE_NAME_SCAN_FOR_VIRUSES      "ScanForViruses"
#define SILENT_VALUE_NAME_SCHEDULE_SCAN         "ScheduleScan"
#define SILENT_VALUE_NAME_START_AUTO_PROTECT    "StartAutoProtect"


// registry keys (do not translate)
#define SERVICE_PACK2_HOTFIX_KEY        "\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\HOTFIX\\Q135707Q141239NTOSKRNLFIX"
#define DISABLE_SPLASH_KEY              "Software\\Symantec\\Norton AntiVirus NT\\Clinic"
#define DISABLE_SPLASH_VALUE            "DisableSplashScreen"
#define MSAPP_PATH_KEY                  "\\Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\"
#define MSSHARED_DLL_KEY                "\\Software\\Microsoft\\Windows\\CurrentVersion\\SharedDlls"
#define MSUNINSTALL_KEY                 "\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
#define MSWINDOWS_KEY                   "\\Software\\Microsoft\\Windows"
#define MS_WINDOWS_CURRENT_VERSION_KEY  "\\Software\\Microsoft\\Windows\\CurrentVersion"
#define NAVNT_INSTALL_KEY               "\\Software\\Symantec\\Norton AntiVirus NT\\Install"
#define NAVNT_KEY                       "\\Software\\Symantec\\Norton AntiVirus NT"
#define NAV_INSTALLDIR_VALUENAME        "InstallDir"
#define NAV_KEY                         "\\Software\\Symantec\\Norton AntiVirus"
#define NAV_LIVE_UPDATE_FILE            "\\Software\\Classes\\NavLuFile"
#define NAV_LIVE_UPDATE_FILE_SHELL      "\\Software\\Classes\\NavLuFile\\shell"
#define NAV_LIVE_UPDATE_REGKEY_PATH     "\\Software\\Symantec\\Norton AntiVirus\\LiveUpdate"
#define NAV_SCANNER_KEY                 "\\Software\\Symantec\\Norton AntiVirus Scanner"
#define NAV_VIRUS_DEFS_LOCATION_KEY     "\\Software\\Symantec\\Norton AntiVirus\\Virus Defs\\Location"
#define NETSCAPE_KEY                    "\\Software\\Netscape\\Netscape Navigator"
#define NETSCAPE_MAIN_REGKEY            "\\Software\\Netscape\\Netscape Navigator\\Main"
#define SOFTWARE_CLASSES_CLASSID_KEY    "\\Software\\Classes\\CLSID"
#define SOFTWARE_CLASSES_KEY            "\\Software\\Classes"
#define SYMANTEC_INSTALLED_APPS_KEY     "\\Software\\Symantec\\InstalledApps"
#define SYMANTEC_KEY                    "\\Software\\Symantec"
#define SYMANTEC_SHAREDUSAGE_KEY        "\\Software\\Symantec\\SharedUsage"
#define SYMANTEC_SHARED_NAV_VALUENAME   "NAVNT"
#define SYMEVENT_SERVICE_KEY            "\\SYSTEM\\CurrentControlSet\\Services\\symevent"
#define WINDOWS_HELP_KEY                "\\Software\\Microsoft\\Windows\\Help"
#define CORP_FEATURES_VALUENAME         "CorporateFeatures"
#define NAV_OEM_SYSWORKS_KEY            "\\SOFTWARE\\Symantec\\Norton AntiVirus (OEM)\\5.02\\English\\Installed About Extensions\\SystemWorks"
#define NAV_SYSWORKS_KEY                "\\SOFTWARE\\Symantec\\Norton AntiVirus\\5.02\\English\\Installed About Extensions\\SystemWorks"
#define NAV_SYSWORKS_VALUENAME          "Enable"
#define RUNONCE_KEY				        "\\Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce"

// disk directory paths and filenames
#define INFODESK_COMPONENT              "Norton AntiVirus for Windows NT\\Infodesk"
#define LIVE_UPDATE_PATH                "\\Program Files\\Symantec\\LiveUpdate"
#define RUN_ONCE_BATCH_FILENAME         "navrunr.bat"
#define RUN_ONCE_PIF_FILENAME           "navrunr.pif"
#define SYMANTEC_PATH                   "\\Program Files\\Symantec"
#define VIRSCAN_DAT_FILENAME            "virscan.dat"


// previous version logic
#define CLEAN_INSTALL                   0
#define INSTALLING_OVER_NEWER_VERSION   1
#define INSTALLING_OVER_OLDER_VERSION   2
#define INSTALLING_OVER_SAME_VERSION    3


// used for comparing symevent before and after
#define INSTALLED_SAME_SYMEVENT         0
#define INSTALLED_OLDER_SYMEVENT        1
#define INSTALLED_NEWER_SYMEVENT        2
#define NO_COMPARISON_MADE              3


// Hawking tracking for aborting installs
#define HAWKING_NO_ACTIVITY             0
#define HAWKING_TEMP_DIR_ONLY           1
#define HAWKING_USING_DEFS              2


// first version of NAV that shipped LiveUpdate (w95 is "4.5", NT is "2.01")
#define NAV_1ST_LU_MAJOR                2
#define NAV_1ST_LU_MINOR1               0
#define NAV_1ST_LU_MINOR2               1
#define NAV_1ST_LU_MINOR3               0


// media component names (used to access components within the InstallShield
// compressed library)
#define DRIVERS_COMPONENT                   "Drivers"
#define LIVEUPDATE_COMPONENT                "LiveUpdate"
#define LIVEUPDATE_NS_COMPONENT             "LiveUpdateNS"
#define NAVTARGET_SERVER_COMPONENT          "NAVTARGET Server"
#define NAVTARGET_WORKSTATION_COMPONENT     "NAVTARGET Workstation"
#define NAV_WINSYS_NS_COMPONENT             "NAV WinsysNS"
#define NEW_SETTINGS_COMPONENT              "New Settings"
#define NEW_SETTINGS_COMPONENT_AT           "New Settings-AT"
#define NEW_SETTINGS_COMPONENT_NEC          "New Settings-NEC"
#define PROGRAM_COMPONENT                   "Program"
#define QCONSOLE_COMPONENT                  "Quarantine Console"
#define ROOT_COMPONENT                      "Norton AntiVirus for Windows NT"
#define SCHEDULED_SCAN_COMPONENT            "Default Scheduled Scan"
#define SEVINST_COMPONENT                   "Sevinst"
#define SYMANTEC_STRING_SHARED_COMPONENT    "SymantecStringShared"
#define SYMREG_COMPONENT                    "Symreg"
#define WINDOWS_COMPONENT                   "Windows"
#define WINDOWS_COMPONENT_AT                "Windows-AT"
#define WINDOWS_COMPONENT_NEC               "Windows-NEC"



// these values are used to read the "navstart.dat" file; the values are originally
// defined in the header for the custom DLL ("navinst.h").  InstallShield claims the
// use of "C-style" header files can cause unpredictable behavior, so we've just
// pasted the defined values here.
#define NAVSTART_OPTION_SCAN_DOS_FILES          500
#define NAVSTART_OPTION_SCAN_WINDOWS_FILES      501
#define NAVSTART_OPTION_LOAD_TSR                502
#define NAVSTART_OPTION_LOAD_AP_AT_STARTUP      503
#define NAVSTART_OPTION_PROTECT_FEATURE         504


// error values
#define ERR_INPUT_CONFIG_FILE                   200


// function prototypes from "setup.rul"
prototype   CheckDiskSpace(STRING);
prototype   CheckPreviousVersion(BYREF STRING);
prototype   CheckRequirements();
prototype   CleanUpInstall();
prototype   CopySymantecStringSharedFiles();
prototype   CopyUncompressedFiles();
prototype   CreateLiveUpdateKeys();
prototype   CreateRegDBEntries();
prototype   DecrementPrevVerSharedUsageCounts();
prototype   DialogShowInitialSettings();
prototype   DialogShowNetscapeDetected();
prototype   DialogShowScanForViruses();
prototype   DialogShowSdAskDestPath();
prototype   DialogShowSdFinishReboot();
prototype   DoCopyMFC42DLLs();
prototype   Get351SharedSymantecDir(BYREF  STRING);
prototype   HandleError(LONG);
prototype   HandlePostInstallationTasks();
prototype   HandleMSSelfExtractors();
prototype   IncrementSymantecSharedKey();
prototype   InitCustomDialogs();
prototype   InitGlobalVars();
prototype   InstalledNewerSymevent(STRING);
prototype   InstallNavServices(BOOL);
prototype   InstallingOverPreAPVersion();
prototype   IsNavRunning();
prototype   IsNTServer();
prototype   MoveFileData();
prototype   MoveNonSharedFiles();
prototype   MySdAskDestPath(STRING, STRING, BYREF STRING, NUMBER);
prototype   OurSevinstIsNewer();
prototype   PreDataMoveHook();
prototype   PreventServerInstall();
prototype   ProcessAfterDataMove();
prototype   ProcessBeforeDataMove();
prototype   ProcessCommandLine();
prototype   ReadConfigInfo(STRING);
prototype   RemovePrevVerFiles();
prototype   RemovePrevVerRegEntries();
prototype   RemovePreviousVersion();
prototype   ServicesMarkedForDeletion();
prototype   SetInstalledRegEntry();
prototype   SetupFolders();
prototype   SetupInstall();
prototype   SetupTrialVersion();
prototype   ShowPreCopyDialogs();
prototype   StopNavServices(BOOL);
prototype   UpdateSetupReviewList();
prototype   DisableRegistration();
prototype   ValidNewDefs();
prototype   InstallMicroDefs();
prototype   SetupMicroVirusDefs();
prototype   CopyFilesToNAVDir();
prototype   CopyFilesToLUDir(STRING);
prototype   InstallLiveUpdate();



// ****************************************************************************
// ****************************************************************************




// functions exported from "NAVInst" custom DLL
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.CopyVirusDefinitionFiles(STRING);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.EnableQuarService();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.GetCommonStartupDisplayName(STRING, NUMBER);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.GetDefsDirectory(STRING, STRING, BOOL);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.GetFileVersionString ( STRING, STRING );
prototype   NUMBER  SCRIPT_CUSTOM_DLLNAME.GetNavStartInfo(STRING, NUMBER);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.InstallSingleService(STRING, STRING, STRING
                                                                            , LONG
                                                                            , BOOL
                                                                            , STRING);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.InstallServices(STRING, STRING, BOOL);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.IsFileLoaded(STRING);
prototype   NUMBER  SCRIPT_CUSTOM_DLLNAME.GetNT40ServicePack();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.IsOEMPersonalityDLL(STRING);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.IsServiceMarkedForDeletetion(STRING);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.IsServiceRunning(STRING);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.RemoveAPUI();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.RemoveQConsole();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.RemoveServices();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.SetNavStartInfo(STRING, NUMBER, BOOL);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.StopServices();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.StopSingleService(STRING, BOOL, BOOL);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.StringContainsDBCSChars(STRING);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.UninstallVirusDefinitions();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.MachineIsNEC();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.CreateProcessAndWait ( STRING, BYREF NUMBER );
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.CreateNAVMutex ( STRING );
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.ReleaseNAVMutex();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.CreateInternetShortcut ( STRING, STRING );

// functions exported from symreg DLL (online registration COM object)
prototype   BOOL    SYMREG_PROTOTYPE_DLL_NAME.RegisterUser(HWND, LONG, STRING, INT);

// functions exported from symtrial DLL (trial version COM object)
prototype   BOOL    SYMTRIAL_PROTOTYPE_DLL_NAME.SetupTrialWare(STRING, STRING
                                                                    , STRING, NUMBER);

// functions exported from "Kernel32" DLL (Windows NT/95 system DLL)
prototype   NUMBER  Kernel32.GetCurrentDirectory(NUMBER,POINTER);
prototype   BOOL    Kernel32.SetCurrentDirectory(STRING);

// functions exported from DEFANNUITY_SCRIPT_DLL

prototype   NUMBER  DEFANNUITY_SCRIPT_DLL.DefAnnuityInitialize(NUMBER, NUMBER, NUMBER, NUMBER, NUMBER, NUMBER, NUMBER, NUMBER);

// sac
#define CLUSTER_DLLNAME					"NAVCS.DLL"
prototype 	NUMBER	NAVCS.ClusterIsServer();
prototype	NUMBER	NAVCS.CreateNAVResources();























