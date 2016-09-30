////////////////////////////////////////////////////////////////////////////////
//
// File: setup.h
//
////////////////////////////////////////////////////////////////////////////////


// global define statements (do not translate)
#define CONFIG_SECTION_NAME_NAV             "NAV"
#define CUSTOM_DLLNAME                      "NAVINST95.DLL"
#define PERSONALITY_DLLNAME                 "N32PDLL.DLL"
#define QUAKE_DLL                           "S32NAVN.DLL"
#define COMCTL32_UPDATER                    "40COMUPD.EXE"
#define DEFAULT_SKU_NUMBER                  "00-00-00000"
#define EXTRA_TARGETDISK_MEGS               5
#define EXTRA_WINDISK_MEGS                  5
#define INSTALL_OPTIONS_FILE                "INSTOPTS.INI"
#define INSTALL_CONFIG_FILE                 "CONFIG.INF"
#define NAV_WIN_CLASS_NAME                  "nav"
#define NSPLUGIN_EXE                        "nsplugin.exe"
#define NAV_LU_EXE_NAME                     "navlu32.exe"
#define NAV_LU_CPL_NAME                     "s32lucp1.cpl"
#define NSPLUGIN_INSTALL_SWITCHES           "/i /n"
#define NTASKMGR_EXE_NAME                   "ntaskmgr.exe"
#define NTASKMGR_ADD_SCAN_SWITCH            " /n"
#define NTASKMGR_USE_98_SCHED_SWITCH        " /w"
#define NTASKMGR_CONVERT_NPS_EVENTS_SWITCH  " /c"
#define NTASKMGR_SILENT_MODE_SWITCH         " /s"
#define PARTNUMER_SECTION_NAME              "PartNumber"
#define SCRIPT_CUSTOM_DLLNAME               NAVINST95
#define SETUP_REVIEW_TAB                    "    "
#define ULOGFILE_NAME                       "_ISNAV95.ULG"
#define PRESERVE_GROUP_ITEMS                "Preserve Group Items"
#define NETSCAPE_INTEGRATION_SECTION        "Netscape Integration"
#define NETSCAPE_PLUGIN_SWITCH              "InstallAsPlugin"
#define INSTALL_OPTIONS_SECTION             "Install Options"
#define OVERWRITE_SCHEDULE_DAT_SWITCH       "OverwriteScheduleDat"
#define FORCE_NPS_ON_98_SWITCH              "ForceNPSOnWin98"
#define CORPORATE_VERSION_SWITCH            "CorporateFeatures"
#define DISABLE_QCONSOLE_SWITCH             "DisableQuarantineConsole"
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


// Version numbers
#define THIS_PRODUCT_MAJOR                  5
#define THIS_PRODUCT_MINOR1                 3
#define THIS_PRODUCT_MINOR2                 1

#define KIRIN_NEC_MAJOR                     4
#define KIRIN_NEC_MINOR1                    5
#define KIRIN_NEC_MINOR2                    3

#define ROSWELL_NEC_MAJOR                   5
#define ROSWELL_NEC_MINOR1                  0
#define ROSWELL_NEC_MINOR2                  1


// defines for the SYMREG COM object (online registration)
#define SYMREG_DLL_NAME                     "IRAREG.DLL"
#define SYMREG_INSTALLATION_TYPE_CORP       "/machine"
#define SYMREG_INSTALLATION_TYPE_RETL       "/user"
#define SYMREG_PRODUCT_COMPANY              "/co:Symantec"
#define SYMREG_PRODUCT_LANGUAGE             "/lang:English"
#define SYMREG_PRODUCT_NAME_OEM             "/name:Norton AntiVirus (OEM)"
#define SYMREG_PRODUCT_NAME                 "/name:Norton AntiVirus "
#define SYMREG_PRODUCT_VERSION              "5.01"
                                        // SYMREG_PRODUCT_VERSION is only
                                        // used as a default for GetProfString(),
                                        // but BE SURE it matches the
                                        // PublicVersion in VERSION.DAT!!

#define LU_DEFAULT_PUBLIC_REV               "5.01.00"
                                        // This is also used only as a default.
                                        // BE SURE it matches PublicRevision
                                        // in VERSION.DAT!!


#define DIALOG_CLASS                    "#32770"

// IDs for custom dialogs (_isuser.dll)
#define IDC_RESUME_BUTTON               1
#define IDC_EXIT_BUTTON                 2
#define IDD_EXIT_DIALOG                 102
#define IDC_STATIC_NETSCAPE1            1002
#define IDC_NETSCAPE_CHECKBOX           1003
#define IDC_STATIC_NETSCAPE3            1004
#define IDC_SCAN_FOR_VIRUSES2           1006
#define IDC_KEEP_SETTINGS1              1007
#define IDC_KEEP_SETTINGS2              1008
#define IDC_KEEP_SETTINGS_YES           1009
#define IDC_KEEP_SETTINGS_NO            1010
#define IDC_INITIAL_SETTINGS1           1011
#define IDC_SCHEDULE_SCAN_CHECKBOX      1012
#define IDC_AUTO_PROTECT_CHECKBOX       1013
#define IDC_AUTO_PROTECT_TEXT           1014
#define IDC_BUTTON_NETSCAPE_ADVANCED    1015
#define IDC_BOOT_SCAN_CHECKBOX          1015
#define IDC_RUN_LIVE_UPDATE_CHECKBOX    1016
#define IDC_BOOT_SCAN_TEXT              1016
#define IDC_RUN_NAV_CHECKBOX            1017
#define IDC_RESCUE_CHECKBOX             1018
#define DLG_TEMPLATE                    13029
#define IDD_TEMPLATE                    13029
#define IDD_SCAN_FOR_VIRUSES            13030
#define IDD_KEEP_SETTINGS               13032
#define IDD_INITIAL_SETTINGS            13033
#define IDD_INITIAL_SETTINGS95          13034
#define IDD_SCAN_FOR_VIRUSES95          13035
#define IDD_ADVOTEXT_2                  13036
#define IDD_ADVOTEXT_1                  13037
#define IDD_ADVOTEXT_1_NEC              13038
#define IDD_SCAN_FOR_VIRUSES95_NEC      13039
#define IDC_STATIC_NETSCAPE2            -1
#define IDC_SCAN_FOR_VIRUSES1           -1
#define IDC_SCHEDULE_SCAN_TEXT          -1
#define IDD_RADIO_SCAN_YES                  1000
#define IDD_RADIO_SCAN_NO                   1001
#define IDS_EXIT_DIALOG                     "DlgExitHandler"
#define IDS_INITIAL_SETTINGS                "DlgInitialSettings95"
#define IDS_KEEP_SETTINGS                   "DlgKeepSettings"
#define IDS_SCAN_FOR_VIRUSES                "DlgScanForViruses95"
#define IDS_ADVO1                           "DlgAdvo1"
#define IDS_ADVO2                           "DlgAdvo2"

// define values for custom dialogs during silent installs
#define SILENT_SECTION_INITIAL_SETTINGS         "DlgInitialSettings-0"
#define SILENT_SECTION_NETSCAPE_PLUGINS         "DlgNetscapePlugins-0"
#define SILENT_SECTION_SCAN_FOR_VIRUSES         "DlgScanForViruses-0"
#define SILENT_SECTION_ADVO1                    "DlgAdvo1-0"
#define SILENT_SECTION_ADVO2                    "DlgAdvo2-0"
#define SILENT_VALUE_NAME_NETSCAPE_PLUGINS      "NetscapePlugins"
#define SILENT_VALUE_NAME_RESULT                "Result"
#define SILENT_VALUE_NAME_RUN_RESCUE            "Rescue"
#define SILENT_VALUE_NAME_RUN_LIVEUPDATE        "RunLiveUpdateAfterInstall"
#define SILENT_VALUE_NAME_RUN_NAVSCAN           "RunScanAfterInstall"
#define SILENT_VALUE_NAME_SCAN_AT_STARTUP       "ScanAtStartup"
#define SILENT_VALUE_NAME_SCAN_FOR_VIRUSES      "ScanForViruses"
#define SILENT_VALUE_NAME_SCHEDULE_SCAN         "ScheduleScan"
#define SILENT_VALUE_NAME_START_AUTO_PROTECT    "StartAutoProtect"


// registry keys (do not translate)
#define BORG_VERSION_KEY                "\\Software\\Symantec\\Norton Utilities\\Onramp"
#define BORG_VERSION_VALUE              "version"
#define CORP_FEATURES_VALUENAME         "CorporateFeatures"
#define DISABLE_SPLASH_KEY              "Software\\Symantec\\Norton AntiVirus\\Clinic"
#define DISABLE_SPLASH_VALUE            "DisableSplashScreen"
#define MIGRATION_DLL_KEY               "Software\\Microsoft\\Windows\\CurrentVersion\\Setup\\Migration DLLs"
#define MIGRATION_DLL_VALUE             "Norton AntiVirus 5.0"
#define MIGRATION_DLL_FILENAME          "migrate.dll"
#define MSAPP_PATH_KEY                  "\\Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\"
#define MSSHARED_DLL_KEY                "\\Software\\Microsoft\\Windows\\CurrentVersion\\SharedDlls"
#define MSUNINSTALL_KEY                 "\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall"
#define MSWINDOWS_KEY                   "\\Software\\Microsoft\\Windows"
#define MS_WINDOWS_CURRENT_VERSION_KEY  "\\Software\\Microsoft\\Windows\\CurrentVersion"
#define NAV_INSTALLDIR_VALUENAME        "InstallDir"
#define NAV_INSTALL_KEY                 "\\Software\\Symantec\\Norton AntiVirus\\Install\\5.31"
#define NAV_KEY                         "\\Software\\Symantec\\Norton AntiVirus"
#define NAV_LIVE_UPDATE_FILE            "\\Software\\Classes\\NavLuFile"
#define NAV_LIVE_UPDATE_FILE_SHELL      "\\Software\\Classes\\NavLuFile\\shell"
#define NAV_LIVE_UPDATE_REGKEY_PATH     "\\Software\\Symantec\\Norton AntiVirus\\LiveUpdate"
#define NAV_VIRUS_DEFS_LOCATION_KEY     "\\Software\\Symantec\\Norton AntiVirus\\Virus Defs\\Location"
#define NETSCAPE_KEY                    "\\Software\\Netscape\\Netscape Navigator"
#define NETSCAPE_MAIN_REGKEY            "\\Software\\Netscape\\Netscape Navigator\\Main"
#define SAGE_KEY                        "\\SOFTWARE\\Microsoft\\Plus!\\System Agent\\SAGE"
#define SYMANTEC_SHARED_VALUENAME       "SymantecShared"
#define SOFTWARE_CLASSES_CLASSID_KEY    "\\Software\\Classes\\CLSID"
#define SOFTWARE_CLASSES_KEY            "\\Software\\Classes"
#define SYMANTEC_INSTALLED_APPS_KEY     "\\Software\\Symantec\\InstalledApps"
#define SYMANTEC_KEY                    "\\Software\\Symantec"
#define SYMANTEC_SHAREDUSAGE_KEY        "\\Software\\Symantec\\SharedUsage"
#define SYMANTEC_SHARED_NAV_VALUENAME   "NAV95"
#define SYSTEM_SERVICES_KEY             "\\System\\CurrentControlSet\\Services\\VxD"
#define WINDOWS_HELP_KEY                "\\Software\\Microsoft\\Windows\\Help"
#define NAV_OEM_SYSWORKS_KEY            "\\SOFTWARE\\Symantec\\Norton AntiVirus (OEM)\\5.01\\English\\Installed About Extensions\\SystemWorks"
#define NAV_SYSWORKS_KEY                "\\SOFTWARE\\Symantec\\Norton AntiVirus\\5.01\\English\\Installed About Extensions\\SystemWorks"
#define NAV_SYSWORKS_VALUENAME          "Enable"

// disk directory paths and filenames
#define VIRSCAN_DAT_FILENAME            "virscan.dat"

// previous version logic
#define CLEAN_INSTALL                   0
#define INSTALLING_OVER_NEWER_VERSION   1
#define INSTALLING_OVER_OLDER_VERSION   2
#define INSTALLING_OVER_SAME_VERSION    3

// Hawking tracking for aborting installs
#define HAWKING_NO_ACTIVITY             0
#define HAWKING_TEMP_DIR_ONLY           1
#define HAWKING_USING_DEFS              2

// first version of NAV that shipped LiveUpdate (w95 is "4.5", NT is "2.01")
#define NAV_1ST_LU_MAJOR                4
#define NAV_1ST_LU_MINOR1               5
#define NAV_1ST_LU_MINOR2               0
#define NAV_1ST_LU_MINOR3               0


// media component names (used to access components within the InstallShield
// compressed library)
#define LIVEUPDATE_COMPONENT                "LiveUpdate"
#define LIVEUPDATE_NS_COMPONENT             "LiveUpdateNS"
#define NAV_WINSYS_NS_COMPONENT             "NAV WinsysNS"
#define NEW_SETTINGS_COMPONENT              "New Settings"
#define NEW_SETTINGS_COMPONENT_AT           "New Settings-AT"
#define NEW_SETTINGS_COMPONENT_NEC          "New Settings-NEC"
#define PROGRAM_COMPONENT                   "Program"
#define PROGRAM_COMPONENT_AT                "Program-AT"
#define PROGRAM_COMPONENT_NEC               "Program-NEC"
#define QCONSOLE_COMPONENT                  "Quarantine Console"
#define ROOT_COMPONENT                      "Norton AntiVirus for Windows 95"
#define SCHEDULED_SCAN_COMPONENT            "Default Scheduled Scan"
#define SCHEDULED_SCAN_COMPONENT_AT         "Default Scheduled Scan-AT"
#define SCHEDULED_SCAN_COMPONENT_NEC        "Default Scheduled Scan-NEC"
#define SYMREG_COMPONENT                    "Symreg"
#define VIRUS_DEFS_COMPONENT                "VirusDefinitions"
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
#define ERR_INPUT_CONFIG_FILE           200


// function prototypes from "setup.rul"
prototype   AddVirusDefsLocationKey();
prototype   BackupSystemFiles();
prototype   BorgUsingOldVirusDefs();
prototype   CheckAVFileList(STRING);
prototype   CheckDiskSpace(STRING);
prototype   CheckPreviousVersion(BYREF STRING);
prototype   CheckRequirements();
prototype   CleanUpInstall();
prototype   CopyUncompressedFiles();
prototype   CreateLiveUpdateKeys();
prototype   CreateNortonRescueKeys();
prototype   CreateRegDBEntries();
prototype   CreateRunOnceBatchFile();
prototype   DecrementPrevVerSharedUsageCounts();
prototype   DialogShowInitialSettings();
prototype   DialogShowNetscapeDetected();
prototype   DialogShowScanForViruses();
prototype   DialogShowSdAskDestPath();
prototype   DialogShowSdFinishReboot();
prototype   DoCopyMFC42DLLs();
prototype   HandleError(LONG);
prototype   HandlePostInstallationTasks();
prototype   HandleMSSelfExtractors();
prototype   IncrementSymantecSharedKey();
prototype   InitCustomDialogs();
prototype   InitGlobalVars();
prototype   IsNavRunning();
prototype   MoveFileData();
prototype   MoveNonSharedFiles();
prototype   MySdAskDestPath(STRING, STRING, BYREF STRING, NUMBER);
prototype   PreDataMoveHook();
prototype   PreventServerInstall();
prototype   ProcessAfterDataMove();
prototype   ProcessBeforeDataMove();
prototype   ProcessCommandLine();
prototype   ReadConfigInfo(STRING);
prototype   RemovePrevVerFiles();
prototype   RemovePrevVerRegEntries();
prototype   RemovePreviousVersion();
prototype   SetInstalledRegEntry();
prototype   SetupFolders();
prototype   SetupInstall();
prototype   SetupRestart();
prototype   SetupTrialVersion();
prototype   ShowPreCopyDialogs();
prototype   UpdateScanAtStartup();
prototype   UpdateSetupReviewList();


// ****************************************************************************
// ****************************************************************************




// functions exported from "NAVInst" custom DLL
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.BackupRegistry();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.CopyVirusDefinitionFiles(STRING);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.GetBootDir(STRING, NUMBER);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.GetDefsDirectory(STRING, STRING);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.GetFileVersionString(STRING, STRING);
prototype   NUMBER  SCRIPT_CUSTOM_DLLNAME.GetNavStartInfo(STRING, NUMBER);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.GetUsageCount(STRING,NUMBER);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.IsOEMPersonalityDLL(STRING);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.MyCharToOem(STRING, STRING );
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.RemoveAPUI();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.RemoveSched();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.RemoveQConsole();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.RemoveScheduledScans();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.RemoveLine(STRING,STRING);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.SetInocDirRegEntry(STRING);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.SetNavStartInfo(STRING,NUMBER,BOOL);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.StringContainsDBCSChars(STRING);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.UninstallVirusDefinitions();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.MachineIsNEC();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.FixUpNECConfigSys ( STRING );
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.UpdateEastEuropeanAutoexec ( STRING, STRING );
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.CreateProcessAndWait ( STRING, BYREF NUMBER );
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.CreateNAVMutex ( STRING );
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.ReleaseNAVMutex();
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.CreateInternetShortcut(STRING, STRING);
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.ShellBrowseFolder (NUMBER, STRING, STRING) ;
prototype   BOOL    SCRIPT_CUSTOM_DLLNAME.SetUpNAVDXTrialInfo ( STRING, NUMBER );

// Functions exported from "InstScan.dll"
prototype   NUMBER  InstScan.ScanMemory(STRING, STRING);

// functions exported from "IRAReg" DLL (online registration COM object)
prototype   BOOL    IRAReg.RegisterUser(HWND, LONG, STRING, INT);

// functions exported from "IRATrial" DLL (online registration COM object)
prototype   BOOL    IRATrial.SetupTrialWare(STRING,STRING,STRING,NUMBER);

// functions exported from "Kernel32" DLL (Windows NT/95 system DLL)
prototype   NUMBER  Kernel32.GetCurrentDirectory(NUMBER,POINTER);
prototype   BOOL    Kernel32.SetCurrentDirectory(STRING);

// functions exported from DEFANNUITY_SCRIPT_DLL

prototype   NUMBER  DEFANNUITY_SCRIPT_DLL.DefAnnuityInitialize(NUMBER, NUMBER, NUMBER, NUMBER, NUMBER, NUMBER, NUMBER, NUMBER);

//

prototype   NUMBER  NAVLUCBK.DllRegisterServer();








